cmake_minimum_required(VERSION 3.31)
project(my_stl)

set(CMAKE_CXX_STANDARD 23)

add_executable(my_stl main.cpp
        my/vector.h)
