cmake_minimum_required(VERSION 3.16) # Latest version on apt.
project(TSCB VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(TSCB main.cpp yaml.cpp yaml.h)
