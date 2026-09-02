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

#include <fmt/base.h>
#include <fmt/compile.h>
#include <fmt/core.h>

#include <string_view>

namespace LuxLibrary {
    struct MoveCursor {
        std::size_t line{};
        std::size_t column{ 1 };
    };

    struct Clear {
        /*
          S = Clear screen.
          L = Clear full line.
          F = Clear cursor forward.
          B = Clear cursor backwards.
        */
    };

}; // namespace LuxLibrary

template <> struct fmt::formatter< LuxLibrary::Clear > {
    std::string code{};

    constexpr auto parse( fmt::format_parse_context& ctx ) -> const char* {
        const auto* iter      = ctx.begin();
        const auto* const end = ctx.end();

        if ( iter == end or *iter == '}' ) {
            return iter;
        }

        switch ( *iter ) {
            case 's':
            case 'S': code = "\x1b[2J"; break;

            case 'l':
            case 'L': code = "\x1b[2K"; break;

            case 'f':
            case 'F': code = "\x1b[K"; break;

            case 'b':
            case 'B': code = "\x1b[1K"; break;

            default : throw fmt::format_error( "Invalid format specifier for Clear." ); break;
        };

        static_cast< void >( ++iter );

        if ( iter != end and *iter != '}' ) {
            throw fmt::format_error( "Invalid format specifier for Clear" );
        }

        return iter;
    }

    template < typename FormatContext > auto format( const LuxLibrary::Clear& /*unused*/, FormatContext& ctx ) const {
        return fmt::format_to( ctx.out(), "{}", code );
    }
};

template <> struct fmt::formatter< LuxLibrary::MoveCursor > {
    bool home{ false };

    constexpr auto parse( fmt::format_parse_context& ctx ) {
        const auto* iter = ctx.begin();
        const auto* end  = ctx.end();

        if ( iter != end && *iter == 'h' ) {
            home = true;
            ++iter;
        }

        if ( iter != end and *iter != '}' ) {
            throw fmt::format_error( "Invalid format specifier for MoveCursor." );
        }
        return iter;
    }

    template < typename FormatContext > auto format( const LuxLibrary::MoveCursor& cursor, FormatContext& ctx ) const {
        constexpr std::string_view ShowCursor{ "\x1b[?25h" };
        constexpr std::string_view HideCursor{ "\x1b[?25l" };

        if ( home ) {
            return fmt::format_to( ctx.out(), FMT_COMPILE( "{}\x1b[H{}" ), HideCursor, ShowCursor );
        }
        return fmt::format_to(
            ctx.out(), FMT_COMPILE( "{}\x1b[{};{}H{}" ), HideCursor, cursor.line, cursor.column, ShowCursor );
    }
};
