cmake_minimum_required(VERSION 3.10)
project(DNFClient VERSION 1.0.0 LANGUAGES C)

# C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
endif()

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Source files
set(CLIENT_SOURCES
    src/core/client.c
    src/core/input_manager.c
    src/render/renderer.c
    src/render/map_renderer.c
    src/render/entity_renderer.c
    src/render/animation.c
    src/render/effects.c
    src/render/camera.c
    src/network/network.c
    src/network/packet_parser.c
    src/network/message_handler.c
    src/network/sync_position.c
    src/network/sync_combat.c
    src/network/sync_state.c
    src/network/lag_compensation.c
    src/game/entity.c
    src/game/player_controller.c
    src/game/skill_system.c
    src/game/interaction.c
    src/scene/scene_manager.c
    src/scene/scene_login.c
    src/scene/scene_charselect.c
    src/scene/scene_town.c
    src/scene/scene_dungeon.c
    src/ui/ui.c
    src/ui/ui_inventory.c
    src/ui/ui_skill.c
    src/ui/ui_character.c
    src/ui/ui_chat.c
    src/ui/ui_minimap.c
    src/ui/ui_hud.c
)

# Resource module sources
set(RESOURCE_SOURCES
    src/resource/npk_parser.c
    src/resource/img_decoder.c
    src/resource/resource_manager.c
    src/resource/ani_loader.c
    src/resource/cache_manager.c
)
list(APPEND CLIENT_SOURCES ${RESOURCE_SOURCES})

# PVF library
set(PVF_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib)
set(PVF_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/pvf)

# Find zlib
find_package(ZLIB REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PVF_INC_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

# Create executable
add_executable(dnf_client ${CLIENT_SOURCES})

# Link libraries
target_link_libraries(dnf_client
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    ${PVF_LIB_DIR}/libpvf.a
    ZLIB::ZLIB
)

# Windows specific
if(WIN32)
    target_link_libraries(dnf_client ws2_32)
    # Copy SDL2 DLLs to output directory
    add_custom_command(TARGET dnf_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:dnf_client>
    )
endif()

# Install
install(TARGETS dnf_client DESTINATION bin)
install(DIRECTORY assets/ DESTINATION share/dnf_client/assets)
install(DIRECTORY config/ DESTINATION share/dnf_client/config)

# Print configuration
message(STATUS "")
message(STATUS "DNF Client Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "")
