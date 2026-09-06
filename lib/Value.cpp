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

#include "Value.hpp"
#include "Common.hpp"
#include "Object.hpp"

namespace LuxLibrary {
    auto Value::IsNil() const noexcept -> bool {
        return Is< Nil >();
    }

    auto Value::IsBool() const noexcept -> bool {
        return Is< bool >();
    }

    auto Value::IsDouble() const noexcept -> bool {
        return Is< double >();
    }

    auto Value::IsInteger() const noexcept -> bool {
        return Is< integer >();
    }

    auto Value::IsChar() const noexcept -> bool {
        return Is< character >();
    }

    auto Value::IsString() const noexcept -> bool {
        return Is< std::string >();
    }

    auto Value::IsObject() const noexcept -> bool {
        return Is< Object >();
    }

    auto Value::IsFalsey() const noexcept -> bool {
        return IsNil() or ( IsBool() and !AsBool() );
    }

    auto Value::AsString() const noexcept -> std::string {
        if ( const std::optional< std::string > val = As< std::string >(); val.has_value() ) {
            return *val;
        }
        assert( false && "Value was not a double." );
    }

    auto Value::AsStringView() const noexcept -> std::string_view {
        try {
            return std::visit(
                []( auto&& arg CATCH_ATTR_LIFETIMEBOUND ) -> std::string_view {
                    using T = std::decay_t< decltype( arg ) >;
                    if constexpr ( std::is_same_v< T, std::string > ) {
                        return arg; // Implicit conversion from const std::string& to
                                    // std::string_view
                    } else {
                        return "";  // Fallback for non-string types
                    }
                },
                tag );
        } catch ( const std::bad_variant_access& /*ex*/ ) {
            assert( false && "Bad variant error thrown in ValuesEqual function." );
        }
    }

    auto Value::AsObject() const noexcept -> Object {
        if ( const std::optional< Object > val = As< Object >(); val.has_value() ) { return *val; }
        assert( false && "Value was not an object." );
    }

    auto Value::AsChar() const noexcept -> character {
        if ( const std::optional< character > val = As< character >(); val.has_value() ) {
            return *val;
        }

        assert( false && "Value was not a character." );
    }

    auto Value::AsDouble() const noexcept -> double {
        if ( const std::optional< double > val = As< double >(); val.has_value() ) { return *val; }

        assert( false && "Value was not a double." );
    }

    auto Value::AsInteger() const noexcept -> integer {
        if ( const std::optional< integer > val = As< integer >(); val.has_value() ) {
            return *val;
        }

        assert( false && "Value was not an integer." );
    }

    auto Value::AsBool() const noexcept -> bool {
        if ( const std::optional< bool > val = As< bool >(); val.has_value() ) { return *val; }
        assert( false && "Value was not a bool." );
    }

    auto Value::ValuesEqual(
        const std::optional< Value >& l_val, const std::optional< Value >& r_val ) noexcept
        -> Result< bool > {
        if ( !l_val.has_value() or !r_val.has_value() ) {
            return ResultError( "Invalid expression." );
        }
        const auto [lhs, rhs] = [&l_val, &r_val] { return std::tuple{ *l_val, *r_val }; }();

        if ( lhs.tag.index() != rhs.tag.index() ) { return false; }
        try {
            return std::visit(
                [lhs, rhs]( auto&& arg ) {
                    using T = std::decay_t< decltype( arg ) >; // Get the clean type

                    if constexpr ( std::is_same_v< T, bool > ) {
                        return lhs.AsBool() == rhs.AsBool();
                    } else if constexpr ( std::is_same_v< T, double > ) {
                        return detail::SafeFloatingPointCompare< T >(
                            lhs.AsDouble(), rhs.AsDouble() );
                    } else if constexpr ( std::is_same_v< T, integer > ) {
                        return lhs.AsInteger() == rhs.AsInteger();
                    } else if constexpr ( std::is_same_v< T, character > ) {
                        return lhs.AsChar() == rhs.AsChar();
                    } else if constexpr ( std::is_same_v< T, Nil > ) {
                        return true;
                    } else if constexpr ( std::is_same_v< T, std::string > ) {
                        return lhs.AsString() == rhs.AsString();
                    } else if constexpr ( std::is_same_v< T, Object > ) {
                        return lhs.AsObject() == rhs.AsObject();
                    } else if constexpr ( std::is_same_v< T, LuxArray > ) {
                        assert( false && "LuxArray is Unimplemented." );
                        return false;
                    } else if constexpr ( std::is_same_v< T, LuxMap > ) {
                        assert( false && "LuxMap is Unimplemented." );
                        return true;
                    } else {
                        return ResultError( "Invalid expression." );
                    }
                },
                lhs.tag );

        } catch ( const std::bad_variant_access& /*ex*/ ) {
            assert( false && "Bad variant error thrown in ValuesEqual function." );
        }
    }

    auto Value::AsUTFChar( const character utf8_char ) noexcept -> std::string {
        std::array< char, 4 > buffer{};
        std::size_t code_point_cnt{ 0 };

        // NOLINTBEGIN
        if ( utf8_char <= 0x7F ) {
            buffer[0]      = static_cast< char >( utf8_char );
            code_point_cnt = 1;
        } else if ( utf8_char <= 0x7FF ) {
            buffer[0]      = static_cast< char >( 0xC0 | ( ( utf8_char >> 6 ) & 0x1F ) );
            buffer[1]      = static_cast< char >( 0x80 | ( utf8_char & 0x3F ) );
            code_point_cnt = 2;
        } else if ( utf8_char <= 0xFFFF ) {
            buffer[0]      = static_cast< char >( 0xE0 | ( ( utf8_char >> 12 ) & 0x0F ) );
            buffer[1]      = static_cast< char >( 0x80 | ( ( utf8_char >> 6 ) & 0x3F ) );
            buffer[2]      = static_cast< char >( 0x80 | ( utf8_char & 0x3F ) );
            code_point_cnt = 3;
        } else if ( utf8_char <= 0x10FFFF ) {
            buffer[0]      = static_cast< char >( 0xF0 | ( ( utf8_char >> 18 ) & 0x07 ) );
            buffer[1]      = static_cast< char >( 0x80 | ( ( utf8_char >> 12 ) & 0x3F ) );
            buffer[2]      = static_cast< char >( 0x80 | ( ( utf8_char >> 6 ) & 0x3F ) );
            buffer[3]      = static_cast< char >( 0x80 | ( utf8_char & 0x3F ) );
            code_point_cnt = 4;
        } else {
            // Substitution character for invalid code points
            buffer[0]      = '?';
            code_point_cnt = 1;
        }
        // NOLINTEND
        return { buffer.data(), code_point_cnt };
    }

}; // namespace LuxLibrary
