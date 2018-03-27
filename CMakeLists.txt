cmake_minimum_required(VERSION 3.9)
project(MCSTimeoutLock C)

set(CMAKE_C_STANDARD 99)

add_executable(MCSTimeoutLock main.c)
target_link_libraries(MCSTimeoutLock ${CMAKE_SOURCE_DIR}/librafkids_dom1_win_fix.a)