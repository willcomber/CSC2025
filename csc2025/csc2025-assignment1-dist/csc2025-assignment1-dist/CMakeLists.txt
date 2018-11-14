# cmake_minimum_required(VERSION <specify CMake version here>)
project(csc2025_assignment1_dist C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(test_src)

add_executable(csc2025_assignment1_dist
        test_src/integer_app.c
        test_src/string_app.c
        test_src/strtest_lib.c
        test_src/strtest_lib.h
        test_src/test_integer.c
        test_src/test_lib.c
        test_src/test_lib.h
        test_src/test_obj_store.c
        test_src/test_string.c
        integer.c
        integer.h
        obj_store.c
        obj_store.h
        string_o.c
        string_o.h)
