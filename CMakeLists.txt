cmake_minimum_required(VERSION 3.1)
project(fbrs C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(example_switch examples/switch.c src/fbrs.c)
add_executable(example_basic examples/basic.c src/fbrs.c)
target_include_directories(example_basic PRIVATE include)
