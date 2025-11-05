cmake_minimum_required(VERSION 3.17)
project(stb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE include)