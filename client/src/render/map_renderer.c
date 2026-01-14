/**
 * @file map_renderer.c
 * @brief Map and tile rendering system
 */

#include "../../include/renderer.h"
#include "../../include/scene.h"
#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_TILESETS        16
#define MAX_MAP_LAYERS      8
#define PARALLAX_LAYERS     4
#define TILE_CACHE_SIZE     256

/*
 * ============================================================================
 * Tileset Management
 * ============================================================================
 */

typedef struct {
    u32         id;
    SDL_Texture* texture;
    int         tile_width;
    int         tile_height;
    int         columns;
    int         rows;
    int         tile_count;
    int         first_gid;
} Tileset;

typedef struct {
    Tileset     sets[MAX_TILESETS];
    int         count;
} TilesetManager;

static TilesetManager g_tilesets = {0};

int tileset_load(Renderer* renderer, const char* path, int tile_w, int tile_h, int first_gid)
{
    Tileset* ts;
    int tex_w, tex_h;

    if (g_tilesets.count >= MAX_TILESETS) {
        fprintf(stderr, "Tileset limit reached\n");
        return -1;
    }

    ts = &g_tilesets.sets[g_tilesets.count];
    ts->id = g_tilesets.count;
    ts->tile_width = tile_w;
    ts->tile_height = tile_h;
    ts->first_gid = first_gid;

    ts->texture = renderer_load_texture(renderer, path);
    if (!ts->texture) {
        fprintf(stderr, "Failed to load tileset: %s\n", path);
        return -1;
    }

    SDL_QueryTexture(ts->texture, NULL, NULL, &tex_w, &tex_h);
    ts->columns = tex_w / tile_w;
    ts->rows = tex_h / tile_h;
    ts->tile_count = ts->columns * ts->rows;

    printf("Loaded tileset %d: %s (%dx%d tiles)\n",
           g_tilesets.count, path, ts->columns, ts->rows);

    return g_tilesets.count++;
}

Tileset* tileset_get(int id)
{
    if (id < 0 || id >= g_tilesets.count) return NULL;
    return &g_tilesets.sets[id];
}

Tileset* tileset_find_for_gid(int gid)
{
    int i;
    Tileset* result = NULL;

    for (i = 0; i < g_tilesets.count; i++) {
        if (g_tilesets.sets[i].first_gid <= gid) {
            if (!result || g_tilesets.sets[i].first_gid > result->first_gid) {
                result = &g_tilesets.sets[i];
            }
        }
    }

    return result;
}

void tileset_cleanup(void)
{
    int i;
    for (i = 0; i < g_tilesets.count; i++) {
        if (g_tilesets.sets[i].texture) {
            SDL_DestroyTexture(g_tilesets.sets[i].texture);
        }
    }
    g_tilesets.count = 0;
}

/*
 * ============================================================================
 * Parallax Background
 * ============================================================================
 */

typedef struct {
    SDL_Texture* texture;
    float       scroll_x;
    float       scroll_y;
    float       speed_x;    /* Parallax factor (0=static, 1=full scroll) */
    float       speed_y;
    bool        repeat_x;
    bool        repeat_y;
    int         width;
    int         height;
} ParallaxLayer;

typedef struct {
    ParallaxLayer layers[PARALLAX_LAYERS];
    int           count;
    Color         bg_color;
} ParallaxBackground;

static ParallaxBackground g_parallax = {0};

int parallax_add_layer(Renderer* renderer, const char* path,
                       float speed_x, float speed_y, bool repeat_x, bool repeat_y)
{
    ParallaxLayer* layer;

    if (g_parallax.count >= PARALLAX_LAYERS) return -1;

    layer = &g_parallax.layers[g_parallax.count];
    layer->texture = renderer_load_texture(renderer, path);
    if (!layer->texture) return -1;

    SDL_QueryTexture(layer->texture, NULL, NULL, &layer->width, &layer->height);
    layer->scroll_x = 0;
    layer->scroll_y = 0;
    layer->speed_x = speed_x;
    layer->speed_y = speed_y;
    layer->repeat_x = repeat_x;
    layer->repeat_y = repeat_y;

    return g_parallax.count++;
}

void parallax_set_bgcolor(Color color)
{
    g_parallax.bg_color = color;
}

void parallax_draw(Renderer* renderer, float camera_x, float camera_y)
{
    int i;

    /* Draw background color */
    renderer_clear(renderer, g_parallax.bg_color);

    /* Draw layers back to front */
    for (i = 0; i < g_parallax.count; i++) {
        ParallaxLayer* layer = &g_parallax.layers[i];
        if (!layer->texture) continue;

        float offset_x = camera_x * layer->speed_x;
        float offset_y = camera_y * layer->speed_y;

        if (layer->repeat_x) {
            /* Tile horizontally */
            int start_x = (int)(-offset_x) % layer->width;
            if (start_x > 0) start_x -= layer->width;

            int x;
            for (x = start_x; x < renderer->width; x += layer->width) {
                Rect src = {0, 0, (float)layer->width, (float)layer->height};
                Rect dst = {(float)x, 0, (float)layer->width, (float)layer->height};
                renderer_draw_texture_ex(renderer, layer->texture, src, dst, 0, false, false);
            }
        } else {
            Rect src = {0, 0, (float)layer->width, (float)layer->height};
            Rect dst = {-offset_x, -offset_y, (float)layer->width, (float)layer->height};
            renderer_draw_texture_ex(renderer, layer->texture, src, dst, 0, false, false);
        }
    }
}

void parallax_cleanup(void)
{
    int i;
    for (i = 0; i < g_parallax.count; i++) {
        if (g_parallax.layers[i].texture) {
            SDL_DestroyTexture(g_parallax.layers[i].texture);
        }
    }
    g_parallax.count = 0;
}

/*
 * ============================================================================
 * Map Renderer
 * ============================================================================
 */

typedef struct {
    Renderer*   renderer;
    GameMap*    map;
    float       camera_x;
    float       camera_y;
    float       zoom;

    /* Culling */
    int         visible_start_x;
    int         visible_start_y;
    int         visible_end_x;
    int         visible_end_y;

    /* Debug */
    bool        show_grid;
    bool        show_collision;
} MapRenderer;

static MapRenderer g_map_renderer = {0};

void map_renderer_init(Renderer* renderer)
{
    memset(&g_map_renderer, 0, sizeof(MapRenderer));
    g_map_renderer.renderer = renderer;
    g_map_renderer.zoom = 1.0f;
}

void map_renderer_set_map(GameMap* map)
{
    g_map_renderer.map = map;
}

void map_renderer_set_camera(float x, float y)
{
    g_map_renderer.camera_x = x;
    g_map_renderer.camera_y = y;
}

void map_renderer_set_zoom(float zoom)
{
    if (zoom < 0.25f) zoom = 0.25f;
    if (zoom > 4.0f) zoom = 4.0f;
    g_map_renderer.zoom = zoom;
}

static void calculate_visible_tiles(void)
{
    Renderer* r = g_map_renderer.renderer;
    GameMap* map = g_map_renderer.map;
    float cam_x = g_map_renderer.camera_x;
    float cam_y = g_map_renderer.camera_y;
    float zoom = g_map_renderer.zoom;

    if (!r || !map) return;

    float view_w = r->width / zoom;
    float view_h = r->height / zoom;

    g_map_renderer.visible_start_x = (int)((cam_x - view_w / 2) / TILE_SIZE) - 1;
    g_map_renderer.visible_start_y = (int)((cam_y - view_h / 2) / TILE_SIZE) - 1;
    g_map_renderer.visible_end_x = (int)((cam_x + view_w / 2) / TILE_SIZE) + 2;
    g_map_renderer.visible_end_y = (int)((cam_y + view_h / 2) / TILE_SIZE) + 2;

    /* Clamp to map bounds */
    if (g_map_renderer.visible_start_x < 0) g_map_renderer.visible_start_x = 0;
    if (g_map_renderer.visible_start_y < 0) g_map_renderer.visible_start_y = 0;
    if (g_map_renderer.visible_end_x > map->width) g_map_renderer.visible_end_x = map->width;
    if (g_map_renderer.visible_end_y > map->height) g_map_renderer.visible_end_y = map->height;
}

void map_renderer_draw(void)
{
    Renderer* r = g_map_renderer.renderer;
    GameMap* map = g_map_renderer.map;
    int layer, x, y;

    if (!r || !map) return;

    calculate_visible_tiles();

    float cam_x = g_map_renderer.camera_x;
    float cam_y = g_map_renderer.camera_y;
    float zoom = g_map_renderer.zoom;
    float offset_x = r->width / 2.0f - cam_x * zoom;
    float offset_y = r->height / 2.0f - cam_y * zoom;

    /* Draw each layer */
    for (layer = 0; layer < map->layer_count; layer++) {
        if (!map->layers[layer].visible) continue;

        for (y = g_map_renderer.visible_start_y; y < g_map_renderer.visible_end_y; y++) {
            for (x = g_map_renderer.visible_start_x; x < g_map_renderer.visible_end_x; x++) {
                Tile* tile = &map->layers[layer].tiles[x][y];
                if (tile->type == TILE_EMPTY) continue;

                float draw_x = x * TILE_SIZE * zoom + offset_x;
                float draw_y = y * TILE_SIZE * zoom + offset_y;
                float draw_size = TILE_SIZE * zoom;

                if (tile->sprite_id > 0) {
                    /* Draw from tileset */
                    Tileset* ts = tileset_find_for_gid(tile->sprite_id);
                    if (ts && ts->texture) {
                        int local_id = tile->sprite_id - ts->first_gid;
                        int src_x = (local_id % ts->columns) * ts->tile_width;
                        int src_y = (local_id / ts->columns) * ts->tile_height;

                        Rect src = {(float)src_x, (float)src_y,
                                   (float)ts->tile_width, (float)ts->tile_height};
                        Rect dst = {draw_x, draw_y, draw_size, draw_size};
                        renderer_draw_texture_ex(r, ts->texture, src, dst, 0, false, false);
                    }
                } else {
                    /* Fallback: draw colored rectangle */
                    Color color;
                    switch (tile->type) {
                        case TILE_GROUND:   color = (Color){100, 80, 60, 255}; break;
                        case TILE_PLATFORM: color = (Color){80, 80, 100, 255}; break;
                        case TILE_WALL:     color = (Color){60, 60, 60, 255}; break;
                        default:            color = (Color){150, 150, 150, 255}; break;
                    }
                    Rect rect = {draw_x, draw_y, draw_size, draw_size};
                    renderer_draw_rect(r, rect, color, true);
                }
            }
        }
    }

    /* Debug: draw grid */
    if (g_map_renderer.show_grid) {
        Color grid_color = {100, 100, 100, 100};
        for (x = g_map_renderer.visible_start_x; x <= g_map_renderer.visible_end_x; x++) {
            float draw_x = x * TILE_SIZE * zoom + offset_x;
            Rect line = {draw_x, 0, 1, (float)r->height};
            renderer_draw_rect(r, line, grid_color, true);
        }
        for (y = g_map_renderer.visible_start_y; y <= g_map_renderer.visible_end_y; y++) {
            float draw_y = y * TILE_SIZE * zoom + offset_y;
            Rect line = {0, draw_y, (float)r->width, 1};
            renderer_draw_rect(r, line, grid_color, true);
        }
    }

    /* Debug: draw collision */
    if (g_map_renderer.show_collision) {
        Color collision_color = {255, 0, 0, 100};
        for (y = g_map_renderer.visible_start_y; y < g_map_renderer.visible_end_y; y++) {
            for (x = g_map_renderer.visible_start_x; x < g_map_renderer.visible_end_x; x++) {
                Tile* tile = &map->layers[0].tiles[x][y];
                if (tile->type == TILE_GROUND || tile->type == TILE_WALL) {
                    float draw_x = x * TILE_SIZE * zoom + offset_x;
                    float draw_y = y * TILE_SIZE * zoom + offset_y;
                    Rect rect = {draw_x, draw_y, TILE_SIZE * zoom, TILE_SIZE * zoom};
                    renderer_draw_rect(r, rect, collision_color, true);
                }
            }
        }
    }
}

void map_renderer_toggle_grid(void)
{
    g_map_renderer.show_grid = !g_map_renderer.show_grid;
}

void map_renderer_toggle_collision(void)
{
    g_map_renderer.show_collision = !g_map_renderer.show_collision;
}

/*
 * ============================================================================
 * Screen to World Conversion
 * ============================================================================
 */

void map_renderer_screen_to_world(int screen_x, int screen_y, float* world_x, float* world_y)
{
    Renderer* r = g_map_renderer.renderer;
    float zoom = g_map_renderer.zoom;

    if (!r) {
        *world_x = (float)screen_x;
        *world_y = (float)screen_y;
        return;
    }

    *world_x = g_map_renderer.camera_x + (screen_x - r->width / 2.0f) / zoom;
    *world_y = g_map_renderer.camera_y + (screen_y - r->height / 2.0f) / zoom;
}

void map_renderer_world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y)
{
    Renderer* r = g_map_renderer.renderer;
    float zoom = g_map_renderer.zoom;

    if (!r) {
        *screen_x = (int)world_x;
        *screen_y = (int)world_y;
        return;
    }

    *screen_x = (int)((world_x - g_map_renderer.camera_x) * zoom + r->width / 2.0f);
    *screen_y = (int)((world_y - g_map_renderer.camera_y) * zoom + r->height / 2.0f);
}

/*
 * ============================================================================
 * Cleanup
 * ============================================================================
 */

void map_renderer_cleanup(void)
{
    tileset_cleanup();
    parallax_cleanup();
    memset(&g_map_renderer, 0, sizeof(MapRenderer));
}
