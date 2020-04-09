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
  # Legacy process:
  # Include ExternalProject module
  configure_file(${PROJECT_SOURCE_DIR}/cmake/global/GoogleTestCMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)
endif()

