cmake_minimum_required(VERSION 3.12)
project(my_parseFat C)

set(CMAKE_C_STANDARD 11)

add_executable(my_parseFat code/main.c code/fat_structs.h code/disk_parser.h code/disk_parser.c code/bios_param_block.h code/bios_param_block.c code/macros.h)