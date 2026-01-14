/**
 * @file lua_combat.c
 * @brief Combat system Lua bindings
 */

#include "lua_bindings.h"
#include "../../game/combat/combat_engine.h"
#include "../../game/entity/entity.h"

#ifdef USE_LUA

/* Global entity registry for Lua access */
static Entity* g_entity_registry[1024];
static int g_entity_count = 0;

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
 * combat.calculate_damage(source_id, target_id, base_damage, damage_type)
 * Returns: damage, is_crit, is_miss
 */
static int l_combat_calculate_damage(lua_State* L)
{
    u32 source_id, target_id;
    i32 base_damage;
    int damage_type;
    Entity *source, *target;
    SkillDefinition skill;
    CombatResult result;
    int ret;

    LUA_CHECK_ARGS(L, 4);

    source_id = (u32)LUA_GET_INT(L, 1);
    target_id = (u32)LUA_GET_INT(L, 2);
    base_damage = LUA_GET_INT(L, 3);
    damage_type = LUA_GET_INT(L, 4);

    source = get_entity(source_id);
    target = get_entity(target_id);

    if (!source || !target) {
        lua_pushnil(L);
        lua_pushstring(L, "entity not found");
        return 2;
    }

    /* Create minimal skill for damage calculation */
    memset(&skill, 0, sizeof(skill));
    skill.base_damage = base_damage;
    skill.damage_type = (DamageType)damage_type;
    skill.can_crit = 1;
    skill.hit_count = 1;

    ret = combat_calculate_damage(source, target, &skill, &result);
    if (ret != ENGINE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "calculation failed");
        return 2;
    }

    lua_pushinteger(L, result.damage);
    lua_pushboolean(L, result.is_crit);
    lua_pushboolean(L, result.is_miss);
    return 3;
}

/*
 * combat.apply_damage(target_id, damage, source_id)
 * Returns: actual_damage
 */
static int l_combat_apply_damage(lua_State* L)
{
    u32 target_id, source_id;
    i32 damage;
    Entity* target;
    i32 actual;

    LUA_CHECK_ARGS(L, 3);

    target_id = (u32)LUA_GET_INT(L, 1);
    damage = LUA_GET_INT(L, 2);
    source_id = (u32)LUA_GET_INT(L, 3);

    target = get_entity(target_id);
    if (!target) {
        lua_pushnil(L);
        return 1;
    }

    actual = entity_take_damage(target, damage, DAMAGE_TYPE_PURE, source_id);
    lua_pushinteger(L, actual);
    return 1;
}

/*
 * combat.heal(target_id, amount, source_id)
 * Returns: actual_heal
 */
static int l_combat_heal(lua_State* L)
{
    u32 target_id, source_id;
    i32 amount;
    Entity* target;
    i32 actual;

    LUA_CHECK_ARGS(L, 3);

    target_id = (u32)LUA_GET_INT(L, 1);
    amount = LUA_GET_INT(L, 2);
    source_id = (u32)LUA_GET_INT(L, 3);

    target = get_entity(target_id);
    if (!target) {
        lua_pushnil(L);
        return 1;
    }

    actual = entity_heal(target, amount, source_id);
    lua_pushinteger(L, actual);
    return 1;
}

/*
 * combat.check_hit(source_id, target_id)
 * Returns: boolean
 */
static int l_combat_check_hit(lua_State* L)
{
    u32 source_id, target_id;
    Entity *source, *target;

    LUA_CHECK_ARGS(L, 2);

    source_id = (u32)LUA_GET_INT(L, 1);
    target_id = (u32)LUA_GET_INT(L, 2);

    source = get_entity(source_id);
    target = get_entity(target_id);

    if (!source || !target) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, combat_check_hit(source, target));
    return 1;
}

/*
 * combat.check_critical(source_id)
 * Returns: boolean
 */
static int l_combat_check_critical(lua_State* L)
{
    u32 source_id;
    Entity* source;

    LUA_CHECK_ARGS(L, 1);

    source_id = (u32)LUA_GET_INT(L, 1);
    source = get_entity(source_id);

    if (!source) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, combat_check_critical(source));
    return 1;
}

/*
 * combat.in_range(source_id, target_id, range)
 * Returns: boolean
 */
static int l_combat_in_range(lua_State* L)
{
    u32 source_id, target_id;
    i32 range;
    Entity *source, *target;

    LUA_CHECK_ARGS(L, 3);

    source_id = (u32)LUA_GET_INT(L, 1);
    target_id = (u32)LUA_GET_INT(L, 2);
    range = LUA_GET_INT(L, 3);

    source = get_entity(source_id);
    target = get_entity(target_id);

    if (!source || !target) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, entity_in_range(source, target, range));
    return 1;
}

/*
 * combat.get_distance(source_id, target_id)
 * Returns: distance
 */
static int l_combat_get_distance(lua_State* L)
{
    u32 source_id, target_id;
    Entity *source, *target;

    LUA_CHECK_ARGS(L, 2);

    source_id = (u32)LUA_GET_INT(L, 1);
    target_id = (u32)LUA_GET_INT(L, 2);

    source = get_entity(source_id);
    target = get_entity(target_id);

    if (!source || !target) {
        lua_pushinteger(L, -1);
        return 1;
    }

    lua_pushinteger(L, entity_distance_to(source, target));
    return 1;
}

/* Registration function */
int lua_register_combat(LuaEngine* engine)
{
    lua_State* L;

    if (!engine || !engine->L) {
        return ENGINE_ERROR_PARAM;
    }

    L = engine->L;

    /* Create combat table */
    lua_newtable(L);

    lua_pushcfunction(L, l_combat_calculate_damage);
    lua_setfield(L, -2, "calculate_damage");

    lua_pushcfunction(L, l_combat_apply_damage);
    lua_setfield(L, -2, "apply_damage");

    lua_pushcfunction(L, l_combat_heal);
    lua_setfield(L, -2, "heal");

    lua_pushcfunction(L, l_combat_check_hit);
    lua_setfield(L, -2, "check_hit");

    lua_pushcfunction(L, l_combat_check_critical);
    lua_setfield(L, -2, "check_critical");

    lua_pushcfunction(L, l_combat_in_range);
    lua_setfield(L, -2, "in_range");

    lua_pushcfunction(L, l_combat_get_distance);
    lua_setfield(L, -2, "get_distance");

    /* Damage type constants */
    lua_pushinteger(L, DAMAGE_TYPE_PHYSICAL);
    lua_setfield(L, -2, "DAMAGE_PHYSICAL");

    lua_pushinteger(L, DAMAGE_TYPE_MAGICAL);
    lua_setfield(L, -2, "DAMAGE_MAGICAL");

    lua_pushinteger(L, DAMAGE_TYPE_FIRE);
    lua_setfield(L, -2, "DAMAGE_FIRE");

    lua_pushinteger(L, DAMAGE_TYPE_ICE);
    lua_setfield(L, -2, "DAMAGE_ICE");

    lua_pushinteger(L, DAMAGE_TYPE_LIGHT);
    lua_setfield(L, -2, "DAMAGE_LIGHT");

    lua_pushinteger(L, DAMAGE_TYPE_DARK);
    lua_setfield(L, -2, "DAMAGE_DARK");

    lua_pushinteger(L, DAMAGE_TYPE_PURE);
    lua_setfield(L, -2, "DAMAGE_PURE");

    lua_setglobal(L, "combat");

    return ENGINE_OK;
}

#else /* !USE_LUA */

int lua_register_combat(LuaEngine* engine)
{
    (void)engine;
    return ENGINE_OK;
}

#endif /* USE_LUA */
