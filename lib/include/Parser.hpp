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
#include <utility>

namespace LuxLibrary {

    enum class Precedence : std::uint8_t {
        NONE,
        ASSIGNMENT, // =
        OR,         // or
        AND,        // and
        EQUALITY,   // == !=
        COMPARISON, // < > <= >=
        TERM,       // + -
        FACTOR,     // * /
        UNARY,      // ! -
        CALL,       // . ()
        PRIMARY,
    };

    // 1. Pre-increment (++Precedence)
    auto operator++( Precedence& precedence ) -> Precedence&;

    // 2. Post-increment (Precedence++)
    auto operator++( Precedence& precedence, int ) -> Precedence;

    class Parser {
      public:
        using line_type = std::uint32_t;

      private:
        Token current{};
        Token previous{};

        bool had_error{ false };
        bool panic_mode{ false };

      public:
        Parser()           = default;
        ~Parser() noexcept = default;

        Parser( const Parser& other )                = default;
        Parser( Parser&& other ) noexcept            = default;
        Parser& operator=( const Parser& other )     = default;
        Parser& operator=( Parser&& other ) noexcept = default;

        /*
         * @brief Set the current token as the previous token.
         */
        auto Stash() noexcept -> void;

        /*
         * @brief Set the current token as the previous token.
         */
        auto CurrentIs( const TokenType type ) noexcept -> bool;

        /*
         * @brief
         * @return bool return false unless Panic() has been called.
         */
        auto PanicMode() const noexcept -> bool;

        /*
         * @brief
         * @return Set panic mode to true.
         */
      auto PanicAndError() noexcept -> void;
      
        /*
         * @brief
         * @return Set panic mode to true.
         */
        auto Panic() noexcept -> void;

        /*
         * @brief
         * @return bool
         */
        auto HadError() const noexcept -> bool;

        /*
         * @brief
         * @return bool
         */
        auto SetError() noexcept -> void;

        /*
         * @brief
         * @return bool True if the current token is an Error token.
         */
        auto ErrorToken() const noexcept -> bool;

        /*
         * @brief
         * @return bool True if the current token is an Error token.
         */
        auto ResetErrorFlags() noexcept -> void;

        /*
         * @brief
         * @return bool True if the current token is an Error token.
         */
        auto Current() const noexcept -> const Token&;

        /*
         * @brief
         * @return
         */
        auto CurrentLine() const noexcept -> line_type;

        /*
         * @brief
         * @return
         */
        auto Previous() const noexcept -> Token;

        /*
         * @brief
         * @return
         */
        auto PreviousLine() const noexcept -> line_type;

        /*
         * @brief
         * @return
         */
        auto CurrentType( const TokenType token ) const noexcept -> bool;

        /*
         * @brief
         * @return
         */
        auto CurrentIs( const Token& token ) const noexcept -> bool;

        /*
         * @brief
         * @return
         */
        template < typename T > auto ParseToken( T&& next_token ) noexcept -> void {
            current = std::forward< T >( next_token );
        }
    };
}; // namespace LuxLibrary
