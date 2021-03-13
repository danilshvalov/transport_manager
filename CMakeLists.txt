cmake_minimum_required(VERSION 3.17)
project(transport-manager VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

file(GLOB SOURCES "./src/*"
        "./src/descriptions/*"
        "./src/descriptions/errors/*"
        "./src/json/*"
        "./src/requests/*"
        "./src/requests/errors/*"
        "./src/responses/*"
        "./src/routing/*"
        "./src/svg/*"
        "./src/utils/*"
        "./src/transport/*"
        "./src/configuration/*"
        "./src/configuration/json/*"
        "./src/configuration/json/errors/*")

include_directories(${CMAKE_SOURCE_DIR}/src/descriptions)
include_directories(${CMAKE_SOURCE_DIR}/src/descriptions/errors)
include_directories(${CMAKE_SOURCE_DIR}/src/json)
include_directories(${CMAKE_SOURCE_DIR}/src/requests)
include_directories(${CMAKE_SOURCE_DIR}/src/requests/errors)
include_directories(${CMAKE_SOURCE_DIR}/src/responses)
include_directories(${CMAKE_SOURCE_DIR}/src/routing)
include_directories(${CMAKE_SOURCE_DIR}/src/svg)
include_directories(${CMAKE_SOURCE_DIR}/src/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/transport)
include_directories(${CMAKE_SOURCE_DIR}/src/configuration)
include_directories(${CMAKE_SOURCE_DIR}/src/configuration/json)
include_directories(${CMAKE_SOURCE_DIR}/src/configuration/json/errors)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(transport-manager ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
