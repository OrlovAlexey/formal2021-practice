cmake_minimum_required(VERSION 3.5)
project(earley)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)

add_executable(earley main.cpp Earley.h Earley.cpp Grammar.h Grammar.cpp)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(CodeCoverage)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(Tests run_tests.cpp tests/Tests.cpp tests/Tests.h Earley.cpp Earley.h Grammar.h Grammar.cpp)
target_link_libraries(Tests ${GTEST_LIBRARIES} Threads::Threads)

setup_target_for_coverage(coverage_report Tests coverage)
SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_BUILD_TYPE "Debug")

enable_testing()