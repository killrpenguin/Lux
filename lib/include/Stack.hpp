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
#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>

#include <fmt/base.h>
#include <fmt/ostream.h>

namespace LuxLibrary {

    namespace ranges = std::ranges;

    namespace detail {
        constexpr std::size_t SmallStackSize{ 256 };
    }

    template < typename T, std::size_t MaxCapacity = detail::SmallStackSize >
        requires std::default_initializable< T >
    class Stack {
        using Buffer  = std::array< T, MaxCapacity >;
        using pBuffer = std::unique_ptr< Buffer >;

        pBuffer buff{ std::make_unique< Buffer >() };
        Buffer::iterator finger{ buff->begin() };
        Buffer::iterator buff_begin{ buff->begin() };
        Buffer::iterator buff_end{ buff->end() };

      public:
        using value_type      = T;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;

        /**
         * @brief
         */
        Stack() noexcept  = default;
        ~Stack() noexcept = default;

        Stack( const Stack& other )                = delete;
        Stack( Stack&& other ) noexcept            = default;
        Stack& operator=( const Stack& other )     = delete;
        Stack& operator=( Stack&& other ) noexcept = default;

        /**
         * @brief
         * @param  const T& item
         */
        auto Push( const T& item ) noexcept -> void;

        /**
         * @brief
         * @param  const T& item
         */
        template < typename ValueInner >
            requires detail::IsValueVariant< ValueInner, detail::Tag >
        auto Push( const ValueInner& item ) noexcept -> void;

        /**
         * @brief
         * @return std::optional<T>
         */
        auto Pop() noexcept -> std::optional< T >;

        /**
         * @brief
         * @return bool
         */
        auto Clear() noexcept -> bool;

        /**
         * @brief Checks if the stack has no elements.
         * @return bool True if the container is empty, false otherwise.
         */
        auto Empty() const noexcept -> bool;

        /**
         * @brief
         * @return std::optional<size_type>
         */
        auto Size() const noexcept -> std::optional< size_type >;

        /**
         * @brief
         * @return
         */
        auto Reset() noexcept -> void;

        /**
         * @brief Get the maximum number of elements.
         * @return size_type Returns the maximum possible number of elements as size_type.
         */
        auto Capacity() const noexcept -> size_type;

        /**
         * @brief
         * @return
         */
        auto PeekPrev( const std::size_t distance = 0 ) const noexcept -> const value_type&;

        /**
         * @brief Get a pointer to the top of the stack.
         * @return Iterator to the first element.
         */
        template < typename Self >
        constexpr decltype( auto ) Top( this Self&& self CLANG_LIFETIME_BOUND ) noexcept {
            return std::forward< Self >( self ).finger;
        }

        /**
         * @brief Get an iterator to the beginning of the stack.
         * @return Iterator to the first element.
         */
        template < typename Self >
        constexpr decltype( auto ) cbegin( this Self&& self CLANG_LIFETIME_BOUND ) noexcept {
            return std::forward< Self >( self ).buff->begin();
        }

        /**
         * @brief Get an iterator to the beginning of the stack.
         * @return Iterator to the first element.
         */
        template < typename Self >
        constexpr decltype( auto ) cend( this Self&& self CLANG_LIFETIME_BOUND ) noexcept {
            const difference_type current_size{ ranges::distance( self.buff_begin, self.finger ) };
            return ranges::next( std::forward< Self >( self ).buff->begin(), current_size );
        }
        /**
         * @brief Get an iterator to the beginning of the stack.
         * @return Iterator to the first element.
         */
        template < typename Self >
        constexpr decltype( auto ) begin( this Self&& self CLANG_LIFETIME_BOUND ) noexcept {
            return std::forward< Self >( self ).buff->begin();
        }

        /**
         * @brief Get an iterator to the beginning of the stack.
         * @return Iterator to the first element.
         */
        template < typename Self >
        constexpr decltype( auto ) end( this Self&& self CLANG_LIFETIME_BOUND ) noexcept {
            const difference_type current_size{ ranges::distance( self.buff_begin, self.finger ) };
            return ranges::next( std::forward< Self >( self ).buff->begin(), current_size );
        }
    };

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Reset() noexcept -> void {
        finger = buff_begin;
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::PeekPrev( const std::size_t distance ) const noexcept
        -> const value_type& {
        const auto tmp{ ranges::prev( finger, 1 + distance, buff_begin ) };
        return *tmp;
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Push( const T& item ) noexcept -> void {
        if ( finger == nullptr ) {
            fmt::println( std::cerr, "Finger was a nullptr in Stack::Push()" );
            return;
        }

        if ( finger == buff_end ) {
            fmt::println( std::cerr, "The stack is full" );
            return;
        }
        *finger = item;
        ranges::advance( finger, 1 );
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    template < typename ValueInner >
        requires detail::IsValueVariant< ValueInner, detail::Tag >
    auto Stack< T, MaxCapacity >::Push( const ValueInner& item ) noexcept -> void {
        Push( Value( item ) );
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Pop() noexcept -> std::optional< T > {
        if ( Empty() ) { return std::nullopt; }
        finger = ranges::prev( finger, 1, buff_begin );

        return *finger;
    };

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Empty() const noexcept -> bool {
        if ( finger == nullptr ) {
            fmt::println( std::cerr, "nullptr in Stack::Empty()" );
            return true;
        }
        return finger == buff_begin;
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Size() const noexcept -> std::optional< size_type > {
        if ( finger == nullptr ) {
            fmt::println( std::cerr, " nullptr in Stack::Size()" );
            return std::nullopt;
        }
        return ranges::distance( buff_begin, finger );
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Capacity() const noexcept -> size_type {
        return MaxCapacity;
    }

    template < typename T, std::size_t MaxCapacity >
        requires std::default_initializable< T >
    auto Stack< T, MaxCapacity >::Clear() noexcept -> bool {
        auto ret = ranges::fill_n( buff_begin, buff->size(), T{} );
        return ret == buff_end;
    }

    template < typename T > using SmallStack = Stack< T, detail::SmallStackSize >;

} // namespace LuxLibrary
