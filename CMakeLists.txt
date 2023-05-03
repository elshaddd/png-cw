cmake_minimum_required(VERSION 3.25)
project(test C)
#
set(CMAKE_C_STANDARD 99)

add_executable(test main.c io_com.c read_and_write.c libs_and_strucs.h read_and_write.h functions.c functions.h)
target_link_libraries(test png)