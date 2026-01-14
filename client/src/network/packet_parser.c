/**
 * @file packet_parser.c
 * @brief Packet parsing utilities for server responses
 */

#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * Packet Reader Context
 * ============================================================================
 */

typedef struct {
    const u8* data;
    int       len;
    int       pos;
    int       error;
} PacketReader;

static void reader_init(PacketReader* reader, const u8* data, int len)
{
    reader->data = data;
    reader->len = len;
    reader->pos = 0;
    reader->error = 0;
}

static int reader_remaining(PacketReader* reader)
{
    return reader->len - reader->pos;
}

static u8 reader_u8(PacketReader* reader)
{
    if (reader->pos + 1 > reader->len) {
        reader->error = 1;
        return 0;
    }
    return reader->data[reader->pos++];
}

static u16 reader_u16(PacketReader* reader)
{
    if (reader->pos + 2 > reader->len) {
        reader->error = 1;
        return 0;
    }
    u16 val = reader->data[reader->pos] | (reader->data[reader->pos + 1] << 8);
    reader->pos += 2;
    return val;
}

static u32 reader_u32(PacketReader* reader)
{
    if (reader->pos + 4 > reader->len) {
        reader->error = 1;
        return 0;
    }
    u32 val = reader->data[reader->pos] |
              (reader->data[reader->pos + 1] << 8) |
              (reader->data[reader->pos + 2] << 16) |
              (reader->data[reader->pos + 3] << 24);
    reader->pos += 4;
    return val;
}

static i32 reader_i32(PacketReader* reader)
{
    return (i32)reader_u32(reader);
}

static float reader_float(PacketReader* reader)
{
    union { u32 u; float f; } conv;
    conv.u = reader_u32(reader);
    return conv.f;
}

static void reader_string(PacketReader* reader, char* out, int max_len)
{
    int i;
    for (i = 0; i < max_len - 1 && reader->pos < reader->len; i++) {
        char c = (char)reader->data[reader->pos++];
        out[i] = c;
        if (c == '\0') break;
    }
    out[i] = '\0';
}

static void reader_fixed_string(PacketReader* reader, char* out, int len)
{
    if (reader->pos + len > reader->len) {
        reader->error = 1;
        out[0] = '\0';
        return;
    }
    memcpy(out, &reader->data[reader->pos], len);
    out[len - 1] = '\0';
    reader->pos += len;
}

static void reader_bytes(PacketReader* reader, u8* out, int len)
{
    if (reader->pos + len > reader->len) {
        reader->error = 1;
        return;
    }
    memcpy(out, &reader->data[reader->pos], len);
    reader->pos += len;
}

static void reader_skip(PacketReader* reader, int len)
{
    reader->pos += len;
    if (reader->pos > reader->len) {
        reader->pos = reader->len;
        reader->error = 1;
    }
}

/*
 * ============================================================================
 * Item Parsing
 * ============================================================================
 */

static void parse_item_data(PacketReader* reader, ItemData* item)
{
    int i;

    item->item_id = reader_u32(reader);
    item->unique_id = reader_u32(reader);
    item->slot = reader_u16(reader);
    item->count = reader_u16(reader);
    item->type = reader_u8(reader);
    item->rarity = reader_u8(reader);
    item->bound = reader_u8(reader);
    item->enhance_level = reader_u8(reader);
    item->durability = reader_u16(reader);
    item->max_durability = reader_u16(reader);
    item->expire_time = reader_u32(reader);
    item->attr_count = reader_u8(reader);

    if (item->attr_count > 8) item->attr_count = 8;

    for (i = 0; i < item->attr_count; i++) {
        item->attrs[i].type = reader_u8(reader);
        item->attrs[i].value = (i16)reader_u16(reader);
    }
}

static void parse_equipment_data(PacketReader* reader, EquipmentData* equip)
{
    parse_item_data(reader, &equip->base);

    equip->equip_slot = reader_u8(reader);
    equip->req_level = reader_u8(reader);
    equip->req_class = reader_u8(reader);
    equip->phys_atk = (i16)reader_u16(reader);
    equip->mag_atk = (i16)reader_u16(reader);
    equip->phys_def = (i16)reader_u16(reader);
    equip->mag_def = (i16)reader_u16(reader);
    equip->str_bonus = (i16)reader_u16(reader);
    equip->int_bonus = (i16)reader_u16(reader);
    equip->vit_bonus = (i16)reader_u16(reader);
    equip->spr_bonus = (i16)reader_u16(reader);
}

/*
 * ============================================================================
 * Character Parsing
 * ============================================================================
 */

static void parse_character_info(PacketReader* reader, CharacterInfo* info)
{
    info->charac_no = reader_u32(reader);
    reader_fixed_string(reader, info->name, MAX_NAME_LENGTH);
    info->job = reader_u8(reader);
    info->grow_type = reader_u8(reader);
    info->level = reader_u16(reader);
    info->exp = reader_u32(reader);
    info->gold = reader_u32(reader);
    info->fatigue = reader_u16(reader);
    info->max_fatigue = reader_u16(reader);
    info->last_play = reader_u32(reader);
    info->delete_flag = reader_u8(reader);
    info->delete_time = reader_u32(reader);
    info->hair_style = reader_u8(reader);
    info->hair_color = reader_u8(reader);
    info->face_style = reader_u8(reader);
    info->skin_color = reader_u8(reader);
    info->weapon_id = reader_u32(reader);
    info->avatar_weapon_id = reader_u32(reader);
    info->avatar_top_id = reader_u32(reader);
    info->avatar_bottom_id = reader_u32(reader);
}

static void parse_character_data(PacketReader* reader, CharacterData* data)
{
    parse_character_info(reader, &data->info);

    data->hp = reader_i32(reader);
    data->max_hp = reader_i32(reader);
    data->mp = reader_i32(reader);
    data->max_mp = reader_i32(reader);
    data->str = (i16)reader_u16(reader);
    data->intel = (i16)reader_u16(reader);
    data->vit = (i16)reader_u16(reader);
    data->spr = (i16)reader_u16(reader);
    data->phys_atk = (i16)reader_u16(reader);
    data->mag_atk = (i16)reader_u16(reader);
    data->phys_def = (i16)reader_u16(reader);
    data->mag_def = (i16)reader_u16(reader);
    data->crit_rate = (i16)reader_u16(reader);
    data->crit_damage = (i16)reader_u16(reader);
    data->move_speed = (i16)reader_u16(reader);
    data->attack_speed = (i16)reader_u16(reader);
    data->cast_speed = (i16)reader_u16(reader);
    data->x = reader_float(reader);
    data->y = reader_float(reader);
    data->map_id = reader_u32(reader);
    data->channel = reader_u8(reader);
    data->pk_mode = reader_u8(reader);
    data->guild_id = reader_u32(reader);
    data->party_id = reader_u32(reader);
    data->sp = reader_u32(reader);
    data->tp = reader_u32(reader);
    data->qp = reader_u16(reader);
}

/*
 * ============================================================================
 * Entity Parsing
 * ============================================================================
 */

static void parse_spawn_data(PacketReader* reader, SpawnData* spawn)
{
    int i;

    spawn->entity_id = reader_u32(reader);
    spawn->entity_type = reader_u8(reader);
    spawn->template_id = reader_u32(reader);
    reader_fixed_string(reader, spawn->name, MAX_NAME_LENGTH);
    spawn->x = reader_float(reader);
    spawn->y = reader_float(reader);
    spawn->dir = reader_float(reader);
    spawn->hp = reader_i32(reader);
    spawn->max_hp = reader_i32(reader);
    spawn->level = reader_u8(reader);
    spawn->state = reader_u8(reader);
    spawn->job = reader_u8(reader);
    spawn->grow_type = reader_u8(reader);
    spawn->guild_id = reader_u32(reader);
    spawn->weapon_id = reader_u32(reader);

    for (i = 0; i < 4; i++) {
        spawn->avatar_ids[i] = reader_u32(reader);
    }
}

static void parse_position_sync(PacketReader* reader, PositionSync* pos)
{
    pos->entity_id = reader_u32(reader);
    pos->x = reader_float(reader);
    pos->y = reader_float(reader);
    pos->vx = reader_float(reader);
    pos->vy = reader_float(reader);
    pos->dir = reader_float(reader);
    pos->state = reader_u8(reader);
    pos->timestamp = reader_u32(reader);
}

static void parse_damage_event(PacketReader* reader, DamageEvent* dmg)
{
    dmg->attacker_id = reader_u32(reader);
    dmg->target_id = reader_u32(reader);
    dmg->skill_id = reader_u32(reader);
    dmg->damage = reader_i32(reader);
    dmg->damage_type = reader_u8(reader);
    dmg->target_hp = reader_i32(reader);
    dmg->knockback_x = reader_float(reader);
    dmg->knockback_y = reader_float(reader);
}

static void parse_death_event(PacketReader* reader, DeathEvent* death)
{
    death->entity_id = reader_u32(reader);
    death->killer_id = reader_u32(reader);
    death->x = reader_float(reader);
    death->y = reader_float(reader);
}

/*
 * ============================================================================
 * Skill Parsing
 * ============================================================================
 */

static void parse_skill_data(PacketReader* reader, SkillData* skill)
{
    skill->skill_id = reader_u32(reader);
    skill->level = reader_u8(reader);
    skill->slot = reader_u8(reader);
    skill->cooldown_end = reader_u32(reader);
}

static void parse_skill_list(PacketReader* reader, SkillList* list)
{
    u32 i;

    list->skill_count = reader_u32(reader);
    if (list->skill_count > MAX_SKILL_SLOTS) {
        list->skill_count = MAX_SKILL_SLOTS;
    }

    for (i = 0; i < list->skill_count; i++) {
        parse_skill_data(reader, &list->skills[i]);
    }
}

/*
 * ============================================================================
 * Buff Parsing
 * ============================================================================
 */

static void parse_buff_data(PacketReader* reader, BuffData* buff)
{
    buff->buff_id = reader_u32(reader);
    buff->level = reader_u8(reader);
    buff->source_id = reader_u32(reader);
    buff->end_time = reader_u32(reader);
    buff->stack_count = reader_u8(reader);
}

static void parse_buff_list(PacketReader* reader, BuffList* list)
{
    int i;

    list->count = reader_u8(reader);
    if (list->count > MAX_BUFF_SLOTS) {
        list->count = MAX_BUFF_SLOTS;
    }

    for (i = 0; i < list->count; i++) {
        parse_buff_data(reader, &list->buffs[i]);
    }
}

/*
 * ============================================================================
 * Response Parsing Functions (Public API)
 * ============================================================================
 */

int packet_parse_login_response(const u8* data, int len, LoginResponse* resp)
{
    PacketReader reader;
    int i;

    reader_init(&reader, data, len);

    resp->result = reader_u8(&reader);
    if (resp->result != 0) {
        return reader.error ? -1 : 0;
    }

    resp->account_id = reader_u32(&reader);
    resp->character_count = reader_u8(&reader);

    if (resp->character_count > MAX_CHARACTERS) {
        resp->character_count = MAX_CHARACTERS;
    }

    for (i = 0; i < resp->character_count; i++) {
        parse_character_info(&reader, &resp->characters[i]);
    }

    return reader.error ? -1 : 0;
}

int packet_parse_enter_world(const u8* data, int len, EnterWorldResponse* resp)
{
    PacketReader reader;
    int i;

    reader_init(&reader, data, len);

    resp->result = reader_u8(&reader);
    if (resp->result != 0) {
        return reader.error ? -1 : 0;
    }

    parse_character_data(&reader, &resp->character);

    /* Inventory */
    resp->inventory_count = reader_u8(&reader);
    if (resp->inventory_count > MAX_INVENTORY_SLOTS) {
        resp->inventory_count = MAX_INVENTORY_SLOTS;
    }
    for (i = 0; i < resp->inventory_count; i++) {
        parse_item_data(&reader, &resp->inventory[i]);
    }

    /* Equipment */
    for (i = 0; i < MAX_EQUIPMENT_SLOTS; i++) {
        parse_equipment_data(&reader, &resp->equipment[i]);
    }

    /* Skills */
    parse_skill_list(&reader, &resp->skills);

    /* Buffs */
    parse_buff_list(&reader, &resp->buffs);

    return reader.error ? -1 : 0;
}

int packet_parse_spawn(const u8* data, int len, SpawnData* spawn)
{
    PacketReader reader;
    reader_init(&reader, data, len);
    parse_spawn_data(&reader, spawn);
    return reader.error ? -1 : 0;
}

int packet_parse_position(const u8* data, int len, PositionSync* pos)
{
    PacketReader reader;
    reader_init(&reader, data, len);
    parse_position_sync(&reader, pos);
    return reader.error ? -1 : 0;
}

int packet_parse_damage(const u8* data, int len, DamageEvent* dmg)
{
    PacketReader reader;
    reader_init(&reader, data, len);
    parse_damage_event(&reader, dmg);
    return reader.error ? -1 : 0;
}

int packet_parse_death(const u8* data, int len, DeathEvent* death)
{
    PacketReader reader;
    reader_init(&reader, data, len);
    parse_death_event(&reader, death);
    return reader.error ? -1 : 0;
}

int packet_parse_inventory_update(const u8* data, int len, InventoryUpdate* update)
{
    PacketReader reader;
    reader_init(&reader, data, len);

    update->operation = reader_u8(&reader);
    update->slot = reader_u8(&reader);
    parse_item_data(&reader, &update->item);

    return reader.error ? -1 : 0;
}

int packet_parse_stat_update(const u8* data, int len, StatUpdate* update)
{
    PacketReader reader;
    reader_init(&reader, data, len);

    update->stat_type = reader_u8(&reader);
    update->value = reader_i32(&reader);

    return reader.error ? -1 : 0;
}

int packet_parse_chat(const u8* data, int len, ChatMessage* msg)
{
    PacketReader reader;
    reader_init(&reader, data, len);

    msg->channel = reader_u8(&reader);
    msg->sender_id = reader_u32(&reader);
    reader_fixed_string(&reader, msg->sender_name, MAX_NAME_LENGTH);
    reader_fixed_string(&reader, msg->message, MAX_CHAT_LENGTH);
    msg->timestamp = reader_u32(&reader);

    return reader.error ? -1 : 0;
}

int packet_parse_dungeon_info(const u8* data, int len, DungeonInfo* info)
{
    PacketReader reader;
    reader_init(&reader, data, len);

    info->dungeon_id = reader_u32(&reader);
    info->difficulty = reader_u8(&reader);
    info->room_count = reader_u8(&reader);
    info->current_room = reader_u8(&reader);
    info->clear_rank = reader_u8(&reader);
    info->time_limit = reader_u32(&reader);
    info->monster_count = reader_u32(&reader);
    info->monster_killed = reader_u32(&reader);

    return reader.error ? -1 : 0;
}

int packet_parse_dungeon_result(const u8* data, int len, DungeonResult* result)
{
    PacketReader reader;
    int i;

    reader_init(&reader, data, len);

    result->exp_gained = reader_u32(&reader);
    result->gold_gained = reader_u32(&reader);
    result->item_count = reader_u8(&reader);

    if (result->item_count > 16) result->item_count = 16;

    for (i = 0; i < result->item_count; i++) {
        parse_item_data(&reader, &result->items[i]);
    }

    result->rank = reader_u8(&reader);
    result->clear_time = reader_u32(&reader);

    return reader.error ? -1 : 0;
}

/*
 * ============================================================================
 * Simple Value Parsers
 * ============================================================================
 */

int packet_parse_u8(const u8* data, int len, u8* out)
{
    if (len < 1) return -1;
    *out = data[0];
    return 0;
}

int packet_parse_u16(const u8* data, int len, u16* out)
{
    if (len < 2) return -1;
    *out = data[0] | (data[1] << 8);
    return 0;
}

int packet_parse_u32(const u8* data, int len, u32* out)
{
    if (len < 4) return -1;
    *out = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    return 0;
}

int packet_parse_i32(const u8* data, int len, i32* out)
{
    u32 val;
    if (packet_parse_u32(data, len, &val) < 0) return -1;
    *out = (i32)val;
    return 0;
}

/*
 * ============================================================================
 * Debug Helpers
 * ============================================================================
 */

void packet_dump_hex(const u8* data, int len)
{
    int i;
    printf("Packet dump (%d bytes):\n", len);
    for (i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (len % 16 != 0) printf("\n");
}
