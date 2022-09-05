cmake_minimum_required(VERSION 3.16) # Latest version on apt.
project(TSCB VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)

add_executable(TSCB main.c ryml.cpp)
