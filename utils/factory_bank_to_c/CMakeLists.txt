# factory_bank_to_c CMakeLists.txt
#
# Copyright (C) 2022-2023 AnClark Liu
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# Build the factory bank converter
add_executable(factory_bank_to_c
    binary_to_compressed_c.cpp
    main.cpp
)
target_include_directories(factory_bank_to_c PRIVATE .)

# Fallback to pre-C++20 struct definition syntax on MSVC
if(MSVC)
    target_compile_definitions(factory_bank_to_c PRIVATE -DMSVC_COMPATIBILITY)
endif()

# Statically link on Windows and Win32 cross-build
# This prevents "libgcc/libstdc++ not found" error on most conditions
if(WIN32 AND NOT MSVC)
    target_link_options(factory_bank_to_c PRIVATE -static)
endif()

# Include config file for setting output code name
# NOTICE: You must set variable BANK_CODE_NAME before add_subdirectory(<this directory>).
configure_file (./fb2c_config.h.in ${PROJECT_BINARY_DIR}/fb2c_config.h)
target_include_directories (factory_bank_to_c PRIVATE ${PROJECT_BINARY_DIR})
