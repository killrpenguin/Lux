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
#include "Object.hpp"

#include <optional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/ranges.h> // Required if your recursive variant contains vectors/containers.
#include <fmt/std.h>    // Required for std::variant support.

namespace LuxLibrary {
    namespace detail {

        struct Node;

        using Nil       = std::monostate;
        using character = char;
        using integer   = std::int64_t;
        using LuxArray  = std::vector< Node >;
        using LuxMap    = std::unordered_map< std::string, Node >;
        using LuxString = std::string;

        using Tag = std::variant<
            Nil, bool, character, double, integer, LuxString, Object, LuxMap, LuxArray >;

        struct Node : Tag {
            using Tag::variant;
        };

    } // namespace detail

    using namespace detail;

    template < typename T > using Result = std::expected< T, Error >;

    class Value {
      private:
        Tag tag{};

        template < typename T >
            requires IsValueVariant< T, Tag >
        constexpr auto Is() const noexcept -> bool {
            return std::holds_alternative< T >( tag );
        }

        template < typename T >
            requires IsValueVariant< T, Tag >
        constexpr auto As() const noexcept -> std::optional< T > {
            if ( auto pval = std::get_if< T >( &tag ) ) { return *pval; }
            return std::nullopt;
        }

        friend struct fmt::formatter< Value >;

      public:
        Value() = default;

        explicit Value( Tag val )
            : tag{ std::move( val ) } {};

        explicit Value( const std::string_view text )
            : tag{ LuxString( text ) } {};

        template < typename LuxTypes >
            requires IsValueVariant< LuxTypes, Tag >
        explicit Value( const LuxTypes type )
            : tag{ type } {};

        ~Value() noexcept = default;

        Value( const Value& other )                = default;
        Value( Value&& other ) noexcept            = default;
        Value& operator=( const Value& other )     = default;
        Value& operator=( Value&& other ) noexcept = default;

        auto IsNil() const noexcept -> bool;

        auto IsBool() const noexcept -> bool;

        auto IsObject() const noexcept -> bool;

        auto IsDouble() const noexcept -> bool;

        auto IsString() const noexcept -> bool;

        auto IsInteger() const noexcept -> bool;

        auto IsChar() const noexcept -> bool;

        auto IsLuxArray() const noexcept -> bool;

        auto IsLuxMap() const noexcept -> bool;

        auto IsFalsey() const noexcept -> bool;

        auto AsObject() const noexcept -> Object;

        auto AsString() const noexcept -> LuxString;

        auto AsLuxArray() const noexcept -> LuxArray;

        auto AsLuxMap() const noexcept -> LuxMap;

        auto AsStringView() const noexcept -> std::string_view;

        auto AsDouble() const noexcept -> double;

        auto AsInteger() const noexcept -> integer;

        auto AsChar() const noexcept -> character;

        auto AsBool() const noexcept -> bool;

        auto static ValuesEqual(
            const std::optional< Value >& lval, const std::optional< Value >& rval ) noexcept
            -> Result< bool >;
    };

} // namespace LuxLibrary

template <> struct fmt::formatter< LuxLibrary::detail::Nil > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    constexpr auto format( LuxLibrary::detail::Nil /*inner*/, FormatContext& ctx ) const {
        return fmt::format_to( ctx.out(), "Nil" );
    }
};

template <> struct fmt::formatter< LuxLibrary::Value > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    constexpr auto format( const LuxLibrary::Value& inner, FormatContext& ctx ) const {
        return std::visit(
            LuxLibrary::overloaded{
                [&]( auto val ) { return fmt::format_to( ctx.out(), "{}", val ); },
            },
            static_cast< const LuxLibrary::Node::variant& >( inner.tag ) );
    }
};

// I don't think it's possible to get RVO to work from std::visit() in this case.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnrvo"
template <> struct fmt::formatter< LuxLibrary::detail::Node > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    auto format( const LuxLibrary::detail::Node& node_val, FormatContext& ctx ) const
        -> decltype( ctx.out() ) {
        return std::visit(
            [&ctx]( const auto& arg ) {
                using T = std::decay_t< decltype( arg ) >;
                if constexpr ( std::is_same_v< T, LuxLibrary::detail::LuxArray > ) {
                    auto out   = ctx.out();
                    *out++     = '[';
                    bool first = true;
                    for ( const auto& item : arg ) {
                        if ( !first ) {
                            *out++ = ',';
                            *out++ = ' ';
                        }
                        first = false;
                        ctx.advance_to( fmt::format_to( ctx.out(), "{}", item ) );
                    }
                    *out++ = ']';
                    return out;
                } else if constexpr ( std::is_same_v< T, LuxLibrary::detail::LuxMap > ) {
                    auto out = ctx.out();
                    *out++   = '{';
                    bool first{ true };
                    for ( const auto& [key, val] : arg ) {
                        if ( !first ) {
                            *out++ = ':';
                            *out++ = ' ';
                        }
                        first = false;
                        ctx.advance_to( fmt::format_to( ctx.out(), "{}: {}", key, val ) );
                    }
                    *out++ = '}';
                    return out;
                } else {
                    return fmt::format_to( ctx.out(), "{}", arg );
                }
            },
            static_cast< const LuxLibrary::detail::Tag& >(
                node_val ) ); // Cast to base variant for std::visit
    }
};
#pragma clang diagnostic pop

static_assert( fmt::formattable< LuxLibrary::Value >, "Value is not formattable." );
static_assert( fmt::formattable< LuxLibrary::detail::Node >, "ValueNode is not formattable." );
static_assert( fmt::formattable< LuxLibrary::detail::LuxArray >, "LuxArray is not formattable." );
static_assert( fmt::formattable< LuxLibrary::detail::LuxMap >, "LuxMap is not formattable." );
