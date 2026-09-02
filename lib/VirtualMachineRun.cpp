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

#include "OpCode.hpp"
#include "Value.hpp"
#include "VirtualMachine.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <functional>
#include <optional>
#include <tuple>

namespace LuxLibrary {

    namespace {
        template < typename Func >
        auto BinaryOperation( auto& stack, Func operation ) noexcept
            -> std::optional< std::string > {
            auto [lhs, rhs] = [&stack] {
                const std::optional< Value > _rhs{ stack.Pop() };
                const std::optional< Value > _lhs{ stack.Pop() };
                if ( _lhs.has_value() and _rhs.has_value() ) { return std::tuple{ *_lhs, *_rhs }; }
                return std::tuple{ Value(), Value() };
            }();

            if ( lhs.IsDouble() and rhs.IsDouble() ) {
                stack.Push( operation( lhs.AsDouble(), rhs.AsDouble() ) );
                return std::nullopt;
            }

            if ( lhs.IsInteger() and rhs.IsInteger() ) {
                stack.Push( operation( lhs.AsInteger(), rhs.AsInteger() ) );
                return std::nullopt;
            }

            return std::string( "Operands must be numbers." );
        }

        auto StringConcat( auto& stack ) noexcept -> std::optional< std::string > {
            auto [lhs, rhs] = [&stack] {
                const std::optional< Value > _rhs{ stack.Pop() };
                const std::optional< Value > _lhs{ stack.Pop() };
                if ( _lhs.has_value() and _rhs.has_value() ) { return std::tuple{ *_lhs, *_rhs }; }
                return std::tuple{ Value(), Value() };
            }();
            if ( lhs.IsString() and rhs.IsString() ) {
                const std::string new_string{
                    std::string(
                        std::from_range,
                        std::views::join( std::array{ lhs.AsStringView(), rhs.AsStringView() } ) ),
                };

                stack.Push( new_string );

                return std::nullopt;
            }
            return std::string( "Invalid operation on string." );
        }
    } // namespace

    auto VirtualMachine::Run() noexcept -> Result< Value > {
        if ( ip == current_chunk->cend() or current_chunk == nullptr ) {
            return ResultError( "Cannot process empty code chunk." );
        }

        while ( true ) {
            if constexpr ( detail::DEBUG_TRACE_EXECUTION ) { TraceDebug(); }

            const Instruction instruction{ InstructionPtrValue() };

            switch ( instruction ) {
                case std::to_underlying( OpCode::OP_CONSTANT ): {
                    stack.Push( InstructionPtrConstant() );
                    break;
                }
                case std::to_underlying( OpCode::OP_NIL ): {
                    stack.Push( Value() );
                    break;
                }
                case std::to_underlying( OpCode::OP_TRUE ): {
                    stack.Push( true );
                    break;
                }
                case std::to_underlying( OpCode::OP_FALSE ): {
                    stack.Push( false );
                    break;
                }
                case std::to_underlying( OpCode::OP_NOT ): {
                    if ( const StackOptional value{ stack.Pop() }; value.has_value() ) {
                        stack.Push( value->IsFalsey() );
                        break;
                    }
                    return ResultError( RuntimeError( "Invalid expression." ) );
                }
                case std::to_underlying( OpCode::OP_EQUAL ): {
                    const std::optional< Value > lhs{ stack.Pop() };
                    const std::optional< Value > rhs{ stack.Pop() };

                    const Result< bool > compared_equal{ Value::ValuesEqual( lhs, rhs ) };
                    if ( !compared_equal ) {
                        return ResultError( RuntimeError( compared_equal.error() ) );
                    }
                    stack.Push( *compared_equal );
                    break;
                }
                case std::to_underlying( OpCode::OP_GREATER ): {
                    const OptionalError error{ BinaryOperation( stack, std::greater<>() ) };

                    if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                    break;
                }
                case std::to_underlying( OpCode::OP_LESS ): {
                    const OptionalError error{ BinaryOperation( stack, std::less<>() ) };

                    if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                    break;
                }
                case std::to_underlying( OpCode::OP_ADD ): {
                    const Value& current{ stack.PeekPrev() };
                    if ( current.IsDouble() or current.IsInteger() ) {
                        const OptionalError error{ BinaryOperation( stack, std::plus<>() ) };

                        if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                        break;
                    } else if ( current.IsString() ) {
                        const OptionalError error{ StringConcat( stack ) };

                        if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                        break;
                    }
                    break;
                }
                case std::to_underlying( OpCode::OP_SUBTRACT ): {
                    const OptionalError error{ BinaryOperation( stack, std::minus<>() ) };

                    if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                    break;
                }
                case std::to_underlying( OpCode::OP_MULTIPLY ): {
                    const OptionalError error{ BinaryOperation( stack, std::multiplies<>() ) };

                    if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                    break;
                }
                case std::to_underlying( OpCode::OP_DIVIDE ): {
                    const OptionalError error{ BinaryOperation( stack, std::divides<>() ) };

                    if ( error.has_value() ) { return ResultError( RuntimeError( *error ) ); }
                    break;
                }
                case std::to_underlying( OpCode::OP_NEGATE ): {
                    if ( const Value& current{ stack.PeekPrev() };
                         !current.IsDouble() and !current.IsInteger() ) {
                        return ResultError( RuntimeError( "Operand must be a number." ) );
                    }

                    const StackOptional optional{ stack.Pop() };

                    if ( !optional.has_value() ) {
                        return ResultError( RuntimeError( "Invalid expression." ) );
                    }
                    if ( optional->IsDouble() ) {
                        const double num{ optional->AsDouble() };
                        stack.Push( -num );
                        break;
                    }
                    const integer num{ optional->AsInteger() };
                    stack.Push( -num );
                    break;
                }
                case std::to_underlying( OpCode::OP_RETURN ): {
                    if ( const StackOptional optional{ stack.Pop() }; optional.has_value() ) {
                        return *optional;
                    }
                    return ResultError( RuntimeError( "Invalid expression." ) );
                }
                default: {
                    return ResultError( "Invalid opcode detected." );
                }
            }
        }
    }

} // namespace LuxLibrary
