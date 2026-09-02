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
#include <type_traits>
#include <utility>
#include <variant>

namespace LuxLibrary {
    namespace detail {

        using Nil       = std::monostate;
        using character = char32_t;
        using integer   = std::int64_t;

        using Tag = std::variant< Nil, bool, character, double, integer, std::string, Object >;

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
            : tag{ std::string( text ) } {};

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

        auto IsFalsey() const noexcept -> bool;

        auto AsObject() const noexcept -> Object;

        auto AsString() const noexcept -> std::string;

        auto AsStringView() const noexcept -> std::string_view;

        auto AsDouble() const noexcept -> double;

        auto AsInteger() const noexcept -> integer;

        auto AsChar() const noexcept -> character;

        auto AsBool() const noexcept -> bool;

        auto static ValuesEqual(
            const std::optional< Value >& l_val, const std::optional< Value >& r_val ) noexcept
            -> Result< bool >;

        auto static AsUTFChar( const character utf8_char ) noexcept -> std::string;
    };

    constexpr std::size_t ExpectedByteSize{ 16 };

} // namespace LuxLibrary

template <> struct fmt::formatter< LuxLibrary::detail::Nil > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    constexpr auto format( LuxLibrary::detail::Nil /*inner*/, FormatContext& ctx ) const {
        return fmt::format_to( ctx.out(), "Nil" );
    }
};

template <> struct fmt::formatter< LuxLibrary::Value > : fmt::formatter< std::string_view > {
    template < typename FormatContext >
    auto format( const LuxLibrary::Value& value, FormatContext& ctx ) const {
        return std::visit(
            [&ctx]( const auto& arg ) {
                using T = std::decay_t< decltype( arg ) >;
                if constexpr ( std::is_same_v< T, LuxLibrary::detail::character > ) {
                    return fmt::format_to( ctx.out(), "{}", LuxLibrary::Value::AsUTFChar( arg ) );
                } else {
                    return fmt::format_to( ctx.out(), "{}", arg );
                }
            },
            value.tag );
    }
};
