cmake_minimum_required(VERSION 3.24)
project(sbl C)

add_compile_options(-fsanitize=address)
add_link_options(-lasan -fsanitize=address)

set(CMAKE_C_STANDARD 23)

add_executable(test pug.c types.h)
