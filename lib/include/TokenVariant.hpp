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

#include <string_view>
#include <type_traits>

#include <fmt/core.h>
#include <fmt/format.h>

namespace LuxLibrary {
    namespace detail {

        template < typename T >
        concept IsTokenType = std::is_scoped_enum_v< T >;

    } // namespace detail

    enum class TokenType : std::uint8_t {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        INTEGER,
        DOUBLE,
        CHAR,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FOR,
        FUN,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        ERROR,
        END_OF_FILE,
        COUNT
    };

}; // namespace LuxLibrary

auto format_as( LuxLibrary::TokenType type ) -> std::string_view;

template <> struct fmt::formatter< LuxLibrary::TokenType > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    constexpr auto format( LuxLibrary::TokenType opcode, FormatContext& ctx ) const {
        return fmt::formatter< std::string_view >::format( format_as( opcode ), ctx );
    }
};
