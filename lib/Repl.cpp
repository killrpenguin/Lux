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

#include "Repl.hpp"
#include "Common.hpp"
#include "EscapeCodes.hpp"
#include "Value.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>

namespace LuxLibrary {
    /* This is the only way to get a value into sigwinch_handler. */
    Repl* Repl::InstancePtr{ nullptr };

    namespace ranges = std::ranges;

    namespace {
        void sigwinch_handler( int sig ) {
            if ( Repl::InstancePtr == nullptr ) {
                fmt::println(
                    std::cerr, "Repl instance pointer is nullptr. The system will now exit." );
                std::exit( EXIT_FAILURE );
            }

            if ( sig != SIGWINCH ) { return; }

            try {
                Repl::InstancePtr->GetWindowSize();
            } catch ( const std::exception& err ) {
                fmt::println(
                    std::cerr, "Unable to resize in window resize signal handler\nMessage: {}.",
                    err.what() );
                return;
            }
        };
    } // namespace

    Repl::~Repl() noexcept {
        if ( raw_mode_enabled ) { ResetTerminal(); }
    }

    auto Repl::Running() const noexcept -> bool {
        return running;
    }

    auto Repl::ResetScreen( std::ostream& output ) noexcept -> void {
        try {
            fmt::print( output, "{:S}{:h}", Clear{}, MoveCursor{} );
        } catch ( const fmt::format_error& err ) {
            std::cerr << "Format error formatting " << typeid( MoveCursor ).name() << "\n"
                      << typeid( Clear ).name() << "\n\t" << err.what();
        } catch ( const std::system_error& err ) {
            std::cerr << "Error string: " << err.what() << '\n';
            std::cerr << "Numeric code: " << err.code().value() << '\n';
        } catch ( const std::exception& err ) {
            std::cerr << "Uncaught exception.\n\t" << err.what();
        }

        output << std::flush;

        cursor_column = 2;
        cursor_line   = 1;
    }

    namespace Key {
        constexpr char Empty{ '\000' };
        constexpr char Escape{ '\x1b' };
        constexpr char BackSpace{ '\x7F' };
        constexpr char Submit{ '\x13' }; // ctrl-s

    }; // namespace Key

    auto Repl::Read( std::istream& input, std::ostream& output ) noexcept -> Expression {
        Expression expression{};

        std::streambuf* const pbuf{ input.rdbuf() };

        assert( pbuf != nullptr );

        while ( pbuf->sgetc() != EOF ) {
            const char letter{ static_cast< char >( pbuf->sbumpc() ) };

            constexpr std::size_t column_min{ 2 };

            switch ( letter ) {
                case Key::Empty : continue; // jump directly back to while loop check.
                case Key::Escape: {
                    running = false;
                    return {};
                }
                case Key::BackSpace: {
                    if ( !expression.empty() ) { expression.pop_back(); }
                    try {
                        fmt::print(
                            output, "{}{:f}",
                            MoveCursor{ .line = cursor_line, .column = cursor_column }, Clear{} );
                    } catch ( const fmt::format_error& err ) {
                        std::cerr << "Format error formatting " << typeid( MoveCursor ).name()
                                  << "\n\t" << err.what();
                    } catch ( const std::system_error& err ) {
                        std::cerr << "Error string: " << err.what() << '\n';
                        std::cerr << "Numeric code: " << err.code().value() << '\n';
                        std::cerr << "Category:     " << err.code().category().name() << '\n';
                    } catch ( const std::exception& err ) {
                        std::cerr << "Uncaught exception.\n\t" << err.what();
                    }

                    cursor_column = ranges::clamp( --cursor_column, column_min, screen_columns );

                    output << std::flush;
                    break;
                }
                case Key::Submit: { // submit expression with ctrl-S.
                    return expression;
                }
                default: {
                    if ( std::isprint( static_cast< unsigned char >( letter ) ) != 0 ) {
                        expression += letter;
                        cursor_column =
                            ranges::clamp( ++cursor_column, column_min, screen_columns - 1 );
                        try {
                            fmt::print(
                                output, "{}{}",
                                MoveCursor{ .line = cursor_line, .column = cursor_column },
                                letter );
                        } catch ( const std::system_error& err ) {
                            std::cerr << "Error string: " << err.what() << '\n';
                            std::cerr << "Numeric code: " << err.code().value() << '\n';
                            std::cerr << "Category:     " << err.code().category().name() << '\n';
                        } catch ( const std::exception& err ) {
                            std::cerr << "Uncaught exception.\n\t" << err.what();
                        }

                        output << std::flush;
                    }
                    break;
                }
            }
        }
        std::unreachable();
    }

    auto Repl::Print(
        // NOLINTNEXTLINE
        std::span< const Expression > inputs, std::span< const Result< Value > > results,
        std::ostream& output ) noexcept -> void {
        ResetScreen( output );

        for ( auto&& [expr, result] : std::views::zip( inputs, results ) ) {
            try {
                fmt::print( output, "{}> {}", MoveCursor{ .line = cursor_line }, expr );

                cursor_line = ranges::min( ++cursor_line, screen_lines );

                if ( result.has_value() ) {
                    fmt::println( output, "{} {}", MoveCursor{ .line = cursor_line }, *result );
                } else {
                    fmt::println(
                        output, "{} {}", MoveCursor{ .line = cursor_line }, result.error() );
                }

                cursor_line = ranges::min( ++cursor_line, screen_lines );

            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n'
                          << "Numeric code: " << err.code().value() << '\n';
            }
        }

        try {
            fmt::print( output, "{}> ", MoveCursor{ .line = cursor_line } );
        } catch ( const std::system_error& err ) {
            std::cerr << "Error string: " << err.what() << '\n';
            std::cerr << "Numeric code: " << err.code().value() << '\n';
        }

        output << std::flush;
    }

    auto Repl::Initialize( std::istream& input ) noexcept -> void {
        try {
            ConfigureTerminal();
        } catch ( const std::runtime_error& err ) {
            std::cerr << "Failed to Initialize the terminal.\n\t" << err.what();
        }

        raw_mode_enabled = true;

        try {
            SetResizeSigAction();
        } catch ( const std::runtime_error& err ) {
            std::cerr << "Failed to initialize screen resize signal handler.\n\t" << err.what();
        }

        try {
            GetWindowSize();
        } catch ( const std::runtime_error& err ) {
            std::cerr << "Failed to poplulate the terminal screen dimensions.\n\t" << err.what();
        }

        std::ios_base::sync_with_stdio( false );

        input.tie( nullptr );
    }

    constexpr auto Repl::ResetTerminal() noexcept -> void {
        if ( const auto val = tcsetattr( STDIN_FILENO, TCSAFLUSH, &original_termios ); val == -1 ) {
            std::cerr << "Failed to disable raw mode on exit.";
        }
    }

    constexpr auto Repl::ConfigureTerminal() -> void {
        if ( const int err{ tcgetattr( STDIN_FILENO, &original_termios ) }; err == -1 ) {
            throw std::runtime_error( "tcgetattr failed to populate oringial termios struct." );
        }

        termios raw{};
        if ( const int err{ tcgetattr( STDIN_FILENO, &raw ) }; err == -1 ) {
            throw std::runtime_error( "tcgetattr failed to populate raw struct." );
        }

        raw.c_iflag &=
            ~( static_cast< unsigned int >( BRKINT ) | static_cast< unsigned int >( ICRNL ) |
               static_cast< unsigned int >( INPCK ) | static_cast< unsigned int >( ISTRIP ) |
               static_cast< unsigned int >( IXON ) );

        raw.c_oflag &= ~( static_cast< unsigned int >( OPOST ) );

        raw.c_cflag |= ( static_cast< unsigned int >( CS8 ) );

        raw.c_lflag &=
            ~( static_cast< unsigned int >( ECHO ) | static_cast< unsigned int >( ICANON ) |
               static_cast< unsigned int >( IEXTEN ) | static_cast< unsigned int >( ISIG ) );

        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 1;

        const int err{ tcsetattr( STDIN_FILENO, TCSAFLUSH, &raw ) };

        raw_mode_enabled = err != -1;
    }

    auto Repl::GetWindowSize() -> void {
        struct winsize win_size{};

        if ( const int ioctl_ret_val{ ioctl( STDOUT_FILENO, TIOCGWINSZ, &win_size ) };
             ioctl_ret_val == -1 or win_size.ws_col == 0 ) {
            if ( write( STDOUT_FILENO, ESC::MaxScreenPos.c_str(), ESC::MaxScreenPos.size() ) !=
                 ESC::MaxScreenPos.size() ) {
                throw std::runtime_error( "Failed to write max screen position escape code." );
            }
            try {
                ManualCursorPosition();
            } catch ( const std::system_error& /*err*/ ) {
                throw;
            } catch ( const std::exception& /*err*/ ) { throw; }
        }
        screen_columns = win_size.ws_col;
        screen_lines   = win_size.ws_row;
    }

    constexpr auto Repl::SetResizeSigAction() -> void {
        Repl::InstancePtr = this;

        struct sigaction data{};

        if ( sigemptyset( &data.sa_mask ) == -1 ) {
            throw std::runtime_error( "sigemptyset() failed. The sigaction struct is not empty." );
        }

        data.sa_flags   = SA_RESTART; // fixes EIENTER
        data.sa_handler = &sigwinch_handler;
        errno           = 0;

        if ( sigaction( SIGWINCH, &data, nullptr ) == -1 ) {
            if ( errno == EINVAL ) {
                /* sigaction() might return EINVAL invalid signal. This is not an error.*/
                return;
            }
            throw std::runtime_error(
                "sigaction() argument act or oact points to memory which is not a valid part of "
                "the process address "
                "space." );
        }
    }

    constexpr auto Repl::ManualCursorPosition() -> void {
        constexpr std::size_t buff_size{ 32 };
        std::array< char, buff_size > buffer{};

        auto* begin{ buffer.begin() };
        const auto* const end{ buffer.end() };

        std::ptrdiff_t write_count{};
        if ( write_count =
                 write( STDOUT_FILENO, ESC::GetCursorPos.c_str(), ESC::GetCursorPos.size() );
             write_count != ESC::GetCursorPos.size() ) {
            throw std::runtime_error(
                "Write function failed to write cursor position escape code." );
        }

        while ( begin != end ) {
            if ( read( STDIN_FILENO, &*begin, 1 ) != 1 ) { break; }
            if ( *begin == 'R' ) { break; }
            std::advance( begin, 1 );
        }

        *begin = '\0';

        try {
            if ( buffer.at( 0 ) != ESC::EscapeChar || buffer.at( 1 ) != '[' ) {
                throw std::runtime_error( "Invalid escape code response from terminal." );
            }

        } catch ( const std::out_of_range& err ) {
            fmt::println(
                std::cerr, "Error in Manual Cursor Position function:\n\t{}", err.what() );
        }

        if ( const auto [ptr, ec]{
                 std::from_chars(
                     buffer.data(), std::ranges::next( buffer.begin(), write_count ),
                     screen_lines ),
             };
             ec == std::errc() ) {
            fmt::println( std::cerr, "Screen rows Result: {} ptr-> {}", screen_lines, ptr );

            if ( ec == std::errc::invalid_argument ) {
                throw std::system_error(
                    std::make_error_code( std::errc::invalid_argument ), "Not a number." );
            } else if ( ec == std::errc::result_out_of_range ) {
                throw std::system_error(
                    std::make_error_code( std::errc::result_out_of_range ),
                    "The number is to large." );
            }
        }

        if ( const auto [ptr, ec]{
                 std::from_chars(
                     buffer.data(), std::ranges::next( buffer.begin(), write_count ),
                     screen_columns ),
             };
             ec == std::errc() ) {
            fmt::println( std::cerr, "Screen columns Result: {} ptr-> {}", screen_columns, ptr );

            if ( ec == std::errc::invalid_argument ) {
                throw std::system_error(
                    std::make_error_code( std::errc::invalid_argument ), "Not a number." );
            } else if ( ec == std::errc::result_out_of_range ) {
                throw std::system_error(
                    std::make_error_code( std::errc::result_out_of_range ),
                    "The number is to large." );
            }
        }
    }
}; // namespace LuxLibrary
