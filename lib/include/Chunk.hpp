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
#include "OpCode.hpp"
#include "Value.hpp"
#include "ValueArray.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <cstddef>

namespace LuxLibrary {

    class Chunk {
        using ByteChunk = std::vector< Instruction >;
        using Lines     = std::vector< unsigned int >;

      private:
        ByteChunk code{};

        Lines lines{};
        ValueArray constants{};

      public:
        using value_type      = ByteChunk::value_type;
        using size_type       = ByteChunk::size_type;
        using difference_type = ByteChunk::difference_type;
        using reference       = ByteChunk::reference;
        using const_reference = ByteChunk::const_reference;
        using pointer         = ByteChunk::pointer;
        using const_pointer   = ByteChunk::const_pointer;

        using iterator       = ByteChunk::iterator;
        using const_iterator = ByteChunk::const_iterator;

        constexpr explicit Chunk( const std::size_t reserve_size = 8 ) noexcept {
            code.reserve( reserve_size );
        };

        ~Chunk() noexcept = default;

        Chunk( const Chunk& other )                = default;
        Chunk( Chunk&& other ) noexcept            = default;
        Chunk& operator=( const Chunk& other )     = default;
        Chunk& operator=( Chunk&& other ) noexcept = default;

        /**
         * @brief Add an element to the end of the byte chunk array.
         * @param byte The byte to add.
         * @param line The line assoicated to the byte.
         */
        template < typename T >
        auto WriteChunk( const T byte, const unsigned int line ) noexcept -> void;

        template < value_type >
        auto WriteChunk( const value_type byte, const unsigned int line ) noexcept -> void;

        template < OpCode >
        auto WriteChunk( const OpCode byte, const unsigned int line ) noexcept -> void;

        /**
         * @brief Add an element to the constants array.
         * @param value the value to add.
         */
        auto AddConstant( const Value& value ) noexcept -> std::uint8_t;

        /**
         * @brief Get an element from the constants array at the given offset.
         * @param offset Offset of the value.
         */
        auto GetConstant( const std::size_t offset ) const noexcept -> Value;

        /**
         * @brief Get an element from the lines array at the given offset.
         * @param offset Offset of the value.
         */
        auto GetLine( const std::size_t offset ) const noexcept -> Lines::value_type;

        /**
         * @brief Get the number of elements the byte chunk array can hold in currently allocated
         * storage.
         * @return The number of LuxBytes the chunk can hold.
         */
        auto Capacity() const noexcept -> size_type;

        /**
         * @brief Get the number of elements in the byte chunk array.
         * @return The number of elements currently in the chunk.
         */
        auto Count() const noexcept -> size_type;

        /**
         * @brief .
         * @return .
         */
        auto IsActive( const ByteChunk::iterator itr ) const noexcept -> bool;

        /**
         * @brief Get a constant iterator to the beginning of the byte chunk array.
         * @return LegacyRandomAccessIterator, contiguous_iterator, and ConstexprIterator to const
         * value_type.
         */
        template < typename Self > auto cbegin( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).code.cbegin();
        }

        /**
         * @brief Get a constant iterator to the end of the byte chunk array.
         * @return LegacyRandomAccessIterator, contiguous_iterator, and ConstexprIterator to const
         * value_type.
         */
        template < typename Self > auto cend( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).code.cend();
        }

        /**
         * @brief Get an iterator to the beginning of the byte chunk array.
         * @return LegacyRandomAccessIterator, contiguous_iterator, and ConstexprIterator to
         * value_type.
         */
        template < typename Self > auto begin( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).code.begin();
        }

        /**
         * @brief Get an iterator to the end of the byte chunk array.
         * @return LegacyRandomAccessIterator, contiguous_iterator, and ConstexprIterator to
         * value_type.
         */
        template < typename Self > auto end( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).code.end();
        }

        /**
         * @brief operator[] access specified element with bounds checking.
         * @param index Index of the element in the byte chunk array or the last value if index is
         * out of bounds.
         */
        template < typename Self >
        constexpr decltype( auto ) operator[](
            this Self&& self CLANG_LIFETIME_BOUND, std::size_t index ) {
            try {
                return std::forward< Self >( self ).code.at( index );
            } catch ( const std::out_of_range& err ) {
                fmt::println( std::cerr, "Index {} out of bounds.\nMessage: {}", index, err.what() );
                std::exit( EXIT_FAILURE );
                return std::forward< Self >( self ).code.back();
            } catch ( ... ) {
                fmt::println( std::cerr, "Uncaught exception." );
                std::exit( EXIT_FAILURE );
            }
        }
    };
} // namespace LuxLibrary
