/**
 * @file lua_entity.c
 * @brief Entity system Lua bindings
 */

#include "lua_bindings.h"
#include "../../game/entity/entity.h"

#ifdef USE_LUA

/* Entity registry - shared with lua_combat.c */
extern Entity* g_entity_registry[1024];
extern int g_entity_count;

static Entity* get_entity(u32 id)
{
    int i;
    for (i = 0; i < g_entity_count; i++) {
        if (g_entity_registry[i] && g_entity_registry[i]->id == id) {
            return g_entity_registry[i];
        }
    }
    return NULL;
}

/*
 * entity.get_hp(entity_id)
 * Returns: hp, max_hp
 */
static int l_entity_get_hp(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->hp);
    lua_pushinteger(L, e->max_hp);
    return 2;
}

/*
 * entity.get_mp(entity_id)
 * Returns: mp, max_mp
 */
static int l_entity_get_mp(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->mp);
    lua_pushinteger(L, e->max_mp);
    return 2;
}

/*
 * entity.get_position(entity_id)
 * Returns: x, y, z
 */
static int l_entity_get_position(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->pos_x);
    lua_pushinteger(L, e->pos_y);
    lua_pushinteger(L, e->pos_z);
    return 3;
}

/*
 * entity.get_level(entity_id)
 * Returns: level
 */
static int l_entity_get_level(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->level);
    return 1;
}

/*
 * entity.get_state(entity_id)
 * Returns: state (number), state_name (string)
 */
static int l_entity_get_state(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->state);
    lua_pushstring(L, entity_state_name(e->state));
    return 2;
}

/*
 * entity.is_alive(entity_id)
 * Returns: boolean
 */
static int l_entity_is_alive(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, entity_is_alive(e));
    return 1;
}

/*
 * entity.get_type(entity_id)
 * Returns: type (number), type_name (string)
 */
static int l_entity_get_type(lua_State* L)
{
    u32 id;
    Entity* e;

    LUA_CHECK_ARGS(L, 1);
    id = (u32)LUA_GET_INT(L, 1);
    e = get_entity(id);

    if (!e) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, e->type);
    lua_pushstring(L, entity_type_name(e->type));
    return 2;
}

/*
 * entity.get_stat(entity_id, stat_name)
 * Returns: value
 */
static int l_entity_get_stat(lua_State* L)
{
    u32 id;
    const char* stat;
    Entity* e;
    i32 value = 0;

    LUA_CHECK_ARGS(L, 2);
    id = (u32)LUA_GET_INT(L, 1);
    stat = LUA_GET_STRING(L, 2);
    e = get_entity(id);

    if (!e || !stat) {
        lua_pushnil(L);
        return 1;
    }

    if (strcmp(stat, "strength") == 0) {
        value = e->final_stats.strength;
    } else if (strcmp(stat, "intelligence") == 0) {
        value = e->final_stats.intelligence;
    } else if (strcmp(stat, "vitality") == 0) {
        value = e->final_stats.vitality;
    } else if (strcmp(stat, "spirit") == 0) {
        value = e->final_stats.spirit;
    } else if (strcmp(stat, "phys_attack") == 0) {
        value = e->final_stats.physical_attack;
    } else if (strcmp(stat, "mag_attack") == 0) {
        value = e->final_stats.magical_attack;
    } else if (strcmp(stat, "phys_defense") == 0) {
        value = e->final_stats.physical_defense;
    } else if (strcmp(stat, "mag_defense") == 0) {
        value = e->final_stats.magical_defense;
    } else if (strcmp(stat, "hit_rate") == 0) {
        value = e->final_stats.hit_rate;
    } else if (strcmp(stat, "evasion") == 0) {
        value = e->final_stats.evasion;
    } else if (strcmp(stat, "crit_rate") == 0) {
        value = e->final_stats.critical_rate;
    } else if (strcmp(stat, "crit_damage") == 0) {
        value = e->final_stats.critical_damage;
    } else if (strcmp(stat, "attack_speed") == 0) {
        value = e->final_stats.attack_speed;
    } else if (strcmp(stat, "move_speed") == 0) {
        value = e->final_stats.move_speed;
    } else {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, value);
    return 1;
}

/*
 * entity.has_buff(entity_id, buff_id)
 * Returns: boolean
 */
static int l_entity_has_buff(lua_State* L)
{
    u32 id;
    u16 buff_id;
    Entity* e;

    LUA_CHECK_ARGS(L, 2);
    id = (u32)LUA_GET_INT(L, 1);
    buff_id = (u16)LUA_GET_INT(L, 2);
    e = get_entity(id);

    if (!e) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, entity_has_buff(e, buff_id));
    return 1;
}

/*
 * entity.add_buff(entity_id, buff_id, skill_id, source_id, duration_ms, value)
 * Returns: success
 */
static int l_entity_add_buff(lua_State* L)
{
    u32 id, source_id;
    u16 buff_id, skill_id;
    u32 duration;
    i32 value;
    Entity* e;
    int ret;

    LUA_CHECK_ARGS(L, 6);
    id = (u32)LUA_GET_INT(L, 1);
    buff_id = (u16)LUA_GET_INT(L, 2);
    skill_id = (u16)LUA_GET_INT(L, 3);
    source_id = (u32)LUA_GET_INT(L, 4);
    duration = (u32)LUA_GET_INT(L, 5);
    value = LUA_GET_INT(L, 6);
    e = get_entity(id);

    if (!e) {
        lua_pushboolean(L, 0);
        return 1;
    }

    ret = entity_add_buff(e, buff_id, skill_id, source_id, duration, value);
    lua_pushboolean(L, ret == ENGINE_OK);
    return 1;
}

/*
 * entity.remove_buff(entity_id, buff_id)
 * Returns: success
 */
static int l_entity_remove_buff(lua_State* L)
{
    u32 id;
    u16 buff_id;
    Entity* e;
    int ret;

    LUA_CHECK_ARGS(L, 2);
    id = (u32)LUA_GET_INT(L, 1);
    buff_id = (u16)LUA_GET_INT(L, 2);
    e = get_entity(id);

    if (!e) {
        lua_pushboolean(L, 0);
        return 1;
    }

    ret = entity_remove_buff(e, buff_id);
    lua_pushboolean(L, ret == ENGINE_OK);
    return 1;
}

/* Registration function */
int lua_register_entity(LuaEngine* engine)
{
    lua_State* L;

    if (!engine || !engine->L) {
        return ENGINE_ERROR_PARAM;
    }

    L = engine->L;

    /* Create entity table */
    lua_newtable(L);

    lua_pushcfunction(L, l_entity_get_hp);
    lua_setfield(L, -2, "get_hp");

    lua_pushcfunction(L, l_entity_get_mp);
    lua_setfield(L, -2, "get_mp");

    lua_pushcfunction(L, l_entity_get_position);
    lua_setfield(L, -2, "get_position");

    lua_pushcfunction(L, l_entity_get_level);
    lua_setfield(L, -2, "get_level");

    lua_pushcfunction(L, l_entity_get_state);
    lua_setfield(L, -2, "get_state");

    lua_pushcfunction(L, l_entity_is_alive);
    lua_setfield(L, -2, "is_alive");

    lua_pushcfunction(L, l_entity_get_type);
    lua_setfield(L, -2, "get_type");

    lua_pushcfunction(L, l_entity_get_stat);
    lua_setfield(L, -2, "get_stat");

    lua_pushcfunction(L, l_entity_has_buff);
    lua_setfield(L, -2, "has_buff");

    lua_pushcfunction(L, l_entity_add_buff);
    lua_setfield(L, -2, "add_buff");

    lua_pushcfunction(L, l_entity_remove_buff);
    lua_setfield(L, -2, "remove_buff");

    /* Entity type constants */
    lua_pushinteger(L, ENTITY_TYPE_PLAYER);
    lua_setfield(L, -2, "TYPE_PLAYER");

    lua_pushinteger(L, ENTITY_TYPE_MONSTER);
    lua_setfield(L, -2, "TYPE_MONSTER");

    lua_pushinteger(L, ENTITY_TYPE_NPC);
    lua_setfield(L, -2, "TYPE_NPC");

    lua_pushinteger(L, ENTITY_TYPE_PET);
    lua_setfield(L, -2, "TYPE_PET");

    /* Entity state constants */
    lua_pushinteger(L, ENTITY_STATE_IDLE);
    lua_setfield(L, -2, "STATE_IDLE");

    lua_pushinteger(L, ENTITY_STATE_MOVING);
    lua_setfield(L, -2, "STATE_MOVING");

    lua_pushinteger(L, ENTITY_STATE_ATTACKING);
    lua_setfield(L, -2, "STATE_ATTACKING");

    lua_pushinteger(L, ENTITY_STATE_CASTING);
    lua_setfield(L, -2, "STATE_CASTING");

    lua_pushinteger(L, ENTITY_STATE_STUNNED);
    lua_setfield(L, -2, "STATE_STUNNED");

    lua_pushinteger(L, ENTITY_STATE_DEAD);
    lua_setfield(L, -2, "STATE_DEAD");

    lua_setglobal(L, "entity");

    return ENGINE_OK;
}

#else /* !USE_LUA */

int lua_register_entity(LuaEngine* engine)
{
    (void)engine;
    return ENGINE_OK;
}

#endif /* USE_LUA */
