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
    set(RAPIDJSON_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/rapidjson)
endif()
find_package(RapidJSON REQUIRED)

if (EXISTS ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/cgal/lib)
    set(MANUAL_CGAL_BUILD TRUE)
else()
    set(MANUAL_CGAL_BUILD FALSE)
endif()
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
        set(CGAL_LIBRARY optimized ${CGAL_LIBRARIES_DIR}/libCGAL-vc140-mt-${CGAL_VERSION}.lib debug ${CGAL_LIBRARIES_DIR}/libCGAL-vc140-mt-gd-${CGAL_VERSION}.lib)
    endif()
    string(REPLACE ";" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CGAL_DEBUG_CFLAGS}")
    string(REPLACE ";" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CGAL_DEBUG_CFLAGS}")
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

if(WIN32 AND NOT ENV{V8_HOME})
    set(ENV{V8_HOME} ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8)
endif()
find_package(V8 REQUIRED)


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
                           src/helper/unique_id.cpp)
source_group("Source Files\\helper" FILES ${HELPER_SOURCES})
set(BASE_SOURCES ${BASE_SOURCES} ${HELPER_SOURCES})

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
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

add_library(hoibase SHARED
    ${BASE_INCLUDES} ${BASE_SOURCES})

target_link_libraries(hoibase
                      ${FILESYSTEM_LIB}
                      Boost::dynamic_linking Boost::disable_autolinking
                      OpenSSL::SSL OpenSSL::Crypto
                      ZLIB::ZLIB
                      ${OGRE_LIBRARIES}
                      ${CGAL_IMPORTED_TARGET}
                      ${LIBUUID_IMPORTED_TARGET}
                      ${V8_LIBRARIES})

target_include_directories(hoibase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
        ${OGRE_INCLUDE_DIRS}
        ${CGAL_INCLUDE_DIRS_HOI}
        ${GMP_INCLUDE_DIR}
        ${MPFR_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${RAPIDJSON_INCLUDES}
        ${V8_INCLUDE_DIR})


# Copy hoibase windows dependencies to output directories
if(WIN32)
    function(copy_hoibase_dll target ogre_suffix)
        add_custom_command(TARGET hoibase POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_atomic-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_chrono-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_date_time-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_filesystem-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_locale-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_program_options-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_regex-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_system-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/boost/bin/boost_thread-vc141-mt-x64-1_69.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/bin/libgmp-10.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/gmp/bin/libmpfr-4.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/Codec_STBI${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreBites${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreHLMS${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreMain${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreMeshLodGenerator${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreOverlay${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreProperty${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/OgreRTShaderSystem${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/RenderSystem_Direct3D11${ogre_suffix}" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/ogre3d/bin/SDL2.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openssl/bin/libcrypto-1_1-x64.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openssl/bin/libssl-1_1-x64.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/icudtl.dat" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/icui18n.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/icuuc.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/natives_blob.bin" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/snapshot_blob.bin" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/v8.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/v8_libbase.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/v8/bin/v8_libplatform.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/zlib/bin/zlib.dll" "${CMAKE_BINARY_DIR}/output/${target}/"
                           VERBATIM)
    endfunction()
    copy_hoibase_dll(Debug _d.dll)
    copy_hoibase_dll(RelWithDebInfo .dll)
endif()


# Set error level
target_compile_options(hoibase PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wc++17-compat-pedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>)
