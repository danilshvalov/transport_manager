cmake_minimum_required(VERSION 3.0.0)
project(transport-manager VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB SOURCES "./src/*")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Werror -fsanitize=address -fsanitize=leak -fsanitize=undefined")

add_executable(transport-manager ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
