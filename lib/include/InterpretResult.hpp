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

#include "Common.hpp"
#include "Value.hpp"

#include <optional>
#include <variant>

namespace LuxLibrary {

    struct INTERPRET_OK {
        std::optional< Value > inner{ std::nullopt };
    };
    struct INTERPRET_COMPILE_ERROR {
        Error error{};
    };
    struct INTERPRET_RUNTIME_ERROR {
        Error error{};
    };

    struct InterpretResultVisitor {
        auto operator()( const INTERPRET_RUNTIME_ERROR& val ) const -> std::string {
            return val.error;
        }
        auto operator()( const INTERPRET_COMPILE_ERROR& val ) const -> std::string {
            return val.error;
        }
        auto operator()( const INTERPRET_OK& val ) const -> std::string {
            if ( val.inner.has_value() ) { return fmt::format( "{}", *val.inner ); }
            return "";
        }
    };

    using InterpretResult =
        std::variant< INTERPRET_OK, INTERPRET_COMPILE_ERROR, INTERPRET_RUNTIME_ERROR >;

}; // namespace LuxLibrary

template <>
struct fmt::formatter< LuxLibrary::InterpretResult > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    auto format( const LuxLibrary::InterpretResult& result, FormatContext& ctx ) const {
        return fmt::format_to(
            ctx.out(), "{}", std::visit( LuxLibrary::InterpretResultVisitor{}, result ) );
    }
};
