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

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <expected>
#include <numeric>
#include <optional>
#include <string>
#include <type_traits>

namespace LuxLibrary {

    using Instruction   = std::uint8_t;
    using Expression    = std::string;
    using OptionalError = std::optional< std::string >;
    using Error         = std::string;
    using ResultError   = std::unexpected< Error >;

    template < typename T > using Result = std::expected< T, Error >;

    namespace detail {
        constexpr bool DEBUG_TRACE_EXECUTION{ false };
        constexpr bool DEBUG_PRINT_CODE{ true };

        template < typename T, typename Variant >
        struct is_variant_alternative : std::false_type {};

        template < typename T, typename... Args >
        struct is_variant_alternative< T, std::variant< Args... > >
            : std::bool_constant< ( std::is_same_v< T, Args > || ... ) > {};

        template < typename T, typename Variant >
        inline constexpr bool is_variant_alternative_v =
            is_variant_alternative< T, Variant >::value;

        template < typename T, typename Variant >
        concept IsValueVariant = is_variant_alternative_v< T, Variant >;

        template < class... Ts > struct overloaded : Ts... {
            using Ts::operator()...;
        };
        template < class... Ts > overloaded( Ts... ) -> overloaded< Ts... >;

        template < typename FloatingPointValue >
        concept IsFloatingPoint =
            std::is_floating_point_v< FloatingPointValue > && requires( FloatingPointValue val ) {
                { std::isnan( val ) } -> std::same_as< bool >;
            };
        constexpr double REL_Tol{ 1e-9 };
        constexpr double ABS_Tol{ 1e-9 };

        template < detail::IsFloatingPoint T >
        auto SafeFloatingPointCompare(
            T lhs, T rhs, T abs_tol = detail::ABS_Tol, T rel_tol = detail::REL_Tol ) noexcept
            -> bool {
            if ( lhs == rhs ) { return true; }

            if ( std::isnan( lhs ) or std::isnan( rhs ) ) { return false; }

            const T diff{ std::abs( lhs - rhs ) };

            return diff <= abs_tol or
                   diff <= ( rel_tol * std::ranges::max( std::abs( lhs ), std::abs( rhs ) ) );
        }
    }; // namespace detail

#if defined( __clang__ ) && __has_cpp_attribute( clang::lifetimebound )
#define CLANG_LIFETIME_BOUND [[clang::lifetimebound]]
#else
#define CLANG_LIFETIME_BOUND
#endif

} // namespace LuxLibrary
