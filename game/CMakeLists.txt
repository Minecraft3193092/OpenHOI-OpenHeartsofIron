# Setup project details
project(game
        VERSION "${OPENHOI_VERSION_MAJOR}.${OPENHOI_VERSION_MINOR}.${OPENHOI_VERSION_PATCH}"
        LANGUAGES CXX
        DESCRIPTION "openhoi game executable")


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

if(WIN32)
    set(OS_LIBRARIES shcore)
endif()


# Add main code
list(APPEND GAME_INCLUDES include/game_manager.hpp
                          include/options.hpp)
source_group("Header Files" FILES ${GAME_INCLUDES})
list(APPEND GAME_SOURCES src/game_manager.cpp
                         src/openhoi.cpp
                         src/options.cpp)
source_group("Source Files" FILES ${GAME_SOURCES})

# Add audio code
list(APPEND AUDIO_INCLUDES include/audio/audio_device.hpp
                           include/audio/audio_manager.hpp
                           include/audio/sound.hpp)
source_group("Header Files\\audio" FILES ${AUDIO_INCLUDES})
list(APPEND GAME_INCLUDES ${AUDIO_INCLUDES})

list(APPEND AUDIO_SOURCES src/audio/audio_device.cpp
                          src/audio/audio_manager.cpp
                          src/audio/sound.cpp)
source_group("Source Files\\audio" FILES ${AUDIO_SOURCES})
list(APPEND GAME_SOURCES ${AUDIO_SOURCES})

# Add graphic code
list(APPEND GRAPHIC_INCLUDES include/graphic/texture_helper.hpp)
source_group("Header Files\\graphic" FILES ${GRAPHIC_INCLUDES})
list(APPEND GAME_INCLUDES ${GRAPHIC_INCLUDES})

list(APPEND GRAPHIC_SOURCES src/graphic/texture_helper.cpp)
source_group("Source Files\\graphic" FILES ${GRAPHIC_SOURCES})
list(APPEND GAME_SOURCES ${GRAPHIC_SOURCES})

# Add gui code
list(APPEND GUI_INCLUDES include/gui/debug_console.hpp
                         include/gui/gui_element.hpp
                         include/gui/gui_manager.hpp
                         include/gui/imgui_renderable.hpp)
source_group("Header Files\\gui" FILES ${GUI_INCLUDES})
list(APPEND GAME_INCLUDES ${GUI_INCLUDES})

list(APPEND GUI_SOURCES src/gui/debug_console.cpp
                        src/gui/gui_manager.cpp
                        src/gui/imgui_renderable.cpp)
source_group("Source Files\\gui" FILES ${GUI_SOURCES})
list(APPEND GAME_SOURCES ${GUI_SOURCES})

# Add state code
list(APPEND STATE_INCLUDES include/state/menu_state.hpp
                           include/state/state_manager.hpp
                           include/state/state.hpp)
source_group("Header Files\\state" FILES ${STATE_INCLUDES})
list(APPEND GAME_INCLUDES ${STATE_INCLUDES})

list(APPEND STATE_SOURCES src/state/menu_state.cpp
                          src/state/state_manager.cpp
                          src/state/state.cpp)
source_group("Source Files\\state" FILES ${STATE_SOURCES})
list(APPEND GAME_SOURCES ${STATE_SOURCES})

# Add third-party library stb_vorbis
list(APPEND STBVORBIS_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/stb_vorbis/stb_vorbis.c)
source_group("Source Files\\thirdparty\\stb_vorbis" FILES ${STBVORBIS_SOURCES})
set(THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${STBVORBIS_SOURCES})
list(APPEND THIRDPARTY_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/stb_vorbis)

# Add third-party library ImGui
list(APPEND IMGUI_INCLUDES ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imconfig.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_impl_sdl.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_internal.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_rectpack.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_textedit.h
                           ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imstb_truetype.h)
source_group("Header Files\\thirdparty\\imgui" FILES ${IMGUI_INCLUDES})
set(THIRDPARTY_INCLUDES ${THIRDPARTY_INCLUDES} ${IMGUI_INCLUDES})
list(APPEND THIRDPARTY_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/imgui)

list(APPEND IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_impl_sdl.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp)
source_group("Source Files\\thirdparty\\imgui" FILES ${IMGUI_SOURCES})
set(THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${IMGUI_SOURCES})



# Create executable
if(WIN32)
    set(WINDOWS_RESOURCE_PATH ${CMAKE_SOURCE_DIR}/winicon.rc)
endif()
add_executable(game WIN32 MACOSX_BUNDLE
    ${GAME_INCLUDES} ${GAME_SOURCES}
    ${THIRDPARTY_INCLUDES} ${THIRDPARTY_SOURCES}
    ${WINDOWS_RESOURCE_PATH})

set_target_properties(game PROPERTIES OUTPUT_NAME "openhoi")

target_link_libraries(game hoibase
                           ${FILESYSTEM_LIB}
                           ${OGRE_LIBRARIES}
                           ${SDL2_LIBRARIES}
                           ${OPENAL_LIBRARY}
                           Boost::dynamic_linking Boost::disable_autolinking Boost::program_options Boost::locale
                           Threads::Threads
                           ${OS_LIBRARIES})

target_include_directories(game
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>)
target_include_directories(game SYSTEM
     PRIVATE
        ${OGRE_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${OPENAL_INCLUDE_DIR}
        ${THIRDPARTY_INCLUDE_DIRS})


# Set error level
target_compile_options(game PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wc++17-compat-pedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>)
