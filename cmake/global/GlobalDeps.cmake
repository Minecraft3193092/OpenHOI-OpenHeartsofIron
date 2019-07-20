find_package(Threads REQUIRED)


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_DEBUG_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)
if(WIN32)
    set(Boost_ARCHITECTURE "-x64")
    set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost)
endif()
find_package(Boost 1.65 REQUIRED COMPONENTS program_options log locale)


if(APPLE OR EXISTS ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/lib/libOgreMainStatic.a)
   set(OpenGL_GL_PREFERENCE GLVND)
   set(BUILD_OGRE_STATIC_LIB TRUE)
   add_definitions(-DBUILD_OGRE_STATIC_LIB)
else()
   set(BUILD_OGRE_STATIC_LIB FALSE)
endif()
if(WIN32 OR ${BUILD_OGRE_STATIC_LIB})
    set(OGRE_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d)
endif()
find_package(OGRE REQUIRED)
add_definitions(-DHAVE_OGRE_BITES)

if(${BUILD_OGRE_STATIC_LIB})
    find_package(Freetype REQUIRED)
    list(APPEND OGRE_LIBRARIES Freetype::Freetype)
endif()


if(NOT WIN32)
    set(FILESYSTEM_LIB stdc++fs)
endif()