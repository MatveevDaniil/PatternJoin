cmake_minimum_required(VERSION 3.10)
project(pattern_join)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(OBJ_DIR "${CMAKE_CURRENT_BINARY_DIR}/obj")

file(GLOB SOURCES "${SRC_DIR}/*.cpp")
add_executable(pattern_join ${SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")