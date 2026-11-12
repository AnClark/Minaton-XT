cmake_minimum_required (VERSION 3.20)

project (minaton
    VERSION 1.0.0
    LANGUAGES C CXX
)

add_subdirectory (dpf)

# Explicitly specify C++ standard
set (CMAKE_CXX_STANDARD 17)

# Add our CMake extention module path
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#
# Manage dependencies
#

set (USE_SYSTEM_LIBSNDFILE OFF CACHE BOOL "Prefer system libsndfile (shared library). Otherwise build from source (static library)")
set (USE_SYSTEM_LIBSAMPLERATE OFF CACHE BOOL "Prefer system libsamplerate (shared library). Otherwise build from source (static library)")
set (BUILD_JACK_STANDALONE OFF CACHE BOOL "Build JACK standalone application on supported platforms (if JACK is installed)")

if (USE_SYSTEM_LIBSNDFILE OR USE_SYSTEM_LIBSAMPLERATE OR BUILD_JACK_STANDALONE)
    find_package (PkgConfig REQUIRED)
endif ()

if (USE_SYSTEM_LIBSNDFILE)
    pkg_check_modules (SNDFILE IMPORTED_TARGET REQUIRED sndfile)
    if (SNDFILE_FOUND)
        find_library (SNDFILE_LIBRARY NAMES ${SNDFILE_LIBRARIES} HINTS ${SNDFILE_LIBDIR})
    endif ()
else ()
    # Force build of static lib
    set (BUILD_SHARED_LIBS OFF CACHE BOOL "")

    # Disable external formats (FLAC, Opus, MPEG etc.), since we only decode WAV files
    set (ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "")
    set (ENABLE_MPEG OFF CACHE BOOL "")

    # Liten our builds
    set (BUILD_EXAMPLES OFF CACHE BOOL "")
    set (BUILD_PROGRAMS OFF CACHE BOOL "")
    set (BUILD_REGTEST OFF CACHE BOOL "")
    set (BUILD_TESTING OFF CACHE BOOL "")
    set (ENABLE_BOW_DOCS OFF CACHE BOOL "")
    set (ENABLE_CPACK OFF CACHE BOOL "")

    # EXCLUDE_FROM_ALL: ignore libsndfile's own install() rules, so its
    # libraries/headers/manpages won't leak into our CPack packages (ZIP/pkg).
    # The sndfile target is still built via target_link_libraries() dependency.
    add_subdirectory (vendor/libsndfile EXCLUDE_FROM_ALL)

    # Need this to fix linker errors around Glibc
    target_compile_options (sndfile PRIVATE -fPIC)

    include_directories (vendor/libsndfile/include)
endif ()

if (USE_SYSTEM_LIBSAMPLERATE)
    pkg_check_modules (SAMPLERATE IMPORTED_TARGET REQUIRED samplerate)
    if (SAMPLERATE_FOUND)
        find_library (SAMPLERATE_LIBRARY NAMES ${SAMPLERATE_LIBRARIES} HINTS ${SAMPLERATE_LIBDIR})
    endif ()
else ()
    # Liten our builds
    set (BUILD_TESTING OFF CACHE BOOL "")
    set (LIBSAMPLERATE_EXAMPLES OFF CACHE BOOL "")

    add_subdirectory (vendor/libsamplerate)

    include_directories (vendor/libsamplerate/include)
endif ()

#
# CI workarounds
#

# GitHub CI's Ubuntu runner cannot directly run lv2-ttl-generator.exe, even though Wine and wine-binfmt is installed.
# And, LV2 is not so common on Windows (only REAPER, Ardour and Harrison Mixbus support LV2 on Windows).
# So disable LV2 on Windows by default.
option (CONFIG_BUILD_LV2_ON_WINDOWS "Build LV2 on Windows (including cross-compiling)." OFF)
if (WIN32 AND CONFIG_BUILD_LV2_ON_WINDOWS)
    message (STATUS "Win32 environment and LV2 build is enabled, will build LV2 plugin")
    set (TARGET_LV2 lv2)
elseif (WIN32)
    message (STATUS "Win32 environment and LV2 build is disabled. Bypass LV2")
else ()
    message (STATUS "Non-Win32 environment, will build LV2 plugin")
    set (TARGET_LV2 lv2)
endif ()

#
# Convert waves into C array
#

set (GENERATED_WAVES_DIR ${PROJECT_BINARY_DIR}/generated_waves)
make_directory (${GENERATED_WAVES_DIR})

# Binary-to-C converter from Dear ImGui
set (binary_to_compressed_c_BUILD_DIR ${PROJECT_BINARY_DIR}/binary_to_compressed_c_build)
set (binary_to_compressed_c_PATH ${binary_to_compressed_c_BUILD_DIR}/binary_to_compressed_c)

# Always build Binary-to-C converter with local native toolchain, so it can run properly on cross-compilation
# (especially when cross-building for Windows).
include (ExternalProject)
ExternalProject_Add (build_native_binary_to_compressed_c
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/utils/binary_to_compressed_c
  BINARY_DIR ${binary_to_compressed_c_BUILD_DIR}
  CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      "-DCMAKE_TOOLCHAIN_FILE:STRING="    # KEY: Prevent subproject from inheriting cross toolchain via environment variable
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS
      ${binary_to_compressed_c_PATH}
)

set (WAVES_DIR ${PROJECT_SOURCE_DIR}/src/waves)
set (GENERATED_WAVE_HEADER ${GENERATED_WAVES_DIR}/minaton_waves.hpp)

add_custom_command (
    OUTPUT ${GENERATED_WAVE_HEADER}
    COMMAND ${binary_to_compressed_c_PATH} ${WAVES_DIR} minaton_wave > ${GENERATED_WAVE_HEADER}
    DEPENDS build_native_binary_to_compressed_c
    VERBATIM
)

add_custom_target (generate_waves DEPENDS ${GENERATED_WAVE_HEADER})

#
# Build plug-ins
#

# Only build standalone (JACK) application if JACK is installed on system
if (BUILD_JACK_STANDALONE)
    set (JACK_TARGET jack)
endif ()

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 ${TARGET_LV2} clap ${JACK_TARGET}
    UI_TYPE opengl
    FILES_DSP
        src/synth.cpp
        src/synth_dpf.cpp
        plugin/MinatonPlugin.cpp
        plugin/MinatonProcess.cpp
        plugin/MinatonParamAccess.cpp
    FILES_UI
        plugin/MinatonUI.cpp
        plugin/MinatonUIHelper.cpp
        plugin/MinatonArtwork.cpp
        plugin/MinatonPresets.cpp
    FILES_COMMON
        plugin/MinatonParams.cpp
)

configure_file (plugin/config.h.in ${PROJECT_BINARY_DIR}/config.h)

target_include_directories (${PROJECT_NAME} PUBLIC 
    plugin/
    src/
    ${GENERATED_WAVES_DIR}
    vendor/resampler
    ${PROJECT_BINARY_DIR}
)

add_dependencies (${PROJECT_NAME}-dsp generate_waves)

if (USE_SYSTEM_LIBSNDFILE)
    set (LINK_LIBSNDFILE PkgConfig::SNDFILE)
else ()
    set (LINK_LIBSNDFILE sndfile)
endif ()

if (USE_SYSTEM_LIBSAMPLERATE)
    set (LINK_LIBSAMPLERATE PkgConfig::SAMPLERATE)
else ()
    set (LINK_LIBSAMPLERATE samplerate)
endif ()

target_link_libraries (${PROJECT_NAME} PRIVATE ${LINK_LIBSAMPLERATE} ${LINK_LIBSNDFILE})

#
# Convert factory patches into C array
# Should be included after dpf_add_plugin().
#

include (FactoryPresets)

#
# Dear ImGui context menu & overlay widget support
#

# Dear ImGui sources
set (DEAR_IMGUI_SOURCE vendor/dpf-widgets/dpf-imgui/DearImGui.cpp)

target_sources (${PROJECT_NAME}-ui PRIVATE
    ${DEAR_IMGUI_SOURCE}
    plugin/MinatonUIMenu.cpp
)

target_include_directories (${PROJECT_NAME}-ui PRIVATE
    vendor/dpf-widgets/dpf-imgui
)

#
# CPack stuff
#

# Metadata (for CPack)
set (AUTHOR "AnClark Liu")
set (INSTALL_DIRECTORY ${PROJECT_NAME})
set (ICON_PATH "${PROJECT_SOURCE_DIR}/Screenshot.png")  # TODO: Re-design icon
set (PRODUCTBUILD_IDENTIFIER "com.anclark.minaton.xt")

# Import CPack routines
include (Packaging)
