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

#include "Chunk.hpp"
#include "Common.hpp"
#include "OpCode.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "TokenVariant.hpp"
#include "Value.hpp"

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace LuxLibrary {

    namespace detail {
        template < typename... Args >
        concept IsEnumType =
            requires() { sizeof...( Args ) > 0 && ( std::is_enum_v< Args > && ... ); };

    } // namespace detail

    class Compiler {
      private:
        Chunk* current_chunk{};
        Parser parser{};
        Scanner scanner{};

        auto EmitByte( const OpCode byte ) noexcept -> void;
        auto EmitByte( const std::uint8_t byte ) noexcept -> void;
        auto EmitConstant( const Value& val ) noexcept -> void;
        auto EmitReturn() noexcept -> void;
        auto EndCompiler() noexcept -> void;
        auto Error( const std::string_view message ) noexcept -> void;
        auto ErrorAtCurrent( const std::string_view message ) noexcept -> void;
        auto MakeConstant( const Value& val ) noexcept -> std::uint8_t;
        auto ParsePrecedence( Precedence precedence ) noexcept -> void;

      public:
        Compiler() = default;
        Compiler( Scanner scanner, Parser parser ) noexcept;
        ~Compiler() noexcept = default;

        Compiler( const Compiler& other )                = default;
        Compiler( Compiler&& other ) noexcept            = default;
        Compiler& operator=( const Compiler& other )     = default;
        Compiler& operator=( Compiler&& other ) noexcept = default;

        std::string error_message{};

        auto Advance() noexcept -> void;
        auto Compile( const std::string& source, Chunk& chunk ) noexcept -> Result< void >;
        auto Consume( const TokenType token, const std::string_view message ) noexcept -> void;
        auto Expression() noexcept -> void;

        auto Binary() noexcept -> void;
        auto Grouping() noexcept -> void;
        auto Literal() noexcept -> void;
        auto Number() noexcept -> void;
        auto String() noexcept -> void;
        auto Char() noexcept -> void;
        auto Unary() noexcept -> void;

        auto ErrorAt( const Token& token, const std::string_view message ) noexcept -> void;

        template < detail::IsEnumType... Args > auto EmitBytes( Args&&... args ) noexcept -> void {
            ( this->EmitByte( std::forward< Args >( args ) ), ... );
        }
    };
}; // namespace LuxLibrary
