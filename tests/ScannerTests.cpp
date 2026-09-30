#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "Scanner.hpp"
#include "TokenVariant.hpp"

#include <string>

using namespace LuxLibrary;
using namespace LuxLibrary::detail;

// NOLINTBEGIN
TEST_CASE( "Scanner Testing", "[main]" ) {
    SECTION( "Scanner Class Tokens" ) {
        const std::string source{ "();,.-+/*=!<>!= //comment\n==\t<=\r>=\"string\"5.9" };

        Scanner scanner{ source };

        REQUIRE( scanner.Line() == 1 );

        const Token lhs{ scanner.NewToken( TokenType::LEFT_PAREN ) };
        const Token rhs{ scanner.NewToken( TokenType::LEFT_PAREN ) };

        REQUIRE( lhs.Is( TokenType::LEFT_PAREN ) );

        REQUIRE( TokenType::LEFT_PAREN == TokenType::LEFT_PAREN );
        REQUIRE( TokenType::LEFT_PAREN != TokenType::RIGHT_PAREN );

        REQUIRE( lhs == rhs );
        REQUIRE( rhs == lhs );

        const Token left_paren{ scanner.NextToken() };
        CHECK( left_paren == scanner.NewToken( TokenType::LEFT_PAREN ) );

        const Token right_paren{ scanner.NextToken() };
        CHECK( right_paren == scanner.NewToken( TokenType::RIGHT_PAREN ) );

        const Token semicolon{ scanner.NextToken() };
        CHECK( semicolon == scanner.NewToken( TokenType::SEMICOLON ) );

        const Token comma{ scanner.NextToken() };
        CHECK( comma == scanner.NewToken( TokenType::COMMA ) );

        const Token dot{ scanner.NextToken() };
        CHECK( dot == scanner.NewToken( TokenType::DOT ) );

        const Token minus{ scanner.NextToken() };
        CHECK( minus == scanner.NewToken( TokenType::MINUS ) );

        const Token plus{ scanner.NextToken() };
        CHECK( plus == scanner.NewToken( TokenType::PLUS ) );

        const Token slash{ scanner.NextToken() };
        CHECK( slash == scanner.NewToken( TokenType::SLASH ) );

        const Token star{ scanner.NextToken() };
        CHECK( star == scanner.NewToken( TokenType::STAR ) );

        const Token equal{ scanner.NextToken() };
        CHECK( equal == scanner.NewToken( TokenType::EQUAL ) );

        const Token bang{ scanner.NextToken() };
        CHECK( bang == scanner.NewToken( TokenType::BANG ) );

        const Token less{ scanner.NextToken() };
        CHECK( less == scanner.NewToken( TokenType::LESS ) );

        const Token greater{ scanner.NextToken() };
        CHECK( greater == scanner.NewToken( TokenType::GREATER ) );

        const Token bang_equal{ scanner.NextToken() };
        CHECK( bang_equal == scanner.NewToken( TokenType::BANG_EQUAL ) );

        const Token equal_equal{ scanner.NextToken() };
        CHECK( equal_equal == scanner.NewToken( TokenType::EQUAL_EQUAL ) );

        REQUIRE( scanner.Line() == 2 );

        const Token less_equal{ scanner.NextToken() };
        CHECK( less_equal == scanner.NewToken( TokenType::LESS_EQUAL ) );

        const Token greater_equal{ scanner.NextToken() };
        CHECK( greater_equal == scanner.NewToken( TokenType::GREATER_EQUAL ) );

        const Token string_token{ scanner.NextToken() };
        CHECK( string_token == scanner.NewToken( TokenType::STRING ) );

        const Token number_token{ scanner.NextToken() };
        CHECK( number_token == scanner.NewToken( TokenType::DOUBLE ) );

        {
            const Token eof_token{ scanner.NextToken() };
            CHECK( eof_token == scanner.NewToken( TokenType::END_OF_FILE ) );
        }

        const std::string keywords{
            "and class else if nil or print return super var while this true false for fun apple"
        };

        scanner.NewSource( keywords );

        const Token and_token{ scanner.NextToken() };
        CHECK( and_token == scanner.NewToken( TokenType::AND ) );

        const Token class_token{ scanner.NextToken() };
        CHECK( class_token == scanner.NewToken( TokenType::CLASS ) );

        const Token else_token{ scanner.NextToken() };
        CHECK( else_token == scanner.NewToken( TokenType::ELSE ) );

        const Token if_token{ scanner.NextToken() };
        CHECK( if_token == scanner.NewToken( TokenType::IF ) );

        const Token nil_token{ scanner.NextToken() };
        CHECK( nil_token == scanner.NewToken( TokenType::NIL ) );

        const Token or_token{ scanner.NextToken() };
        CHECK( or_token == scanner.NewToken( TokenType::OR ) );

        const Token print_token{ scanner.NextToken() };
        CHECK( print_token == scanner.NewToken( TokenType::PRINT ) );

        const Token return_token{ scanner.NextToken() };
        CHECK( return_token == scanner.NewToken( TokenType::RETURN ) );

        const Token super_token{ scanner.NextToken() };
        CHECK( super_token == scanner.NewToken( TokenType::SUPER ) );

        const Token var_token{ scanner.NextToken() };
        CHECK( var_token == scanner.NewToken( TokenType::VAR ) );

        const Token while_token{ scanner.NextToken() };
        CHECK( while_token == scanner.NewToken( TokenType::WHILE ) );

        const Token this_token{ scanner.NextToken() };
        CHECK( this_token == scanner.NewToken( TokenType::THIS ) );

        const Token true_token{ scanner.NextToken() };
        CHECK( true_token == scanner.NewToken( TokenType::TRUE ) );

        const Token false_token{ scanner.NextToken() };
        CHECK( false_token == scanner.NewToken( TokenType::FALSE ) );

        const Token for_token{ scanner.NextToken() };
        CHECK( for_token == scanner.NewToken( TokenType::FOR ) );

        const Token fun_token{ scanner.NextToken() };
        CHECK( fun_token == scanner.NewToken( TokenType::FUN ) );

        const Token identifier_token{ scanner.NextToken() };

        CHECK( identifier_token == scanner.NewToken( TokenType::IDENTIFIER ) );

        {
            const Token eof_token{ scanner.NextToken() };
            CHECK( eof_token == scanner.NewToken( TokenType::END_OF_FILE ) );
        }
    }
    SECTION( "Test Primitive types." ) {
        const std::string double_source{ "5.8" };

        Scanner scanner{ double_source };

        const Token double_token{ scanner.NextToken() };
        CHECK( double_token == scanner.NewToken( TokenType::DOUBLE ) );

        const std::string integer_source{ "58" };
        scanner.NewSource( integer_source );

        const Token number_token{ scanner.NextToken() };
        CHECK( number_token == scanner.NewToken( TokenType::INTEGER ) );

        const std::string char_source{ "\'Z\'" };
        scanner.NewSource( char_source );

        const Token char_token{ scanner.NextToken() };
        CHECK( char_token == scanner.NewToken( TokenType::CHAR ) );
    }

    SECTION( "Test Vector Value scanning." ) {
        // Inconsistent spacing in vec_source is deliberate. It shouldn't matter.
        const std::string vec_source{ "[1, 'A', \"Apple\",[],1.0]" };
        Scanner scanner{ vec_source };

        const Token array_token{ scanner.NextToken() };
        CHECK( array_token == scanner.NewToken( TokenType::LUX_VECTOR_OPEN ) );

        const Token number_token{ scanner.NextToken() };
        CHECK( number_token == scanner.NewToken( TokenType::INTEGER ) );

        const Token first_comma{ scanner.NextToken() };
        CHECK( first_comma == scanner.NewToken( TokenType::COMMA ) );

        const Token char_token{ scanner.NextToken() };
        CHECK( char_token == scanner.NewToken( TokenType::CHAR ) );

        const Token second_comma{ scanner.NextToken() };
        CHECK( second_comma == scanner.NewToken( TokenType::COMMA ) );

        const Token string_token{ scanner.NextToken() };
        CHECK( string_token == scanner.NewToken( TokenType::STRING ) );

        const Token third_comma{ scanner.NextToken() };
        CHECK( third_comma == scanner.NewToken( TokenType::COMMA ) );

        const Token nested_vec_start{ scanner.NextToken() };
        CHECK( nested_vec_start == scanner.NewToken( TokenType::LUX_VECTOR_OPEN ) );

        const Token nested_vec_end{ scanner.NextToken() };
        CHECK( nested_vec_end == scanner.NewToken( TokenType::LUX_VECTOR_CLOSE ) );

        const Token fourth_comma{ scanner.NextToken() };
        CHECK( fourth_comma == scanner.NewToken( TokenType::COMMA ) );

        const Token double_token{ scanner.NextToken() };
        CHECK( double_token == scanner.NewToken( TokenType::DOUBLE ) );
    }

    SECTION( "Test Map value scanning." ) {
        const std::string map_source{ "{}" };

        Scanner scanner{ map_source };

        const Token map_open{ scanner.NextToken() };
        CHECK( map_open == scanner.NewToken( TokenType::LUX_MAP_OPEN ) );

        const Token map_close{ scanner.NextToken() };
        CHECK( map_close == scanner.NewToken( TokenType::LUX_MAP_CLOSE ) );
    }
}
// NOLINTEND
