# We want GTest 1.10.0
set(GTEST_VERSION 1.10.0 CACHE STRING "Google test version")

# Force GTest to build MD and not MT
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(GOOGLE_TEST_GIT_REPO https://github.com/google/googletest.git)
set(GOOGLE_TEST_GIT_TAG release-${GTEST_VERSION})

# Fetch GTest
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.11)
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
    INSTALL_COMMAND """"
  )
  
  ExternalProject_Get_Property(googletest binary_dir)
  
  add_library(gtest UNKNOWN IMPORTED)
  add_library(gtest_main UNKNOWN IMPORTED)
  
  set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION ${binary_dir}/libgtest.a
  )
  set_target_properties(gtest_main PROPERTIES
      IMPORTED_LOCATION ${binary_dir}/libgtest_main.a
  )
  
  add_dependencies(gtest googletest)
  add_dependencies(gtest_main googletest)
endif()

