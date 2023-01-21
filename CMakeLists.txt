cmake_minimum_required (VERSION 3.20)

project (minaton
    VERSION 0.0.3
    LANGUAGES C CXX
)

add_subdirectory (dpf)

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 lv2 clap
    FILES_DSP
        src/synth.cpp
        plugin/MinatonPlugin.cpp
        plugin/MinatonProcess.cpp
        plugin/MinatonParams.cpp
)

target_include_directories (${PROJECT_NAME} PUBLIC 
    plugin/
    src/
)

# TODO: Use pkg-config to find out those two libraries
target_link_libraries (${PROJECT_NAME} PRIVATE samplerate sndfile)
