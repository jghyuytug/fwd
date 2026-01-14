/**
 * @file scene.h
 * @brief Scene management system
 */

#ifndef CLIENT_SCENE_H
#define CLIENT_SCENE_H

#include "client.h"
#include "renderer.h"
#include "entity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAP_MAX_LAYERS      4
#define MAP_MAX_WIDTH       256
#define MAP_MAX_HEIGHT      32

/* Tile types */
typedef enum {
    TILE_EMPTY = 0,
    TILE_GROUND,
    TILE_PLATFORM,      /* Can jump through */
    TILE_WALL,
    TILE_SLOPE_UP,
    TILE_SLOPE_DOWN,
    TILE_WATER,
    TILE_LAVA
} TileType;

/* Map tile */
typedef struct {
    u16         sprite_id;
    TileType    type;
    u8          flags;
} Tile;

/* Map layer */
typedef struct {
    Tile        tiles[MAP_MAX_WIDTH][MAP_MAX_HEIGHT];
    float       parallax_x;
    float       parallax_y;
    bool        visible;
} MapLayer;

/* Game map */
typedef struct {
    u32         id;
    char        name[64];
    int         width;
    int         height;
    MapLayer    layers[MAP_MAX_LAYERS];
    int         layer_count;
    Sprite*     tileset;
    Color       bg_color;
    SDL_Texture* background;
} GameMap;

/* Scene types */
typedef enum {
    SCENE_NONE = 0,
    SCENE_LOGIN,
    SCENE_CHARSELECT,
    SCENE_TOWN,
    SCENE_DUNGEON
} SceneType;

/* Scene base */
typedef struct Scene {
    SceneType       type;
    bool            initialized;

    /* Virtual functions */
    int (*init)(struct Scene* scene);
    void (*cleanup)(struct Scene* scene);
    void (*update)(struct Scene* scene, float delta);
    void (*draw)(struct Scene* scene, Renderer* renderer);
    void (*handle_input)(struct Scene* scene, SDL_Event* event);
    void (*on_packet)(struct Scene* scene, u16 msg_no, u8* data, int len);

    /* Scene data */
    void*           data;
} Scene;

/* Scene manager */
typedef struct SceneManager {
    Scene*          current;
    Scene*          next;
    bool            transitioning;
    float           transition_alpha;
} SceneManager;

/**
 * Initialize scene manager
 * @param manager Scene manager instance
 * @return CLIENT_OK on success
 */
int scene_manager_init(SceneManager* manager);

/**
 * Cleanup scene manager
 * @param manager Scene manager instance
 */
void scene_manager_cleanup(SceneManager* manager);

/**
 * Change to new scene
 * @param manager Scene manager instance
 * @param type    Scene type
 */
void scene_manager_change(SceneManager* manager, SceneType type);

/**
 * Update current scene
 * @param manager Scene manager instance
 * @param delta   Delta time
 */
void scene_manager_update(SceneManager* manager, float delta);

/**
 * Draw current scene
 * @param manager  Scene manager instance
 * @param renderer Renderer instance
 */
void scene_manager_draw(SceneManager* manager, Renderer* renderer);

/**
 * Handle input for current scene
 * @param manager Scene manager instance
 * @param event   SDL event
 */
void scene_manager_handle_input(SceneManager* manager, SDL_Event* event);

/**
 * Handle network packet for current scene
 * @param manager Scene manager instance
 * @param msg_no  Message number
 * @param data    Packet data
 * @param len     Data length
 */
void scene_manager_on_packet(SceneManager* manager, u16 msg_no, u8* data, int len);

/*
 * Scene creation functions
 */

/**
 * Create login scene
 * @return Scene pointer
 */
Scene* scene_create_login(void);

/**
 * Create character select scene
 * @return Scene pointer
 */
Scene* scene_create_charselect(void);

/**
 * Create town scene
 * @return Scene pointer
 */
Scene* scene_create_town(void);

/**
 * Create dungeon scene
 * @return Scene pointer
 */
Scene* scene_create_dungeon(void);

/*
 * Map functions
 */

/**
 * Load map from file
 * @param renderer Renderer instance
 * @param path     Map file path
 * @return GameMap or NULL
 */
GameMap* map_load(Renderer* renderer, const char* path);

/**
 * Free map
 * @param map Map instance
 */
void map_free(GameMap* map);

/**
 * Draw map
 * @param map      Map instance
 * @param renderer Renderer instance
 * @param camera_x Camera X position
 * @param camera_y Camera Y position
 */
void map_draw(GameMap* map, Renderer* renderer, float camera_x, float camera_y);

/**
 * Get tile at position
 * @param map   Map instance
 * @param layer Layer index
 * @param x     Tile X
 * @param y     Tile Y
 * @return Tile pointer or NULL
 */
Tile* map_get_tile(GameMap* map, int layer, int x, int y);

/**
 * Check collision with map
 * @param map  Map instance
 * @param rect Collision rect
 * @return true if collision
 */
bool map_check_collision(GameMap* map, Rect rect);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_SCENE_H */
