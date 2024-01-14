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
add_executable(${PROJECT_NAME}-googletest src/HttpMessage.cpp test/test.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
# target_compile_definitions(${PROJECT_NAME}-googletest PRIVATE TEST) # To avoid including main() when testing.
enable_testing()
