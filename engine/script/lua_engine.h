/**
 * @file lua_engine.h
 * @brief Lua scripting engine
 *
 * Provides Lua 5.1 integration for game scripting.
 */

#ifndef ENGINE_SCRIPT_LUA_ENGINE_H
#define ENGINE_SCRIPT_LUA_ENGINE_H

#include "../core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct lua_State;
typedef struct lua_State lua_State;

#define LUA_SCRIPT_PATH_MAX     256
#define LUA_ERROR_MSG_MAX       512
#define LUA_MAX_SCRIPTS         256
#define LUA_GC_INTERVAL         30000   /* 30 seconds */

/* Script state */
typedef enum {
    SCRIPT_STATE_UNLOADED = 0,
    SCRIPT_STATE_LOADED,
    SCRIPT_STATE_ERROR
} ScriptState;

/* Script info */
typedef struct {
    char            path[LUA_SCRIPT_PATH_MAX];
    ScriptState     state;
    u32             load_time;
    u32             last_modified;
    u32             call_count;
    u32             error_count;
} ScriptInfo;

/* Lua engine context */
typedef struct {
    lua_State*      L;
    char            error_msg[LUA_ERROR_MSG_MAX];
    ScriptInfo      scripts[LUA_MAX_SCRIPTS];
    int             script_count;
    u32             last_gc;
    u8              initialized;
} LuaEngine;

/**
 * Initialize Lua engine
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_engine_init(LuaEngine* engine);

/**
 * Cleanup Lua engine
 * @param engine Engine instance
 */
void lua_engine_cleanup(LuaEngine* engine);

/**
 * Load a Lua script file
 * @param engine Engine instance
 * @param path   Script file path
 * @return ENGINE_OK on success
 */
int lua_engine_load_file(LuaEngine* engine, const char* path);

/**
 * Execute a Lua string
 * @param engine Engine instance
 * @param code   Lua code string
 * @return ENGINE_OK on success
 */
int lua_engine_exec(LuaEngine* engine, const char* code);

/**
 * Call a Lua function with no arguments
 * @param engine Engine instance
 * @param func   Function name
 * @return ENGINE_OK on success
 */
int lua_engine_call(LuaEngine* engine, const char* func);

/**
 * Call a Lua function with integer argument
 * @param engine Engine instance
 * @param func   Function name
 * @param arg    Integer argument
 * @return ENGINE_OK on success
 */
int lua_engine_call_int(LuaEngine* engine, const char* func, i32 arg);

/**
 * Call a Lua function with two integer arguments
 * @param engine Engine instance
 * @param func   Function name
 * @param arg1   First argument
 * @param arg2   Second argument
 * @return ENGINE_OK on success
 */
int lua_engine_call_int2(LuaEngine* engine, const char* func, i32 arg1, i32 arg2);

/**
 * Call a Lua function with string argument
 * @param engine Engine instance
 * @param func   Function name
 * @param arg    String argument
 * @return ENGINE_OK on success
 */
int lua_engine_call_str(LuaEngine* engine, const char* func, const char* arg);

/**
 * Call a Lua function with integer argument and get integer result
 * @param engine Engine instance
 * @param func   Function name
 * @param arg    Integer argument
 * @param result Output result
 * @return ENGINE_OK on success
 */
int lua_engine_call_int_ret(LuaEngine* engine, const char* func,
                            i32 arg, i32* result);

/**
 * Set global integer variable
 * @param engine Engine instance
 * @param name   Variable name
 * @param value  Value
 */
void lua_engine_set_int(LuaEngine* engine, const char* name, i32 value);

/**
 * Set global string variable
 * @param engine Engine instance
 * @param name   Variable name
 * @param value  Value
 */
void lua_engine_set_string(LuaEngine* engine, const char* name, const char* value);

/**
 * Get global integer variable
 * @param engine Engine instance
 * @param name   Variable name
 * @return Value, or 0 if not found
 */
i32 lua_engine_get_int(LuaEngine* engine, const char* name);

/**
 * Get global string variable
 * @param engine Engine instance
 * @param name   Variable name
 * @param buf    Output buffer
 * @param len    Buffer length
 * @return 1 if found, 0 otherwise
 */
int lua_engine_get_string(LuaEngine* engine, const char* name,
                          char* buf, int len);

/**
 * Run garbage collection
 * @param engine Engine instance
 */
void lua_engine_gc(LuaEngine* engine);

/**
 * Get last error message
 * @param engine Engine instance
 * @return Error message string
 */
const char* lua_engine_get_error(const LuaEngine* engine);

/**
 * Check if function exists
 * @param engine Engine instance
 * @param func   Function name
 * @return 1 if exists, 0 otherwise
 */
int lua_engine_has_function(LuaEngine* engine, const char* func);

/**
 * Get Lua state (for advanced usage)
 * @param engine Engine instance
 * @return Lua state pointer
 */
lua_State* lua_engine_state(LuaEngine* engine);

/**
 * Update engine (GC, hot reload check, etc.)
 * @param engine       Engine instance
 * @param current_time Current time
 */
void lua_engine_update(LuaEngine* engine, u32 current_time);

/*
 * Binding registration functions
 */

/**
 * Register combat bindings
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_register_combat(LuaEngine* engine);

/**
 * Register skill bindings
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_register_skill(LuaEngine* engine);

/**
 * Register event bindings
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_register_event(LuaEngine* engine);

/**
 * Register entity bindings
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_register_entity(LuaEngine* engine);

/**
 * Register all game bindings
 * @param engine Engine instance
 * @return ENGINE_OK on success
 */
int lua_register_all(LuaEngine* engine);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_SCRIPT_LUA_ENGINE_H */
