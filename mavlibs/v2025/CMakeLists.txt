cmake_minimum_required(VERSION 3.11)
project(mavlibs C)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")
add_library(mavlibs STATIC ${SRC_FILES})
