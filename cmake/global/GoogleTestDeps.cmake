# We want GTest 1.10.0
set(GTEST_VERSION 1.10.0 CACHE STRING "Google test version")

# Force GTest to build MD and not MT
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(GOOGLE_TEST_GIT_REPO https://github.com/google/googletest.git)
set(GOOGLE_TEST_GIT_TAG release-${GTEST_VERSION})

# Fetch GTest
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 30.11)
  # Include FetchContent module
  include(FetchContent)

  # see https://cliutils.gitlab.io/modern-cmake/chapters/projects/fetch.html
  if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
  endif()
  
  FetchContent_Declare(googletest
                       GIT_REPOSITORY ${GOOGLE_TEST_GIT_REPO}
                       GIT_TAG ${GOOGLE_TEST_GIT_TAG})
          
  FetchContent_MakeAvailable(googletest)
else()
  # Include ExternalProject module
  include(ExternalProject)
  
  ExternalProject_Add(googletest
    GIT_REPOSITORY ${GOOGLE_TEST_GIT_REPO}
    GIT_TAG ${GOOGLE_TEST_GIT_TAG}
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND  ""
  )
endif()

