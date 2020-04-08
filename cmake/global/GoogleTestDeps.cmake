# We want GTest 1.10.0
set(GTEST_VERSION 1.10.0 CACHE STRING "Google test version")

# Force GTest to build MD and not MT
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# see https://cliutils.gitlab.io/modern-cmake/chapters/projects/fetch.html
if(${CMAKE_VERSION} VERSION_LESS 3.14)
  macro(FetchContent_MakeAvailable NAME)
      FetchContent_GetProperties(${NAME})
      if(NOT ${NAME}_POPULATED)
          FetchContent_Populate(${NAME})
          add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
      endif()
  endmacro()
else()
  # Include FetchContent module
  include(FetchContent)
endif()


# Fetch GTest
FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest.git
                     GIT_TAG release-${GTEST_VERSION})
        
FetchContent_MakeAvailable(googletest)