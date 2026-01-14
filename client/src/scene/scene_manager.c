/**
 * @file scene_manager.c
 * @brief Scene management implementation
 */

#include "../../include/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int scene_manager_init(SceneManager* manager)
{
    if (!manager) return CLIENT_ERROR;

    memset(manager, 0, sizeof(SceneManager));
    return CLIENT_OK;
}

void scene_manager_cleanup(SceneManager* manager)
{
    if (!manager) return;

    if (manager->current) {
        if (manager->current->cleanup) {
            manager->current->cleanup(manager->current);
        }
        free(manager->current);
    }

    if (manager->next) {
        free(manager->next);
    }

    memset(manager, 0, sizeof(SceneManager));
}

void scene_manager_change(SceneManager* manager, SceneType type)
{
    Scene* new_scene = NULL;

    if (!manager) return;

    /* Create new scene */
    switch (type) {
        case SCENE_LOGIN:
            new_scene = scene_create_login();
            break;
        case SCENE_CHARSELECT:
            new_scene = scene_create_charselect();
            break;
        case SCENE_TOWN:
            new_scene = scene_create_town();
            break;
        case SCENE_DUNGEON:
            new_scene = scene_create_dungeon();
            break;
        default:
            fprintf(stderr, "Unknown scene type: %d\n", type);
            return;
    }

    if (!new_scene) {
        fprintf(stderr, "Failed to create scene\n");
        return;
    }

    /* Cleanup current scene */
    if (manager->current) {
        if (manager->current->cleanup) {
            manager->current->cleanup(manager->current);
        }
        free(manager->current);
    }

    /* Initialize new scene */
    manager->current = new_scene;
    if (new_scene->init) {
        new_scene->init(new_scene);
    }
    new_scene->initialized = true;

    printf("Changed to scene: %d\n", type);
}

void scene_manager_update(SceneManager* manager, float delta)
{
    if (!manager || !manager->current) return;

    if (manager->current->update) {
        manager->current->update(manager->current, delta);
    }
}

void scene_manager_draw(SceneManager* manager, Renderer* renderer)
{
    if (!manager || !manager->current || !renderer) return;

    if (manager->current->draw) {
        manager->current->draw(manager->current, renderer);
    }
}

void scene_manager_handle_input(SceneManager* manager, SDL_Event* event)
{
    if (!manager || !manager->current || !event) return;

    if (manager->current->handle_input) {
        manager->current->handle_input(manager->current, event);
    }
}

void scene_manager_on_packet(SceneManager* manager, u16 msg_no, u8* data, int len)
{
    if (!manager || !manager->current) return;

    if (manager->current->on_packet) {
        manager->current->on_packet(manager->current, msg_no, data, len);
    }
}

/*
 * Map functions
 */

GameMap* map_load(Renderer* renderer, const char* path)
{
    GameMap* map;

    (void)path;

    if (!renderer) return NULL;

    map = calloc(1, sizeof(GameMap));
    if (!map) return NULL;

    /* Default values */
    map->width = 100;
    map->height = 20;
    map->layer_count = 2;
    map->bg_color = (Color){48, 48, 64, 255};

    /* Initialize ground layer */
    {
        int x;
        for (x = 0; x < map->width; x++) {
            map->layers[0].tiles[x][15].type = TILE_GROUND;
            map->layers[0].tiles[x][15].sprite_id = 1;
        }
        map->layers[0].visible = true;
    }

    return map;
}

void map_free(GameMap* map)
{
    if (!map) return;

    if (map->tileset) {
        sprite_free(map->tileset);
    }
    if (map->background) {
        SDL_DestroyTexture(map->background);
    }

    free(map);
}

void map_draw(GameMap* map, Renderer* renderer, float camera_x, float camera_y)
{
    int layer, x, y;
    int start_x, end_x, start_y, end_y;
    Rect tile_rect;

    if (!map || !renderer) return;

    /* Calculate visible tiles */
    start_x = (int)(camera_x / TILE_SIZE) - 1;
    end_x = start_x + (renderer->width / TILE_SIZE) + 3;
    start_y = (int)(camera_y / TILE_SIZE) - 1;
    end_y = start_y + (renderer->height / TILE_SIZE) + 3;

    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;
    if (end_x > map->width) end_x = map->width;
    if (end_y > map->height) end_y = map->height;

    /* Draw layers */
    for (layer = 0; layer < map->layer_count; layer++) {
        if (!map->layers[layer].visible) continue;

        for (y = start_y; y < end_y; y++) {
            for (x = start_x; x < end_x; x++) {
                Tile* tile = &map->layers[layer].tiles[x][y];
                if (tile->type == TILE_EMPTY) continue;

                tile_rect.x = x * TILE_SIZE;
                tile_rect.y = y * TILE_SIZE;
                tile_rect.w = TILE_SIZE;
                tile_rect.h = TILE_SIZE;

                /* Draw tile placeholder */
                Color color;
                switch (tile->type) {
                    case TILE_GROUND:
                        color = (Color){128, 96, 64, 255};
                        break;
                    case TILE_PLATFORM:
                        color = (Color){96, 96, 128, 255};
                        break;
                    case TILE_WALL:
                        color = (Color){64, 64, 64, 255};
                        break;
                    default:
                        color = (Color){200, 200, 200, 255};
                        break;
                }

                renderer_draw_rect(renderer, tile_rect, color, true);
            }
        }
    }
}

Tile* map_get_tile(GameMap* map, int layer, int x, int y)
{
    if (!map) return NULL;
    if (layer < 0 || layer >= map->layer_count) return NULL;
    if (x < 0 || x >= map->width) return NULL;
    if (y < 0 || y >= map->height) return NULL;

    return &map->layers[layer].tiles[x][y];
}

bool map_check_collision(GameMap* map, Rect rect)
{
    int x1, y1, x2, y2;
    int x, y;

    if (!map) return false;

    /* Get tile range */
    x1 = (int)(rect.x / TILE_SIZE);
    y1 = (int)(rect.y / TILE_SIZE);
    x2 = (int)((rect.x + rect.w) / TILE_SIZE);
    y2 = (int)((rect.y + rect.h) / TILE_SIZE);

    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x++) {
            Tile* tile = map_get_tile(map, 0, x, y);
            if (tile && (tile->type == TILE_GROUND || tile->type == TILE_WALL)) {
                return true;
            }
        }
    }

    return false;
}
