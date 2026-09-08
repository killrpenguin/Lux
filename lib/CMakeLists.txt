set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(HEADER_LIST
        "${INCLUDE_PATH}Chunk.hpp"
	"${INCLUDE_PATH}Common.hpp"
	"${INCLUDE_PATH}Compiler.hpp"
	"${INCLUDE_PATH}CustomTypes.hpp"
        "${INCLUDE_PATH}Debug.hpp"
	"${INCLUDE_PATH}EscapeCodes.hpp"
	"${INCLUDE_PATH}InterpretResult.hpp"
	"${INCLUDE_PATH}OpCode.hpp"
	"${INCLUDE_PATH}Object.hpp"
        "${INCLUDE_PATH}Parser.hpp"
        "${INCLUDE_PATH}ParserRules.hpp"
	"${INCLUDE_PATH}Repl.hpp"
	"${INCLUDE_PATH}Scanner.hpp"
	"${INCLUDE_PATH}Stack.hpp"
	"${INCLUDE_PATH}Token.hpp"
	"${INCLUDE_PATH}TokenVariant.hpp"
	"${INCLUDE_PATH}Value.hpp"
	"${INCLUDE_PATH}ValueArray.hpp"
	"${INCLUDE_PATH}VirtualMachine.hpp"
)

set(SRC_LIST
	"Chunk.cpp"
	"Common.cpp"
	"Compiler.cpp"
	"CustomTypes.cpp"
	"Debug.cpp"
	"InterpretResult.cpp"
	"OpCode.cpp"
	"Object.cpp"
    	"Parser.cpp"
    	"ParserRules.cpp"
	"Repl.cpp"
	"Scanner.cpp"
	"Stack.cpp"
	"Value.cpp"
	"ValueArray.cpp"
	"VirtualMachine.cpp"
	"VirtualMachineRun.cpp"
    	"Token.cpp"
    	"TokenVariant.cpp"
)

add_library(LL ${SRC_LIST} ${HEADER_LIST})

target_precompile_headers(LL PUBLIC "${INCLUDE_PATH}/pch.hpp")

set_target_properties(LL PROPERTIES UNITY_BUILD ON)

target_compile_options(LL PUBLIC
    $<$<CONFIG:Debug>:-g -O3>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CXX_COMPILER_ID:Clang>: ${COMPILER_FLAGS} -std=c++2c>
)

target_include_directories(LL PUBLIC ${INCLUDE_PATH})

target_link_libraries(LL PUBLIC fmt::fmt)
