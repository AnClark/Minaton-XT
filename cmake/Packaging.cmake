# Installation and CPack packaging.

if(APPLE)
    # Install plugins where macOS audio hosts look for them.  PATTERN
    # "param.dat" EXCLUDE keeps any build-tree copy/symlink out of the
    # bundles: the single shared copy goes to Application Support below
    # instead (found by loadBankFromDisk()'s system-wide candidate).
    install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.vst/"
        DESTINATION "Library/Audio/Plug-Ins/VST/${PROJECT_NAME}.vst"
        COMPONENT VST2)
    install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.vst3/"
        DESTINATION "Library/Audio/Plug-Ins/VST3/${PROJECT_NAME}.vst3"
        COMPONENT VST3)
    install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.clap/"
        DESTINATION "Library/Audio/Plug-Ins/CLAP/${PROJECT_NAME}.clap"
        COMPONENT CLAP)
    if(TARGET_LV2)
        install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.lv2/"
            DESTINATION "Library/Audio/Plug-Ins/LV2/${PROJECT_NAME}.lv2"
            COMPONENT LV2)
    endif()
else()
    # On Windows and Linux, only VST3 format is put into a directory,
    # while VST 2.4 and CLAP are single file.
    install(FILES "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}-vst2${CMAKE_SHARED_LIBRARY_SUFFIX}"
        DESTINATION ".")
    install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.vst3/"
        DESTINATION "${PROJECT_NAME}.vst3"
        PATTERN "param.dat" EXCLUDE)
    install(FILES "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.clap"
        DESTINATION ".")
    if(TARGET_LV2)
        install(DIRECTORY "${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.lv2/"
            DESTINATION "${PROJECT_NAME}.lv2"
            PATTERN "param.dat" EXCLUDE)
    endif()
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR ${AUTHOR})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${INSTALL_DIRECTORY}")

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/CPackWelcome.txt.in"
    "${PROJECT_BINARY_DIR}/CPackWelcome.txt"
    @ONLY)

execute_process(
    COMMAND git rev-parse --short=8 HEAD
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE PLUGIN_GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
if(NOT PLUGIN_GIT_COMMIT)
    set(PLUGIN_GIT_COMMIT "unknown")
endif()

if(APPLE)
    set(CPACK_GENERATOR "productbuild")
    set(CPACK_COMPONENTS_ALL VST2 VST3 CLAP)
    if(TARGET_LV2)
        list(APPEND CPACK_COMPONENTS_ALL LV2)
    endif()
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
    set(CPACK_PRODUCTBUILD_IDENTIFIER ${PRODUCTBUILD_IDENTIFIER})
    set(CPACK_PRODUCTBUILD_DOMAINS ON)
    set(CPACK_PRODUCTBUILD_DOMAINS_ANYWHERE OFF)
    set(CPACK_PRODUCTBUILD_DOMAINS_USER ON)
    set(CPACK_PRODUCTBUILD_DOMAINS_ROOT ON)
    set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_BINARY_DIR}/CPackWelcome.txt")
    set(CPACK_PACKAGE_FILE_NAME
        "${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-macOS-${PROJECT_VERSION}-${PLUGIN_GIT_COMMIT}")
elseif(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME
        "${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Windows-${PROJECT_VERSION}-${PLUGIN_GIT_COMMIT}")
else()
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME
        "${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Linux-${PROJECT_VERSION}-${PLUGIN_GIT_COMMIT}")
endif()

# README / License stuff
if(APPLE)
    # On macOS, Integrate README and license into install wizard.
    # CPack only allows 4 formats. Convert them first.
    configure_file("${PROJECT_SOURCE_DIR}/README.md" "${PROJECT_BINARY_DIR}/CPackREADME.txt" COPYONLY)
    configure_file ("${PROJECT_SOURCE_DIR}/LICENSE" "${PROJECT_BINARY_DIR}/CPackLICENSE.txt" COPYONLY)

    set(CPACK_RESOURCE_FILE_README "${PROJECT_BINARY_DIR}/CPackREADME.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/CPackLICENSE.txt")
else()
    # On Windows / Linux, no installer provided. Put them into Zip file
    install(FILES
        "${PROJECT_SOURCE_DIR}/README.md"
        "${PROJECT_SOURCE_DIR}/LICENSE"
        DESTINATION "."
    )
endif()

include(CPack)
