cmake_minimum_required(VERSION 3.14)
project(webserv)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}-googletest 
	src/server/HttpMessage.cpp 
	src/config/Lexer.cpp
	test/conf/main_parser_test.cpp 
	src/config/Parser.cpp
	src/config/Parser.cpp
	src/config/conf.cpp
)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
target_include_directories(${PROJECT_NAME}-googletest PRIVATE
	./src
	./src/config
	./src/config/parser
)
add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
# target_compile_definitions(${PROJECT_NAME}-googletest PRIVATE TEST) # To avoid including main() when testing.
enable_testing()
