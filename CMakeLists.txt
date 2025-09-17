cmake_minimum_required(VERSION 3.29)
project(Assignment_1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_executable(Assignment_1 main.c
        core.c
        core.h
        cbmp.c
        cbmp.h)

