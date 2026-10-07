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

#include "Scanner.hpp"
#include "Token.hpp"
#include "TokenVariant.hpp"

#include <cctype>
#include <iterator>
#include <variant>

namespace LuxLibrary {
    namespace ranges = std::ranges;
    using namespace LuxLibrary::detail;

    Scanner::Scanner( const std::string& source ) noexcept
        : start{ source.cbegin() }, current{ source.cbegin() }, end{ source.cend() } {};

    auto Scanner::IsDigit( const char_type val ) noexcept -> bool {
        return std::isdigit( static_cast< unsigned char >( val ) ) != 0;
    }

    auto Scanner::IsAlpha( const char_type val ) noexcept -> bool {
        return std::isalpha( val ) != 0 or val == '_';
    }

    auto Scanner::IdentifierType() noexcept -> TokenType {
        constexpr std::string_view r_AND{ "nd" };
        constexpr std::string_view r_CLASS{ "lass" };
        constexpr std::string_view r_ELSE{ "lse" };
        constexpr std::string_view r_IF{ "f" };
        constexpr std::string_view r_NIL{ "il" };
        constexpr std::string_view r_OR{ "r" };
        constexpr std::string_view r_PRINT{ "rint" };
        constexpr std::string_view r_RETURN{ "eturn" };
        constexpr std::string_view r_SUPER{ "uper" };
        constexpr std::string_view r_VAR{ "ar" };
        constexpr std::string_view r_WHILE{ "hile" };

        constexpr std::size_t Pos{ 1 };

        switch ( *start ) {
            case 'a': return CheckKeyword( Pos, r_AND.size(), r_AND, TokenType::AND );
            case 'c': return CheckKeyword( Pos, r_CLASS.size(), r_CLASS, TokenType::CLASS );
            case 'e': return CheckKeyword( Pos, r_ELSE.size(), r_ELSE, TokenType::ELSE );
            case 'f': {
                if ( current - start > 1 ) {
                    constexpr std::string_view r_FALSE{ "lse" };
                    constexpr std::string_view r_FOR{ "r" };
                    constexpr std::string_view r_FUN{ "n" };
                    constexpr std::size_t NextPos{ Pos + 1 };

                    const auto next_char{ ranges::next( start, 1, end ) };
                    switch ( *next_char ) {
                        case 'a':
                            return CheckKeyword(
                                NextPos, r_FALSE.size(), r_FALSE, TokenType::FALSE );
                        case 'o':
                            return CheckKeyword( NextPos, r_FOR.size(), r_FOR, TokenType::FOR );
                        case 'u':
                            return CheckKeyword( NextPos, r_FUN.size(), r_FUN, TokenType::FUN );
                        default: return TokenType::IDENTIFIER;
                    }
                }
                break;
            }
            case 'i': return CheckKeyword( Pos, r_IF.size(), r_IF, TokenType::IF );
            case 'n': return CheckKeyword( Pos, r_NIL.size(), r_NIL, TokenType::NIL );
            case 'o': return CheckKeyword( Pos, r_OR.size(), r_OR, TokenType::OR );
            case 'p': return CheckKeyword( Pos, r_PRINT.size(), r_PRINT, TokenType::PRINT );
            case 'r': return CheckKeyword( Pos, r_RETURN.size(), r_RETURN, TokenType::RETURN );
            case 's': return CheckKeyword( Pos, r_SUPER.size(), r_SUPER, TokenType::SUPER );
            case 't': {
                if ( current - start > 1 ) {
                    constexpr std::string_view r_THIS{ "is" };
                    constexpr std::string_view r_TRUE{ "ue" };
                    constexpr std::size_t NextPos{ Pos + 1 };

                    const auto next_char{ ranges::next( start, 1, end ) };
                    switch ( *next_char ) {
                        case 'h':
                            return CheckKeyword( NextPos, r_THIS.size(), r_THIS, TokenType::THIS );
                        case 'r':
                            return CheckKeyword( NextPos, r_TRUE.size(), r_TRUE, TokenType::TRUE );
                        default: return TokenType::IDENTIFIER;
                    }
                }
                break;
            }
            case 'v': return CheckKeyword( Pos, r_VAR.size(), r_VAR, TokenType::VAR );
            case 'w': return CheckKeyword( Pos, r_WHILE.size(), r_WHILE, TokenType::WHILE );
            default : return TokenType::IDENTIFIER;
        }
        return TokenType::IDENTIFIER;
    }

    auto Scanner::IsAtEnd() const noexcept -> bool {
        return current == end or *current == '\0';
    }

    auto Scanner::Match( const char_type expected ) noexcept -> bool {
        if ( IsAtEnd() ) { return false; }
        if ( static_cast< char_type >( *current ) != expected ) { return false; }
        return ranges::advance( current, 1, end ) == 0;
    }

    auto Scanner::Next( const std::ptrdiff_t amount ) noexcept -> void {
        ranges::advance( current, amount, end );
    }

    auto Scanner::NextChar() noexcept -> char_type {
        Next();
        return *ranges::prev( current, 1, start );
    }

    auto Scanner::Peek( const std::size_t amount ) const noexcept -> char_type {
        if ( amount == 1 ) { return *current; }

        return *current;
    }

    auto Scanner::PeekNext() const noexcept -> char_type {
        if ( IsAtEnd() ) { return '\0'; }
        const auto tmp{ ranges::next( current, 1, end ) };
        return *tmp;
    }

    auto Scanner::SkipWhitespace() noexcept -> void {
        while ( true ) {
            const char_type val{ Peek() };
            switch ( val ) {
                case ' ' :
                case '\r':
                case '\t': Next(); break;
                case '\n': {
                    line++;
                    Next();
                    break;
                }
                case '/': {
                    if ( PeekNext() == '/' ) {
                        while ( Peek() != '\n' and !IsAtEnd() ) {
                            Next();
                        }
                    } else {
                        return;
                    }
                    break;
                }
                default: return;
            }
        }
    }

    auto Scanner::ErrorToken( const std::string& messsage ) const noexcept -> Token {
        return Token{
            .type   = TokenType::ERROR,
            .start  = start,
            .length = messsage.size(),
            .line   = line,
        };
    }

    auto Scanner::Line() const noexcept -> line_type {
        return line;
    }

    auto Scanner::NewSource( const std::string& source ) noexcept -> void {
        start   = source.cbegin();
        current = source.cbegin();
        end     = source.cend();
        line    = 1;
    }

    auto Scanner::NextToken() noexcept -> Token {
        SkipWhitespace();

        start = current;

        if ( IsAtEnd() ) { return NewToken( TokenType::END_OF_FILE ); }

        const char_type current_char{ NextChar() };

        if ( IsAlpha( current_char ) ) { return IdentifierToken(); }

        if ( IsDigit( current_char ) ) { return NumberToken(); }

        switch ( current_char ) {
            case '(': return NewToken( TokenType::LEFT_PAREN );
            case ')': return NewToken( TokenType::RIGHT_PAREN );
            case '{': return NewToken( TokenType::LEFT_BRACE );
            case ';': return NewToken( TokenType::SEMICOLON );
            case ',': return NewToken( TokenType::COMMA );
            case '.': return NewToken( TokenType::DOT );
            case '-': return NewToken( TokenType::MINUS );
            case '+': return NewToken( TokenType::PLUS );
            case '/': return NewToken( TokenType::SLASH );
            case '*': return NewToken( TokenType::STAR );
            case '}': {
                if ( InString() ) { return ContinueString(); }
                return NewToken( TokenType::RIGHT_BRACE );
            };
            case '$': {
                if ( InString() and Match( '{' ) ) { return NewToken( TokenType::RIGHT_BRACE ); }
                return ErrorToken( "Invalid token." );
            };
            case '!': {
                if ( Match( '=' ) ) { return NewToken( TokenType::BANG_EQUAL ); }
                return NewToken( TokenType::BANG );
            };
            case '=': {
                if ( Match( '=' ) ) { return NewToken( TokenType::EQUAL_EQUAL ); }
                return NewToken( TokenType::EQUAL );
            };
            case '<': {
                if ( Match( '=' ) ) { return NewToken( TokenType::LESS_EQUAL ); }
                return NewToken( TokenType::LESS );
            }
            case '>': {
                if ( Match( '=' ) ) { return NewToken( TokenType::GREATER_EQUAL ); }
                return NewToken( TokenType::GREATER );
            }
            case '"' : return StringToken();

            case ']' : return NewToken( TokenType::RIGHT_BRACKET );
            case '[' : return LuxVectorToken();

            case '\'': return CharToken();
            default  : return ErrorToken( "Invalid token." );
        }
        return ErrorToken( "Invalid token." );
    }

    auto Scanner::NewToken( const TokenType& variant ) const noexcept -> Token {
        return Token{
            .type   = variant,
            .start  = start,
            .length = static_cast< std::size_t >( current - start ),
            .line   = line,
        };
    }

    auto Scanner::NumberToken() noexcept -> Token {
        while ( IsDigit( Peek() ) ) {
            Next();
        }

        const bool is_double{ Peek() == '.' };

        if ( is_double and IsDigit( PeekNext() ) ) {
            Next();

            while ( IsDigit( Peek() ) ) {
                Next();
            }
        }
        if ( is_double ) { return NewToken( TokenType::DOUBLE ); }

        return NewToken( TokenType::INTEGER );
    }

    auto Scanner::ContinueString() noexcept -> Token {
        while ( Peek() != '"' and !IsAtEnd() ) {
            if ( Peek() == '\n' ) { line++; }

            if ( Peek() == '$' and PeekNext() == '{' ) {
                Next();
                const Token token{ NewToken( TokenType::INTERPOLATION_START ) };
                Next();
                return token;
            }
            Next();
        }

        if ( IsAtEnd() ) { return ErrorToken( "Unterminated string." ); }

        Next();
        DefaultScanning();
        return NewToken( TokenType::INTERPOLATION_END );
    }

    auto Scanner::StringToken() noexcept -> Token {
        while ( Peek() != '"' and !IsAtEnd() ) {
            if ( Peek() == '\n' ) { line++; }

            if ( Peek() == '$' and PeekNext() == '{' ) {
                Next();
                ScanningString();
                const Token token{ NewToken( TokenType::INTERPOLATION_START ) };
                Next();
                return token;
            }
            Next();
        }

        if ( IsAtEnd() ) { return ErrorToken( "Unterminated string." ); }

        Next();
        return NewToken( TokenType::STRING );
    }

    auto Scanner::LuxVectorToken() noexcept -> Token {
        while ( Peek() != ']' and !IsAtEnd() ) {
            if ( Peek() == '\n' ) { line++; }
            Next();
        }

        if ( IsAtEnd() ) { return ErrorToken( "Unterminated Vector." ); }

        Next();
        return NewToken( TokenType::LUX_VECTOR );
    }

    auto Scanner::LuxMapToken() noexcept -> Token {
        if ( *start == '{' ) { return NewToken( TokenType::LUX_MAP_OPEN ); }

        while ( Peek() != '}' and !IsAtEnd() ) {
            if ( Peek() == '\n' ) { line++; }
            Next();
        }

        if ( IsAtEnd() ) { return ErrorToken( "Unterminated Map." ); }

        return NewToken( TokenType::LUX_MAP_CLOSE );
    }

    auto Scanner::CharToken() noexcept -> Token {
        while ( Peek() != '\'' and !IsAtEnd() ) {
            Next();
        }

        if ( IsAtEnd() ) { return ErrorToken( "Unterminated char." ); }

        Next();
        return NewToken( TokenType::CHAR );
    }

    auto Scanner::IdentifierToken() noexcept -> Token {
        while ( IsAlpha( Peek() ) or IsDigit( Peek() ) ) {
            Next();
        }

        return NewToken( IdentifierType() );
    }

    auto Scanner::DefaultScanning() noexcept -> void {
        state = detail::DefaultMode{};
    }

    auto Scanner::ScanningString() noexcept -> void {
        state = detail::StringMode{};
    }

    auto Scanner::InString() const noexcept -> bool {
        return StateIs< detail::StringMode >();
    }

    auto Scanner::InArray() const noexcept -> bool {
        return StateIs< detail::ArrayMode >();
    }

    auto Scanner::InMap() const noexcept -> bool {
        return StateIs< detail::MapMode >();
    }

    auto Scanner::IsDefault() const noexcept -> bool {
        return StateIs< detail::DefaultMode >();
    }

}; // namespace LuxLibrary
