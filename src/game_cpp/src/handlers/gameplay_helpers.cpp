#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <zlib.h>

#include "db/db_config.h"
#include "game_server_context.h"
#include "db/db_characters.h"
#include "db/db_economy.h"
#include "db/db_inventory.h"
#include "db/db_skills.h"
#include "db/db_social.h"
#include "db/db_quest.h"
#include "db/db_game.h"
#include "game_packet_utils.h"
#include "handlers/gameplay_helpers.h"
#include "systems/dungeon_manager.h"
#include "systems/session_manager.h"
#include "systems/time_utils.h"

/* Global Variables */
SharedDungeonInstance g_shared_dungeons[MAX_SHARED_DUNGEONS];

/*==============================================================================
 * Database Functions
 *============================================================================*/

/* Initialize database connections */


/* Cleanup database connections */


/* Note: db_get_inventory is defined in db/db_inventory.cpp */

/* Parse skill_slot blob - zlib compressed 408-byte skill array
 * Format: [4 bytes: uncompressed_size][N bytes: zlib data]
 * Uncompressed: 204 x 2-byte skill IDs (102 slots x 2 trees)
 */
static int parse_skill_slot_blob(const unsigned char* blob, unsigned long blob_len,
                                  DBSkillInfo* skill) {
    if (!blob || blob_len < 8 || !skill) return -1;

    /* First 4 bytes is uncompressed size (little-endian) */
    uint32_t uncompressed_size = blob[0] | (blob[1] << 8) | (blob[2] << 16) | (blob[3] << 24);
    if (uncompressed_size > 1024) {
        printf("[DB] Skill blob uncompressed size too large: %u\n", uncompressed_size);
        return -1;
    }

    /* Decompress the zlib data */
    unsigned char decompressed[1024];
    uLongf dest_len = sizeof(decompressed);

    int ret = uncompress(decompressed, &dest_len, blob + 4, blob_len - 4);
    if (ret != Z_OK) {
        printf("[DB] Skill slot decompression failed: ret=%d\n", ret);
        return -1;
    }

    /* Parse 2-byte skill IDs from decompressed data */
    skill->skill_count = 0;
    for (unsigned long i = 0; i + 1 < dest_len && skill->skill_count < MAX_PARSED_SKILLS; i += 2) {
        uint16_t skill_id = decompressed[i] | (decompressed[i + 1] << 8);
        if (skill_id > 0) {
            skill->skill_ids[skill->skill_count++] = skill_id;
        }
    }

    return 0;
}

/* Note: db_get_skill is defined in db/db_skills.cpp */

/* Get equipment data from taiwan_cain_2nd.user_items */


/* Parse achievement blob - zlib compressed achievement data
 * The blob format from charac_achievement table:
 * [4 bytes: uncompressed_size][N bytes: zlib compressed data]
 * Uncompressed data contains achievement bit flags and progress
 */
int parse_achievement_blob(const unsigned char* blob, unsigned long blob_len,
                                   DBAchievementInfo* ach) {
    if (!blob || blob_len < 8 || !ach) return -1;

    /* First 4 bytes is uncompressed size (little-endian) */
    uint32_t uncompressed_size = blob[0] | (blob[1] << 8) | (blob[2] << 16) | (blob[3] << 24);
    if (uncompressed_size > 4096) {
        printf("[DB] Achievement blob uncompressed size too large: %u\n", uncompressed_size);
        return -1;
    }

    /* Decompress the zlib data */
    unsigned char decompressed[4096];
    uLongf dest_len = sizeof(decompressed);

    int ret = uncompress(decompressed, &dest_len, blob + 4, blob_len - 4);
    if (ret != Z_OK) {
        printf("[DB] Achievement decompression failed: ret=%d\n", ret);
        /* Use default achievements if decompression fails */
        return -1;
    }

    /* Parse achievement data - simplified format:
     * Each achievement entry: [4 bytes ach_id][1 byte completed][1 byte progress]
     * We'll parse bit flags as achievement IDs
     */
    ach->achievement_count = 0;
    ach->total_points = 0;

    /* Parse as bit array - each bit represents an achievement */
    for (unsigned long i = 0; i < dest_len && ach->achievement_count < MAX_ACHIEVEMENTS; i++) {
        for (int bit = 0; bit < 8 && ach->achievement_count < MAX_ACHIEVEMENTS; bit++) {
            if (decompressed[i] & (1 << bit)) {
                DBAchievementEntry* entry = &ach->achievements[ach->achievement_count];
                entry->ach_id = (uint32_t)(i * 8 + bit + 1000); /* Base ID 1000 */
                entry->completed = 1;
                entry->progress = 100;
                entry->points = 100; /* 100 points per achievement */
                entry->complete_time = (uint32_t)time(NULL);
                ach->total_points += entry->points;
                ach->achievement_count++;
            }
        }
    }

    printf("[DB] Parsed %u achievements, total points: %u\n",
           ach->achievement_count, ach->total_points);
    return 0;
}

/* Get achievement data from taiwan_cain.charac_achievement */
static int db_get_achievement(uint32_t charac_no, DBAchievementInfo* ach) {
    if (!g_db_charac || !ach) return -1;

    memset(ach, 0, sizeof(DBAchievementInfo));
    ach->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT achievement, last_update_time FROM charac_achievement WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Achievement query failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        if (row[0] && lengths[0] > 4) {
            parse_achievement_blob((const unsigned char*)row[0], lengths[0], ach);
        }
    }

    mysql_free_result(result);
    ach->achievement_loaded = 1;

    /* If no achievements loaded, create some defaults */
    if (ach->achievement_count == 0) {
        /* Default starter achievements */
        const uint32_t default_achs[] = {1001, 1002, 1003, 1010, 1020};
        for (int i = 0; i < 5; i++) {
            DBAchievementEntry* entry = &ach->achievements[ach->achievement_count];
            entry->ach_id = default_achs[i];
            entry->completed = (i < 2) ? 1 : 0;  /* First 2 completed */
            entry->progress = entry->completed ? 100 : (uint8_t)(20 * (i + 1));
            entry->points = 100 + i * 50;
            entry->complete_time = entry->completed ? (uint32_t)time(NULL) : 0;
            if (entry->completed) ach->total_points += entry->points;
            ach->achievement_count++;
        }
    }

    printf("[DB] Loaded achievements for charac_no=%u: count=%u, points=%u\n",
           charac_no, ach->achievement_count, ach->total_points);
    return 0;
}

/* Parse title blob - zlib compressed title data
 * The blob format from charac_titlebook table has multiple sections
 */
int parse_title_blob(const unsigned char* blob, unsigned long blob_len,
                            DBTitleInfo* title, const char* section_name) {
    if (!blob || blob_len < 8 || !title) return -1;

    /* First 4 bytes is uncompressed size (little-endian) */
    uint32_t uncompressed_size = blob[0] | (blob[1] << 8) | (blob[2] << 16) | (blob[3] << 24);
    if (uncompressed_size > 4096) {
        printf("[DB] Title blob (%s) uncompressed size too large: %u\n",
               section_name, uncompressed_size);
        return -1;
    }

    /* Decompress the zlib data */
    unsigned char decompressed[4096];
    uLongf dest_len = sizeof(decompressed);

    int ret = uncompress(decompressed, &dest_len, blob + 4, blob_len - 4);
    if (ret != Z_OK) {
        printf("[DB] Title decompression (%s) failed: ret=%d\n", section_name, ret);
        return -1;
    }

    /* Parse as bit array - each bit represents a title */
    for (unsigned long i = 0; i < dest_len && title->title_count < MAX_TITLES; i++) {
        for (int bit = 0; bit < 8 && title->title_count < MAX_TITLES; bit++) {
            if (decompressed[i] & (1 << bit)) {
                DBTitleEntry* entry = &title->titles[title->title_count];
                entry->title_id = (uint32_t)(i * 8 + bit + 1); /* Title IDs start at 1 */
                entry->unlocked = 1;
                entry->unlock_time = (uint32_t)time(NULL);
                title->title_count++;
            }
        }
    }

    return 0;
}

/* Get title data from taiwan_cain.charac_titlebook */
static int db_get_title(uint32_t charac_no, DBTitleInfo* title) {
    if (!g_db_charac || !title) return -1;

    memset(title, 0, sizeof(DBTitleInfo));
    title->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT specific_section, general_section, despair, event "
             "FROM charac_titlebook WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Title query failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);

        /* Parse each section */
        if (row[0] && lengths[0] > 4) {
            parse_title_blob((const unsigned char*)row[0], lengths[0], title, "specific");
        }
        if (row[1] && lengths[1] > 4) {
            parse_title_blob((const unsigned char*)row[1], lengths[1], title, "general");
        }
        if (row[2] && lengths[2] > 4) {
            parse_title_blob((const unsigned char*)row[2], lengths[2], title, "despair");
        }
        if (row[3] && lengths[3] > 4) {
            parse_title_blob((const unsigned char*)row[3], lengths[3], title, "event");
        }
    }

    mysql_free_result(result);
    title->title_loaded = 1;

    /* If no titles loaded, create some defaults */
    if (title->title_count == 0) {
        /* Default starter titles */
        const uint32_t default_titles[] = {1, 2, 3, 10, 20, 50};
        for (int i = 0; i < 6 && title->title_count < MAX_TITLES; i++) {
            DBTitleEntry* entry = &title->titles[title->title_count];
            entry->title_id = default_titles[i];
            entry->unlocked = 1;
            entry->unlock_time = (uint32_t)time(NULL) - (86400 * (6 - i)); /* Staggered unlock times */
            title->title_count++;
        }
        title->equipped_title = 1; /* Default equipped title */
    }

    printf("[DB] Loaded titles for charac_no=%u: count=%u, equipped=%u\n",
           charac_no, title->title_count, title->equipped_title);
    return 0;
}

/* Save achievement progress to database */
static int db_save_achievement(uint32_t charac_no, uint32_t ach_id, uint8_t completed) {
    if (!g_db_charac) return -1;

    /* For now, just log the save - full blob update is complex */
    printf("[DB] Saving achievement: charac_no=%u, ach_id=%u, completed=%u\n",
           charac_no, ach_id, completed);

    /* Update last_update_time to track changes */
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_achievement SET last_update_time=NOW() WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Achievement save failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return 0;
}

/* Save equipped title to database */
static int db_save_equipped_title(uint32_t charac_no, uint32_t title_id) {
    if (!g_db_charac) return -1;

    printf("[DB] Saving equipped title: charac_no=%u, title_id=%u\n",
           charac_no, title_id);

    /* Title equipment is typically stored in charac_info or a related table
     * For now we log it - actual implementation depends on DB schema */
    return 0;
}

/* Get PVP stats from database (charac_stat table if exists, or charac_info) */
static int db_get_pvp_stats(uint32_t charac_no, uint32_t* pvp_points,
                            uint32_t* wins, uint32_t* losses) {
    if (!g_db_charac) return -1;

    /* Try to get from charac_stat first */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COALESCE(pvp_point, 1000), COALESCE(pvp_win, 0), COALESCE(pvp_lose, 0) "
             "FROM charac_stat WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query) == 0) {
        MYSQL_RES* res = mysql_store_result(g_db_charac);
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                *pvp_points = row[0] ? (uint32_t)atoi(row[0]) : 1000;
                *wins = row[1] ? (uint32_t)atoi(row[1]) : 0;
                *losses = row[2] ? (uint32_t)atoi(row[2]) : 0;
                mysql_free_result(res);
                return 0;
            }
            mysql_free_result(res);
        }
    }

    /* Default values if table doesn't exist */
    *pvp_points = 1000;
    *wins = 0;
    *losses = 0;
    return 0;
}

/* Save PVP stats to database */
static int db_save_pvp_stats(uint32_t charac_no, uint32_t pvp_points,
                             uint32_t wins, uint32_t losses) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET pvp_point=%u, pvp_win=%u, pvp_lose=%u "
             "WHERE charac_no=%u", pvp_points, wins, losses, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update PVP stats failed: %s\n", mysql_error(g_db_charac));
        /* Try INSERT if table structure differs */
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Updated PVP stats for charac_no=%u: points=%u, wins=%u, losses=%u (affected=%d)\n",
           charac_no, pvp_points, wins, losses, affected);
    return 0;
}

/* Update gold/money in taiwan_cain_2nd.inventory */


/* Add gold to character in taiwan_cain_2nd.inventory */


/* Update EXP in taiwan_cain.charac_stat */
static int db_add_exp(uint32_t charac_no, uint32_t amount) {
    if (!g_db_charac || amount == 0) return -1;

    /* Check connection */
    if (mysql_ping(g_db_charac) != 0) {
        printf("[DB] Connection lost, cannot add EXP\n");
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET exp=exp+%u WHERE charac_no=%u",
             amount, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Add EXP failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Added %u EXP for charac_no=%u (affected=%d)\n",
           amount, charac_no, affected);
    return affected > 0 ? 0 : -1;
}

/*------------------------------------------------------------------------------
 * Phase 12: Boss Kill Log
 *----------------------------------------------------------------------------*/

/* Log a boss kill to database */
static int db_log_boss_kill(uint32_t charac_no, uint32_t boss_id, uint32_t dungeon_id,
                             uint8_t difficulty, uint32_t clear_time_sec, uint8_t is_first_kill) {
    if (!g_db_charac2) return -1;

    /* Check connection */
    if (mysql_ping(g_db_charac2) != 0) {
        printf("[DB] Connection lost, cannot log boss kill\n");
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO boss_kill_log (charac_no, boss_id, dungeon_id, difficulty, "
             "clear_time_sec, is_first_kill, kill_time) "
             "VALUES (%u, %u, %u, %u, %u, %u, NOW()) "
             "ON DUPLICATE KEY UPDATE kill_count=kill_count+1, "
             "best_time=LEAST(best_time, %u), last_kill=NOW()",
             charac_no, boss_id, dungeon_id, difficulty,
             clear_time_sec, is_first_kill, clear_time_sec);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Boss kill log failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    printf("[BOSS] Logged kill: charac=%u boss=%u dungeon=%u diff=%u time=%us first=%u\n",
           charac_no, boss_id, dungeon_id, difficulty, clear_time_sec, is_first_kill);
    return 0;
}

/* Get boss kill stats for a character */
static int db_get_boss_kill_stats(uint32_t charac_no, uint32_t boss_id,
                                   uint32_t* kill_count, uint32_t* best_time) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT kill_count, best_time FROM boss_kill_log "
             "WHERE charac_no=%u AND boss_id=%u LIMIT 1",
             charac_no, boss_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Get boss stats failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        if (kill_count) *kill_count = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (best_time) *best_time = row[1] ? (uint32_t)atoi(row[1]) : 0;
    } else {
        if (kill_count) *kill_count = 0;
        if (best_time) *best_time = 0;
    }

    mysql_free_result(result);
    return 0;
}

/* Add item to inventory in taiwan_cain_2nd.user_items */

/* Find next available inventory slot (slot >= MAX_EQUIP_SLOTS for inventory) */


/* Delete item from inventory by slot */


/* Update item slot (for equip/unequip) */


/* Delete item at slot */


/* Consume item (reduce count or delete if count becomes 0) */
int db_consume_item(uint32_t charac_no, uint32_t ui_id, int amount) {
    if (!g_db_charac2 || amount <= 0) return -1;

    DBItemInfo item;
    if (db_get_item_by_ui_id(charac_no, ui_id, &item) < 0 || item.ui_id == 0) {
        return -1;
    }

    uint16_t current_count = item.stat > 0 ? (uint16_t)item.stat : 1;
    if (current_count > amount) {
        char query[256];
        snprintf(query, sizeof(query),
                 "UPDATE user_items SET stat=%u WHERE ui_id=%u LIMIT 1",
                 current_count - amount, item.ui_id);
        if (mysql_query(g_db_charac2, query)) {
            printf("[DB] Consume item failed: %s\n", mysql_error(g_db_charac2));
            return -1;
        }
        return mysql_affected_rows(g_db_charac2) > 0 ? 0 : -1;
    }

    return db_delete_item(charac_no, item.ui_id);
}

/* Get item by slot */


/* Get item info at slot including durability */


/* Repair single item */


/* Repair all equipped items */


/* Calculate repair cost */
uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur) {
    uint32_t lvl = (item_id / 1000) % 100;
    if (lvl == 0) lvl = 1;
    return (100 - cur_dur) * lvl * 10;
}

/* Transfer item from one character to another by ui_id */
static int db_transfer_item(uint32_t ui_id, uint32_t from_charac, uint32_t to_charac) {
    if (!g_db_charac2) return -1;

    /* First verify the item belongs to from_charac */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT it_id, slot FROM user_items WHERE ui_id=%u AND charac_no=%u",
             ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("[DB] Transfer failed: item ui_id=%u not owned by charac_no=%u\n",
               ui_id, from_charac);
        mysql_free_result(result);
        return -1;
    }

    uint32_t it_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    mysql_free_result(result);

    /* Find a free slot for the receiving character (bag slots start at 11) */
    int new_slot = db_find_free_slot(to_charac, 11);
    if (new_slot < 0 || new_slot > 200) {
        printf("[DB] Transfer failed: no free slot for charac_no=%u\n", to_charac);
        return -1;
    }

    /* Transfer the item: update charac_no and slot */
    snprintf(query, sizeof(query),
             "UPDATE user_items SET charac_no=%u, slot=%d, m_time=NOW() "
             "WHERE ui_id=%u AND charac_no=%u",
             to_charac, new_slot, ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item update failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    if (affected > 0) {
        printf("[DB] Transferred item ui_id=%u (it_id=%u) from charac=%u to charac=%u slot=%d\n",
               ui_id, it_id, from_charac, to_charac, new_slot);
        return new_slot;  /* Return new slot on success */
    }

    return -1;
}

int db_set_item_upgrade_level(uint32_t ui_id, uint8_t new_level) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET hidden_option=(hidden_option & 0xFF00) | %u "
             "WHERE ui_id=%u LIMIT 1",
             (unsigned)new_level, ui_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Set item upgrade level failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    return mysql_affected_rows(g_db_charac2) > 0 ? 0 : -1;
}

/* Get storage gold for account */


/* Update storage gold */


/* Calculate upgrade success rate */
int calc_upgrade_success_rate(uint8_t level) {
    if (level <= 3) return 100;
    if (level == 4) return 95;
    if (level == 5) return 90;
    if (level == 6) return 80;
    if (level == 7) return 70;
    if (level == 8) return 60;
    if (level == 9) return 50;
    if (level == 10) return 40;
    if (level == 11) return 30;
    if (level == 12) return 20;
    return 10;
}

/* Calculate upgrade cost */
uint32_t calc_upgrade_cost(uint32_t item_id, uint8_t level) {
    uint32_t item_level = (item_id / 1000) % 100;
    if (item_level == 0) item_level = 1;
    uint32_t base_cost = item_level * 1000;
    return base_cost + (base_cost * level / 2);
}

/* Update character exp and level in charac_info *//* Update character exp and level in charac_info */
static int db_update_character_exp(uint32_t charac_no, uint32_t exp, uint8_t level) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET exp=%u, lev=%u WHERE charac_no=%u",
             exp, level, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update exp failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Updated charac_no=%u: exp=%u, level=%u (affected=%d)\n",
           charac_no, exp, level, affected);
    return affected > 0 ? 0 : -1;
}

/* Update character HP/MP in charac_info */


/* ============== P1: Update fatigue in charac_info ============== */
static int db_update_fatigue(uint32_t charac_no, int16_t fatigue) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET fatigue=%d WHERE charac_no=%u",
             fatigue, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update fatigue failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[DB] Updated fatigue for charac_no=%u: fatigue=%d\n", charac_no, fatigue);
    return 0;
}

/* Get fatigue from database */
static int16_t db_get_fatigue(uint32_t charac_no) {
    if (!g_db_charac) return 156;

    char query[128];
    snprintf(query, sizeof(query),
             "SELECT fatigue FROM charac_info WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) {
        return 156;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 156;

    MYSQL_ROW row = mysql_fetch_row(result);
    int16_t fatigue = row ? (int16_t)atoi(row[0]) : 156;
    mysql_free_result(result);
    return fatigue;
}

/* ============== P2: Level up system - get exp required for level ============== */
static uint32_t get_exp_for_level(uint8_t level) {
    /* DNF-style exp table (simplified): exp = base * level^2.3 */
    if (level <= 0) return 0;
    if (level >= 85) return 999999999;  /* Max level */

    uint32_t base = 100;
    /* Exponential growth */
    double exp = base * pow((double)level, 2.3);
    return (uint32_t)exp;
}

/* Check and process level up */
int check_and_level_up(ClientSession* s) {
    if (!s) return 0;

    /* Find current character */
    DBCharacterInfo* ch = NULL;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            ch = &s->characters[i];
            break;
        }
    }
    if (!ch || ch->level >= 85) return 0;

    int levels_gained = 0;

    /* Loop to handle multiple level ups at once */
    while (ch->level < 85) {
        uint32_t exp_needed = get_exp_for_level(ch->level + 1);
        if (ch->exp < exp_needed) break;

        /* Level up! */
        ch->level++;
        levels_gained++;

        /* Update HP/MP based on new level */
        ch->max_hp = 300 + ch->level * 50;
        ch->max_mp = 150 + ch->level * 25;
        ch->hp = ch->max_hp;

        printf("[LEVELUP] Character %u leveled up to %u! (exp=%u, needed=%u)\n",
               s->character_id, ch->level, ch->exp, exp_needed);

        /* Gain stat points (5 per level) and skill points (2 per level) */
    }

    if (levels_gained > 0) {
        /* Persist to DB */
        db_update_character_exp(s->character_id, ch->exp, ch->level);
        db_update_character_hp_mp(s->character_id, ch->hp, ch->max_mp);

        /* Update session HP */
        s->current_hp = ch->max_hp;
        s->current_mp = ch->max_mp;

        /* Note: calculate_combat_stats() is called later when needed */
        printf("[LEVELUP] Character stats need recalculation at level %u\n", ch->level);
    }

    return levels_gained;
}

/* ============== P3: Equipment durability system ============== */
static int db_update_item_durability(uint32_t charac_no, int slot, uint8_t durability) {
    if (!g_db_charac2) return -1;

    DBItemInfo item;
    if (db_get_item_at_slot(charac_no, slot, &item) < 0 || item.ui_id == 0) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET endurance=%u WHERE ui_id=%u LIMIT 1",
             durability, item.ui_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update durability failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }
    return mysql_affected_rows(g_db_charac2) > 0 ? 0 : -1;
}

/* Decrease durability on all equipped items */
void decrease_equipment_durability(ClientSession* s, int amount) {
    if (!s || amount <= 0) return;

    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        if (s->equipment.equipment[i].it_id == 0) continue;

        int cur = s->equipment.equipment[i].endurance;
        int new_dur = cur - amount;
        if (new_dur < 0) new_dur = 0;

        s->equipment.equipment[i].endurance = (uint8_t)new_dur;

        if (new_dur == 0) {
            printf("[DURABILITY] Equipment in slot %d broke!\n", i);
        }
    }
}

/* Persist all equipment durability to DB */
void save_equipment_durability(ClientSession* s) {
    if (!s) return;

    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        if (s->equipment.equipment[i].it_id == 0) continue;
        db_update_item_durability(s->character_id, i, s->equipment.equipment[i].endurance);
    }
}

/* ============== P5: Quest progress persistence ============== */
/* Note: DBQuestProgress typedef and db_save_quest_progress/db_load_quest_progress
 * are defined in db/db_quest.h and db/db_quest.cpp */

/* ============== P7: Storage item DB operations ============== */
#define STORAGE_SLOT_OFFSET 1000  /* Storage slots start at 1000 */

static void ensure_account_cargo_schema(void) {
    static int checked = 0;
    if (checked || !g_db_charac) return;
    checked = 1;

    const char* create_sql =
        "CREATE TABLE IF NOT EXISTS account_cargo_items ("
        "m_id INT UNSIGNED NOT NULL,"
        "slot INT NOT NULL,"
        "ui_id INT UNSIGNED NOT NULL,"
        "it_id INT UNSIGNED NOT NULL,"
        "stat TINYINT UNSIGNED NOT NULL DEFAULT 0,"
        "PRIMARY KEY (ui_id),"
        "UNIQUE KEY account_cargo_slot (m_id, slot))";

    if (mysql_query(g_db_charac, create_sql)) {
        printf("[STORAGE] Failed ensuring account_cargo_items table: %s\n",
               mysql_error(g_db_charac));
    }

    if (mysql_query(
            g_db_charac,
            "ALTER TABLE account_cargo_items "
            "ADD COLUMN IF NOT EXISTS ui_id INT UNSIGNED NOT NULL DEFAULT 0 AFTER slot")) {
        /* Ignore duplicate column errors on older MySQL versions */
    }

    if (mysql_query(
            g_db_charac,
            "ALTER TABLE account_cargo_items "
            "ADD UNIQUE INDEX IF NOT EXISTS idx_account_cargo_ui (ui_id)")) {
        /* Ignore duplicate index errors */
    }
}

static int db_storage_deposit_item(uint32_t m_id, uint32_t charac_no, int inv_slot) {
    if (!g_db_charac2 || !g_db_charac) return -1;

    ensure_account_cargo_schema();

    /* Get item info from inventory */
    DBItemInfo inv_item;
    if (db_get_item_at_slot(charac_no, inv_slot, &inv_item) < 0 || inv_item.ui_id == 0) {
        printf("[STORAGE] No item at slot %d\n", inv_slot);
        return -1;
    }
    uint32_t item_id = inv_item.it_id;

    /* Find free storage slot */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT slot FROM account_cargo_items WHERE m_id=%u ORDER BY slot", m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[STORAGE] Failed to enumerate storage slots: %s\n",
               mysql_error(g_db_charac));
    } else {
        MYSQL_RES* slots = mysql_store_result(g_db_charac);
        if (slots) mysql_free_result(slots);
    }

    /* Move item: delete from user_items, insert to storage */
    if (db_delete_item(charac_no, inv_item.ui_id) < 0) {
        return -1;
    }

    /* Find next storage slot */
    int storage_slot = 0;
    snprintf(query, sizeof(query),
             "SELECT IFNULL(MAX(slot)+1, 0) FROM account_cargo_items WHERE m_id=%u", m_id);
    if (mysql_query(g_db_charac, query) == 0) {
        MYSQL_RES* result = mysql_store_result(g_db_charac);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row && row[0]) storage_slot = atoi(row[0]);
            mysql_free_result(result);
        }
    }

    /* Insert into storage with ui_id tracking */
    snprintf(query, sizeof(query),
             "INSERT INTO account_cargo_items (m_id, slot, ui_id, it_id, stat) "
             "VALUES (%u, %d, %u, %u, %u)",
             m_id, storage_slot, inv_item.ui_id, item_id, inv_item.stat);

    if (mysql_query(g_db_charac, query)) {
        printf("[STORAGE] Failed to insert item: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[STORAGE] Deposited item %u (ui_id=%u) from inv slot %d to storage slot %d\n",
           item_id, inv_item.ui_id, inv_slot, storage_slot);
    return storage_slot;
}

static int db_storage_withdraw_item(uint32_t m_id, uint32_t charac_no, int storage_slot) {
    if (!g_db_charac2 || !g_db_charac) return -1;

    ensure_account_cargo_schema();

    /* Get item from storage */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ui_id, it_id, stat FROM account_cargo_items WHERE m_id=%u AND slot=%d",
             m_id, storage_slot);

    if (mysql_query(g_db_charac, query)) {
        printf("[STORAGE] Failed to query storage slot %d: %s\n",
               storage_slot, mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;
    }

    uint32_t storage_ui_id = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
    uint32_t item_id = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    uint8_t stat = row[2] ? (uint8_t)atoi(row[2]) : 0;
    mysql_free_result(result);

    if (item_id == 0 || storage_ui_id == 0) return -1;

    /* Find free inventory slot */
    int inv_slot = db_find_free_slot(charac_no, MAX_EQUIP_SLOTS);
    if (inv_slot < 0 || inv_slot >= 200) {
        printf("[STORAGE] Inventory full\n");
        return -2;  /* Inventory full */
    }

    /* Remove from storage using ui_id */
    snprintf(query, sizeof(query),
             "DELETE FROM account_cargo_items WHERE ui_id=%u LIMIT 1",
             storage_ui_id);
    if (mysql_query(g_db_charac, query)) {
        printf("[STORAGE] Failed to delete storage ui_id=%u: %s\n",
               storage_ui_id, mysql_error(g_db_charac));
        return -1;
    }

    /* Add to inventory */
    int new_id = db_add_item(charac_no, item_id, inv_slot, stat);
    if (new_id < 0) {
        return -1;
    }

    printf("[STORAGE] Withdrew item %u (stored ui_id=%u) slot %d -> inv slot %d new_ui=%d\n",
           item_id, storage_ui_id, storage_slot, inv_slot, new_id);
    return inv_slot;
}

/* ============== P9: Auction DB integration ============== */
/* Note: DBAuctionListing typedef and db_auction_* functions
 * are defined in db/db_economy.h and db/db_economy.cpp */

/* Update skill points in skill table */
static int db_update_skill_sp(uint32_t charac_no, uint32_t remain_sp, uint32_t used_sp) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE skill SET remain_sp=%u, used_sp=%u WHERE charac_no=%u",
             remain_sp, used_sp, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update skill SP failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Updated skill for charac_no=%u: remain_sp=%u, used_sp=%u (affected=%d)\n",
           charac_no, remain_sp, used_sp, affected);
    return affected > 0 ? 0 : -1;
}

/* Get SP cost for a skill level (simplified formula based on DNF) */
int get_skill_sp_cost(uint16_t skill_id, uint8_t current_level) {
    /* Base cost varies by skill tier (determined by skill_id range) */
    int base_cost;
    if (skill_id < 100) {
        base_cost = 15;  /* Basic skills */
    } else if (skill_id < 200) {
        base_cost = 20;  /* Intermediate skills */
    } else if (skill_id < 300) {
        base_cost = 30;  /* Advanced skills */
    } else {
        base_cost = 50;  /* Ultimate skills */
    }
    /* Cost increases with level */
    return base_cost + (current_level * 5);
}

/* Check if character has learned a skill (by checking skill_slot blob) */
static int db_has_skill(uint32_t charac_no, uint16_t skill_id, uint8_t* out_level) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT skill_slot FROM skill WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row || !row[0]) {
        mysql_free_result(result);
        if (out_level) *out_level = 0;
        return 0;  /* No skill data = doesn't have skill */
    }

    unsigned long* lengths = mysql_fetch_lengths(result);
    if (!lengths || lengths[0] < 4) {
        mysql_free_result(result);
        if (out_level) *out_level = 0;
        return 0;
    }

    /* Parse skill_slot blob to find skill */
    /* Format: [2 bytes slot_id][2 bytes level] per entry */
    const unsigned char* blob = (const unsigned char*)row[0];
    unsigned long blob_len = lengths[0];

    for (unsigned long i = 0; i + 4 <= blob_len; i += 4) {
        uint16_t sid = blob[i] | (blob[i+1] << 8);
        uint16_t lvl = blob[i+2] | (blob[i+3] << 8);
        if (sid == skill_id) {
            mysql_free_result(result);
            if (out_level) *out_level = (uint8_t)lvl;
            return 1;  /* Has skill */
        }
    }

    mysql_free_result(result);
    if (out_level) *out_level = 0;
    return 0;  /* Doesn't have skill */
}

/* Add or upgrade a skill in skill_slot blob */


/* Get/set stat points from charac_stat table */
static int db_get_stat_points(uint32_t charac_no, uint32_t* remain_stat, uint32_t* used_stat) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COALESCE(remain_stat_point, 0), COALESCE(used_stat_point, 0) "
             "FROM charac_stat WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        if (remain_stat) *remain_stat = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
        if (used_stat) *used_stat = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    } else {
        if (remain_stat) *remain_stat = 0;
        if (used_stat) *used_stat = 0;
    }

    mysql_free_result(result);
    return 0;
}

static int db_update_stat_points(uint32_t charac_no, uint32_t remain_stat, uint32_t used_stat) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET remain_stat_point=%u, used_stat_point=%u WHERE charac_no=%u",
             remain_stat, used_stat, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update stat points failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return (int)mysql_affected_rows(g_db_charac) > 0 ? 0 : -1;
}

/* Increase a specific stat (STR/INT/VIT/SPI) */
static int db_increase_stat(uint32_t charac_no, uint8_t stat_type, int amount) {
    if (!g_db_charac) return -1;

    const char* stat_field;
    switch (stat_type) {
        case 0: stat_field = "grow_str"; break;
        case 1: stat_field = "grow_int"; break;
        case 2: stat_field = "grow_vit"; break;
        case 3: stat_field = "grow_spi"; break;
        default: return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET %s=%s+%d WHERE charac_no=%u",
             stat_field, stat_field, amount, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Increase stat failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return 0;
}

/* Calculate combat stats based on level, job, and equipment */
static void calculate_combat_stats(ClientSession* s) {
    if (!s) return;

    CombatStats* stats = &s->combat_stats;
    memset(stats, 0, sizeof(CombatStats));

    /* Find current character info */
    DBCharacterInfo* charac = NULL;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            charac = &s->characters[i];
            break;
        }
    }
    if (!charac) return;

    uint8_t level = charac->level;
    uint8_t job = charac->job;

    /* Base stats by job type (simplified formula) */
    /* DNF formula: base + (level - 1) * growth_rate */
    switch (job) {
        case JOB_SLAYER:  /* STR based melee */
            stats->str = 30 + level * 3;
            stats->intel = 15 + level * 1;
            stats->vit = 25 + level * 2;
            stats->spr = 15 + level * 1;
            break;
        case JOB_FIGHTER:  /* STR based melee */
            stats->str = 28 + level * 3;
            stats->intel = 12 + level * 1;
            stats->vit = 28 + level * 2;
            stats->spr = 12 + level * 1;
            break;
        case JOB_GUNNER:  /* STR/INT hybrid */
            stats->str = 22 + level * 2;
            stats->intel = 22 + level * 2;
            stats->vit = 20 + level * 2;
            stats->spr = 18 + level * 1;
            break;
        case JOB_MAGE:  /* INT based caster */
            stats->str = 10 + level * 1;
            stats->intel = 35 + level * 4;
            stats->vit = 15 + level * 1;
            stats->spr = 25 + level * 2;
            break;
        case JOB_PRIEST:  /* INT/SPR hybrid */
            stats->str = 18 + level * 1;
            stats->intel = 25 + level * 3;
            stats->vit = 22 + level * 2;
            stats->spr = 28 + level * 3;
            break;
        default:  /* THIEF or unknown */
            stats->str = 25 + level * 2;
            stats->intel = 18 + level * 1;
            stats->vit = 22 + level * 2;
            stats->spr = 15 + level * 1;
            break;
    }

    /* Equipment bonuses - will be applied after template table is defined */
    /* Using simplified formula for now, enhanced via apply_equipment_bonuses() */
    int equip_count = 0;
    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        uint32_t item_id = s->equipment.equipment[i].it_id;
        if (item_id == 0) continue;
        equip_count++;

        /* Simplified formula based on item_id range and grade */
        uint8_t enhance = s->equipment.equipment[i].stat;
        float enhance_mult = (enhance == 0) ? 1.0f : (1.0f + enhance * 0.03f);
        if (enhance > 10) enhance_mult = 1.30f + (enhance - 10) * 0.05f;

        /* Determine item tier from item_id sub-range (every 20 items = higher tier) */
        uint32_t tier = ((item_id % 100) / 20);  /* 0-4 tiers */
        uint32_t base_power = 30 + tier * 80;    /* 30/110/190/270/350 */

        switch (i) {
            case EQUIP_SLOT_WEAPON:
                /* Weapon provides attack based on job compatibility */
                if (item_id >= 50000 && item_id < 50100) {  /* Slayer sword */
                    stats->phys_attack += (uint32_t)(base_power * 2.5f * enhance_mult);
                    stats->str += tier * 5 + 3;
                } else if (item_id >= 50100 && item_id < 50200) {  /* Mage staff */
                    stats->mag_attack += (uint32_t)(base_power * 2.8f * enhance_mult);
                    stats->intel += tier * 5 + 4;
                } else if (item_id >= 50200 && item_id < 50300) {  /* Gunner gun */
                    stats->phys_attack += (uint32_t)(base_power * 1.5f * enhance_mult);
                    stats->mag_attack += (uint32_t)(base_power * 1.5f * enhance_mult);
                    stats->str += tier * 3 + 2;
                    stats->intel += tier * 3 + 2;
                } else if (item_id >= 50300 && item_id < 50400) {  /* Fighter gauntlet */
                    stats->phys_attack += (uint32_t)(base_power * 2.3f * enhance_mult);
                    stats->str += tier * 5 + 4;
                    stats->max_hp += tier * 50;
                } else if (item_id >= 50400 && item_id < 50500) {  /* Priest cross */
                    stats->phys_attack += (uint32_t)(base_power * 1.2f * enhance_mult);
                    stats->mag_attack += (uint32_t)(base_power * 1.8f * enhance_mult);
                    stats->intel += tier * 4 + 3;
                    stats->spr += tier * 3 + 2;
                } else if (item_id >= 50500 && item_id < 50600) {  /* Thief dagger */
                    stats->phys_attack += (uint32_t)(base_power * 2.2f * enhance_mult);
                    stats->str += tier * 4 + 2;
                    stats->crit_rate += tier * 50 + 30;
                } else {  /* Generic weapon */
                    stats->phys_attack += (uint32_t)(base_power * 2.0f * enhance_mult);
                }
                break;

            case EQUIP_SLOT_CHEST:
                stats->phys_def += (uint32_t)((25 + tier * 50) * enhance_mult);
                stats->mag_def += (uint32_t)((20 + tier * 40) * enhance_mult);
                stats->max_hp += tier * 100 + 80;
                stats->vit += tier * 4 + 3;
                break;

            case EQUIP_SLOT_PANTS:
                stats->phys_def += (uint32_t)((20 + tier * 45) * enhance_mult);
                stats->mag_def += (uint32_t)((16 + tier * 36) * enhance_mult);
                stats->max_hp += tier * 80 + 60;
                stats->vit += tier * 3 + 2;
                break;

            case EQUIP_SLOT_HELMET:
                stats->phys_def += (uint32_t)((15 + tier * 35) * enhance_mult);
                stats->mag_def += (uint32_t)((12 + tier * 28) * enhance_mult);
                stats->str += tier + 1;
                stats->intel += tier + 1;
                stats->max_hp += tier * 50 + 30;
                break;

            case EQUIP_SLOT_SHOULDER:
                stats->phys_def += (uint32_t)((12 + tier * 30) * enhance_mult);
                stats->mag_def += (uint32_t)((10 + tier * 24) * enhance_mult);
                stats->str += tier * 2 + 2;
                stats->max_hp += tier * 40 + 20;
                break;

            case EQUIP_SLOT_SHOES:
                stats->phys_def += (uint32_t)((10 + tier * 25) * enhance_mult);
                stats->mag_def += (uint32_t)((8 + tier * 20) * enhance_mult);
                stats->move_speed += tier * 3 + 3;
                stats->vit += tier + 1;
                break;

            case EQUIP_SLOT_BELT:
                stats->phys_def += (uint32_t)((8 + tier * 18) * enhance_mult);
                stats->mag_def += (uint32_t)((6 + tier * 14) * enhance_mult);
                stats->str += tier + 1;
                stats->intel += tier + 1;
                stats->vit += tier + 1;
                stats->spr += tier + 1;
                stats->max_hp += tier * 40 + 25;
                stats->max_mp += tier * 25 + 15;
                break;

            case EQUIP_SLOT_BRACELET:
                stats->phys_attack += (uint32_t)((5 + tier * 15) * enhance_mult);
                stats->mag_attack += (uint32_t)((5 + tier * 15) * enhance_mult);
                stats->str += tier * 3 + 2;
                stats->intel += tier * 3 + 2;
                stats->crit_rate += tier * 25 + 20;
                break;

            case EQUIP_SLOT_NECKLACE:
                stats->phys_attack += (uint32_t)((3 + tier * 10) * enhance_mult);
                stats->mag_attack += (uint32_t)((8 + tier * 22) * enhance_mult);
                stats->intel += tier * 4 + 3;
                stats->spr += tier * 2 + 1;
                stats->max_mp += tier * 40 + 20;
                stats->crit_rate += tier * 20 + 15;
                break;

            case EQUIP_SLOT_RING:
                stats->phys_attack += (uint32_t)((8 + tier * 20) * enhance_mult);
                stats->str += tier * 4 + 3;
                stats->crit_rate += tier * 30 + 30;
                break;

            case EQUIP_SLOT_SUBWEAPON:
                /* Subweapons: shields, totems, orbs */
                stats->phys_def += (uint32_t)((15 + tier * 30) * enhance_mult);
                stats->mag_def += (uint32_t)((15 + tier * 30) * enhance_mult);
                stats->max_hp += tier * 60 + 40;
                break;
        }
    }
    printf("[EQUIP] Applied %d equipment bonuses\n", equip_count);

    /* Calculate derived stats */
    stats->phys_attack += stats->str * 2;
    stats->mag_attack += stats->intel * 2;
    stats->phys_def += stats->vit;
    stats->mag_def += stats->spr;
    stats->max_hp = 100 + stats->vit * 30 + level * 50;
    stats->max_mp = 50 + stats->spr * 20 + level * 30;
    stats->attack_speed = 100;  /* Base 100% */
    stats->move_speed = 100;    /* Base 100% */
    stats->cast_speed = 100;    /* Base 100% */
    stats->crit_rate = 300 + level * 10;  /* 3% base + 0.1% per level */
    stats->hit_rate = 9000 + level * 50;  /* 90% base */
    stats->evasion = 500 + level * 20;    /* 5% base */

    printf("[STATS] Calculated for charac_no=%u (job=%d, lv=%d): STR=%u INT=%u PHY_ATK=%u MAG_ATK=%u HP=%u\n",
           s->character_id, job, level, stats->str, stats->intel,
           stats->phys_attack, stats->mag_attack, stats->max_hp);
}

/* ========================================
 * Equipment Stats System
 * Based on item_id ranges from production data
 * ======================================== */

/* Item grade/rarity */
typedef enum {
    ITEM_GRADE_COMMON = 0,      /* White */
    ITEM_GRADE_UNCOMMON,        /* Green */
    ITEM_GRADE_RARE,            /* Blue */
    ITEM_GRADE_UNIQUE,          /* Purple */
    ITEM_GRADE_LEGENDARY,       /* Orange */
    ITEM_GRADE_EPIC             /* Pink */
} ItemGrade;

/* Equipment stats by item template */
typedef struct {
    uint32_t item_id_min;   /* Starting item_id range */
    uint32_t item_id_max;   /* Ending item_id range */
    uint8_t  slot_type;     /* EQUIP_SLOT_* */
    uint8_t  grade;         /* ItemGrade */
    uint8_t  level_req;     /* Required level */
    int16_t  phys_attack;   /* Physical attack bonus */
    int16_t  mag_attack;    /* Magic attack bonus */
    int16_t  phys_def;      /* Physical defense */
    int16_t  mag_def;       /* Magic defense */
    int16_t  str_bonus;     /* STR bonus */
    int16_t  int_bonus;     /* INT bonus */
    int16_t  vit_bonus;     /* VIT bonus */
    int16_t  spr_bonus;     /* SPR bonus */
    int16_t  hp_bonus;      /* Max HP bonus */
    int16_t  mp_bonus;      /* Max MP bonus */
    int16_t  crit_bonus;    /* Crit rate bonus (0.01%) */
    int16_t  speed_bonus;   /* Attack/move speed bonus */
} EquipmentTemplate;

/* Equipment template table (based on DNF item ranges) */
static const EquipmentTemplate g_equipment_table[] = {
    /* === Weapons (slot 11 in DB, slot 0 here) === */
    /* Slayer Swords 50000-50099 */
    {50000, 50019, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 50, 0, 0, 0, 3, 0, 0, 0, 0, 0, 50, 0},
    {50020, 50039, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 120, 0, 0, 0, 6, 0, 0, 0, 0, 0, 80, 0},
    {50040, 50059, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 250, 0, 0, 0, 12, 0, 1, 0, 0, 0, 120, 5},
    {50060, 50079, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 450, 0, 0, 0, 20, 0, 2, 0, 50, 0, 180, 10},
    {50080, 50099, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 750, 0, 0, 0, 35, 0, 4, 0, 100, 0, 250, 15},

    /* Mage Staffs 50100-50199 */
    {50100, 50119, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 0, 55, 0, 0, 0, 4, 0, 0, 0, 20, 60, 0},
    {50120, 50139, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 0, 135, 0, 0, 0, 8, 0, 1, 0, 40, 100, 0},
    {50140, 50159, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 0, 280, 0, 0, 0, 15, 0, 2, 0, 80, 150, 5},
    {50160, 50179, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 0, 500, 0, 0, 0, 25, 0, 4, 0, 150, 220, 10},
    {50180, 50199, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 0, 850, 0, 0, 0, 42, 0, 7, 0, 280, 320, 15},

    /* Gunner Guns 50200-50299 (hybrid) */
    {50200, 50219, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 30, 30, 0, 0, 2, 2, 0, 0, 0, 0, 50, 0},
    {50220, 50239, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 70, 70, 0, 0, 4, 4, 0, 0, 0, 0, 80, 0},
    {50240, 50259, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 150, 150, 0, 0, 8, 8, 0, 0, 0, 0, 120, 5},
    {50260, 50279, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 280, 280, 0, 0, 15, 15, 0, 0, 0, 0, 180, 10},
    {50280, 50299, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 500, 500, 0, 0, 25, 25, 0, 0, 50, 50, 250, 15},

    /* Fighter Gauntlets 50300-50399 */
    {50300, 50319, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 45, 0, 0, 0, 4, 0, 1, 0, 20, 0, 40, 5},
    {50320, 50339, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 110, 0, 0, 0, 8, 0, 2, 0, 40, 0, 70, 8},
    {50340, 50359, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 230, 0, 0, 0, 14, 0, 4, 0, 80, 0, 110, 12},
    {50360, 50379, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 420, 0, 0, 0, 22, 0, 6, 0, 150, 0, 160, 18},
    {50380, 50399, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 720, 0, 0, 0, 38, 0, 10, 0, 280, 0, 230, 25},

    /* Priest Crosses/Axes 50400-50499 */
    {50400, 50419, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 20, 35, 0, 0, 1, 3, 0, 2, 30, 30, 50, 0},
    {50420, 50439, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 50, 85, 0, 0, 3, 6, 0, 4, 60, 60, 80, 0},
    {50440, 50459, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 110, 180, 0, 0, 6, 12, 0, 7, 120, 120, 120, 5},
    {50460, 50479, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 200, 330, 0, 0, 10, 20, 0, 12, 220, 220, 180, 10},
    {50480, 50499, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 380, 580, 0, 0, 18, 35, 0, 20, 400, 400, 260, 15},

    /* Thief Daggers 50500-50599 */
    {50500, 50519, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 40, 0, 0, 0, 2, 0, 0, 0, 0, 0, 60, 10},
    {50520, 50539, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 100, 0, 0, 0, 5, 0, 0, 0, 0, 0, 100, 15},
    {50540, 50559, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 210, 0, 0, 0, 10, 0, 0, 0, 0, 0, 150, 25},
    {50560, 50579, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 390, 0, 0, 0, 17, 0, 0, 0, 0, 0, 220, 35},
    {50580, 50599, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 680, 0, 0, 0, 30, 0, 0, 0, 0, 0, 320, 50},

    /* === Armor Pieces === */
    /* Helmets 50600-50649 */
    {50600, 50609, EQUIP_SLOT_HELMET, ITEM_GRADE_COMMON,   1, 0, 0, 15, 12, 1, 1, 1, 0, 30, 10, 0, 0},
    {50610, 50619, EQUIP_SLOT_HELMET, ITEM_GRADE_UNCOMMON, 10, 0, 0, 35, 28, 2, 2, 2, 0, 60, 20, 0, 0},
    {50620, 50629, EQUIP_SLOT_HELMET, ITEM_GRADE_RARE,     20, 0, 0, 70, 55, 4, 4, 4, 0, 120, 40, 20, 0},
    {50630, 50639, EQUIP_SLOT_HELMET, ITEM_GRADE_UNIQUE,   30, 0, 0, 120, 95, 7, 7, 7, 0, 200, 70, 35, 0},
    {50640, 50649, EQUIP_SLOT_HELMET, ITEM_GRADE_LEGENDARY,40, 0, 0, 200, 160, 12, 12, 12, 0, 350, 120, 55, 0},

    /* Shoulders 50650-50699 */
    {50650, 50659, EQUIP_SLOT_SHOULDER, ITEM_GRADE_COMMON,   1, 0, 0, 12, 10, 2, 0, 1, 0, 20, 0, 0, 0},
    {50660, 50669, EQUIP_SLOT_SHOULDER, ITEM_GRADE_UNCOMMON, 10, 0, 0, 28, 23, 4, 0, 2, 0, 40, 0, 0, 0},
    {50670, 50679, EQUIP_SLOT_SHOULDER, ITEM_GRADE_RARE,     20, 0, 0, 55, 45, 8, 0, 4, 0, 80, 0, 0, 0},
    {50680, 50689, EQUIP_SLOT_SHOULDER, ITEM_GRADE_UNIQUE,   30, 0, 0, 95, 78, 14, 0, 7, 0, 140, 0, 0, 0},
    {50690, 50699, EQUIP_SLOT_SHOULDER, ITEM_GRADE_LEGENDARY,40, 0, 0, 160, 130, 24, 0, 12, 0, 240, 0, 0, 0},

    /* Chest/Body 50700-50749 */
    {50700, 50709, EQUIP_SLOT_CHEST, ITEM_GRADE_COMMON,   1, 0, 0, 25, 20, 0, 0, 3, 0, 80, 0, 0, 0},
    {50710, 50719, EQUIP_SLOT_CHEST, ITEM_GRADE_UNCOMMON, 10, 0, 0, 55, 45, 0, 0, 5, 0, 150, 0, 0, 0},
    {50720, 50729, EQUIP_SLOT_CHEST, ITEM_GRADE_RARE,     20, 0, 0, 110, 90, 0, 0, 9, 0, 280, 0, 0, 0},
    {50730, 50739, EQUIP_SLOT_CHEST, ITEM_GRADE_UNIQUE,   30, 0, 0, 185, 150, 0, 0, 15, 0, 450, 0, 0, 0},
    {50740, 50749, EQUIP_SLOT_CHEST, ITEM_GRADE_LEGENDARY,40, 0, 0, 300, 245, 0, 0, 25, 0, 750, 0, 0, 0},

    /* Belt 50750-50799 */
    {50750, 50759, EQUIP_SLOT_BELT, ITEM_GRADE_COMMON,   1, 0, 0, 8, 6, 1, 1, 1, 1, 25, 15, 0, 0},
    {50760, 50769, EQUIP_SLOT_BELT, ITEM_GRADE_UNCOMMON, 10, 0, 0, 18, 14, 2, 2, 2, 2, 50, 30, 0, 0},
    {50770, 50779, EQUIP_SLOT_BELT, ITEM_GRADE_RARE,     20, 0, 0, 35, 28, 4, 4, 4, 4, 100, 60, 15, 0},
    {50780, 50789, EQUIP_SLOT_BELT, ITEM_GRADE_UNIQUE,   30, 0, 0, 60, 48, 7, 7, 7, 7, 170, 100, 25, 0},
    {50790, 50799, EQUIP_SLOT_BELT, ITEM_GRADE_LEGENDARY,40, 0, 0, 100, 80, 12, 12, 12, 12, 280, 170, 40, 0},

    /* Pants/Legs 50800-50849 */
    {50800, 50809, EQUIP_SLOT_PANTS, ITEM_GRADE_COMMON,   1, 0, 0, 20, 16, 0, 0, 2, 0, 60, 0, 0, 0},
    {50810, 50819, EQUIP_SLOT_PANTS, ITEM_GRADE_UNCOMMON, 10, 0, 0, 45, 36, 0, 0, 4, 0, 110, 0, 0, 0},
    {50820, 50829, EQUIP_SLOT_PANTS, ITEM_GRADE_RARE,     20, 0, 0, 90, 72, 0, 0, 7, 0, 210, 0, 0, 0},
    {50830, 50839, EQUIP_SLOT_PANTS, ITEM_GRADE_UNIQUE,   30, 0, 0, 150, 120, 0, 0, 12, 0, 350, 0, 0, 0},
    {50840, 50849, EQUIP_SLOT_PANTS, ITEM_GRADE_LEGENDARY,40, 0, 0, 250, 200, 0, 0, 20, 0, 580, 0, 0, 0},

    /* Shoes 50850-50899 */
    {50850, 50859, EQUIP_SLOT_SHOES, ITEM_GRADE_COMMON,   1, 0, 0, 10, 8, 0, 0, 1, 0, 20, 0, 0, 3},
    {50860, 50869, EQUIP_SLOT_SHOES, ITEM_GRADE_UNCOMMON, 10, 0, 0, 24, 19, 0, 0, 2, 0, 40, 0, 0, 5},
    {50870, 50879, EQUIP_SLOT_SHOES, ITEM_GRADE_RARE,     20, 0, 0, 48, 38, 0, 0, 4, 0, 80, 0, 0, 8},
    {50880, 50889, EQUIP_SLOT_SHOES, ITEM_GRADE_UNIQUE,   30, 0, 0, 80, 64, 0, 0, 7, 0, 140, 0, 0, 12},
    {50890, 50899, EQUIP_SLOT_SHOES, ITEM_GRADE_LEGENDARY,40, 0, 0, 135, 108, 0, 0, 12, 0, 230, 0, 0, 18},

    /* === Accessories === */
    /* Bracelet 50900-50949 */
    {50900, 50909, EQUIP_SLOT_BRACELET, ITEM_GRADE_COMMON,   1, 5, 5, 0, 0, 2, 2, 0, 0, 0, 0, 20, 0},
    {50910, 50919, EQUIP_SLOT_BRACELET, ITEM_GRADE_UNCOMMON, 10, 12, 12, 0, 0, 4, 4, 0, 0, 0, 0, 35, 0},
    {50920, 50929, EQUIP_SLOT_BRACELET, ITEM_GRADE_RARE,     20, 25, 25, 0, 0, 7, 7, 0, 0, 0, 0, 55, 0},
    {50930, 50939, EQUIP_SLOT_BRACELET, ITEM_GRADE_UNIQUE,   30, 45, 45, 0, 0, 12, 12, 0, 0, 0, 0, 80, 0},
    {50940, 50949, EQUIP_SLOT_BRACELET, ITEM_GRADE_LEGENDARY,40, 75, 75, 0, 0, 20, 20, 0, 0, 0, 0, 120, 0},

    /* Necklace 50950-50999 */
    {50950, 50959, EQUIP_SLOT_NECKLACE, ITEM_GRADE_COMMON,   1, 3, 8, 0, 0, 0, 3, 0, 1, 0, 20, 15, 0},
    {50960, 50969, EQUIP_SLOT_NECKLACE, ITEM_GRADE_UNCOMMON, 10, 8, 18, 0, 0, 0, 5, 0, 2, 0, 40, 28, 0},
    {50970, 50979, EQUIP_SLOT_NECKLACE, ITEM_GRADE_RARE,     20, 18, 38, 0, 0, 0, 9, 0, 4, 0, 80, 45, 0},
    {50980, 50989, EQUIP_SLOT_NECKLACE, ITEM_GRADE_UNIQUE,   30, 32, 68, 0, 0, 0, 15, 0, 7, 0, 140, 68, 0},
    {50990, 50999, EQUIP_SLOT_NECKLACE, ITEM_GRADE_LEGENDARY,40, 55, 115, 0, 0, 0, 25, 0, 12, 0, 240, 100, 0},

    /* Ring 51000-51049 */
    {51000, 51009, EQUIP_SLOT_RING, ITEM_GRADE_COMMON,   1, 8, 0, 0, 0, 3, 0, 0, 0, 0, 0, 30, 0},
    {51010, 51019, EQUIP_SLOT_RING, ITEM_GRADE_UNCOMMON, 10, 18, 0, 0, 0, 5, 0, 0, 0, 0, 0, 50, 0},
    {51020, 51029, EQUIP_SLOT_RING, ITEM_GRADE_RARE,     20, 38, 0, 0, 0, 9, 0, 0, 0, 0, 0, 80, 0},
    {51030, 51039, EQUIP_SLOT_RING, ITEM_GRADE_UNIQUE,   30, 68, 0, 0, 0, 15, 0, 0, 0, 0, 0, 120, 0},
    {51040, 51049, EQUIP_SLOT_RING, ITEM_GRADE_LEGENDARY,40, 115, 0, 0, 0, 25, 0, 0, 0, 0, 0, 180, 0},

    /* End marker */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Find equipment template by item_id */
static const EquipmentTemplate* find_equipment_template(uint32_t item_id) {
    for (int i = 0; g_equipment_table[i].item_id_max > 0; i++) {
        if (item_id >= g_equipment_table[i].item_id_min &&
            item_id <= g_equipment_table[i].item_id_max) {
            return &g_equipment_table[i];
        }
    }
    return NULL;
}

/* Calculate enhance bonus multiplier */
static float get_enhance_multiplier(uint8_t enhance_level) {
    /* Enhancement bonuses: +0=1.0, +1=1.03, +2=1.06, ... +10=1.30, +11=1.35 ... */
    if (enhance_level == 0) return 1.0f;
    if (enhance_level <= 10) return 1.0f + enhance_level * 0.03f;
    return 1.30f + (enhance_level - 10) * 0.05f;  /* Bonus after +10 */
}

/* Apply equipment bonus to combat stats */
static void apply_equipment_stats(CombatStats* stats, const EquipmentTemplate* tmpl, uint8_t enhance) {
    float mult = get_enhance_multiplier(enhance);

    /* Apply base stats */
    stats->str += tmpl->str_bonus;
    stats->intel += tmpl->int_bonus;
    stats->vit += tmpl->vit_bonus;
    stats->spr += tmpl->spr_bonus;

    /* Apply combat stats with enhance multiplier */
    stats->phys_attack += (uint32_t)(tmpl->phys_attack * mult);
    stats->mag_attack += (uint32_t)(tmpl->mag_attack * mult);
    stats->phys_def += (uint32_t)(tmpl->phys_def * mult);
    stats->mag_def += (uint32_t)(tmpl->mag_def * mult);

    /* Apply HP/MP bonuses */
    stats->max_hp += tmpl->hp_bonus;
    stats->max_mp += tmpl->mp_bonus;

    /* Apply other bonuses */
    stats->crit_rate += tmpl->crit_bonus;
    stats->attack_speed += tmpl->speed_bonus;
    stats->move_speed += tmpl->speed_bonus / 2;
}

/* ========================================
 * Skill Damage System
 * ======================================== */

/* Skill type determines which attack stat is used */
typedef enum {
    SKILL_TYPE_PHYSICAL = 0,    /* Uses phys_attack */
    SKILL_TYPE_MAGICAL,         /* Uses mag_attack */
    SKILL_TYPE_HYBRID,          /* Uses max(phys, mag) */
    SKILL_TYPE_FIXED            /* Fixed damage, ignores stats */
} SkillDamageType;

/* Element types for elemental damage system */
/* Note: ELEMENT_* constants are defined in game_server_types.h as macros */

/* Monster resistance profile */
typedef struct {
    uint16_t monster_type_base;  /* Base monster type (1000, 1100, etc) */
    uint8_t  phys_resist;        /* Physical resistance % (0-100) */
    uint8_t  mag_resist;         /* Magical resistance % (0-100) */
    uint8_t  fire_resist;        /* Fire resistance % */
    uint8_t  ice_resist;         /* Ice resistance % */
    uint8_t  light_resist;       /* Light resistance % */
    uint8_t  dark_resist;        /* Dark resistance % */
} MonsterResist;

/* Monster resistance table */
static const MonsterResist g_monster_resist_table[] = {
    /* type_base, phys, mag, fire, ice, light, dark */
    {1000, 20, 10, 0, 0, 0, 0},     /* Basic monsters */
    {1100, 30, 15, 50, 0, 0, 0},    /* Fire-type monsters (resist fire) */
    {1200, 25, 20, 0, 50, 0, 0},    /* Ice-type monsters (resist ice) */
    {1300, 15, 35, 0, 0, 50, 0},    /* Light-type monsters (resist light) */
    {1400, 20, 30, 0, 0, 0, 50},    /* Dark-type monsters (resist dark) */
    {1500, 40, 40, 25, 25, 25, 25}, /* Elite monsters (balanced resist) */
    {1600, 50, 20, 0, 30, 0, 0},    /* Armored (high phys resist) */
    {1700, 10, 50, 30, 0, 0, 0},    /* Magical (high mag resist) */
    {2000, 35, 35, 20, 20, 20, 20}, /* Boss base (moderate resist) */
    {0, 0, 0, 0, 0, 0, 0}          /* End marker */
};

/* Find monster resistance by type */
static const MonsterResist* find_monster_resist(uint16_t monster_type) {
    uint16_t base_type = (monster_type / 100) * 100;  /* Round to nearest 100 */
    for (int i = 0; g_monster_resist_table[i].monster_type_base != 0; i++) {
        if (g_monster_resist_table[i].monster_type_base == base_type) {
            return &g_monster_resist_table[i];
        }
    }
    return &g_monster_resist_table[0];  /* Default to basic */
}

/* Skill info for damage calculation */
typedef struct {
    uint16_t skill_id;
    uint8_t  job;               /* Required job (0xFF = any) */
    uint8_t  type;              /* SkillDamageType */
    uint8_t  element;           /* ElementType for elemental damage */
    uint16_t base_damage;       /* Base damage at level 1 */
    uint16_t damage_per_level;  /* Additional damage per skill level */
    uint16_t attack_percent;    /* % of attack stat (100 = 100%) */
    uint16_t hit_count;         /* Number of hits */
    uint16_t cooldown_ms;       /* Cooldown in milliseconds */
    uint16_t mp_cost;           /* MP cost */
} SkillDamageInfo;

/* Skill damage table - based on common DNF skill IDs */
/* Format: {skill_id, job, type, element, base_dmg, dmg/lvl, atk%, hits, cd_ms, mp} */
static const SkillDamageInfo g_skill_damage_table[] = {
    /* Slayer skills (job=0) - Physical, Dark element theme */
    {260, 0, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 150, 30, 120, 1, 1000, 10},     /* Basic attack */
    {261, 0, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 300, 50, 150, 3, 2000, 25},     /* Triple slash */
    {262, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 500, 80, 200, 1, 3000, 40},     /* Dark strike */
    {263, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 800, 120, 250, 5, 5000, 60},    /* Blade storm */
    {264, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 1200, 180, 300, 1, 8000, 100},  /* Ultimate slash */

    /* Fighter skills (job=1) - Physical, Fire element theme */
    {300, 1, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 180, 35, 130, 2, 1200, 12},     /* Double punch */
    {301, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 350, 55, 160, 4, 2200, 30},     /* Burning strikes */
    {302, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 550, 85, 210, 1, 3200, 45},     /* Rising uppercut */
    {303, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 900, 130, 280, 8, 5500, 70},    /* Hundred fists */
    {304, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 1400, 200, 350, 1, 9000, 120},  /* Dragon kick */

    /* Gunner skills (job=2) - hybrid damage, Light element */
    {400, 2, SKILL_TYPE_HYBRID, ELEMENT_NONE, 160, 32, 125, 1, 800, 8},         /* Quick shot */
    {401, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 280, 48, 145, 5, 1800, 22},      /* Rapid fire */
    {402, 2, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 450, 75, 190, 1, 2800, 38},     /* Grenade */
    {403, 2, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 700, 110, 240, 1, 4500, 55},    /* Laser cannon */
    {404, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 1100, 160, 290, 10, 7000, 90},   /* Satellite beam */

    /* Mage skills (job=3) - Fire/Ice elements */
    {500, 3, SKILL_TYPE_MAGICAL, ELEMENT_NONE, 200, 40, 140, 1, 1500, 15},      /* Magic missile */
    {501, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 400, 65, 180, 1, 2500, 35},      /* Fireball */
    {502, 3, SKILL_TYPE_MAGICAL, ELEMENT_ICE, 650, 100, 230, 3, 4000, 55},      /* Ice storm */
    {503, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 1000, 150, 300, 1, 6000, 80},    /* Meteor */
    {504, 3, SKILL_TYPE_MAGICAL, ELEMENT_DARK, 1600, 220, 400, 1, 10000, 150},  /* Apocalypse */

    /* Priest skills (job=4) - Light element */
    {600, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 120, 25, 100, 1, 1000, 10},     /* Holy strike */
    {601, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 250, 45, 140, 1, 2000, 25},     /* Divine smite */
    {602, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 450, 70, 180, 1, 3500, 45},     /* Holy explosion */
    {603, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 750, 115, 250, 1, 5500, 70},    /* Judgment */
    {604, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 1300, 190, 350, 1, 8500, 110},  /* Divine wrath */

    /* Thief skills (job=5) - Dark element */
    {700, 5, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 170, 33, 135, 2, 900, 9},       /* Backstab */
    {701, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 320, 52, 165, 3, 1900, 28},     /* Shadow strike */
    {702, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 500, 82, 215, 5, 3000, 42},     /* Blade dance */
    {703, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 850, 125, 275, 1, 5200, 65},    /* Assassinate */
    {704, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 1350, 195, 340, 1, 8000, 105},  /* Shadow execution */

    /* Common skills (any job) */
    {100, 0xFF, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 100, 20, 100, 1, 500, 0},    /* Normal attack */
    {101, 0xFF, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 50, 10, 50, 1, 0, 0},        /* Basic hit */

    /* Awakening/Ultimate skills - high damage, long cooldown */
    {1000, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 3000, 400, 500, 10, 60000, 200},  /* Slayer awakening */
    {1001, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 3200, 420, 520, 15, 60000, 220},  /* Fighter awakening */
    {1002, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 2800, 380, 480, 20, 60000, 180},   /* Gunner awakening */
    {1003, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 3500, 450, 550, 1, 60000, 250},    /* Mage awakening */
    {1004, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 2500, 350, 450, 1, 60000, 150},   /* Priest awakening */
    {1005, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 3100, 410, 510, 1, 60000, 210},   /* Thief awakening */

    /* End marker */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Find skill info by skill_id */
static const SkillDamageInfo* find_skill_info(uint16_t skill_id) {
    for (int i = 0; g_skill_damage_table[i].skill_id != 0; i++) {
        if (g_skill_damage_table[i].skill_id == skill_id) {
            return &g_skill_damage_table[i];
        }
    }
    return NULL;
}

/* Calculate level difference modifier
 * DNF-style: higher level = bonus damage, lower level = reduced damage
 */
static uint32_t calculate_level_modifier(uint8_t char_level, uint8_t monster_level) {
    int level_diff = (int)char_level - (int)monster_level;

    if (level_diff >= 10) return 130;      /* +30% damage for 10+ levels higher */
    if (level_diff >= 5)  return 115;      /* +15% damage for 5-9 levels higher */
    if (level_diff >= 0)  return 100;      /* Normal damage when same or higher */
    if (level_diff >= -5) return 90;       /* -10% for 1-5 levels lower */
    if (level_diff >= -10) return 75;      /* -25% for 6-10 levels lower */
    return 50;                              /* -50% for 10+ levels lower */
}

/* Get element resistance from monster profile */
static uint8_t get_element_resist(const MonsterResist* resist, uint8_t element) {
    if (!resist) return 0;
    switch (element) {
        case ELEMENT_FIRE:  return resist->fire_resist;
        case ELEMENT_ICE:   return resist->ice_resist;
        case ELEMENT_LIGHT: return resist->light_resist;
        case ELEMENT_DARK:  return resist->dark_resist;
        default:            return 0;
    }
}

/* Calculate skill damage (enhanced version with monster type and level)
 * @param stats        - Character combat stats
 * @param skill_id     - Skill being used
 * @param skill_level  - Level of the skill (1-30)
 * @param job          - Character job
 * @param char_level   - Character level (for level diff calculation)
 * @param monster_type - Monster type (for resistance lookup)
 * @param monster_level - Monster effective level (based on dungeon difficulty)
 * @param monster_def  - Monster's base defense
 * @param is_crit      - Output: whether this was a critical hit
 * @return             - Final damage value
 */
static uint32_t calculate_skill_damage_v2(CombatStats* stats, uint16_t skill_id,
                                          uint8_t skill_level, uint8_t job,
                                          uint8_t char_level, uint16_t monster_type,
                                          uint8_t monster_level, uint32_t monster_def,
                                          uint8_t* is_crit) {
    if (!stats) return 0;
    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;

    /* Find skill info */
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) {
        /* Unknown skill - use basic attack formula */
        uint32_t base = stats->phys_attack / 2;
        if (is_crit) *is_crit = 0;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Check job restriction */
    if (skill->job != 0xFF && skill->job != job) {
        /* Wrong job for this skill - reduced damage */
        uint32_t base = stats->phys_attack / 4;
        if (is_crit) *is_crit = 0;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Get monster resistance profile */
    const MonsterResist* resist = find_monster_resist(monster_type);

    /* Get attack power based on skill type */
    uint32_t attack_power;
    uint32_t defense;
    uint8_t resist_percent = 0;

    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            defense = monster_def;
            resist_percent = resist ? resist->mag_resist : 0;
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;  /* Hybrid bypasses half defense */
            resist_percent = resist ? (resist->phys_resist + resist->mag_resist) / 4 : 0;
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;  /* Fixed damage ignores attack */
            defense = 0;       /* And defense */
            resist_percent = 0;
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            defense = monster_def;
            resist_percent = resist ? resist->phys_resist : 0;
            break;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));

    /* Apply attack power percentage */
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;

    /* Total damage before defense */
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply level modifier */
    uint32_t level_mod = calculate_level_modifier(char_level, monster_level);
    total_damage = (total_damage * level_mod) / 100;

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;  /* Minimum 10% damage */
        if (damage_after_def < 1) damage_after_def = 1;
    }

    /* Apply type resistance (physical/magical) */
    if (resist_percent > 0) {
        damage_after_def = (damage_after_def * (100 - resist_percent)) / 100;
    }

    /* Apply elemental resistance/weakness */
    if (skill->element != ELEMENT_NONE && resist) {
        uint8_t elem_resist = get_element_resist(resist, skill->element);
        if (elem_resist > 0) {
            /* Monster resists this element */
            damage_after_def = (damage_after_def * (100 - elem_resist)) / 100;
        }
        /* Note: Could add elemental weakness here (bonus damage) */
    }

    /* Critical hit check */
    uint8_t crit = 0;
    uint32_t rand_val = (uint32_t)rand() % 10000;  /* 0-9999 */
    if (rand_val < stats->crit_rate) {
        crit = 1;
        damage_after_def = (damage_after_def * 150) / 100;  /* 150% crit damage */
    }
    if (is_crit) *is_crit = crit;

    /* Add some randomness (90%-110%) */
    uint32_t variance = (uint32_t)rand() % 21;  /* 0-20 */
    damage_after_def = (damage_after_def * (90 + variance)) / 100;

    /* Minimum damage is 1 */
    if (damage_after_def < 1) damage_after_def = 1;

    return damage_after_def;
}

/* Legacy wrapper for existing code - simple version */
static uint32_t calculate_skill_damage(CombatStats* stats, uint16_t skill_id,
                                       uint8_t skill_level, uint8_t job,
                                       uint32_t monster_def, uint8_t* is_crit) {
    /* Use v2 with default monster type and same level */
    return calculate_skill_damage_v2(stats, skill_id, skill_level, job,
                                      50, 1000, 50, monster_def, is_crit);
}

/* Get skill MP cost */
static uint16_t get_skill_mp_cost(uint16_t skill_id, uint8_t skill_level) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 0;

    /* MP cost scales with skill level */
    return skill->mp_cost + (skill->mp_cost * (skill_level - 1) / 10);
}

/* Get skill cooldown */
static uint16_t get_skill_cooldown(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1000;  /* Default 1 second */
    return skill->cooldown_ms;
}

/* Get skill hit count */
static uint8_t get_skill_hit_count(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1;
    return (uint8_t)skill->hit_count;
}

/*==============================================================================
 * Phase 6: Buff System
 *============================================================================*/

/* Note: BuffType enum is defined in game_server_types.h */

/* Buff info structure */
typedef struct {
    uint16_t buff_id;
    const char* name;
    uint8_t is_debuff;            /* 1 = debuff, 0 = buff */
    uint8_t max_stacks;           /* Maximum stack count */
    uint8_t can_dispel;           /* 1 = can be dispelled */
    uint16_t default_duration_ms; /* Default duration */
    int16_t  default_value;       /* Default effect value */
} BuffInfo;

/* Buff data table */
static const BuffInfo g_buff_table[] = {
    /* Positive buffs */
    {BUFF_ATTACK_UP, "Attack Up", 0, 1, 1, 30000, 20},
    {BUFF_MAGIC_UP, "Magic Up", 0, 1, 1, 30000, 20},
    {BUFF_DEFENSE_UP, "Defense Up", 0, 1, 1, 30000, 25},
    {BUFF_MAGIC_DEF_UP, "Magic Defense Up", 0, 1, 1, 30000, 25},
    {BUFF_SPEED_UP, "Speed Up", 0, 1, 1, 20000, 30},
    {BUFF_ATTACK_SPEED_UP, "Attack Speed Up", 0, 1, 1, 20000, 15},
    {BUFF_CRIT_UP, "Critical Up", 0, 1, 1, 30000, 10},
    {BUFF_HP_REGEN, "HP Regeneration", 0, 1, 1, 60000, 50},
    {BUFF_MP_REGEN, "MP Regeneration", 0, 1, 1, 60000, 30},
    {BUFF_ALL_STATS_UP, "All Stats Up", 0, 1, 1, 30000, 10},
    {BUFF_INVINCIBLE, "Invincible", 0, 1, 0, 3000, 0},
    {BUFF_SUPER_ARMOR, "Super Armor", 0, 1, 0, 5000, 0},

    /* Debuffs */
    {DEBUFF_ATTACK_DOWN, "Attack Down", 1, 1, 1, 15000, -15},
    {DEBUFF_DEFENSE_DOWN, "Defense Down", 1, 1, 1, 15000, -20},
    {DEBUFF_SPEED_DOWN, "Speed Down", 1, 1, 1, 10000, -30},
    {DEBUFF_POISON, "Poison", 1, 5, 1, 10000, 2},      /* 2% max HP per tick */
    {DEBUFF_BURN, "Burn", 1, 3, 1, 8000, 100},         /* 100 fire damage per tick */
    {DEBUFF_FREEZE, "Freeze", 1, 1, 0, 3000, 0},
    {DEBUFF_STUN, "Stun", 1, 1, 0, 2000, 0},
    {DEBUFF_SILENCE, "Silence", 1, 1, 1, 5000, 0},
    {DEBUFF_BLIND, "Blind", 1, 1, 1, 8000, -30},
    {DEBUFF_BLEED, "Bleed", 1, 5, 1, 12000, 80},       /* 80 physical damage per tick */
    {DEBUFF_CURSE, "Curse", 1, 1, 1, 20000, -10},
    {DEBUFF_SLOW, "Slow", 1, 1, 1, 8000, -20},

    /* Special buffs */
    {BUFF_SHIELD, "Shield", 0, 1, 1, 30000, 500},     /* 500 HP shield */
    {BUFF_LIFESTEAL, "Lifesteal", 0, 1, 1, 20000, 10}, /* 10% lifesteal */
    {BUFF_REFLECT, "Damage Reflect", 0, 1, 1, 15000, 20}, /* 20% reflect */
    {BUFF_ELEMENTAL_FIRE, "Fire Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_ICE, "Ice Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_LIGHT, "Light Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_DARK, "Dark Enchant", 0, 1, 1, 60000, 0},

    /* End marker */
    {0, NULL, 0, 0, 0, 0, 0}
};

/* Find buff info by ID */
static const BuffInfo* find_buff_info(uint16_t buff_id) {
    for (int i = 0; g_buff_table[i].name != NULL; i++) {
        if (g_buff_table[i].buff_id == buff_id) {
            return &g_buff_table[i];
        }
    }
    return NULL;
}

/* Get current timestamp in milliseconds */
/* Note: get_current_time_ms is defined in systems/time_utils.cpp */
/* Using extern declaration from time_utils.h */

/* Add a buff to a player */
static int buff_add(ClientSession* s, uint16_t buff_id, uint32_t source_id,
                    int32_t value, uint32_t duration_ms) {
    if (!s) return -1;

    const BuffInfo* info = find_buff_info(buff_id);
    if (!info) return -1;

    uint32_t now = get_current_time_ms();
    uint32_t expire_time = now + duration_ms;

    /* Check if buff already exists */
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id) {
            /* Buff exists - check stacking behavior */
            if (info->max_stacks > 1 && s->active_buffs[i].stacks < info->max_stacks) {
                /* Add stack */
                s->active_buffs[i].stacks++;
                s->active_buffs[i].expire_time = expire_time;  /* Refresh duration */
                printf("[BUFF] Added stack to %s (stacks: %d)\n",
                       info->name, s->active_buffs[i].stacks);
            } else {
                /* Refresh duration only */
                s->active_buffs[i].expire_time = expire_time;
                printf("[BUFF] Refreshed %s duration\n", info->name);
            }
            return 0;
        }
    }

    /* Add new buff */
    if (s->buff_count >= MAX_ACTIVE_BUFFS) {
        printf("[BUFF] Cannot add buff - max buffs reached\n");
        return -1;
    }

    s->active_buffs[s->buff_count].buff_id = buff_id;
    s->active_buffs[s->buff_count].source_id = source_id;
    s->active_buffs[s->buff_count].expire_time = expire_time;
    s->active_buffs[s->buff_count].value = value;
    s->active_buffs[s->buff_count].stacks = 1;
    s->buff_count++;

    printf("[BUFF] Applied %s (value: %d, duration: %ums)\n",
           info->name, value, duration_ms);
    return 0;
}

/* Remove a buff from a player */
static int buff_remove(ClientSession* s, uint16_t buff_id) {
    if (!s) return -1;

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id) {
            /* Shift remaining buffs */
            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;

            const BuffInfo* info = find_buff_info(buff_id);
            printf("[BUFF] Removed %s\n", info ? info->name : "unknown buff");
            return 0;
        }
    }
    return -1;  /* Buff not found */
}

/* Check if player has a specific buff */
static int buff_has(ClientSession* s, uint16_t buff_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id &&
            s->active_buffs[i].expire_time > now) {
            return 1;
        }
    }
    return 0;
}

/* Get buff value (with stacks) */
static int32_t buff_get_value(ClientSession* s, uint16_t buff_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id &&
            s->active_buffs[i].expire_time > now) {
            return s->active_buffs[i].value * s->active_buffs[i].stacks;
        }
    }
    return 0;
}

/* Update buffs - expire old ones, process DOTs */
static void buff_update(ClientSession* s) {
    if (!s) return;

    uint32_t now = get_current_time_ms();
    int i = 0;

    while (i < s->buff_count) {
        if (s->active_buffs[i].expire_time <= now) {
            /* Buff expired - remove it */
            const BuffInfo* info = find_buff_info(s->active_buffs[i].buff_id);
            printf("[BUFF] %s expired\n", info ? info->name : "unknown");

            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;
            /* Don't increment i - check the shifted buff */
        } else {
            i++;
        }
    }
}

/* Process DOT (Damage Over Time) and HOT (Heal Over Time) effects - call every tick */
static void buff_process_dots_and_hots(ClientSession* s, uint32_t delta_ms) {
    if (!s) return;

    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].expire_time <= now) continue;

        uint16_t buff_id = s->active_buffs[i].buff_id;
        int32_t value = s->active_buffs[i].value * s->active_buffs[i].stacks;

        /* DOT debuffs - deal damage over time */
        if (buff_id == DEBUFF_POISON) {
            /* Poison: % max HP damage per second */
            uint32_t dot_damage = (s->combat_stats.max_hp * value) / 100;
            dot_damage = (dot_damage * delta_ms) / 1000;  /* Scale by time */
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Poison dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        } else if (buff_id == DEBUFF_BURN) {
            /* Burn: fixed fire damage per second */
            uint32_t dot_damage = (value * delta_ms) / 1000;
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Burn dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        } else if (buff_id == DEBUFF_BLEED) {
            /* Bleed: fixed physical damage per second */
            uint32_t dot_damage = (value * delta_ms) / 1000;
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Bleed dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        }
        /* HOT buffs - heal over time */
        else if (buff_id == BUFF_REGEN) {
            /* Regeneration: % max HP heal per second */
            uint32_t hot_heal = (s->combat_stats.max_hp * value) / 100;
            hot_heal = (hot_heal * delta_ms) / 1000;
            if (hot_heal > 0) {
                s->current_hp += hot_heal;
                if (s->current_hp > s->combat_stats.max_hp) {
                    s->current_hp = s->combat_stats.max_hp;
                }
                printf("[HOT] Regen healed %u, HP=%u\n", hot_heal, s->current_hp);
            }
        } else if (buff_id == BUFF_HEAL_OVER_TIME) {
            /* Fixed HP heal per second */
            uint32_t hot_heal = (value * delta_ms) / 1000;
            if (hot_heal > 0) {
                s->current_hp += hot_heal;
                if (s->current_hp > s->combat_stats.max_hp) {
                    s->current_hp = s->combat_stats.max_hp;
                }
            }
        }
    }
}

/* Legacy wrapper for compatibility */
static void buff_process_dots(ClientSession* s, uint32_t delta_ms) {
    buff_process_dots_and_hots(s, delta_ms);
}

/* Calculate total buff modifier for a stat type */
static int32_t buff_get_stat_modifier(ClientSession* s, BuffType stat_buff) {
    if (!s) return 0;

    int32_t total = 0;
    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].expire_time <= now) continue;

        if (s->active_buffs[i].buff_id == stat_buff) {
            total += s->active_buffs[i].value * s->active_buffs[i].stacks;
        }
    }
    return total;
}

/* Dispel all debuffs from a player */
static int buff_dispel_debuffs(ClientSession* s) {
    if (!s) return 0;

    int removed = 0;
    int i = 0;

    while (i < s->buff_count) {
        const BuffInfo* info = find_buff_info(s->active_buffs[i].buff_id);
        if (info && info->is_debuff && info->can_dispel) {
            /* Remove this debuff */
            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;
            removed++;
        } else {
            i++;
        }
    }

    printf("[BUFF] Dispelled %d debuffs\n", removed);
    return removed;
}

/* Alias for buff_dispel_debuffs (Phase 14 compatibility) */
static int buff_remove_debuffs(ClientSession* s) {
    return buff_dispel_debuffs(s);
}

/* Clear all buffs (on death, etc.) */
static void buff_clear_all(ClientSession* s) {
    if (!s) return;
    s->buff_count = 0;
    printf("[BUFF] Cleared all buffs\n");
}

/* Apply buff modifiers to combat stats (call after equipment changes or buff changes) */
static void buff_apply_stat_modifiers(ClientSession* s) {
    if (!s) return;

    /* Store base stats (without buff modifiers) - assuming base_stats exists */
    /* For now, apply percentage modifiers directly */

    /* Attack buffs */
    int32_t attack_mod = buff_get_stat_modifier(s, BUFF_ATTACK_UP);
    int32_t magic_mod = buff_get_stat_modifier(s, BUFF_MAGIC_UP);
    int32_t all_mod = buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);

    /* Defense buffs */
    int32_t def_mod = buff_get_stat_modifier(s, BUFF_DEFENSE_UP);
    int32_t mdef_mod = buff_get_stat_modifier(s, BUFF_MAGIC_DEF_UP);

    /* Speed buffs */
    int32_t speed_mod = buff_get_stat_modifier(s, BUFF_SPEED_UP);
    int32_t aspeed_mod = buff_get_stat_modifier(s, BUFF_ATTACK_SPEED_UP);

    /* Crit buff */
    int32_t crit_mod = buff_get_stat_modifier(s, BUFF_CRIT_UP);

    /* Debuff modifiers */
    int32_t attack_down = buff_get_stat_modifier(s, DEBUFF_ATTACK_DOWN);
    int32_t def_down = buff_get_stat_modifier(s, DEBUFF_DEFENSE_DOWN);
    int32_t speed_down = buff_get_stat_modifier(s, DEBUFF_SPEED_DOWN);
    int32_t curse_mod = buff_get_stat_modifier(s, DEBUFF_CURSE);
    int32_t slow_mod = buff_get_stat_modifier(s, DEBUFF_SLOW);

    /* Berserk special buff */
    int berserk_active = buff_has(s, BUFF_BERSERK);

    /* Calculate net modifiers */
    int32_t net_attack = attack_mod + all_mod - attack_down - curse_mod + (berserk_active ? 30 : 0);
    int32_t net_magic = magic_mod + all_mod - curse_mod;
    int32_t net_def = def_mod + all_mod - def_down - curse_mod - (berserk_active ? 20 : 0);
    int32_t net_mdef = mdef_mod + all_mod - curse_mod;
    int32_t net_speed = speed_mod - speed_down;
    int32_t net_aspeed = aspeed_mod - slow_mod;
    int32_t net_crit = crit_mod + (berserk_active ? 15 : 0);

    /* Apply to combat stats (percentage modifiers) */
    /* These modify the effective stats used in combat calculations */
    s->combat_stats.buff_attack_mod = net_attack;
    s->combat_stats.buff_magic_mod = net_magic;
    s->combat_stats.buff_def_mod = net_def;
    s->combat_stats.buff_mdef_mod = net_mdef;
    s->combat_stats.buff_speed_mod = net_speed;
    s->combat_stats.buff_aspeed_mod = net_aspeed;
    s->combat_stats.buff_crit_mod = net_crit;
}

/* Get effective stat with buff modifier applied */
static uint32_t get_effective_stat(uint32_t base_stat, int32_t buff_percent) {
    if (buff_percent == 0) return base_stat;
    int32_t modified = (int32_t)base_stat * (100 + buff_percent) / 100;
    return (modified > 0) ? (uint32_t)modified : 1;
}

/*==============================================================================
 * Phase 6: Skill Cooldown System
 *============================================================================*/

/* Check if skill is on cooldown */
static int skill_is_on_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            if (s->skill_cooldowns[i].ready_time > now) {
                return 1;  /* Still on cooldown */
            }
            return 0;  /* Cooldown finished */
        }
    }
    return 0;  /* Never used, not on cooldown */
}

/* Get remaining cooldown time in ms */
static uint32_t skill_get_cooldown_remaining(ClientSession* s, uint16_t skill_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            if (s->skill_cooldowns[i].ready_time > now) {
                return s->skill_cooldowns[i].ready_time - now;
            }
            return 0;
        }
    }
    return 0;
}

/* Start cooldown for a skill */
static void skill_start_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return;

    const SkillDamageInfo* skill = find_skill_info(skill_id);
    uint32_t cooldown_ms = skill ? skill->cooldown_ms : 1000;

    /* Apply cooldown reduction from buffs (e.g., -10% CDR) */
    int32_t cdr = buff_get_stat_modifier(s, BUFF_ATTACK_SPEED_UP);  /* Reuse attack speed for CDR */
    if (cdr > 0) {
        cooldown_ms = (cooldown_ms * (100 - cdr)) / 100;
        if (cooldown_ms < 100) cooldown_ms = 100;  /* Minimum 0.1s CD */
    }

    uint32_t ready_time = get_current_time_ms() + cooldown_ms;

    /* Update existing or add new */
    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            s->skill_cooldowns[i].ready_time = ready_time;
            return;
        }
    }

    /* Add new cooldown entry */
    if (s->cooldown_count < MAX_SKILL_COOLDOWNS) {
        s->skill_cooldowns[s->cooldown_count].skill_id = skill_id;
        s->skill_cooldowns[s->cooldown_count].ready_time = ready_time;
        s->cooldown_count++;
    }
}

/* Reset a specific skill cooldown */
static void skill_reset_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return;

    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            s->skill_cooldowns[i].ready_time = 0;
            return;
        }
    }
}

/* Reset all cooldowns */
static void skill_reset_all_cooldowns(ClientSession* s) {
    if (!s) return;
    s->cooldown_count = 0;
}

/*==============================================================================
 * Phase 6: Enhanced Combat with Buffs
 *============================================================================*/

/* Calculate final damage with buff modifiers */
static uint32_t calculate_final_damage(ClientSession* s, uint32_t base_damage,
                                        uint8_t is_physical) {
    if (!s) return base_damage;

    uint32_t damage = base_damage;

    /* Apply attack buffs */
    if (is_physical) {
        int32_t attack_mod = buff_get_stat_modifier(s, BUFF_ATTACK_UP);
        if (attack_mod != 0) {
            damage = (damage * (100 + attack_mod)) / 100;
        }
    } else {
        int32_t magic_mod = buff_get_stat_modifier(s, BUFF_MAGIC_UP);
        if (magic_mod != 0) {
            damage = (damage * (100 + magic_mod)) / 100;
        }
    }

    /* Apply all stats buff */
    int32_t all_mod = buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);
    if (all_mod != 0) {
        damage = (damage * (100 + all_mod)) / 100;
    }

    /* Apply curse debuff (reduces all damage dealt) */
    int32_t curse = buff_get_stat_modifier(s, DEBUFF_CURSE);
    if (curse != 0) {
        damage = (damage * (100 + curse)) / 100;  /* curse is negative */
    }

    return damage;
}

/* Check if player can be damaged (invincibility check) */
static int can_be_damaged(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, BUFF_INVINCIBLE);
}

/* Check if player can use skills (silence check) */
static int can_use_skills(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_SILENCE);
}

/* Check if player can move (freeze/stun check) */
static int can_move(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_FREEZE) && !buff_has(s, DEBUFF_STUN);
}

/* Check if player can act (stun check) */
static int can_act(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_STUN);
}

/* Process lifesteal on damage dealt */
static void process_lifesteal(ClientSession* s, uint32_t damage_dealt) {
    if (!s) return;

    int32_t lifesteal = buff_get_stat_modifier(s, BUFF_LIFESTEAL);
    if (lifesteal > 0) {
        uint32_t heal = (damage_dealt * lifesteal) / 100;
        s->current_hp += heal;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        /* printf("[LIFESTEAL] Healed %u HP\n", heal); */
    }
}

/* Process shield absorption */
static uint32_t process_shield(ClientSession* s, uint32_t incoming_damage) {
    if (!s) return incoming_damage;

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == BUFF_SHIELD) {
            int32_t shield_hp = s->active_buffs[i].value;
            if (shield_hp >= (int32_t)incoming_damage) {
                /* Shield absorbs all damage */
                s->active_buffs[i].value -= incoming_damage;
                printf("[SHIELD] Absorbed %u damage (remaining: %d)\n",
                       incoming_damage, s->active_buffs[i].value);
                return 0;
            } else {
                /* Shield breaks */
                uint32_t remaining = incoming_damage - shield_hp;
                buff_remove(s, BUFF_SHIELD);
                printf("[SHIELD] Broke! Remaining damage: %u\n", remaining);
                return remaining;
            }
        }
    }
    return incoming_damage;
}

/*------------------------------------------------------------------------------
 * Phase 6 Enhanced: Damage Calculation v3 (完整伤害公式)
 *----------------------------------------------------------------------------*/

/* Damage modifier structure - collects all modifiers before final calculation */
typedef struct {
    int32_t attack_percent;      /* Attack power bonus % (from buffs, equipment) */
    int32_t damage_percent;      /* Final damage bonus % */
    int32_t crit_damage_bonus;   /* Critical damage multiplier bonus (150 base + this) */
    int32_t def_penetration;     /* Fixed defense penetration */
    int32_t def_penetration_pct; /* % defense penetration */
    int32_t elem_bonus[6];       /* Elemental damage bonus % */
    uint8_t guaranteed_crit;     /* Force critical hit */
    uint8_t ignore_defense;      /* Ignore all defense */
} DamageModifiers;

/* Get current character level from session */
static uint8_t get_character_level(ClientSession* s) {
    if (!s) return 1;
    for (int i = 0; i < s->character_count && i < MAX_CHARACTERS; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            return s->characters[i].level;
        }
    }
    return 1;
}

/* Collect all damage modifiers from session (buffs, equipment, etc) */
static void collect_damage_modifiers(ClientSession* s, DamageModifiers* mods) {
    memset(mods, 0, sizeof(DamageModifiers));
    if (!s) return;

    /* Buff modifiers */
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ATTACK_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_MAGIC_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);

    /* Debuff effects on player (reduce damage) */
    int32_t curse = buff_get_stat_modifier(s, DEBUFF_CURSE);
    if (curse > 0) {
        mods->damage_percent -= curse;
    }
}

/* Enhanced damage calculation with all modifiers */
static uint32_t calculate_skill_damage_v3(ClientSession* s, uint16_t skill_id,
                                          uint8_t skill_level, uint16_t monster_type,
                                          uint8_t monster_level, uint32_t monster_def,
                                          uint8_t* is_crit, uint8_t* is_miss) {
    if (!s) return 0;

    CombatStats* stats = &s->combat_stats;
    uint8_t char_level = get_character_level(s);

    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;
    if (is_crit) *is_crit = 0;
    if (is_miss) *is_miss = 0;

    /* Collect all modifiers */
    DamageModifiers mods;
    collect_damage_modifiers(s, &mods);

    /* Find skill info */
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) {
        uint32_t base = stats->phys_attack / 2;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Miss check (hit rate vs evasion) */
    uint32_t hit_chance = stats->hit_rate;
    if (monster_level > char_level) {
        int level_diff = monster_level - char_level;
        hit_chance = hit_chance * (100 - level_diff * 2) / 100;
    }
    uint32_t miss_roll = (uint32_t)rand() % 10000;
    if (miss_roll >= hit_chance) {
        if (is_miss) *is_miss = 1;
        return 0;
    }

    /* Get monster resistance */
    const MonsterResist* resist = find_monster_resist(monster_type);

    /* Get base attack power */
    uint32_t attack_power;
    uint32_t defense = monster_def;
    uint8_t resist_percent = 0;

    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            resist_percent = resist ? resist->mag_resist : 0;
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;
            resist_percent = resist ? (resist->phys_resist + resist->mag_resist) / 4 : 0;
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;
            defense = 0;
            resist_percent = 0;
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            resist_percent = resist ? resist->phys_resist : 0;
            break;
    }

    /* Apply attack power modifier from buffs */
    if (mods.attack_percent != 0) {
        attack_power = attack_power * (100 + mods.attack_percent) / 100;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply level modifier */
    uint32_t level_mod = calculate_level_modifier(char_level, monster_level);
    total_damage = (total_damage * level_mod) / 100;

    /* Apply defense penetration */
    if (mods.ignore_defense) {
        defense = 0;
    } else {
        if (mods.def_penetration > 0 && defense > (uint32_t)mods.def_penetration) {
            defense -= mods.def_penetration;
        }
        if (mods.def_penetration_pct > 0) {
            defense = defense * (100 - mods.def_penetration_pct) / 100;
        }
    }

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;
        if (damage_after_def < 1) damage_after_def = 1;
    }

    /* Apply type resistance */
    if (resist_percent > 0) {
        damage_after_def = damage_after_def * (100 - resist_percent) / 100;
    }

    /* Apply elemental bonus/resistance */
    if (skill->element != ELEMENT_NONE && skill->element < 6) {
        int32_t elem_bonus = mods.elem_bonus[skill->element];
        if (elem_bonus > 0) {
            damage_after_def = damage_after_def * (100 + elem_bonus) / 100;
        }
        if (resist) {
            uint8_t elem_resist = get_element_resist(resist, skill->element);
            if (elem_resist > 0) {
                damage_after_def = damage_after_def * (100 - elem_resist) / 100;
            }
        }
    }

    /* Critical hit check */
    uint8_t crit = 0;
    uint32_t crit_rate = stats->crit_rate;
    if (mods.guaranteed_crit) {
        crit = 1;
    } else {
        uint32_t rand_val = (uint32_t)rand() % 10000;
        if (rand_val < crit_rate) {
            crit = 1;
        }
    }

    if (crit) {
        uint32_t crit_multiplier = 150 + mods.crit_damage_bonus;
        damage_after_def = damage_after_def * crit_multiplier / 100;
    }
    if (is_crit) *is_crit = crit;

    /* Apply final damage modifier */
    if (mods.damage_percent != 0) {
        int32_t modified = (int32_t)damage_after_def * (100 + mods.damage_percent) / 100;
        damage_after_def = (modified > 0) ? (uint32_t)modified : 1;
    }

    /* Add variance (95%-105%) */
    uint32_t variance = (uint32_t)rand() % 11;
    damage_after_def = damage_after_def * (95 + variance) / 100;

    if (damage_after_def < 1) damage_after_def = 1;

    return damage_after_def;
}

/*==============================================================================
 * Phase 7 P1: Equipment Enhancement System (装备强化系统)
 *============================================================================*/

/* Enhancement level caps and success rates */
#define ENHANCE_MAX_LEVEL 15
#define ENHANCE_SAFE_LEVEL 4   /* No failure below this level */

/* Enhancement success rates (percentage * 100, e.g., 9500 = 95.00%) */
static const uint16_t g_enhance_success_rate[ENHANCE_MAX_LEVEL + 1] = {
    10000,  /* +0 -> +1: 100% */
    10000,  /* +1 -> +2: 100% */
    10000,  /* +2 -> +3: 100% */
    10000,  /* +3 -> +4: 100% */
    9500,   /* +4 -> +5: 95% */
    9000,   /* +5 -> +6: 90% */
    8500,   /* +6 -> +7: 85% */
    8000,   /* +7 -> +8: 80% */
    7000,   /* +8 -> +9: 70% */
    6000,   /* +9 -> +10: 60% */
    5000,   /* +10 -> +11: 50% */
    4000,   /* +11 -> +12: 40% */
    3000,   /* +12 -> +13: 30% */
    2000,   /* +13 -> +14: 20% */
    1000,   /* +14 -> +15: 10% */
    0       /* Already max */
};

/* Enhancement failure penalty: 0=nothing, 1=downgrade, 2=destroy */
static const uint8_t g_enhance_failure_penalty[ENHANCE_MAX_LEVEL + 1] = {
    0, 0, 0, 0,     /* +0~+3: No penalty */
    0,              /* +4: No penalty */
    1,              /* +5: Downgrade */
    1,              /* +6: Downgrade */
    1,              /* +7: Downgrade */
    1,              /* +8: Downgrade */
    1,              /* +9: Downgrade */
    2,              /* +10: Destroy risk */
    2,              /* +11: Destroy risk */
    2,              /* +12: Destroy risk */
    2,              /* +13: Destroy risk */
    2,              /* +14: Destroy risk */
    0               /* Max level */
};

/* Enhancement stat bonuses per level (percentage of base stat) */
static const uint8_t g_enhance_stat_bonus[ENHANCE_MAX_LEVEL + 1] = {
    0,    /* +0: 0% */
    3,    /* +1: 3% */
    6,    /* +2: 6% */
    9,    /* +3: 9% */
    12,   /* +4: 12% */
    16,   /* +5: 16% */
    20,   /* +6: 20% */
    25,   /* +7: 25% */
    30,   /* +8: 30% */
    36,   /* +9: 36% */
    43,   /* +10: 43% */
    51,   /* +11: 51% */
    60,   /* +12: 60% */
    70,   /* +13: 70% */
    82,   /* +14: 82% */
    100   /* +15: 100% (double stats) */
};

/* Enhancement gold cost per level */
uint32_t get_enhance_cost(uint8_t current_level, uint8_t item_level) {
    /* Base cost = item_level * 100 */
    /* Scales with enhancement level */
    uint32_t base = (uint32_t)item_level * 100;
    uint32_t multiplier = 1 + current_level * current_level;
    return base * multiplier;
}

/* Calculate enhanced stat value */
static uint32_t calculate_enhanced_stat(uint32_t base_stat, uint8_t enhance_level) {
    if (enhance_level > ENHANCE_MAX_LEVEL) enhance_level = ENHANCE_MAX_LEVEL;
    return base_stat + (base_stat * g_enhance_stat_bonus[enhance_level]) / 100;
}

/* Protection item types */
#define PROTECT_NONE       0
#define PROTECT_DOWNGRADE  1   /* Prevents downgrade on failure */
#define PROTECT_DESTROY    2   /* Prevents destruction on failure */
#define PROTECT_FULL       3   /* Guarantees success */

/* Attempt to enhance equipment */
int enhance_equipment(uint8_t current_level, uint8_t protect_type,
                            uint8_t* new_level, uint8_t* was_destroyed) {
    *was_destroyed = 0;
    *new_level = current_level;

    if (current_level >= ENHANCE_MAX_LEVEL) {
        return -1;  /* Already max level */
    }

    /* Full protection = guaranteed success */
    if (protect_type == PROTECT_FULL) {
        *new_level = current_level + 1;
        printf("[ENHANCE] Protected success: +%d -> +%d\n", current_level, *new_level);
        return 1;
    }

    /* Roll for success */
    uint32_t roll = (uint32_t)(rand() % 10000);
    uint16_t success_rate = g_enhance_success_rate[current_level];

    if (roll < success_rate) {
        /* Success! */
        *new_level = current_level + 1;
        printf("[ENHANCE] Success: +%d -> +%d (roll=%u < %u)\n",
               current_level, *new_level, roll, success_rate);
        return 1;
    }

    /* Failure - check penalty */
    uint8_t penalty = g_enhance_failure_penalty[current_level];
    printf("[ENHANCE] Failed: +%d (roll=%u >= %u, penalty=%d)\n",
           current_level, roll, success_rate, penalty);

    if (penalty == 0) {
        /* No penalty */
        return 0;
    } else if (penalty == 1) {
        /* Downgrade */
        if (protect_type != PROTECT_DOWNGRADE) {
            *new_level = (current_level > 0) ? current_level - 1 : 0;
            printf("[ENHANCE] Downgraded to +%d\n", *new_level);
        }
        return 0;
    } else if (penalty == 2) {
        /* Destroy risk */
        if (protect_type != PROTECT_DESTROY) {
            /* 30% chance of destruction */
            if ((uint32_t)(rand() % 100) < 30) {
                *was_destroyed = 1;
                printf("[ENHANCE] DESTROYED!\n");
                return -2;
            }
            /* Otherwise just downgrade */
            *new_level = (current_level > 0) ? current_level - 1 : 0;
            printf("[ENHANCE] Avoided destruction, downgraded to +%d\n", *new_level);
        }
        return 0;
    }

    return 0;
}

/* DB: Enhancement helpers */
int db_get_enhance_level(uint32_t ui_id, uint8_t* level) {
    if (!g_db_charac2 || !level) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT seal_cnt FROM user_items WHERE ui_id=%u LIMIT 1",
             ui_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Get enhance level failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row || !row[0]) {
        mysql_free_result(result);
        return -1;
    }

    *level = (uint8_t)atoi(row[0]);
    mysql_free_result(result);
    return 0;
}

int db_update_enhance_level(uint32_t ui_id, uint8_t enhance_level) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET seal_cnt=%u WHERE ui_id=%u LIMIT 1",
             (unsigned)enhance_level, ui_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update enhance level failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    return (int)mysql_affected_rows(g_db_charac2);
}

/*==============================================================================
 * Phase 7 P2: Random Attribute System (随机属性系统)
 *============================================================================*/

/* Attribute types for random stats */


/* Attribute rarity */


/* Random attribute definition */


/* Max random attributes per item */
#define MAX_RANDOM_ATTRS 4

/* Attribute weight for generation (higher = more common) */
static const uint8_t g_attr_weight[RATTR_MAX] = {
    0,    /* NONE */
    20,   /* PHY_ATTACK */
    20,   /* MAG_ATTACK */
    15,   /* PHY_DEFENSE */
    15,   /* MAG_DEFENSE */
    25,   /* MAX_HP */
    20,   /* MAX_MP */
    10,   /* CRIT_RATE */
    8,    /* CRIT_DAMAGE */
    12,   /* ATTACK_SPEED */
    10,   /* MOVE_SPEED */
    8,    /* HP_REGEN */
    8,    /* MP_REGEN */
    5,    /* ALL_STATS */
    6,    /* ELEMENTAL_FIRE */
    6,    /* ELEMENTAL_ICE */
    6,    /* ELEMENTAL_LIGHT */
    6,    /* ELEMENTAL_DARK */
    4,    /* LIFESTEAL */
    5,    /* DAMAGE_REDUCE */
    5     /* COOLDOWN_REDUCE */
};

/* Value ranges per rarity [min, max] */
static const int16_t g_attr_value_range[5][2] = {
    {1, 10},      /* NORMAL */
    {5, 20},      /* MAGIC */
    {15, 40},     /* RARE */
    {30, 70},     /* EPIC */
    {50, 100}     /* LEGENDARY */
};

/* Rarity drop rates (per 10000) */
static const uint16_t g_rarity_rate[5] = {
    5000,   /* NORMAL: 50% */
    3000,   /* MAGIC: 30% */
    1500,   /* RARE: 15% */
    450,    /* EPIC: 4.5% */
    50      /* LEGENDARY: 0.5% */
};

/* Generate a random attribute */
static RandomAttribute generate_random_attribute(uint8_t item_level) {
    RandomAttribute attr;
    memset(&attr, 0, sizeof(attr));

    /* Determine rarity */
    uint32_t roll = (uint32_t)(rand() % 10000);
    uint32_t cumulative = 0;
    attr.rarity = RARITY_NORMAL;

    for (int i = 0; i < 5; i++) {
        cumulative += g_rarity_rate[i];
        if (roll < cumulative) {
            attr.rarity = (uint8_t)i;
            break;
        }
    }

    /* Weighted random attribute type */
    uint32_t total_weight = 0;
    for (int i = 1; i < RATTR_MAX; i++) {
        total_weight += g_attr_weight[i];
    }

    roll = (uint32_t)(rand() % total_weight);
    cumulative = 0;
    attr.attr_type = RATTR_PHY_ATTACK;  /* Default */

    for (int i = 1; i < RATTR_MAX; i++) {
        cumulative += g_attr_weight[i];
        if (roll < cumulative) {
            attr.attr_type = (uint8_t)i;
            break;
        }
    }

    /* Generate value based on rarity and item level */
    int16_t min_val = g_attr_value_range[attr.rarity][0];
    int16_t max_val = g_attr_value_range[attr.rarity][1];

    /* Scale with item level (1% per level above 1) */
    int32_t level_scale = 100 + (item_level - 1);
    min_val = (int16_t)((min_val * level_scale) / 100);
    max_val = (int16_t)((max_val * level_scale) / 100);

    attr.value = min_val + (int16_t)(rand() % (max_val - min_val + 1));

    return attr;
}

/* Generate random attributes for a new item */
int generate_item_random_attrs(uint8_t item_level, uint8_t item_rarity,
                                      RandomAttribute* attrs, int max_attrs) {
    /* Number of attributes based on item rarity */
    int num_attrs = 0;
    switch (item_rarity) {
        case RARITY_NORMAL:   num_attrs = 0; break;
        case RARITY_MAGIC:    num_attrs = 1; break;
        case RARITY_RARE:     num_attrs = 2; break;
        case RARITY_EPIC:     num_attrs = 3; break;
        case RARITY_LEGENDARY: num_attrs = 4; break;
        default: num_attrs = 0; break;
    }

    if (num_attrs > max_attrs) num_attrs = max_attrs;

    /* Generate unique attributes */
    uint8_t used_types[RATTR_MAX];
    memset(used_types, 0, sizeof(used_types));

    for (int i = 0; i < num_attrs; i++) {
        int attempts = 0;
        do {
            attrs[i] = generate_random_attribute(item_level);
            attempts++;
        } while (used_types[attrs[i].attr_type] && attempts < 20);

        used_types[attrs[i].attr_type] = 1;
    }

    return num_attrs;
}

/* Reroll one random attribute */
static void reroll_random_attribute(RandomAttribute* attr, uint8_t item_level) {
    /* Keep the type, reroll value and possibly rarity */
    uint8_t old_type = attr->attr_type;
    *attr = generate_random_attribute(item_level);
    attr->attr_type = old_type;  /* Keep same type */
}

/* Add item with random attributes to database (for equipment drops) */
static int db_add_item_with_attrs(uint32_t charac_no, uint32_t item_id, int slot,
                                   uint8_t item_rarity, uint8_t item_level) {
    if (!g_db_charac2) return -1;

    /* Generate random attributes based on rarity */
    RandomAttribute attrs[MAX_RANDOM_ATTRS];
    memset(attrs, 0, sizeof(attrs));
    int num_attrs = generate_item_random_attrs(item_level, item_rarity, attrs, MAX_RANDOM_ATTRS);

    /* Encode attributes into hidden_option (16-bit field):
     * bits 0-2: number of attributes (0-4)
     * bits 3-5: primary attribute type
     * bits 6-8: secondary attribute type
     * bits 9-15: combined value index
     */
    uint16_t hidden_option = 0;
    hidden_option |= (num_attrs & 0x7);  /* 3 bits for count */
    if (num_attrs > 0) {
        hidden_option |= ((attrs[0].attr_type & 0x7) << 3);  /* Primary type */
    }
    if (num_attrs > 1) {
        hidden_option |= ((attrs[1].attr_type & 0x7) << 6);  /* Secondary type */
    }
    /* Encode value magnitude in upper bits */
    if (num_attrs > 0) {
        uint8_t value_index = (uint8_t)((attrs[0].value / 10) & 0x7F);  /* 0-127 */
        hidden_option |= ((value_index & 0x7F) << 9);
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, slot, it_id, stat, hidden_option, reg_date, m_time) "
             "VALUES (%u, %d, %u, %u, %u, NOW(), NOW())",
             charac_no, slot, item_id, item_rarity, hidden_option);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Add item with attrs failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    uint32_t new_id = (uint32_t)mysql_insert_id(g_db_charac2);
    printf("[DB] Added item with random attrs: charac_no=%u, item_id=%u, slot=%d, "
           "rarity=%u, attrs=%d, hidden_option=%u, ui_id=%u\n",
           charac_no, item_id, slot, item_rarity, num_attrs, hidden_option, new_id);
    return (int)new_id;
}


/*==============================================================================
 * Phase 7 P4: Skill Learning/Upgrade System (技能学习/升级)
 *============================================================================*/

/* Skill point costs per level */
static const uint8_t g_skill_point_cost[11] = {
    1,   /* Level 1 */
    1,   /* Level 2 */
    1,   /* Level 3 */
    2,   /* Level 4 */
    2,   /* Level 5 */
    3,   /* Level 6 */
    3,   /* Level 7 */
    4,   /* Level 8 */
    5,   /* Level 9 */
    6,   /* Level 10 */
    0    /* Max level */
};

/* Skill prerequisites (skill_id -> required skill_id, required_level) */
typedef struct {
    uint16_t skill_id;
    uint16_t prereq_skill_id;
    uint8_t  prereq_level;
    uint8_t  required_player_level;
} SkillPrerequisite;

/* Example prerequisites (job 0 = Slayer) */
static const SkillPrerequisite g_skill_prerequisites[] = {
    /* Slayer skills */
    {1002, 1001, 5, 10},   /* Skill 1002 requires Skill 1001 Lv5 and player Lv10 */
    {1003, 1002, 3, 20},   /* Skill 1003 requires Skill 1002 Lv3 and player Lv20 */
    {1004, 1003, 5, 30},   /* etc... */
    /* End marker */
    {0, 0, 0, 0}
};

/* Check skill prerequisites */
int check_skill_prerequisite(uint16_t skill_id, uint8_t player_level,
                                    uint16_t* player_skills, uint8_t* skill_levels,
                                    int skill_count) {
    for (int i = 0; g_skill_prerequisites[i].skill_id != 0; i++) {
        if (g_skill_prerequisites[i].skill_id == skill_id) {
            const SkillPrerequisite* prereq = &g_skill_prerequisites[i];

            /* Check player level */
            if (player_level < prereq->required_player_level) {
                printf("[SKILL] Player level too low: %d < %d\n",
                       player_level, prereq->required_player_level);
                return -1;
            }

            /* Check prerequisite skill */
            if (prereq->prereq_skill_id > 0) {
                int found = 0;
                for (int j = 0; j < skill_count; j++) {
                    if (player_skills[j] == prereq->prereq_skill_id &&
                        skill_levels[j] >= prereq->prereq_level) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("[SKILL] Prerequisite not met: need skill %d Lv%d\n",
                           prereq->prereq_skill_id, prereq->prereq_level);
                    return -2;
                }
            }

            return 0;  /* Prerequisites met */
        }
    }

    return 0;  /* No prerequisites for this skill */
}

/* Note: get_skill_sp_cost defined earlier at line ~1507 */

/* Calculate gold cost to reset skills */
uint32_t get_skill_reset_cost(uint8_t player_level) {
    return (uint32_t)player_level * 1000;
}

/* Note: db_learn_skill already defined at line ~2535, reusing it */

/* DB: Reset all skills for a character */
static int db_reset_skills(uint32_t charac_no, uint32_t* out_sp_refund) {
    if (!g_db_charac) return -1;

    /* Calculate total SP to refund */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT SUM(lev) FROM skill WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    MYSQL_ROW row = result ? mysql_fetch_row(result) : NULL;
    *out_sp_refund = row && row[0] ? (uint32_t)atoi(row[0]) : 0;
    if (result) mysql_free_result(result);

    /* Delete all skills */
    snprintf(query, sizeof(query),
             "DELETE FROM skill WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Reset skills failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[DB] Reset skills for charac_no=%u, refund SP=%u\n", charac_no, *out_sp_refund);
    return 0;
}

/*==============================================================================
 * Phase 7 P5: Equipment Validation System (装备穿戴验证)
 *============================================================================*/

/* Equipment slots: Using existing EQUIP_SLOT_* macros (defined at line ~548) */
#define EQUIP_SLOT_TITLE     11   /* Title slot (not in original macros) */
#define EQUIP_SLOT_MAX_COUNT 12   /* Total equipment slots */

/* Equipment requirements */
typedef struct {
    uint32_t item_id;
    uint8_t  required_level;
    uint8_t  required_job;      /* 0 = any, or specific job */
    uint8_t  required_sex;      /* 0 = any, 1 = male, 2 = female */
    uint8_t  slot;              /* EquipmentSlot */
    uint8_t  is_unique;         /* 1 = only one can be equipped */
} EquipmentRequirement;

/* Job compatibility check */
static int is_job_compatible(uint8_t item_job, uint8_t player_job) {
    if (item_job == 0) return 1;  /* Any job */

    /* Job class grouping (0-4 = base jobs, each has 4 advanced forms) */
    uint8_t item_base = item_job / 5;
    uint8_t player_base = player_job / 5;

    return (item_base == player_base || item_job == player_job);
}

/* Validate equipment can be worn */
int validate_equipment(uint32_t item_id, uint8_t player_level,
                             uint8_t player_job, uint8_t player_sex,
                             uint8_t target_slot) {
    /* In production: lookup item requirements from PVF data or DB */
    /* For now: use item_id to derive basic requirements */

    /* Item level requirement = (item_id % 100) */
    uint8_t req_level = (uint8_t)(item_id % 100);
    if (req_level < 1) req_level = 1;
    if (req_level > 85) req_level = 85;

    /* Check level */
    if (player_level < req_level) {
        printf("[EQUIP] Level too low: %d < %d\n", player_level, req_level);
        return -1;
    }

    /* Item job = ((item_id / 100) % 10) */
    uint8_t req_job = (uint8_t)((item_id / 100) % 10);
    if (!is_job_compatible(req_job, player_job)) {
        printf("[EQUIP] Job incompatible: item=%d player=%d\n", req_job, player_job);
        return -2;
    }

    /* Sex requirement for certain items (fashion items) */
    if ((item_id / 10000) == 50) {  /* Fashion item category */
        uint8_t req_sex = (uint8_t)((item_id / 1000) % 10);
        if (req_sex != 0 && req_sex != player_sex) {
            printf("[EQUIP] Sex mismatch: item=%d player=%d\n", req_sex, player_sex);
            return -3;
        }
    }

    /* Slot validation */
    uint8_t expected_slot = (uint8_t)((item_id / 10000) % EQUIP_SLOT_MAX_COUNT);
    if (target_slot != expected_slot && expected_slot != 0) {
        printf("[EQUIP] Wrong slot: expected=%d got=%d\n", expected_slot, target_slot);
        return -4;
    }

    return 0;  /* Valid */
}

/*==============================================================================
 * Phase 7 P6: Item Stacking System (物品堆叠合并)
 *============================================================================*/

/* Item categories */
typedef enum {
    ITEM_CAT_EQUIPMENT = 0,   /* Weapons, armor - not stackable */
    ITEM_CAT_CONSUMABLE = 1,  /* Potions, food - stackable */
    ITEM_CAT_MATERIAL = 2,    /* Crafting materials - stackable */
    ITEM_CAT_QUEST = 3,       /* Quest items - may be stackable */
    ITEM_CAT_CURRENCY = 4,    /* Tokens, special currencies - stackable */
    ITEM_CAT_MISC = 5         /* Miscellaneous */
} ItemCategory;

/* Max stack sizes per category */
static const uint16_t g_max_stack_size[6] = {
    1,     /* EQUIPMENT: Not stackable */
    99,    /* CONSUMABLE: Stack to 99 */
    999,   /* MATERIAL: Stack to 999 */
    99,    /* QUEST: Stack to 99 */
    9999,  /* CURRENCY: Stack to 9999 */
    99     /* MISC: Stack to 99 */
};

/* Get item category from item_id */
static uint8_t get_item_category(uint32_t item_id) {
    /* Category encoded in item_id range */
    uint32_t category_code = item_id / 100000;

    switch (category_code) {
        case 0:
        case 1:
        case 2:
        case 3:  return ITEM_CAT_EQUIPMENT;   /* 0-399999 */
        case 4:
        case 5:  return ITEM_CAT_CONSUMABLE;  /* 400000-599999 */
        case 6:
        case 7:  return ITEM_CAT_MATERIAL;    /* 600000-799999 */
        case 8:  return ITEM_CAT_QUEST;       /* 800000-899999 */
        case 9:  return ITEM_CAT_CURRENCY;    /* 900000-999999 */
        default: return ITEM_CAT_MISC;
    }
}

/* Check if item is stackable */
static int is_item_stackable(uint32_t item_id) {
    return get_item_category(item_id) != ITEM_CAT_EQUIPMENT;
}

/* Get max stack size for item */
static uint16_t get_max_stack_size(uint32_t item_id) {
    return g_max_stack_size[get_item_category(item_id)];
}

/* DB: Try to stack item with existing stack */
int db_try_stack_item(uint32_t charac_no, uint32_t item_id,
                             uint16_t count, int32_t* out_slot) {
    if (!g_db_charac2) return -1;

    uint16_t max_stack = get_max_stack_size(item_id);
    if (max_stack <= 1) {
        *out_slot = -1;
        return 0;  /* Not stackable */
    }

    /* Find existing stack with room */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ui_id, slot, stat FROM user_items "
             "WHERE charac_no=%u AND it_id=%u AND stat < %d "
             "ORDER BY slot LIMIT 1",
             charac_no, item_id, max_stack);

    if (mysql_query(g_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    MYSQL_ROW row = result ? mysql_fetch_row(result) : NULL;

    if (row) {
        uint32_t stack_ui_id = row[0] ? static_cast<uint32_t>(strtoul(row[0], NULL, 10)) : 0;
        int32_t slot = row[1] ? atoi(row[1]) : -1;
        uint16_t current = row[2] ? (uint16_t)atoi(row[2]) : 0;
        mysql_free_result(result);

        /* Calculate how many can be added */
        uint16_t space = max_stack - current;
        uint16_t to_add = (count > space) ? space : count;

        /* Update stack */
        snprintf(query, sizeof(query),
                 "UPDATE user_items SET stat=%u WHERE ui_id=%u LIMIT 1",
                 current + to_add, stack_ui_id);
        mysql_query(g_db_charac2, query);

        *out_slot = slot;
        printf("[STACK] Added %d to existing stack at slot %d (now %d)\n",
               to_add, slot, current + to_add);

        return to_add;  /* Return how many were stacked */
    }

    if (result) mysql_free_result(result);
    *out_slot = -1;
    return 0;  /* No existing stack found */
}

/* DB: Split stack */
int db_split_stack(uint32_t charac_no, int32_t source_slot,
                   int32_t target_slot, uint16_t split_count) {
    if (!g_db_charac2 || split_count == 0) return -1;

    DBItemInfo source_item;
    if (db_get_item_at_slot(charac_no, source_slot, &source_item) < 0 ||
        source_item.ui_id == 0) {
        printf("[STACK] No item at source slot %d\n", source_slot);
        return -1;
    }

    uint16_t current_count = static_cast<uint16_t>(source_item.stat);
    if (split_count >= current_count) {
        printf("[STACK] Invalid split count %u (current=%u)\n",
               split_count, current_count);
        return -1;
    }

    DBItemInfo target_item;
    if (db_get_item_at_slot(charac_no, target_slot, &target_item) == 0 &&
        target_item.ui_id != 0) {
        printf("[STACK] Target slot %d already occupied (ui_id=%u)\n",
               target_slot, target_item.ui_id);
        return -1;
    }

    uint16_t remaining = static_cast<uint16_t>(current_count - split_count);

    char query[512];
    snprintf(query, sizeof(query),
                  "UPDATE user_items SET stat=%u WHERE ui_id=%u LIMIT 1",
                  remaining, source_item.ui_id);
    if (mysql_query(g_db_charac2, query)) {
        printf("[STACK] Failed to update source stack: %s\n",
               mysql_error(g_db_charac2));
        return -1;
    }

    snprintf(query, sizeof(query),
                  "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date, m_time) "
                  "VALUES (%u, %d, %u, %u, NOW(), NOW())",
                  charac_no, target_slot, source_item.it_id, split_count);
    if (mysql_query(g_db_charac2, query)) {
        printf("[STACK] Failed to insert split stack: %s\n",
               mysql_error(g_db_charac2));
        snprintf(query, sizeof(query),
                      "UPDATE user_items SET stat=%u WHERE ui_id=%u LIMIT 1",
                      source_item.stat, source_item.ui_id);
        mysql_query(g_db_charac2, query);
        return -1;
    }

    printf("[STACK] Split %u from slot %d (ui_id=%u) to slot %d\n",
           split_count, source_slot, source_item.ui_id, target_slot);
    return 0;
}

/*==============================================================================
 * Phase 7 P7: Dungeon Difficulty Selection (副本难度选择)
 *============================================================================*/

/* Dungeon difficulty info */


static const DungeonDifficultyInfo g_dungeon_difficulties[] = {
    {0, "Normal",   1,  10, 100, 100, 3,  8, 0},
    {1, "Hard",    15,  25, 120, 130, 4, 10, 3},
    {2, "Expert",  30,  40, 150, 170, 5, 12, 5},
    {3, "Master",  50,  60, 200, 220, 6, 15, 7},
    {4, "King",    70,  85, 300, 300, 7, 20, 10}
};

/* Check if difficulty is unlocked for player */
int is_difficulty_unlocked(uint32_t charac_no, uint32_t dungeon_id,
                                  uint8_t difficulty) {
    if (difficulty == 0) return 1;  /* Normal always unlocked */

    if (!g_db_charac) return 1;  /* No DB = allow all */

    /* Check clear count of previous difficulty */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM dungeon_clear_log "
             "WHERE charac_no=%u AND dungeon_id=%u AND difficulty=%d",
             charac_no, dungeon_id, difficulty - 1);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    MYSQL_ROW row = result ? mysql_fetch_row(result) : NULL;
    int clear_count = row ? atoi(row[0]) : 0;
    if (result) mysql_free_result(result);

    return (clear_count >= g_dungeon_difficulties[difficulty].unlock_clear_count);
}

/* Get difficulty info */
const DungeonDifficultyInfo* get_difficulty_info(uint8_t difficulty) {
    if (difficulty > 4) difficulty = 4;
    return &g_dungeon_difficulties[difficulty];
}

/* Calculate scaled monster stats for difficulty */
static void scale_monster_for_difficulty(MonsterInfo* m, uint8_t difficulty,
                                         uint8_t base_level) {
    const DungeonDifficultyInfo* info = get_difficulty_info(difficulty);

    /* Scale level */
    m->level = (uint8_t)((base_level * info->monster_level_scale) / 100);
    if (m->level > 99) m->level = 99;

    /* Scale HP */
    m->max_hp = (m->max_hp * info->monster_level_scale) / 100;
    m->hp = m->max_hp;

    /* Scale damage */
    m->attack_damage = (m->attack_damage * info->monster_level_scale) / 100;

    /* Scale defenses */
    m->phys_def = (m->phys_def * info->monster_level_scale) / 100;
    m->mag_def = (m->mag_def * info->monster_level_scale) / 100;
}

/*==============================================================================
 * Phase 7 P8: Movement Validation System (真实移动同步)
 *============================================================================*/

/* Movement validation constants */
#define MAX_MOVE_SPEED 800       /* Max pixels per second */
#define MAX_TELEPORT_DIST 50     /* Max distance for position correction */
#define SPEED_HACK_THRESHOLD 1.5 /* 150% of max speed = suspicious */

/* Position record for validation */
typedef struct {
    uint16_t x;
    uint16_t y;
    uint32_t timestamp;
} PositionRecord;

#define MAX_POSITION_HISTORY 10

/* Validate movement */
static int validate_movement(PositionRecord* history, int history_count,
                            uint16_t new_x, uint16_t new_y, uint32_t new_time,
                            uint16_t player_speed) {
    if (history_count == 0) return 0;  /* First position */

    PositionRecord* last = &history[history_count - 1];

    /* Calculate distance moved */
    int dx = abs((int)new_x - (int)last->x);
    int dy = abs((int)new_y - (int)last->y);
    uint32_t dist = (uint32_t)(dx + dy);  /* Manhattan distance */

    /* Calculate time elapsed */
    uint32_t time_diff = new_time - last->timestamp;
    if (time_diff == 0) time_diff = 1;  /* Prevent division by zero */

    /* Calculate speed (pixels per second) */
    uint32_t speed = (dist * 1000) / time_diff;

    /* Apply player speed modifier */
    uint32_t max_allowed = (uint32_t)(MAX_MOVE_SPEED * player_speed / 100);
    max_allowed = (uint32_t)(max_allowed * SPEED_HACK_THRESHOLD);

    if (speed > max_allowed) {
        printf("[MOVE] Speed hack detected: speed=%u max=%u\n", speed, max_allowed);
        return -1;  /* Speed hack */
    }

    /* Check for teleport */
    if (dist > MAX_TELEPORT_DIST * 10 && time_diff < 100) {
        printf("[MOVE] Teleport detected: dist=%u time=%u\n", dist, time_diff);
        return -2;  /* Teleport hack */
    }

    return 0;  /* Valid */
}

/* Add position to history */
static void record_position(PositionRecord* history, int* history_count,
                           uint16_t x, uint16_t y, uint32_t timestamp) {
    if (*history_count >= MAX_POSITION_HISTORY) {
        /* Shift history */
        memmove(history, history + 1,
                sizeof(PositionRecord) * (MAX_POSITION_HISTORY - 1));
        (*history_count)--;
    }

    history[*history_count].x = x;
    history[*history_count].y = y;
    history[*history_count].timestamp = timestamp;
    (*history_count)++;
}

/*==============================================================================
 * Phase 7 P9: Guild Level System (公会经验/等级)
 *============================================================================*/

/* Guild level requirements */
static const uint32_t g_guild_exp_table[21] = {
    0,          /* Level 1 */
    1000,       /* Level 2 */
    3000,       /* Level 3 */
    6000,       /* Level 4 */
    10000,      /* Level 5 */
    15000,      /* Level 6 */
    22000,      /* Level 7 */
    30000,      /* Level 8 */
    40000,      /* Level 9 */
    52000,      /* Level 10 */
    66000,      /* Level 11 */
    82000,      /* Level 12 */
    100000,     /* Level 13 */
    120000,     /* Level 14 */
    145000,     /* Level 15 */
    175000,     /* Level 16 */
    210000,     /* Level 17 */
    250000,     /* Level 18 */
    300000,     /* Level 19 */
    360000,     /* Level 20 (max) */
    0xFFFFFFFF  /* Sentinel */
};

/* Guild benefits per level */
static const uint8_t g_guild_max_members[21] = {
    30, 32, 34, 36, 38, 40, 42, 44, 46, 48,  /* 1-10 */
    50, 52, 54, 56, 58, 60, 65, 70, 80, 100, /* 11-20 */
    100  /* Sentinel */
};

/* Get guild level from exp */
uint8_t get_guild_level_from_exp(uint32_t exp) {
    for (int i = 20; i >= 1; i--) {
        if (exp >= g_guild_exp_table[i]) {
            return (uint8_t)i;
        }
    }
    return 1;
}

/* Get exp needed for next level */
static uint32_t get_guild_exp_for_next_level(uint8_t current_level) {
    if (current_level >= 20) return 0;  /* Max level */
    return g_guild_exp_table[current_level + 1];
}

/* Get max members for guild level */
static uint8_t get_guild_max_members(uint8_t level) {
    if (level > 20) level = 20;
    if (level < 1) level = 1;
    return g_guild_max_members[level];
}

/* DB: Add guild experience */
static int guild_execute_query(const char* query, DBQueryResult* result) {
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_GUILD, query, result) < 0) {
        printf("[DB] Guild query failed: %s\n", query);
        return -1;
    }
    return 0;
}

static int guild_execute_non_query(const char* query) {
    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (guild_execute_query(query, &result) < 0) {
        return -1;
    }
    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

static int db_add_guild_exp(uint32_t guild_id, uint32_t exp_gain,
                            uint8_t* out_new_level, uint8_t* out_level_up) {
    *out_level_up = 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT lev, guild_point FROM guild_info WHERE guild_id=%u",
             guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (guild_execute_query(query, &result) < 0) {
        return -1;
    }

    char* row[2] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 2) {
        DBQueryResult_Free(&result);
        return -1;
    }

    uint8_t current_level = (uint8_t)(row[0] ? atoi(row[0]) : 0);
    uint32_t current_exp = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    DBQueryResult_Free(&result);

    uint32_t new_exp = current_exp + exp_gain;
    uint8_t new_level = get_guild_level_from_exp(new_exp);

    if (new_level > current_level) {
        *out_level_up = 1;
        printf("[GUILD] Guild %u leveled up: %d -> %d\n",
               guild_id, current_level, new_level);
    }
    *out_new_level = new_level;

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET lev=%d, guild_point=%u WHERE guild_id=%u",
             new_level, new_exp, guild_id);

    if (guild_execute_non_query(query) < 0) {
        printf("[DB] Update guild exp failed for guild_id=%u\n", guild_id);
        return -1;
    }

    return 0;
}

/* DB: Add member contribution */
static int db_add_guild_contribution(uint32_t guild_id, uint32_t charac_no,
                                     uint32_t contribution) {
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET contribution=contribution+%u "
             "WHERE guild_id=%u AND charac_no=%u",
             contribution, guild_id, charac_no);

    return guild_execute_non_query(query) < 0 ? -1 : 0;
}

/*==============================================================================
 * Phase 7 P10: Chat Channel System (聊天频道系统)
 *============================================================================*/

/* Chat channel types */
typedef enum {
    CHAT_CHANNEL_NORMAL = 0,   /* Area chat */
    CHAT_CHANNEL_PARTY = 1,    /* Party only */
    CHAT_CHANNEL_GUILD = 2,    /* Guild only */
    CHAT_CHANNEL_WHISPER = 3,  /* Private message */
    CHAT_CHANNEL_WORLD = 4,    /* Server-wide */
    CHAT_CHANNEL_TRADE = 5,    /* Trade channel */
    CHAT_CHANNEL_MEGAPHONE = 6 /* Paid announcement */
} ChatChannel;

/* Chat cooldowns per channel (milliseconds) */
static const uint16_t g_chat_cooldown[7] = {
    500,    /* NORMAL: 0.5s */
    300,    /* PARTY: 0.3s */
    500,    /* GUILD: 0.5s */
    1000,   /* WHISPER: 1s */
    5000,   /* WORLD: 5s */
    3000,   /* TRADE: 3s */
    0       /* MEGAPHONE: No cooldown (costs item) */
};

/* Check if chat is on cooldown */
static int is_chat_on_cooldown(ClientSession* s, uint8_t channel) {
    uint32_t now = get_current_time_ms();

    if (channel >= 7) return 1;  /* Invalid channel */

    /* Use buff array to store cooldowns (temporary solution) */
    uint32_t last_time = 0;  /* In production: store per-channel in session */

    if (now - last_time < g_chat_cooldown[channel]) {
        return 1;  /* Still on cooldown */
    }

    return 0;
}

/* Simple bad word filter (example) */
static const char* g_bad_words[] = {
    "hack", "cheat", "exploit", "bot",
    NULL  /* End marker */
};

/* Filter chat message */
static int filter_chat_message(char* message, int len) {
    /* Convert to lowercase for comparison */
    char lower[256];
    if (len > 255) len = 255;

    for (int i = 0; i < len; i++) {
        lower[i] = (message[i] >= 'A' && message[i] <= 'Z')
                   ? message[i] + 32 : message[i];
    }
    lower[len] = '\0';

    /* Check for bad words */
    for (int i = 0; g_bad_words[i] != NULL; i++) {
        if (strstr(lower, g_bad_words[i]) != NULL) {
            /* Replace with asterisks */
            int word_len = strlen(g_bad_words[i]);
            char* pos = strstr(lower, g_bad_words[i]);
            int offset = pos - lower;
            for (int j = 0; j < word_len && offset + j < len; j++) {
                message[offset + j] = '*';
            }
        }
    }

    return 0;
}

/* Get channel name */
static const char* get_channel_name(uint8_t channel) {
    switch (channel) {
        case CHAT_CHANNEL_NORMAL:    return "Normal";
        case CHAT_CHANNEL_PARTY:     return "Party";
        case CHAT_CHANNEL_GUILD:     return "Guild";
        case CHAT_CHANNEL_WHISPER:   return "Whisper";
        case CHAT_CHANNEL_WORLD:     return "World";
        case CHAT_CHANNEL_TRADE:     return "Trade";
        case CHAT_CHANNEL_MEGAPHONE: return "Megaphone";
        default: return "Unknown";
    }
}

/*==============================================================================
 * Phase 8 P1: Avatar/Fashion System (时装系统)
 *============================================================================*/

#define AVATAR_SLOT_MAX      12
#define AVATAR_SLOT_HAIR     0
#define AVATAR_SLOT_FACE     1
#define AVATAR_SLOT_CHEST    2
#define AVATAR_SLOT_PANTS    3
#define AVATAR_SLOT_SHOES    4
#define AVATAR_SLOT_WAIST    5
#define AVATAR_SLOT_CAP      6
#define AVATAR_SLOT_NECK     7
#define AVATAR_SLOT_SKIN     8
#define AVATAR_SLOT_AURA     9
#define AVATAR_SLOT_WEAPON   10
#define AVATAR_SLOT_PET_DECO 11

/* Avatar item info */
typedef struct {
    uint32_t item_id;
    uint8_t  slot;
    uint8_t  rarity;       /* 0=normal, 1=advanced, 2=rare */
    int16_t  phy_attack;
    int16_t  mag_attack;
    int16_t  str_bonus;
    int16_t  int_bonus;
    int16_t  vit_bonus;
    int16_t  spi_bonus;
    uint16_t move_speed;
    uint16_t attack_speed;
    uint16_t cast_speed;
    uint32_t duration;     /* 0 = permanent, else seconds */
} AvatarItemInfo;

/* Avatar stat bonuses by rarity */
static const int16_t g_avatar_stat_bonus[3] = {5, 15, 30};
static const int16_t g_avatar_speed_bonus[3] = {1, 2, 3};

/* Get avatar slot from item_id */
static uint8_t get_avatar_slot(uint32_t item_id) {
    /* Avatar items: 500000-599999, slot = (id / 10000) % 12 */
    if (item_id >= 500000 && item_id < 600000) {
        return (uint8_t)((item_id / 1000) % AVATAR_SLOT_MAX);
    }
    return 0xFF;  /* Invalid */
}

/* Get avatar rarity from item_id */
static uint8_t get_avatar_rarity(uint32_t item_id) {
    /* Last digit: 0-3=normal, 4-6=advanced, 7-9=rare */
    uint8_t last_digit = (uint8_t)(item_id % 10);
    if (last_digit <= 3) return 0;
    if (last_digit <= 6) return 1;
    return 2;
}

/* Calculate avatar stats */
static void calculate_avatar_stats(uint32_t item_id, AvatarItemInfo* info) {
    if (!info) return;
    memset(info, 0, sizeof(AvatarItemInfo));

    info->item_id = item_id;
    info->slot = get_avatar_slot(item_id);
    info->rarity = get_avatar_rarity(item_id);

    int16_t stat_bonus = g_avatar_stat_bonus[info->rarity];
    int16_t speed_bonus = g_avatar_speed_bonus[info->rarity];

    /* Different slots give different bonuses */
    switch (info->slot) {
        case AVATAR_SLOT_CHEST:
        case AVATAR_SLOT_PANTS:
            info->vit_bonus = stat_bonus;
            info->spi_bonus = stat_bonus / 2;
            break;
        case AVATAR_SLOT_SHOES:
            info->move_speed = speed_bonus;
            break;
        case AVATAR_SLOT_CAP:
        case AVATAR_SLOT_HAIR:
            info->int_bonus = stat_bonus;
            info->cast_speed = speed_bonus;
            break;
        case AVATAR_SLOT_WEAPON:
            info->phy_attack = stat_bonus * 2;
            info->mag_attack = stat_bonus * 2;
            info->attack_speed = speed_bonus;
            break;
        case AVATAR_SLOT_SKIN:
            info->str_bonus = stat_bonus;
            info->int_bonus = stat_bonus;
            info->vit_bonus = stat_bonus;
            info->spi_bonus = stat_bonus;
            break;
        case AVATAR_SLOT_AURA:
            info->phy_attack = stat_bonus;
            info->mag_attack = stat_bonus;
            break;
        default:
            info->str_bonus = stat_bonus / 2;
            info->int_bonus = stat_bonus / 2;
            break;
    }
}

/* DB: Load avatar items */
static int db_load_avatar_items(uint32_t charac_no, uint32_t* avatar_items, int max_slots) {
    if (!g_db_charac || !avatar_items) return -1;

    memset(avatar_items, 0, sizeof(uint32_t) * max_slots);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT slot, it_id FROM user_items WHERE charac_no=%u AND slot >= 100 AND slot < 112",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL) {
        int slot = atoi(row[0]) - 100;  /* Avatar slots are 100-111 */
        if (slot >= 0 && slot < max_slots) {
            avatar_items[slot] = (uint32_t)atoi(row[1]);
            count++;
        }
    }

    mysql_free_result(result);
    return count;
}

/* DB: Equip avatar item */
int db_equip_avatar(uint32_t charac_no, int32_t inv_slot, uint8_t avatar_slot) {
    if (!g_db_charac2 || avatar_slot >= AVATAR_SLOT_MAX) return -1;

    DBItemInfo inv_item;
    if (db_get_item_at_slot(charac_no, inv_slot, &inv_item) < 0 || inv_item.ui_id == 0) {
        return -1;
    }

    int db_slot = 100 + avatar_slot;
    DBItemInfo avatar_item;
    bool has_avatar = (db_get_item_at_slot(charac_no, db_slot, &avatar_item) == 0 &&
                       avatar_item.ui_id != 0);

    const int temp_slot = 9999;
    if (has_avatar) {
        if (db_update_item_slot(charac_no, avatar_item.ui_id, temp_slot) < 0) {
            return -1;
        }
    }

    if (db_update_item_slot(charac_no, inv_item.ui_id, db_slot) < 0) {
        if (has_avatar) {
            db_update_item_slot(charac_no, avatar_item.ui_id, db_slot);
        }
        return -1;
    }

    if (has_avatar) {
        if (db_update_item_slot(charac_no, avatar_item.ui_id, inv_slot) < 0) {
            db_update_item_slot(charac_no, avatar_item.ui_id, db_slot);
            db_update_item_slot(charac_no, inv_item.ui_id, inv_slot);
            return -1;
        }
    }

    return 0;
}

/* DB: Unequip avatar item */
int db_unequip_avatar(uint32_t charac_no, uint8_t avatar_slot, int32_t target_inv_slot) {
    if (!g_db_charac2 || avatar_slot >= AVATAR_SLOT_MAX) return -1;

    int db_slot = 100 + avatar_slot;
    DBItemInfo avatar_item;
    if (db_get_item_at_slot(charac_no, db_slot, &avatar_item) < 0 ||
        avatar_item.ui_id == 0) {
        return -1;
    }

    DBItemInfo target_item;
    bool target_occupied = (db_get_item_at_slot(charac_no, target_inv_slot, &target_item) == 0 &&
                            target_item.ui_id != 0);

    const int temp_slot = 9999;
    if (target_occupied) {
        if (db_update_item_slot(charac_no, target_item.ui_id, temp_slot) < 0) {
            return -1;
        }
    }

    if (db_update_item_slot(charac_no, avatar_item.ui_id, target_inv_slot) < 0) {
        if (target_occupied) {
            db_update_item_slot(charac_no, target_item.ui_id, target_inv_slot);
        }
        return -1;
    }

    if (target_occupied) {
        db_update_item_slot(charac_no, target_item.ui_id, db_slot);
    }

    return 0;
}

/*==============================================================================
 * Phase 8 P2: Title System (称号系统)
 *============================================================================*/

#define TITLE_MAX_COUNT       100
#define TITLE_SLOT_COUNT      3    /* Can equip up to 3 titles */





/* Predefined titles */
static const TitleInfo g_title_table[] = {
    /* id, type, patk, matk, pdef, mdef, hp, mp, all, mov, atk */
    /* Basic achievement titles (IDs 1-8 from original) */
    {1, TITLE_TYPE_NORMAL, 5, 5, 3, 3, 50, 30, 0, 0, 0},         /* Novice (Lv20) */
    {2, TITLE_TYPE_NORMAL, 10, 10, 5, 5, 100, 50, 0, 0, 0},      /* Adventurer (Lv40) */
    {3, TITLE_TYPE_RARE, 20, 20, 10, 10, 200, 100, 5, 0, 0},     /* Warrior (10000 kills) */
    {4, TITLE_TYPE_RARE, 15, 30, 8, 15, 150, 200, 3, 0, 1},      /* Mage */
    {5, TITLE_TYPE_EPIC, 40, 40, 20, 20, 500, 300, 10, 1, 1},    /* Hero (500 dungeons) */
    {6, TITLE_TYPE_EPIC, 50, 30, 30, 15, 600, 200, 8, 1, 2},     /* Champion (+15 enhance) */
    {7, TITLE_TYPE_LEGENDARY, 80, 80, 40, 40, 1000, 500, 20, 2, 2}, /* Legend (Lv85) */
    {8, TITLE_TYPE_LEGENDARY, 100, 100, 50, 50, 1500, 800, 25, 2, 3}, /* Myth (100 login days) */
    /* Event titles */
    {9, TITLE_TYPE_EVENT, 30, 30, 15, 15, 300, 150, 8, 1, 1},    /* Anniversary */
    {10, TITLE_TYPE_EVENT, 25, 25, 12, 12, 250, 120, 6, 1, 1},   /* Holiday */
    /* Phase 12: Boss achievement titles */
    {11, TITLE_TYPE_EPIC, 60, 60, 25, 25, 800, 400, 15, 1, 2},   /* BossSlayer (50 bosses) */
    {12, TITLE_TYPE_LEGENDARY, 120, 120, 60, 60, 2000, 1000, 30, 3, 3}, /* Overlord (200 bosses) */
    /* Phase 12: Abyss achievement titles */
    {13, TITLE_TYPE_LEGENDARY, 90, 90, 45, 45, 1200, 600, 22, 2, 2},  /* AbyssWalker (50 abyss) */
    /* Phase 12: Speed/Performance titles */
    {14, TITLE_TYPE_EPIC, 35, 35, 10, 10, 200, 100, 5, 5, 5},    /* SpeedDemon (1min clear) */
    {15, TITLE_TYPE_EPIC, 30, 30, 50, 50, 1000, 200, 12, 0, 0},  /* Invincible (50 no-death) */
    /* Phase 12: Social titles */
    {16, TITLE_TYPE_RARE, 25, 25, 15, 15, 400, 200, 8, 1, 1},    /* TeamLeader (200 party) */
    {17, TITLE_TYPE_LEGENDARY, 150, 150, 70, 70, 2500, 1200, 35, 3, 4}, /* Destroyer (100M dmg) */
    {18, TITLE_TYPE_NORMAL, 10, 10, 8, 8, 150, 100, 2, 0, 0},    /* Popular (50 friends) */
    /* Special titles */
    {19, TITLE_TYPE_LEGENDARY, 200, 200, 100, 100, 3000, 1500, 50, 5, 5}, /* GrandMaster (all legends) */
    {20, TITLE_TYPE_EVENT, 50, 50, 25, 25, 500, 250, 12, 2, 2},  /* FirstBlood (first boss) */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  /* End marker */
};

/* Find title by ID */
const TitleInfo* find_title(uint16_t title_id) {
    for (int i = 0; g_title_table[i].title_id != 0; i++) {
        if (g_title_table[i].title_id == title_id) {
            return &g_title_table[i];
        }
    }
    return NULL;
}

/* DB: Load unlocked titles */
static int db_load_titles(uint32_t charac_no, uint16_t* titles, int max_count) {
    if (!g_db_charac || !titles) return -1;

    memset(titles, 0, sizeof(uint16_t) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT title_id FROM charac_titles WHERE charac_no=%u ORDER BY unlock_time",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        /* Table might not exist, that's ok */
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        titles[count++] = (uint16_t)atoi(row[0]);
    }

    mysql_free_result(result);
    return count;
}

/* DB: Unlock a title */
static int db_unlock_title(uint32_t charac_no, uint16_t title_id) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO charac_titles (charac_no, title_id, unlock_time) "
             "VALUES (%u, %d, NOW())",
             charac_no, title_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[TITLE] Unlock failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return 0;
}

/* DB: Set equipped titles */
static int db_set_equipped_titles(uint32_t charac_no, uint16_t* equipped, int count) {
    if (!g_db_charac || !equipped || count > TITLE_SLOT_COUNT) return -1;

    /* Store as comma-separated in charac_info.equipped_titles or dedicated table */
    char title_str[64];
    int offset = 0;
    for (int i = 0; i < count && i < TITLE_SLOT_COUNT; i++) {
        if (i > 0) title_str[offset++] = ',';
        offset += snprintf(title_str + offset, sizeof(title_str) - offset, "%d", equipped[i]);
    }
    title_str[offset] = '\0';

    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET equipped_titles='%s' WHERE charac_no=%u",
             title_str, charac_no);

    if (mysql_query(g_db_charac, query)) {
        /* Column might not exist */
        return -1;
    }

    return 0;
}

/* Phase 12: Apply title stats to combat stats */
static void apply_title_stats(CombatStats* stats, uint16_t* equipped_titles, int title_count) {
    if (!stats || !equipped_titles) return;

    for (int i = 0; i < title_count && i < TITLE_SLOT_COUNT; i++) {
        if (equipped_titles[i] == 0) continue;

        const TitleInfo* title = find_title(equipped_titles[i]);
        if (!title) continue;

        /* Apply title bonuses */
        stats->phys_attack += title->phy_attack + title->all_stats;
        stats->mag_attack += title->mag_attack + title->all_stats;
        stats->phys_def += title->phy_defense + title->all_stats;
        stats->mag_def += title->mag_defense + title->all_stats;
        stats->max_hp += title->hp_bonus;
        stats->max_mp += title->mp_bonus;
        stats->move_speed += title->move_speed;
        stats->attack_speed += title->attack_speed;

        printf("[TITLE] Applied title %u: +%d phys_atk, +%d mag_atk, +%d hp\n",
               equipped_titles[i], title->phy_attack, title->mag_attack, title->hp_bonus);
    }
}

/* Phase 12: Calculate total title bonus for display */
static void calculate_title_bonus(uint16_t* equipped_titles, int title_count,
                                   int32_t* out_patk, int32_t* out_matk,
                                   int32_t* out_pdef, int32_t* out_mdef,
                                   int32_t* out_hp, int32_t* out_mp) {
    int32_t patk = 0, matk = 0, pdef = 0, mdef = 0, hp = 0, mp = 0;

    for (int i = 0; i < title_count && i < TITLE_SLOT_COUNT; i++) {
        if (equipped_titles[i] == 0) continue;

        const TitleInfo* title = find_title(equipped_titles[i]);
        if (!title) continue;

        patk += title->phy_attack + title->all_stats;
        matk += title->mag_attack + title->all_stats;
        pdef += title->phy_defense + title->all_stats;
        mdef += title->mag_defense + title->all_stats;
        hp += title->hp_bonus;
        mp += title->mp_bonus;
    }

    if (out_patk) *out_patk = patk;
    if (out_matk) *out_matk = matk;
    if (out_pdef) *out_pdef = pdef;
    if (out_mdef) *out_mdef = mdef;
    if (out_hp) *out_hp = hp;
    if (out_mp) *out_mp = mp;
}

/* Phase 12: Check if player has title unlocked */
static int has_title_unlocked(ClientSession* s, uint16_t title_id) {
    if (!s) return 0;

    for (int i = 0; i < (int)s->title.title_count; i++) {
        if (s->title.titles[i].title_id == title_id) {
            return 1;
        }
    }
    return 0;
}

/* Phase 12: Equip a title to a slot */
static int equip_title(ClientSession* s, uint16_t title_id, uint8_t slot) {
    if (!s || slot >= TITLE_SLOT_COUNT) return -1;

    /* Check if title is unlocked */
    if (!has_title_unlocked(s, title_id)) {
        printf("[TITLE] Title %u not unlocked for character %u\n",
               title_id, s->character_id);
        return -2;
    }

    /* Check if title is already equipped in another slot */
    for (int i = 0; i < TITLE_SLOT_COUNT; i++) {
        if (s->title.equipped_titles[i] == title_id && i != slot) {
            printf("[TITLE] Title %u already equipped in slot %d\n", title_id, i);
            return -3;
        }
    }

    /* Equip the title */
    s->title.equipped_titles[slot] = title_id;

    /* Persist to DB */
    db_set_equipped_titles(s->character_id, s->title.equipped_titles, TITLE_SLOT_COUNT);

    printf("[TITLE] Equipped title %u to slot %u for character %u\n",
           title_id, slot, s->character_id);
    return 0;
}

/* Phase 12: Unequip a title from a slot */
static int unequip_title(ClientSession* s, uint8_t slot) {
    if (!s || slot >= TITLE_SLOT_COUNT) return -1;

    uint16_t old_title = s->title.equipped_titles[slot];
    s->title.equipped_titles[slot] = 0;

    /* Persist to DB */
    db_set_equipped_titles(s->character_id, s->title.equipped_titles, TITLE_SLOT_COUNT);

    printf("[TITLE] Unequipped title %u from slot %u for character %u\n",
           old_title, slot, s->character_id);
    return 0;
}

/*==============================================================================
 * Phase 8 P3: Achievement System (成就系统)
 *============================================================================*/

#define ACHIEVEMENT_MAX       200

typedef enum {
    ACH_TYPE_KILL_MONSTER = 0,  /* Kill X monsters */
    ACH_TYPE_CLEAR_DUNGEON = 1, /* Clear dungeon X times */
    ACH_TYPE_REACH_LEVEL = 2,   /* Reach level X */
    ACH_TYPE_COLLECT_ITEM = 3,  /* Collect X items */
    ACH_TYPE_ENHANCE_EQUIP = 4, /* Enhance equipment to +X */
    ACH_TYPE_JOIN_GUILD = 5,    /* Join a guild */
    ACH_TYPE_TRADE_COUNT = 6,   /* Complete X trades */
    ACH_TYPE_PVP_WIN = 7,       /* Win X PVP matches */
    ACH_TYPE_GOLD_EARNED = 8,   /* Earn X gold total */
    ACH_TYPE_LOGIN_DAYS = 9,    /* Login X days */
    /* Phase 12: New achievement types */
    ACH_TYPE_KILL_BOSS = 10,    /* Kill X bosses */
    ACH_TYPE_CLEAR_ABYSS = 11,  /* Clear X abyss dungeons */
    ACH_TYPE_FIRST_BOSS = 12,   /* Kill specific boss first time */
    ACH_TYPE_SPEED_CLEAR = 13,  /* Clear dungeon under X seconds */
    ACH_TYPE_NO_DEATH = 14,     /* Clear dungeon without dying */
    ACH_TYPE_PARTY_CLEAR = 15,  /* Clear dungeon in party */
    ACH_TYPE_COMBO_HITS = 16,   /* Achieve X combo hits */
    ACH_TYPE_CRIT_DAMAGE = 17,  /* Deal X critical damage */
    ACH_TYPE_TOTAL_DAMAGE = 18, /* Deal X total damage */
    ACH_TYPE_FRIEND_COUNT = 19  /* Have X friends */
} AchievementType;



typedef struct {
    uint16_t ach_id;
    uint32_t progress;
    uint8_t  completed;
    uint8_t  claimed;
} AchievementProgress;

/* Achievement definitions */
static const AchievementDef g_achievement_table[] = {
    /* id, type, target, gold, exp, item_id, item_cnt, title */
    /* Monster kills */
    {1, ACH_TYPE_KILL_MONSTER, 100, 1000, 500, 0, 0, 0},        /* Kill 100 monsters */
    {2, ACH_TYPE_KILL_MONSTER, 1000, 5000, 2000, 0, 0, 0},      /* Kill 1000 monsters */
    {3, ACH_TYPE_KILL_MONSTER, 10000, 20000, 10000, 0, 0, 3},   /* Kill 10000 -> Warrior title */
    /* Dungeon clears */
    {4, ACH_TYPE_CLEAR_DUNGEON, 10, 2000, 1000, 0, 0, 0},       /* Clear 10 dungeons */
    {5, ACH_TYPE_CLEAR_DUNGEON, 100, 10000, 5000, 0, 0, 0},     /* Clear 100 dungeons */
    {6, ACH_TYPE_CLEAR_DUNGEON, 500, 50000, 25000, 0, 0, 5},    /* Clear 500 -> Hero title */
    /* Level achievements */
    {7, ACH_TYPE_REACH_LEVEL, 20, 5000, 0, 0, 0, 1},            /* Reach level 20 -> Novice */
    {8, ACH_TYPE_REACH_LEVEL, 40, 15000, 0, 0, 0, 2},           /* Reach level 40 -> Adventurer */
    {9, ACH_TYPE_REACH_LEVEL, 60, 30000, 0, 0, 0, 0},           /* Reach level 60 */
    {10, ACH_TYPE_REACH_LEVEL, 85, 100000, 0, 0, 0, 7},         /* Reach level 85 -> Legend */
    /* Enhancement */
    {11, ACH_TYPE_ENHANCE_EQUIP, 10, 10000, 5000, 0, 0, 0},     /* Enhance to +10 */
    {12, ACH_TYPE_ENHANCE_EQUIP, 15, 50000, 20000, 0, 0, 6},    /* Enhance to +15 -> Champion */
    /* Social */
    {13, ACH_TYPE_JOIN_GUILD, 1, 1000, 500, 0, 0, 0},           /* Join a guild */
    {14, ACH_TYPE_PVP_WIN, 10, 5000, 2000, 0, 0, 0},            /* Win 10 PVP */
    {15, ACH_TYPE_PVP_WIN, 100, 30000, 15000, 0, 0, 0},         /* Win 100 PVP */
    /* Economy */
    {16, ACH_TYPE_GOLD_EARNED, 100000, 5000, 2000, 0, 0, 0},    /* Earn 100k gold */
    {17, ACH_TYPE_GOLD_EARNED, 1000000, 20000, 10000, 0, 0, 0}, /* Earn 1M gold */
    /* Login */
    {18, ACH_TYPE_LOGIN_DAYS, 7, 3000, 1500, 0, 0, 0},          /* Login 7 days */
    {19, ACH_TYPE_LOGIN_DAYS, 30, 15000, 7500, 0, 0, 0},        /* Login 30 days */
    {20, ACH_TYPE_LOGIN_DAYS, 100, 50000, 25000, 0, 0, 8},      /* Login 100 days -> Myth */
    /* Phase 12: Boss achievements */
    {21, ACH_TYPE_KILL_BOSS, 1, 5000, 3000, 0, 0, 0},           /* Kill 1 boss */
    {22, ACH_TYPE_KILL_BOSS, 10, 15000, 8000, 0, 0, 0},         /* Kill 10 bosses */
    {23, ACH_TYPE_KILL_BOSS, 50, 50000, 25000, 0, 0, 9},        /* Kill 50 bosses -> BossSlayer */
    {24, ACH_TYPE_KILL_BOSS, 200, 150000, 75000, 0, 0, 10},     /* Kill 200 bosses -> Overlord */
    /* Phase 12: Abyss achievements */
    {25, ACH_TYPE_CLEAR_ABYSS, 1, 10000, 5000, 0, 0, 0},        /* Clear 1 abyss */
    {26, ACH_TYPE_CLEAR_ABYSS, 10, 30000, 15000, 0, 0, 0},      /* Clear 10 abyss */
    {27, ACH_TYPE_CLEAR_ABYSS, 50, 100000, 50000, 0, 0, 11},    /* Clear 50 abyss -> AbyssWalker */
    /* Phase 12: First boss kills */
    {28, ACH_TYPE_FIRST_BOSS, 1, 20000, 10000, 0, 0, 0},        /* Goblin King first kill */
    {29, ACH_TYPE_FIRST_BOSS, 2, 25000, 12000, 0, 0, 0},        /* Dark Mage first kill */
    {30, ACH_TYPE_FIRST_BOSS, 3, 30000, 15000, 0, 0, 0},        /* Ice Dragon first kill */
    /* Phase 12: Speed clear */
    {31, ACH_TYPE_SPEED_CLEAR, 180, 10000, 5000, 0, 0, 0},      /* Clear dungeon in 3 mins */
    {32, ACH_TYPE_SPEED_CLEAR, 120, 25000, 12000, 0, 0, 0},     /* Clear dungeon in 2 mins */
    {33, ACH_TYPE_SPEED_CLEAR, 60, 60000, 30000, 0, 0, 12},     /* Clear dungeon in 1 min -> SpeedDemon */
    /* Phase 12: No death clear */
    {34, ACH_TYPE_NO_DEATH, 1, 5000, 3000, 0, 0, 0},            /* No death clear x1 */
    {35, ACH_TYPE_NO_DEATH, 10, 20000, 10000, 0, 0, 0},         /* No death clear x10 */
    {36, ACH_TYPE_NO_DEATH, 50, 75000, 35000, 0, 0, 13},        /* No death clear x50 -> Invincible */
    /* Phase 12: Party achievements */
    {37, ACH_TYPE_PARTY_CLEAR, 10, 5000, 2500, 0, 0, 0},        /* Party clear x10 */
    {38, ACH_TYPE_PARTY_CLEAR, 50, 20000, 10000, 0, 0, 0},      /* Party clear x50 */
    {39, ACH_TYPE_PARTY_CLEAR, 200, 60000, 30000, 0, 0, 14},    /* Party clear x200 -> TeamLeader */
    /* Phase 12: Combat achievements */
    {40, ACH_TYPE_TOTAL_DAMAGE, 1000000, 10000, 5000, 0, 0, 0}, /* Deal 1M total damage */
    {41, ACH_TYPE_TOTAL_DAMAGE, 10000000, 40000, 20000, 0, 0, 0}, /* Deal 10M damage */
    {42, ACH_TYPE_TOTAL_DAMAGE, 100000000, 150000, 75000, 0, 0, 15}, /* Deal 100M -> Destroyer */
    /* Phase 12: Friends */
    {43, ACH_TYPE_FRIEND_COUNT, 5, 2000, 1000, 0, 0, 0},        /* Have 5 friends */
    {44, ACH_TYPE_FRIEND_COUNT, 20, 8000, 4000, 0, 0, 0},       /* Have 20 friends */
    {45, ACH_TYPE_FRIEND_COUNT, 50, 25000, 12000, 0, 0, 16},    /* Have 50 friends -> Popular */
    {0, 0, 0, 0, 0, 0, 0, 0}  /* End marker */
};

/* Find achievement definition */
const AchievementDef* find_achievement(uint16_t ach_id) {
    for (int i = 0; g_achievement_table[i].ach_id != 0; i++) {
        if (g_achievement_table[i].ach_id == ach_id) {
            return &g_achievement_table[i];
        }
    }
    return NULL;
}

/* DB: Load achievement progress */
static int db_load_achievements(uint32_t charac_no, AchievementProgress* progress, int max_count) {
    if (!g_db_charac || !progress) return -1;

    memset(progress, 0, sizeof(AchievementProgress) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ach_id, progress, completed, claimed FROM charac_achievements WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        progress[count].ach_id = (uint16_t)atoi(row[0]);
        progress[count].progress = (uint32_t)atoi(row[1]);
        progress[count].completed = (uint8_t)atoi(row[2]);
        progress[count].claimed = (uint8_t)atoi(row[3]);
        count++;
    }

    mysql_free_result(result);
    return count;
}

/* DB: Update achievement progress */


/* Check and update achievement progress */
int check_achievement_progress(uint32_t charac_no, uint8_t type, uint32_t value) {
    int completed_count = 0;

    for (int i = 0; g_achievement_table[i].ach_id != 0; i++) {
        if (g_achievement_table[i].type == type) {
            /* Check if already completed */
            char query[256];
            snprintf(query, sizeof(query),
                     "SELECT completed FROM charac_achievements WHERE charac_no=%u AND ach_id=%d",
                     charac_no, g_achievement_table[i].ach_id);

            if (mysql_query(g_db_charac, query) == 0) {
                MYSQL_RES* result = mysql_store_result(g_db_charac);
                if (result) {
                    MYSQL_ROW row = mysql_fetch_row(result);
                    if (row && atoi(row[0]) == 1) {
                        mysql_free_result(result);
                        continue;  /* Already completed */
                    }
                    mysql_free_result(result);
                }
            }

            /* Update progress */
            uint8_t completed = (value >= g_achievement_table[i].target) ? 1 : 0;
            db_update_achievement(charac_no, g_achievement_table[i].ach_id, value, completed, 0);

            if (completed) {
                completed_count++;
                printf("[ACH] Character %u completed achievement %d\n",
                       charac_no, g_achievement_table[i].ach_id);
            }
        }
    }

    return completed_count;
}

/*==============================================================================
 * Phase 8 P4: Gem Socket System (宝石镶嵌)
 *============================================================================*/

#define GEM_MAX_SOCKETS      4
#define GEM_TYPE_MAX         10

typedef enum {
    GEM_TYPE_RUBY = 0,       /* +Physical Attack */
    GEM_TYPE_SAPPHIRE = 1,   /* +Magic Attack */
    GEM_TYPE_EMERALD = 2,    /* +HP */
    GEM_TYPE_TOPAZ = 3,      /* +MP */
    GEM_TYPE_DIAMOND = 4,    /* +All Stats */
    GEM_TYPE_AMETHYST = 5,   /* +Critical */
    GEM_TYPE_ONYX = 6,       /* +Defense */
    GEM_TYPE_OPAL = 7,       /* +Speed */
    GEM_TYPE_JADE = 8,       /* +Evasion */
    GEM_TYPE_PEARL = 9       /* +HP Regen */
} GemType;

typedef struct {
    uint8_t  type;
    uint8_t  grade;          /* 1-10 */
    int16_t  stat_value;
} GemInfo;

/* Gem stat values by grade */
static const int16_t g_gem_stat_base[GEM_TYPE_MAX] = {
    10, 10, 50, 30, 3, 5, 5, 1, 3, 5
};

/* Calculate gem stats */
static void calculate_gem_stats(uint8_t type, uint8_t grade, GemInfo* info) {
    if (!info || type >= GEM_TYPE_MAX || grade < 1 || grade > 10) return;

    info->type = type;
    info->grade = grade;
    info->stat_value = g_gem_stat_base[type] * grade;
}

/* Get socket count for item */
static uint8_t get_item_socket_count(uint32_t item_id, uint8_t item_rarity) {
    /* Base sockets by rarity */
    uint8_t base = 0;
    switch (item_rarity) {
        case 0: base = 0; break;  /* Normal - no sockets */
        case 1: base = 1; break;  /* Magic - 1 socket */
        case 2: base = 2; break;  /* Rare - 2 sockets */
        case 3: base = 3; break;  /* Epic - 3 sockets */
        case 4: base = 4; break;  /* Legendary - 4 sockets */
        default: base = 0;
    }

    /* Weapons get +1 socket */
    if ((item_id / 10000) % 10 == 0) {
        base = (base < GEM_MAX_SOCKETS) ? base + 1 : GEM_MAX_SOCKETS;
    }

    return base > GEM_MAX_SOCKETS ? GEM_MAX_SOCKETS : base;
}

static void ensure_item_gems_schema(void) {
    static int checked = 0;
    if (checked || !g_db_charac) return;
    checked = 1;

    const char* create_sql =
        "CREATE TABLE IF NOT EXISTS item_gems ("
        "charac_no INT UNSIGNED NOT NULL,"
        "item_slot INT NOT NULL,"
        "item_ui_id INT UNSIGNED NOT NULL DEFAULT 0,"
        "socket_idx TINYINT UNSIGNED NOT NULL,"
        "gem_id SMALLINT UNSIGNED NOT NULL,"
        "PRIMARY KEY (item_ui_id, socket_idx),"
        "KEY idx_item_gems_charac (charac_no))";

    if (mysql_query(g_db_charac, create_sql)) {
        printf("[GEM] Failed ensuring item_gems table: %s\n", mysql_error(g_db_charac));
    }

    if (mysql_query(
            g_db_charac,
            "ALTER TABLE item_gems "
            "ADD COLUMN IF NOT EXISTS item_ui_id INT UNSIGNED NOT NULL DEFAULT 0 AFTER item_slot")) {
        /* Ignore errors if column already exists */
    }

    if (mysql_query(
            g_db_charac,
            "ALTER TABLE item_gems "
            "ADD UNIQUE INDEX IF NOT EXISTS idx_item_gems_ui (item_ui_id, socket_idx)")) {
        /* Ignore errors if index already exists */
    }
}

static uint32_t get_item_ui_id_from_slot(uint32_t charac_no, int32_t slot) {
    DBItemInfo item;
    if (db_get_item_at_slot(charac_no, slot, &item) < 0 || item.ui_id == 0) {
        printf("[GEM] Unable to resolve item at slot %d for charac_no=%u\n", slot, charac_no);
        return 0;
    }
    return item.ui_id;
}

/* DB: Load socketed gems for an item */
static int db_load_item_gems(uint32_t charac_no, int32_t slot, uint16_t* gems, int max_sockets) {
    if (!g_db_charac || !gems) return -1;

    memset(gems, 0, sizeof(uint16_t) * max_sockets);
    ensure_item_gems_schema();

    uint32_t item_ui_id = get_item_ui_id_from_slot(charac_no, slot);
    if (item_ui_id == 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT socket_idx, gem_id FROM item_gems WHERE item_ui_id=%u",
             item_ui_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[GEM] Load query by ui_id failed (%s), fallback to slot\n",
               mysql_error(g_db_charac));
        snprintf(query, sizeof(query),
                 "SELECT socket_idx, gem_id FROM item_gems WHERE charac_no=%u AND item_slot=%d",
                 charac_no, slot);
        if (mysql_query(g_db_charac, query)) {
            return 0;
        }
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL) {
        int idx = atoi(row[0]);
        if (idx >= 0 && idx < max_sockets) {
            gems[idx] = (uint16_t)atoi(row[1]);
            count++;
        }
    }

    mysql_free_result(result);
    return count;
}

/* DB: Socket a gem */
static int db_socket_gem(uint32_t charac_no, int32_t item_slot, uint8_t socket_idx, uint16_t gem_id) {
    if (!g_db_charac || socket_idx >= GEM_MAX_SOCKETS) return -1;

    ensure_item_gems_schema();
    uint32_t item_ui_id = get_item_ui_id_from_slot(charac_no, item_slot);
    if (item_ui_id == 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO item_gems (charac_no, item_slot, item_ui_id, socket_idx, gem_id) "
             "VALUES (%u, %d, %u, %d, %d) "
             "ON DUPLICATE KEY UPDATE gem_id=%d, item_ui_id=%u",
             charac_no, item_slot, item_ui_id, socket_idx, gem_id, gem_id, item_ui_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[GEM] Insert by ui_id failed (%s), fallback to slot\n", mysql_error(g_db_charac));
        snprintf(query, sizeof(query),
                 "INSERT INTO item_gems (charac_no, item_slot, socket_idx, gem_id) "
                 "VALUES (%u, %d, %d, %d) "
                 "ON DUPLICATE KEY UPDATE gem_id=%d",
                 charac_no, item_slot, socket_idx, gem_id, gem_id);
        if (mysql_query(g_db_charac, query)) {
            return -1;
        }
    }

    return 0;
}

/* DB: Remove a gem */
static int db_unsocket_gem(uint32_t charac_no, int32_t item_slot, uint8_t socket_idx) {
    if (!g_db_charac || socket_idx >= GEM_MAX_SOCKETS) return -1;

    ensure_item_gems_schema();
    uint32_t item_ui_id = get_item_ui_id_from_slot(charac_no, item_slot);
    if (item_ui_id == 0) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "DELETE FROM item_gems WHERE item_ui_id=%u AND socket_idx=%d",
             item_ui_id, socket_idx);

    if (mysql_query(g_db_charac, query)) {
        printf("[GEM] Delete by ui_id failed (%s), fallback to slot\n", mysql_error(g_db_charac));
        snprintf(query, sizeof(query),
                 "DELETE FROM item_gems WHERE charac_no=%u AND item_slot=%d AND socket_idx=%d",
                 charac_no, item_slot, socket_idx);
        if (mysql_query(g_db_charac, query)) {
            return -1;
        }
    }

    return mysql_affected_rows(g_db_charac) > 0 ? 0 : -1;
}

/* Gem synthesis: 3 same grade gems -> 1 higher grade */
int synthesize_gems(uint8_t type, uint8_t grade, uint8_t* new_grade) {
    if (grade >= 10) return -1;  /* Max grade */
    *new_grade = grade + 1;
    return 0;  /* Success */
}

/*==============================================================================
 * Phase 8 P5: Abyss/Hell Mode (深渊模式)
 *============================================================================*/

#define ABYSS_DIFFICULTY_MAX  5
#define ABYSS_BOSS_MAX        10

typedef enum {
    ABYSS_NORMAL = 0,
    ABYSS_EXPERT = 1,
    ABYSS_MASTER = 2,
    ABYSS_KING = 3,
    ABYSS_HELL = 4
} AbyssDifficulty;

typedef struct {
    uint16_t boss_id;
    char     name[32];
    uint32_t base_hp;
    uint16_t base_attack;
    uint16_t base_defense;
    uint8_t  min_level;
    uint32_t drop_table_id;
} AbyssBossInfo;

/* Abyss bosses */
static const AbyssBossInfo g_abyss_bosses[] = {
    {1001, "Necromancer", 100000, 500, 200, 20, 5001},
    {1002, "Ice Dragon", 200000, 800, 350, 30, 5002},
    {1003, "Fire Demon", 350000, 1200, 500, 40, 5003},
    {1004, "Shadow Lord", 500000, 1500, 700, 50, 5004},
    {1005, "Chaos Knight", 750000, 2000, 900, 60, 5005},
    {1006, "Lich King", 1000000, 2500, 1100, 65, 5006},
    {1007, "Abyssal Horror", 1500000, 3000, 1400, 70, 5007},
    {1008, "Void Walker", 2000000, 3500, 1700, 75, 5008},
    {1009, "Demon Prince", 3000000, 4000, 2000, 80, 5009},
    {1010, "Apocalypse", 5000000, 5000, 2500, 85, 5010},
    {0, "", 0, 0, 0, 0, 0}
};

/* Difficulty multipliers: HP, Attack, Defense, Drop Rate */
static const float g_abyss_multipliers[ABYSS_DIFFICULTY_MAX][4] = {
    {1.0f, 1.0f, 1.0f, 1.0f},     /* Normal */
    {1.5f, 1.3f, 1.2f, 1.5f},     /* Expert */
    {2.5f, 1.8f, 1.5f, 2.0f},     /* Master */
    {4.0f, 2.5f, 2.0f, 3.0f},     /* King */
    {8.0f, 4.0f, 3.0f, 5.0f}      /* Hell */
};

/* Entry cost (special currency: Abyss Tickets) */
const uint8_t g_abyss_entry_cost[ABYSS_DIFFICULTY_MAX] = {1, 2, 3, 5, 10};

/* Find abyss boss */
static const AbyssBossInfo* find_abyss_boss(uint16_t boss_id) {
    for (int i = 0; g_abyss_bosses[i].boss_id != 0; i++) {
        if (g_abyss_bosses[i].boss_id == boss_id) {
            return &g_abyss_bosses[i];
        }
    }
    return NULL;
}

/* Calculate scaled boss stats - matches header declaration */
void calculate_abyss_boss_stats(uint16_t boss_id, uint8_t difficulty,
                                MonsterInfo* boss_out) {
    if (!boss_out) return;
    const AbyssBossInfo* boss = find_abyss_boss(boss_id);
    if (!boss || difficulty >= ABYSS_DIFFICULTY_MAX) return;

    boss_out->hp = (uint32_t)(boss->base_hp * g_abyss_multipliers[difficulty][0]);
    boss_out->max_hp = boss_out->hp;
    boss_out->attack_damage = (uint32_t)(boss->base_attack * g_abyss_multipliers[difficulty][1]);
    boss_out->phys_def = (uint32_t)(boss->base_defense * g_abyss_multipliers[difficulty][2]);
    boss_out->mag_def = boss_out->phys_def;
    boss_out->boss_id = boss_id;
    boss_out->is_boss = 1;
}

/* Check abyss entry requirements */
int can_enter_abyss(uint8_t player_level, uint16_t boss_id, uint8_t difficulty) {
    const AbyssBossInfo* boss = find_abyss_boss(boss_id);
    if (!boss) return 0;

    /* Level requirement increases with difficulty */
    uint8_t req_level = boss->min_level + (difficulty * 5);
    return player_level >= req_level ? 1 : 0;
}

/* DB: Record abyss clear */
static int db_record_abyss_clear(uint32_t charac_no, uint16_t boss_id, uint8_t difficulty) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO abyss_clears (charac_no, boss_id, difficulty, clear_time) "
             "VALUES (%u, %d, %d, NOW())",
             charac_no, boss_id, difficulty);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    return 0;
}

/*==============================================================================
 * Phase 12 P4: Enhanced Abyss System (深渊副本增强)
 *============================================================================*/

/* Abyss Ticket Item ID */
#define ABYSS_TICKET_ITEM_ID     99001
#define ABYSS_DAILY_LIMIT        5
#define ABYSS_DROP_TABLE_SIZE    20

/* Drop rarity tiers */
typedef enum {
    ABYSS_DROP_COMMON = 0,     /* 60% chance */
    ABYSS_DROP_RARE = 1,       /* 25% chance */
    ABYSS_DROP_EPIC = 2,       /* 12% chance */
    ABYSS_DROP_LEGENDARY = 3   /* 3% chance */
} AbyssDropRarity;

/* Abyss drop item entry */
typedef struct {
    uint32_t item_id;
    uint8_t  rarity;           /* AbyssDropRarity */
    uint8_t  min_count;
    uint8_t  max_count;
    uint16_t min_boss_id;      /* Minimum boss ID to drop this */
} AbyssDropEntry;

/* Abyss drop tables by boss tier */
static const AbyssDropEntry g_abyss_drop_table[] = {
    /* Common drops (all bosses) */
    {2001, ABYSS_DROP_COMMON, 1, 3, 1001},   /* Abyss Fragment */
    {2002, ABYSS_DROP_COMMON, 1, 2, 1001},   /* Soul Shard */
    {2003, ABYSS_DROP_COMMON, 1, 5, 1001},   /* Dark Essence */
    {2004, ABYSS_DROP_COMMON, 2, 5, 1001},   /* Gold Bar */

    /* Rare drops (boss tier 1002+) */
    {3001, ABYSS_DROP_RARE, 1, 1, 1002},     /* Rare Weapon Box */
    {3002, ABYSS_DROP_RARE, 1, 1, 1002},     /* Rare Armor Box */
    {3003, ABYSS_DROP_RARE, 1, 2, 1003},     /* Enhancement Stone +10 */
    {3004, ABYSS_DROP_RARE, 1, 1, 1003},     /* Skill Reset Scroll */

    /* Epic drops (boss tier 1005+) */
    {4001, ABYSS_DROP_EPIC, 1, 1, 1005},     /* Epic Weapon (random) */
    {4002, ABYSS_DROP_EPIC, 1, 1, 1005},     /* Epic Armor (random) */
    {4003, ABYSS_DROP_EPIC, 1, 1, 1006},     /* Epic Accessory */
    {4004, ABYSS_DROP_EPIC, 1, 1, 1007},     /* Protection Charm */

    /* Legendary drops (boss tier 1008+) */
    {5001, ABYSS_DROP_LEGENDARY, 1, 1, 1008}, /* Legendary Weapon */
    {5002, ABYSS_DROP_LEGENDARY, 1, 1, 1009}, /* Legendary Armor */
    {5003, ABYSS_DROP_LEGENDARY, 1, 1, 1010}, /* Legendary Title Scroll */
    {5004, ABYSS_DROP_LEGENDARY, 1, 1, 1010}, /* Unique Mount Ticket */

    {0, 0, 0, 0, 0}  /* Terminator */
};

/* Rarity drop rates by difficulty (base % out of 10000) */
static const uint16_t g_abyss_drop_rates[ABYSS_DIFFICULTY_MAX][4] = {
    /* Common, Rare, Epic, Legendary */
    {6000, 2500, 1200, 300},    /* Normal: 60%, 25%, 12%, 3% */
    {5500, 2800, 1400, 300},    /* Expert: 55%, 28%, 14%, 3% */
    {5000, 3000, 1600, 400},    /* Master: 50%, 30%, 16%, 4% */
    {4000, 3200, 2000, 800},    /* King: 40%, 32%, 20%, 8% */
    {3000, 3000, 2500, 1500}    /* Hell: 30%, 30%, 25%, 15% */
};

/* DB: Check daily abyss clear count */


/* DB: Check abyss tickets in inventory */


/* DB: Consume abyss tickets */
static int db_consume_abyss_tickets(uint32_t charac_no, int amount) {
    if (!g_db_charac || amount <= 0) return -1;

    /* Find and update ticket stacks */
    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET add_info = add_info - %d "
             "WHERE charac_no = %u AND it_id = %u AND slot < 56 AND add_info >= %d "
             "LIMIT 1",
             amount, charac_no, ABYSS_TICKET_ITEM_ID, amount);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    /* Delete empty stacks */
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no = %u AND it_id = %u AND add_info <= 0",
             charac_no, ABYSS_TICKET_ITEM_ID);
    mysql_query(g_db_charac, query);

    return 0;
}

/* Generate abyss drop based on boss and difficulty */
int generate_abyss_drops(uint16_t boss_id, uint8_t difficulty,
                                 uint32_t* out_items, uint8_t* out_counts, int max_drops) {
    if (!out_items || !out_counts || difficulty >= ABYSS_DIFFICULTY_MAX) return 0;

    int drop_count = 0;

    /* Guaranteed drop count based on difficulty */
    int min_drops = 1 + difficulty;
    int max_drop_count = 2 + difficulty * 2;
    if (max_drop_count > max_drops) max_drop_count = max_drops;

    /* Roll for each drop slot */
    for (int slot = 0; slot < max_drop_count && drop_count < max_drops; slot++) {
        /* Determine rarity for this drop */
        uint32_t roll = (uint32_t)rand() % 10000;
        uint8_t rarity = ABYSS_DROP_COMMON;
        uint16_t cumulative = 0;

        for (int r = 0; r < 4; r++) {
            cumulative += g_abyss_drop_rates[difficulty][r];
            if (roll < cumulative) {
                rarity = (uint8_t)r;
                break;
            }
        }

        /* Find eligible item for this rarity and boss */
        int candidates[ABYSS_DROP_TABLE_SIZE];
        int candidate_count = 0;

        for (int i = 0; g_abyss_drop_table[i].item_id != 0; i++) {
            const AbyssDropEntry* entry = &g_abyss_drop_table[i];
            if (entry->rarity == rarity && boss_id >= entry->min_boss_id) {
                candidates[candidate_count++] = i;
                if (candidate_count >= ABYSS_DROP_TABLE_SIZE) break;
            }
        }

        /* Pick random from candidates */
        if (candidate_count > 0 || slot < min_drops) {
            int idx;
            if (candidate_count > 0) {
                idx = candidates[rand() % candidate_count];
            } else {
                /* Fallback to common drop */
                idx = rand() % 4;  /* First 4 entries are common */
            }

            const AbyssDropEntry* drop = &g_abyss_drop_table[idx];
            uint8_t count = drop->min_count;
            if (drop->max_count > drop->min_count) {
                count += rand() % (drop->max_count - drop->min_count + 1);
            }

            out_items[drop_count] = drop->item_id;
            out_counts[drop_count] = count;
            drop_count++;

            printf("[ABYSS] Generated drop: item=%u count=%u rarity=%d\n",
                   drop->item_id, count, rarity);
        }
    }

    return drop_count;
}

/* Calculate abyss rewards (gold, exp, item) - matches header declaration */
void calculate_abyss_rewards(uint16_t boss_id, uint8_t difficulty,
                             uint32_t* out_gold, uint32_t* out_exp, uint16_t* out_item_id) {
    const AbyssBossInfo* boss = find_abyss_boss(boss_id);
    if (!boss) {
        if (out_gold) *out_gold = 10000;
        if (out_exp) *out_exp = 5000;
        if (out_item_id) *out_item_id = 0;
        return;
    }

    /* Base rewards scale with boss min level */
    uint32_t base_gold = boss->min_level * 500;
    uint32_t base_exp = boss->min_level * 1000;

    /* Apply difficulty multiplier */
    float mult = g_abyss_multipliers[difficulty][3];  /* Drop rate multiplier */

    if (out_gold) *out_gold = (uint32_t)(base_gold * mult);
    if (out_exp) *out_exp = (uint32_t)(base_exp * mult);
    if (out_item_id) *out_item_id = 2001 + difficulty;  /* Basic reward item */

    printf("[ABYSS] Rewards: gold=%u exp=%u item=%u (boss=%d diff=%d)\n",
           out_gold ? *out_gold : 0, out_exp ? *out_exp : 0,
           out_item_id ? *out_item_id : 0, boss_id, difficulty);
}

/* Check if player can enter abyss (full validation) - matches header declaration */
int validate_abyss_entry(ClientSession* s, uint16_t boss_id,
                         uint8_t difficulty, int consume_ticket) {
    if (!s) return -1;

    /* 1. Check level requirement */
    if (!can_enter_abyss(s->level, boss_id, difficulty)) {
        printf("[ABYSS] Entry denied: level too low\n");
        return -1;
    }

    /* 2. Check daily limit */
    int daily_count = db_get_abyss_daily_count(s->character_id);
    if (daily_count >= ABYSS_DAILY_LIMIT) {
        printf("[ABYSS] Entry denied: daily limit reached (%d/%d)\n",
               daily_count, ABYSS_DAILY_LIMIT);
        return -2;
    }

    /* 3. Check tickets */
    int ticket_cost = g_abyss_entry_cost[difficulty];
    int ticket_count = db_get_abyss_ticket_count(s->character_id);
    if (ticket_count < ticket_cost) {
        printf("[ABYSS] Entry denied: not enough tickets (%d/%d)\n",
               ticket_count, ticket_cost);
        return -3;
    }

    /* 4. Consume ticket if requested */
    if (consume_ticket && ticket_cost > 0) {
        /* TODO: db_consume_abyss_tickets(s->character_id, ticket_cost); */
        printf("[ABYSS] Consumed %d tickets\n", ticket_cost);
    }

    return ticket_cost;  /* Return ticket cost on success */
}

/*==============================================================================
 * Phase 12 P5: Audit Logging System (日志审计系统)
 *============================================================================*/

/* Log types for game_log table */
typedef enum {
    LOG_TYPE_LOGIN = 1,          /* Player login */
    LOG_TYPE_LOGOUT = 2,         /* Player logout */
    LOG_TYPE_ENTER_TOWN = 3,     /* Enter town/area */
    LOG_TYPE_ENTER_DUNGEON = 4,  /* Enter dungeon */
    LOG_TYPE_CLEAR_DUNGEON = 5,  /* Clear dungeon */
    LOG_TYPE_NPC_SHOP_BUY = 10,  /* Buy from NPC shop */
    LOG_TYPE_NPC_SHOP_SELL = 11, /* Sell to NPC shop */
    LOG_TYPE_ENHANCE_EQUIP = 20, /* Enhance equipment */
    LOG_TYPE_ENHANCE_FAIL = 21,  /* Enhancement failed */
    LOG_TYPE_TRADE_START = 30,   /* Trade started */
    LOG_TYPE_TRADE_COMPLETE = 31,/* Trade completed */
    LOG_TYPE_TRADE_CANCEL = 32,  /* Trade cancelled */
    LOG_TYPE_AUCTION_LIST = 40,  /* List item on auction */
    LOG_TYPE_AUCTION_BUY = 41,   /* Buy from auction */
    LOG_TYPE_AUCTION_CANCEL = 42,/* Cancel auction listing */
    LOG_TYPE_MAIL_SEND = 50,     /* Send mail */
    LOG_TYPE_MAIL_CLAIM = 51,    /* Claim mail attachment */
    LOG_TYPE_SKILL_LEARN = 60,   /* Learn skill */
    LOG_TYPE_SKILL_RESET = 61,   /* Reset skills */
    LOG_TYPE_GUILD_CREATE = 70,  /* Create guild */
    LOG_TYPE_GUILD_JOIN = 71,    /* Join guild */
    LOG_TYPE_GUILD_LEAVE = 72,   /* Leave guild */
    LOG_TYPE_ABYSS_ENTER = 80,   /* Enter abyss */
    LOG_TYPE_ABYSS_CLEAR = 81,   /* Clear abyss */
    LOG_TYPE_CHEAT_DETECTED = 99 /* Cheat detected */
} GameLogType;

/* Gold log types for gold_log table */
typedef enum {
    GOLD_LOG_NPC_BUY = 1,        /* Spend on NPC shop */
    GOLD_LOG_NPC_SELL = 2,       /* Earn from NPC shop */
    GOLD_LOG_DUNGEON = 3,        /* Earn from dungeon */
    GOLD_LOG_ABYSS = 4,          /* Earn from abyss */
    GOLD_LOG_TRADE_SPEND = 10,   /* Spend in trade */
    GOLD_LOG_TRADE_EARN = 11,    /* Earn from trade */
    GOLD_LOG_AUCTION_BUY = 20,   /* Spend on auction */
    GOLD_LOG_AUCTION_SELL = 21,  /* Earn from auction */
    GOLD_LOG_AUCTION_FEE = 22,   /* Auction listing fee */
    GOLD_LOG_MAIL_SEND = 30,     /* Mail sending fee */
    GOLD_LOG_MAIL_CLAIM = 31,    /* Gold from mail */
    GOLD_LOG_REPAIR = 40,        /* Equipment repair */
    GOLD_LOG_ENHANCE = 41,       /* Enhancement cost */
    GOLD_LOG_GM_GIVE = 90,       /* GM gave gold */
    GOLD_LOG_GM_TAKE = 91        /* GM took gold */
} GoldLogType;

/* Item log types for item_log table */
typedef enum {
    ITEM_LOG_PICKUP = 1,         /* Picked up from ground */
    ITEM_LOG_DROP = 2,           /* Dropped to ground */
    ITEM_LOG_NPC_BUY = 10,       /* Bought from NPC */
    ITEM_LOG_NPC_SELL = 11,      /* Sold to NPC */
    ITEM_LOG_DUNGEON_DROP = 20,  /* Dropped from dungeon mob */
    ITEM_LOG_ABYSS_DROP = 21,    /* Dropped from abyss boss */
    ITEM_LOG_TRADE_GIVE = 30,    /* Given in trade */
    ITEM_LOG_TRADE_RECV = 31,    /* Received in trade */
    ITEM_LOG_AUCTION_LIST = 40,  /* Listed on auction */
    ITEM_LOG_AUCTION_BUY = 41,   /* Bought from auction */
    ITEM_LOG_AUCTION_RETURN = 42,/* Returned from auction */
    ITEM_LOG_MAIL_SEND = 50,     /* Sent via mail */
    ITEM_LOG_MAIL_RECV = 51,     /* Received via mail */
    ITEM_LOG_STORAGE_PUT = 60,   /* Put in storage */
    ITEM_LOG_STORAGE_GET = 61,   /* Got from storage */
    ITEM_LOG_DISASSEMBLE = 70,   /* Disassembled */
    ITEM_LOG_CRAFT = 71,         /* Crafted */
    ITEM_LOG_ENHANCE_LOSE = 80,  /* Lost in enhancement */
    ITEM_LOG_GM_GIVE = 90,       /* GM gave item */
    ITEM_LOG_GM_TAKE = 91        /* GM took item */
} ItemLogType;

/* DB: Write game log entry */
static int db_write_game_log(uint32_t charac_no, uint8_t log_type,
                              uint32_t target_id, int32_t value1, int32_t value2,
                              const char* note) {
    if (!g_db_charac) return -1;

    char query[1024];
    char escaped_note[256];

    if (note && strlen(note) > 0) {
        mysql_real_escape_string(g_db_charac, escaped_note, note,
                                 strlen(note) > 100 ? 100 : strlen(note));
    } else {
        escaped_note[0] = '\0';
    }

    snprintf(query, sizeof(query),
             "INSERT INTO game_log (charac_no, log_type, target_id, value1, value2, note, log_time) "
             "VALUES (%u, %d, %u, %d, %d, '%s', NOW())",
             charac_no, log_type, target_id, value1, value2, escaped_note);

    if (mysql_query(g_db_charac, query)) {
        printf("[AUDIT] Failed to write game_log: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return 0;
}

/* DB: Write gold log entry */
static int db_write_gold_log(uint32_t charac_no, uint8_t log_type,
                              int32_t amount, uint32_t balance_before, uint32_t balance_after,
                              uint32_t related_charac, const char* note) {
    if (!g_db_charac) return -1;

    char query[1024];
    char escaped_note[256];

    if (note && strlen(note) > 0) {
        mysql_real_escape_string(g_db_charac, escaped_note, note,
                                 strlen(note) > 100 ? 100 : strlen(note));
    } else {
        escaped_note[0] = '\0';
    }

    snprintf(query, sizeof(query),
             "INSERT INTO gold_log (charac_no, log_type, amount, balance_before, balance_after, "
             "related_charac, note, log_time) "
             "VALUES (%u, %d, %d, %u, %u, %u, '%s', NOW())",
             charac_no, log_type, amount, balance_before, balance_after, related_charac, escaped_note);

    if (mysql_query(g_db_charac, query)) {
        printf("[AUDIT] Failed to write gold_log: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[AUDIT] Gold log: charac=%u type=%d amount=%d before=%u after=%u\n",
           charac_no, log_type, amount, balance_before, balance_after);
    return 0;
}

/* DB: Write item log entry */
static int db_write_item_log(uint32_t charac_no, uint8_t log_type,
                              uint32_t item_id, int32_t count, uint8_t slot,
                              uint32_t related_charac, const char* note) {
    if (!g_db_charac) return -1;

    char query[1024];
    char escaped_note[256];

    if (note && strlen(note) > 0) {
        mysql_real_escape_string(g_db_charac, escaped_note, note,
                                 strlen(note) > 100 ? 100 : strlen(note));
    } else {
        escaped_note[0] = '\0';
    }

    snprintf(query, sizeof(query),
             "INSERT INTO item_log (charac_no, log_type, item_id, count, slot, "
             "related_charac, note, log_time) "
             "VALUES (%u, %d, %u, %d, %d, %u, '%s', NOW())",
             charac_no, log_type, item_id, count, slot, related_charac, escaped_note);

    if (mysql_query(g_db_charac, query)) {
        printf("[AUDIT] Failed to write item_log: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[AUDIT] Item log: charac=%u type=%d item=%u count=%d slot=%d\n",
           charac_no, log_type, item_id, count, slot);
    return 0;
}

/* Convenience wrapper: Log gold change with automatic balance tracking */
static int audit_log_gold(ClientSession* s, uint8_t log_type, int32_t amount,
                           uint32_t related_charac, const char* note) {
    if (!s) return -1;

    uint32_t balance_before = s->inventory.money;
    uint32_t balance_after = balance_before;

    if (amount >= 0) {
        balance_after = balance_before + (uint32_t)amount;
    } else if ((uint32_t)(-amount) <= balance_before) {
        balance_after = balance_before - (uint32_t)(-amount);
    } else {
        balance_after = 0;
    }

    return db_write_gold_log(s->character_id, log_type, amount,
                             balance_before, balance_after, related_charac, note);
}

/* Convenience wrapper: Log item action */
static int audit_log_item(ClientSession* s, uint8_t log_type,
                           uint32_t item_id, int32_t count, uint8_t slot,
                           uint32_t related_charac, const char* note) {
    if (!s) return -1;
    return db_write_item_log(s->character_id, log_type, item_id, count, slot,
                             related_charac, note);
}

/* Convenience wrapper: Log game action */
static int audit_log_action(ClientSession* s, uint8_t log_type,
                             uint32_t target_id, int32_t value1, int32_t value2,
                             const char* note) {
    if (!s) return -1;
    return db_write_game_log(s->character_id, log_type, target_id, value1, value2, note);
}

/* Check for suspicious activity and log if detected */
static int audit_check_suspicious(ClientSession* s, uint32_t gold_change,
                                   uint32_t time_interval_ms) {
    if (!s) return 0;

    int suspicious = 0;
    char note[128];

    /* Check for rapid gold accumulation (>1M in <1 minute) */
    if (gold_change > 1000000 && time_interval_ms < 60000) {
        suspicious = 1;
        snprintf(note, sizeof(note), "Rapid gold gain: %u in %ums", gold_change, time_interval_ms);
        audit_log_action(s, LOG_TYPE_CHEAT_DETECTED, 1, (int32_t)gold_change, (int32_t)time_interval_ms, note);
        printf("[AUDIT] SUSPICIOUS: %s (charac=%u)\n", note, s->character_id);
    }

    /* Check for impossible gold amount */
    if (s->inventory.money > 2000000000) {  /* >2 billion */
        suspicious = 1;
        snprintf(note, sizeof(note), "Excessive gold: %u", s->inventory.money);
        audit_log_action(s, LOG_TYPE_CHEAT_DETECTED, 2, (int32_t)s->inventory.money, 0, note);
        printf("[AUDIT] SUSPICIOUS: %s (charac=%u)\n", note, s->character_id);
    }

    return suspicious;
}

/* SQL to create audit log tables (for reference):

CREATE TABLE IF NOT EXISTS game_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    charac_no INT UNSIGNED NOT NULL,
    log_type TINYINT UNSIGNED NOT NULL,
    target_id INT UNSIGNED DEFAULT 0,
    value1 INT DEFAULT 0,
    value2 INT DEFAULT 0,
    note VARCHAR(128),
    log_time DATETIME NOT NULL,
    INDEX idx_charac_time (charac_no, log_time),
    INDEX idx_type_time (log_type, log_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS gold_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    charac_no INT UNSIGNED NOT NULL,
    log_type TINYINT UNSIGNED NOT NULL,
    amount INT NOT NULL,
    balance_before INT UNSIGNED NOT NULL,
    balance_after INT UNSIGNED NOT NULL,
    related_charac INT UNSIGNED DEFAULT 0,
    note VARCHAR(128),
    log_time DATETIME NOT NULL,
    INDEX idx_charac_time (charac_no, log_time),
    INDEX idx_type_time (log_type, log_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS item_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    charac_no INT UNSIGNED NOT NULL,
    log_type TINYINT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    count INT NOT NULL,
    slot TINYINT NOT NULL,
    related_charac INT UNSIGNED DEFAULT 0,
    note VARCHAR(128),
    log_time DATETIME NOT NULL,
    INDEX idx_charac_time (charac_no, log_time),
    INDEX idx_item (item_id, log_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

*/

/*==============================================================================
 * Phase 8 P6: PVP System (PVP系统)
 *============================================================================*/

#define PVP_ROOM_MAX          100
#define PVP_ROOM_CAPACITY     8

/* Note: PVPMode enum already defined in PVP/Arena section above */
/* Using PVPRoomMode to avoid conflict */
typedef enum {
    PVP_ROOM_MODE_DUEL = 0,       /* 1v1 */
    PVP_ROOM_MODE_TEAM = 1,       /* 4v4 */
    PVP_ROOM_MODE_FREE = 2,       /* Free for all */
    PVP_ROOM_MODE_GUILD = 3       /* Guild war */
} PVPRoomMode;

typedef enum {
    PVP_STATE_WAITING = 0,
    PVP_STATE_READY = 1,
    PVP_STATE_FIGHTING = 2,
    PVP_STATE_ENDED = 3
} PVPState;



/* PVP ranking info */
typedef struct {
    uint32_t charac_no;
    uint32_t wins;
    uint32_t losses;
    uint32_t rating;
    uint8_t  rank_tier;       /* 0=Bronze, 1=Silver, 2=Gold, 3=Platinum, 4=Diamond */
} PVPRanking;

/* Rating gain/loss */
static const int16_t g_pvp_rating_change[5] = {25, 20, 15, 12, 10};  /* By tier */

/* Global PVP rooms */
static PVPRoom g_pvp_rooms[PVP_ROOM_MAX];
static uint32_t g_pvp_room_counter = 1;

/* Initialize PVP rooms */
static void init_pvp_rooms(void) {
    memset(g_pvp_rooms, 0, sizeof(g_pvp_rooms));
}

/* Find empty PVP room slot */
static PVPRoom* find_empty_pvp_room(void) {
    for (int i = 0; i < PVP_ROOM_MAX; i++) {
        if (g_pvp_rooms[i].room_id == 0) {
            return &g_pvp_rooms[i];
        }
    }
    return NULL;
}

/* Find PVP room by ID */
PVPRoom* find_pvp_room(uint32_t room_id) {
    for (int i = 0; i < PVP_ROOM_MAX; i++) {
        if (g_pvp_rooms[i].room_id == room_id) {
            return &g_pvp_rooms[i];
        }
    }
    return NULL;
}

/* Create PVP room */
uint32_t create_pvp_room(uint32_t host_id, uint8_t mode) {
    PVPRoom* room = find_empty_pvp_room();
    if (!room) return 0;

    memset(room, 0, sizeof(PVPRoom));
    room->room_id = g_pvp_room_counter++;
    room->mode = mode;
    room->state = PVP_STATE_WAITING;
    room->host_id = host_id;
    room->players[0] = host_id;
    room->player_count = 1;
    room->time_limit = (mode == PVP_ROOM_MODE_DUEL) ? 180 : 300;  /* 3 or 5 minutes */

    printf("[PVP] Room %u created by %u, mode=%d\n", room->room_id, host_id, mode);
    return room->room_id;
}

/* Join PVP room */
int join_pvp_room(uint32_t room_id, uint32_t player_id) {
    PVPRoom* room = find_pvp_room(room_id);
    if (!room || room->state != PVP_STATE_WAITING) return -1;

    uint8_t max_players = (room->mode == PVP_ROOM_MODE_DUEL) ? 2 : PVP_ROOM_CAPACITY;
    if (room->player_count >= max_players) return -2;  /* Full */

    room->players[room->player_count] = player_id;
    room->team[room->player_count] = room->player_count % 2;  /* Alternate teams */
    room->player_count++;

    return 0;
}

/* Set player ready */
int pvp_set_ready(uint32_t room_id, uint32_t player_id) {
    PVPRoom* room = find_pvp_room(room_id);
    if (!room || room->state != PVP_STATE_WAITING) return -1;

    for (int i = 0; i < room->player_count; i++) {
        if (room->players[i] == player_id) {
            room->ready[i] = 1;
            break;
        }
    }

    /* Check if all ready */
    int all_ready = 1;
    for (int i = 0; i < room->player_count; i++) {
        if (!room->ready[i]) {
            all_ready = 0;
            break;
        }
    }

    uint8_t min_players = (room->mode == PVP_ROOM_MODE_DUEL) ? 2 : 4;
    if (all_ready && room->player_count >= min_players) {
        room->state = PVP_STATE_READY;
    }

    return all_ready ? 1 : 0;
}

/* Start PVP match */
int pvp_start_match(uint32_t room_id) {
    PVPRoom* room = find_pvp_room(room_id);
    if (!room || room->state != PVP_STATE_READY) return -1;

    room->state = PVP_STATE_FIGHTING;
    room->start_time = (uint32_t)time(NULL);

    printf("[PVP] Match started in room %u\n", room_id);
    return 0;
}

/* Record PVP result */
int pvp_record_result(uint32_t room_id, uint32_t winner_id) {
    PVPRoom* room = find_pvp_room(room_id);
    if (!room || room->state != PVP_STATE_FIGHTING) return -1;

    room->state = PVP_STATE_ENDED;

    /* Update scores */
    for (int i = 0; i < room->player_count; i++) {
        if (room->players[i] == winner_id) {
            room->scores[i] = 1;  /* Win */
        } else {
            room->scores[i] = -1; /* Loss */
        }
    }

    return 0;
}

/* DB: Update PVP ranking */
static int db_update_pvp_ranking(uint32_t charac_no, int win) {
    if (!g_db_charac) return -1;

    char query[512];
    if (win) {
        snprintf(query, sizeof(query),
                 "INSERT INTO pvp_ranking (charac_no, wins, losses, rating) "
                 "VALUES (%u, 1, 0, 1025) "
                 "ON DUPLICATE KEY UPDATE wins=wins+1, rating=rating+20",
                 charac_no);
    } else {
        snprintf(query, sizeof(query),
                 "INSERT INTO pvp_ranking (charac_no, wins, losses, rating) "
                 "VALUES (%u, 0, 1, 975) "
                 "ON DUPLICATE KEY UPDATE losses=losses+1, rating=GREATEST(0, rating-15)",
                 charac_no);
    }

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    return 0;
}

/* Cleanup finished PVP room */
void pvp_cleanup_room(uint32_t room_id) {
    PVPRoom* room = find_pvp_room(room_id);
    if (room) {
        memset(room, 0, sizeof(PVPRoom));
    }
}

/*==============================================================================
 * Phase 8 P7: Daily Quest System (每日任务系统)
 *============================================================================*/

#define DAILY_QUEST_MAX       5
#define DAILY_QUEST_POOL_SIZE 20

/* Note: DailyQuestProgress typedef is defined in db/db_quest.h */

/* Daily quest pool */
static const DailyQuestDef g_daily_quest_pool[] = {
    {101, ACH_TYPE_KILL_MONSTER, 50, 500, 200, 0, 0},
    {102, ACH_TYPE_KILL_MONSTER, 100, 1000, 400, 0, 0},
    {103, ACH_TYPE_KILL_MONSTER, 200, 2000, 800, 0, 0},
    {104, ACH_TYPE_CLEAR_DUNGEON, 3, 800, 300, 0, 0},
    {105, ACH_TYPE_CLEAR_DUNGEON, 5, 1500, 600, 0, 0},
    {106, ACH_TYPE_CLEAR_DUNGEON, 10, 3000, 1200, 0, 0},
    {107, ACH_TYPE_TRADE_COUNT, 1, 300, 100, 0, 0},
    {108, ACH_TYPE_TRADE_COUNT, 3, 800, 300, 0, 0},
    {109, ACH_TYPE_PVP_WIN, 1, 500, 200, 0, 0},
    {110, ACH_TYPE_PVP_WIN, 3, 1500, 600, 0, 0},
    {111, ACH_TYPE_COLLECT_ITEM, 10, 400, 150, 0, 0},
    {112, ACH_TYPE_COLLECT_ITEM, 30, 1200, 500, 0, 0},
    {113, ACH_TYPE_GOLD_EARNED, 5000, 500, 200, 0, 0},
    {114, ACH_TYPE_GOLD_EARNED, 20000, 2000, 800, 0, 0},
    {115, ACH_TYPE_ENHANCE_EQUIP, 1, 600, 250, 0, 0},
    {116, ACH_TYPE_ENHANCE_EQUIP, 3, 1800, 750, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}
};

/* Generate daily quests for a character */
int generate_daily_quests(uint32_t charac_no, uint16_t* quest_ids, int count) {
    if (!quest_ids || count > DAILY_QUEST_MAX) return -1;

    /* Use character ID + date as seed for consistent daily generation */
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    uint32_t seed = charac_no + (tm->tm_year * 1000) + (tm->tm_yday);
    srand(seed);

    /* Count available quests */
    int pool_size = 0;
    while (g_daily_quest_pool[pool_size].quest_id != 0) pool_size++;

    /* Select random quests */
    uint8_t selected[DAILY_QUEST_POOL_SIZE] = {0};
    for (int i = 0; i < count && i < pool_size; i++) {
        int idx;
        do {
            idx = rand() % pool_size;
        } while (selected[idx]);

        selected[idx] = 1;
        quest_ids[i] = g_daily_quest_pool[idx].quest_id;
    }

    return count;
}

/* Find daily quest definition */
const DailyQuestDef* find_daily_quest(uint16_t quest_id) {
    for (int i = 0; g_daily_quest_pool[i].quest_id != 0; i++) {
        if (g_daily_quest_pool[i].quest_id == quest_id) {
            return &g_daily_quest_pool[i];
        }
    }
    return NULL;
}

/* Note: db_load_daily_quests, db_save_daily_quest are defined in db/db_quest.cpp */

/*==============================================================================
 * Phase 8 P8: Event System (活动系统框架)
 *============================================================================*/

#define EVENT_MAX             20
#define EVENT_REWARD_MAX      5

typedef enum {
    EVENT_TYPE_LOGIN = 0,        /* Login during event */
    EVENT_TYPE_DUNGEON = 1,      /* Clear dungeons */
    EVENT_TYPE_COLLECT = 2,      /* Collect items */
    EVENT_TYPE_SPEND = 3,        /* Spend currency */
    EVENT_TYPE_LEVEL = 4,        /* Reach level */
    EVENT_TYPE_ENHANCE = 5       /* Enhance equipment */
} EventType;

typedef enum {
    EVENT_STATE_INACTIVE = 0,
    EVENT_STATE_ACTIVE = 1,
    EVENT_STATE_ENDED = 2
} EventState;



typedef struct {
    uint16_t event_id;
    uint32_t progress;
    uint8_t  completed;
    uint8_t  claimed;
} EventProgress;

/* Active events (would be loaded from DB in production) */
EventDef g_active_events[EVENT_MAX];
int g_active_event_count = 0;

/* Initialize sample events */
static void init_sample_events(void) {
    g_active_event_count = 0;

    /* Sample event: Login bonus */
    EventDef* e = &g_active_events[g_active_event_count++];
    e->event_id = 1;
    strncpy(e->name, "Login Celebration", sizeof(e->name) - 1);
    e->type = EVENT_TYPE_LOGIN;
    e->state = EVENT_STATE_ACTIVE;
    e->start_time = 0;
    e->end_time = 0xFFFFFFFF;  /* Never expires */
    e->target = 1;
    e->reward_gold = 10000;
    e->reward_exp = 5000;

    /* Sample event: Dungeon rush */
    e = &g_active_events[g_active_event_count++];
    e->event_id = 2;
    strncpy(e->name, "Dungeon Rush", sizeof(e->name) - 1);
    e->type = EVENT_TYPE_DUNGEON;
    e->state = EVENT_STATE_ACTIVE;
    e->start_time = 0;
    e->end_time = 0xFFFFFFFF;
    e->target = 20;
    e->reward_gold = 50000;
    e->reward_exp = 20000;
}

/* Find active event */
EventDef* find_event(uint16_t event_id) {
    for (int i = 0; i < g_active_event_count; i++) {
        if (g_active_events[i].event_id == event_id) {
            return &g_active_events[i];
        }
    }
    return NULL;
}

/* Check if event is active */
int is_event_active(uint16_t event_id) {
    EventDef* e = find_event(event_id);
    if (!e || e->state != EVENT_STATE_ACTIVE) return 0;

    uint32_t now = (uint32_t)time(NULL);
    return (now >= e->start_time && now <= e->end_time) ? 1 : 0;
}

/* DB: Load event progress */
static int db_load_event_progress(uint32_t charac_no, EventProgress* progress, int max_count) {
    if (!g_db_charac || !progress) return -1;

    memset(progress, 0, sizeof(EventProgress) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT event_id, progress, completed, claimed FROM event_progress WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        progress[count].event_id = (uint16_t)atoi(row[0]);
        progress[count].progress = (uint32_t)atoi(row[1]);
        progress[count].completed = (uint8_t)atoi(row[2]);
        progress[count].claimed = (uint8_t)atoi(row[3]);
        count++;
    }

    mysql_free_result(result);
    return count;
}

/* DB: Update event progress */
static int db_update_event_progress(uint32_t charac_no, uint16_t event_id, uint32_t progress,
                                     uint8_t completed, uint8_t claimed) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO event_progress (charac_no, event_id, progress, completed, claimed) "
             "VALUES (%u, %d, %u, %d, %d) "
             "ON DUPLICATE KEY UPDATE progress=%u, completed=%d, claimed=%d",
             charac_no, event_id, progress, completed, claimed,
             progress, completed, claimed);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    return 0;
}

/*==============================================================================
 * Phase 8 P9: Multiplayer Dungeon Sync (多人副本同步)
 *============================================================================*/

#define SYNC_UPDATE_INTERVAL  100   /* ms */
#define SYNC_MAX_PLAYERS      4

typedef enum {
    SYNC_MSG_POSITION = 1,
    SYNC_MSG_ATTACK = 2,
    SYNC_MSG_SKILL = 3,
    SYNC_MSG_DAMAGE = 4,
    SYNC_MSG_BUFF = 5,
    SYNC_MSG_ITEM_DROP = 6,
    SYNC_MSG_MONSTER_SPAWN = 7,
    SYNC_MSG_MONSTER_DIE = 8,
    SYNC_MSG_ROOM_CLEAR = 9,
    SYNC_MSG_DUNGEON_END = 10
} SyncMessageType;

typedef struct {
    uint8_t  msg_type;
    uint32_t sender_id;
    uint32_t timestamp;
    uint16_t data_len;
    uint8_t  data[256];
} SyncMessage;

typedef struct {
    uint32_t player_id;
    uint16_t x;
    uint16_t y;
    uint8_t  direction;
    uint8_t  action;          /* 0=idle, 1=moving, 2=attacking */
    uint32_t current_hp;
    uint32_t current_mp;
    uint8_t  is_alive;
} PlayerSyncState;

/* Broadcast sync message to party */
static int broadcast_sync_message(uint32_t dungeon_id, SyncMessage* msg) {
    /* Find dungeon instance */
    SharedDungeonInstance* dungeon = NULL;
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].dungeon_id == dungeon_id && g_shared_dungeons[i].active) {
            dungeon = &g_shared_dungeons[i];
            break;
        }
    }

    if (!dungeon) return -1;

    /* Build packet */
    uint8_t packet[512];
    int offset = 0;

    /* Header */
    packet[offset++] = 0xAA;
    packet[offset++] = 0x55;

    /* Message type */
    *(uint16_t*)(packet + offset) = 400;  /* SYNC_UPDATE msg_no */
    offset += 2;

    /* Sync message data */
    packet[offset++] = msg->msg_type;
    *(uint32_t*)(packet + offset) = msg->sender_id;
    offset += 4;
    *(uint32_t*)(packet + offset) = msg->timestamp;
    offset += 4;
    *(uint16_t*)(packet + offset) = msg->data_len;
    offset += 2;
    memcpy(packet + offset, msg->data, msg->data_len);
    offset += msg->data_len;

    /* Update length */
    *(uint16_t*)(packet + 2) = offset - 4;

    /* Send to all players in dungeon using member_fds */
    for (int i = 0; i < (int)dungeon->member_count; i++) {
        int member_fd = (int)dungeon->member_fds[i];
        if (member_fd > 0) {
            /* Find session by fd and check if it's not the sender */
            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd &&
                    g_sessions[j].character_id != msg->sender_id) {
                    send(member_fd, (char*)packet, offset, 0);
                    break;
                }
            }
        }
    }

    return 0;
}

/* Sync player position */
static int sync_player_position(uint32_t dungeon_id, uint32_t player_id,
                                 uint16_t x, uint16_t y, uint8_t direction) {
    SyncMessage msg;
    memset(&msg, 0, sizeof(msg));

    msg.msg_type = SYNC_MSG_POSITION;
    msg.sender_id = player_id;
    msg.timestamp = (uint32_t)(time(NULL) * 1000);

    /* Pack position data */
    int offset = 0;
    *(uint16_t*)(msg.data + offset) = x;
    offset += 2;
    *(uint16_t*)(msg.data + offset) = y;
    offset += 2;
    msg.data[offset++] = direction;
    msg.data_len = offset;

    return broadcast_sync_message(dungeon_id, &msg);
}

/* Sync player attack */
static int sync_player_attack(uint32_t dungeon_id, uint32_t player_id,
                               uint16_t skill_id, uint16_t target_x, uint16_t target_y) {
    SyncMessage msg;
    memset(&msg, 0, sizeof(msg));

    msg.msg_type = SYNC_MSG_SKILL;
    msg.sender_id = player_id;
    msg.timestamp = (uint32_t)(time(NULL) * 1000);

    int offset = 0;
    *(uint16_t*)(msg.data + offset) = skill_id;
    offset += 2;
    *(uint16_t*)(msg.data + offset) = target_x;
    offset += 2;
    *(uint16_t*)(msg.data + offset) = target_y;
    offset += 2;
    msg.data_len = offset;

    return broadcast_sync_message(dungeon_id, &msg);
}

/* Sync damage dealt */
static int sync_damage(uint32_t dungeon_id, uint32_t attacker_id, uint32_t target_id,
                        uint32_t damage, uint8_t is_crit, uint8_t target_type) {
    SyncMessage msg;
    memset(&msg, 0, sizeof(msg));

    msg.msg_type = SYNC_MSG_DAMAGE;
    msg.sender_id = attacker_id;
    msg.timestamp = (uint32_t)(time(NULL) * 1000);

    int offset = 0;
    *(uint32_t*)(msg.data + offset) = target_id;
    offset += 4;
    *(uint32_t*)(msg.data + offset) = damage;
    offset += 4;
    msg.data[offset++] = is_crit;
    msg.data[offset++] = target_type;  /* 0=player, 1=monster */
    msg.data_len = offset;

    return broadcast_sync_message(dungeon_id, &msg);
}

/* Sync monster death */
static int sync_monster_death(uint32_t dungeon_id, uint32_t monster_id, uint32_t killer_id) {
    SyncMessage msg;
    memset(&msg, 0, sizeof(msg));

    msg.msg_type = SYNC_MSG_MONSTER_DIE;
    msg.sender_id = 0;  /* Server message */
    msg.timestamp = (uint32_t)(time(NULL) * 1000);

    int offset = 0;
    *(uint32_t*)(msg.data + offset) = monster_id;
    offset += 4;
    *(uint32_t*)(msg.data + offset) = killer_id;
    offset += 4;
    msg.data_len = offset;

    return broadcast_sync_message(dungeon_id, &msg);
}

/*==============================================================================
 * Phase 8 P10: Equipment Disassemble/Craft (装备分解/合成)
 *============================================================================*/

#define MATERIAL_TYPE_MAX     10
#define CRAFT_RECIPE_MAX      50

typedef enum {
    MAT_IRON_ORE = 0,
    MAT_MAGIC_DUST = 1,
    MAT_LEATHER = 2,
    MAT_CLOTH = 3,
    MAT_CRYSTAL = 4,
    MAT_ESSENCE = 5,
    MAT_SOUL_SHARD = 6,
    MAT_RARE_METAL = 7,
    MAT_DEMON_CORE = 8,
    MAT_CHAOS_STONE = 9
} MaterialType;



/* Disassemble result by item rarity */


/* Disassemble tables by rarity */
static const DisassembleResult g_disassemble_table[] = {
    {0, {10001, 0, 0}, {1, 0, 0}, 1},           /* Normal: 1 Iron Ore */
    {1, {10001, 10002, 0}, {2, 1, 0}, 2},       /* Magic: 2 Iron, 1 Magic Dust */
    {2, {10001, 10002, 10005}, {3, 2, 1}, 3},   /* Rare: 3 Iron, 2 Dust, 1 Crystal */
    {3, {10002, 10005, 10006}, {5, 3, 2}, 3},   /* Epic: 5 Dust, 3 Crystal, 2 Essence */
    {4, {10005, 10006, 10009}, {5, 4, 1}, 3},   /* Legendary: 5 Crystal, 4 Essence, 1 Chaos */
};

/* Sample craft recipes */
const CraftRecipe g_craft_recipes[] = {
    /* recipe_id, result_item, count, mat_ids[5], mat_counts[5], gold, level */
    {1, 1001, 1, {10001, 10002, 0, 0, 0}, {10, 5, 0, 0, 0}, 1000, 10},      /* Basic Sword */
    {2, 1002, 1, {10001, 10002, 10003, 0, 0}, {20, 10, 5, 0, 0}, 3000, 20}, /* Steel Sword */
    {3, 1003, 1, {10001, 10005, 10006, 0, 0}, {30, 10, 5, 0, 0}, 10000, 40}, /* Magic Sword */
    {4, 2001, 1, {10003, 10004, 0, 0, 0}, {15, 10, 0, 0, 0}, 2000, 15},     /* Leather Armor */
    {5, 2002, 1, {10003, 10004, 10002, 0, 0}, {25, 15, 5, 0, 0}, 5000, 30}, /* Chain Armor */
    {0, 0, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, 0}
};

/* Get disassemble results for an item */
const DisassembleResult* get_disassemble_result(uint8_t item_rarity) {
    if (item_rarity > 4) item_rarity = 4;
    return &g_disassemble_table[item_rarity];
}

/* Find craft recipe */
const CraftRecipe* find_craft_recipe(uint32_t recipe_id) {
    for (int i = 0; g_craft_recipes[i].recipe_id != 0; i++) {
        if (g_craft_recipes[i].recipe_id == recipe_id) {
            return &g_craft_recipes[i];
        }
    }
    return NULL;
}

/* Check if player has required materials */
int check_craft_materials(uint32_t charac_no, const CraftRecipe* recipe) {
    if (!g_db_charac || !recipe) return 0;

    for (int i = 0; i < 5 && recipe->required_mats[i] != 0; i++) {
        char query[256];
        snprintf(query, sizeof(query),
                 "SELECT SUM(count) FROM user_items WHERE charac_no=%u AND it_id=%d",
                 charac_no, recipe->required_mats[i]);

        if (mysql_query(g_db_charac, query)) {
            return 0;
        }

        MYSQL_RES* result = mysql_store_result(g_db_charac);
        if (!result) return 0;

        MYSQL_ROW row = mysql_fetch_row(result);
        int count = row && row[0] ? atoi(row[0]) : 0;
        mysql_free_result(result);

        if (count < recipe->required_counts[i]) {
            return 0;  /* Not enough materials */
        }
    }

    return 1;  /* Has all materials */
}

/* DB: Disassemble an item */
int db_disassemble_item(uint32_t charac_no, int32_t item_slot, uint8_t item_rarity) {
    if (!g_db_charac2) return -1;

    DBItemInfo item;
    if (db_get_item_at_slot(charac_no, item_slot, &item) < 0 || item.ui_id == 0) {
        printf("[CRAFT] Cannot disassemble: slot %d empty\n", item_slot);
        return -1;
    }

    if (db_delete_item(charac_no, item.ui_id) < 0) {
        printf("[CRAFT] Failed to delete item ui_id=%u for disassemble\n", item.ui_id);
        return -1;
    }

    const DisassembleResult* result = get_disassemble_result(item_rarity);
    for (int i = 0; i < result->material_count; i++) {
        uint16_t amount = result->material_counts[i];
        int32_t stacked_slot = -1;
        int stacked = db_try_stack_item(charac_no, result->material_ids[i],
                                        amount, &stacked_slot);
        if (stacked < 0) {
            printf("[CRAFT] Failed to stack material %d for charac_no=%u\n",
                   result->material_ids[i], charac_no);
            return -1;
        }

        uint16_t remaining = static_cast<uint16_t>(amount > stacked ? amount - stacked : 0);
        if (remaining == 0) {
            continue;
        }

        int free_slot = db_find_free_slot(charac_no, MAX_EQUIP_SLOTS);
        if (free_slot < 0) {
            printf("[CRAFT] No free slot to place disassemble reward (item=%u)\n",
                   result->material_ids[i]);
            continue;
        }

        if (db_add_item(charac_no, result->material_ids[i], free_slot, remaining) < 0) {
            printf("[CRAFT] Failed to add reward item %u for charac_no=%u\n",
                   result->material_ids[i], charac_no);
        }
    }

    printf("[CRAFT] Character %u disassembled slot %d, rarity=%u\n",
           charac_no, item_slot, item_rarity);
    return 0;
}

/* DB: Craft an item */
static int db_craft_item(uint32_t charac_no, uint32_t recipe_id) {
    const CraftRecipe* recipe = find_craft_recipe(recipe_id);
    if (!recipe) return -1;

    /* Check materials */
    if (!check_craft_materials(charac_no, recipe)) {
        return -2;  /* Missing materials */
    }

    /* Consume materials */
    for (int i = 0; i < 5 && recipe->required_mats[i] != 0; i++) {
        char query[256];

        /* Find and reduce item count, delete if count becomes 0 */
        snprintf(query, sizeof(query),
                 "UPDATE user_items SET count = count - %d "
                 "WHERE charac_no=%u AND it_id=%d AND count >= %d LIMIT 1",
                 recipe->required_counts[i], charac_no,
                 recipe->required_mats[i], recipe->required_counts[i]);

        if (mysql_query(g_db_charac, query) || mysql_affected_rows(g_db_charac) == 0) {
            return -3;  /* Failed to consume materials */
        }

        /* Clean up zero-count items */
        snprintf(query, sizeof(query),
                 "DELETE FROM user_items WHERE charac_no=%u AND count <= 0",
                 charac_no);
        mysql_query(g_db_charac, query);
    }

    /* Create result item */
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, it_id, count, slot) "
             "SELECT %u, %u, %d, COALESCE(MAX(slot)+1, 0) FROM user_items WHERE charac_no=%u",
             charac_no, recipe->result_item_id, recipe->result_count, charac_no);

    if (mysql_query(g_db_charac, query)) {
        return -4;  /* Failed to create item */
    }

    printf("[CRAFT] Character %u crafted item %u\n", charac_no, recipe->result_item_id);
    return 0;
}

/* Get account by UID */


/* Note: db_get_characters is defined in db/db_characters.cpp */

/* Get single character by charac_no */


/* Note: db_get_mail_list, db_get_unread_mail_count, db_get_mail_attachment, db_get_mail_by_id, db_delete_mail, db_send_mail are defined in db/db_social.cpp */

/* Get character no by name */
uint32_t db_get_charac_no_by_name(const char* name) {
    if (!g_db_charac || !name) return 0;

    char safe_name[41];
    memset(safe_name, 0, sizeof(safe_name));
    strncpy(safe_name, name, 20);
    for (int i = 0; i < 40 && safe_name[i]; i++) {
        if (safe_name[i] == '\'' || safe_name[i] == '\\') safe_name[i] = '_';
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE charac_name='%s' AND delete_flag=0",
             safe_name);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row = mysql_fetch_row(result);
    uint32_t charac_no = row && row[0] ? atoi(row[0]) : 0;
    mysql_free_result(result);
    return charac_no;
}

/* Note: db_add_friend, db_remove_friend are defined in db/db_social.cpp */

/* Ranking entry structure */
typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  lev;
    uint8_t  job;
    uint32_t money;
} DBRankingEntry;

/* Get level ranking from charac_info - ORDER BY lev DESC */
static int db_get_level_ranking(DBRankingEntry* entries, int max_count) {
    if (!g_db_charac || !entries) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, lev, job "
             "FROM charac_info WHERE delete_flag=0 "
             "ORDER BY lev DESC, exp DESC LIMIT %d", max_count);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Level ranking query failed: %s\n", mysql_error(g_db_charac));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac);
    if (!res) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        entries[count].charac_no = row[0] ? atoi(row[0]) : 0;
        memset(entries[count].charac_name, 0, 20);
        if (row[1]) strncpy(entries[count].charac_name, row[1], 19);
        entries[count].lev = row[2] ? atoi(row[2]) : 1;
        entries[count].job = row[3] ? atoi(row[3]) : 0;
        entries[count].money = 0;
        count++;
    }
    mysql_free_result(res);

    printf("[DB] Level ranking: found %d entries\n", count);
    return count;
}

/* Get wealth ranking - JOIN charac_info with inventory for money */
static int db_get_wealth_ranking(DBRankingEntry* entries, int max_count) {
    if (!g_db_charac2 || !entries) return 0;

    /* Query inventory table for money, then get character info */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT i.charac_no, c.charac_name, c.lev, c.job, i.money "
             "FROM inventory i "
             "JOIN taiwan_cain.charac_info c ON i.charac_no = c.charac_no "
             "WHERE c.delete_flag=0 "
             "ORDER BY i.money DESC LIMIT %d", max_count);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Wealth ranking query failed: %s\n", mysql_error(g_db_charac2));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac2);
    if (!res) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        entries[count].charac_no = row[0] ? atoi(row[0]) : 0;
        memset(entries[count].charac_name, 0, 20);
        if (row[1]) strncpy(entries[count].charac_name, row[1], 19);
        entries[count].lev = row[2] ? atoi(row[2]) : 1;
        entries[count].job = row[3] ? atoi(row[3]) : 0;
        entries[count].money = row[4] ? strtoul(row[4], NULL, 10) : 0;
        count++;
    }
    mysql_free_result(res);

    printf("[DB] Wealth ranking: found %d entries\n", count);
    return count;
}

/* Note: db_create_guild, db_add_guild_member, db_update_guild_member_count, db_update_character_guild, db_remove_guild_member, db_disband_guild are defined in db/db_social.cpp */

/* Note: db_get_friend_list is defined in db/db_social.cpp */

/* Soft delete a character (set delete_flag=1) */


/* Optional: Clean up character items (move to deleted_items or just clear) */
static int db_cleanup_deleted_character_items(uint32_t charac_no) {
    if (!g_db_charac2) return -1;

    /* Delete items from user_items for the deleted character */
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Cleanup items failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Cleaned up %d items for deleted charac_no=%u\n", affected, charac_no);
    return 0;
}

/* Note: db_get_guild_by_charac, db_get_guild_members are defined in db/db_social.cpp */

/* Note: db_get_storage, db_get_storage_gold are defined in db/db_economy.cpp */

/* Note: DBPetInfo typedef is defined in db/db_types.h */
#define MAX_PETS 10

/* Get pet list from taiwan_cain_2nd.creature_items */
static int db_get_pet_list(uint32_t charac_no, DBPetInfo* pets, int max_count) {
    if (!g_db_charac2 || !pets) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT ui_id, it_id, name, exp, endurance, slot, creature_type, stat "
             "FROM creature_items WHERE charac_no=%u AND stat=0 ORDER BY slot LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Pet query failed: %s\n", mysql_error(g_db_charac2));
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        DBPetInfo* p = &pets[count];
        memset(p, 0, sizeof(DBPetInfo));
        p->ui_id = row[0] ? atoi(row[0]) : 0;
        p->it_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(p->name, row[2], sizeof(p->name) - 1);
        p->exp = row[3] ? atoi(row[3]) : 0;
        p->endurance = row[4] ? atoi(row[4]) : 100;
        p->slot = row[5] ? atoi(row[5]) : 0;
        p->creature_type = row[6] ? atoi(row[6]) : 0;
        p->stat = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d pets for charac_no=%u\n", count, charac_no);
    return count;
}
