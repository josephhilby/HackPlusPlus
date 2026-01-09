cmake_minimum_required(VERSION 3.20)
project(P6 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(Main
               src/Main.c
               src/Lexer.c
               src/Parser.c
               src/Generator.c
               src/Structs.c
)