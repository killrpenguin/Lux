#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "Compiler.hpp"
#include "Debug.hpp"
#include "OpCode.hpp"
#include "Stack.hpp"

#include <optional>
#include <sstream>
#include <string>
#include <string_view>

using namespace LuxLibrary;
using namespace LuxLibrary::detail;

// NOLINTBEGIN
TEST_CASE( "Crafting Interpreters cLox Section", "[main]" ) {
    SECTION( "Stack Class Basics" ) {
        LuxLibrary::Stack< int, 3 > stack{};

        CHECK( stack.Empty() );
        CHECK( stack.Size() == 0 );
        CHECK( stack.Capacity() == 3 );

        stack.Push( 10 );
        stack.Push( 20 );
        stack.Push( 30 );

        CHECK( *stack.Pop() == 30 );
        CHECK( *stack.Pop() == 20 );
        CHECK( *stack.Pop() == 10 );
        CHECK( stack.Pop() == std::nullopt );
    }



    SECTION( "Debug Output test from First Chapter" ) {
        Chunk chunk{};

        constexpr unsigned int Line{ 123 };

        const Value constant_value{ 1.2 };

        const std::uint8_t constant{ chunk.AddConstant( constant_value ) };

        chunk.WriteChunk( OpCode::OP_CONSTANT, Line );

        chunk.WriteChunk( constant, Line );

        const Value second_const_value{ 3.4 };

        const std::uint8_t second_const{ chunk.AddConstant( second_const_value ) };

        chunk.WriteChunk( OpCode::OP_CONSTANT, Line );

        chunk.WriteChunk( second_const, Line );

        chunk.WriteChunk( OpCode::OP_ADD, Line );

        const Value third_const_value{ 5.6 };

        const std::uint8_t third_const{ chunk.AddConstant( third_const_value ) };

        chunk.WriteChunk( OpCode::OP_CONSTANT, Line );

        chunk.WriteChunk( third_const, Line );

        chunk.WriteChunk( OpCode::OP_DIVIDE, Line );

        chunk.WriteChunk( OpCode::OP_NEGATE, Line );

        chunk.WriteChunk( OpCode::OP_RETURN, Line );

        std::stringstream captured_output{};

        Debug::DisassembleChunk( chunk, "test chunk", captured_output );

        std::string output_line{};

        constexpr std::string_view header{ "== test chunk ==" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == header );

        constexpr std::string_view first_constant{ "0000 0123 OP_CONSTANT         0 1.2" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == first_constant );

        constexpr std::string_view second_constant{ "0002    | OP_CONSTANT         1 3.4" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == second_constant );

        constexpr std::string_view op_add{ "0004    | OP_ADD" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == op_add );

        constexpr std::string_view third_constant{ "0005    | OP_CONSTANT         2 5.6" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == third_constant );

        constexpr std::string_view op_divide{ "0007    | OP_DIVIDE" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == op_divide );

        constexpr std::string_view op_negate{ "0008    | OP_NEGATE" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == op_negate );

        constexpr std::string_view op_return{ "0009    | OP_RETURN" };
        std::getline( captured_output, output_line, '\n' );
        REQUIRE( output_line == op_return );
    }
} // namespace LL::detail
// NOLINTEND
