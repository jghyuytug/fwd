/**
 * @file lua_engine.c
 * @brief Lua scripting engine implementation
 */

#include "lua_engine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_LUA
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif

static u32 get_time_ms(void)
{
    return 0;  /* Platform-specific implementation needed */
}

int lua_engine_init(LuaEngine* engine)
{
    if (!engine) {
        return ENGINE_ERROR_PARAM;
    }

    memset(engine, 0, sizeof(LuaEngine));

#ifdef USE_LUA
    engine->L = luaL_newstate();
    if (!engine->L) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Failed to create Lua state");
        return ENGINE_ERROR_MEMORY;
    }

    /* Open standard libraries */
    luaL_openlibs(engine->L);

    engine->initialized = 1;
    engine->last_gc = get_time_ms();

    return ENGINE_OK;
#else
    snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
            "Lua support not compiled (USE_LUA not defined)");
    return ENGINE_ERROR;
#endif
}

void lua_engine_cleanup(LuaEngine* engine)
{
    if (!engine) return;

#ifdef USE_LUA
    if (engine->L) {
        lua_close(engine->L);
        engine->L = NULL;
    }
#endif

    engine->initialized = 0;
}

int lua_engine_load_file(LuaEngine* engine, const char* path)
{
#ifdef USE_LUA
    int ret;
    int i;
    ScriptInfo* info = NULL;

    if (!engine || !engine->L || !path) {
        return ENGINE_ERROR_PARAM;
    }

    /* Find or create script info */
    for (i = 0; i < engine->script_count; i++) {
        if (strcmp(engine->scripts[i].path, path) == 0) {
            info = &engine->scripts[i];
            break;
        }
    }

    if (!info) {
        if (engine->script_count >= LUA_MAX_SCRIPTS) {
            snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                    "Too many scripts loaded");
            return ENGINE_ERROR_FULL;
        }
        info = &engine->scripts[engine->script_count];
        engine->script_count++;
        strncpy(info->path, path, LUA_SCRIPT_PATH_MAX - 1);
    }

    /* Load and run file */
    ret = luaL_loadfile(engine->L, path);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Load error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        info->state = SCRIPT_STATE_ERROR;
        info->error_count++;
        return ENGINE_ERROR_IO;
    }

    ret = lua_pcall(engine->L, 0, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Run error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        info->state = SCRIPT_STATE_ERROR;
        info->error_count++;
        return ENGINE_ERROR;
    }

    info->state = SCRIPT_STATE_LOADED;
    info->load_time = get_time_ms();

    return ENGINE_OK;
#else
    (void)engine;
    (void)path;
    return ENGINE_ERROR;
#endif
}

int lua_engine_exec(LuaEngine* engine, const char* code)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !code) {
        return ENGINE_ERROR_PARAM;
    }

    ret = luaL_loadstring(engine->L, code);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Compile error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    ret = lua_pcall(engine->L, 0, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Exec error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    return ENGINE_OK;
#else
    (void)engine;
    (void)code;
    return ENGINE_ERROR;
#endif
}

int lua_engine_call(LuaEngine* engine, const char* func)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !func) {
        return ENGINE_ERROR_PARAM;
    }

    lua_getglobal(engine->L, func);
    if (!lua_isfunction(engine->L, -1)) {
        lua_pop(engine->L, 1);
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Function not found: %s", func);
        return ENGINE_ERROR;
    }

    ret = lua_pcall(engine->L, 0, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Call error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    return ENGINE_OK;
#else
    (void)engine;
    (void)func;
    return ENGINE_ERROR;
#endif
}

int lua_engine_call_int(LuaEngine* engine, const char* func, i32 arg)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !func) {
        return ENGINE_ERROR_PARAM;
    }

    lua_getglobal(engine->L, func);
    if (!lua_isfunction(engine->L, -1)) {
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    lua_pushinteger(engine->L, arg);
    ret = lua_pcall(engine->L, 1, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Call error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    return ENGINE_OK;
#else
    (void)engine;
    (void)func;
    (void)arg;
    return ENGINE_ERROR;
#endif
}

int lua_engine_call_int2(LuaEngine* engine, const char* func, i32 arg1, i32 arg2)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !func) {
        return ENGINE_ERROR_PARAM;
    }

    lua_getglobal(engine->L, func);
    if (!lua_isfunction(engine->L, -1)) {
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    lua_pushinteger(engine->L, arg1);
    lua_pushinteger(engine->L, arg2);
    ret = lua_pcall(engine->L, 2, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Call error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    return ENGINE_OK;
#else
    (void)engine;
    (void)func;
    (void)arg1;
    (void)arg2;
    return ENGINE_ERROR;
#endif
}

int lua_engine_call_str(LuaEngine* engine, const char* func, const char* arg)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !func) {
        return ENGINE_ERROR_PARAM;
    }

    lua_getglobal(engine->L, func);
    if (!lua_isfunction(engine->L, -1)) {
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    lua_pushstring(engine->L, arg ? arg : "");
    ret = lua_pcall(engine->L, 1, 0, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Call error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    return ENGINE_OK;
#else
    (void)engine;
    (void)func;
    (void)arg;
    return ENGINE_ERROR;
#endif
}

int lua_engine_call_int_ret(LuaEngine* engine, const char* func,
                            i32 arg, i32* result)
{
#ifdef USE_LUA
    int ret;

    if (!engine || !engine->L || !func || !result) {
        return ENGINE_ERROR_PARAM;
    }

    lua_getglobal(engine->L, func);
    if (!lua_isfunction(engine->L, -1)) {
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    lua_pushinteger(engine->L, arg);
    ret = lua_pcall(engine->L, 1, 1, 0);
    if (ret != 0) {
        snprintf(engine->error_msg, LUA_ERROR_MSG_MAX,
                "Call error: %s", lua_tostring(engine->L, -1));
        lua_pop(engine->L, 1);
        return ENGINE_ERROR;
    }

    *result = (i32)lua_tointeger(engine->L, -1);
    lua_pop(engine->L, 1);

    return ENGINE_OK;
#else
    (void)engine;
    (void)func;
    (void)arg;
    (void)result;
    return ENGINE_ERROR;
#endif
}

void lua_engine_set_int(LuaEngine* engine, const char* name, i32 value)
{
#ifdef USE_LUA
    if (!engine || !engine->L || !name) return;

    lua_pushinteger(engine->L, value);
    lua_setglobal(engine->L, name);
#else
    (void)engine;
    (void)name;
    (void)value;
#endif
}

void lua_engine_set_string(LuaEngine* engine, const char* name, const char* value)
{
#ifdef USE_LUA
    if (!engine || !engine->L || !name) return;

    lua_pushstring(engine->L, value ? value : "");
    lua_setglobal(engine->L, name);
#else
    (void)engine;
    (void)name;
    (void)value;
#endif
}

i32 lua_engine_get_int(LuaEngine* engine, const char* name)
{
#ifdef USE_LUA
    i32 value;

    if (!engine || !engine->L || !name) return 0;

    lua_getglobal(engine->L, name);
    value = (i32)lua_tointeger(engine->L, -1);
    lua_pop(engine->L, 1);

    return value;
#else
    (void)engine;
    (void)name;
    return 0;
#endif
}

int lua_engine_get_string(LuaEngine* engine, const char* name,
                          char* buf, int len)
{
#ifdef USE_LUA
    const char* str;

    if (!engine || !engine->L || !name || !buf || len <= 0) return 0;

    lua_getglobal(engine->L, name);
    if (!lua_isstring(engine->L, -1)) {
        lua_pop(engine->L, 1);
        return 0;
    }

    str = lua_tostring(engine->L, -1);
    strncpy(buf, str ? str : "", len - 1);
    buf[len - 1] = '\0';
    lua_pop(engine->L, 1);

    return 1;
#else
    (void)engine;
    (void)name;
    (void)buf;
    (void)len;
    return 0;
#endif
}

void lua_engine_gc(LuaEngine* engine)
{
#ifdef USE_LUA
    if (!engine || !engine->L) return;
    lua_gc(engine->L, LUA_GCCOLLECT, 0);
    engine->last_gc = get_time_ms();
#else
    (void)engine;
#endif
}

const char* lua_engine_get_error(const LuaEngine* engine)
{
    if (!engine) return "NULL engine";
    return engine->error_msg;
}

int lua_engine_has_function(LuaEngine* engine, const char* func)
{
#ifdef USE_LUA
    int result;

    if (!engine || !engine->L || !func) return 0;

    lua_getglobal(engine->L, func);
    result = lua_isfunction(engine->L, -1);
    lua_pop(engine->L, 1);

    return result;
#else
    (void)engine;
    (void)func;
    return 0;
#endif
}

lua_State* lua_engine_state(LuaEngine* engine)
{
#ifdef USE_LUA
    if (!engine) return NULL;
    return engine->L;
#else
    (void)engine;
    return NULL;
#endif
}

void lua_engine_update(LuaEngine* engine, u32 current_time)
{
    if (!engine || !engine->initialized) return;

    /* Periodic GC */
    if (current_time - engine->last_gc >= LUA_GC_INTERVAL) {
        lua_engine_gc(engine);
    }
}

/* Binding registration stubs - to be implemented in separate files */

int lua_register_combat(LuaEngine* engine)
{
    (void)engine;
    /* Implemented in bindings/lua_combat.c */
    return ENGINE_OK;
}

int lua_register_skill(LuaEngine* engine)
{
    (void)engine;
    /* Implemented in bindings/lua_skill.c */
    return ENGINE_OK;
}

int lua_register_event(LuaEngine* engine)
{
    (void)engine;
    /* Implemented in bindings/lua_event.c */
    return ENGINE_OK;
}

int lua_register_entity(LuaEngine* engine)
{
    (void)engine;
    /* Implemented in bindings/lua_entity.c */
    return ENGINE_OK;
}

int lua_register_all(LuaEngine* engine)
{
    int ret;

    ret = lua_register_combat(engine);
    if (ret != ENGINE_OK) return ret;

    ret = lua_register_skill(engine);
    if (ret != ENGINE_OK) return ret;

    ret = lua_register_event(engine);
    if (ret != ENGINE_OK) return ret;

    ret = lua_register_entity(engine);
    if (ret != ENGINE_OK) return ret;

    return ENGINE_OK;
}
