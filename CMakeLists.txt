cmake_minimum_required (VERSION 3.20)

project (minaton
    VERSION 0.1.0
    LANGUAGES C CXX
)

add_subdirectory (dpf)

#
# Manage dependencies
#

set (USE_SYSTEM_LIBSNDFILE OFF CACHE BOOL "Prefer system libsndfile (shared library). Otherwise build from source (static library)")
set (USE_SYSTEM_LIBSAMPLERATE OFF CACHE BOOL "Prefer system libsamplerate (shared library). Otherwise build from source (static library)")

if (USE_SYSTEM_LIBSNDFILE OR USE_SYSTEM_LIBSAMPLERATE)
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

    add_subdirectory (vendor/libsndfile)

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
# Convert waves into C array
#

set (GENERATED_WAVES_DIR ${PROJECT_BINARY_DIR}/generated_waves)
make_directory (${GENERATED_WAVES_DIR})

# Binary-to-C converter from Dear ImGui
add_executable (binary_to_compressed_c utils/binary_to_compressed_c.cpp)
target_link_options (binary_to_compressed_c PRIVATE -static)

set (WAVES_DIR ${PROJECT_SOURCE_DIR}/src/waves)
set (GENERATED_WAVE_HEADER ${GENERATED_WAVES_DIR}/minaton_waves.hpp)

add_custom_command (
    OUTPUT ${GENERATED_WAVE_HEADER}
    COMMAND ${PROJECT_BINARY_DIR}/binary_to_compressed_c${CMAKE_EXECUTABLE_SUFFIX} ${WAVES_DIR} minaton_wave > ${GENERATED_WAVE_HEADER}
    DEPENDS binary_to_compressed_c
    VERBATIM
)

add_custom_target (generate_waves DEPENDS ${GENERATED_WAVE_HEADER})

#
# Build plug-ins
#

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 lv2 clap jack
    UI_TYPE opengl
    FILES_DSP
        src/synth.cpp
        src/synth_dpf.cpp
        plugin/MinatonPlugin.cpp
        plugin/MinatonProcess.cpp
        plugin/MinatonParamAccess.cpp
        vendor/resampler/resampler.cpp
    FILES_UI
        plugin/MinatonUI.cpp
        plugin/MinatonUIHelper.cpp
        plugin/MinatonArtwork.cpp
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
