cmake_minimum_required(VERSION 3.10)
project(Networks C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(Networks
        app_mate.c
        common.h
        sockutil.c
        sockutil.h
        alist.c
        alist.h)

#add_executable(Alist
#        alist.c
#        alist.h
#        test.c)
