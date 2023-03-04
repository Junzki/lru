cmake_minimum_required(VERSION 3.24)
project(lru C)

set(CMAKE_C_STANDARD 11)

add_executable(lru main.c lru.c hash.c)
