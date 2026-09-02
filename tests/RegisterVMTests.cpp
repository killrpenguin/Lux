#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "RegisterVM.hpp"

using namespace RegisterVM;
// NOLINTBEGIN
struct InstrMkr {
    constexpr static auto make_ld() -> std::uint16_t {
        std::uint16_t opcode{ 0 };
        opcode  = ( opcode | ( 0x2U & 0x0FU ) << 12 );
        opcode |= ( 0x0U << 9 ); // set destination reg to Rg1
        return ( opcode & ~0x1FF ) | ( 0x3004 & 0x1FF );
    }
};

TEST_CASE( "RegisterVM Test", "[main]" ) {
    SECTION( "Exploration Tests" ) {
        Registers reg{};
        OpsArray ops{};
        const std::uint16_t ld_instr{ InstrMkr::make_ld() };
        const std::array< std::uint16_t, 4 > program{
            ld_instr,
            0x1220, //  0001 0010 0010 0000  ADD R1, R0, x0   ; copy R0 to R1 (R1 = R0 + 0)
            0x1240, //  0001 0010 0100 0000  ADD R1, R1, R0   ; R1 = R1 + R0
            0x1060, //  0001 0000 0110 0000  ADD R0, R1, x0   ; copy result back to R0
        };

        std::uint16_t offset = PC_START; // 3000 = 12288   300F 12303

        for ( const auto& val : program ) {
            MainMemory::mem_write( offset, val );
            offset += 1;
        }

        MainMemory::mem_write( offset + 1, 5U );

        REQUIRE( MainMemory::mem_read( PC_START ) == ld_instr );

        // Initialize the RPC to the program start address
        reg[RPC] = PC_START;

        // reg[Rg0] = 5; // emulates TRAP tinu16 0xF026
        {
            const std::uint16_t instr{ MainMemory::mem_read( reg[RPC]++ ) };
            ops[OPC( instr )]( instr );
        }

        CHECK( reg[Rg0] == 5 );
        {
            const std::uint16_t instr{ MainMemory::mem_read( reg[RPC]++ ) };
            ops[OPC( instr )]( instr );
        }

        CHECK( reg[Rg1] == 5 );

        reg[Rg0] = 3;
        {
            const std::uint16_t instr{ MainMemory::mem_read( reg[RPC]++ ) };
            ops[OPC( instr )]( instr );
        }

        CHECK( reg[Rg1] == 8 );
        {
            const std::uint16_t instr{ MainMemory::mem_read( reg[RPC]++ ) };
            ops[OPC( instr )]( instr );
        }
        CHECK( reg[Rg0] == 8 );
    }

} // namespace LL::detail
// NOLINTEND
