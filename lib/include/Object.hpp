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

#include <variant>

namespace LuxLibrary {
    namespace detail {
        using ObjectTag = std::variant< std::monostate, std::string >;

    } // namespace detail

    using namespace detail;

    class Object {
      private:
        ObjectTag tag{};

        template < typename T >
            requires IsValueVariant< T, ObjectTag >
        constexpr auto Is() const noexcept -> bool {
            return std::holds_alternative< T >( tag );
        }

        template < typename T >
            requires IsValueVariant< T, ObjectTag >
        constexpr auto As() const noexcept -> std::optional< T > {
            return std::nullopt;
        }

        friend struct fmt::formatter< Object >;

      public:
        Object() = default;

        template < typename ObjectType >
            requires IsValueVariant< ObjectType, ObjectTag >
        explicit Object( const ObjectType type )
            : tag{ type } {};

        ~Object() noexcept = default;

        Object( const Object& other )                = default;
        Object( Object&& other ) noexcept            = default;
        Object& operator=( const Object& other )     = default;
        Object& operator=( Object&& other ) noexcept = default;

        friend struct fmt::formatter< Object >;

        auto operator<=>( const Object& ) const = default;
    };

}; // namespace LuxLibrary

template <> struct fmt::formatter< LuxLibrary::Object > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    auto format( const LuxLibrary::Object& /*str*/, FormatContext& ctx ) const {
        return fmt::format_to( ctx.out(), "This is an object." );
    }
};
