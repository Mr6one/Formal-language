cmake_minimum_required(VERSION 2.7)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "--coverage")

add_executable(runTests tests.cpp)

add_subdirectory(googletest-master)
include_directories(googletest-master/googletest/include)
include_directories(googletest-master/googlemock/include)

target_link_libraries(runTests gtest gtest_main)
