cmake_minimum_required(VERSION 3.24)
project(sbl C)

add_compile_options(-fsanitize=address -D_GNU_SOURCE)
add_link_options(-lasan -fsanitize=address)

set(CMAKE_C_STANDARD 23)

add_executable(test pug.c types.h pug.h prints.h macro_builder.h)
