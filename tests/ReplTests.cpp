#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "Repl.hpp"

using namespace LuxLibrary;
using namespace LuxLibrary::detail;

// NOLINTBEGIN
TEST_CASE( "Repl Tests", "[main]" ) {
    SECTION( "Repl Basics" ) {
        Repl repl{};
    }
} // namespace LL::detail
// NOLINTEND
