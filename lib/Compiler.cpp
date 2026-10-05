
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
#include "Common.hpp"
#include "Debug.hpp"
#include "OpCode.hpp"
#include "Parser.hpp"
#include "ParserRules.hpp"
#include "Token.hpp"
#include "TokenVariant.hpp"
#include "Value.hpp"

#include <cstdint>
#include <expected>
#include <iterator>
#include <string_view>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

namespace LuxLibrary {

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

        Expression();

        Consume( TokenType::END_OF_FILE, "Expect end of expression." );

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

        ParserRules::CallPrefix( this, prefix_token.type );

        const Token& current_token{ parser.Current() };

        while ( precedence <= ParserRules::Precedence( current_token.type ) ) {
            Advance();

            const Token& infix_token{ parser.Previous() };

            ParserRules::CallInfix( this, infix_token.type );
        }
    }

    auto Compiler::Binary() noexcept -> void {
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

    auto Compiler::Grouping() noexcept -> void {
        Expression();
        Consume( TokenType::RIGHT_PAREN, "Expect ')' after expression." );
    }

    auto Compiler::Number() noexcept -> void {
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

    auto Compiler::Literal() noexcept -> void {
        const auto last_token{ parser.Previous() };
        switch ( last_token.type ) {
            case LuxLibrary::TokenType::FALSE: EmitByte( OpCode::OP_FALSE ); break;
            case LuxLibrary::TokenType::NIL  : EmitByte( OpCode::OP_NIL ); break;
            case LuxLibrary::TokenType::TRUE : EmitByte( OpCode::OP_TRUE ); break;
            default: assert( false && "Literal fn parsing unsupported token type." );
        }
    }

    auto Compiler::Unary() noexcept -> void {
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

    auto Compiler::Char() noexcept -> void {
        const Token token{ parser.Previous() };

        const auto character{ std::ranges::next( token.start, 1 ) };

        EmitConstant( Value( static_cast< detail::character >( *character ) ) );
    }

    auto Compiler::String() noexcept -> void {
        const Token token{ parser.Previous() };

        const auto text_start{ std::ranges::next( token.start, 1 ) };

        const std::string_view text{ &*text_start, token.length - 2 };

        EmitConstant( Value( text ) );
    }

    auto Compiler::Interpolation() noexcept -> void {
        String();
        while ( parser.Previous().type != TokenType::INTERPOLATION_END ) {
            Expression();

            EmitByte( OpCode::OP_ADD );
        }
    }

    auto Compiler::LuxVector() noexcept -> void {
        Expression();
        Consume( TokenType::LUX_VECTOR, "Expected ] at end of array expression." );
    }

    auto Compiler::LuxMap() noexcept -> void {
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

}; // namespace LuxLibrary
