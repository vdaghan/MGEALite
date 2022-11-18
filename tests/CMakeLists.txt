cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(MGEALite_Tests CXX)

enable_testing()

include(FetchContent)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929) # release-1.11
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
message(STATUS "gtest_SOURCE_DIR = ${gtest_SOURCE_DIR}")

add_executable(MGEALite_Tests)
target_sources(MGEALite_Tests PUBLIC "${CMAKE_SOURCE_DIR}/tests/MGEALite_AngularVelocitySign.cpp")
target_sources(MGEALite_Tests PUBLIC "${CMAKE_SOURCE_DIR}/src/Common.cpp")
add_executable(MGEALite::Tests ALIAS MGEALite_Tests)
#target_link_libraries(MGEALite_Tests PUBLIC MGEALite)

target_include_directories(MGEALite_Tests PUBLIC "../include")
target_include_directories(MGEALite_Tests PUBLIC "${gtest_SOURCE_DIR}/googletest/include")
target_link_libraries(MGEALite_Tests PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(MGEALite_Tests TEST_PREFIX MGEALite::)
