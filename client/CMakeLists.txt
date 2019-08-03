# Setup project details
project(client
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi client executable")


# Find required dependencies
include(GlobalDeps)

if(WIN32)
    set(ENV{OPENALDIR} ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/openal)
endif()
find_package(OpenAL REQUIRED)

if(WIN32)
    set(SDL2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/sdl2/include/SDL2)
    list(APPEND SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/sdl2/lib/SDL2.lib
                               ${CMAKE_SOURCE_DIR}/thirdparty/manual-build/precompiled/sdl2/lib/SDL2main.lib)
else()
    find_package(SDL2 REQUIRED)
endif()


# Add main code
list(APPEND CLIENT_INCLUDES include/game_manager_base.hpp
                            include/game_manager.hpp
                            include/options.hpp)
source_group("Header Files" FILES ${RENDERER_INCLUDES})
list(APPEND CLIENT_SOURCES src/client.cpp
                           src/game_manager_base.cpp
                           src/game_manager.cpp
                           src/options.cpp)
source_group("Source Files" FILES ${CLIENT_SOURCES})

# Add state code
list(APPEND STATE_INCLUDES include/state/menu_state.hpp
                           include/state/state_manager.hpp
                           include/state/state.hpp)
source_group("Header Files\\state" FILES ${STATE_INCLUDES})
list(APPEND CLIENT_INCLUDES ${STATE_INCLUDES})

list(APPEND STATE_SOURCES src/state/menu_state.cpp
                          src/state/state_manager.cpp
                          src/state/state.cpp)
source_group("Source Files\\state" FILES ${STATE_SOURCES})
list(APPEND CLIENT_SOURCES ${STATE_SOURCES})

# Add third-party library ImGui
list(APPEND IMGUI_INCLUDES ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imconfig.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_internal.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_rectpack.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_textedit.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_truetype.h)
source_group("Header Files\\thirdparty\\imgui" FILES ${IMGUI_INCLUDES})
set(THIRDPARTY_INCLUDES ${THIRDPARTY_INCLUDES} ${IMGUI_INCLUDES})
list(APPEND THIRDPARTY_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/imgui)

list(APPEND IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.cpp)
source_group("Source Files\\thirdparty\\imgui" FILES ${IMGUI_SOURCES})
set(THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${IMGUI_SOURCES})

# Add third-party OGRE ImGui binding
list(APPEND OGRE_IMGUI_INCLUDES ${CMAKE_SOURCE_DIR}/thirdparty/ogre-imgui/ImguiManager.h)
source_group("Header Files\\thirdparty\\ogre-imgui" FILES ${OGRE_IMGUI_INCLUDES})
set(THIRDPARTY_INCLUDES ${THIRDPARTY_INCLUDES} ${OGRE_IMGUI_INCLUDES})
list(APPEND THIRDPARTY_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/ogre-imgui)

list(APPEND OGRE_IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/ogre-imgui/ImguiManager.cpp)
source_group("Source Files\\thirdparty\\ogre-imgui" FILES ${OGRE_IMGUI_SOURCES})
set(THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${OGRE_IMGUI_SOURCES})


# Create executable
if(WIN32)
    set(WINDOWS_RESOURCE_PATH ${CMAKE_SOURCE_DIR}/winicon.rc)
endif()
add_executable(client WIN32 MACOSX_BUNDLE
    ${CLIENT_INCLUDES} ${CLIENT_SOURCES}
    ${THIRDPARTY_INCLUDES} ${THIRDPARTY_SOURCES}
    ${WINDOWS_RESOURCE_PATH})

target_link_libraries(client hoibase
                             ${FILESYSTEM_LIB}
                             ${OGRE_LIBRARIES}
                             ${SDL2_LIBRARIES}
                             ${OPENAL_LIBRARY}
                             Boost::dynamic_linking Boost::disable_autolinking Boost::program_options Boost::locale
                             Threads::Threads)

target_include_directories(client
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
    SYSTEM
        ${OGRE_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${OPENAL_INCLUDE_DIR}
        ${THIRDPARTY_INCLUDE_DIRS})


# Set error level
target_compile_options(client PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wc++17-compat-pedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>)
