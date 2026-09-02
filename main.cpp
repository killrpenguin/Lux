#include "Debug.hpp"
#include "VirtualMachine.hpp"

#include <cstdlib>
#include <iostream>

namespace LL = LuxLibrary;
using LLD    = LL::Debug;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#pragma clang unsafe_buffer_usage begin
auto main( int argc, const char* argv[] ) -> int {
    try {
        LL::VirtualMachine virtual_machine{};

        if ( argc == 1 ) {
            virtual_machine.RunRepl();
        } else if ( argc == 2 ) {
            virtual_machine.RunFile( argv[1] );
        } else {
            std::cerr << "Invalid argument passed to Lux.";
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } catch ( const std::exception& err ) {
        std::cerr << "Caught an unknown exception. \n\t" << err.what();
        return EXIT_FAILURE;
    }
}
#pragma clang unsafe_buffer_usage end
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
