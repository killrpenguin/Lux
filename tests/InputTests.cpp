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

    SECTION( "Compile Simple Expression" ) {
        Compiler compiler{};

        Chunk simple_expression{};
        REQUIRE( compiler.Compile( "10 + 20", simple_expression ) );

        // Debug::DisassembleChunk( simple_expression, "Simple expression" );

        Debug::OpCodeAndLine first_constant{ Debug::GetOpcode( simple_expression, 0 ) };

        REQUIRE( first_constant.has_value() );
        CHECK( first_constant->first == OpCode::OP_CONSTANT );
        CHECK( first_constant->second == 1 );

        Debug::OpCodeAndLine second_constant{ Debug::GetOpcode( simple_expression, 2 ) };

        REQUIRE( second_constant.has_value() );
        CHECK( second_constant->first == OpCode::OP_CONSTANT );
        CHECK( second_constant->second == 1 );

        Debug::OpCodeAndLine op_add{ Debug::GetOpcode( simple_expression, 4 ) };

        REQUIRE( op_add.has_value() );
        CHECK( op_add->first == OpCode::OP_ADD );
        CHECK( op_add->second == 1 );

        Debug::OpCodeAndLine op_return{ Debug::GetOpcode( simple_expression, 5 ) };

        REQUIRE( op_return.has_value() );
        CHECK( op_return->first == OpCode::OP_RETURN );
        CHECK( op_return->second == 1 );
    }

    SECTION( "Compile Complex Expression" ) {
        // using namespace std::string_literals; // Required for the 's' suffix

        Compiler compiler{};

        Chunk complex_expression{};

        REQUIRE( compiler.Compile( "(53 * (31 - 13)) + -10", complex_expression ) );
        REQUIRE( compiler.Compile( "\n23 / 3", complex_expression ) );

        // Debug::DisassembleChunk( complex_expression, "Complex Expression" );

        Debug::OpCodeAndLine first_constant{ Debug::GetOpcode( complex_expression, 0 ) };
        REQUIRE( first_constant.has_value() );
        CHECK( first_constant->first == OpCode::OP_CONSTANT );
        CHECK( first_constant->second == 1 );

        Debug::OpCodeAndLine second_constant{ Debug::GetOpcode( complex_expression, 2 ) };
        REQUIRE( second_constant.has_value() );
        CHECK( second_constant->first == OpCode::OP_CONSTANT );
        CHECK( second_constant->second == 1 );

        Debug::OpCodeAndLine third_constant{ Debug::GetOpcode( complex_expression, 4 ) };
        REQUIRE( third_constant.has_value() );
        CHECK( third_constant->first == OpCode::OP_CONSTANT );
        CHECK( third_constant->second == 1 );

        Debug::OpCodeAndLine op_subtract{ Debug::GetOpcode( complex_expression, 6 ) };
        REQUIRE( op_subtract.has_value() );
        CHECK( op_subtract->first == OpCode::OP_SUBTRACT );
        CHECK( op_subtract->second == 1 );

        Debug::OpCodeAndLine op_multiply{ Debug::GetOpcode( complex_expression, 7 ) };
        REQUIRE( op_multiply.has_value() );
        CHECK( op_multiply->first == OpCode::OP_MULTIPLY );
        CHECK( op_multiply->second == 1 );

        Debug::OpCodeAndLine fourth_constant{ Debug::GetOpcode( complex_expression, 8 ) };
        REQUIRE( fourth_constant.has_value() );
        CHECK( fourth_constant->first == OpCode::OP_CONSTANT );
        CHECK( fourth_constant->second == 1 );

        Debug::OpCodeAndLine op_negate{ Debug::GetOpcode( complex_expression, 10 ) };
        REQUIRE( op_negate.has_value() );
        CHECK( op_negate->first == OpCode::OP_NEGATE );
        CHECK( op_negate->second == 1 );

        Debug::OpCodeAndLine op_add{ Debug::GetOpcode( complex_expression, 11 ) };
        REQUIRE( op_add.has_value() );
        CHECK( op_add->first == OpCode::OP_ADD );
        CHECK( op_add->second == 1 );

        Debug::OpCodeAndLine op_return_ln_one{ Debug::GetOpcode( complex_expression, 12 ) };
        REQUIRE( op_return_ln_one.has_value() );
        CHECK( op_return_ln_one->first == OpCode::OP_RETURN );
        CHECK( op_return_ln_one->second == 1 );

        Debug::OpCodeAndLine fifth_constant{ Debug::GetOpcode( complex_expression, 13 ) };
        REQUIRE( fifth_constant.has_value() );
        CHECK( fifth_constant->first == OpCode::OP_CONSTANT );
        CHECK( fifth_constant->second == 2 );

        Debug::OpCodeAndLine sixth_constant{ Debug::GetOpcode( complex_expression, 15 ) };
        REQUIRE( sixth_constant.has_value() );
        CHECK( sixth_constant->first == OpCode::OP_CONSTANT );
        CHECK( sixth_constant->second == 2 );

        Debug::OpCodeAndLine op_divide{ Debug::GetOpcode( complex_expression, 17 ) };
        REQUIRE( op_divide.has_value() );
        CHECK( op_divide->first == OpCode::OP_DIVIDE );
        CHECK( op_divide->second == 2 );

        Debug::OpCodeAndLine op_return_ln_two{ Debug::GetOpcode( complex_expression, 18 ) };
        REQUIRE( op_return_ln_two.has_value() );
        CHECK( op_return_ln_two->first == OpCode::OP_RETURN );
        CHECK( op_return_ln_two->second == 2 );
    }

    SECTION( "Compile Concatenated String." ) {
        Compiler compiler{};
        Chunk expression{};

        const std::string source{ "\"This is a \" + \"string \" + \"test.\"" };

        const bool compiled_successfully{ compiler.Compile( source, expression ) };

        REQUIRE( compiled_successfully );

        // Debug::DisassembleChunk( expression, "Concat String" );

        Debug::OpCodeAndLine start_of_str{ Debug::GetOpcode( expression, 0 ) };
        REQUIRE( start_of_str.has_value() );
        CHECK( start_of_str->first == OpCode::OP_CONSTANT );
        CHECK( start_of_str->second == 1 );

        Debug::OpCodeAndLine middle_of_str{ Debug::GetOpcode( expression, 2 ) };
        REQUIRE( middle_of_str.has_value() );
        CHECK( middle_of_str->first == OpCode::OP_CONSTANT );
        CHECK( middle_of_str->second == 1 );

        Debug::OpCodeAndLine concat_start_to_middle{ Debug::GetOpcode( expression, 4 ) };
        REQUIRE( concat_start_to_middle.has_value() );
        CHECK( concat_start_to_middle->first == OpCode::OP_ADD );
        CHECK( concat_start_to_middle->second == 1 );

        Debug::OpCodeAndLine end_of_str{ Debug::GetOpcode( expression, 5 ) };
        REQUIRE( end_of_str.has_value() );
        CHECK( end_of_str->first == OpCode::OP_CONSTANT );
        CHECK( end_of_str->second == 1 );

        Debug::OpCodeAndLine concat_end_to_middle{ Debug::GetOpcode( expression, 7 ) };
        REQUIRE( concat_end_to_middle.has_value() );
        CHECK( concat_end_to_middle->first == OpCode::OP_ADD );
        CHECK( concat_end_to_middle->second == 1 );

        Debug::OpCodeAndLine op_return{ Debug::GetOpcode( expression, 8 ) };
        REQUIRE( op_return.has_value() );
        CHECK( op_return->first == OpCode::OP_RETURN );
        CHECK( op_return->second == 1 );
    }

    SECTION( "Compile Interpolated string." ) {
        Compiler compiler{};
        Chunk expression{};

        // const std::string source{ "\"Test ${\"compiler\"} string.\"" };
        const std::string source{ "\"Test ${\"compiler\"} string.\"" };

        const bool compiled_successfully{ compiler.Compile( source, expression ) };

        REQUIRE( compiled_successfully );

        Debug::DisassembleChunk( expression, "Interpolation String" );

        Debug::OpCodeAndLine start_of_str{ Debug::GetOpcode( expression, 0 ) };
        REQUIRE( start_of_str.has_value() );
        CHECK( start_of_str->first == OpCode::OP_CONSTANT );
        CHECK( start_of_str->second == 1 );

        Debug::OpCodeAndLine interpolated_str{ Debug::GetOpcode( expression, 2 ) };
        REQUIRE( interpolated_str.has_value() );
        CHECK( interpolated_str->first == OpCode::OP_CONSTANT );
        CHECK( interpolated_str->second == 1 );

        Debug::OpCodeAndLine pre_add_interpolated_expr{ Debug::GetOpcode( expression, 4 ) };
        REQUIRE( pre_add_interpolated_expr.has_value() );
        CHECK( pre_add_interpolated_expr->first == OpCode::OP_ADD );
        CHECK( pre_add_interpolated_expr->second == 1 );

        Debug::OpCodeAndLine end_of_str{ Debug::GetOpcode( expression, 5 ) };
        REQUIRE( end_of_str.has_value() );
        CHECK( end_of_str->first == OpCode::OP_CONSTANT );
        CHECK( end_of_str->second == 1 );

        Debug::OpCodeAndLine post_add_interpolated_expr{ Debug::GetOpcode( expression, 7 ) };
        REQUIRE( post_add_interpolated_expr.has_value() );
        CHECK( post_add_interpolated_expr->first == OpCode::OP_ADD );
        CHECK( post_add_interpolated_expr->second == 1 );

        Debug::OpCodeAndLine op_return{ Debug::GetOpcode( expression, 8 ) };
        REQUIRE( op_return.has_value() );
        CHECK( op_return->first == OpCode::OP_RETURN );
        CHECK( op_return->second == 1 );
    }

    SECTION( "Compile LuxArray" ) {
        Compiler compiler{};
        Chunk complex_expression{};

        const std::string vec_source{ "[1, 'A', \"Apple\",[],1.0]" };

        // REQUIRE( compiler.Compile( vec_source, complex_expression ) );
    }

    SECTION( "Compile String Expression" ) {
        Compiler compiler{};
        Chunk complex_expression{};

        const std::string vec_source{ "I expect 6 to equal ${3+3}" };

        // REQUIRE( compiler.Compile( vec_source, complex_expression ) );
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
