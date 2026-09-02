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
#include "OpCode.hpp"

#include <cstddef>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>

namespace LuxLibrary {

    class Debug {
        auto static To_OpCode( const std::uint8_t instruction ) noexcept -> std::optional< OpCode >;

      public:
        using OpCodeAndLine = std::optional< std::pair< OpCode, unsigned int > >;

        constexpr Debug() noexcept                 = default;
        ~Debug() noexcept                          = default;
        Debug( const Debug& other )                = default;
        Debug( Debug&& other ) noexcept            = default;
        Debug& operator=( const Debug& other )     = default;
        Debug& operator=( Debug&& other ) noexcept = default;

        /**
         * @brief Disassemble all the bytes in a chunk and output the instructions to the given
         * stream.
         * @param const Chunk& The target chunk.
         * @param std::string_view A header for the disassembled output.
         * @param std::ostream& The stream object used to output the instructions.
         */
        auto static DisassembleChunk(
            const Chunk& chunk, const std::string_view name = "Chunk",
            std::ostream& stream = std::cout ) noexcept -> void;

        /**
         * @brief Disassemble a byte in a chunk at the given offset and output the instructions to
         * the given stream.
         * @param const Chunk& The target chunk.
         * @param const std::size_t The position of the element in a chunk to dissassemble.
         * @param const std::ostream& The C++ stream object to output the instructions to.
         * @return std::size_t The offset of the next instruction or 0 if none.
         */
        auto static DisassembleInstruction(
            const Chunk& chunk, const std::size_t offset,
            std::ostream& stream = std::cout ) noexcept -> std::size_t;

        /**
         * @brief A wrapper function for printing a simple OpCode at a given offset.
         * @param const std::string& The OpCode name to output when writing out an instruction to a
         * stream.
         * @param const std::size_t The position of an element in the given chunk to dissassemble.
         * @param const std::ostream& The C++ stream object to output the instructions to.
         * @return std::size_t The offset of the next instruction.
         */
        auto static SimpleInstruction(
            const OpCode instruction, const std::size_t offset, std::ostream& stream ) noexcept
            -> std::size_t;

        /**
         * @brief A wrapper function for printing the Constant OpCode.
         * @param const std::string& The OpCode name to output when writing out an instruction to a
         * stream.
         * @param const std::size_t The position of an element in the given chunk to dissassemble.
         * @param const std::ostream& The C++ stream object to output the instructions to.
         * @return std::size_t The offset of the next instruction.
         */
        auto static ConstantInstruction(
            const OpCode instruction, const Chunk& chunk, const std::size_t offset,
            std::ostream& stream = std::cout ) noexcept -> std::size_t;

        /**
         * @brief A function to get an Opcode from a chunk offset.
         * @param const Chunk& The target chunk.
         * @param const std::size_t The position of an element in the given chunk.
         * @return std::pair<OpCode, std::size_t> The instruction at the given offset and its line
         * number.
         */
        auto static GetOpcode( const Chunk& chunk, const std::size_t offset ) noexcept
            -> std::optional< std::pair< OpCode, unsigned int > >;
    };
} // namespace LuxLibrary
