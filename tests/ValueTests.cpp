#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "Compiler.hpp"
#include "Value.hpp"

#include <string>

using namespace LuxLibrary;
using namespace LuxLibrary::detail;

// NOLINTBEGIN
TEST_CASE( "Value Class Testing", "[main]" ) {
    SECTION( "Value Class Basics" ) {
        const Value nil_type{};
        REQUIRE( nil_type.IsNil() );

        const Value bool_type{ true };
        REQUIRE( bool_type.IsBool() );
        REQUIRE( bool_type.AsBool() );
        REQUIRE( !bool_type.IsObject() );

        const Value float_type{ 2.0 };
        REQUIRE( float_type.IsDouble() );
        REQUIRE( float_type.AsDouble() == 2.0 );
        REQUIRE( !float_type.IsObject() );

        const Value int_type{ 2 };
        REQUIRE( int_type.IsInteger() );
        REQUIRE( int_type.AsInteger() == 2 );
        REQUIRE( !int_type.IsObject() );

        const Value char_type{ 'A' };
        REQUIRE( char_type.IsChar() );
        REQUIRE( char_type.AsChar() == 'A' );
        REQUIRE( !char_type.IsObject() );

        const std::string input_text{ "Test" };
        const Value string_type{ input_text };
        REQUIRE( string_type.IsString() );
        REQUIRE( string_type.AsString() == input_text );
        REQUIRE( string_type.IsObject() );

        detail::LuxArray array_values{ 1, 2.0, true };
        const Value array_type{ array_values };
        REQUIRE( array_type.IsLuxArray() );
        REQUIRE( array_type.AsLuxArray() == array_values );
        REQUIRE( array_type.IsObject() );

        detail::LuxMap map_values{
            { "apple1", 1.0 },
            { "apple2", 1 },
            { "apple3", true },
            { "apple3", "Test123" },
        };
  
        const Value map_type{ map_values };
        REQUIRE( map_type.IsLuxMap() );
        REQUIRE( map_type.AsLuxMap() == map_values );
        REQUIRE( map_type.IsObject() );
    }

    SECTION( "Compile LuxArray" ) {
        Compiler compiler{};

        Chunk lux_array{};
    }
} // namespace LL::detail
// NOLINTEND
