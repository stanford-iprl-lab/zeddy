cmake_minimum_required(VERSION 3.24.2)

project(zeddy CXX)

include(etc/build_type.cmake)
include(etc/cflags.cmake)

include_directories("${PROJECT_SOURCE_DIR}/util")

add_subdirectory("${PROJECT_SOURCE_DIR}/util")
add_subdirectory("${PROJECT_SOURCE_DIR}/apps")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
