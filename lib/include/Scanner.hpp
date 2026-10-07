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

#pragma once

#include "Token.hpp"
#include "TokenVariant.hpp"

#include <cstdint>
#include <cstring>
#include <string>
#include <variant>

namespace LuxLibrary {

    namespace detail {

        struct StringMode {};
        struct ArrayMode {};
        struct MapMode {};
        using DefaultMode = std::monostate;

        using State = std::variant< StringMode, ArrayMode, MapMode, DefaultMode >;

        template < typename T, typename ScannerState >
        struct is_possible_state : std::false_type {};

        template < typename T, typename... Args >
        struct is_possible_state< T, std::variant< Args... > >
            : std::bool_constant< ( std::is_same_v< T, Args > || ... ) > {};

        template < typename T, typename ScannerState >
        inline constexpr bool is_possible_state_v = is_possible_state< T, ScannerState >::value;

        template < typename T, typename ScannerState >
        concept IsStateVariant = is_possible_state_v< T, ScannerState >;

    }; // namespace detail

    class Scanner {
      public:
        using char_type = unsigned char;
        using line_type = std::uint32_t;

      private:
        std::string::const_iterator start{};
        std::string::const_iterator current{};
        std::string::const_iterator end{};

        line_type line{ 1 };

        State state{};

        /*
         * @brief Checks if the given character is one of the 10 decimal digits.
         * @param const char_type val The character to check.
         * @return bool True if val is a numeric character.
         */
        auto static IsDigit( const char_type val ) noexcept -> bool;

        /*
         * @brief Checks if the given character is an uppercase, lowercase alphabetic character or
         * underscore.
         * @param const char_type val The character to check.
         * @return bool True if val is an alphabetic character or underscore.
         */
        auto static IsAlpha( const char_type val ) noexcept -> bool;

        /*
         * @brief Identify Lux keywords and identifiers.
         * @return TokenType A std::variant holding the specific token type..
         */
        auto IdentifierType() noexcept -> TokenType;

        /*
         * @brief Checks if the scanner is at the end of the source code string.
         * @return bool True if the scanner has reached 1 passed the end of the source string.
         */
        auto IsAtEnd() const noexcept -> bool;

        /*
         * @brief .
         * @param const char_type expected The character to compare.
         * @return bool True if expected is the same as the scanners current character.
         */
        auto Match( const char_type expected ) noexcept -> bool;

        /*
         * @brief
         */
        auto Next( const std::ptrdiff_t amount = 1 ) noexcept -> void;

        /*
         * @brief
         */
        auto NextChar() noexcept -> char_type;

        /*
         * @brief
         * @return
         */
        auto Peek( const std::size_t amount = 1 ) const noexcept -> char_type;

        /*
         * @brief
         * @return
         */

        auto PeekNext() const noexcept -> char_type;

        /*
         * @brief
         */
        auto SkipWhitespace() noexcept -> void;

        /*
         * @brief
         * @param
         * @param
         * @param
         * @param
         * @return
         */
        template < typename Variant >
            requires detail::IsTokenType< Variant >
        auto CheckKeyword(
            const int pos, const int length, const std::string_view rest, Variant type )
            -> TokenType {
            const std::string_view sub_view{ start + pos, start + length + pos };
            if ( current - start == pos + length and ( sub_view == rest ) ) { return type; }

            return TokenType::IDENTIFIER;
        }

        template < typename T >
            requires detail::IsStateVariant< T, detail::State >
        constexpr auto StateIs() const noexcept -> bool {
            return std::holds_alternative< T >( state );
        }

      public:
        Scanner() noexcept = default;
        explicit Scanner( const std::string& source ) noexcept;

        ~Scanner() noexcept = default;

        Scanner( const Scanner& other )                = default;
        Scanner( Scanner&& other ) noexcept            = default;
        Scanner& operator=( const Scanner& other )     = default;
        Scanner& operator=( Scanner&& other ) noexcept = default;

        /*
         * @brief Generate an error token at the current line.
         * @param const std::string& An error message.
         * @return An error token with the given message.
         */
        auto ErrorToken( const std::string& messsage ) const noexcept -> Token;

        /*
         * @brief Get the current line being scanned.
         * @return The current line of source code.
         */
        auto Line() const noexcept -> line_type;

        /*
         * @brief The entry point to the virtual machine..
         * @param const std::string& A string of Lux source code.
         * @return InterpretResult The possible results.
         */
        auto NewSource( const std::string& source ) noexcept -> void;

        /*
         * @brief
         * @return
         */
        auto NextToken() noexcept -> Token;

        /*
         * @brief Generate a Lux token.
         * @param TokenType& the type of token to generate.
         * @return The requested token.
         */
        auto NewToken( const TokenType& variant ) const noexcept -> Token;

        /*
         * @brief Generate a double token if the number contains a dot otherwise an integer token.
         * @return A number token.
         */
        auto NumberToken() noexcept -> Token;

        /*
         * @brief Generate a string with the text between two double quotes.
         * @return A string token.
         */
        auto ContinueString() noexcept -> Token;

        /*
         * @brief Generate a string with the text between two double quotes.
         * @return A string token.
         */
        auto StringToken() noexcept -> Token;

        /*
         * @brief Generate a Lux Vector with the values between two square brackets.
         * @return A LuxVector token.
         */
        auto LuxVectorToken() noexcept -> Token;

        /*
         * @brief Generate a Lux Map with the key and value pairs between two curly brackets.
         * @return A LuxMap token.
         */
        auto LuxMapToken() noexcept -> Token;

        /*
         * @brief Generate a char token with a value between two single quotes.
         * @return A char token.
         */
        auto CharToken() noexcept -> Token;

        /*
         * @brief
         * @return
         */
        auto IdentifierToken() noexcept -> Token;

        /*
         * @brief Checks that the scanner is in String scanning mode.
         * @return bool Returns true if in string mode.
         */
        auto InString() const noexcept -> bool;

        /*
         * @brief Checks that the scanner is in Array scanning mode.
         * @return bool Returns true if in array mode.
         */
        auto InArray() const noexcept -> bool;

        /*
         * @brief Checks that the scanner is in Map scanning mode.
         * @return bool Returns true if in map mode.
         */
        auto InMap() const noexcept -> bool;

        /*
         * @brief Checks that the scanner is in default scanning mode.
         * @return bool Returns true if scanning a string.
         */
        auto IsDefault() const noexcept -> bool;

        /*
         * @brief Set the scanning mode to default.
         */
        auto DefaultScanning() noexcept -> void;

        /*
         * @brief Set the scanning mode to string.
         */
        auto ScanningString() noexcept -> void;

        /*
         * @brief
         * @param
         * @return
         */
        template < typename Variant >
            requires detail::IsTokenType< Variant >
        auto NewToken( const Variant& type ) const noexcept -> Token {
            return Token{
                .type   = type,
                .start  = start,
                .length = static_cast< std::size_t >( current - start ),
                .line   = line,
            };
        }
    };

}; // namespace LuxLibrary
