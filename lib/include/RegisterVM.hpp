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

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>

using Instruction = std::uint16_t;
namespace RegisterVM {
    namespace detail {
        constexpr std::size_t MemMax{ std::numeric_limits< std::uint16_t >::max() + 1 };
        constexpr std::uint16_t ByteMax{ 0xFFFFU };
        constexpr std::uint16_t PC_START{ 0x3000U };
        constexpr std::size_t NOPS{ 15 };
        constexpr std::size_t trp_offset{ 0x20U };

        constexpr std::size_t Rg0{ 0 };
        constexpr std::size_t Rg1{ 1 };
        constexpr std::size_t Rg2{ 2 };
        constexpr std::size_t Rg3{ 3 };
        constexpr std::size_t Rg4{ 4 };
        constexpr std::size_t Rg5{ 5 };
        constexpr std::size_t Rg6{ 6 };
        constexpr std::size_t Rg7{ 7 };
        constexpr std::size_t RPC{ 8 };
        constexpr std::size_t RCND{ 9 };
        constexpr std::size_t RCNT{ 10 };

        constexpr auto sext( const Instruction instr, const std::uint16_t val ) -> Instruction {
            return ( ( static_cast< std::uint16_t >(
                           ( instr >> static_cast< std::uint16_t >(( val - 0x1U )) )) &
                       0x1U ) != 0U )
                       ? ( instr | static_cast< std::uint16_t >(( ByteMax << val )) )
                       : instr;
        }
    }; // namespace detail

    using namespace detail;

    struct MainMemory {
        static constexpr auto mem_read( const Instruction address ) -> Instruction {
            try {
                return memory.at( address );
            } catch ( const std::out_of_range& err ) {
                assert( false && "Memory out of bounds access." );
            }
        }

        static constexpr auto mem_write( const Instruction address, const Instruction val ) noexcept
            -> void {
            try {
                memory.at( address ) = val;
            } catch ( const std::out_of_range& err ) {
                assert( false && "Memory out of bounds access." );
            }
        }

        template < typename Self > auto&& operator[]( this Self&& self, std::size_t idx ) {
            return std::forward< decltype( self ) >( self ).memory[idx];
        }

        static constexpr auto data() -> unsigned short* {
            return memory.data();
        }

      private:
        static inline std::array< Instruction, MemMax > memory{};
    };

    struct Registers {
        friend struct OpsArray;

        template < typename Self >
        auto&& operator[]( this Self&& self CLANG_LIFETIME_BOUND, std::size_t idx ) {
            return std::forward< decltype( self ) >( self ).registers[idx];
        }

      private:
        static constexpr auto _and( const Instruction instr ) -> void;
        static constexpr auto add( const Instruction instr ) -> void;
        static constexpr auto sub( const Instruction instr ) -> void;
        static constexpr auto ld( const Instruction instr ) -> void;
        static constexpr auto ldi( const Instruction instr ) -> void;
        static constexpr auto ldr( const Instruction instr ) -> void;
        static constexpr auto lea( const Instruction instr ) -> void;
        static constexpr auto _not( const Instruction instr ) -> void;
        static constexpr auto st( const Instruction instr ) -> void;
        static constexpr auto sti( const Instruction instr ) -> void;
        static constexpr auto str( const Instruction instr ) -> void;
        static constexpr auto jmp( const Instruction instr ) -> void;
        static constexpr auto jsr( const Instruction instr ) -> void;
        static constexpr auto br( const Instruction instr ) -> void;
        static constexpr auto trap( const Instruction instr ) -> void;

        /* Trap functions */
        static constexpr auto tgetc() -> void;
        static constexpr auto tout() -> void;
        static constexpr auto tputs() -> void;
        static constexpr auto tin() -> void;
        static constexpr auto thalt() -> void;
        static constexpr auto tinu16() -> void;
        static constexpr auto toutu16() -> void;

        static constexpr auto update_flags( const std::size_t reg ) -> void;

        static inline std::array< Instruction, RCNT > registers{};

        using TrapFn = void ( * )();

        constexpr static std::size_t TrapFnMax{ 8 };

        static constexpr std::array< TrapFn, TrapFnMax > TrapFnArray{
            &Registers::tgetc, &Registers::tout,   &Registers::tputs,   &Registers::tin,
            &Registers::thalt, &Registers::tinu16, &Registers::toutu16,
        };
    };

    struct OpsArray {
        using InstructionPtr = void ( * )( const Instruction instr );

        template < typename Self >
        auto&& operator[]( this Self&& self CLANG_LIFETIME_BOUND, std::size_t idx ) {
            return std::forward< decltype( self ) >( self ).op_ex[idx];
        }

      private:
        static constexpr std::array< InstructionPtr, NOPS > op_ex{
            &Registers::br,   &Registers::add, &Registers::ld,  &Registers::st,   &Registers::jsr,
            &Registers::_and, &Registers::ldr, &Registers::str, &Registers::_not, &Registers::ldi,
            &Registers::sti,  &Registers::jmp, &Registers::lea, &Registers::trap,
        };
    };

    // Use decltype to ensure the mask type is always the same type as the argument.
    constexpr auto DR( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x7U };
        constexpr decltype( instr ) Shift{ 0x9U };
        return static_cast< Instruction >(( instr >> Shift )) & Mask;
    }
    constexpr auto SR1( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x7U };
        constexpr decltype( instr ) Shift{ 0x6U };
        return static_cast< Instruction >(( instr >> Shift )) & Mask;
    }

    constexpr auto SR2( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x7U };
        return instr & Mask;
    }
    constexpr auto IMM( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x1FU };
        return instr & Mask;
    }
    constexpr auto SEXTIMM( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Byte{ 0x5U };
        return sext( IMM( instr ), Byte );
    }
    constexpr auto FIMM( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 1U };
        constexpr decltype( instr ) Shift{ 0x5 };
        return static_cast< Instruction >(( instr >> Shift )) & Mask;
    }
    constexpr auto POFF9( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x1FFU };
        constexpr decltype( instr ) Byte{ 0x9U };
        return sext( instr & Mask, Byte );
    }
    constexpr auto FL( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 1U };
        constexpr decltype( instr ) Shift{ 11U };
        return static_cast< Instruction >(( instr >> Shift )) & Mask;
    }
    constexpr auto POFF11( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x7FFU };
        constexpr decltype( instr ) Shift{ 11U };
        return sext( instr & Mask, Shift );
    }
    constexpr auto POFF( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x3FU };
        constexpr decltype( instr ) Byte{ 0x6U };
        return sext( instr & Mask, Byte );
    }
    constexpr auto FCND( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0x7U };
        constexpr decltype( instr ) Shift{ 0x9U };
        return static_cast< Instruction >(( instr >> Shift )) & Mask;
    }

    constexpr auto OPC( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Shift{ 0xC };
        return instr >> Shift;
    }

    constexpr auto TRP( const Instruction instr ) noexcept -> Instruction {
        constexpr decltype( instr ) Mask{ 0xFFU };

        return instr & Mask;
    }

    constexpr auto Registers::add( const Instruction instr ) -> void {
        try {
            registers.at( DR( instr ) ) =
                registers.at( SR1( instr ) ) +
                ( ( FIMM( instr ) != 0x0U ) ? SEXTIMM( instr ) : registers.at( SR2( instr ) ) );

        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in add." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::sub( const Instruction instr ) -> void {
        try {
            registers.at( DR( instr ) ) =
                registers.at( SR1( instr ) ) - registers.at( SR2( instr ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in sub." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::_and( const Instruction instr ) -> void {
        try {
            registers.at( DR( instr ) ) =
                registers.at( SR1( instr ) ) &
                ( ( FIMM( instr ) != 0x0U ) ?         // If the 5th bit is 1:
                      SEXTIMM( instr )
                                            :         // Sign-extend IMM5 and AND it with SR1
                      registers.at( SR2( instr ) ) ); // Otherwise, AND the value of SR2 with SR1
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in _and." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::ld( const Instruction instr ) -> void {
        // Calculate the target address using RPC + 9-bit signed offset
        try {
            registers.at( DR( instr ) ) =
                MainMemory::mem_read( registers.at( RPC ) + POFF9( instr ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in ld fn." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::ldi( const Instruction instr ) -> void {
        // We perform two memory reads (mr):
        // 1. Calculate address from RPC + offset and read the pointer stored there.
        // 2. Read the final data from the pointer's address.
        try {
            registers.at( DR( instr ) ) = MainMemory::mem_read(
                MainMemory::mem_read( registers.at( RPC ) + POFF9( instr ) ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in ldi fn." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::ldr( const Instruction instr ) -> void {
        // Uses a general purpose register (SR1 bits) as the anchor
        try {
            registers.at( DR( instr ) ) =
                MainMemory::mem_read( registers.at( SR1( instr ) ) + POFF( instr ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in ldr fn." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::lea( const Instruction instr ) -> void {
        // Calculate the address (RPC + 9-bit signed offset) and store it in DR
        try {
            registers.at( DR( instr ) ) = registers.at( RPC ) + POFF9( instr );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in lea fn." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::_not( const Instruction instr ) -> void {
        // Perform bitwise NOT on SR1 and store the result in DR
        try {
            registers.at( DR( instr ) ) = ~registers.at( SR1( instr ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in not fn." );
        }

        update_flags( DR( instr ) );
    }

    constexpr auto Registers::st( const Instruction instr ) -> void {
        // Write the value of the source register to the calculated memory address
        try {
            MainMemory::mem_write(
                registers.at( RPC ) + POFF9( instr ), registers.at( DR( instr ) ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in st fn." );
        }
    }

    constexpr auto Registers::str( const Instruction instr ) -> void {
        try {
            MainMemory::mem_write(
                registers.at( SR1( instr ) ) + POFF( instr ), registers.at( DR( instr ) ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in str fn." );
        }
    }

    constexpr auto Registers::sti( const Instruction instr ) -> void {
        // Uses a general-purpose register (BASER) as the anchor point
        // We reuse the SR1 macro to extract the base register index
        try {
            MainMemory::mem_write(
                MainMemory::mem_read( registers.at( RPC ) + POFF9( instr ) ),
                registers.at( DR( instr ) ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in sti fn." );
        }
    }

    constexpr auto Registers::jmp( const Instruction instr ) -> void {
        // Set the Program Counter to the value held in the Base Register
        // We reuse our SR1 macro to extract bits 8-6 which define the BASER
        try {
            registers.at( RPC ) = registers.at( SR1( instr ) );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in jmp fn." );
        }
    }

    constexpr auto Registers::jsr( const Instruction instr ) -> void {
        constexpr decltype( instr ) Zero{ 0x0U };
        // First, save the current PC to R7 so we can return later
        try {
            registers.at( Rg7 ) = registers.at( RPC );

            // Then, update the PC based on bit 11
            registers.at( RPC ) = ( FL( instr ) != Zero )
                                      ? ( registers.at( RPC ) + POFF11( instr ) )
                                      :                             // PC-relative mode
                                      registers.at( SR1( instr ) ); // Register mode (Base Register)
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in jsr fn." );
        }
    }

    constexpr auto Registers::br( const Instruction instr ) -> void {
        constexpr decltype( instr ) Zero{ 0x0U };
        try {
            if ( ( registers.at( RCND ) & FCND( instr ) ) != Zero ) {
                registers.at( RPC ) += POFF9( instr );
            }
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in br fn." );
        }
    }

    constexpr auto Registers::trap( const Instruction instr ) -> void {
        // Invoke the function corresponding to the trap vector minus the offset
        try {
            TrapFnArray.at( TRP( instr ) - trp_offset )();
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in trap fn." );
        }
    }

    constexpr auto Registers::update_flags( const std::size_t reg ) -> void {
        constexpr Instruction Byte{ 0xF };
        constexpr std::uint16_t Positive{ 1U << 0U };
        constexpr std::uint16_t Zero{ 1U << 1U };
        constexpr std::uint16_t Negative{ 1U << 2U };
        try {
            if ( registers.at( reg ) == 0 ) {
                registers.at( RCND ) = Zero;
            } else if ( ( registers.at( reg ) >> Byte ) != 0 ) {
                registers.at( RCND ) = Negative;
            } else {
                registers.at( RCND ) = Positive;
            }
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in jsr fn." );
        }
    }

    constexpr auto Registers::tgetc() -> void {
        try {
            registers.at( Rg0 ) = std::getchar();
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in jsr fn." );
        }
    }

    constexpr auto Registers::tout() -> void {
        std::fprintf( stdout, "%c", static_cast< char >( registers[Rg0] ) );
    }

    constexpr auto Registers::tputs() -> void {
        std::uint16_t* ptr{};
        try {
            ptr = MainMemory::data() + registers.at( Rg0 );
        } catch ( const std::out_of_range& err ) {
            assert( false && "Out of bounds access in jsr fn." );
        }

        while ( ( *ptr ) != 0U ) {
            std::fprintf( stdout, "%c", static_cast< char >( *ptr ) );
            ptr++;
        }
    }

    constexpr auto Registers::tin() -> void {
        constexpr int ErrorCode{ -1 };
        registers[Rg0] = std::getchar();
        if ( const int ret{ std::fprintf( stdout, "%c", registers[Rg0] ) }; ret == ErrorCode ) {
            assert( false && "Failure in registers tin function." );
        }
    }

    constexpr auto Registers::thalt() -> void {
    }

    constexpr auto Registers::tinu16() -> void {
        constexpr int ErrorCode{ -1 };
        if ( const int ret{ std::fscanf( stdin, "%hu", &registers[Rg0] ) }; ret == ErrorCode ) {
            assert( false && "Failure in registers tinu16 function." );
        }
    }

    constexpr auto Registers::toutu16() -> void {
        std::fprintf( stdout, "%hu\n", registers[Rg0] );
    }

    struct RegisterDebug {
        static auto fprintf_binary( std::ostream& ostream, Instruction num ) -> void;
        static auto fprintf_inst( std::ostream& ostream, Instruction instr ) -> void;
        static auto fprintf_mem(
            std::ostream& ostream, Instruction* mem, Instruction src, Instruction dst ) -> void;
        static auto fprintf_mem_nonzero( std::ostream& ostream, Instruction* mem, uint32_t stop )
            -> void;
        static auto fprintf_reg( std::ostream& ostream, Instruction* reg, int idx ) -> void;
        static auto fprintf_reg_all( std::ostream& ostream, Instruction* reg, int size ) -> void;
    };
} // namespace RegisterVM
