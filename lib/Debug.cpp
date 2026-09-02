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

#include "Debug.hpp"
#include "OpCode.hpp"

#include <fmt/ostream.h>

#include <exception>
#include <optional>
#include <system_error>
#include <utility>

namespace LuxLibrary {
    auto Debug::To_OpCode( const std::uint8_t instruction ) noexcept -> std::optional< OpCode > {
        switch ( instruction ) {
            case std::to_underlying( OpCode::OP_RETURN ): {
                return OpCode::OP_RETURN;
            }
            case std::to_underlying( OpCode::OP_NEGATE ): {
                return OpCode::OP_NEGATE;
            }
            case std::to_underlying( OpCode::OP_ADD ): {
                return OpCode::OP_ADD;
            }
            case std::to_underlying( OpCode::OP_SUBTRACT ): {
                return OpCode::OP_SUBTRACT;
            }
            case std::to_underlying( OpCode::OP_MULTIPLY ): {
                return OpCode::OP_MULTIPLY;
            }
            case std::to_underlying( OpCode::OP_DIVIDE ): {
                return OpCode::OP_DIVIDE;
            }
            case std::to_underlying( OpCode::OP_CONSTANT ): {
                return OpCode::OP_CONSTANT;
            }
            case std::to_underlying( OpCode::OP_TRUE ): {
                return OpCode::OP_TRUE;
            }
            case std::to_underlying( OpCode::OP_FALSE ): {
                return OpCode::OP_FALSE;
            }
            case std::to_underlying( OpCode::OP_NIL ): {
                return OpCode::OP_NIL;
            }
            case std::to_underlying( OpCode::OP_EQUAL ): {
                return OpCode::OP_EQUAL;
            }
            case std::to_underlying( OpCode::OP_GREATER ): {
                return OpCode::OP_GREATER;
            }
            case std::to_underlying( OpCode::OP_LESS ): {
                return OpCode::OP_LESS;
            }
            default: {
                assert( false && "Undefined instruction in to_opcode. " );
            }
        }
    }

    auto Debug::DisassembleChunk(
        const Chunk& chunk, const std::string_view name, std::ostream& stream ) noexcept -> void {
        try {
            fmt::println( stream, "== {} ==", name );
        } catch ( const std::system_error& err ) {
            std::cerr << "Error string: " << err.what() << '\n'
                      << "Numeric code: " << err.code().value() << '\n';
        }

        std::size_t offset{ 0 };
        while ( offset < chunk.Count() ) {
            offset = DisassembleInstruction( chunk, offset, stream );
        }
    }

    auto Debug::DisassembleInstruction(
        const Chunk& chunk, const std::size_t offset, std::ostream& stream ) noexcept
        -> std::size_t {
        try {
            fmt::print( stream, "{:04} ", offset );
        } catch ( const fmt::format_error& err ) {
            std::cerr << "Format error formatting " << typeid( offset ).name() << "\n\t"
                      << err.what();
        } catch ( const std::system_error& err ) {
            std::cerr << "Error string: " << err.what() << '\n'
                      << "Numeric code: " << err.code().value() << '\n';
        }

        const unsigned int line{ chunk.GetLine( offset ) };

        if ( offset > 0 and line == chunk.GetLine( offset - 1 ) ) {
            try {
                fmt::print(
                    stream, "{:>4} ",
                    '|' ); // 3 spaces before the bar + 1 for the size of the character.
            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n'
                          << "Numeric code: " << err.code().value() << '\n';
            }

        } else {
            try {
                fmt::print( stream, "{:04} ", line );
            } catch ( const fmt::format_error& err ) {
                std::cerr << "Format error formatting " << typeid( line ).name() << "\n\t"
                          << err.what();
            } catch ( const std::system_error& err ) {
                std::cerr << "Error string: " << err.what() << '\n'
                          << "Numeric code: " << err.code().value() << '\n';
            }
        }

        const std::uint8_t instruction{ chunk[offset] };

        switch ( instruction ) {
            case std::to_underlying( OpCode::OP_RETURN ): {
                return SimpleInstruction( OpCode::OP_RETURN, offset, stream );
            }
            case std::to_underlying( OpCode::OP_NEGATE ): {
                return SimpleInstruction( OpCode::OP_NEGATE, offset, stream );
            }
            case std::to_underlying( OpCode::OP_NIL ): {
                return ConstantInstruction( OpCode::OP_NIL, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_TRUE ): {
                return ConstantInstruction( OpCode::OP_TRUE, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_FALSE ): {
                return ConstantInstruction( OpCode::OP_FALSE, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_EQUAL ): {
                return ConstantInstruction( OpCode::OP_EQUAL, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_GREATER ): {
                return ConstantInstruction( OpCode::OP_GREATER, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_LESS ): {
                return ConstantInstruction( OpCode::OP_LESS, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_CONSTANT ): {
                return ConstantInstruction( OpCode::OP_CONSTANT, chunk, offset, stream );
            }
            case std::to_underlying( OpCode::OP_ADD ): {
                return SimpleInstruction( OpCode::OP_ADD, offset, stream );
            }
            case std::to_underlying( OpCode::OP_SUBTRACT ): {
                return SimpleInstruction( OpCode::OP_SUBTRACT, offset, stream );
            }
            case std::to_underlying( OpCode::OP_MULTIPLY ): {
                return SimpleInstruction( OpCode::OP_MULTIPLY, offset, stream );
            }
            case std::to_underlying( OpCode::OP_DIVIDE ): {
                return SimpleInstruction( OpCode::OP_DIVIDE, offset, stream );
            }
            default: {
                try {
                    fmt::println( "Unknown opcode {}", instruction );
                } catch ( const std::system_error& err ) {
                    std::cerr << "Error string: " << err.what() << '\n'
                              << "Numeric code: " << err.code().value() << '\n';
                }

                return offset + 1;
            }
        }
    }

    auto Debug::GetOpcode( const Chunk& chunk, const std::size_t offset ) noexcept
        -> std::optional< std::pair< OpCode, unsigned int > > {
        const unsigned int line{ chunk.GetLine( offset ) };

        const std::uint8_t instruction{ chunk[offset] };
        if ( const std::optional< OpCode > op_code{ To_OpCode( instruction ) };
             op_code.has_value() ) {
            return std::make_pair( *op_code, line );
        }
        return std::nullopt;
    }

    auto Debug::SimpleInstruction(
        const OpCode instruction, const std::size_t offset, std::ostream& stream ) noexcept
        -> std::size_t {
        try {
            fmt::println( stream, "{}", instruction );
        } catch ( const std::system_error& err ) {
            std::cerr << "Error string: " << err.what() << '\n'
                      << "Numeric code: " << err.code().value() << '\n';
        }
        return offset + 1;
    }

    auto Debug::ConstantInstruction(
        const OpCode instruction, const Chunk& chunk, const std::size_t offset,
        std::ostream& stream ) noexcept -> std::size_t {
        const std::uint8_t constant{ chunk[offset + 1] };

        try {
            fmt::println(
                stream, "{:<16} {:4} {}", instruction, constant, chunk.GetConstant( constant ) );
        } catch ( const std::format_error& err ) {
            std::cerr << "Invalid format args for name or constant value. \n\tName: "
                      << format_as( instruction )
                      << "\n\tConstant value: " << static_cast< unsigned int >( constant )
                      << "\n\tError message: " << err.what();
        } catch ( const std::exception& err ) {
            std::cerr << "Caught an exception while printing: " << err.what();
        }

        return offset + 2;
    }

} // namespace LuxLibrary
