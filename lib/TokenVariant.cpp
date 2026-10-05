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

#include "TokenVariant.hpp"

#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

auto format_as( LuxLibrary::TokenType type ) -> std::string_view {
    switch ( type ) {
        case LuxLibrary::TokenType::LEFT_PAREN         : return "LEFT_PAREN";
        case LuxLibrary::TokenType::RIGHT_PAREN        : return "RIGHT_PARE";
        case LuxLibrary::TokenType::LEFT_BRACE         : return "LEFT_BRACE";    // {
        case LuxLibrary::TokenType::RIGHT_BRACE        : return "RIGHT_BRAC";    // }
        case LuxLibrary::TokenType::LEFT_BRACKET       : return "LEFT_BRACKET";  // [
        case LuxLibrary::TokenType::RIGHT_BRACKET      : return "RIGHT_BRACKET"; // ]
        case LuxLibrary::TokenType::COMMA              : return "COMMA     ";
        case LuxLibrary::TokenType::DOT                : return "DOT       ";
        case LuxLibrary::TokenType::MINUS              : return "MINUS     ";
        case LuxLibrary::TokenType::PLUS               : return "PLUS      ";
        case LuxLibrary::TokenType::SEMICOLON          : return "SEMICOLON ";
        case LuxLibrary::TokenType::SLASH              : return "SLASH     ";
        case LuxLibrary::TokenType::STAR               : return "STAR";

        case LuxLibrary::TokenType::BANG               : return "BANG         ";
        case LuxLibrary::TokenType::BANG_EQUAL         : return "BANG_EQUAL   ";
        case LuxLibrary::TokenType::EQUAL              : return "EQUAL";
        case LuxLibrary::TokenType::EQUAL_EQUAL        : return "EQUAL_EQUAL  ";
        case LuxLibrary::TokenType::GREATER            : return "GREATER";
        case LuxLibrary::TokenType::GREATER_EQUAL      : return "GREATER_EQUAL";
        case LuxLibrary::TokenType::LESS               : return "LESS         ";
        case LuxLibrary::TokenType::LESS_EQUAL         : return "LESS_EQUAL";

        case LuxLibrary::TokenType::INTERPOLATION_START: return "INTERPOLATION START";
        case LuxLibrary::TokenType::INTERPOLATION_END  : return "INTERPOLATION END";

        case LuxLibrary::TokenType::IDENTIFIER         : return "IDENTIFIER";
        case LuxLibrary::TokenType::STRING             : return "STRING";
        case LuxLibrary::TokenType::INTEGER            : return "INTEGER";
        case LuxLibrary::TokenType::DOUBLE             : return "DOUBLE";
        case LuxLibrary::TokenType::CHAR               : return "CHAR";
        case LuxLibrary::TokenType::LUX_VECTOR         : return "LUX_VECTOR";
        case LuxLibrary::TokenType::LUX_MAP_OPEN       : return "LUX_MAP_OPEN";
        case LuxLibrary::TokenType::LUX_MAP_CLOSE      : return "LUX_MAP_CLOSE";

        case LuxLibrary::TokenType::AND                : return "AND";
        case LuxLibrary::TokenType::CLASS              : return "CLASS";
        case LuxLibrary::TokenType::ELSE               : return "ELSE";
        case LuxLibrary::TokenType::FALSE              : return "FALSE";
        case LuxLibrary::TokenType::FOR                : return "FOR";
        case LuxLibrary::TokenType::FUN                : return "FUN";
        case LuxLibrary::TokenType::IF                 : return "IF";
        case LuxLibrary::TokenType::NIL                : return "NIL";
        case LuxLibrary::TokenType::OR                 : return "OR";
        case LuxLibrary::TokenType::PRINT              : return "PRINT";
        case LuxLibrary::TokenType::RETURN             : return "RETURN";
        case LuxLibrary::TokenType::SUPER              : return "SUPER";
        case LuxLibrary::TokenType::THIS               : return "THIS";
        case LuxLibrary::TokenType::TRUE               : return "TRUE";
        case LuxLibrary::TokenType::VAR                : return "VAR";
        case LuxLibrary::TokenType::WHILE              : return "WHILE";

        case LuxLibrary::TokenType::ERROR              : return "ERROR";
        case LuxLibrary::TokenType::END_OF_FILE        : return "END_OF_FILE";
        case LuxLibrary::TokenType::COUNT              : return "COUNT";
        default                                        : return "Unknown";
    }
}
