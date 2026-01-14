/**
 * @file message_handler.c
 * @brief Message routing and handler dispatch system
 */

#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include "../../include/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * Handler Types
 * ============================================================================
 */

/* Handler function signature */
typedef void (*MessageHandler)(u16 msg_no, const u8* data, int len, void* user_data);

/* Handler registration entry */
typedef struct {
    u16             msg_no;
    MessageHandler  handler;
    const char*     name;
} HandlerEntry;

/* Handler manager */
typedef struct {
    HandlerEntry*   entries;
    int             count;
    int             capacity;
    void*           user_data;
    /* Statistics */
    u32             handled_count;
    u32             unknown_count;
} HandlerManager;

/* Global handler manager */
static HandlerManager g_handler_mgr = {0};

/* External parser functions (from packet_parser.c) */
extern int packet_parse_login_response(const u8* data, int len, LoginResponse* resp);
extern int packet_parse_enter_world(const u8* data, int len, EnterWorldResponse* resp);
extern int packet_parse_spawn(const u8* data, int len, SpawnData* spawn);
extern int packet_parse_position(const u8* data, int len, PositionSync* pos);
extern int packet_parse_damage(const u8* data, int len, DamageEvent* dmg);
extern int packet_parse_death(const u8* data, int len, DeathEvent* death);
extern int packet_parse_inventory_update(const u8* data, int len, InventoryUpdate* update);
extern int packet_parse_chat(const u8* data, int len, ChatMessage* msg);
extern int packet_parse_dungeon_info(const u8* data, int len, DungeonInfo* info);
extern int packet_parse_dungeon_result(const u8* data, int len, DungeonResult* result);
extern void packet_dump_hex(const u8* data, int len);

/* External game state (to be implemented in game module) */
extern void game_on_login_response(LoginResponse* resp);
extern void game_on_enter_world(EnterWorldResponse* resp);
extern void game_on_player_spawn(SpawnData* spawn);
extern void game_on_monster_spawn(SpawnData* spawn);
extern void game_on_npc_spawn(SpawnData* spawn);
extern void game_on_entity_move(PositionSync* pos);
extern void game_on_damage(DamageEvent* dmg);
extern void game_on_entity_death(DeathEvent* death);
extern void game_on_entity_despawn(u32 entity_id);
extern void game_on_inventory_update(InventoryUpdate* update);
extern void game_on_gold_update(u32 gold);
extern void game_on_hp_update(u32 entity_id, i32 hp, i32 max_hp);
extern void game_on_mp_update(u32 entity_id, i32 mp, i32 max_mp);
extern void game_on_exp_update(u32 exp);
extern void game_on_level_up(u16 level);
extern void game_on_chat(ChatMessage* msg);
extern void game_on_dungeon_info(DungeonInfo* info);
extern void game_on_dungeon_result(DungeonResult* result);
extern void game_on_error(u8 error_code, const char* message);
extern void game_on_system_message(const char* message);

/*
 * ============================================================================
 * Handler Manager Functions
 * ============================================================================
 */

int message_handler_init(void)
{
    g_handler_mgr.capacity = 256;
    g_handler_mgr.entries = calloc(g_handler_mgr.capacity, sizeof(HandlerEntry));
    if (!g_handler_mgr.entries) {
        return CLIENT_ERROR_MEMORY;
    }
    g_handler_mgr.count = 0;
    g_handler_mgr.handled_count = 0;
    g_handler_mgr.unknown_count = 0;
    return CLIENT_OK;
}

void message_handler_cleanup(void)
{
    if (g_handler_mgr.entries) {
        free(g_handler_mgr.entries);
        g_handler_mgr.entries = NULL;
    }
    g_handler_mgr.count = 0;
    g_handler_mgr.capacity = 0;
}

int message_handler_register(u16 msg_no, MessageHandler handler, const char* name)
{
    int i;

    if (!g_handler_mgr.entries) return CLIENT_ERROR;

    /* Check if already registered */
    for (i = 0; i < g_handler_mgr.count; i++) {
        if (g_handler_mgr.entries[i].msg_no == msg_no) {
            /* Update existing */
            g_handler_mgr.entries[i].handler = handler;
            g_handler_mgr.entries[i].name = name;
            return CLIENT_OK;
        }
    }

    /* Add new entry */
    if (g_handler_mgr.count >= g_handler_mgr.capacity) {
        /* Expand capacity */
        int new_capacity = g_handler_mgr.capacity * 2;
        HandlerEntry* new_entries = realloc(g_handler_mgr.entries,
                                            new_capacity * sizeof(HandlerEntry));
        if (!new_entries) return CLIENT_ERROR_MEMORY;
        g_handler_mgr.entries = new_entries;
        g_handler_mgr.capacity = new_capacity;
    }

    g_handler_mgr.entries[g_handler_mgr.count].msg_no = msg_no;
    g_handler_mgr.entries[g_handler_mgr.count].handler = handler;
    g_handler_mgr.entries[g_handler_mgr.count].name = name;
    g_handler_mgr.count++;

    return CLIENT_OK;
}

void message_handler_set_user_data(void* user_data)
{
    g_handler_mgr.user_data = user_data;
}

/*
 * ============================================================================
 * Built-in Handlers
 * ============================================================================
 */

static void handle_login_response(u16 msg_no, const u8* data, int len, void* user_data)
{
    LoginResponse resp;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_login_response(data, len, &resp) == 0) {
        printf("[Handler] Login response: result=%d, chars=%d\n",
               resp.result, resp.character_count);
        game_on_login_response(&resp);
    } else {
        printf("[Handler] Failed to parse login response\n");
    }
}

static void handle_enter_world(u16 msg_no, const u8* data, int len, void* user_data)
{
    EnterWorldResponse resp;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_enter_world(data, len, &resp) == 0) {
        printf("[Handler] Enter world: result=%d, name=%s, level=%d\n",
               resp.result, resp.character.info.name, resp.character.info.level);
        game_on_enter_world(&resp);
    } else {
        printf("[Handler] Failed to parse enter world response\n");
    }
}

static void handle_player_spawn(u16 msg_no, const u8* data, int len, void* user_data)
{
    SpawnData spawn;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_spawn(data, len, &spawn) == 0) {
        printf("[Handler] Player spawn: id=%u, name=%s, pos=(%.1f, %.1f)\n",
               spawn.entity_id, spawn.name, spawn.x, spawn.y);
        game_on_player_spawn(&spawn);
    }
}

static void handle_monster_spawn(u16 msg_no, const u8* data, int len, void* user_data)
{
    SpawnData spawn;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_spawn(data, len, &spawn) == 0) {
        printf("[Handler] Monster spawn: id=%u, template=%u, pos=(%.1f, %.1f)\n",
               spawn.entity_id, spawn.template_id, spawn.x, spawn.y);
        game_on_monster_spawn(&spawn);
    }
}

static void handle_npc_spawn(u16 msg_no, const u8* data, int len, void* user_data)
{
    SpawnData spawn;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_spawn(data, len, &spawn) == 0) {
        printf("[Handler] NPC spawn: id=%u, template=%u\n",
               spawn.entity_id, spawn.template_id);
        game_on_npc_spawn(&spawn);
    }
}

static void handle_entity_move(u16 msg_no, const u8* data, int len, void* user_data)
{
    PositionSync pos;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_position(data, len, &pos) == 0) {
        game_on_entity_move(&pos);
    }
}

static void handle_damage(u16 msg_no, const u8* data, int len, void* user_data)
{
    DamageEvent dmg;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_damage(data, len, &dmg) == 0) {
        printf("[Handler] Damage: %u -> %u, dmg=%d, type=%d\n",
               dmg.attacker_id, dmg.target_id, dmg.damage, dmg.damage_type);
        game_on_damage(&dmg);
    }
}

static void handle_entity_death(u16 msg_no, const u8* data, int len, void* user_data)
{
    DeathEvent death;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_death(data, len, &death) == 0) {
        printf("[Handler] Death: entity=%u killed by %u\n",
               death.entity_id, death.killer_id);
        game_on_entity_death(&death);
    }
}

static void handle_entity_despawn(u16 msg_no, const u8* data, int len, void* user_data)
{
    u32 entity_id;
    (void)msg_no;
    (void)user_data;

    if (len >= 4) {
        entity_id = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        game_on_entity_despawn(entity_id);
    }
}

static void handle_inventory_update(u16 msg_no, const u8* data, int len, void* user_data)
{
    InventoryUpdate update;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_inventory_update(data, len, &update) == 0) {
        printf("[Handler] Inventory update: op=%d, slot=%d\n",
               update.operation, update.slot);
        game_on_inventory_update(&update);
    }
}

static void handle_gold_update(u16 msg_no, const u8* data, int len, void* user_data)
{
    u32 gold;
    (void)msg_no;
    (void)user_data;

    if (len >= 4) {
        gold = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        game_on_gold_update(gold);
    }
}

static void handle_hp_update(u16 msg_no, const u8* data, int len, void* user_data)
{
    (void)msg_no;
    (void)user_data;

    if (len >= 12) {
        u32 entity_id = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        i32 hp = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
        i32 max_hp = data[8] | (data[9] << 8) | (data[10] << 16) | (data[11] << 24);
        game_on_hp_update(entity_id, hp, max_hp);
    }
}

static void handle_chat(u16 msg_no, const u8* data, int len, void* user_data)
{
    ChatMessage msg;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_chat(data, len, &msg) == 0) {
        printf("[Handler] Chat [%d] %s: %s\n",
               msg.channel, msg.sender_name, msg.message);
        game_on_chat(&msg);
    }
}

static void handle_dungeon_info(u16 msg_no, const u8* data, int len, void* user_data)
{
    DungeonInfo info;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_dungeon_info(data, len, &info) == 0) {
        printf("[Handler] Dungeon info: id=%u, room=%d/%d\n",
               info.dungeon_id, info.current_room, info.room_count);
        game_on_dungeon_info(&info);
    }
}

static void handle_dungeon_result(u16 msg_no, const u8* data, int len, void* user_data)
{
    DungeonResult result;
    (void)msg_no;
    (void)user_data;

    if (packet_parse_dungeon_result(data, len, &result) == 0) {
        printf("[Handler] Dungeon clear: rank=%c, exp=%u, gold=%u\n",
               "SABCD"[result.rank], result.exp_gained, result.gold_gained);
        game_on_dungeon_result(&result);
    }
}

static void handle_error(u16 msg_no, const u8* data, int len, void* user_data)
{
    (void)msg_no;
    (void)user_data;

    if (len >= 1) {
        u8 error_code = data[0];
        char message[256] = {0};
        if (len > 1) {
            int msg_len = len - 1;
            if (msg_len > 255) msg_len = 255;
            memcpy(message, &data[1], msg_len);
        }
        printf("[Handler] Error: code=%d, msg=%s\n", error_code, message);
        game_on_error(error_code, message);
    }
}

static void handle_system_message(u16 msg_no, const u8* data, int len, void* user_data)
{
    char message[512] = {0};
    (void)msg_no;
    (void)user_data;

    if (len > 0) {
        int msg_len = len;
        if (msg_len > 511) msg_len = 511;
        memcpy(message, data, msg_len);
        printf("[Handler] System: %s\n", message);
        game_on_system_message(message);
    }
}

/*
 * ============================================================================
 * Handler Registration
 * ============================================================================
 */

void message_handler_register_defaults(void)
{
    /* Core responses */
    message_handler_register(MSG_RESP_LOGIN, handle_login_response, "LoginResponse");
    message_handler_register(MSG_RESP_ENTERWORLD, handle_enter_world, "EnterWorld");

    /* Entity sync */
    message_handler_register(MSG_RESP_PLAYER_SPAWN, handle_player_spawn, "PlayerSpawn");
    message_handler_register(MSG_RESP_MONSTER_SPAWN, handle_monster_spawn, "MonsterSpawn");
    message_handler_register(MSG_RESP_NPC_SPAWN, handle_npc_spawn, "NPCSpawn");
    message_handler_register(MSG_RESP_ENTITY_MOVE, handle_entity_move, "EntityMove");
    message_handler_register(MSG_RESP_DAMAGE, handle_damage, "Damage");
    message_handler_register(MSG_RESP_ENTITY_DEATH, handle_entity_death, "EntityDeath");
    message_handler_register(MSG_RESP_ENTITY_DESPAWN, handle_entity_despawn, "EntityDespawn");

    /* Stats */
    message_handler_register(MSG_RESP_HP_UPDATE, handle_hp_update, "HPUpdate");
    message_handler_register(MSG_RESP_INVENTORY_UPDATE, handle_inventory_update, "InventoryUpdate");
    message_handler_register(MSG_RESP_GOLD_UPDATE, handle_gold_update, "GoldUpdate");

    /* Chat */
    message_handler_register(MSG_RESP_CHAT, handle_chat, "Chat");

    /* Dungeon */
    message_handler_register(MSG_RESP_DUNGEON_INFO, handle_dungeon_info, "DungeonInfo");
    message_handler_register(MSG_RESP_DUNGEON_CLEAR, handle_dungeon_result, "DungeonClear");

    /* System */
    message_handler_register(MSG_RESP_ERROR, handle_error, "Error");
    message_handler_register(MSG_RESP_SYSTEM_MESSAGE, handle_system_message, "SystemMessage");

    printf("[Handler] Registered %d default handlers\n", g_handler_mgr.count);
}

/*
 * ============================================================================
 * Message Dispatch
 * ============================================================================
 */

void message_handler_dispatch(u16 msg_no, const u8* data, int len)
{
    int i;

    /* Find handler */
    for (i = 0; i < g_handler_mgr.count; i++) {
        if (g_handler_mgr.entries[i].msg_no == msg_no) {
            if (g_handler_mgr.entries[i].handler) {
                g_handler_mgr.entries[i].handler(msg_no, data, len, g_handler_mgr.user_data);
                g_handler_mgr.handled_count++;
                return;
            }
        }
    }

    /* No handler found */
    g_handler_mgr.unknown_count++;
    printf("[Handler] Unknown message: %u (len=%d)\n", msg_no, len);

    #ifdef DEBUG
    if (len > 0 && len <= 64) {
        packet_dump_hex(data, len);
    }
    #endif
}

/*
 * ============================================================================
 * Statistics
 * ============================================================================
 */

void message_handler_get_stats(u32* handled, u32* unknown)
{
    if (handled) *handled = g_handler_mgr.handled_count;
    if (unknown) *unknown = g_handler_mgr.unknown_count;
}

void message_handler_reset_stats(void)
{
    g_handler_mgr.handled_count = 0;
    g_handler_mgr.unknown_count = 0;
}

/*
 * ============================================================================
 * Stub implementations for game callbacks (to be replaced)
 * ============================================================================
 */

__attribute__((weak))
void game_on_login_response(LoginResponse* resp) { (void)resp; }

__attribute__((weak))
void game_on_enter_world(EnterWorldResponse* resp) { (void)resp; }

__attribute__((weak))
void game_on_player_spawn(SpawnData* spawn) { (void)spawn; }

__attribute__((weak))
void game_on_monster_spawn(SpawnData* spawn) { (void)spawn; }

__attribute__((weak))
void game_on_npc_spawn(SpawnData* spawn) { (void)spawn; }

__attribute__((weak))
void game_on_entity_move(PositionSync* pos) { (void)pos; }

__attribute__((weak))
void game_on_damage(DamageEvent* dmg) { (void)dmg; }

__attribute__((weak))
void game_on_entity_death(DeathEvent* death) { (void)death; }

__attribute__((weak))
void game_on_entity_despawn(u32 entity_id) { (void)entity_id; }

__attribute__((weak))
void game_on_inventory_update(InventoryUpdate* update) { (void)update; }

__attribute__((weak))
void game_on_gold_update(u32 gold) { (void)gold; }

__attribute__((weak))
void game_on_hp_update(u32 entity_id, i32 hp, i32 max_hp)
{ (void)entity_id; (void)hp; (void)max_hp; }

__attribute__((weak))
void game_on_mp_update(u32 entity_id, i32 mp, i32 max_mp)
{ (void)entity_id; (void)mp; (void)max_mp; }

__attribute__((weak))
void game_on_exp_update(u32 exp) { (void)exp; }

__attribute__((weak))
void game_on_level_up(u16 level) { (void)level; }

__attribute__((weak))
void game_on_chat(ChatMessage* msg) { (void)msg; }

__attribute__((weak))
void game_on_dungeon_info(DungeonInfo* info) { (void)info; }

__attribute__((weak))
void game_on_dungeon_result(DungeonResult* result) { (void)result; }

__attribute__((weak))
void game_on_error(u8 error_code, const char* message)
{ (void)error_code; (void)message; }

__attribute__((weak))
void game_on_system_message(const char* message) { (void)message; }
