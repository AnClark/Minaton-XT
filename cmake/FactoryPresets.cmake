#  FactoryPresets.cmake - Factory bank (built-in presets) support
#
#  Copyright (C) 2023 AnClark Liu. All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

set (BANK_CODE_NAME minaton_factory_bank)
set (factory_bank_to_c_BUILD_DIR ${PROJECT_BINARY_DIR}/fb2c_build)
set (factory_bank_to_c_PATH ${factory_bank_to_c_BUILD_DIR}/factory_bank_to_c)

# Always build factory_bank_to_c with local native toolchain, so factory_bank_to_c can run properly on cross-compilation
# (especially when cross-building for Windows).
include(ExternalProject)
ExternalProject_Add(build_native_fb2c
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/utils/factory_bank_to_c
  BINARY_DIR ${factory_bank_to_c_BUILD_DIR}
  CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      "-DCMAKE_TOOLCHAIN_FILE:STRING="    # KEY: Prevent subproject from inheriting cross toolchain via environment variable
      -DBANK_CODE_NAME=${BANK_CODE_NAME}
      "-DBANK_CONFIG_FILE_EXPORT_DIR=${PROJECT_BINARY_DIR}"
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS
      ${factory_bank_to_c_PATH}
)

set(GENERATED_FACTORY_BANK_DIR "${CMAKE_BINARY_DIR}/factory_bank")
file(MAKE_DIRECTORY ${GENERATED_FACTORY_BANK_DIR})

# Add include directories
include_directories(
    ${GENERATED_FACTORY_BANK_DIR}
    ${PROJECT_BINARY_DIR}    # For fb2c_config.h
)

# Convert factory presets
#if(TR_BUILTIN_FACTORY_PRESETS)
    set(SOURCES_FACTORY_BANK
        ${GENERATED_FACTORY_BANK_DIR}/${BANK_CODE_NAME}.cpp
        ${GENERATED_FACTORY_BANK_DIR}/${BANK_CODE_NAME}.h
    )

    add_custom_command(
        OUTPUT ${SOURCES_FACTORY_BANK}
        COMMAND ${factory_bank_to_c_PATH} ${PROJECT_SOURCE_DIR}/src/patches ${GENERATED_FACTORY_BANK_DIR}
        DEPENDS build_native_fb2c
        VERBATIM
    )

    target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_FACTORY_BANK})
#endif()

