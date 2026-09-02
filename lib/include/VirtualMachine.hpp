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
#include "Compiler.hpp"
#include "Object.hpp"
#include "Repl.hpp"
#include "Stack.hpp"
#include "Value.hpp"

#include <filesystem>
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace LuxLibrary {
    namespace fs = std::filesystem;
    using namespace detail;

    using StackOptional = std::optional< Value >;

    class VirtualMachine {
      private:
        const Chunk* current_chunk{ nullptr };
        Chunk::const_iterator ip{};
        SmallStack< Value > stack{};

        Compiler compiler{};

        Repl repl{};
        std::vector< Expression > expressions;
        std::vector< Result< Value > > results;

        std::vector< Object > objects{};

        auto static ReadFile( const fs::path& path ) noexcept -> std::string;
        auto TraceDebug() const noexcept -> void;

        auto AdvanceInstructionPtr() noexcept -> void;
        auto InstructionPtrValue() noexcept -> Instruction;
        auto InstructionPtrConstant() noexcept -> Value;

      public:
        VirtualMachine() noexcept = default;

        ~VirtualMachine() noexcept = default;

        VirtualMachine( const VirtualMachine& other )                = delete;
        VirtualMachine( VirtualMachine&& other ) noexcept            = delete;
        VirtualMachine& operator=( const VirtualMachine& other )     = delete;
        VirtualMachine& operator=( VirtualMachine&& other ) noexcept = delete;

        /*
         * @brief The entry point to the virtual machine..
         * @param const std::string& A string of Lux source code to interpret.
         * @return InterpretResult The possible results.
         */
        auto Interpret( const std::string& source ) noexcept -> Result< Value >;

        /*
         * @brief The entry point to the virtual machine..
         * @param const std::string& A string of Lux source code to interpret.
         * @return InterpretResult The possible results.
         */
        auto Run() noexcept -> Result< Value >;

        /*
         * @brief The entry point to the Lux repl.
         * @param std::istream& A C++ input stream for the repl to write to.
         * @param std::ostream& A C++ output stream for the repl to write to.
         */
        auto RunRepl( std::istream& input = std::cin, std::ostream& output = std::cout ) noexcept
            -> void;

        /*
         * @brief The entry point to compiling Lux source code.
         * @param const std::string& The file to be compiled.
         */
        auto RunFile( const fs::path& file_name ) noexcept -> void;

        /*
         * @brief The entry point to compiling Lux source code.
         * @param const std::string& The file to be compiled.
         */
        template < typename... Args >
            requires( fmt::formattable< Args, char > && ... )
        auto RuntimeError( Args... args ) noexcept -> std::string;
    };

    template < typename... Args >
        requires( fmt::formattable< Args, char > && ... )
    auto VirtualMachine::RuntimeError( Args... args ) noexcept -> std::string {
        static_assert( sizeof...( args ) != 0 );
        assert(
            current_chunk != nullptr &&
            "Current chunk should never be a nullptr in RuntimeError()." );

        const std::ptrdiff_t ip_distance{ ranges::distance( current_chunk->begin(), ip ) };

        assert( ip_distance != 0 && "instruction ptr distance was 0." );

        const std::size_t line{ current_chunk->GetLine(
            static_cast< std::size_t >( ip_distance - 1 ) ) };

        stack.Reset();

        return fmt::format( "[RunTime Error] line: {} {}", line, std::forward< Args >( args )... );
    }
} // namespace LuxLibrary
