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

#include <variant>

namespace LuxLibrary {

    struct INTERPRET_OK {};
    struct INTERPRET_COMPILE_ERROR {};
    struct INTERPRET_RUNTIME_ERROR {};

    struct InterpretResultVisitor {
        auto operator()( const INTERPRET_RUNTIME_ERROR /*error*/ ) const -> void {
        }
        auto operator()( const INTERPRET_COMPILE_ERROR /*error*/ ) const -> void {
        }
        auto operator()( const INTERPRET_OK /*val*/ ) const -> void {
        }
    };

    using InterpretResult = std::variant< INTERPRET_OK, INTERPRET_COMPILE_ERROR, INTERPRET_RUNTIME_ERROR >;

}; // namespace LuxLibrary
