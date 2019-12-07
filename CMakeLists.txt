cmake_minimum_required(VERSION 3.1)
project(capture_the_castle)

# Set c++11
# https://stackoverflow.com/questions/10851247/how-to-activate-c-11-in-cmake
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()
set(CMAKE_CXX_STANDARD 17)

# nice hierarchichal structure in MSVC
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Find OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(IS_OS_MAC 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(IS_OS_LINUX 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(IS_OS_WINDOWS 1)
else ()
    message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif ()

# Create executable target

# Generate the shader folder location to the header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/project_path.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/project_path.hpp")

# You can switch to use the file GLOB for simplicity but at your own risk
# file(GLOB SOURCE_FILES src/*.cpp src/*.hpp)

set(SOURCE_FILES
        src/main.cpp
        src/common.cpp
        src/ecs/entity_manager.cpp
        src/systems/movement_system.cpp
        src/systems/player_input_system.cpp
        src/systems/render_system.cpp
        src/systems/bandit_spawn_system.cpp
		src/systems/bandit_ai_system.cpp
		src/systems/soldier_ai_system.cpp
        src/texture_manager.cpp
        src/mesh_manager.cpp
        src/components.cpp
        src/world.cpp
        src/background.cpp
        src/tile.cpp
		src/tilemap.cpp
        src/states/game.cpp
        src/states/state.hpp
        src/states/menu.cpp

        src/project_path.hpp
        src/common.hpp
        src/ecs/common_ecs.hpp
        src/ecs/ecs_manager.hpp
        src/ecs/component_array.hpp
        src/ecs/component_manager.hpp
        src/ecs/entity_manager.hpp
        src/ecs/system_manager.hpp
        src/ecs/events.hpp
        src/ecs/event_manager.hpp
        src/ecs/event_handler.hpp
        src/components.hpp
        src/systems/movement_system.hpp
        src/systems/player_input_system.hpp
        src/systems/render_system.hpp
        src/systems/bandit_spawn_system.hpp
        src/systems/bandit_ai_system.hpp
		src/systems/soldier_ai_system.hpp
        src/texture_manager.hpp
        src/mesh_manager.hpp
        src/world.hpp
        src/background.hpp
        src/tile.hpp
		src/tilemap.hpp
        src/ui/helpbtn.cpp
        src/ui/helpbtn.hpp
        src/ui/help_window.cpp
        src/ui/help_window.hpp
        src/ui/closebtn.cpp
        src/ui/closebtn.hpp
        src/ui/win_window.cpp
        src/ui/win_window.hpp
        src/ui/popup_background.cpp
        src/ui/popup_background.hpp
        src/ui/popUpImg.cpp
        src/ui/popUpImg.hpp
        src/ui/howtoplay_background.cpp
        src/ui/howtoplay_background.hpp
        src/ui/play_instructions.cpp
        src/ui/play_instructions.hpp
        src/ui/popupbtn.cpp
        src/ui/popupbtn.hpp
        src/systems/collision_system.cpp
        src/systems/collision_system.hpp
        src/systems/box_collision_system.cpp
        src/systems/box_collision_system.hpp
        src/systems/item_spawn_system.cpp
        src/systems/item_spawn_system.hpp
        src/states/game.hpp
        src/states/menu.hpp
        src/states/howtoplay.cpp
        src/states/howtoplay.hpp
        src/systems/box_collision_system.hpp
        src/ui/menus_background.cpp
        src/systems/item_board_system.cpp 
        src/systems/item_board_system.hpp
        src/ui/menus_background.hpp
        src/ui/firework.cpp
        src/ui/firework.hpp
        src/curve.hpp
        src/systems/curve_movement_system.hpp
        src/systems/item_effect_system.hpp
        src/ui/countdown_timer.hpp
        src/ui/countdown_timer.cpp
        src/ui/pause_window.hpp
        src/ui/pause_window.cpp
        src/ui/soldier_setup_window.cpp
		src/ui/hint.cpp src/ui/hint.hpp
		src/systems/rain_system.hpp
		src/systems/rain_system.cpp
        src/ui/soldier_setup_window.hpp)

if (IS_OS_MAC)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} src/ui/helpbtn.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

# Added this so policy CMP0065 doesn't scream
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS 0)


# External header-only libraries in the ext/

target_include_directories(${PROJECT_NAME} PUBLIC ext/stb_image/)
target_include_directories(${PROJECT_NAME} PUBLIC ext/gl3w)

# Find OpenGL
find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
endif ()

# glfw, sdl could be precompiled (on windows) or installed by a package manager (on OSX and Linux)

if (IS_OS_LINUX OR IS_OS_MAC)
    # Try to find packages rather than to use the precompiled ones
    # Since we're on OSX or Linux, we can just use pkgconfig.
    find_package(PkgConfig REQUIRED)

    pkg_search_module(GLFW REQURIED glfw3)

    pkg_search_module(SDL2 REQURIED sdl2)
    pkg_search_module(SDL2MIXER REQURIED SDL2_mixer)
    pkg_search_module(FREETYPE REQUIRED freetype)

    # Link Frameworks on OSX
    if (IS_OS_MAC)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(CF_LIBRARY CoreFoundation)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY} ${CF_LIBRARY})
    endif ()
elseif (IS_OS_WINDOWS)
    # https://stackoverflow.com/questions/17126860/cmake-link-precompiled-library-depending-on-os-and-architecture
    set(GLFW_FOUND TRUE)
    set(SDL2_FOUND TRUE)
    set(FREETYPE_FOUND TRUE)

    set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include")
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/include/SDL")
    set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/include")

    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x64.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.lib")
        set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.lib")
        set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win64/freetype.lib")

        set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x64.dll")
        set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.dll")
        set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.dll")
        set(FREETYPE_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win64/freetype.dll")
    else ()
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x86.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.lib")
        set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.lib")
        set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win32/freetype.lib")

        set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x86.dll")
        set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.dll")
        set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.dll")
        set(FREETYPE_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/win32/freetype.dll")
    endif ()

    # Copy and rename dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDLMIXER_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FREETYPE_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/freetype.dll")
endif ()

# Can't find the include and lib. Quit.
if (NOT GLFW_FOUND OR NOT SDL2_FOUND OR NOT FREETYPE_FOUND)
    if (NOT GLFW_FOUND)
        message(FATAL_ERROR "Can't find GLFW.")
    elseif (NOT SDL2_FOUND)
        message(FATAL_ERROR "Can't find SDL.")
    else ()
        message(FATAL_ERROR "Can't find FREETYPE.")
    endif()
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${FREETYPE_LIBRARIES})

# Needed to add this
if (IS_OS_LINUX)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})
endif ()