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

#include "ParserRules.hpp"
#include "Parser.hpp"

namespace LuxLibrary {

    // ++Precedence
    auto operator++( Precedence& precedence ) -> Precedence& {
        using T    = std::underlying_type_t< Precedence >;
        precedence = static_cast< Precedence >( static_cast< T >( precedence ) + 1 );
        return precedence;
    }
    // auto operator++( Precedence& precedence ) -> Precedence& {
    //     if ( precedence == Precedence::PRIMARY ) {
    //         precedence = Precedence::NONE;
    //     } else {
    //         using T    = std::underlying_type_t< Precedence >;
    //         precedence = static_cast< Precedence >( static_cast< T >( precedence ) + 1 );
    //     }
    //     return precedence;
    // }

    // Precedence++
    auto operator++( Precedence& precedence, int ) -> Precedence {
        const Precedence result = precedence;
        ++precedence;
        return result;
    }

}; // namespace LuxLibrary
