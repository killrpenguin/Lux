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

#include <memory>
#include <utility>

namespace LuxLibrary {
    class GarbageCollector {
      public:
        using size_type     = std::size_t;
        using pointer       = Value*;
        using const_pointer = const Value*;

        GarbageCollector() = default;
        ~GarbageCollector() noexcept;

        GarbageCollector( const GarbageCollector& other )            = delete;
        GarbageCollector& operator=( const GarbageCollector& other ) = delete;

        GarbageCollector( GarbageCollector&& other ) noexcept            = default;
        GarbageCollector& operator=( GarbageCollector&& other ) noexcept = default;

        /**
         * @brief Add an element to the front of the list.
         * @param const T& Any type that could be stored in the Value variant.
         */
        template < typename T >
            requires IsValueVariant< T, detail::Tag >
        constexpr auto push_front( T val ) noexcept -> void;

        /**
         * @brief Add an element to the front of the list.
         * @param const Value& the value to add.
         */
        auto push_front( const Value& val ) noexcept -> void;

        /**
         * @brief Add an element to the end of the list.
         * @param const T& Any type that could be stored in the Value variant.
         */
        template < typename T >
            requires IsValueVariant< T, detail::Tag >
        constexpr auto push_back( T val ) noexcept -> void;

        /**
         * @brief Add an element to the end of the list.
         * @param const Value& the value to add.
         */
        auto push_back( const Value& val ) noexcept -> void;

        /**
         * @brief Check if the list is empty.
         */
        auto empty() const noexcept -> bool;

        /**
         * @brief Return the number of elements in the linked list.
         */
        auto length() const noexcept -> size_type;

        /**
         * @brief operator[] access specified element with bounds checking.
         * @param index size_type of the element in the linked list or nullptr if out of bounds.
         */
        auto operator[]( const size_type index ) CLANG_LIFETIME_BOUND->pointer;

      private:
        struct Node;
        using uPtr = std::unique_ptr< Node >;

        struct Node {
            Value value{};
            uPtr next{ nullptr };

            explicit Node( Value val )
                : value{ std::move( val ) } {
            }
        };

        uPtr head{};
        size_type count{};
    };

    template < typename T >
        requires IsValueVariant< T, detail::Tag >
    constexpr auto GarbageCollector::push_front( T val ) noexcept -> void {
        auto new_node{ std::make_unique< Node >( Value( val ) ) };

        new_node->next = std::move( head );

        head = std::move( new_node );

        count++;
    }

    /**
     * @brief Add an element to the end of the list.
     * @param const T& Any type that could be stored in the Value variant.
     */
    template < typename T >
        requires IsValueVariant< T, detail::Tag >
    constexpr auto GarbageCollector::push_back( T val ) noexcept -> void {
        auto new_node{ std::make_unique< Node >( Value( val ) ) };
        count++;

        if ( head == nullptr ) {
            head = std::move( new_node );
            return;
        }

        Node* current{ head.get() };

        while ( current->next != nullptr ) {
            current = current->next.get();
        }
        current->next = std::move( new_node );
    }
}; // namespace LuxLibrary
