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

#include <fmt/core.h>
#include <fmt/format.h>

#include <string_view>

namespace LuxLibrary {
    enum class OpCode : std::uint8_t {
        OP_CONSTANT,
        OP_NOT,
        OP_NIL,
        OP_TRUE,
        OP_FALSE,
        OP_EQUAL,
        OP_GREATER,
        OP_LESS,
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_NEGATE,
        OP_RETURN,
    };

}; // namespace LuxLibrary

auto format_as( LuxLibrary::OpCode code ) -> std::string_view;

template <> struct fmt::formatter< LuxLibrary::OpCode > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    constexpr auto format( LuxLibrary::OpCode opcode, FormatContext& ctx ) const {
        return fmt::formatter< std::string_view >::format( format_as( opcode ), ctx );
    }
};
