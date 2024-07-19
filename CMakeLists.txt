cmake_minimum_required(VERSION 3.29)
project(simpleparser)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_executable(simpleparser calculate.cpp)
