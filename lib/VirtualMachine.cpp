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

#include "VirtualMachine.hpp"
#include "Debug.hpp"
#include "Value.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <expected>

namespace LuxLibrary {

    namespace ranges = std::ranges;

    auto VirtualMachine::RunRepl( std::istream& input, std::ostream& output ) noexcept -> void {
        repl.Initialize( input );

        repl.ResetScreen( output );

        output << "> " << std::flush;

        while ( repl.Running() ) {
            expressions.emplace_back( repl.Read( input, output ) );

            results.emplace_back( Interpret( expressions.back() ) );

            repl.Print( { expressions }, { results }, output );
        }

        repl.ResetScreen( output );
    }

    auto VirtualMachine::RunFile( const fs::path& file_name ) noexcept -> void {
        const std::string source_code{ ReadFile( file_name ) };

        if ( source_code.empty() ) { return; }

        const Result< Value > result{ Interpret( source_code ) };

        if ( result ) {
            try {
                fmt::println( "{}", *result );
            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n'
                          << "Numeric code: " << err.code().value() << '\n';
            }
        } else {
            try {
                fmt::println( "{}", result.error() );
            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n'
                          << "Numeric code: " << err.code().value() << '\n';
            }
        }
    }

    // NOLINTNEXTLINE
    auto VirtualMachine::Interpret( const std::string& source ) noexcept -> Result< Value > {
        Chunk chunk{};

        if ( source.empty() ) { return {}; }

        if ( const auto result = compiler.Compile( source, chunk ); !result.has_value() ) {
            return std::unexpected( result.error() );
        }

        current_chunk = &chunk;
        ip            = chunk.begin();

        return Run();
    }

    auto VirtualMachine::AdvanceInstructionPtr() noexcept -> void {
        ranges::advance( ip, 1 );
    }

    auto VirtualMachine::InstructionPtrValue() noexcept -> Instruction {
        const Instruction value{ *ip };
        AdvanceInstructionPtr();
        return value;
    }

    auto VirtualMachine::InstructionPtrConstant() noexcept -> Value {
        const Value constant{ current_chunk->GetConstant( *ip ) };
        AdvanceInstructionPtr();
        return constant;
    }

    auto VirtualMachine::TraceDebug() const noexcept -> void {
        for ( const auto& value : ranges::subrange( stack.begin(), stack.Top() ) ) {
            try {
                fmt::println( "[ {} ]", value );
            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n';
                std::cerr << "Numeric code: " << err.code().value() << '\n';
            }
        }
        Debug::DisassembleInstruction(
            *current_chunk,
            static_cast< std::size_t >( ranges::distance( current_chunk->cbegin(), ip ) ) );
    }

    auto VirtualMachine::ReadFile( const fs::path& path ) noexcept -> std::string {
        if ( !std::filesystem::is_regular_file( path ) ) { return ""; }

        long file_size{};
        if ( const std::size_t tmp_size = fs::file_size( path );
             !std::in_range< long >( tmp_size ) ) {
            return "";
        } else {
            file_size = static_cast< long >( tmp_size );
        }

        std::ifstream file( path, std::ios::in );
        if ( !file.is_open() ) { return ""; }

        std::string source{};
        source.resize( static_cast< std::size_t >( file_size ) );

        file.read( source.data(), file_size );

        return source;
    }

} // namespace LuxLibrary
