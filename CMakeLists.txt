cmake_minimum_required(VERSION 3.8)
project(IFJ18)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES parser.c parser.h stackgenerator.c stackgenerator.h symtable.c symtable.h expression.c expression.h dynamic_string.c dynamic_string.h code_generator.c code_generator.h scanner.c scanner.h error.h)
add_library(IFJ18 ${SOURCE_FILES})