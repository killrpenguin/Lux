
/*
 * Copyright (c) 2026 David McFarland
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Compiler.hpp"
#include "Chunk.hpp"
#include "Common.hpp"
#include "Debug.hpp"
#include "OpCode.hpp"
#include "Parser.hpp"
#include "ParserRules.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "TokenVariant.hpp"
#include "Value.hpp"

#include <cstdint>
#include <expected>
#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

namespace LuxLibrary {

    using namespace detail;

    Compiler::Compiler( Scanner scnr, Parser prsr ) noexcept
        : parser{ prsr }, scanner{ scnr } {
    }

    auto Compiler::Advance() noexcept -> void {
        parser.Stash();

        while ( true ) {
            const Token token{ scanner.NextToken() };

            parser.ParseToken( token );

            if ( !parser.CurrentType( TokenType::ERROR ) ) { break; }

            const Token& current{ parser.Current() };

            ErrorAtCurrent( &*current.start );
        }
    }

    auto Compiler::Compile( const std::string& source, Chunk& chunk ) noexcept -> Result< void > {
        scanner.NewSource( source );

        current_chunk = &chunk;

        parser.ResetErrorFlags();

        Advance();

        while ( !Match( TokenType::END_OF_FILE ) ) {
            Declaration();
        }

        EndCompiler();

        if ( parser.HadError() ) { return std::unexpected( error_message ); }

        return {};
    }

    auto Compiler::Consume( const TokenType token, const std::string_view message ) noexcept
        -> void {
        if ( parser.CurrentType( token ) ) {
            Advance();
            return;
        }

        ErrorAtCurrent( message );
    }

    auto Compiler::Expression() noexcept -> void {
        ParsePrecedence( Precedence::ASSIGNMENT );
    }

    auto Compiler::EmitByte( const OpCode byte ) noexcept -> void {
        if ( current_chunk != nullptr ) {
            current_chunk->WriteChunk( byte, parser.PreviousLine() );
        }
    }

    auto Compiler::EmitByte( const std::uint8_t byte ) noexcept -> void {
        if ( current_chunk != nullptr ) {
            current_chunk->WriteChunk( byte, parser.PreviousLine() );
        }
    }

    auto Compiler::ParsePrecedence( const Precedence precedence ) noexcept -> void {
        Advance();

        const Token& prefix_token{ parser.Previous() };

        const bool can_assign{ precedence <= Precedence::ASSIGNMENT };

        ParserRules::CallPrefix( this, prefix_token.type, can_assign );

        const Token& current_token{ parser.Current() };

        while ( precedence <= ParserRules::Precedence( current_token.type ) ) {
            Advance();

            const Token& infix_token{ parser.Previous() };

            ParserRules::CallInfix( this, infix_token.type, can_assign );
        }

        if ( can_assign and Match( TokenType::EQUAL ) ) { Error( "Invalid assignment target." ); }
    }

    auto Compiler::Binary( const bool /* can_assign */ ) noexcept -> void {
        const Token& operator_token{ parser.Previous() };

        Precedence precedence{ ParserRules::Precedence( operator_token.type ) };

        ParsePrecedence( ++precedence );

        switch ( operator_token.type ) {
            case TokenType::BANG_EQUAL   : EmitBytes( OpCode::OP_EQUAL, OpCode::OP_NOT ); break;
            case TokenType::EQUAL_EQUAL  : EmitByte( OpCode::OP_EQUAL ); break;
            case TokenType::GREATER      : EmitByte( OpCode::OP_GREATER ); break;
            case TokenType::GREATER_EQUAL: EmitBytes( OpCode::OP_LESS, OpCode::OP_NOT ); break;
            case TokenType::LESS         : EmitByte( OpCode::OP_LESS ); break;
            case TokenType::LESS_EQUAL   : EmitBytes( OpCode::OP_GREATER, OpCode::OP_NOT ); break;
            case TokenType::PLUS         : EmitByte( OpCode::OP_ADD ); break;
            case TokenType::MINUS        : EmitByte( OpCode::OP_SUBTRACT ); break;
            case TokenType::STAR         : EmitByte( OpCode::OP_MULTIPLY ); break;
            case TokenType::SLASH        : EmitByte( OpCode::OP_DIVIDE ); break;
            default: assert( false && "Binary fn parsing unsupported token type." );
        }
    }

    auto Compiler::Grouping( const bool /* can_assign */ ) noexcept -> void {
        Expression();
        Consume( TokenType::RIGHT_PAREN, "Expect ')' after expression." );
    }

    auto Compiler::Number( const bool /* can_assign */ ) noexcept -> void {
        const std::optional< Value > number = [this] -> std::optional< Value > {
            const Token& num{ parser.Previous() };
            try {
                // FIX ME.
                const double value{ std::strtod( &*num.start, nullptr ) };
                return Value{ value };
            } catch ( const std::invalid_argument& err ) {
                error_message = fmt::format( "Invalid argument: {}.\n{}", *num.start, err.what() );

                parser.PanicAndError();

                return std::nullopt;
            } catch ( const std::out_of_range& err ) {
                error_message = fmt::format( "Value out of range in Number().\n{}", err.what() );

                parser.PanicAndError();

                return std::nullopt;
            }
        }();

        if ( number.has_value() ) { EmitConstant( *number ); }
    }

    auto Compiler::Literal( const bool /* can_assign */ ) noexcept -> void {
        const auto last_token{ parser.Previous() };
        switch ( last_token.type ) {
            case LuxLibrary::TokenType::FALSE: EmitByte( OpCode::OP_FALSE ); break;
            case LuxLibrary::TokenType::NIL  : EmitByte( OpCode::OP_NIL ); break;
            case LuxLibrary::TokenType::TRUE : EmitByte( OpCode::OP_TRUE ); break;
            default: assert( false && "Literal fn parsing unsupported token type." );
        }
    }

    auto Compiler::Unary( const bool /* can_assign */ ) noexcept -> void {
        const Token& token{ parser.Previous() };

        ParsePrecedence( Precedence::UNARY );

        switch ( token.type ) {
            case TokenType::MINUS: {
                EmitByte( OpCode::OP_NEGATE );
                break;
            }
            case TokenType::BANG: {
                EmitByte( OpCode::OP_NOT );
                break;
            }
            default: assert( false && "Compiler::Unary fn parsing unsupported token type." );
        }
    }

    auto Compiler::Char( const bool /* can_assign */ ) noexcept -> void {
        const Token token{ parser.Previous() };

        const auto character{ std::ranges::next( token.start, 1 ) };

        EmitConstant( Value( static_cast< detail::character >( *character ) ) );
    }

    auto Compiler::String( const bool /* can_assign */ ) noexcept -> void {
        const Token token{ parser.Previous() };

        const auto text_start{ std::ranges::next( token.start, 1 ) };

        const std::string_view text{ &*text_start, token.length - 2 };

        EmitConstant( Value( text ) );
    }

    auto Compiler::Interpolation( const bool can_assign ) noexcept -> void {
        String( can_assign );

        while ( parser.Previous().type != TokenType::INTERPOLATION_END ) {
            Expression();

            EmitByte( OpCode::OP_ADD );
        }
    }

    auto Compiler::Variable( const bool can_assign ) noexcept -> void {
        NamedVariable( parser.Previous(), can_assign );
    }

    auto Compiler::NamedVariable( const Token& name, const bool can_assign ) noexcept -> void {
        const std::uint8_t arg{ IdentifierConstant( name ) };
        if ( can_assign and Match( TokenType::EQUAL ) ) {
            Expression();
            EmitBytes( OpCode::OP_SET_GLOBAL, arg );
        } else {
            EmitBytes( OpCode::OP_GET_GLOBAL, arg );
        }
    }

    auto Compiler::MakeConstant( const Value& val ) noexcept -> std::uint8_t {
        if ( const std::uint8_t constant{ current_chunk->AddConstant( val ) };
             constant < std::numeric_limits< std::uint8_t >::max() ) {
            return constant;
        }
        Error( "To many constants in byte chunk." );
        return 0;
    }

    auto Compiler::EmitConstant( const Value& val ) noexcept -> void {
        EmitByte( OpCode::OP_CONSTANT );
        EmitByte( MakeConstant( val ) );
    }

    auto Compiler::EmitReturn() noexcept -> void {
        EmitByte( OpCode::OP_RETURN );
    }

    auto Compiler::EndCompiler() noexcept -> void {
        if constexpr ( detail::DEBUG_PRINT_CODE ) {
            if ( parser.HadError() ) { Debug::DisassembleChunk( *current_chunk ); }
        }
        EmitReturn();
    }

    auto Compiler::Error( const std::string_view message ) noexcept -> void {
        ErrorAt( parser.Previous(), message );
    }

    auto Compiler::ErrorAtCurrent( const std::string_view message ) noexcept -> void {
        ErrorAt( parser.Current(), message );
    }

    auto Compiler::ErrorAt( const Token& token, const std::string_view message ) noexcept -> void {
        if ( parser.PanicMode() ) { return; }

        parser.Panic();

        const std::string head{ fmt::format( "[Compiler Error] line: {} error ", token.line ) };

        if ( token.Is( TokenType::END_OF_FILE ) ) {
            error_message = fmt::format( "{}at end. Message: {}", head, message );
        } else {
            error_message =
                fmt::format( "{}at {} {} Message: {}", head, token.length, *token.start, message );
        }

        parser.SetError();
    }
    auto Compiler::ExpressionStatement() noexcept -> void {
        Expression();

        Consume( TokenType::SEMICOLON, "Expect ';' after expression." );

        EmitByte( OpCode::OP_POP );
    }

    auto Compiler::PrintStatement() noexcept -> void {
        Expression();

        Consume( TokenType::SEMICOLON, "Expect ';' after value." );

        EmitByte( OpCode::OP_PRINT );
    }

    auto Compiler::Match( const TokenType type ) noexcept -> bool {
        if ( !Check( type ) ) { return false; }
        Advance();
        return true;
    }

    auto Compiler::Check( const TokenType type ) const noexcept -> bool {
        return parser.CurrentType( type );
    }

    auto Compiler::Statement() noexcept -> void {
        if ( Match( TokenType::PRINT ) ) {
            PrintStatement();
        } else {
            ExpressionStatement();
        }
    }

    auto Compiler::DefineVariable( const std::uint8_t global ) noexcept -> void {
        EmitBytes( OpCode::OP_DEFINE_GLOBAL, global );
    }

    auto Compiler::ParseVariable( const std::string_view error_message ) noexcept -> std::uint8_t {
        Consume( TokenType::IDENTIFIER, error_message );

        return IdentifierConstant( parser.Previous() );
    }

    auto Compiler::IdentifierConstant( const Token& ident_token ) noexcept -> std::uint8_t {
        const std::string_view name{
            ident_token.start,
            std::ranges::next(
                ident_token.start,
                static_cast< std::iter_difference_t< decltype( ident_token.start ) > >(
                    ident_token.length ) )
        };

        return MakeConstant( Value( name ) );
    }

    auto Compiler::VariableDeclaration() noexcept -> void {
        const std::uint8_t global{ ParseVariable( "Expected variable name." ) };

        if ( Match( TokenType::EQUAL ) ) {
            Expression();
        } else {
            EmitByte( OpCode::OP_NIL );
        }

        Consume( TokenType::SEMICOLON, "Expect ';' after variable declaration." );

        DefineVariable( global );
    }

    auto Compiler::ArrayDeclaration() noexcept -> void {
        const std::uint8_t global{ ParseVariable( "Expected variable name." ) };

        if ( !Match( TokenType::EQUAL ) ) {
            Error( "Expected assignment operator in Array declaration." );
            return;
        }

        Consume( TokenType::VECTOR_START, "Missing opening '[' in array declaration." );

        Chunk* old_chunk{ current_chunk };

        Chunk vector_chunk{};

        current_chunk = &vector_chunk;

        while ( !parser.CurrentType( TokenType::VECTOR_END ) ) {
            if ( parser.CurrentType( TokenType::COMMA ) ) {
                Consume( TokenType::COMMA, "Array values should be a comma seperated list." );
            }

            Expression();
        }

        Consume( TokenType::VECTOR_END, "Missing closing ']'." );

        current_chunk = old_chunk;

        LuxArray arr{};

        const std::span vector_values{ vector_chunk.constants_begin(),
                                       vector_chunk.constants_end() };

        for ( const auto& val : vector_values ) {
            std::visit( [&arr]( auto&& arg ) { arr.emplace_back( arg ); }, val.tag );
        }

        EmitConstant( Value( arr ) );

        DefineVariable( global );

        Consume( TokenType::SEMICOLON, "Expect ';' after array declaration." );
    }

    auto Compiler::MapDeclaration() noexcept -> void {
        const std::uint8_t global{ ParseVariable( "Expected variable name." ) };

        if ( !Match( TokenType::EQUAL ) ) {
            Error( "Expected assignment operator in map declaration." );
            return;
        }

        Consume( TokenType::MAP_START, "Missing opening '{' in map declaration." );

        Chunk* old_chunk{ current_chunk };

        Chunk map_chunk{};

        current_chunk = &map_chunk;

        while ( !parser.CurrentType( TokenType::MAP_END ) ) {
            if ( parser.CurrentType( TokenType::COMMA ) ) {
                Consume( TokenType::COMMA, "Map pairs should be a comma seperated list." );
            }

            if ( parser.CurrentType( TokenType::COLON ) ) {
                Consume( TokenType::COLON, "Map keys and values should be seperated by a colon." );
            }

            Expression();
        }

        Consume( TokenType::MAP_END, "Missing closing ']'." );

        current_chunk = old_chunk;

        LuxMap map{};

        const std::span parsed_constants{ map_chunk.constants_begin(), map_chunk.constants_end() };

        for ( auto [key, val] :
              parsed_constants | std::views::pairwise | std::views::stride( 2 ) ) {
            if ( !key.IsString() ) { Error( "Maps require strings as keys." ); }

            const std::string_view key_view{ key.AsStringView() };

            if ( map.contains( key_view ) ) {
                Error( "Map key already exists." );
                return;
            }

            std::visit(
                [&map, &key_view]( auto&& val ) { map[std::string( key_view )] = val; }, val.tag );
        }

        EmitConstant( Value( map ) );

        DefineVariable( global );

        Consume( TokenType::SEMICOLON, "Expect ';' after map declaration." );
    }

    auto Compiler::Declaration() noexcept -> void {
        if ( Match( TokenType::VAR ) ) {
            VariableDeclaration();
        } else if ( Match( TokenType::VECTOR ) ) {
            ArrayDeclaration();
        } else if ( Match( TokenType::MAP ) ) {
            MapDeclaration();
        } else {
            Statement();
        }

        if ( parser.PanicMode() ) { Synchronize(); }
    }

    auto Compiler::Synchronize() noexcept -> void {
        parser.Panic( false );

        while ( !parser.CurrentType( TokenType::END_OF_FILE ) ) {
            if ( parser.Previous().type == TokenType::SEMICOLON ) { return; }

            switch ( parser.Current().type ) {
                case TokenType::CLASS :
                case TokenType::FUN   :
                case TokenType::VAR   :
                case TokenType::FOR   :
                case TokenType::IF    :
                case TokenType::WHILE :
                case TokenType::PRINT :
                case TokenType::RETURN: return;
                default               :; // Do nothing.
            }

            Advance();
        }
    }
}; // namespace LuxLibrary
