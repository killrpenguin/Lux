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

#include "TokenVariant.hpp"

namespace LuxLibrary {

    using namespace LuxLibrary::detail;

    struct Token {
        using line_type = std::uint32_t;
        using size_type = std::size_t;

        TokenType type{};
        std::string::const_iterator start{};
        size_type length{};
        line_type line{};

        constexpr friend bool operator==( const Token& lhs, const Token& rhs ) noexcept {
            return lhs.type == rhs.type;
        }

        template < typename T >
            requires IsTokenType< T >
        auto Is( const T& token_variant ) const noexcept -> bool {
            return this->type == token_variant;
        }
    };

}; // namespace LuxLibrary
