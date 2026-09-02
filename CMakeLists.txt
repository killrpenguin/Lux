cmake_minimum_required(VERSION 3.30...4.1)

project(
  Lux
  VERSION 1.0
  DESCRIPTION "A tutorial based project."
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
endif()

include(FetchContent)

# Global Settings
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER "/usr/lib/llvm/23/bin/clang")

set(CMAKE_CXX_COMPILER "/usr/lib/llvm/23/bin/clang++")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--allow-no-checks;-checks=")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_subdirectory(tests)

add_subdirectory(lib)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG main
	EXCLUDE_FROM_ALL
	SYSTEM
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/fmt"
)

FetchContent_MakeAvailable(fmt)

add_executable(Lux main.cpp)

include(${CMAKE_SOURCE_DIR}/CMake/CompilerFlags.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/CPPCheck.cmake)

target_compile_options(Lux PUBLIC
    $<$<CONFIG:Debug>:-g -O0 -march=native>
    $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
    $<$<CXX_COMPILER_ID:Clang>: ${COMPILER_FLAGS} -std=c++2c>
)

target_compile_features(Lux PRIVATE)

target_link_libraries(Lux PRIVATE LL fmt::fmt)

target_link_options(Lux PUBLIC
 	 $<$<CONFIG:Debug>:>
	 $<$<CONFIG:Release>:>
)
