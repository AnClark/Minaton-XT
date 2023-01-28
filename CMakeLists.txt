cmake_minimum_required (VERSION 3.20)

project (minaton
    VERSION 0.0.3
    LANGUAGES C CXX
)

add_subdirectory (dpf)

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
        plugin/MinatonParams.cpp
    FILES_UI
        plugin/MinatonUI.cpp
        plugin/MinatonUIHelper.cpp
        plugin/MinatonArtwork.cpp
)

target_include_directories (${PROJECT_NAME} PUBLIC 
    plugin/
    src/
    ${GENERATED_WAVES_DIR}
)

add_dependencies (${PROJECT_NAME}-dsp generate_waves)

# TODO: Use pkg-config to find out those two libraries
target_link_libraries (${PROJECT_NAME} PRIVATE samplerate sndfile speexdsp)
