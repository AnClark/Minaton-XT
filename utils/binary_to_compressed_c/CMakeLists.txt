# binary_to_compressed_c CMakeLists.txt
#
# Copyright (C) 2026 AnClark Liu
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.20)
project(binary_to_compressed_c LANGUAGES CXX)

add_executable(${PROJECT_NAME} binary_to_compressed_c.cpp)

if (WIN32 AND NOT MSVC)
    target_link_options (${PROJECT_NAME} PRIVATE -static)
endif ()
