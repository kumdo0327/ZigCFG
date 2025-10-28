cmake_minimum_required(VERSION 3.20.0)
project(zigcfg)

# Compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Compiler config
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/externals
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/include
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-zig/bindings/c
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-zig/src
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

# Source
file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/*.cpp
)

# External
file(GLOB EXTERNAL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/src/lib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-zig/src/*.c
)

# Executable
add_executable(${CMAKE_PROJECT_NAME}
  ${EXTERNAL_SRC}
  ${SOURCES}
)

# Binary path
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)