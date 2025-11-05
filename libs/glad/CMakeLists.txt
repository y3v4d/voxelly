# CMakeLists.txt for a directory with include/glad.h file. want it to be a library
cmake_minimum_required(VERSION 3.17)
project(glad)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(glad STATIC src/gl.c)
target_include_directories(glad PUBLIC include)