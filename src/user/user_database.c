/**
 * User Module Database Implementation
 * Database operations for User module (Phase 6.1 - Week 3)
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "common/defs.h"
#include "common/error_codes.h"
#include "database/headers/database_interface.h"
#include "database/headers/MySQL.h"
#include "user/user_account.h"
#include "user/user_character.h"
#include "user/user_session.h"
#include "user/user_database.h"

/* ==================== Account Database Operations ==================== */

/**
 * Load account from database by account name
 */
int User_DB_LoadAccount(const char* account_name, UserAccount* account)
{
    if (!account_name || !account) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_ACCOUNT);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Build and execute SELECT query
    MySQL_select_fmt(conn,
        "SELECT "
        "account_id, account_name, password_hash, "
        "status, user_type, "
        "register_date, register_ip, "
        "last_login_date, last_login_ip, current_login_time, is_currently_online, "
        "is_blocked, block_start_time, block_expire_time, block_reason, block_by_gm_id, "
        "vip_level, vip_expire_time, vip_points, vip_benefits_flags, "
        "security_question, security_answer_hash, email, phone, two_factor_enabled, last_password_change, "
        "login_attempt_count, last_failed_login, last_failed_ip, "
        "total_login_count, total_play_time, character_count, deleted_char_count, "
        "total_gold_earned, total_gold_spent, "
        "dungeon_cleared, pvp_matches, pvp_wins, pvp_losses, "
        "quests_completed, achievements_unlocked, "
        "warehouse_id, warehouse_slots_unlocked, warehouse_gold, "
        "is_gm, gm_level, is_return_user, return_user_expire, is_first_login, tutorial_completed, "
        "pc_room_no, pc_room_benefits_flags, "
        "school_no, school_name, "
        "gold_trade_restricted, gold_trade_restrict_expire, item_trade_restricted, item_trade_restrict_expire, "
        "hack_detection_flags, last_hack_detection_time, is_blacklisted, "
        "created_at, updated_at "
        "FROM user_accounts WHERE account_name = '%s'",
        account_name
    );

    // Fetch result
    if (MySQL_fetch(conn) != 1) {
        return ERR_ACCOUNT_NOT_FOUND;
    }

    // Load data from result set
    int col = 0;
    MySQL_get_uint(conn, col++, &account->account_id);
    MySQL_get_str(conn, col++, account->account_name, sizeof(account->account_name));
    MySQL_get_str(conn, col++, account->password_hash, sizeof(account->password_hash));

    MySQL_get_ubyte(conn, col++, &account->status);
    MySQL_get_ubyte(conn, col++, &account->user_type);

    MySQL_get_uint(conn, col++, &account->register_date);
    MySQL_get_str(conn, col++, account->register_ip, sizeof(account->register_ip));

    MySQL_get_uint(conn, col++, &account->last_login_date);
    MySQL_get_str(conn, col++, account->last_login_ip, sizeof(account->last_login_ip));
    MySQL_get_uint(conn, col++, &account->current_login_time);
    MySQL_get_ubyte(conn, col++, &account->is_currently_online);

    MySQL_get_ubyte(conn, col++, &account->is_blocked);
    MySQL_get_uint(conn, col++, &account->block_start_time);
    MySQL_get_uint(conn, col++, &account->block_expire_time);
    MySQL_get_str(conn, col++, account->block_reason, sizeof(account->block_reason));
    MySQL_get_uint(conn, col++, &account->block_by_gm_id);

    MySQL_get_ubyte(conn, col++, &account->vip_info.vip_level);
    MySQL_get_uint(conn, col++, &account->vip_info.vip_expire_time);
    MySQL_get_uint(conn, col++, &account->vip_info.vip_points);
    MySQL_get_ubyte(conn, col++, &account->vip_info.vip_benefits_flags);

    MySQL_get_str(conn, col++, account->security_info.security_question, sizeof(account->security_info.security_question));
    MySQL_get_str(conn, col++, account->security_info.security_answer_hash, sizeof(account->security_info.security_answer_hash));
    MySQL_get_str(conn, col++, account->security_info.email, sizeof(account->security_info.email));
    MySQL_get_str(conn, col++, account->security_info.phone, sizeof(account->security_info.phone));
    MySQL_get_ubyte(conn, col++, &account->security_info.two_factor_enabled);
    MySQL_get_uint(conn, col++, &account->security_info.last_password_change);

    MySQL_get_uint(conn, col++, &account->security_info.login_attempt_count);
    MySQL_get_uint(conn, col++, &account->security_info.last_failed_login);
    MySQL_get_str(conn, col++, account->security_info.last_failed_ip, sizeof(account->security_info.last_failed_ip));

    MySQL_get_uint(conn, col++, &account->statistics.total_login_count);
    MySQL_get_uint(conn, col++, &account->statistics.total_play_time);
    MySQL_get_ubyte(conn, col++, &account->statistics.character_count);
    MySQL_get_ubyte(conn, col++, &account->statistics.deleted_char_count);

    MySQL_get_ulonglong(conn, col++, &account->statistics.total_gold_earned);
    MySQL_get_ulonglong(conn, col++, &account->statistics.total_gold_spent);

    MySQL_get_uint(conn, col++, &account->statistics.dungeon_cleared);
    MySQL_get_uint(conn, col++, &account->statistics.pvp_matches);
    MySQL_get_ushort(conn, col++, &account->statistics.pvp_wins);
    MySQL_get_ushort(conn, col++, &account->statistics.pvp_losses);

    MySQL_get_uint(conn, col++, &account->statistics.quests_completed);
    MySQL_get_uint(conn, col++, &account->statistics.achievements_unlocked);

    MySQL_get_uint(conn, col++, &account->warehouse_id);
    MySQL_get_ubyte(conn, col++, &account->warehouse_slots_unlocked);
    MySQL_get_ulonglong(conn, col++, &account->warehouse_gold);

    MySQL_get_ubyte(conn, col++, &account->is_gm);
    MySQL_get_ubyte(conn, col++, &account->gm_level);
    MySQL_get_ubyte(conn, col++, &account->is_return_user);
    MySQL_get_uint(conn, col++, &account->return_user_expire);
    MySQL_get_ubyte(conn, col++, &account->is_first_login);
    MySQL_get_ubyte(conn, col++, &account->tutorial_completed);

    MySQL_get_uint(conn, col++, &account->pc_room_no);
    MySQL_get_ubyte(conn, col++, &account->pc_room_benefits_flags);

    MySQL_get_uint(conn, col++, &account->school_no);
    MySQL_get_str(conn, col++, account->school_name, sizeof(account->school_name));

    MySQL_get_ubyte(conn, col++, &account->gold_trade_restricted);
    MySQL_get_uint(conn, col++, &account->gold_trade_restrict_expire);
    MySQL_get_ubyte(conn, col++, &account->item_trade_restricted);
    MySQL_get_uint(conn, col++, &account->item_trade_restrict_expire);

    MySQL_get_uint(conn, col++, &account->hack_detection_flags);
    MySQL_get_uint(conn, col++, &account->last_hack_detection_time);
    MySQL_get_ubyte(conn, col++, &account->is_blacklisted);

    MySQL_get_uint(conn, col++, &account->created_at);
    MySQL_get_uint(conn, col++, &account->updated_at);

    MySQL_clear_result_set(conn);
    return ERR_SUCCESS;
}

/**
 * Save account to database
 */
int User_DB_SaveAccount(const UserAccount* account)
{
    if (!account) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_ACCOUNT);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Update timestamp
    uint32_t now = (uint32_t)time(NULL);

    // Execute UPDATE query
    int result = MySQL_update(conn,
        "UPDATE user_accounts SET "
        "password_hash = '%s', "
        "status = %u, user_type = %u, "
        "last_login_date = %u, last_login_ip = '%s', current_login_time = %u, is_currently_online = %u, "
        "is_blocked = %u, block_start_time = %u, block_expire_time = %u, block_reason = '%s', block_by_gm_id = %u, "
        "vip_level = %u, vip_expire_time = %u, vip_points = %u, vip_benefits_flags = %u, "
        "security_question = '%s', security_answer_hash = '%s', email = '%s', phone = '%s', two_factor_enabled = %u, last_password_change = %u, "
        "login_attempt_count = %u, last_failed_login = %u, last_failed_ip = '%s', "
        "total_login_count = %u, total_play_time = %u, character_count = %u, deleted_char_count = %u, "
        "total_gold_earned = %llu, total_gold_spent = %llu, "
        "dungeon_cleared = %u, pvp_matches = %u, pvp_wins = %u, pvp_losses = %u, "
        "quests_completed = %u, achievements_unlocked = %u, "
        "warehouse_id = %u, warehouse_slots_unlocked = %u, warehouse_gold = %llu, "
        "is_gm = %u, gm_level = %u, is_return_user = %u, return_user_expire = %u, is_first_login = %u, tutorial_completed = %u, "
        "pc_room_no = %u, pc_room_benefits_flags = %u, "
        "school_no = %u, school_name = '%s', "
        "gold_trade_restricted = %u, gold_trade_restrict_expire = %u, item_trade_restricted = %u, item_trade_restrict_expire = %u, "
        "hack_detection_flags = %u, last_hack_detection_time = %u, is_blacklisted = %u, "
        "updated_at = %u "
        "WHERE account_id = %u",
        account->password_hash,
        account->status, account->user_type,
        account->last_login_date, account->last_login_ip, account->current_login_time, account->is_currently_online,
        account->is_blocked, account->block_start_time, account->block_expire_time, account->block_reason, account->block_by_gm_id,
        account->vip_info.vip_level, account->vip_info.vip_expire_time, account->vip_info.vip_points, account->vip_info.vip_benefits_flags,
        account->security_info.security_question, account->security_info.security_answer_hash, account->security_info.email, account->security_info.phone, account->security_info.two_factor_enabled, account->security_info.last_password_change,
        account->security_info.login_attempt_count, account->security_info.last_failed_login, account->security_info.last_failed_ip,
        account->statistics.total_login_count, account->statistics.total_play_time, account->statistics.character_count, account->statistics.deleted_char_count,
        account->statistics.total_gold_earned, account->statistics.total_gold_spent,
        account->statistics.dungeon_cleared, account->statistics.pvp_matches, account->statistics.pvp_wins, account->statistics.pvp_losses,
        account->statistics.quests_completed, account->statistics.achievements_unlocked,
        account->warehouse_id, account->warehouse_slots_unlocked, account->warehouse_gold,
        account->is_gm, account->gm_level, account->is_return_user, account->return_user_expire, account->is_first_login, account->tutorial_completed,
        account->pc_room_no, account->pc_room_benefits_flags,
        account->school_no, account->school_name,
        account->gold_trade_restricted, account->gold_trade_restrict_expire, account->item_trade_restricted, account->item_trade_restrict_expire,
        account->hack_detection_flags, account->last_hack_detection_time, account->is_blacklisted,
        now,
        account->account_id
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/**
 * Create new account in database
 */
int User_DB_CreateAccount(const UserAccount* account)
{
    if (!account) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_ACCOUNT);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    uint32_t now = (uint32_t)time(NULL);

    // Use INSERT statement to create new account
    int result = MySQL_insert(conn,
        "INSERT INTO user_accounts ("
        "account_name, password_hash, email, register_ip, "
        "register_date, status, user_type, "
        "created_at, updated_at"
        ") VALUES ("
        "'%s', '%s', '%s', '%s', "
        "%u, %u, %u, "
        "%u, %u"
        ")",
        account->account_name,
        account->password_hash,
        account->security_info.email,
        account->register_ip,
        now,
        0,  // status = Active
        account->user_type,
        now,
        now
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/**
 * Delete account from database
 */
int User_DB_DeleteAccount(uint32_t account_id)
{
    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_ACCOUNT);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Soft delete: mark as deleted
    uint32_t now = (uint32_t)time(NULL);
    int result = MySQL_update(conn,
        "UPDATE user_accounts SET status = 3, updated_at = %u WHERE account_id = %u",
        now, account_id
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/* ==================== Character Database Operations ==================== */

/**
 * Load all characters for an account from database
 */
int User_DB_LoadCharacters(uint32_t account_id, Character** characters, uint8_t* character_count)
{
    if (!characters || !character_count) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_CHARACTER);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Execute SELECT query
    MySQL_select_fmt(conn,
        "SELECT "
        "character_id, character_name, status, "
        "job_class, job_grow_type, level, experience, next_level_exp, "
        "sp_points, tp_points, stat_points, "
        "gender, skin_color, hair_style, hair_color, face_style, eye_color, body_type, voice_type, "
        "current_zone, current_map, position_x, position_y, position_z, facing_angle, "
        "strength, intelligence, vitality, spirit, "
        "max_hp, current_hp, max_mp, current_mp, "
        "physical_attack, magical_attack, physical_defense, magical_defense, "
        "attack_speed, move_speed, cast_speed, "
        "critical_rate, critical_damage, hp_regen, mp_regen, "
        "gold, silver, cera_point, used_cera, mileage, win_point, "
        "inventory_id, inventory_slots_total, inventory_slots_used, "
        "guild_id, guild_rank, guild_contribution, "
        "party_index, party_position, "
        "dungeon_clear_count, fatigue_value, fatigue_max, last_fatigue_reset, "
        "quests_completed_count, quests_active_count, "
        "pvp_ranking, pvp_points, pvp_win_count, pvp_lose_count, pvp_draw_count, pvp_win_rate, "
        "achievements_unlocked_count, "
        "current_title_id, title_count, "
        "total_play_time, total_kills, total_deaths, total_damage_dealt, total_damage_received, "
        "is_awakened, awakening_level, is_in_combat, is_in_dungeon, is_in_pvp, is_trading, trade_partner_id, can_trade, can_drop, "
        "revival_time, revival_count, "
        "create_date, last_login, last_logout, delete_date, delete_expire, "
        "created_at, updated_at "
        "FROM characters WHERE account_id = %u AND status = 0 ORDER BY create_date",
        account_id
    );

    // Get row count
    int n_rows = MySQL_get_n_rows(conn);
    if (n_rows <= 0) {
        *character_count = 0;
        *characters = NULL;
        return ERR_SUCCESS;
    }

    // Allocate character array
    *characters = (Character*)malloc(sizeof(Character) * n_rows);
    if (!*characters) {
        return ERR_OUT_OF_MEMORY;
    }
    *character_count = (uint8_t)n_rows;

    // Fetch each character
    for (int i = 0; i < n_rows && MySQL_fetch(conn) == 1; i++) {
        Character* ch = &(*characters)[i];
        memset(ch, 0, sizeof(Character));

        int col = 0;
        MySQL_get_uint(conn, col++, &ch->character_id);
        MySQL_get_str(conn, col++, ch->character_name, sizeof(ch->character_name));
        MySQL_get_ubyte(conn, col++, &ch->status);

        MySQL_get_ubyte(conn, col++, &ch->job_class);
        MySQL_get_ubyte(conn, col++, &ch->job_grow_type);
        MySQL_get_ushort(conn, col++, &ch->level);
        MySQL_get_ulonglong(conn, col++, &ch->experience);
        MySQL_get_ulonglong(conn, col++, &ch->next_level_exp);

        MySQL_get_int(conn, col++, &ch->sp_points);
        MySQL_get_int(conn, col++, &ch->tp_points);
        MySQL_get_ushort(conn, col++, &ch->stat_points);

        MySQL_get_ubyte(conn, col++, &ch->appearance.gender);
        MySQL_get_ubyte(conn, col++, &ch->appearance.skin_color);
        MySQL_get_ubyte(conn, col++, &ch->appearance.hair_style);
        MySQL_get_ubyte(conn, col++, &ch->appearance.hair_color);
        MySQL_get_ubyte(conn, col++, &ch->appearance.face_style);
        MySQL_get_ubyte(conn, col++, &ch->appearance.eye_color);
        MySQL_get_ubyte(conn, col++, &ch->appearance.body_type);
        MySQL_get_ubyte(conn, col++, &ch->appearance.voice_type);

        MySQL_get_int(conn, col++, &ch->position.current_zone);
        MySQL_get_int(conn, col++, &ch->position.current_map);
        MySQL_get_float(conn, col++, &ch->position.position_x);
        MySQL_get_float(conn, col++, &ch->position.position_y);
        MySQL_get_float(conn, col++, &ch->position.position_z);
        MySQL_get_float(conn, col++, &ch->position.facing_angle);

        MySQL_get_ushort(conn, col++, &ch->attributes.strength);
        MySQL_get_ushort(conn, col++, &ch->attributes.intelligence);
        MySQL_get_ushort(conn, col++, &ch->attributes.vitality);
        MySQL_get_ushort(conn, col++, &ch->attributes.spirit);

        MySQL_get_int(conn, col++, &ch->attributes.max_hp);
        MySQL_get_int(conn, col++, &ch->attributes.current_hp);
        MySQL_get_int(conn, col++, &ch->attributes.max_mp);
        MySQL_get_int(conn, col++, &ch->attributes.current_mp);

        MySQL_get_int(conn, col++, &ch->attributes.physical_attack);
        MySQL_get_int(conn, col++, &ch->attributes.magical_attack);
        MySQL_get_int(conn, col++, &ch->attributes.physical_defense);
        MySQL_get_int(conn, col++, &ch->attributes.magical_defense);

        MySQL_get_int(conn, col++, &ch->attributes.attack_speed);
        MySQL_get_int(conn, col++, &ch->attributes.move_speed);
        MySQL_get_int(conn, col++, &ch->attributes.cast_speed);

        MySQL_get_float(conn, col++, &ch->attributes.critical_rate);
        MySQL_get_float(conn, col++, &ch->attributes.critical_damage);
        MySQL_get_int(conn, col++, &ch->attributes.hp_regen);
        MySQL_get_int(conn, col++, &ch->attributes.mp_regen);

        MySQL_get_ulonglong(conn, col++, &ch->currency.gold);
        MySQL_get_ulonglong(conn, col++, &ch->currency.silver);
        MySQL_get_uint(conn, col++, &ch->currency.cera_point);
        MySQL_get_uint(conn, col++, &ch->currency.used_cera);
        MySQL_get_int(conn, col++, &ch->currency.mileage);
        MySQL_get_int(conn, col++, &ch->currency.win_point);

        MySQL_get_uint(conn, col++, &ch->inventory_id);
        MySQL_get_ubyte(conn, col++, &ch->inventory_slots_total);
        MySQL_get_ubyte(conn, col++, &ch->inventory_slots_used);

        MySQL_get_uint(conn, col++, &ch->guild_id);
        MySQL_get_ubyte(conn, col++, &ch->guild_rank);
        MySQL_get_uint(conn, col++, &ch->guild_contribution);

        MySQL_get_short_i16(conn, col++, &ch->party_index);
        MySQL_get_ubyte(conn, col++, &ch->party_position);

        MySQL_get_uint(conn, col++, &ch->dungeon_clear_count);
        MySQL_get_ushort(conn, col++, &ch->fatigue_value);
        MySQL_get_ushort(conn, col++, &ch->fatigue_max);
        MySQL_get_uint(conn, col++, &ch->last_fatigue_reset);

        MySQL_get_ushort(conn, col++, &ch->quests_completed_count);
        MySQL_get_ushort(conn, col++, &ch->quests_active_count);

        MySQL_get_int(conn, col++, &ch->pvp_ranking);
        MySQL_get_int(conn, col++, &ch->pvp_points);
        MySQL_get_ushort(conn, col++, &ch->pvp_win_count);
        MySQL_get_ushort(conn, col++, &ch->pvp_lose_count);
        MySQL_get_ushort(conn, col++, &ch->pvp_draw_count);
        MySQL_get_float(conn, col++, &ch->pvp_win_rate);

        MySQL_get_ushort(conn, col++, &ch->achievements_unlocked_count);

        MySQL_get_ushort(conn, col++, &ch->current_title_id);
        MySQL_get_ushort(conn, col++, &ch->title_count);

        MySQL_get_uint(conn, col++, &ch->total_play_time);
        MySQL_get_uint(conn, col++, &ch->total_kills);
        MySQL_get_uint(conn, col++, &ch->total_deaths);
        MySQL_get_ulonglong(conn, col++, &ch->total_damage_dealt);
        MySQL_get_ulonglong(conn, col++, &ch->total_damage_received);

        MySQL_get_ubyte(conn, col++, &ch->is_awakened);
        MySQL_get_ubyte(conn, col++, &ch->awakening_level);
        MySQL_get_ubyte(conn, col++, &ch->is_in_combat);
        MySQL_get_ubyte(conn, col++, &ch->is_in_dungeon);
        MySQL_get_ubyte(conn, col++, &ch->is_in_pvp);
        MySQL_get_ubyte(conn, col++, &ch->is_trading);
        MySQL_get_uint(conn, col++, &ch->trade_partner_id);
        MySQL_get_ubyte(conn, col++, &ch->can_trade);
        MySQL_get_ubyte(conn, col++, &ch->can_drop);

        MySQL_get_uint(conn, col++, &ch->revival_time);
        MySQL_get_ubyte(conn, col++, &ch->revival_count);

        MySQL_get_uint(conn, col++, &ch->create_date);
        MySQL_get_uint(conn, col++, &ch->last_login);
        MySQL_get_uint(conn, col++, &ch->last_logout);
        MySQL_get_uint(conn, col++, &ch->delete_date);
        MySQL_get_uint(conn, col++, &ch->delete_expire);

        MySQL_get_uint(conn, col++, &ch->created_at);
        MySQL_get_uint(conn, col++, &ch->updated_at);
    }

    MySQL_clear_result_set(conn);
    return ERR_SUCCESS;
}

/**
 * Load single character from database by character ID
 */
int User_DB_LoadCharacter(uint32_t character_id, Character* character)
{
    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_CHARACTER);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Execute SELECT query
    MySQL_select_fmt(conn,
        "SELECT "
        "character_id, character_name, status, "
        "job_class, job_grow_type, level, experience, next_level_exp, "
        "sp_points, tp_points, stat_points, "
        "gender, skin_color, hair_style, hair_color, face_style, eye_color, body_type, voice_type, "
        "current_zone, current_map, position_x, position_y, position_z, facing_angle, "
        "strength, intelligence, vitality, spirit, "
        "max_hp, current_hp, max_mp, current_mp, "
        "physical_attack, magical_attack, physical_defense, magical_defense, "
        "attack_speed, move_speed, cast_speed, "
        "critical_rate, critical_damage, hp_regen, mp_regen, "
        "gold, silver, cera_point, used_cera, mileage, win_point, "
        "inventory_id, inventory_slots_total, inventory_slots_used, "
        "guild_id, guild_rank, guild_contribution, "
        "party_index, party_position, "
        "dungeon_clear_count, fatigue_value, fatigue_max, last_fatigue_reset, "
        "quests_completed_count, quests_active_count, "
        "pvp_ranking, pvp_points, pvp_win_count, pvp_lose_count, pvp_draw_count, pvp_win_rate, "
        "achievements_unlocked_count, "
        "current_title_id, title_count, "
        "total_play_time, total_kills, total_deaths, total_damage_dealt, total_damage_received, "
        "is_awakened, awakening_level, is_in_combat, is_in_dungeon, is_in_pvp, is_trading, trade_partner_id, can_trade, can_drop, "
        "revival_time, revival_count, "
        "create_date, last_login, last_logout, delete_date, delete_expire, "
        "created_at, updated_at "
        "FROM characters WHERE character_id = %u",
        character_id
    );

    // Fetch result
    if (MySQL_fetch(conn) != 1) {
        return ERR_CHARACTER_NOT_FOUND;
    }

    // Load character data
    memset(character, 0, sizeof(Character));

    int col = 0;
    MySQL_get_uint(conn, col++, &character->character_id);
    MySQL_get_str(conn, col++, character->character_name, sizeof(character->character_name));
    MySQL_get_ubyte(conn, col++, &character->status);

    MySQL_get_ubyte(conn, col++, &character->job_class);
    MySQL_get_ubyte(conn, col++, &character->job_grow_type);
    MySQL_get_ushort(conn, col++, &character->level);
    MySQL_get_ulonglong(conn, col++, &character->experience);
    MySQL_get_ulonglong(conn, col++, &character->next_level_exp);

    MySQL_get_int(conn, col++, &character->sp_points);
    MySQL_get_int(conn, col++, &character->tp_points);
    MySQL_get_ushort(conn, col++, &character->stat_points);

    MySQL_get_ubyte(conn, col++, &character->appearance.gender);
    MySQL_get_ubyte(conn, col++, &character->appearance.skin_color);
    MySQL_get_ubyte(conn, col++, &character->appearance.hair_style);
    MySQL_get_ubyte(conn, col++, &character->appearance.hair_color);
    MySQL_get_ubyte(conn, col++, &character->appearance.face_style);
    MySQL_get_ubyte(conn, col++, &character->appearance.eye_color);
    MySQL_get_ubyte(conn, col++, &character->appearance.body_type);
    MySQL_get_ubyte(conn, col++, &character->appearance.voice_type);

    MySQL_get_int(conn, col++, &character->position.current_zone);
    MySQL_get_int(conn, col++, &character->position.current_map);
    MySQL_get_float(conn, col++, &character->position.position_x);
    MySQL_get_float(conn, col++, &character->position.position_y);
    MySQL_get_float(conn, col++, &character->position.position_z);
    MySQL_get_float(conn, col++, &character->position.facing_angle);

    MySQL_get_ushort(conn, col++, &character->attributes.strength);
    MySQL_get_ushort(conn, col++, &character->attributes.intelligence);
    MySQL_get_ushort(conn, col++, &character->attributes.vitality);
    MySQL_get_ushort(conn, col++, &character->attributes.spirit);

    MySQL_get_int(conn, col++, &character->attributes.max_hp);
    MySQL_get_int(conn, col++, &character->attributes.current_hp);
    MySQL_get_int(conn, col++, &character->attributes.max_mp);
    MySQL_get_int(conn, col++, &character->attributes.current_mp);

    MySQL_get_int(conn, col++, &character->attributes.physical_attack);
    MySQL_get_int(conn, col++, &character->attributes.magical_attack);
    MySQL_get_int(conn, col++, &character->attributes.physical_defense);
    MySQL_get_int(conn, col++, &character->attributes.magical_defense);

    MySQL_get_int(conn, col++, &character->attributes.attack_speed);
    MySQL_get_int(conn, col++, &character->attributes.move_speed);
    MySQL_get_int(conn, col++, &character->attributes.cast_speed);

    MySQL_get_float(conn, col++, &character->attributes.critical_rate);
    MySQL_get_float(conn, col++, &character->attributes.critical_damage);
    MySQL_get_int(conn, col++, &character->attributes.hp_regen);
    MySQL_get_int(conn, col++, &character->attributes.mp_regen);

    MySQL_get_ulonglong(conn, col++, &character->currency.gold);
    MySQL_get_ulonglong(conn, col++, &character->currency.silver);
    MySQL_get_uint(conn, col++, &character->currency.cera_point);
    MySQL_get_uint(conn, col++, &character->currency.used_cera);
    MySQL_get_int(conn, col++, &character->currency.mileage);
    MySQL_get_int(conn, col++, &character->currency.win_point);

    MySQL_get_uint(conn, col++, &character->inventory_id);
    MySQL_get_ubyte(conn, col++, &character->inventory_slots_total);
    MySQL_get_ubyte(conn, col++, &character->inventory_slots_used);

    MySQL_get_uint(conn, col++, &character->guild_id);
    MySQL_get_ubyte(conn, col++, &character->guild_rank);
    MySQL_get_uint(conn, col++, &character->guild_contribution);

    MySQL_get_short_i16(conn, col++, &character->party_index);
    MySQL_get_ubyte(conn, col++, &character->party_position);

    MySQL_get_uint(conn, col++, &character->dungeon_clear_count);
    MySQL_get_ushort(conn, col++, &character->fatigue_value);
    MySQL_get_ushort(conn, col++, &character->fatigue_max);
    MySQL_get_uint(conn, col++, &character->last_fatigue_reset);

    MySQL_get_ushort(conn, col++, &character->quests_completed_count);
    MySQL_get_ushort(conn, col++, &character->quests_active_count);

    MySQL_get_int(conn, col++, &character->pvp_ranking);
    MySQL_get_int(conn, col++, &character->pvp_points);
    MySQL_get_ushort(conn, col++, &character->pvp_win_count);
    MySQL_get_ushort(conn, col++, &character->pvp_lose_count);
    MySQL_get_ushort(conn, col++, &character->pvp_draw_count);
    MySQL_get_float(conn, col++, &character->pvp_win_rate);

    MySQL_get_ushort(conn, col++, &character->achievements_unlocked_count);

    MySQL_get_ushort(conn, col++, &character->current_title_id);
    MySQL_get_ushort(conn, col++, &character->title_count);

    MySQL_get_uint(conn, col++, &character->total_play_time);
    MySQL_get_uint(conn, col++, &character->total_kills);
    MySQL_get_uint(conn, col++, &character->total_deaths);
    MySQL_get_ulonglong(conn, col++, &character->total_damage_dealt);
    MySQL_get_ulonglong(conn, col++, &character->total_damage_received);

    MySQL_get_ubyte(conn, col++, &character->is_awakened);
    MySQL_get_ubyte(conn, col++, &character->awakening_level);
    MySQL_get_ubyte(conn, col++, &character->is_in_combat);
    MySQL_get_ubyte(conn, col++, &character->is_in_dungeon);
    MySQL_get_ubyte(conn, col++, &character->is_in_pvp);
    MySQL_get_ubyte(conn, col++, &character->is_trading);
    MySQL_get_uint(conn, col++, &character->trade_partner_id);
    MySQL_get_ubyte(conn, col++, &character->can_trade);
    MySQL_get_ubyte(conn, col++, &character->can_drop);

    MySQL_get_uint(conn, col++, &character->revival_time);
    MySQL_get_ubyte(conn, col++, &character->revival_count);

    MySQL_get_uint(conn, col++, &character->create_date);
    MySQL_get_uint(conn, col++, &character->last_login);
    MySQL_get_uint(conn, col++, &character->last_logout);
    MySQL_get_uint(conn, col++, &character->delete_date);
    MySQL_get_uint(conn, col++, &character->delete_expire);

    MySQL_get_uint(conn, col++, &character->created_at);
    MySQL_get_uint(conn, col++, &character->updated_at);

    MySQL_clear_result_set(conn);
    return ERR_SUCCESS;
}

/**
 * Save character to database
 */
int User_DB_SaveCharacter(const Character* character)
{
    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_CHARACTER);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    uint32_t now = (uint32_t)time(NULL);

    // Execute UPDATE query
    int result = MySQL_update(conn,
        "UPDATE characters SET "
        "character_name = '%s', status = %u, "
        "job_class = %u, job_grow_type = %u, level = %u, experience = %llu, next_level_exp = %llu, "
        "sp_points = %d, tp_points = %d, stat_points = %u, "
        "gender = %u, skin_color = %u, hair_style = %u, hair_color = %u, face_style = %u, eye_color = %u, body_type = %u, voice_type = %u, "
        "current_zone = %d, current_map = %d, position_x = %f, position_y = %f, position_z = %f, facing_angle = %f, "
        "strength = %u, intelligence = %u, vitality = %u, spirit = %u, "
        "max_hp = %d, current_hp = %d, max_mp = %d, current_mp = %d, "
        "physical_attack = %d, magical_attack = %d, physical_defense = %d, magical_defense = %d, "
        "attack_speed = %d, move_speed = %d, cast_speed = %d, "
        "critical_rate = %f, critical_damage = %f, hp_regen = %d, mp_regen = %d, "
        "gold = %llu, silver = %llu, cera_point = %u, used_cera = %u, mileage = %d, win_point = %d, "
        "inventory_id = %u, inventory_slots_total = %u, inventory_slots_used = %u, "
        "guild_id = %u, guild_rank = %u, guild_contribution = %u, "
        "party_index = %d, party_position = %u, "
        "dungeon_clear_count = %u, fatigue_value = %u, fatigue_max = %u, last_fatigue_reset = %u, "
        "quests_completed_count = %u, quests_active_count = %u, "
        "pvp_ranking = %d, pvp_points = %d, pvp_win_count = %u, pvp_lose_count = %u, pvp_draw_count = %u, pvp_win_rate = %f, "
        "achievements_unlocked_count = %u, "
        "current_title_id = %u, title_count = %u, "
        "total_play_time = %u, total_kills = %u, total_deaths = %u, total_damage_dealt = %llu, total_damage_received = %llu, "
        "is_awakened = %u, awakening_level = %u, is_in_combat = %u, is_in_dungeon = %u, is_in_pvp = %u, is_trading = %u, trade_partner_id = %u, can_trade = %u, can_drop = %u, "
        "revival_time = %u, revival_count = %u, "
        "last_login = %u, last_logout = %u, delete_date = %u, delete_expire = %u, "
        "updated_at = %u "
        "WHERE character_id = %u",
        character->character_name, character->status,
        character->job_class, character->job_grow_type, character->level, character->experience, character->next_level_exp,
        character->sp_points, character->tp_points, character->stat_points,
        character->appearance.gender, character->appearance.skin_color, character->appearance.hair_style, character->appearance.hair_color, character->appearance.face_style, character->appearance.eye_color, character->appearance.body_type, character->appearance.voice_type,
        character->position.current_zone, character->position.current_map, character->position.position_x, character->position.position_y, character->position.position_z, character->position.facing_angle,
        character->attributes.strength, character->attributes.intelligence, character->attributes.vitality, character->attributes.spirit,
        character->attributes.max_hp, character->attributes.current_hp, character->attributes.max_mp, character->attributes.current_mp,
        character->attributes.physical_attack, character->attributes.magical_attack, character->attributes.physical_defense, character->attributes.magical_defense,
        character->attributes.attack_speed, character->attributes.move_speed, character->attributes.cast_speed,
        character->attributes.critical_rate, character->attributes.critical_damage, character->attributes.hp_regen, character->attributes.mp_regen,
        character->currency.gold, character->currency.silver, character->currency.cera_point, character->currency.used_cera, character->currency.mileage, character->currency.win_point,
        character->inventory_id, character->inventory_slots_total, character->inventory_slots_used,
        character->guild_id, character->guild_rank, character->guild_contribution,
        character->party_index, character->party_position,
        character->dungeon_clear_count, character->fatigue_value, character->fatigue_max, character->last_fatigue_reset,
        character->quests_completed_count, character->quests_active_count,
        character->pvp_ranking, character->pvp_points, character->pvp_win_count, character->pvp_lose_count, character->pvp_draw_count, character->pvp_win_rate,
        character->achievements_unlocked_count,
        character->current_title_id, character->title_count,
        character->total_play_time, character->total_kills, character->total_deaths, character->total_damage_dealt, character->total_damage_received,
        character->is_awakened, character->awakening_level, character->is_in_combat, character->is_in_dungeon, character->is_in_pvp, character->is_trading, character->trade_partner_id, character->can_trade, character->can_drop,
        character->revival_time, character->revival_count,
        character->last_login, character->last_logout, character->delete_date, character->delete_expire,
        now,
        character->character_id
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/**
 * Create new character in database
 */
int User_DB_CreateCharacter(const Character* character)
{
    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_CHARACTER);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    uint32_t now = (uint32_t)time(NULL);

    // Use INSERT statement to create new character
    int result = MySQL_insert(conn,
        "INSERT INTO characters ("
        "account_id, character_name, status, "
        "job_class, job_grow_type, level, experience, next_level_exp, "
        "sp_points, tp_points, stat_points, "
        "gender, skin_color, hair_style, hair_color, face_style, eye_color, body_type, voice_type, "
        "current_zone, current_map, position_x, position_y, position_z, facing_angle, "
        "strength, intelligence, vitality, spirit, "
        "max_hp, current_hp, max_mp, current_mp, "
        "physical_attack, magical_attack, physical_defense, magical_defense, "
        "attack_speed, move_speed, cast_speed, "
        "critical_rate, critical_damage, hp_regen, mp_regen, "
        "gold, silver, cera_point, used_cera, mileage, win_point, "
        "inventory_slots_total, inventory_slots_used, "
        "guild_id, guild_rank, guild_contribution, "
        "party_index, party_position, "
        "dungeon_clear_count, fatigue_value, fatigue_max, "
        "quests_completed_count, quests_active_count, "
        "pvp_ranking, pvp_points, pvp_win_count, pvp_lose_count, pvp_draw_count, pvp_win_rate, "
        "achievements_unlocked_count, "
        "current_title_id, title_count, "
        "total_play_time, total_kills, total_deaths, total_damage_dealt, total_damage_received, "
        "is_awakened, awakening_level, is_in_combat, is_in_dungeon, is_in_pvp, is_trading, trade_partner_id, can_trade, can_drop, "
        "revival_time, revival_count, "
        "create_date, last_login, last_logout, "
        "created_at, updated_at"
        ") VALUES ("
        "%u, '%s', %u, "
        "%u, %u, %u, %llu, %llu, "
        "%d, %d, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%d, %d, %f, %f, %f, %f, "
        "%u, %u, %u, %u, "
        "%d, %d, %d, %d, "
        "%d, %d, %d, %d, "
        "%d, %d, %d, "
        "%f, %f, %d, %d, "
        "%llu, %llu, %u, %u, %d, %d, "
        "%u, %u, "
        "%u, %u, %u, "
        "%d, %u, "
        "%u, %u, %u, "
        "%u, %u, "
        "%d, %d, %u, %u, %u, %f, "
        "%u, "
        "%u, %u, "
        "%u, %u, %u, %llu, %llu, "
        "%u, %u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %u, "
        "%u, %u, %u, "
        "%u, %u"
        ")",
        character->account_id,
        character->character_name,
        0,  // status = Active
        character->job_class,
        character->job_grow_type,
        character->level,
        character->experience,
        character->next_level_exp,
        character->sp_points,
        character->tp_points,
        character->stat_points,
        character->appearance.gender,
        character->appearance.skin_color,
        character->appearance.hair_style,
        character->appearance.hair_color,
        character->appearance.face_style,
        character->appearance.eye_color,
        character->appearance.body_type,
        character->appearance.voice_type,
        character->position.current_zone,
        character->position.current_map,
        character->position.position_x,
        character->position.position_y,
        character->position.position_z,
        character->position.facing_angle,
        character->attributes.strength,
        character->attributes.intelligence,
        character->attributes.vitality,
        character->attributes.spirit,
        character->attributes.max_hp,
        character->attributes.current_hp,
        character->attributes.max_mp,
        character->attributes.current_mp,
        character->attributes.physical_attack,
        character->attributes.magical_attack,
        character->attributes.physical_defense,
        character->attributes.magical_defense,
        character->attributes.attack_speed,
        character->attributes.move_speed,
        character->attributes.cast_speed,
        character->attributes.critical_rate,
        character->attributes.critical_damage,
        character->attributes.hp_regen,
        character->attributes.mp_regen,
        character->currency.gold,
        character->currency.silver,
        character->currency.cera_point,
        character->currency.used_cera,
        character->currency.mileage,
        character->currency.win_point,
        character->inventory_slots_total,
        character->inventory_slots_used,
        character->guild_id,
        character->guild_rank,
        character->guild_contribution,
        character->party_index,
        character->party_position,
        character->dungeon_clear_count,
        character->fatigue_value,
        character->fatigue_max,
        character->quests_completed_count,
        character->quests_active_count,
        character->pvp_ranking,
        character->pvp_points,
        character->pvp_win_count,
        character->pvp_lose_count,
        character->pvp_draw_count,
        character->pvp_win_rate,
        character->achievements_unlocked_count,
        character->current_title_id,
        character->title_count,
        character->total_play_time,
        character->total_kills,
        character->total_deaths,
        character->total_damage_dealt,
        character->total_damage_received,
        character->is_awakened,
        character->awakening_level,
        character->is_in_combat,
        character->is_in_dungeon,
        character->is_in_pvp,
        character->is_trading,
        character->trade_partner_id,
        character->can_trade,
        character->can_drop,
        character->revival_time,
        character->revival_count,
        now,  // create_date
        0,    // last_login
        0,    // last_logout
        now,  // created_at
        now   // updated_at
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/**
 * Delete character from database
 */
int User_DB_DeleteCharacter(uint32_t character_id)
{
    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_CHARACTER);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    uint32_t now = (uint32_t)time(NULL);

    // Soft delete: mark for deletion
    int result = MySQL_update(conn,
        "UPDATE characters SET status = 1, delete_date = %u, delete_expire = %u WHERE character_id = %u",
        now, now + (7 * 24 * 60 * 60), character_id  // 7-day deletion period
    );

    return (result == 1) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/* ==================== Session Database Operations ==================== */

/**
 * Save session to database
 */
int User_DB_SaveSession(const UserSession* session)
{
    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_GAME);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    uint32_t now = (uint32_t)time(NULL);

    // Use REPLACE INTO for upsert behavior (insert or update if account_id exists)
    MySQL_set_query_fmt(conn,
        "REPLACE INTO user_sessions ("
        "account_id, user_id, username, "
        "is_authenticated, session_state, "
        "active_character_id, active_character_index, "
        "socket_fd, client_ip, client_port, "
        "login_time, last_activity, last_heartbeat, session_duration, "
        "user_type, is_gm, gm_level, "
        "client_version, client_hash, "
        "packet_sequence, invalid_packet_count, suspicious_action_count, "
        "speed_hack_detected, teleport_hack_detected, duplicate_login_detected, "
        "packets_sent, packets_received, bytes_sent, bytes_received, "
        "is_in_transaction, pending_disconnect, force_disconnect, "
        "session_token, login_ticket, "
        "pc_room_no"
        ") VALUES ("
        "%u, %u, '%s', "
        "%u, %u, "
        "%u, %d, "
        "%d, '%s', %u, "
        "%u, %u, %u, %u, "
        "%u, %u, %u, "
        "%u, '%s', "
        "%u, %u, %u, "
        "%u, %u, %u, "
        "%u, %u, %llu, %llu, "
        "%u, %u, %u, "
        "'%s', %u, "
        "%u"
        ")",
        session->account_id,
        session->user_id,
        session->username,
        session->is_authenticated,
        session->session_state,
        session->active_character_id,
        session->active_character_index,
        session->socket_fd,
        session->client_ip,
        session->client_port,
        session->login_time,
        now,  // last_activity = now
        now,  // last_heartbeat = now
        session->session_duration,
        session->user_type,
        session->is_gm,
        session->gm_level,
        session->client_version,
        session->client_hash,
        session->packet_sequence,
        session->invalid_packet_count,
        session->suspicious_action_count,
        session->speed_hack_detected,
        session->teleport_hack_detected,
        session->duplicate_login_detected,
        session->packets_sent,
        session->packets_received,
        session->bytes_sent,
        session->bytes_received,
        session->is_in_transaction,
        session->pending_disconnect,
        session->force_disconnect,
        session->session_token,
        session->login_ticket,
        session->pc_room_no
    );

    int result = MySQL_exec_query(conn);
    return (result >= 0) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/**
 * Load session from database by account ID
 */
int User_DB_LoadSession(uint32_t account_id, UserSession* session)
{
    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_GAME);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Execute SELECT query
    MySQL_select_fmt(conn,
        "SELECT "
        "session_id, account_id, user_id, username, "
        "is_authenticated, session_state, "
        "active_character_id, active_character_index, "
        "socket_fd, client_ip, client_port, "
        "login_time, last_activity, last_heartbeat, session_duration, "
        "user_type, is_gm, gm_level, "
        "client_version, client_hash, "
        "packet_sequence, invalid_packet_count, suspicious_action_count, "
        "speed_hack_detected, teleport_hack_detected, duplicate_login_detected, "
        "packets_sent, packets_received, bytes_sent, bytes_received, "
        "is_in_transaction, pending_disconnect, force_disconnect, "
        "session_token, login_ticket, "
        "pc_room_no "
        "FROM user_sessions WHERE account_id = %u",
        account_id
    );

    // Fetch result
    if (MySQL_fetch(conn) != 1) {
        return ERR_SESSION_NOT_FOUND;
    }

    // Load data
    int col = 0;
    MySQL_get_uint(conn, col++, &session->session_id);
    MySQL_get_uint(conn, col++, &session->account_id);
    MySQL_get_uint(conn, col++, &session->user_id);
    MySQL_get_str(conn, col++, session->username, sizeof(session->username));

    MySQL_get_ubyte(conn, col++, &session->is_authenticated);
    MySQL_get_ubyte(conn, col++, &session->session_state);

    MySQL_get_uint(conn, col++, &session->active_character_id);
    MySQL_get_byte_char(conn, col++, &session->active_character_index);

    MySQL_get_int(conn, col++, &session->socket_fd);
    MySQL_get_str(conn, col++, session->client_ip, sizeof(session->client_ip));
    MySQL_get_ushort(conn, col++, &session->client_port);

    MySQL_get_uint(conn, col++, &session->login_time);
    MySQL_get_uint(conn, col++, &session->last_activity);
    MySQL_get_uint(conn, col++, &session->last_heartbeat);
    MySQL_get_uint(conn, col++, &session->session_duration);

    MySQL_get_ubyte(conn, col++, &session->user_type);
    MySQL_get_ubyte(conn, col++, &session->is_gm);
    MySQL_get_ubyte(conn, col++, &session->gm_level);

    MySQL_get_uint(conn, col++, &session->client_version);
    MySQL_get_str(conn, col++, session->client_hash, sizeof(session->client_hash));

    MySQL_get_uint(conn, col++, &session->packet_sequence);
    MySQL_get_uint(conn, col++, &session->invalid_packet_count);
    MySQL_get_uint(conn, col++, &session->suspicious_action_count);

    MySQL_get_ubyte(conn, col++, &session->speed_hack_detected);
    MySQL_get_ubyte(conn, col++, &session->teleport_hack_detected);
    MySQL_get_ubyte(conn, col++, &session->duplicate_login_detected);

    MySQL_get_uint(conn, col++, &session->packets_sent);
    MySQL_get_uint(conn, col++, &session->packets_received);
    MySQL_get_ulonglong(conn, col++, &session->bytes_sent);
    MySQL_get_ulonglong(conn, col++, &session->bytes_received);

    MySQL_get_ubyte(conn, col++, &session->is_in_transaction);
    MySQL_get_ubyte(conn, col++, &session->pending_disconnect);
    MySQL_get_ubyte(conn, col++, &session->force_disconnect);

    MySQL_get_str(conn, col++, session->session_token, sizeof(session->session_token));
    MySQL_get_uint(conn, col++, &session->login_ticket);

    MySQL_get_uint(conn, col++, &session->pc_room_no);

    MySQL_clear_result_set(conn);
    return ERR_SUCCESS;
}

/**
 * Delete session from database
 */
int User_DB_DeleteSession(uint32_t account_id)
{
    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_GAME);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    int result = MySQL_update(conn,
        "DELETE FROM user_sessions WHERE account_id = %u",
        account_id
    );

    return (result >= 0) ? ERR_SUCCESS : ERR_DATABASE_QUERY_FAILED;
}

/* ==================== Stored Procedure Wrappers ==================== */

/**
 * Authenticate account using stored procedure
 */
int User_DB_AuthenticateAccount(const char* account_name, const char* password_hash,
                                  const char* client_ip, uint32_t* account_id)
{
    if (!account_name || !password_hash || !client_ip || !account_id) {
        return ERR_INVALID_PARAMETER;
    }

    MySQL* conn = (MySQL*)Database_GetTypedConnection(DB_ACCOUNT);
    if (!conn) {
        return ERR_DATABASE_CONNECTION_FAILED;
    }

    // Call sp_authenticate_account stored procedure with output parameters
    MySQL_set_query_fmt(conn,
        "CALL sp_authenticate_account('%s', '%s', '%s', @out_account_id, @out_result)",
        account_name,
        password_hash,
        client_ip
    );

    int result = MySQL_exec_query(conn);
    if (result < 0) {
        return ERR_DATABASE_QUERY_FAILED;
    }

    // Clear any result set from CALL statement
    MySQL_clear_result_set(conn);

    // Retrieve output parameters using SELECT
    MySQL_select_fmt(conn, "SELECT @out_account_id, @out_result");

    if (MySQL_fetch(conn) != 1) {
        return ERR_DATABASE_QUERY_FAILED;
    }

    // Get output values
    uint32_t sp_result;
    MySQL_get_uint(conn, 0, account_id);
    MySQL_get_uint(conn, 1, &sp_result);

    MySQL_clear_result_set(conn);

    // Interpret stored procedure result code
    switch (sp_result) {
        case 0:  // Success
            return ERR_SUCCESS;
        case 1:  // Account not found
            return ERR_ACCOUNT_NOT_FOUND;
        case 2:  // Account blocked
            return ERR_ACCOUNT_BLOCKED;
        case 3:  // Wrong password
            return ERR_AUTHENTICATION_FAILED;
        default:
            return ERR_DATABASE_QUERY_FAILED;
    }
}
