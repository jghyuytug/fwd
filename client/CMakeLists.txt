cmake_minimum_required(VERSION 3.10)
project(DNFClient VERSION 1.0.0 LANGUAGES C)

# C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_DNF_CLIENT "Build dnf_client (SDL2)" ON)
option(BUILD_DNF_TESTS "Build non-SDL test tools" ON)

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

# Find zlib (used by IMG decoder and some tools)
find_package(ZLIB REQUIRED)

# PVF sources (compiled directly; no prebuilt lib required)
set(PVF_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_decrypt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_packset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_stringtable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pvf/pvf_data_parser.c
)
add_library(pvf STATIC ${PVF_SOURCES})
target_include_directories(pvf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(pvf PUBLIC ZLIB::ZLIB)

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

# Include directories
set(CLIENT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${ZLIB_INCLUDE_DIRS}
)

if(BUILD_DNF_CLIENT)
    # Find SDL2 packages (only required for the main client)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)

    add_executable(dnf_client ${CLIENT_SOURCES})
    target_include_directories(dnf_client PRIVATE ${CLIENT_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
    target_link_libraries(dnf_client
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_mixer::SDL2_mixer
        pvf
        ZLIB::ZLIB
    )

    if(WIN32)
        target_link_libraries(dnf_client ws2_32)
        # Copy SDL2 DLLs to output directory
        add_custom_command(TARGET dnf_client POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:dnf_client>
        )
    endif()
endif()

if(BUILD_DNF_TESTS)
    add_executable(test_ani_resolve
        tests/test_ani_resolve.c
        src/resource/ani_loader.c
        src/resource/npk_parser.c
    )
    target_include_directories(test_ani_resolve PRIVATE ${CLIENT_INCLUDE_DIRS})
    target_link_libraries(test_ani_resolve pvf ZLIB::ZLIB)

    add_executable(list_pvf_ani tests/list_pvf_ani.c)
    target_include_directories(list_pvf_ani PRIVATE ${CLIENT_INCLUDE_DIRS})
    target_link_libraries(list_pvf_ani pvf ZLIB::ZLIB)

    add_executable(dump_pvf_file tests/dump_pvf_file.c)
    target_include_directories(dump_pvf_file PRIVATE ${CLIENT_INCLUDE_DIRS})
    target_link_libraries(dump_pvf_file pvf ZLIB::ZLIB)

    add_executable(test_ani_decode
        tests/test_ani_decode.c
        src/resource/ani_loader.c
        src/resource/npk_parser.c
        src/resource/img_decoder.c
    )
    target_include_directories(test_ani_decode PRIVATE ${CLIENT_INCLUDE_DIRS})
    target_compile_definitions(test_ani_decode PRIVATE IMG_NO_SDL=1)
    target_link_libraries(test_ani_decode pvf ZLIB::ZLIB)
endif()

# Install (only when the main client is built)
if(BUILD_DNF_CLIENT)
    install(TARGETS dnf_client DESTINATION bin)
    install(DIRECTORY assets/ DESTINATION share/dnf_client/assets)
    install(DIRECTORY config/ DESTINATION share/dnf_client/config)
endif()

# Print configuration
message(STATUS "")
message(STATUS "DNF Client Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "")
