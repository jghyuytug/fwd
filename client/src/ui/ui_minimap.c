/**
 * @file ui_minimap.c
 * @brief Minimap display with markers and navigation
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MINIMAP_SIZE            150
#define MINIMAP_PADDING         5
#define MAX_MARKERS             64
#define MARKER_SIZE             6
#define PLAYER_MARKER_SIZE      8
#define PING_DURATION           3.0f
#define PING_EXPAND_SPEED       50.0f

/*
 * ============================================================================
 * Marker Types
 * ============================================================================
 */

typedef enum {
    MARKER_PLAYER,
    MARKER_PARTY_MEMBER,
    MARKER_ENEMY,
    MARKER_NPC,
    MARKER_QUEST,
    MARKER_BOSS,
    MARKER_PORTAL,
    MARKER_ITEM,
    MARKER_WAYPOINT,
    MARKER_PING
} MarkerType;

typedef struct {
    float           world_x;
    float           world_y;
    MarkerType      type;
    uint32_t        entity_id;
    char            label[32];
    bool            active;
    float           pulse_timer;    /* For pulsing effect */
} MapMarker;

typedef struct {
    float           x;
    float           y;
    float           radius;
    float           timer;
    bool            active;
} PingMarker;

typedef struct {
    /* Display state */
    bool            visible;
    bool            expanded;       /* Full map mode */
    float           x;
    float           y;
    float           size;

    /* Map bounds (world coordinates) */
    float           map_min_x;
    float           map_min_y;
    float           map_max_x;
    float           map_max_y;
    char            map_name[64];

    /* Player position */
    float           player_x;
    float           player_y;
    float           player_facing;

    /* Markers */
    MapMarker       markers[MAX_MARKERS];
    int             marker_count;

    /* Pings */
    PingMarker      pings[8];

    /* Zoom */
    float           zoom;
    float           min_zoom;
    float           max_zoom;

    /* Options */
    bool            show_enemies;
    bool            show_npcs;
    bool            show_items;
    bool            rotate_map;

    bool            initialized;
} MinimapUI;

static MinimapUI g_minimap = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static Color get_marker_color(MarkerType type)
{
    switch (type) {
        case MARKER_PLAYER:       return (Color){100, 255, 100, 255};
        case MARKER_PARTY_MEMBER: return (Color){100, 200, 255, 255};
        case MARKER_ENEMY:        return (Color){255, 100, 100, 255};
        case MARKER_NPC:          return (Color){255, 255, 100, 255};
        case MARKER_QUEST:        return (Color){255, 200, 0, 255};
        case MARKER_BOSS:         return (Color){255, 50, 50, 255};
        case MARKER_PORTAL:       return (Color){150, 100, 255, 255};
        case MARKER_ITEM:         return (Color){200, 200, 200, 255};
        case MARKER_WAYPOINT:     return (Color){100, 255, 200, 255};
        case MARKER_PING:         return (Color){255, 255, 255, 255};
        default:                  return (Color){255, 255, 255, 255};
    }
}

static void world_to_minimap(float world_x, float world_y,
                              float* map_x, float* map_y)
{
    float map_width = g_minimap.map_max_x - g_minimap.map_min_x;
    float map_height = g_minimap.map_max_y - g_minimap.map_min_y;

    if (map_width <= 0) map_width = 1;
    if (map_height <= 0) map_height = 1;

    /* Calculate relative to player (centered view) */
    float rel_x = world_x - g_minimap.player_x;
    float rel_y = world_y - g_minimap.player_y;

    /* Apply zoom */
    float scale = g_minimap.size / (map_width / g_minimap.zoom);

    /* Convert to minimap coordinates */
    *map_x = g_minimap.x + g_minimap.size / 2 + rel_x * scale;
    *map_y = g_minimap.y + g_minimap.size / 2 + rel_y * scale;
}

static bool is_in_minimap_bounds(float map_x, float map_y)
{
    return map_x >= g_minimap.x && map_x <= g_minimap.x + g_minimap.size &&
           map_y >= g_minimap.y && map_y <= g_minimap.y + g_minimap.size;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void ui_minimap_init(void)
{
    memset(&g_minimap, 0, sizeof(MinimapUI));

    g_minimap.size = MINIMAP_SIZE;
    g_minimap.x = WINDOW_WIDTH - MINIMAP_SIZE - 10;
    g_minimap.y = 10;

    g_minimap.visible = true;
    g_minimap.zoom = 1.0f;
    g_minimap.min_zoom = 0.5f;
    g_minimap.max_zoom = 4.0f;

    g_minimap.show_enemies = true;
    g_minimap.show_npcs = true;
    g_minimap.show_items = true;

    g_minimap.initialized = true;
}

void ui_minimap_shutdown(void)
{
    g_minimap.initialized = false;
}

/*
 * ============================================================================
 * Map Setup
 * ============================================================================
 */

void ui_minimap_set_map(const char* name, float min_x, float min_y,
                         float max_x, float max_y)
{
    strncpy(g_minimap.map_name, name, sizeof(g_minimap.map_name) - 1);
    g_minimap.map_min_x = min_x;
    g_minimap.map_min_y = min_y;
    g_minimap.map_max_x = max_x;
    g_minimap.map_max_y = max_y;

    /* Clear markers on map change */
    ui_minimap_clear_markers();
}

void ui_minimap_set_player_position(float x, float y, float facing)
{
    g_minimap.player_x = x;
    g_minimap.player_y = y;
    g_minimap.player_facing = facing;
}

/*
 * ============================================================================
 * Marker Management
 * ============================================================================
 */

void ui_minimap_add_marker(uint32_t entity_id, float x, float y,
                            MarkerType type, const char* label)
{
    /* Check if marker already exists */
    for (int i = 0; i < g_minimap.marker_count; i++) {
        if (g_minimap.markers[i].entity_id == entity_id &&
            g_minimap.markers[i].active) {
            /* Update existing marker */
            g_minimap.markers[i].world_x = x;
            g_minimap.markers[i].world_y = y;
            return;
        }
    }

    /* Add new marker */
    if (g_minimap.marker_count >= MAX_MARKERS) {
        /* Find inactive slot */
        for (int i = 0; i < MAX_MARKERS; i++) {
            if (!g_minimap.markers[i].active) {
                MapMarker* m = &g_minimap.markers[i];
                m->world_x = x;
                m->world_y = y;
                m->type = type;
                m->entity_id = entity_id;
                if (label) strncpy(m->label, label, sizeof(m->label) - 1);
                m->active = true;
                return;
            }
        }
        return;  /* No room */
    }

    MapMarker* m = &g_minimap.markers[g_minimap.marker_count++];
    m->world_x = x;
    m->world_y = y;
    m->type = type;
    m->entity_id = entity_id;
    if (label) strncpy(m->label, label, sizeof(m->label) - 1);
    m->active = true;
}

void ui_minimap_remove_marker(uint32_t entity_id)
{
    for (int i = 0; i < g_minimap.marker_count; i++) {
        if (g_minimap.markers[i].entity_id == entity_id) {
            g_minimap.markers[i].active = false;
            return;
        }
    }
}

void ui_minimap_update_marker(uint32_t entity_id, float x, float y)
{
    for (int i = 0; i < g_minimap.marker_count; i++) {
        if (g_minimap.markers[i].entity_id == entity_id &&
            g_minimap.markers[i].active) {
            g_minimap.markers[i].world_x = x;
            g_minimap.markers[i].world_y = y;
            return;
        }
    }
}

void ui_minimap_clear_markers(void)
{
    for (int i = 0; i < MAX_MARKERS; i++) {
        g_minimap.markers[i].active = false;
    }
    g_minimap.marker_count = 0;
}

/*
 * ============================================================================
 * Ping
 * ============================================================================
 */

void ui_minimap_ping(float world_x, float world_y)
{
    for (int i = 0; i < 8; i++) {
        if (!g_minimap.pings[i].active) {
            g_minimap.pings[i].x = world_x;
            g_minimap.pings[i].y = world_y;
            g_minimap.pings[i].radius = 0;
            g_minimap.pings[i].timer = 0;
            g_minimap.pings[i].active = true;
            return;
        }
    }

    /* Recycle oldest */
    g_minimap.pings[0].x = world_x;
    g_minimap.pings[0].y = world_y;
    g_minimap.pings[0].radius = 0;
    g_minimap.pings[0].timer = 0;
}

/*
 * ============================================================================
 * Input
 * ============================================================================
 */

bool ui_minimap_handle_input(int mouse_x, int mouse_y, bool left_click,
                              bool right_click)
{
    if (!g_minimap.visible) return false;

    bool over = mouse_x >= g_minimap.x && mouse_x <= g_minimap.x + g_minimap.size &&
                mouse_y >= g_minimap.y && mouse_y <= g_minimap.y + g_minimap.size;

    if (!over) return false;

    /* Scroll to zoom */
    int wheel = input_get_mouse_wheel();
    if (wheel != 0) {
        g_minimap.zoom += wheel * 0.2f;
        if (g_minimap.zoom < g_minimap.min_zoom) g_minimap.zoom = g_minimap.min_zoom;
        if (g_minimap.zoom > g_minimap.max_zoom) g_minimap.zoom = g_minimap.max_zoom;
    }

    /* Right click to ping */
    if (right_click) {
        /* Convert minimap coords to world coords */
        float rel_x = (mouse_x - g_minimap.x - g_minimap.size / 2);
        float rel_y = (mouse_y - g_minimap.y - g_minimap.size / 2);

        float map_width = g_minimap.map_max_x - g_minimap.map_min_x;
        float scale = g_minimap.size / (map_width / g_minimap.zoom);

        float world_x = g_minimap.player_x + rel_x / scale;
        float world_y = g_minimap.player_y + rel_y / scale;

        ui_minimap_ping(world_x, world_y);
    }

    /* Left click to toggle expanded */
    if (left_click) {
        g_minimap.expanded = !g_minimap.expanded;
        if (g_minimap.expanded) {
            g_minimap.size = 300;
            g_minimap.x = WINDOW_WIDTH / 2 - 150;
            g_minimap.y = WINDOW_HEIGHT / 2 - 150;
        } else {
            g_minimap.size = MINIMAP_SIZE;
            g_minimap.x = WINDOW_WIDTH - MINIMAP_SIZE - 10;
            g_minimap.y = 10;
        }
    }

    return true;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void ui_minimap_update(float delta)
{
    if (!g_minimap.initialized) return;

    /* Update marker pulses */
    for (int i = 0; i < g_minimap.marker_count; i++) {
        if (g_minimap.markers[i].active) {
            g_minimap.markers[i].pulse_timer += delta * 3.0f;
            if (g_minimap.markers[i].pulse_timer > 6.28318f) {
                g_minimap.markers[i].pulse_timer -= 6.28318f;
            }
        }
    }

    /* Update pings */
    for (int i = 0; i < 8; i++) {
        if (g_minimap.pings[i].active) {
            g_minimap.pings[i].timer += delta;
            g_minimap.pings[i].radius += PING_EXPAND_SPEED * delta;

            if (g_minimap.pings[i].timer >= PING_DURATION) {
                g_minimap.pings[i].active = false;
            }
        }
    }
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

extern void renderer_draw_rect(Renderer* r, Rect rect, Color color, bool filled);
extern void renderer_draw_text(Renderer* r, const char* text, int x, int y, Color color);

void ui_minimap_render(Renderer* renderer)
{
    if (!g_minimap.visible || !renderer) return;

    Color bg = {20, 20, 30, 200};
    Color border = {80, 80, 100, 255};
    Color text_color = {255, 255, 255, 255};

    /* Background */
    Rect panel = {g_minimap.x, g_minimap.y, g_minimap.size, g_minimap.size};
    renderer_draw_rect(renderer, panel, bg, true);
    renderer_draw_rect(renderer, panel, border, false);

    /* Draw markers */
    for (int i = 0; i < g_minimap.marker_count; i++) {
        MapMarker* m = &g_minimap.markers[i];
        if (!m->active) continue;

        /* Filter by type */
        if (m->type == MARKER_ENEMY && !g_minimap.show_enemies) continue;
        if (m->type == MARKER_NPC && !g_minimap.show_npcs) continue;
        if (m->type == MARKER_ITEM && !g_minimap.show_items) continue;

        float map_x, map_y;
        world_to_minimap(m->world_x, m->world_y, &map_x, &map_y);

        if (!is_in_minimap_bounds(map_x, map_y)) continue;

        Color c = get_marker_color(m->type);

        /* Pulsing for quest/boss markers */
        if (m->type == MARKER_QUEST || m->type == MARKER_BOSS) {
            float pulse = 0.5f + 0.5f * sinf(m->pulse_timer);
            c.a = (uint8_t)(128 + 127 * pulse);
        }

        float size = (m->type == MARKER_BOSS) ? MARKER_SIZE * 1.5f : MARKER_SIZE;
        Rect marker = {map_x - size / 2, map_y - size / 2, size, size};
        renderer_draw_rect(renderer, marker, c, true);
    }

    /* Draw pings */
    for (int i = 0; i < 8; i++) {
        if (!g_minimap.pings[i].active) continue;

        float map_x, map_y;
        world_to_minimap(g_minimap.pings[i].x, g_minimap.pings[i].y, &map_x, &map_y);

        float alpha = 1.0f - (g_minimap.pings[i].timer / PING_DURATION);
        Color ping_color = {255, 255, 255, (uint8_t)(alpha * 255)};

        float r = g_minimap.pings[i].radius;
        Rect ping = {map_x - r, map_y - r, r * 2, r * 2};
        renderer_draw_rect(renderer, ping, ping_color, false);
    }

    /* Draw player marker (center) */
    float player_x = g_minimap.x + g_minimap.size / 2;
    float player_y = g_minimap.y + g_minimap.size / 2;

    Color player_color = {100, 255, 100, 255};
    Rect player = {player_x - PLAYER_MARKER_SIZE / 2,
                   player_y - PLAYER_MARKER_SIZE / 2,
                   PLAYER_MARKER_SIZE, PLAYER_MARKER_SIZE};
    renderer_draw_rect(renderer, player, player_color, true);

    /* Direction indicator */
    float dir_x = cosf(g_minimap.player_facing) * 6;
    float dir_y = sinf(g_minimap.player_facing) * 6;
    Rect dir = {player_x + dir_x - 2, player_y + dir_y - 2, 4, 4};
    renderer_draw_rect(renderer, dir, player_color, true);

    /* Map name */
    renderer_draw_text(renderer, g_minimap.map_name,
                       (int)g_minimap.x + 4, (int)g_minimap.y + g_minimap.size + 2,
                       text_color);

    /* Coordinates */
    char coords[32];
    snprintf(coords, sizeof(coords), "%.0f, %.0f", g_minimap.player_x, g_minimap.player_y);
    renderer_draw_text(renderer, coords,
                       (int)g_minimap.x + g_minimap.size - 60,
                       (int)g_minimap.y + g_minimap.size + 2, text_color);
}

/*
 * ============================================================================
 * Options
 * ============================================================================
 */

void ui_minimap_toggle(void)
{
    g_minimap.visible = !g_minimap.visible;
}

void ui_minimap_set_zoom(float zoom)
{
    g_minimap.zoom = zoom;
    if (g_minimap.zoom < g_minimap.min_zoom) g_minimap.zoom = g_minimap.min_zoom;
    if (g_minimap.zoom > g_minimap.max_zoom) g_minimap.zoom = g_minimap.max_zoom;
}

void ui_minimap_toggle_enemies(void)
{
    g_minimap.show_enemies = !g_minimap.show_enemies;
}

void ui_minimap_toggle_npcs(void)
{
    g_minimap.show_npcs = !g_minimap.show_npcs;
}

bool ui_minimap_is_visible(void)
{
    return g_minimap.visible;
}
