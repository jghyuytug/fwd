/**
 * @file lua_event.c
 * @brief Event system Lua bindings
 */

#include "lua_bindings.h"
#include "../../core/event/event_bus.h"

#ifdef USE_LUA

/* Global event bus reference */
static EventBus* g_lua_event_bus = NULL;

/*
 * event.set_bus(bus_ptr)
 * Internal function to set the event bus
 */
void lua_event_set_bus(EventBus* bus)
{
    g_lua_event_bus = bus;
}

/*
 * event.emit(event_type, param1, param2, param3, param4)
 * Emits an event
 */
static int l_event_emit(lua_State* L)
{
    int event_type;
    Event evt;

    if (!g_lua_event_bus) {
        lua_pushboolean(L, 0);
        return 1;
    }

    LUA_CHECK_ARGS(L, 1);
    event_type = LUA_GET_INT(L, 1);

    memset(&evt, 0, sizeof(evt));
    evt.type = event_type;
    evt.param1 = LUA_OPT_INT(L, 2, 0);
    evt.param2 = LUA_OPT_INT(L, 3, 0);
    evt.param3 = LUA_OPT_INT(L, 4, 0);
    evt.param4 = LUA_OPT_INT(L, 5, 0);

    event_bus_emit(g_lua_event_bus, &evt);
    lua_pushboolean(L, 1);
    return 1;
}

/*
 * event.emit_delayed(event_type, delay_ms, param1, param2)
 * Emits an event after delay
 */
static int l_event_emit_delayed(lua_State* L)
{
    int event_type;
    u32 delay_ms;
    Event evt;
    int ret;

    if (!g_lua_event_bus) {
        lua_pushboolean(L, 0);
        return 1;
    }

    LUA_CHECK_ARGS(L, 2);
    event_type = LUA_GET_INT(L, 1);
    delay_ms = (u32)LUA_GET_INT(L, 2);

    memset(&evt, 0, sizeof(evt));
    evt.type = event_type;
    evt.param1 = LUA_OPT_INT(L, 3, 0);
    evt.param2 = LUA_OPT_INT(L, 4, 0);

    ret = event_bus_emit_delayed(g_lua_event_bus, &evt, delay_ms);
    lua_pushboolean(L, ret == ENGINE_OK);
    return 1;
}

/*
 * event.queue(event_type, param1, param2, param3, param4)
 * Queues an event for later processing
 */
static int l_event_queue(lua_State* L)
{
    int event_type;
    Event evt;
    int ret;

    if (!g_lua_event_bus) {
        lua_pushboolean(L, 0);
        return 1;
    }

    LUA_CHECK_ARGS(L, 1);
    event_type = LUA_GET_INT(L, 1);

    memset(&evt, 0, sizeof(evt));
    evt.type = event_type;
    evt.param1 = LUA_OPT_INT(L, 2, 0);
    evt.param2 = LUA_OPT_INT(L, 3, 0);
    evt.param3 = LUA_OPT_INT(L, 4, 0);
    evt.param4 = LUA_OPT_INT(L, 5, 0);

    ret = event_bus_queue(g_lua_event_bus, &evt);
    lua_pushboolean(L, ret == ENGINE_OK);
    return 1;
}

/* Registration function */
int lua_register_event(LuaEngine* engine)
{
    lua_State* L;

    if (!engine || !engine->L) {
        return ENGINE_ERROR_PARAM;
    }

    L = engine->L;

    /* Create event table */
    lua_newtable(L);

    lua_pushcfunction(L, l_event_emit);
    lua_setfield(L, -2, "emit");

    lua_pushcfunction(L, l_event_emit_delayed);
    lua_setfield(L, -2, "emit_delayed");

    lua_pushcfunction(L, l_event_queue);
    lua_setfield(L, -2, "queue");

    /* Event type constants */
    lua_pushinteger(L, EVENT_PLAYER_JOIN);
    lua_setfield(L, -2, "PLAYER_JOIN");

    lua_pushinteger(L, EVENT_PLAYER_LEAVE);
    lua_setfield(L, -2, "PLAYER_LEAVE");

    lua_pushinteger(L, EVENT_PLAYER_MOVE);
    lua_setfield(L, -2, "PLAYER_MOVE");

    lua_pushinteger(L, EVENT_PLAYER_ATTACK);
    lua_setfield(L, -2, "PLAYER_ATTACK");

    lua_pushinteger(L, EVENT_PLAYER_DEATH);
    lua_setfield(L, -2, "PLAYER_DEATH");

    lua_pushinteger(L, EVENT_MONSTER_SPAWN);
    lua_setfield(L, -2, "MONSTER_SPAWN");

    lua_pushinteger(L, EVENT_MONSTER_DEATH);
    lua_setfield(L, -2, "MONSTER_DEATH");

    lua_pushinteger(L, EVENT_SKILL_USE);
    lua_setfield(L, -2, "SKILL_USE");

    lua_pushinteger(L, EVENT_BUFF_ADD);
    lua_setfield(L, -2, "BUFF_ADD");

    lua_pushinteger(L, EVENT_BUFF_REMOVE);
    lua_setfield(L, -2, "BUFF_REMOVE");

    lua_pushinteger(L, EVENT_ITEM_PICKUP);
    lua_setfield(L, -2, "ITEM_PICKUP");

    lua_pushinteger(L, EVENT_ITEM_DROP);
    lua_setfield(L, -2, "ITEM_DROP");

    lua_pushinteger(L, EVENT_DUNGEON_START);
    lua_setfield(L, -2, "DUNGEON_START");

    lua_pushinteger(L, EVENT_DUNGEON_CLEAR);
    lua_setfield(L, -2, "DUNGEON_CLEAR");

    lua_pushinteger(L, EVENT_ROOM_ENTER);
    lua_setfield(L, -2, "ROOM_ENTER");

    lua_pushinteger(L, EVENT_ROOM_CLEAR);
    lua_setfield(L, -2, "ROOM_CLEAR");

    lua_setglobal(L, "event");

    return ENGINE_OK;
}

#else /* !USE_LUA */

int lua_register_event(LuaEngine* engine)
{
    (void)engine;
    return ENGINE_OK;
}

#endif /* USE_LUA */
