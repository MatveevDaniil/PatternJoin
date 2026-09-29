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

add_library(patternjoin INTERFACE)
target_include_directories(patternjoin INTERFACE "${SRC_DIR}")

include(CTest)
if(BUILD_TESTING)
  add_executable(test_join tests/test_join.cpp)
  target_link_libraries(test_join PRIVATE patternjoin)
  add_test(NAME cross_join COMMAND test_join)
endif()
