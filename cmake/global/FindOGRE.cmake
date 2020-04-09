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
# - OGRE_NIX_MANUAL_BUILD : Flag, if we did an manualy OGRE build

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


set (FINDOGRE_VERSION 1.12.5)
if(${OGRE_NIX_PREBUILD})
    set(LIB_PREFIX lib)
    set(NON_PREFIX_SUFFIX .so.${FINDOGRE_VERSION}-openhoi)
    set(NON_PREFIX_SUFFIX_A .a) # TODO: We need to adjust the build process here in order to produce e.g. .a.1.12.5-openhoi
    #set(NON_PREFIX_SUFFIX_A .a.${FINDOGRE_VERSION}-openhoi)
    set(CMAKE_FIND_LIBRARY_SUFFIXES_BAK ${CMAKE_FIND_LIBRARY_SUFFIXES})
    list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ${NON_PREFIX_SUFFIX})
endif()

# Find library files
find_library(
    OGRE_MAIN_LIBRARY
    NAMES ${LIB_PREFIX}OgreMain${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    PATH_SUFFIXES .so.${FINDOGRE_VERSION}
    DOC "The OGRE OgreMain library")

find_library(
    OGRE_CODECSTBI_LIBRARY
    NAMES Codec_STBI${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    ${OGRE_ROOT_DIR}/lib/OGRE
    /usr/lib64
    /usr/lib64/OGRE
    /usr/lib
    /usr/lib/OGRE
    /usr/local/lib64
    /usr/local/lib64/OGRE
    /usr/local/lib
    /usr/local/lib/OGRE
    /sw/lib
    /opt/local/lib
    DOC "The OGRE Codec_STBI library")

find_library(
    OGRE_PARTICLEFX_LIBRARY
    NAMES Plugin_ParticleFX${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    ${OGRE_ROOT_DIR}/lib/OGRE
    /usr/lib64
    /usr/lib64/OGRE
    /usr/lib
    /usr/lib/OGRE
    /usr/local/lib64
    /usr/local/lib64/OGRE
    /usr/local/lib
    /usr/local/lib/OGRE
    /sw/lib
    /opt/local/lib
    DOC "The OGRE Plugin_ParticleFX library")

find_library(
    OGRE_OVERLAY_LIBRARY
    NAMES ${LIB_PREFIX}OgreOverlay${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
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
    NAMES ${LIB_PREFIX}OgreProperty${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreProperty library")

find_library(
    OGRE_RTSHADERSYSTEM_LIBRARY
    NAMES ${LIB_PREFIX}OgreRTShaderSystem${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The OGRE OgreRTShaderSystem library")

find_library(
    OGRE_GLSUPPORT_LIBRARY
    NAMES ${LIB_PREFIX}OgreGLSupport${NON_PREFIX_SUFFIX_A}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
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
    NAMES RenderSystem_GL3Plus${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    ${OGRE_ROOT_DIR}/lib/OGRE
    /usr/lib64
    /usr/lib64/OGRE
    /usr/lib
    /usr/lib/OGRE
    /usr/local/lib64
    /usr/local/lib64/OGRE
    /usr/local/lib
    /usr/local/lib/OGRE
    /sw/lib
    /opt/local/lib
    DOC "The OGRE RenderSystem_GL3Plus library")

find_library(
    OGRE_RENDERSYSTEMGL_LIBRARY
    NAMES RenderSystem_GL${NON_PREFIX_SUFFIX}
    PATHS
    $ENV{PROGRAMFILES}/lib
    ${OGRE_ROOT_DIR}/lib/macosx/Release
    ${OGRE_ROOT_DIR}/lib
    ${OGRE_ROOT_DIR}/lib/OGRE
    /usr/lib64
    /usr/lib64/OGRE
    /usr/lib
    /usr/lib/OGRE
    /usr/local/lib64
    /usr/local/lib64/OGRE
    /usr/local/lib
    /usr/local/lib/OGRE
    /sw/lib
    /opt/local/lib
    DOC "The OGRE RenderSystem_GL library")

if (WIN32)
    find_library(
        OGRE_MAIN_LIBRARY_DEBUG
        NAMES OgreMain_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE OgreMain library")

    find_library(
        OGRE_CODECSTBI_LIBRARY_DEBUG
        NAMES Codec_STBI_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        ${OGRE_ROOT_DIR}/lib/OGRE
        DOC "The DEBUG OGRE Codec_STBI library")

    find_library(
        OGRE_PARTICLEFX_LIBRARY_DEBUG
        NAMES Plugin_ParticleFX_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        ${OGRE_ROOT_DIR}/lib/OGRE
        DOC "The DEBUG OGRE Plugin_ParticleFX library")

    find_library(
        OGRE_OVERLAY_LIBRARY_DEBUG
        NAMES OgreOverlay_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE OgreOverlay library")

    find_library(
        OGRE_PROPERTY_LIBRARY_DEBUG
        NAMES OgreProperty_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE OgreProperty library")

    find_library(
        OGRE_RTSHADERSYSTEM_LIBRARY_DEBUG
        NAMES OgreRTShaderSystem_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE OgreRTShaderSystem library")

    find_library(
        OGRE_RENDERSYSTEMD3D11_LIBRARY_DEBUG
        NAMES RenderSystem_Direct3D11_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE RenderSystem_Direct3D11 library")
        
    find_library(
        OGRE_RENDERSYSTEMD3D11_LIBRARY
        NAMES RenderSystem_Direct3D11
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The OGRE RenderSystem_Direct3D11 library")
        
    find_library(
        OGRE_GLSUPPORT_LIBRARY_DEBUG
        NAMES OgreGLSupport_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE OgreGLSupport library")
       
    find_library(
        OGRE_RENDERSYSTEMGL3PLUS_LIBRARY_DEBUG
        NAMES RenderSystem_GL3Plus_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE RenderSystem_GL3Plus library")
        
    find_library(
        OGRE_RENDERSYSTEMGL_LIBRARY_DEBUG
        NAMES RenderSystem_GL_d
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib
        DOC "The DEBUG OGRE RenderSystem_GL library")
else()
    find_library(
        OGRE_CODECFREEIMAGE_LIBRARY
        NAMES Codec_FreeImage${NON_PREFIX_SUFFIX}
        PATHS
        $ENV{PROGRAMFILES}/lib
        ${OGRE_ROOT_DIR}/lib/macosx/Release
        ${OGRE_ROOT_DIR}/lib
        ${OGRE_ROOT_DIR}/lib/OGRE
        /usr/lib64
        /usr/lib64/OGRE
        /usr/lib
        /usr/lib/OGRE
        /usr/local/lib64
        /usr/local/lib64/OGRE
        /usr/local/lib
        /usr/local/lib/OGRE
        /sw/lib
        /opt/local/lib
        DOC "The OGRE Codec_FreeImage library")
endif()

# Handle REQUIRED argument, define *_FOUND variable
if (WIN32)
    find_package_handle_standard_args(OGRE DEFAULT_MSG OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_CODECSTBI_LIBRARY OGRE_PARTICLEFX_LIBRARY OGRE_GLSUPPORT_LIBRARY
                                      OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY OGRE_RTSHADERSYSTEM_LIBRARY OGRE_RENDERSYSTEMD3D11_LIBRARY OGRE_RENDERSYSTEMGL3PLUS_LIBRARY
                                      OGRE_RENDERSYSTEMGL_LIBRARY)
else()
    find_package_handle_standard_args(OGRE DEFAULT_MSG OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_GLSUPPORT_LIBRARY OGRE_CODECSTBI_LIBRARY OGRE_PARTICLEFX_LIBRARY
                                      OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY OGRE_RTSHADERSYSTEM_LIBRARY OGRE_RENDERSYSTEMGL3PLUS_LIBRARY OGRE_RENDERSYSTEMGL_LIBRARY
                                      OGRE_CODECFREEIMAGE_LIBRARY)
endif()

# Define OGRE_LIBRARIES and OGRE_INCLUDE_DIRS
if (OGRE_FOUND)
    if(WIN32 AND OGRE_RENDERSYSTEMD3D11_LIBRARY_DEBUG)
        list(APPEND OGRE_LIBRARIES debug ${OGRE_MAIN_LIBRARY_DEBUG} optimized ${OGRE_MAIN_LIBRARY}
                                   debug ${OGRE_CODECSTBI_LIBRARY_DEBUG} optimized ${OGRE_CODECSTBI_LIBRARY}
                                   debug ${OGRE_PARTICLEFX_LIBRARY_DEBUG} optimized ${OGRE_PARTICLEFX_LIBRARY}
                                   debug ${OGRE_OVERLAY_LIBRARY_DEBUG} optimized ${OGRE_OVERLAY_LIBRARY}
                                   debug ${OGRE_PROPERTY_LIBRARY_DEBUG} optimized ${OGRE_PROPERTY_LIBRARY}
                                   debug ${OGRE_RTSHADERSYSTEM_LIBRARY_DEBUG} optimized ${OGRE_RTSHADERSYSTEM_LIBRARY}
                                   debug ${OGRE_RENDERSYSTEMD3D11_LIBRARY_DEBUG} optimized ${OGRE_RENDERSYSTEMD3D11_LIBRARY}
                                   debug ${OGRE_RENDERSYSTEMGL3PLUS_LIBRARY_DEBUG} optimized ${OGRE_RENDERSYSTEMGL3PLUS_LIBRARY}
                                   debug ${OGRE_RENDERSYSTEMGL_LIBRARY_DEBUG} optimized ${OGRE_RENDERSYSTEMGL_LIBRARY}
                                   debug ${OGRE_GLSUPPORT_LIBRARY_DEBUG} optimized ${OGRE_GLSUPPORT_LIBRARY})
    else()
        list(APPEND OGRE_LIBRARIES ${OGRE_MAIN_LIBRARY} ${OGRE_CODECSTBI_LIBRARY} ${OGRE_PARTICLEFX_LIBRARY} ${OGRE_OVERLAY_LIBRARY} ${OGRE_PROPERTY_LIBRARY}
                                   ${OGRE_RTSHADERSYSTEM_LIBRARY} ${OGRE_GLSUPPORT_LIBRARY} ${OGRE_RENDERSYSTEMGL3PLUS_LIBRARY} ${OGRE_RENDERSYSTEMGL_LIBRARY})
        if(WIN32)
            list(APPEND OGRE_LIBRARIES ${OGRE_RENDERSYSTEMD3D11_LIBRARY})
        else()
            list(APPEND OGRE_LIBRARIES ${OGRE_CODECFREEIMAGE_LIBRARY})
        endif()
    endif()
    
    
	set(OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR} ${OGRE_INCLUDE_DIR}/MeshLodGenerator ${OGRE_INCLUDE_DIR}/Overlay
                          ${OGRE_INCLUDE_DIR}/Property ${OGRE_INCLUDE_DIR}/RTShaderSystem ${OGRE_INCLUDE_DIR}/Threading ${OGRE_INCLUDE_DIR}/Plugins/ParticleFX
                          ${OGRE_INCLUDE_DIR}/Plugins/STBICodec ${OGRE_INCLUDE_DIR}/RenderSystems/GL3Plus ${OGRE_INCLUDE_DIR}/RenderSystems/GL)
    if(WIN32)
        list(APPEND OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/RenderSystems/Direct3D11)
    endif()
endif()

# Hide some variables
mark_as_advanced(OGRE_INCLUDE_DIR OGRE_MAIN_LIBRARY OGRE_GLSUPPORT_LIBRARY OGRE_CODECSTBI_LIBRARY OGRE_OVERLAY_LIBRARY OGRE_PROPERTY_LIBRARY
                 OGRE_RTSHADERSYSTEM_LIBRARY OGRE_RENDERSYSTEMGL3PLUS_LIBRARY OGRE_RENDERSYSTEMD3D11_LIBRARY OGRE_MAIN_LIBRARY_DEBUG OGRE_PARTICLEFX_LIBRARY
                 OGRE_GLSUPPORT_LIBRARY_DEBUG OGRE_BITES_LIBRARY_DEBUG OGRE_CODECSTBI_LIBRARY_DEBUG OGRE_MESHLODGENERATOR_LIBRARY_DEBUG
                 OGRE_OVERLAY_LIBRARY_DEBUG OGRE_PROPERTY_LIBRARY_DEBUG OGRE_RTSHADERSYSTEM_LIBRARY_DEBUG OGRE_RENDERSYSTEMD3D11_LIBRARY_DEBUG OGRE_CODECFREEIMAGE_LIBRARY
                 OGRE_RENDERSYSTEMGL3PLUS_LIBRARY_DEBUG OGRE_RENDERSYSTEMGL_LIBRARY OGRE_RENDERSYSTEMGL_LIBRARY_DEBUG OGRE_PARTICLEFX_LIBRARY_DEBUG)

# Reset suffixes
if(CMAKE_FIND_LIBRARY_SUFFIXES_BAK)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_BAK})
endif()
