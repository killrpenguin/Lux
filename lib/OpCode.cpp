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

#include "OpCode.hpp"

namespace LuxLibrary {}; // namespace LuxLibrary

auto format_as( LuxLibrary::OpCode code ) -> std::string_view {
    switch ( code ) {
        case LuxLibrary::OpCode::OP_CONSTANT: return "OP_CONSTANT";
        case LuxLibrary::OpCode::OP_NOT     : return "OP_NOT";
        case LuxLibrary::OpCode::OP_NIL     : return "OP_NIL";
        case LuxLibrary::OpCode::OP_TRUE    : return "OP_TRUE";
        case LuxLibrary::OpCode::OP_FALSE   : return "OP_FALSE";
        case LuxLibrary::OpCode::OP_EQUAL   : return "OP_EQUAL";
        case LuxLibrary::OpCode::OP_GREATER : return "OP_GREATER";
        case LuxLibrary::OpCode::OP_LESS    : return "OP_LESS";
        case LuxLibrary::OpCode::OP_ADD     : return "OP_ADD";
        case LuxLibrary::OpCode::OP_SUBTRACT: return "OP_SUBTRACT";
        case LuxLibrary::OpCode::OP_MULTIPLY: return "OP_MULTIPLY";
        case LuxLibrary::OpCode::OP_DIVIDE  : return "OP_DIVIDE";
        case LuxLibrary::OpCode::OP_NEGATE  : return "OP_NEGATE";
        case LuxLibrary::OpCode::OP_RETURN  : return "OP_RETURN";
        default                             : return "Unknown";
    }
}

template <> struct std::formatter< LuxLibrary::OpCode > : std::formatter< std::string_view > {
    auto format( LuxLibrary::OpCode opcode, std::format_context& ctx ) const {
        return std::formatter< std::string_view >::format( format_as( opcode ), ctx );
    }
};
