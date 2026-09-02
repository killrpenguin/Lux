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

#include "Chunk.hpp"
#include <limits>

namespace LuxLibrary {

    template <>
    // NOLINTNEXTLINE
    auto Chunk::WriteChunk< Chunk::value_type >(
        const value_type byte, const unsigned int line ) noexcept -> void {
        code.emplace_back( byte );

        lines.emplace_back( line );
    }

    template <>
    auto Chunk::WriteChunk< OpCode >( const OpCode byte, const unsigned int line ) noexcept
        -> void {
        WriteChunk( std::to_underlying( byte ), line );
    }

    auto Chunk::Capacity() const noexcept -> size_type {
        return code.capacity();
    }

    auto Chunk::Count() const noexcept -> size_type {
        return code.size();
    }

    auto Chunk::AddConstant( const Value& value ) noexcept -> std::uint8_t {
        constants.WriteValueArray( value );

        assert(
            constants.Count() - 1 != std::numeric_limits< std::uint8_t >::max() &&
            "To many constants in Chunk::AddConstant()" );

        return static_cast< std::uint8_t >( constants.Count() ) - 1;
    }

    auto Chunk::GetConstant( const std::size_t offset ) const noexcept -> Value {
        return constants[offset];
    }

    auto Chunk::GetLine( const std::size_t offset ) const noexcept -> Lines::value_type {
        try {
            return lines.at( offset );
        } catch ( const std::out_of_range& err ) {
            fmt::println(
                std::cerr, "{}\nIndex {} out of bounds. Returning last value.", err.what(),
                offset );

            return lines.back();
        } catch ( ... ) {
            std::cerr << "Uncaught exception..";
            std::exit( EXIT_FAILURE );
        }
    }
} // namespace LuxLibrary
