cmake_minimum_required(VERSION 3.20)
project(lehman_yao_final C)

set(CMAKE_C_STANDARD 99)
add_subdirectory(index)
link_directories(/usr/local/lib)
include_directories(/usr/local/include)
add_executable(lehman_yao_final main.c)
target_link_libraries(lehman_yao_final PUBLIC index cslice)
