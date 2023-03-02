# Different plugin formats should be splitted into different packages
cpack_add_component (VST2Plugin
    DISPLAY_NAME "VST2 Plugin"
    DESCRIPTION "Install VST 2.4 version of Minaton."
)
cpack_add_component (VST3Plugin
    DISPLAY_NAME "VST3 Plugin"
    DESCRIPTION "Install VST3 version of Minaton."
)
cpack_add_component (CLAPPlugin
    DISPLAY_NAME "CLAP Plugin"
    DESCRIPTION "Install CLAP version of Minaton."
)
cpack_add_component (LV2Plugin
    DISPLAY_NAME "LV2 Plugin"
    DESCRIPTION "Install LV2 version of Minaton.\nNOTICE: On Windows, only few hosts (Ardour, Harrison Mixbus, REAPER) supports this format."
)

# Exclude unspecified components (libsndfile, etc.)
# They are not required by Minaton plugins, since they have been statically linked.
# On the contrary, plugin binaries should explicitly specify COMPONENT in install() command.
set (CPACK_COMPONENTS_ALL
    VST2Plugin
    VST3Plugin
    CLAPPlugin
    LV2Plugin
)
