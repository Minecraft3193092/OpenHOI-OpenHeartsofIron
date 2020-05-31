# Setup project details
project(hoibase_tests
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi base library tests")


# Find required dependencies
include(GlobalDeps)
include(GoogleTest)


# Add map tests
list(APPEND MAP_TESTS map/province.cpp)
source_group("Test Files\\map" FILES ${MAP_TESTS})
set(TEST_SOURCES ${TEST_SOURCES} ${MAP_TESTS})


# Create test executable
add_executable(hoibase_test
               ${TEST_SOURCES})

target_link_libraries(hoibase_test
                      hoibase
                      gtest_main)

target_include_directories(hoibase_test
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>)
target_include_directories(hoibase_test SYSTEM
    PRIVATE
        ${OGRE_INCLUDE_DIRS})


# Discover tests
gtest_discover_tests(hoibase_test)