#
# Find OGRE
#
# Try to find OGRE libraries.
# This module defines the following variables:
# - OGRE_INCLUDE_DIRS
# - OGRE_LIBRARIES
# - OGRE_FOUND
#
# The following variables can be set as arguments for the module.
# - OGRE_ROOT_DIR : Root library directory of OGRE
# - BUILD_OGRE_STATIC_LIB : Flag if we want to use static build of OGRE

# Additional modules
include(FindPackageHandleStandardArgs)

# Find include files
find_path(
    OGRE_INCLUDE_DIR
    NAMES Ogre.h
    PATHS
    $ENV{PROGRAMFILES}/include
    ${OGRE_ROOT_DIR}/include
    /usr/include
    /usr/local/include
    /sw/include
    /opt/local/include
    PATH_SUFFIXES OGRE
    DOC "The directory where Ogre.h resides")


if(BUILD_OGRE_STATIC_LIB)
    set(OGRE_LIB_PREFIX Static)
endif()

# Find library files
find_library(
    OGRE_MAIN_LIBRARY
    NAMES OgreMain${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreMain library")

find_library(
    OGRE_HLMS_LIBRARY
    NAMES OgreHLMS${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreHLMS library")

find_library(
    OGRE_BITES_LIBRARY
    NAMES OgreBites${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreBites library")

find_library(
    OGRE_CODECSTBI_LIBRARY
    NAMES Codec_STBI${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE Codec_STBI library")

find_library(
    OGRE_MESHLODGENERATOR_LIBRARY
    NAMES OgreMeshLodGenerator${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreMeshLodGenerator library")

find_library(
    OGRE_OVERLAY_LIBRARY
    NAMES OgreOverlay${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreOverlay library")

find_library(
    OGRE_PROPERTY_LIBRARY
    NAMES OgreProperty${OGRE_LIB_PREFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreProperty library")

if (WIN32)
    find_library(
        OGRE_RENDERSYSTEMD3D11_LIBRARY
        NAMES RenderSystem_Direct3D11${OGRE_LIB_PREFIX}
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The OGRE RenderSystem_Direct3D11 library")
else()
    find_library(
        OGRE_GLSUPPORT_LIBRARY
        NAMES OgreGLSupport${OGRE_LIB_PREFIX}
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The OGRE OgreGLSupport library")

    find_library(
        OGRE_RENDERSYSTEMGL3PLUS_LIBRARY
        NAMES RenderSystem_GL3Plus${OGRE_LIB_PREFIX}
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The OGRE RenderSystem_GL3Plus library")
endif()

# Handle REQUIRED argument, define *_FOUND variable
if (WIN32)
    find_package_handle_standard_args(OGRE DEFAULT_MSG OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_HLMS_LIBRARY OGRE_BITES_LIBRARY OGRE_CODECSTBI_LIBRARY
                                      OGRE_MESHLODGENERATOR_LIBRARY OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY OGRE_RENDERSYSTEMD3D11_LIBRARY)
else()
    find_package_handle_standard_args(OGRE DEFAULT_MSG OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_HLMS_LIBRARY OGRE_GLSUPPORT_LIBRARY OGRE_BITES_LIBRARY OGRE_CODECSTBI_LIBRARY
                                      OGRE_MESHLODGENERATOR_LIBRARY OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY OGRE_RENDERSYSTEMGL3PLUS_LIBRARY)
endif()

# Define OGRE_LIBRARIES and OGRE_INCLUDE_DIRS
if (OGRE_FOUND)
	list(APPEND OGRE_LIBRARIES ${OGRE_MAIN_LIBRARY} ${OGRE_HLMS_LIBRARY} ${OGRE_GLSUPPORT_LIBRARY} ${OGRE_BITES_LIBRARY} ${OGRE_CODECSTBI_LIBRARY} ${OGRE_MESHLODGENERATOR_LIBRARY}
                               ${OGRE_OVERLAY_LIBRARY} ${OGRE_PROPERTY_LIBRARY} ${OGRE_RENDERSYSTEMGL3PLUS_LIBRARY} ${OGRE_RENDERSYSTEMD3D11_LIBRARY})
	set(OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_HLMS_LIBRARY OGRE_GLSUPPORT_LIBRARY OGRE_BITES_LIBRARY OGRE_CODECSTBI_LIBRARY OGRE_MESHLODGENERATOR_LIBRARY
                 OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY OGRE_RENDERSYSTEMGL3PLUS_LIBRARY OGRE_RENDERSYSTEMD3D11_LIBRARY)
