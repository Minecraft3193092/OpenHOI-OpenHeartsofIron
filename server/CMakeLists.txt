# Setup project details
project(server
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi server executable")


# Find required dependencies
include(GlobalDeps)


# Add main code
list(APPEND SERVER_SOURCES src/server.cpp)
source_group("Source Files" FILES ${SERVER_SOURCES})


# Create executable
if(WIN32)
    set(WINDOWS_RESOURCE_PATH ${CMAKE_SOURCE_DIR}/winicon.rc)
endif()
add_executable(server
    ${SERVER_SOURCES}
    ${WINDOWS_RESOURCE_PATH})

set_target_properties(server PROPERTIES OUTPUT_NAME "openhoi-server")

target_link_libraries(server hoibase
                             ${FILESYSTEM_LIB}
                             Boost::dynamic_linking Boost::disable_autolinking Boost::program_options Boost::locale
                             Threads::Threads)

target_include_directories(server
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>)
target_include_directories(server SYSTEM
    PRIVATE
        ${OGRE_INCLUDE_DIRS})


# Set C++ standard
target_compile_features(server PRIVATE cxx_std_17)

# Set error level
target_compile_options(server PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wc++17-compat-pedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>)