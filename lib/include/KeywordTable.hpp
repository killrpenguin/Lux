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

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace LuxLibrary {
    namespace keyword_details {
        constexpr static std::uint32_t DefaultSeed{ 0x811C9DC5 };

        static constexpr std::size_t KeywordCount{ 16 };
        using KeywordType   = std::string_view;
        using KeywordsArray = std::array< KeywordType, KeywordCount >;

        static constexpr KeywordsArray LuxKeywords{
            "and",   "class", "else",  "if",   "nil",  "or",    "print", "return",
            "super", "var",   "while", "this", "true", "false", "for",   "fun",
        };

        template < std::size_t NUM > consteval auto IsPrime() noexcept -> bool {
            if constexpr ( NUM <= 1 ) { return false; }
            if constexpr ( NUM <= 3 ) { return true; }
            if constexpr ( NUM % 2 == 0 or NUM % 3 == 0 ) { return false; }

            constexpr std::size_t Start{ 5 };
            constexpr std::size_t Step{ 6 };

            for ( std::size_t idx{ Start }; idx * idx <= NUM; idx += Step ) {
                if ( NUM % idx == 0 or NUM % ( idx + 2 ) == 0 ) { return false; }
            }

            return true;
        }

        // FNV-1a hash function.
        constexpr auto seeded_hash( const std::string_view str, const std::uint32_t seed )
            -> std::uint32_t {
            std::uint32_t hash = seed;

            constexpr std::uint32_t FNVPrime{ 0x01000193 };
            for ( char ltr : str ) {
                hash ^= static_cast< std::uint32_t >( ltr );
                hash *= FNVPrime;
            }
            return hash;
        }

        template < std::size_t ArrSize, std::size_t TblSize >
        consteval auto find_seed( const std::array< std::string_view, ArrSize >& keys )
            -> std::uint32_t {
            constexpr std::size_t SeedMax{ 100000 };

            for ( std::uint32_t seed = 0; seed < SeedMax; ++seed ) {
                std::array< bool, TblSize > occupied{};
                bool collision = false;

                for ( const auto& key : keys ) {
                    const std::uint32_t idx = seeded_hash( key, seed ) % TblSize;
                    if ( occupied.at( idx ) ) {
                        collision = true;
                        break;
                    }
                    occupied.at( idx ) = true;
                }

                if ( !collision ) { return seed; }
            }
            // Failed. Return the 32bit FNV-1a default offset basis tuned by people smarter than I.
            constexpr std::uint32_t DefaultSeed{ 0x811C9DC5 };
            return DefaultSeed;
        }

        struct Slot {
            constexpr Slot() = default;

            template < typename T >
                requires std::is_same_v< T, std::uint32_t >
            constexpr explicit Slot( const T val )
                : key{ val >> ShiftAmt } {};

            constexpr auto Exists() const noexcept -> bool {
                return !static_cast< bool >( key >> ShiftAmt );
            }

            friend constexpr auto operator==( const Slot& lhs, const std::size_t& rhs ) -> bool {
                return lhs.key == ( rhs >> ShiftAmt );
            }

          private:
            constexpr static std::uint32_t DefaultValue{ 0x80000000U };
            constexpr static std::uint32_t ShiftAmt{ 31 };

            std::uint32_t key{ DefaultValue };
        };

        constexpr static std::uint32_t hashed_test_key{ seeded_hash( "Apple", DefaultSeed ) };
        constexpr static Slot slot{ hashed_test_key };
        static_assert( slot.Exists() );
        static_assert( slot == static_cast< std::size_t >( hashed_test_key ) );

        constexpr static Slot EmptySlot{};
        static_assert( !EmptySlot.Exists() );

    }; // namespace keyword_details

    using namespace keyword_details;

    template < std::size_t Size_N > class KeywordTable {
        static constexpr size_t TableSize = 67;
        // 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71
        static_assert(
            TableSize > Size_N, "TableSize must be greater than the size of the keywords "
                                "array being hashed." );

        static_assert( IsPrime< TableSize >(), "TableSize must be a prime number." );

        std::array< Slot, TableSize > table{};
        std::uint32_t seed{ 0 };

      public:
        KeywordTable() noexcept  = delete;
        ~KeywordTable() noexcept = default;

        KeywordTable( const KeywordTable& other )                = default;
        KeywordTable( KeywordTable&& other ) noexcept            = default;
        KeywordTable& operator=( const KeywordTable& other )     = default;
        KeywordTable& operator=( KeywordTable&& other ) noexcept = default;

        constexpr KeywordTable(
            const std::array< std::string_view, Size_N >& lux_keys = LuxKeywords )
            : seed( find_seed< Size_N, TableSize >( lux_keys ) ) {
            for ( const auto& key : lux_keys ) {
                std::size_t idx{ seeded_hash( key, seed ) % TableSize };

                constexpr std::uint32_t Bits{ 31 };
                idx ^= ( idx >> Bits );
                try {
                    table.at( idx ) = Slot( static_cast< std::uint32_t >( idx ) );
                } catch ( const std::out_of_range& err ) {
                    std::cerr << "Index out of bounds in KeywordTable constructor." << err.what();
                    break;
                }
            }
        }

        constexpr auto contains( const std::string_view key ) const -> bool {
            const std::size_t idx{ seeded_hash( key, seed ) % TableSize };
            try {
                return table.at( idx ).Exists() and table.at( idx ) == idx;
            } catch ( const std::out_of_range& err ) {
                std::cerr << "Index out of bounds in KeywordsTable->contains()." << err.what();
                return false;
            }
        }
    };

    namespace testing {
        constexpr static auto CompMap = KeywordTable< KeywordCount >( LuxKeywords );

        template < std::size_t... Idx, typename Fn >
            requires std::invocable< Fn, KeywordType > and std::copy_constructible< Fn >
        constexpr auto all_keywords_hashed_impl(
            const KeywordsArray& arr, std::index_sequence< Idx... > /**/, Fn&& func ) -> bool {
            return ( std::forward< Fn >( func )( arr.at( Idx ) ), ... );
        }

        constexpr auto all_keywords_hashed() -> bool {
            return all_keywords_hashed_impl(
                       LuxKeywords, std::make_index_sequence< KeywordCount >{},
                       []( const KeywordType key ) constexpr -> bool {
                           return CompMap.contains( key );
                       } ) and
                   !CompMap.contains( std::string_view( "Invalid" ) );
        }

        static_assert( all_keywords_hashed() );
    } // namespace testing
} // namespace LuxLibrary
