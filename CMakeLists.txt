cmake_minimum_required(VERSION 3.14)
project(webserv)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}-googletest test/conf/confLexerTest.cpp src/config/parser/Lexer.cpp)
#include_directories(${CMake_CURRENT_SOURCE_DIR}/src/config/parser)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
target_include_directories(${PROJECT_NAME}-googletest PRIVATE ./src/config/parser)
add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
enable_testing()
