# Define minimum required CMake version
cmake_minimum_required (VERSION 3.10 FATAL_ERROR)


# Setup project details
set (OPENHOI_VERSION_MAJOR 0)
set (OPENHOI_VERSION_MINOR 0)
set (OPENHOI_VERSION_PATCH 1)
project(openhoi
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi solution")
configure_file(
    ${PROJECT_SOURCE_DIR}/template/version.hpp.in
    ${PROJECT_BINARY_DIR}/generated/version.hpp
)


# Setup compiler/linker flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_STANDARD_LIBRARIES "-ldl")
endif()



# Make sure that our custom CMake modules are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/global)



# Set output directories for Windows and limit configuration types
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
    if(CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "" FORCE) 
    else()
        if(NOT CMAKE_BUILD_TYPE)
            message("Defaulting to release build.")
            set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
        endif()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;RelWithDebInfo")
    endif()
endif()


# Add a "test" target
enable_testing()


# Add game base library
add_subdirectory(hoibase)


# Add game client executable
add_subdirectory(client)


# Add game server executable
add_subdirectory(server)
