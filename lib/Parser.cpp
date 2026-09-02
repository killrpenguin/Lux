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

#include "Parser.hpp"
#include "Token.hpp"

namespace LuxLibrary {
    auto Parser::Stash() noexcept -> void {
        previous = current;
    }

    auto Parser::Current() const noexcept -> const Token& {
        return current;
    }

    auto Parser::CurrentLine() const noexcept -> line_type {
        return current.line;
    }

    auto Parser::Previous() const noexcept -> Token {
        return previous;
    }

    auto Parser::PreviousLine() const noexcept -> line_type {
        return previous.line;
    }

    auto Parser::PanicMode() const noexcept -> bool {
        return had_error;
    }

    auto Parser::PanicAndError() noexcept -> void {
        Panic();
        SetError();
    }

    auto Parser::Panic() noexcept -> void {
        panic_mode = true;
    }

    auto Parser::HadError() const noexcept -> bool {
        return had_error;
    }

    auto Parser::SetError() noexcept -> void {
        had_error = true;
    }

    auto Parser::ErrorToken() const noexcept -> bool {
        return current.Is( TokenType::ERROR );
    }

    auto Parser::ResetErrorFlags() noexcept -> void {
        had_error  = false;
        panic_mode = false;
    }

    auto Parser::CurrentType( const TokenType token ) const noexcept -> bool {
        return current.type == token;
    }

    auto Parser::CurrentIs( const Token& token ) const noexcept -> bool {
        return current == token;
    }
}; // namespace LuxLibrary
