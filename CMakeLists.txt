cmake_minimum_required(VERSION 3.16)

project(WFI LANGUAGES CXX VERSION 0.0.1)

include(CheckCXXCompilerFlag)

# Options
option(BUILD_TESTS "Build tests using googletest" ON)

# Basic CMake setup
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/WFI
  ${gtest_SOURCE_DIR}
  ${gtest_SOURCE_DIR}/include
)

# WFI sources
set(SOURCES_WFI
  src/main.cpp
  src/repl.cpp
  src/evaluator.cpp
  src/object.cpp
  src/parser.cpp
  src/ast.cpp
  src/lexer.cpp
  src/token.cpp
)

# Test sources
set(SOURCES_TESTS
  tests/evaluator_test.cpp
  tests/ast_test.cpp
  tests/lexer_test.cpp
  tests/parser_test.cpp
  src/object.cpp
  src/evaluator.cpp
  src/parser.cpp
  src/ast.cpp
  src/lexer.cpp
  src/token.cpp
)

# WFI config
add_executable(wfi ${SOURCES_WFI})

# Tests config
if(BUILD_TESTS)
  enable_testing()

  # Run short form tests
  add_executable(run_tests ${SOURCES_TESTS})
  target_link_libraries(run_tests GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(run_tests)
endif()