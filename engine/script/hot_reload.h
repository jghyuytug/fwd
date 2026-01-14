/**
 * @file hot_reload.h
 * @brief Script hot reload system
 *
 * Monitors scripts for changes and reloads them automatically.
 */

#ifndef ENGINE_SCRIPT_HOT_RELOAD_H
#define ENGINE_SCRIPT_HOT_RELOAD_H

#include "../core/types.h"
#include "lua_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HOT_RELOAD_MAX_WATCH    64
#define HOT_RELOAD_CHECK_INTERVAL 1000  /* 1 second */

/* Watch entry */
typedef struct {
    char    path[LUA_SCRIPT_PATH_MAX];
    u32     last_modified;
    u8      active;
} WatchEntry;

/* Hot reload context */
typedef struct {
    LuaEngine*  engine;
    WatchEntry  watches[HOT_RELOAD_MAX_WATCH];
    int         watch_count;
    u32         last_check;
    u8          enabled;
} HotReload;

/**
 * Initialize hot reload system
 * @param hr     Hot reload instance
 * @param engine Lua engine
 * @return ENGINE_OK on success
 */
int hot_reload_init(HotReload* hr, LuaEngine* engine);

/**
 * Cleanup hot reload system
 * @param hr Hot reload instance
 */
void hot_reload_cleanup(HotReload* hr);

/**
 * Add file to watch list
 * @param hr   Hot reload instance
 * @param path Script path
 * @return ENGINE_OK on success
 */
int hot_reload_watch(HotReload* hr, const char* path);

/**
 * Remove file from watch list
 * @param hr   Hot reload instance
 * @param path Script path
 */
void hot_reload_unwatch(HotReload* hr, const char* path);

/**
 * Check for changes and reload if needed
 * @param hr           Hot reload instance
 * @param current_time Current time
 * @return Number of scripts reloaded
 */
int hot_reload_check(HotReload* hr, u32 current_time);

/**
 * Force reload a specific script
 * @param hr   Hot reload instance
 * @param path Script path
 * @return ENGINE_OK on success
 */
int hot_reload_force(HotReload* hr, const char* path);

/**
 * Enable/disable hot reload
 * @param hr      Hot reload instance
 * @param enabled Enable state
 */
void hot_reload_set_enabled(HotReload* hr, int enabled);

/**
 * Reload all watched scripts
 * @param hr Hot reload instance
 * @return Number of scripts reloaded
 */
int hot_reload_all(HotReload* hr);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_SCRIPT_HOT_RELOAD_H */
