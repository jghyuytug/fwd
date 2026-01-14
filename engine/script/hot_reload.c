/**
 * @file hot_reload.c
 * @brief Script hot reload system implementation
 */

#include "hot_reload.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static u32 get_file_mtime(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0) {
        return 0;
    }

    return (u32)st.st_mtime;
}

int hot_reload_init(HotReload* hr, LuaEngine* engine)
{
    if (!hr || !engine) {
        return ENGINE_ERROR_PARAM;
    }

    memset(hr, 0, sizeof(HotReload));
    hr->engine = engine;
    hr->enabled = 1;

    return ENGINE_OK;
}

void hot_reload_cleanup(HotReload* hr)
{
    if (!hr) return;
    memset(hr, 0, sizeof(HotReload));
}

int hot_reload_watch(HotReload* hr, const char* path)
{
    int i;
    WatchEntry* entry;

    if (!hr || !path) {
        return ENGINE_ERROR_PARAM;
    }

    /* Check if already watching */
    for (i = 0; i < hr->watch_count; i++) {
        if (hr->watches[i].active &&
            strcmp(hr->watches[i].path, path) == 0) {
            return ENGINE_OK;  /* Already watching */
        }
    }

    /* Find free slot */
    entry = NULL;
    for (i = 0; i < HOT_RELOAD_MAX_WATCH; i++) {
        if (!hr->watches[i].active) {
            entry = &hr->watches[i];
            break;
        }
    }

    if (!entry) {
        if (hr->watch_count >= HOT_RELOAD_MAX_WATCH) {
            return ENGINE_ERROR_FULL;
        }
        entry = &hr->watches[hr->watch_count];
    }

    /* Setup watch */
    strncpy(entry->path, path, LUA_SCRIPT_PATH_MAX - 1);
    entry->last_modified = get_file_mtime(path);
    entry->active = 1;

    if (i >= hr->watch_count) {
        hr->watch_count++;
    }

    return ENGINE_OK;
}

void hot_reload_unwatch(HotReload* hr, const char* path)
{
    int i;

    if (!hr || !path) return;

    for (i = 0; i < hr->watch_count; i++) {
        if (hr->watches[i].active &&
            strcmp(hr->watches[i].path, path) == 0) {
            hr->watches[i].active = 0;
            break;
        }
    }
}

int hot_reload_check(HotReload* hr, u32 current_time)
{
    int i;
    int reloaded = 0;
    u32 mtime;

    if (!hr || !hr->engine) return 0;
    if (!hr->enabled) return 0;

    /* Throttle checks */
    if (current_time - hr->last_check < HOT_RELOAD_CHECK_INTERVAL) {
        return 0;
    }
    hr->last_check = current_time;

    /* Check each watched file */
    for (i = 0; i < hr->watch_count; i++) {
        if (!hr->watches[i].active) continue;

        mtime = get_file_mtime(hr->watches[i].path);
        if (mtime == 0) continue;  /* File not found */

        if (mtime > hr->watches[i].last_modified) {
            /* File changed, reload */
            printf("[HotReload] Reloading: %s\n", hr->watches[i].path);

            if (lua_engine_load_file(hr->engine, hr->watches[i].path) == ENGINE_OK) {
                hr->watches[i].last_modified = mtime;
                reloaded++;
            } else {
                printf("[HotReload] Failed: %s\n",
                       lua_engine_get_error(hr->engine));
            }
        }
    }

    return reloaded;
}

int hot_reload_force(HotReload* hr, const char* path)
{
    int i;

    if (!hr || !hr->engine || !path) {
        return ENGINE_ERROR_PARAM;
    }

    /* Find watched entry */
    for (i = 0; i < hr->watch_count; i++) {
        if (hr->watches[i].active &&
            strcmp(hr->watches[i].path, path) == 0) {
            /* Force reload */
            if (lua_engine_load_file(hr->engine, path) == ENGINE_OK) {
                hr->watches[i].last_modified = get_file_mtime(path);
                return ENGINE_OK;
            }
            return ENGINE_ERROR;
        }
    }

    /* Not watching, just load */
    return lua_engine_load_file(hr->engine, path);
}

void hot_reload_set_enabled(HotReload* hr, int enabled)
{
    if (!hr) return;
    hr->enabled = enabled ? 1 : 0;
}

int hot_reload_all(HotReload* hr)
{
    int i;
    int reloaded = 0;

    if (!hr || !hr->engine) return 0;

    for (i = 0; i < hr->watch_count; i++) {
        if (!hr->watches[i].active) continue;

        printf("[HotReload] Reloading: %s\n", hr->watches[i].path);

        if (lua_engine_load_file(hr->engine, hr->watches[i].path) == ENGINE_OK) {
            hr->watches[i].last_modified = get_file_mtime(hr->watches[i].path);
            reloaded++;
        } else {
            printf("[HotReload] Failed: %s\n",
                   lua_engine_get_error(hr->engine));
        }
    }

    return reloaded;
}
