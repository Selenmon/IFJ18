cmake_minimum_required(VERSION 3.8)
project(IFJ18)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES parser.c parser.h)
add_library(IFJ18 ${SOURCE_FILES})