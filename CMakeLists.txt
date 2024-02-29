cmake_minimum_required(VERSION 3.24.2)

project(zeddy CXX)

include(etc/build_type.cmake)
include(etc/cflags.cmake)

find_package(PkgConfig)

pkg_check_modules(X264 REQUIRED x264)
include_directories(${X264_INCLUDE_DIRS})
add_compile_options(${X264_CFLAGS_OTHER})

pkg_check_modules(swscale REQUIRED libswscale)
include_directories(${swscale_INCLUDE_DIRS})
add_compile_options(${swscale_CFLAGS})

include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/video")

add_subdirectory("${PROJECT_SOURCE_DIR}/util")
add_subdirectory("${PROJECT_SOURCE_DIR}/video")
add_subdirectory("${PROJECT_SOURCE_DIR}/apps")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
