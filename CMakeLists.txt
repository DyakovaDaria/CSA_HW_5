cmake_minimum_required(VERSION 3.21)
project(CSA_HW_5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_C_FLAGS -pthread)

add_executable(CSA_HW_5 main.cpp)
