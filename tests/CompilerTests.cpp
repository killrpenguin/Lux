#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "Compiler.hpp"
#include "Debug.hpp"
#include "OpCode.hpp"

using namespace LuxLibrary;
using namespace LuxLibrary::detail;

// NOLINTBEGIN
TEST_CASE( "Compiler Testing", "[main]" ) {
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

    SECTION( "Compile Simple Interpolated String." ) {
        Compiler compiler{};
        Chunk expression{};

        const std::string source{ "\"Compile ${\"test\"} string.\"" };

        const bool compiled_successfully{ compiler.Compile( source, expression ) };

        REQUIRE( compiled_successfully );

        // Debug::DisassembleChunk( expression, "Interpolation String" );

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

    SECTION( "Compile Complex Interpolated string." ) {
        Compiler compiler{};
        Chunk expression{};

        const std::string source{ "\"Comp ${\"cplx\"} tst ${10 - 5}.\"" };

        const bool compiled_successfully{ compiler.Compile( source, expression ) };

        // Debug::DisassembleChunk( expression, "Complex Interpolation String" );

        REQUIRE( compiled_successfully );

        Debug::OpCodeAndLine first_lhr_str{ Debug::GetOpcode( expression, 0 ) };
        REQUIRE( first_lhr_str.has_value() );
        CHECK( first_lhr_str->first == OpCode::OP_CONSTANT );
        CHECK( first_lhr_str->second == 1 );

        Debug::OpCodeAndLine first_interpolated_str{ Debug::GetOpcode( expression, 2 ) };
        REQUIRE( first_interpolated_str.has_value() );
        CHECK( first_interpolated_str->first == OpCode::OP_CONSTANT );
        CHECK( first_interpolated_str->second == 1 );

        Debug::OpCodeAndLine first_add{ Debug::GetOpcode( expression, 4 ) };
        REQUIRE( first_add.has_value() );
        CHECK( first_add->first == OpCode::OP_ADD );
        CHECK( first_add->second == 1 );

        Debug::OpCodeAndLine third_interpolated_str{ Debug::GetOpcode( expression, 5 ) };
        REQUIRE( third_interpolated_str.has_value() );
        CHECK( third_interpolated_str->first == OpCode::OP_CONSTANT );
        CHECK( third_interpolated_str->second == 1 );

        Debug::OpCodeAndLine first_constant{ Debug::GetOpcode( expression, 7 ) };
        REQUIRE( first_constant.has_value() );
        CHECK( first_constant->first == OpCode::OP_CONSTANT );
        CHECK( first_constant->second == 1 );

        Debug::OpCodeAndLine second_constant{ Debug::GetOpcode( expression, 9 ) };
        REQUIRE( second_constant.has_value() );
        CHECK( second_constant->first == OpCode::OP_CONSTANT );
        CHECK( second_constant->second == 1 );

        Debug::OpCodeAndLine op_add{ Debug::GetOpcode( expression, 11 ) };
        REQUIRE( op_add.has_value() );
        CHECK( op_add->first == OpCode::OP_SUBTRACT );
        CHECK( op_add->second == 1 );

        Debug::OpCodeAndLine _third_add_interpolated_expr{ Debug::GetOpcode( expression, 12 ) };
        REQUIRE( _third_add_interpolated_expr.has_value() );
        CHECK( _third_add_interpolated_expr->first == OpCode::OP_ADD );
        CHECK( _third_add_interpolated_expr->second == 1 );

        Debug::OpCodeAndLine last_str{ Debug::GetOpcode( expression, 13 ) };
        REQUIRE( last_str.has_value() );
        CHECK( last_str->first == OpCode::OP_CONSTANT );
        CHECK( last_str->second == 1 );

        Debug::OpCodeAndLine third_add_interpolated_expr{ Debug::GetOpcode( expression, 15 ) };
        REQUIRE( third_add_interpolated_expr.has_value() );
        CHECK( third_add_interpolated_expr->first == OpCode::OP_ADD );
        CHECK( third_add_interpolated_expr->second == 1 );

        Debug::OpCodeAndLine last_add{ Debug::GetOpcode( expression, 16 ) };
        REQUIRE( last_add.has_value() );
        CHECK( last_add->first == OpCode::OP_ADD );
        CHECK( last_add->second == 1 );

        Debug::OpCodeAndLine op_return{ Debug::GetOpcode( expression, 17 ) };
        REQUIRE( op_return.has_value() );
        CHECK( op_return->first == OpCode::OP_RETURN );
        CHECK( op_return->second == 1 );
    }

    SECTION( "Compile Simple LuxArray" ) {
        Compiler compiler{};
        Chunk expression{};

        const std::string source{ "[1, 'A', \"Apple\", 1.5]" };

        const bool successful{ compiler.Compile( source, expression ) };

        REQUIRE( successful );

        Debug::OpCodeAndLine val{ Debug::GetOpcode( expression, 0 ) };

        // Debug::DisassembleChunk( expression, "Basic Array" );

        REQUIRE( val.has_value() );
        CHECK( val->first == OpCode::OP_CONSTANT );
        CHECK( val->second == 1 );

        auto compiled_array{ expression.GetConstant( 0 ) };

        REQUIRE( compiled_array.IsLuxArray() );
    }

    SECTION( "Compile Simple LuxMap" ) {
        Compiler compiler{};
        Chunk expression{};

        const std::string source{
            "{\"Number\": 1, \"Char\": 'A', \"String\": \"Apple\", \"Double\" :1.5}"
        };

        const bool successful{ compiler.Compile( source, expression ) };

        REQUIRE( successful );

        // Debug::DisassembleChunk( expression, "Basic Map" );
    }
}

// NOLINTEND
