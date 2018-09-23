cmake_minimum_required(VERSION 3.12)
project(my_parseFat C)

set(CMAKE_C_STANDARD 11)

add_executable(my_parseFat main.c fat_structs.h disk_parser.h disk_parser.c bios_param_block.h bios_param_block.c macros.h)