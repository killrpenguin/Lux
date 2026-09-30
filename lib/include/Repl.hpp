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

#include "Value.hpp"

#include <cstddef>
#include <span>
#include <termios.h>

namespace LuxLibrary {
    namespace ESC {
        constexpr char EscapeChar{ '\x1b' };
        constexpr std::string GetCursorPos{ "\x1b[6n" };
        constexpr std::string MaxScreenPos{ "\x1b[999C\x1b[999B" };

    }; // namespace ESC

    class Repl {
      private:
        termios original_termios{};
        bool raw_mode_enabled{ false };

        bool running{ true };

        std::size_t cursor_column{};
        std::size_t cursor_line{};

        std::size_t screen_lines{};
        std::size_t screen_columns{};

        constexpr auto SetResizeSigAction() -> void;
        constexpr auto ConfigureTerminal() -> void;
        constexpr auto ManualCursorPosition() -> void;
        constexpr auto ResetTerminal() noexcept -> void;

      public:
        Repl() = default;
        ~Repl() noexcept;

        Repl( const Repl& other )                = delete;
        Repl( Repl&& other ) noexcept            = delete;
        Repl& operator=( const Repl& other )     = delete;
        Repl& operator=( Repl&& other ) noexcept = delete;

        static Repl* InstancePtr; // required for interacting with the C sigaction api.

        auto Initialize( std::istream& input ) noexcept -> void;
        auto Running() const noexcept -> bool;

        auto Read( std::istream& input, std::ostream& output ) noexcept -> Expression;
        auto Print(
            std::span< const Expression > inputs, std::span< const Result< Value > > results,
            std::ostream& output ) noexcept -> void;

        auto GetWindowSize() -> void;
        auto ResetScreen( std::ostream& output ) noexcept -> void;
    };
}; // namespace LuxLibrary
