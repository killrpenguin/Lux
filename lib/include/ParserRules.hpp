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

#include "Compiler.hpp"
#include "Parser.hpp"
#include "TokenVariant.hpp"

#include <array>
#include <functional>
#include <utility>

namespace LuxLibrary {

    class ParserRules {
      public:
        using ParseFn = void ( Compiler::* )();

        struct Rule {
            ParseFn prefix{};
            ParseFn infix{};
            Precedence precedence{ Precedence::NONE };
        };

      private:
        static constexpr std::size_t RulesMax{ static_cast< std::size_t >( TokenType::COUNT ) };

        static constexpr std::array< Rule, RulesMax > rules{
            // clang-format off
            Rule{ .prefix = &Compiler::Grouping, .infix = nullptr, .precedence = Precedence::NONE },     // LEFT_PAREN
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // RIGHT_PAREN
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // LEFT_BRACE
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // RIGHT_BRACE
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // LEFT_BRACKET
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // RIGHT_BRACKET
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // COMMA
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // DOT
                Rule{ .prefix = &Compiler::Unary, .infix = &Compiler::Binary, .precedence = Precedence::TERM },   // MINUS
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::TERM },   // PLUS
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE },   // SEMICOLON
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::FACTOR }, // SLASH
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::FACTOR }, // STAR
                Rule{ .prefix = &Compiler::Unary, .infix = nullptr, .precedence = Precedence::NONE }, // BANG
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // BANG_EQUAL
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // EQUAL

                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::EQUALITY }, // EQUAL_EQUAL
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::COMPARISON }, // GREATER
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::COMPARISON }, // GREATER_EQUAL
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::COMPARISON }, // LESS
                Rule{ .prefix = nullptr, .infix = &Compiler::Binary, .precedence = Precedence::COMPARISON }, // LESS_EQUAL
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // IDENTIFIER

                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // TOKEN_INTERPOLATION_START
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // TOKEN_INTERPOLATION_MIDDLE
                                                                                             // 
                Rule{ .prefix = &Compiler::String, .infix = nullptr, .precedence = Precedence::NONE }, // STRING
                Rule{ .prefix = &Compiler::Number, .infix = nullptr, .precedence = Precedence::NONE }, // INTEGER
                Rule{ .prefix = &Compiler::Number, .infix = nullptr, .precedence = Precedence::NONE }, // DOUBLE
                Rule{ .prefix = &Compiler::Char, .infix = nullptr, .precedence = Precedence::NONE }, // CHAR
                Rule{ .prefix = &Compiler::LuxVector, .infix = nullptr, .precedence = Precedence::NONE }, // LUX_VECTOR				
                Rule{ .prefix = &Compiler::LuxMap, .infix = nullptr, .precedence = Precedence::NONE }, // LUX_MAP_OPEN				
                Rule{ .prefix = &Compiler::LuxMap, .infix = nullptr, .precedence = Precedence::NONE }, // LUX_MAP_CLOSE				
                                                                                             // 
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // AND
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // CLASS
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // ELSE
                Rule{ .prefix = &Compiler::Literal, .infix = nullptr, .precedence = Precedence::NONE }, // FALSE
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // FOR
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // FUN
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // IF
                Rule{ .prefix = &Compiler::Literal, .infix = nullptr, .precedence = Precedence::NONE }, // NIL
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // OR
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // PRINT
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // RETURN
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // SUPER
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // THIS
                Rule{ .prefix = &Compiler::Literal, .infix = nullptr, .precedence = Precedence::NONE }, // TRUE
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // VAR
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // WHILE
                Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // ERROR
				Rule{ .prefix = nullptr, .infix = nullptr, .precedence = Precedence::NONE }, // EOF
            // clang-format on
        };

      public:
        ParserRules()           = default;
        ~ParserRules() noexcept = default;

        ParserRules( const ParserRules& other )                = default;
        ParserRules( ParserRules&& other ) noexcept            = default;
        ParserRules& operator=( const ParserRules& other )     = default;
        ParserRules& operator=( ParserRules&& other ) noexcept = default;

        template < typename T >
            requires detail::IsTokenType< T >
        static constexpr auto Get( T type ) -> const Rule& {
            return rules[std::to_underlying( type )];
        }

        template < typename T >
            requires detail::IsTokenType< T >
        static constexpr auto Precedence( T type ) -> const Precedence& {
            const Rule& rule{ rules[std::to_underlying( type )] };
            return rule.precedence;
        }

        template < typename T >
            requires detail::IsTokenType< T >
        static constexpr auto CallPrefix(
            Compiler* const instance, T type, std::ostream& cerr = std::cerr ) noexcept -> void {
            const Rule& rule{ ParserRules::Get( type ) };

            if ( rule.prefix != nullptr ) {
                std::invoke( rule.prefix, instance );
                return;
            }
            fmt::println( cerr, "No prefix rule defined for {}", type );
        }

        template < typename T >
            requires detail::IsTokenType< T >
        static constexpr auto CallInfix(
            Compiler* const instance, T type, std::ostream& cerr = std::cerr ) noexcept -> void {
            const Rule& rule{ ParserRules::Get( type ) };

            if ( rule.infix != nullptr ) {
                std::invoke( rule.infix, instance );
                return;
            }
            fmt::println( cerr, "No infix rule defined for {}", type );
        }
    };

}; // namespace LuxLibrary
