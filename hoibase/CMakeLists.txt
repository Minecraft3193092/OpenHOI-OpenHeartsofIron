# Setup project details
project(hoibase
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi base library")


# Find required dependencies
include(GlobalDeps)

if(WIN32)
    set(OPENSSL_MSVC_STATIC_RT TRUE)
    set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openssl)
elseif(APPLE AND NOT OPENSSL_ROOT_DIR)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()
find_package(OpenSSL 1.0.2 REQUIRED)

if(WIN32)
    cmake_policy(SET CMP0074 OLD)
    set(ZLIB_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/zlib)
endif()
find_package(ZLIB REQUIRED)
if(WIN32)
    cmake_policy(SET CMP0074 NEW)
endif()

if(APPLE)
    find_library(CFLIB CoreFoundation)
elseif(NOT WIN32)
    find_package(Libuuid REQUIRED)
     set(LIBUUID_IMPORTED_TARGET LibUUID::UUID)
endif()

if(WIN32)
    set(LUA_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/lua/lib/lua.lib)
    set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/lua/include)
else()
    find_package(Lua 5.3 REQUIRED)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal/lib)
    set(MANUAL_CGAL_BUILD TRUE)
else()
    set(MANUAL_CGAL_BUILD FALSE)
endif()
set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
if(${MANUAL_CGAL_BUILD})
    if(WIN32)
        if(NOT GMP_INCLUDE_DIR)
            set(GMP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/include)
        endif()
        if(NOT GMP_LIBRARIES)
            set(GMP_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib)
        endif()
        if(NOT MPFR_INCLUDE_DIR)
            set(MPFR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/include)
        endif()
        if(NOT MPFR_LIBRARIES)
            set(MPFR_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib)
        endif()
    endif()
    if(NOT CGAL_ROOT)
        set(CGAL_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal)
    endif()
    if(NOT CGAL_LIBRARIES_DIR)
        set(CGAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal/lib)
    endif()
    list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/cgal)
else()
    set(CGAL_VERSION 4.11)
endif()
find_package(CGAL ${CGAL_VERSION} REQUIRED)
if(${MANUAL_CGAL_BUILD})
    set(CGAL_VERSION "${CGAL_VERSION_MAJOR}.${CGAL_VERSION_MINOR}")
    if(WIN32 AND NOT CGAL_LIBRARY)
        set(CGAL_LIBRARY optimized ${CGAL_LIBRARIES_DIR}/CGAL-vc142-mt-${CGAL_VERSION}.lib debug ${CGAL_LIBRARIES_DIR}/CGAL-vc142-mt-gd-${CGAL_VERSION}.lib)
    endif()
    string(REPLACE ";" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CGAL_DEBUG_CFLAGS}")
    string(REPLACE ";" " " CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CGAL_RELEASE_CFLAGS}")
    add_definitions(${CGAL_3RD_PARTY_DEFINITIONS} ${CGAL_Core_3RD_PARTY_DEFINITIONS})
    if(NOT CGAL_3RD_PARTY_LIBRARIES)
        list(APPEND CGAL_3RD_PARTY_LIBRARIES ${GMP_LIBRARIES} ${MPFR_LIBRARIES})
    endif()
    list(APPEND CGAL_IMPORTED_TARGET ${CGAL_LIBRARY} ${CGAL_3RD_PARTY_LIBRARIES} ${CGAL_Core_LIBRARY} ${CGAL_Core_3RD_PARTY_LIBRARIES})
    list(APPEND CGAL_INCLUDE_DIRS_HOI ${CGAL_INCLUDE_DIRS} ${CGAL_Core_INCLUDE_DIRS})
    if (UNIX AND NOT APPLE)
        list(APPEND CGAL_IMPORTED_TARGET gmp)
    endif()
else()
    set(CGAL_IMPORTED_TARGET CGAL::CGAL)
endif()


# openhoi.hpp
list(APPEND HOIBASE_INCLUDES include/hoibase/openhoi.hpp)
source_group("Header Files" FILES ${HOIBASE_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${HOIBASE_INCLUDES})

# Add file access code
list(APPEND FILE_INCLUDES include/hoibase/file/file_access.hpp
                          include/hoibase/file/filesystem.hpp)
source_group("Header Files\\file" FILES ${FILE_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${FILE_INCLUDES})

list(APPEND FILE_SOURCES src/file/file_access.cpp)
source_group("Source Files\\file" FILES ${FILE_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${FILE_SOURCES})

# Add helper code
list(APPEND HELPER_INCLUDES include/hoibase/helper/debug.hpp
                            include/hoibase/helper/library.hpp
                            include/hoibase/helper/os.hpp
                            include/hoibase/helper/synchronization.hpp
                            include/hoibase/helper/unique_id.hpp)
source_group("Header Files\\helper" FILES ${HELPER_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${HELPER_INCLUDES})

list(APPEND HELPER_SOURCES src/helper/debug.cpp
                           src/helper/os.cpp
                           src/helper/unique_id.cpp)
source_group("Source Files\\helper" FILES ${HELPER_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${HELPER_SOURCES})

# Add log code
list(APPEND LOG_INCLUDES include/hoibase/log/log_handler.hpp
                         include/hoibase/log/log_listener.hpp)
source_group("Header Files\\log" FILES ${LOG_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${LOG_INCLUDES})

list(APPEND LOG_SOURCES src/log/log_listener.cpp)
source_group("Source Files\\log" FILES ${LOG_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${LOG_SOURCES})

# Add map code
list(APPEND MAP_INCLUDES include/hoibase/map/map_factory.hpp
                         include/hoibase/map/map.hpp
                         include/hoibase/map/province.hpp)
source_group("Header Files\\map" FILES ${MAP_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${MAP_INCLUDES})

list(APPEND MAP_SOURCES src/map/map_factory.cpp
                           src/map/map.cpp
                           src/map/province.cpp)
source_group("Source Files\\map" FILES ${MAP_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${MAP_SOURCES})

# Add scripting code
list(APPEND SCRIPTING_INCLUDES include/hoibase/scripting/scripting_runtime.hpp)
source_group("Header Files\\scripting" FILES ${SCRIPTING_INCLUDES})
set(BASE_INCLUDES ${BASE_INCLUDES} ${SCRIPTING_INCLUDES})

list(APPEND SCRIPTING_SOURCES src/scripting/scripting_runtime.cpp)
source_group("Source Files\\scripting" FILES ${SCRIPTING_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${SCRIPTING_SOURCES})

# Create library
add_library(hoibase SHARED
    ${BASE_INCLUDES} ${BASE_SOURCES})

target_link_libraries(hoibase
                      ${FILESYSTEM_LIB}
                      Boost::dynamic_linking Boost::disable_autolinking
                      OpenSSL::SSL OpenSSL::Crypto
                      ZLIB::ZLIB
                      ${OGRE_LIBRARIES}
                      ${CGAL_IMPORTED_TARGET}
                      ${LUA_LIBRARIES}
                      ${LIBUUID_IMPORTED_TARGET}
                      ${OS_LIBRARIES})

target_include_directories(hoibase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>)
target_include_directories(hoibase SYSTEM
    PRIVATE
        ${OGRE_INCLUDE_DIRS}
        ${CGAL_INCLUDE_DIRS_HOI}
        ${LUA_INCLUDE_DIR}
        ${GMP_INCLUDE_DIR}
        ${MPFR_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${RAPIDJSON_INCLUDES})


# Copy hoibase windows dependencies to output directories
if(WIN32)
    if(NOT CMAKE_MAKE_PROGRAM MATCHES ninja)
        set(WIN32_DEP_OUT_DIR "${CMAKE_BINARY_DIR}/output/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/")
    else()
        set(WIN32_DEP_OUT_DIR "${CMAKE_BINARY_DIR}/output/")
    endif()
    add_custom_command(TARGET hoibase POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/Codec_STBI$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreHLMS$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreMain$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreOverlay$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreProperty$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreRTShaderSystem$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/Plugin_ParticleFX$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/RenderSystem_Direct3D11$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/RenderSystem_GL3Plus$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/RenderSystem_GL$<$<CONFIG:Debug>:_d>.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_atomic-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_chrono-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_date_time-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_filesystem-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_locale-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_program_options-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_regex-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_system-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_thread-vc141-mt-x64-1_69.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/bin/libgmp-10.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/bin/libmpfr-4.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal/bin/CGAL_Core-vc142-mt-$<$<CONFIG:Debug>:gd->5.0.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal/bin/CGAL-vc142-mt-$<$<CONFIG:Debug>:gd->5.0.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/sdl2/bin/SDL2.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openssl/bin/libcrypto-1_1-x64.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openssl/bin/libssl-1_1-x64.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/zlib/bin/zlib.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openal/bin/OpenAL32.dll" "${WIN32_DEP_OUT_DIR}"
                       COMMAND "${CMAKE_SOURCE_DIR}/cmake/win64/copy-pdb.bat" "${CMAKE_SOURCE_DIR}" "${WIN32_DEP_OUT_DIR}" $<CONFIG:Debug>
                       VERBATIM)
endif()


# Set error level
target_compile_options(hoibase PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wc++17-compat-pedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>)