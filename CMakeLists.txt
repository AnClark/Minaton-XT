cmake_minimum_required (VERSION 3.20)

project (minaton
    VERSION 0.0.3
    LANGUAGES C CXX
)

add_subdirectory (dpf)

#
# CPack configuration #1
#

# NOTICE:
#
# libsamplerate CMakeLists.txt will unconditionally include CPack.
# If included twice, CPack will ONLY package source tree in all condition!
# This is a trap. Mind your steps.

# Where to find our CMake modules
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include our CPack config file
include (Packing)

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

    # Reduce install targets
    # Note: Cannot thoroughly switch off installation. See cmake/PackingComponents.cmake for workaround.
    set (ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "")
    set (INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "")

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

    # Switch off installation because we are using CPack
    set (LIBSAMPLERATE_INSTALL OFF CACHE BOOL "")

    add_subdirectory (vendor/libsamplerate)

    include_directories (vendor/libsamplerate/include)
endif ()

#
# CPack configurations #2
#

# Prevent including CPack twice.
# If not using system-provided libsamplerate, CPack is already included.
if (USE_SYSTEM_LIBSAMPLERATE)
    include (CPack)
endif ()

# Include our CPack component definition
# Must included CPack before including this
include (PackingComponents)

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
    TARGETS vst2 vst3 lv2 clap
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

target_include_directories (${PROJECT_NAME} PUBLIC 
    plugin/
    src/
    ${GENERATED_WAVES_DIR}
    vendor/resampler
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
# Install
#

# NOTICE:
# Must use relative path for NSIS generator, otherwise this error will occur:
#    ABSOLUTE path INSTALL DESTINATION forbidden (by caller)
# See: https://stackoverflow.com/questions/23613624/cpack-nsis-generate-installer-for-windows

dpf__determine_vst3_package_architecture(vst3_arch)
if (WIN32)
    install (TARGETS ${PROJECT_NAME}-vst2
        COMPONENT VST2Plugin
        LIBRARY DESTINATION "${CPACK_INSTALL_PREFIX}/Steinberg/VstPlugins"
    )
    install (TARGETS ${PROJECT_NAME}-vst3
        COMPONENT VST3Plugin
        LIBRARY DESTINATION "${CPACK_INSTALL_PREFIX}/Common Files/VST3/${PROJECT_NAME}.vst3/${vst3_arch}/"
    )
    install (TARGETS ${PROJECT_NAME}-clap
        COMPONENT CLAPPlugin
        LIBRARY DESTINATION "${CPACK_INSTALL_PREFIX}/Common Files/CLAP"
    )
    install (TARGETS ${PROJECT_NAME}-lv2
        COMPONENT LV2Plugin
        LIBRARY DESTINATION "${CPACK_INSTALL_PREFIX}/Common Files/LV2/${PROJECT_NAME}.lv2"
    )
    install (TARGETS ${PROJECT_NAME}-lv2-ui
        COMPONENT LV2Plugin
        LIBRARY DESTINATION "${CPACK_INSTALL_PREFIX}/Common Files/LV2/${PROJECT_NAME}.lv2"
    )

    install(FILES
        ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.lv2/manifest.ttl
        ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.lv2/${PROJECT_NAME}_dsp.ttl
        ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.lv2/${PROJECT_NAME}_ui.ttl
        COMPONENT LV2Plugin
        DESTINATION "${CPACK_INSTALL_PREFIX}/Common Files/LV2/${PROJECT_NAME}.lv2"
    )
endif ()

# Must include CPack again to apply install() COMPONENT settings.
include (CPack)
