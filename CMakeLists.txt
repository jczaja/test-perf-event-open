cmake_minimum_required(VERSION 3.14)
project(test-perf)

find_program(PERF NAMES perf)

if(PERF)
message(STATUS "Perf found!")
endif(PERF)

if (DEFINED ENABLE_PERF)      
add_definitions("-DENABLE_PERF")
endif()


add_executable(test-perf main.cpp)

