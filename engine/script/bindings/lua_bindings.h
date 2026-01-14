/**
 * @file lua_bindings.h
 * @brief Common header for Lua bindings
 */

#ifndef ENGINE_SCRIPT_BINDINGS_LUA_BINDINGS_H
#define ENGINE_SCRIPT_BINDINGS_LUA_BINDINGS_H

#include "../../core/types.h"
#include "../lua_engine.h"

#ifdef USE_LUA
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Helper macros for binding registration
 */
#ifdef USE_LUA

#define LUA_REG_FUNC(L, name, func) \
    lua_pushcfunction(L, func); \
    lua_setglobal(L, name)

#define LUA_REG_INT(L, name, val) \
    lua_pushinteger(L, val); \
    lua_setglobal(L, name)

#define LUA_CHECK_ARGS(L, n) \
    if (lua_gettop(L) < n) { \
        lua_pushnil(L); \
        lua_pushstring(L, "insufficient arguments"); \
        return 2; \
    }

#define LUA_GET_INT(L, n) ((i32)luaL_checkinteger(L, n))
#define LUA_GET_FLOAT(L, n) ((float)luaL_checknumber(L, n))
#define LUA_GET_STRING(L, n) luaL_checkstring(L, n)
#define LUA_OPT_INT(L, n, def) ((i32)luaL_optinteger(L, n, def))

#endif /* USE_LUA */

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_SCRIPT_BINDINGS_LUA_BINDINGS_H */
