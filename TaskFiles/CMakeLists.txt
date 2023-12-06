cmake_minimum_required(VERSION 3.26)
project(TaskFiles C)

set(CMAKE_C_STANDARD 11)

add_executable(TaskFiles main.c)

target_link_libraries(TaskFiles m)