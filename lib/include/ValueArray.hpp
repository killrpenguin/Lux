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
#include "Value.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <fmt/base.h>
#include <fmt/ostream.h>

namespace LuxLibrary {
    class ValueArray {
        using Vector = std::vector< Value >;

      private:
        Vector values;

      public:
        using size_type  = Vector::size_type;
        using value_type = Vector::value_type;

        constexpr explicit ValueArray( const std::size_t reserve_size = 8 ) noexcept {
            values.reserve( reserve_size );
        };

        ~ValueArray() noexcept = default;

        ValueArray( const ValueArray& other )                = default;
        ValueArray( ValueArray&& other ) noexcept            = default;
        ValueArray& operator=( const ValueArray& other )     = default;
        ValueArray& operator=( ValueArray&& other ) noexcept = default;

        /**
         * @brief Add a byte to the end of the chunk.
         */
        auto WriteValueArray( const value_type& byte ) noexcept -> void;

        /**
         * @brief Get the capacity of the chunk.
         * @return The number of LuxBytes the chunk can hold.
         */
        auto Capacity() const noexcept -> size_type;

        /**
         * @brief Get the size of the chunk.
         * @return The number of LuxBytes in the chunk.
         */
        auto Count() const noexcept -> size_type;

        template < typename Self > auto cbegin( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).values.cbegin();
        }

        template < typename Self > auto cend( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).values.cend();
        }

        template < typename Self > auto begin( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).values.begin();
        }

        template < typename Self > auto end( this Self&& self CLANG_LIFETIME_BOUND ) {
            return std::forward< Self >( self ).values.end();
        }

        template < typename Self >
        constexpr decltype( auto ) operator[](
            this Self&& self CLANG_LIFETIME_BOUND, std::size_t index ) {
            try {
                return std::forward< Self >( self ).values.at( index );
            } catch ( const std::out_of_range& err ) {
                fmt::println( std::cerr, "Index {} outof bounds.\nMessage: {}", index, err.what() );
                return std::forward< Self >( self ).values.back();
            } catch ( ... ) {
                std::cerr << "Uncaught exception..";
                std::exit( EXIT_FAILURE );
            }
        }
    };
} // namespace LuxLibrary
