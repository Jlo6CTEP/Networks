cmake_minimum_required(VERSION 3.10)
project(Networks C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

#add_executable(Networks
#        app_mate.c
#        array_list/alist.c
#        array_list/alist.h)

add_executable(Alist
        array_list/alist.c
        array_list/alist.h
        array_list/test.c)
