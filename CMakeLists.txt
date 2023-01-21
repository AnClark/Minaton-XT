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

find_package(Python3 REQUIRED)

set (SOURCES_WAVES
    ${GENERATED_WAVES_DIR}/minaton_waves.cpp
    ${GENERATED_WAVES_DIR}/minaton_waves.h
)

add_custom_command (
    OUTPUT ${SOURCES_WAVES}
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/res2c_mod.py minaton_waves wave_ ${PROJECT_SOURCE_DIR}/src/waves ${GENERATED_WAVES_DIR}
    VERBATIM
)

add_library (${PROJECT_NAME}_waves STATIC ${SOURCES_WAVES})


#
# Build plug-ins
#

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 lv2 clap
    FILES_DSP
        src/synth.cpp
        src/synth_dpf.cpp
        plugin/MinatonPlugin.cpp
        plugin/MinatonProcess.cpp
        plugin/MinatonParams.cpp
)

target_include_directories (${PROJECT_NAME} PUBLIC 
    plugin/
    src/
    ${GENERATED_WAVES_DIR}
)

target_link_libraries (${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_waves)

# TODO: Use pkg-config to find out those two libraries
target_link_libraries (${PROJECT_NAME} PRIVATE samplerate sndfile)
