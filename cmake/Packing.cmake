# Packaging configurations
#
# Some usages is referenced from:
# - https://github.com/retifrav/cmake-cpack-example/

set (CPACK_PACKAGE_NAME ${PROJECT_NAME})

set (CPACK_NSIS_DISPLAY_NAME "Minaton ${PROJECT_VERSION}")

set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set (CPACK_PACKAGE_VENDOR "ThunderOx, AnClark Liu")

set (CPACK_VERBATIM_VARIABLES YES)

set (CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set (CPACK_OUTPUT_FILE_PREFIX "${PROJECT_BINARY_DIR}")
set (CPACK_STRIP_FILES YES)

set (
    CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

# Specify where CPack should run "install" recipes to install files into
# This is not directly used by CPack.
# NOTICE: CPack will automatically handle the actual install path. Simply set it to ".".
set (CPACK_INSTALL_PREFIX .)

# Specify where package installers should install files into
# Not to be confused with CPACK_INSTALL_PREFIX!
set (CPACK_PACKAGING_INSTALL_PREFIX .)

set (CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set (CPACK_PACKAGE_CONTACT "clarklaw4701@qq.com")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER "AnClark Liu <${CPACK_PACKAGE_CONTACT}>")

set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# package name for deb. If set, then instead of some-application-0.9.2-Linux.deb
# you'll get some-application_0.9.2_amd64.deb (note the underscores too)
set (CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# that is if you want every group to have its own package,
# although the same will happen if this is not set (so it defaults to ONE_PER_GROUP)
# and CPACK_DEB_COMPONENT_INSTALL is set to YES
set (CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)#ONE_PER_GROUP)
# without this you won't be able to pack only specified component
set (CPACK_DEB_COMPONENT_INSTALL YES)
# list dependencies
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

set (CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_SOURCE_DIR}/.git
    ${PROJECT_SOURCE_DIR}/.cache
    ${PROJECT_SOURCE_DIR}/build*
)

set (CPACK_IGNORE_FILES
    ${PROJECT_SOURCE_DIR}/.git
    ${PROJECT_SOURCE_DIR}/.cache
    ${PROJECT_SOURCE_DIR}/build*
)

# On NSIS generator, prefer CPACK_NSIS_MODIFY_PATH rather than CPACK_SET_DESTDIR
set (CPACK_NSIS_MODIFY_PATH ON)
# set(CPACK_SET_DESTDIR ON)

# Bypass unspecified components (libsndfile, etc.)
# They are not required by Minaton plugins, since they have been statically linked.
# On the contrary, plugin binaries should explicitly specify COMPONENT in install() command.
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "FALSE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "FALSE")
