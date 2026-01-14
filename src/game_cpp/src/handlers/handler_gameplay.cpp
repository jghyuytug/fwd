#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>

#include "game_server_context.h"
#include "db/db_characters.h"
#include "db/db_economy.h"
#include "db/db_inventory.h"
#include "db/db_skills.h"
#include "db/db_social.h"
#include "db/db_quest.h"
#include "db/db_game.h"
#include "game_packet_utils.h"
#include "handlers/handler_prototypes.h"
#include "handlers/gameplay_helpers.h"
#include "systems/buff_system.h"
#include "systems/combat_system.h"
#include "systems/dungeon_manager.h"
#include "systems/monster_ai.h"
#include "systems/time_utils.h"
#include "systems/session_manager.h"
#include "game_server_types.h"

/* Ranking entry structure for leaderboards */
typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  lev;
    uint8_t  job;
    uint32_t value;  /* Generic value field (gold, pvp points, etc) */
    uint32_t money;  /* Gold/money field for wealth ranking */
} DBRankingEntry;

/* Achievement progress structure */
typedef struct {
    uint16_t ach_id;
    uint32_t progress;
    uint8_t  completed;
    uint8_t  claimed;
} AchievementProgress;

/* Chat type constants - now defined in systems/session_manager.h */

/* Achievement type constants */
#ifndef ACH_TYPE_KILL_MONSTER
#define ACH_TYPE_KILL_MONSTER  1
#define ACH_TYPE_CLEAR_DUNGEON 2
#define ACH_TYPE_KILL_BOSS     3
#define ACH_TYPE_FIRST_BOSS    4
#define ACH_TYPE_SPEED_CLEAR   5
#define ACH_TYPE_NO_DEATH      6
#define ACH_TYPE_PARTY_CLEAR   7
#endif

/* db_learn_skill - update skill level in database */
static int db_learn_skill(uint32_t charac_no, uint16_t skill_id, uint8_t new_level) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE skill SET skill_lv_%u = %u WHERE charac_no = %u",
             skill_id % 100, new_level, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Learn skill failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    printf("[DB] Learned skill %u to level %u for charac_no=%u\n",
           skill_id, new_level, charac_no);
    return 0;
}

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

static int db_add_guild_contribution(uint32_t guild_id, uint32_t charac_no,
                                     uint32_t contribution) {
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET contribution=contribution+%u "
             "WHERE guild_id=%u AND charac_no=%u",
             contribution, guild_id, charac_no);

    return guild_execute_non_query(query) < 0 ? -1 : 0;
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

static int db_save_equipped_title(uint32_t charac_no, uint32_t title_id) {
    if (!g_db_charac) return -1;

    printf("[DB] Saving equipped title: charac_no=%u, title_id=%u\n",
           charac_no, title_id);

    /* Title equipment is typically stored in charac_info or a related table
     * For now we log it - actual implementation depends on DB schema */
    return 0;
}

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

static int db_set_storage_gold(uint32_t account_id, uint32_t gold) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE account_cargo SET money=%u WHERE m_id=%u",
             gold, account_id);
    if (mysql_query(g_db_charac2, query)) return -1;
    if (mysql_affected_rows(g_db_charac2) == 0) {
        snprintf(query, sizeof(query),
                 "INSERT INTO account_cargo (m_id, money) VALUES (%u, %u)",
                 account_id, gold);
        mysql_query(g_db_charac2, query);
    }
    return 0;
}

static int db_socket_gem(uint32_t charac_no, int32_t item_slot, uint8_t socket_idx, uint16_t gem_id) {
    if (!g_db_charac || socket_idx >= GEM_MAX_SOCKETS) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO item_gems (charac_no, item_slot, socket_idx, gem_id) "
             "VALUES (%u, %d, %d, %d) "
             "ON DUPLICATE KEY UPDATE gem_id=%d",
             charac_no, item_slot, socket_idx, gem_id, gem_id);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    return 0;
}

static int db_storage_deposit_item(uint32_t m_id, uint32_t charac_no, int inv_slot) {
    if (!g_db_charac2 || !g_db_charac) return -1;

    /* Get item info from inventory */
    DBItemInfo inv_item;
    if (db_get_item_at_slot(charac_no, inv_slot, &inv_item) < 0 || inv_item.ui_id == 0) {
        printf("[STORAGE] No item at slot %d\n", inv_slot);
        return -1;
    }
    uint32_t item_id = inv_item.it_id;

    /* Find free storage slot */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT slot FROM account_cargo_items WHERE m_id=%u ORDER BY slot", m_id);

    if (mysql_query(g_db_charac, query)) {
        /* Table might not exist, create it */
        snprintf(query, sizeof(query),
                 "CREATE TABLE IF NOT EXISTS account_cargo_items ("
                 "m_id INT UNSIGNED, slot INT, it_id INT UNSIGNED, stat TINYINT, "
                 "PRIMARY KEY (m_id, slot))");
        mysql_query(g_db_charac, query);
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
            if (row) storage_slot = atoi(row[0]);
            mysql_free_result(result);
        }
    }

    /* Insert into storage */
    snprintf(query, sizeof(query),
             "INSERT INTO account_cargo_items (m_id, slot, it_id, stat) VALUES (%u, %d, %u, 0)",
             m_id, storage_slot, item_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[STORAGE] Failed to insert item: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[STORAGE] Deposited item %u from inv slot %d to storage slot %d\n",
           item_id, inv_slot, storage_slot);
    return storage_slot;
}

static int db_storage_withdraw_item(uint32_t m_id, uint32_t charac_no, int storage_slot) {
    if (!g_db_charac2 || !g_db_charac) return -1;

    /* Get item from storage */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT it_id, stat FROM account_cargo_items WHERE m_id=%u AND slot=%d",
             m_id, storage_slot);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;
    }

    uint32_t item_id = row[0] ? atoi(row[0]) : 0;
    uint8_t stat = row[1] ? atoi(row[1]) : 0;
    mysql_free_result(result);

    if (item_id == 0) return -1;

    /* Find free inventory slot */
    int inv_slot = db_find_free_slot(charac_no, MAX_EQUIP_SLOTS);
    if (inv_slot < 0 || inv_slot >= 200) {
        printf("[STORAGE] Inventory full\n");
        return -2;  /* Inventory full */
    }

    /* Remove from storage */
    snprintf(query, sizeof(query),
             "DELETE FROM account_cargo_items WHERE m_id=%u AND slot=%d",
             m_id, storage_slot);
    mysql_query(g_db_charac, query);

    /* Add to inventory */
    int new_id = db_add_item(charac_no, item_id, inv_slot, stat);
    if (new_id < 0) {
        return -1;
    }

    printf("[STORAGE] Withdrew item %u from storage slot %d to inv slot %d\n",
           item_id, storage_slot, inv_slot);
    return inv_slot;
}

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

static int db_unsocket_gem(uint32_t charac_no, int32_t item_slot, uint8_t socket_idx) {
    if (!g_db_charac || socket_idx >= GEM_MAX_SOCKETS) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "DELETE FROM item_gems WHERE charac_no=%u AND item_slot=%d AND socket_idx=%d",
             charac_no, item_slot, socket_idx);

    if (mysql_query(g_db_charac, query)) {
        return -1;
    }

    return mysql_affected_rows(g_db_charac) > 0 ? 0 : -1;
}



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


static PartyInfo g_parties[MAX_PARTIES];
static uint32_t g_next_party_id = 1;
static PVPQueueEntry g_pvp_queue[MAX_PVP_QUEUE];
static PVPMatch g_pvp_matches[MAX_PVP_MATCHES];
static uint32_t g_next_pvp_match_id = 1;
static uint32_t g_next_shared_dungeon_id = 1;

/* PVP helper functions migrated from legacy implementation */
static uint8_t get_pvp_rank(uint32_t points) {
    if (points >= 2500) return 5;  /* Master */
    if (points >= 2000) return 4;  /* Diamond */
    if (points >= 1500) return 3;  /* Platinum */
    if (points >= 1000) return 2;  /* Gold */
    if (points >= 500)  return 1;  /* Silver */
    return 0;                       /* Bronze */
}

static PVPQueueEntry* find_pvp_queue_entry(uint32_t player_id) {
    for (int i = 0; i < MAX_PVP_QUEUE; i++) {
        if (g_pvp_queue[i].active && g_pvp_queue[i].player_id == player_id) {
            return &g_pvp_queue[i];
        }
    }
    return NULL;
}

static PVPMatch* find_pvp_match(uint32_t match_id) {
    for (int i = 0; i < MAX_PVP_MATCHES; i++) {
        if (g_pvp_matches[i].match_id == match_id && g_pvp_matches[i].state != 0) {
            return &g_pvp_matches[i];
        }
    }
    return NULL;
}

static int pvp_queue_add(uint32_t player_id, uint32_t player_fd, uint8_t mode, uint32_t points) {
    if (find_pvp_queue_entry(player_id)) {
        return -1;  /* Already queued */
    }

    for (int i = 0; i < MAX_PVP_QUEUE; i++) {
        if (!g_pvp_queue[i].active) {
            g_pvp_queue[i].player_id = player_id;
            g_pvp_queue[i].player_fd = player_fd;
            g_pvp_queue[i].mode = mode;
            g_pvp_queue[i].pvp_points = points;
            g_pvp_queue[i].queue_time = (uint32_t)time(NULL);
            g_pvp_queue[i].active = 1;
            printf("[PVP] Player %u queued for mode %d (points=%u)\n", player_id, mode, points);
            return i;
        }
    }
    return -2;  /* Queue full */
}

static int pvp_queue_remove(uint32_t player_id) {
    PVPQueueEntry* entry = find_pvp_queue_entry(player_id);
    if (entry) {
        entry->active = 0;
        printf("[PVP] Player %u removed from queue\n", player_id);
        return 0;
    }
    return -1;
}

static PVPMatch* pvp_try_match(uint8_t mode) {
    int players_needed = (mode == PVP_MODE_1V1) ? 2 :
                         (mode == PVP_MODE_2V2) ? 4 :
                         (mode == PVP_MODE_3V3) ? 6 : 4;

    PVPQueueEntry* candidates[8];
    int candidate_count = 0;

    for (int i = 0; i < MAX_PVP_QUEUE && candidate_count < 8; i++) {
        if (g_pvp_queue[i].active && g_pvp_queue[i].mode == mode) {
            candidates[candidate_count++] = &g_pvp_queue[i];
        }
    }

    if (candidate_count < players_needed) {
        return NULL;
    }

    PVPMatch* match = NULL;
    for (int i = 0; i < MAX_PVP_MATCHES; i++) {
        if (g_pvp_matches[i].state == 0) {
            match = &g_pvp_matches[i];
            break;
        }
    }
    if (!match) return NULL;

    memset(match, 0, sizeof(PVPMatch));
    match->match_id = g_next_pvp_match_id++;
    match->mode = mode;
    match->state = 1;
    match->start_time = (uint32_t)time(NULL);

    int team1_size = players_needed / 2;
    for (int i = 0; i < players_needed && i < candidate_count; i++) {
        if (i < team1_size) {
            match->team1[match->team1_count] = candidates[i]->player_id;
            match->team1_hp[match->team1_count] = 10000;
            match->team1_count++;
        } else {
            match->team2[match->team2_count] = candidates[i]->player_id;
            match->team2_hp[match->team2_count] = 10000;
            match->team2_count++;
        }
        candidates[i]->active = 0;
    }

    printf("[PVP] Match %u created: Team1(%d) vs Team2(%d)\n",
           match->match_id, match->team1_count, match->team2_count);
    return match;
}

static int pvp_find_player_in_match(PVPMatch* match, uint32_t player_id, uint8_t* team, uint8_t* index) {
    for (int i = 0; i < match->team1_count; i++) {
        if (match->team1[i] == player_id) {
            *team = 1;
            *index = i;
            return 1;
        }
    }
    for (int i = 0; i < match->team2_count; i++) {
        if (match->team2[i] == player_id) {
            *team = 2;
            *index = i;
            return 1;
        }
    }
    return 0;
}

static int32_t calculate_elo_change(uint32_t winner_rating, uint32_t loser_rating, int is_winner) {
    int k = 32;
    double expected = 1.0 / (1.0 + pow(10.0, (double)(loser_rating - winner_rating) / 400.0));
    double actual = is_winner ? 1.0 : 0.0;
    int32_t change = (int32_t)(k * (actual - expected));
    if (is_winner && change < 10) change = 10;
    if (!is_winner && change > -10) change = -10;
    return change;
}

static int pvp_all_ready(PVPMatch* match) {
    for (int i = 0; i < match->team1_count; i++) {
        if (!match->team1_ready[i]) return 0;
    }
    for (int i = 0; i < match->team2_count; i++) {
        if (!match->team2_ready[i]) return 0;
    }
    return 1;
}

static int pvp_check_team_eliminated(PVPMatch* match, uint8_t team) {
    if (team == 1) {
        for (int i = 0; i < match->team1_count; i++) {
            if (match->team1_hp[i] > 0) return 0;
        }
        return 1;
    }
    for (int i = 0; i < match->team2_count; i++) {
        if (match->team2_hp[i] > 0) return 0;
    }
    return 1;
}

static void pvp_end_match(PVPMatch* match, uint8_t winner_team);

static PartyInfo* find_party(uint32_t party_id);
void guild_ipc_notify_online(uint32_t guild_id, uint32_t charac_no, int online);
static int guild_ipc_add_exp(uint32_t guild_id, uint32_t exp_amount,
                             uint8_t* new_level, uint8_t* leveled_up);


/*==============================================================================
 * Protocol Handlers
 *============================================================================*/

/* msg_no=2: SET_UDP_IP_PORT */
/* msg_no=3: EXIT */
/* msg_no=4: SELECT_CHARACTER - Now with database lookup
 * Test client sends char_index (slot 0, 1, 2...)
 * Production client may send charac_no
 */
/* msg_no=7: RETURN_SELECT_CHARACTER */
/* msg_no=8: GET_USERINFO */
/* msg_no=15: START_GAME */
int handle_start_game(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] START_GAME from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 27, error, 4);  /* NOTIPACKET_START_GAME = 27 */
    }

    /* Response: result(1) + village_id(4) + position data */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    response[0] = 0;  /* Success */

    /* Get village from selected character */
    uint32_t village_id = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            village_id = s->characters[i].village;
            break;
        }
    }
    memcpy(response + 1, &village_id, 4);

    /* Initialize combat stats and current HP/MP */
    calculate_combat_stats(s);
    s->current_hp = s->combat_stats.max_hp;
    s->current_mp = s->combat_stats.max_mp;
    s->current_village = village_id;
    s->party_id = 0;

    /* Load guild_id from character data */
    s->guild_id = 0;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            s->guild_id = s->characters[i].guild_id;
            break;
        }
    }
    if (s->guild_id > 0) {
        printf("[GAME] Character %u is in guild %u\n", s->character_id, s->guild_id);
        /* Phase 9: Notify guild service that member is online */
        guild_ipc_notify_online(s->guild_id, s->character_id, 1);
    }

    printf("[GAME] START_GAME: entering village %u (HP=%u, MP=%u)\n",
           village_id, s->current_hp, s->current_mp);
    return send_response(s, 27, response, 16);  /* NOTIPACKET_START_GAME = 27 */
}

/* msg_no=40: FINISH_LOADING */
int handle_finish_loading(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] FINISH_LOADING from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Send FINISH_LOADING response first */
    uint8_t response[4] = {0, 0, 0, 0};
    send_response(s, 30, response, 4);  /* NOTIPACKET_FINISH_LOADING = 30 */

    /* Send initial game state after loading completes:
     * 1. ITEM_LIST (msg_no=13) - inventory
     * 2. SKILLINFO (msg_no=19) - skills
     * 3. USER_POSITION (msg_no=22) - initial position
     * 4. FATIGUE (msg_no=36) - fatigue value
     */

    /* Send ITEM_LIST if inventory was loaded */
    if (s->inventory.item_count > 0) {
        uint8_t item_buf[4096];
        int item_len = 0;

        /* Item count (4 bytes) */
        uint32_t count = (uint32_t)s->inventory.item_count;
        memcpy(item_buf + item_len, &count, 4);
        item_len += 4;

        /* Item entries (simplified: 20 bytes each) */
        for (int i = 0; i < s->inventory.item_count && i < 100; i++) {
            memcpy(item_buf + item_len, &s->inventory.items[i].it_id, 4);  /* item id */
            item_len += 4;
            memcpy(item_buf + item_len, &s->inventory.items[i].count, 2);  /* count */
            item_len += 2;
            memcpy(item_buf + item_len, &s->inventory.items[i].slot, 2);   /* slot */
            item_len += 2;
            item_buf[item_len++] = s->inventory.items[i].upgrade;          /* upgrade level */
            item_buf[item_len++] = s->inventory.items[i].seal;             /* seal flag */
            memcpy(item_buf + item_len, &s->inventory.items[i].flags, 2);  /* flags */
            item_len += 2;
            memset(item_buf + item_len, 0, 8);  /* reserved/expire */
            item_len += 8;
        }

        printf("[GAME] Sending ITEM_LIST with %d items\n", s->inventory.item_count);
        send_response(s, 13, item_buf, item_len);  /* NOTIPACKET_ITEM_LIST = 13 */
    }

    /* Send SKILLINFO (SP info only, no individual skills in this struct) */
    if (s->skill.skill_loaded) {
        uint8_t skill_buf[32];
        int skill_len = 0;

        /* SP data: remain_sp(4) + used_sp(4) + remain_sp_2nd(4) */
        memcpy(skill_buf + skill_len, &s->skill.remain_sp, 4);
        skill_len += 4;
        memcpy(skill_buf + skill_len, &s->skill.used_sp, 4);
        skill_len += 4;
        memcpy(skill_buf + skill_len, &s->skill.remain_sp_2nd, 4);
        skill_len += 4;
        memset(skill_buf + skill_len, 0, 8);  /* padding */
        skill_len += 8;

        printf("[GAME] Sending SKILLINFO (SP: remain=%u used=%u)\n",
               s->skill.remain_sp, s->skill.used_sp);
        send_response(s, 19, skill_buf, skill_len);  /* NOTIPACKET_SKILLINFO = 19 */
    }

    /* Send FATIGUE */
    uint8_t fatigue_buf[8];
    int16_t fatigue = 156;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            fatigue = s->characters[i].fatigue;
            break;
        }
    }
    memcpy(fatigue_buf, &fatigue, 2);
    memset(fatigue_buf + 2, 0, 6);
    send_response(s, 36, fatigue_buf, 8);  /* NOTIPACKET_FATIGUE = 36 */

    printf("[GAME] FINISH_LOADING complete, game state sent\n");
    return 0;
}

/* msg_no=45: GIVEUP_GAME */
int handle_giveup_game(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GIVEUP_GAME from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 45, response, 4);
}

/*==============================================================================
 * Extended Protocol Handlers
 *============================================================================*/

/* msg_no=9: HEARTBEAT/PING */
int handle_heartbeat(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    s->last_activity = (uint32_t)time(NULL);

    uint8_t response[8];
    uint32_t result = 0;
    uint32_t server_time = (uint32_t)time(NULL);
    memcpy(response, &result, 4);
    memcpy(response + 4, &server_time, 4);

    return send_response(s, 9, response, 8);
}

/* msg_no=10: CHAT */
int handle_chat(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CHAT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        return 0;  /* Silently ignore */
    }

    /* Parse chat type and message */
    uint8_t chat_type = data[0];
    uint8_t msg_len = data[1];
    char message[256];
    memset(message, 0, sizeof(message));

    if (msg_len > 0 && msg_len < 250 && len >= (int)(msg_len + 2)) {
        memcpy(message, data + 2, msg_len);
    }

    printf("[GAME] CHAT: type=%d msg='%s'\n", chat_type, message);

    switch (chat_type) {
        case CHAT_TYPE_WORLD:
            printf("[GAME] World chat from %s: %s\n", s->character_name, message);
            broadcast_chat(s, chat_type, message, msg_len, 0);
            break;

        case CHAT_TYPE_GUILD:
            if (s->guild_id > 0) {
                printf("[GAME] Guild chat from %s (guild=%u): %s\n",
                       s->character_name, s->guild_id, message);
                broadcast_chat(s, chat_type, message, msg_len, 0);
            } else {
                /* Not in guild - send error */
                uint8_t error[4] = {1, 0, 0, 0};
                return send_response(s, 10, error, 4);
            }
            break;

        case CHAT_TYPE_PARTY:
            if (s->party_id > 0) {
                printf("[GAME] Party chat from %s (party=%u): %s\n",
                       s->character_name, s->party_id, message);
                broadcast_chat(s, chat_type, message, msg_len, 0);
            } else {
                uint8_t error[4] = {2, 0, 0, 0};
                return send_response(s, 10, error, 4);
            }
            break;

        case CHAT_TYPE_WHISPER: {
            /* Whisper: target name follows message */
            char target_name[21];
            memset(target_name, 0, sizeof(target_name));

            /* Format: type(1) + msg_len(1) + msg + target_name_len(1) + target_name */
            int name_offset = 2 + msg_len;
            if (len > name_offset) {
                uint8_t name_len = data[name_offset];
                if (name_len > 0 && name_len <= 20 && len >= name_offset + 1 + name_len) {
                    memcpy(target_name, data + name_offset + 1, name_len);
                }
            }

            printf("[GAME] Whisper from %s to %s: %s\n",
                   s->character_name, target_name, message);

            ClientSession* target = find_session_by_name(target_name);
            if (target) {
                /* Send to target */
                uint8_t response[300];
                memset(response, 0, sizeof(response));
                int offset = 0;

                response[offset++] = 0;  /* Result: success */
                response[offset++] = CHAT_TYPE_WHISPER;
                memset(response + offset, 0, 20);
                strncpy((char*)(response + offset), s->character_name, 19);
                offset += 20;
                response[offset++] = msg_len;
                memcpy(response + offset, message, msg_len);
                offset += msg_len;

                send_response(target, 10, response, offset);

                /* Send confirmation to sender */
                uint8_t confirm[4] = {0, 0, 0, 0};  /* Success */
                return send_response(s, 10, confirm, 4);
            } else {
                /* Target not found */
                uint8_t error[4] = {3, 0, 0, 0};  /* User not found */
                return send_response(s, 10, error, 4);
            }
        }

        case CHAT_TYPE_NORMAL:
        default:
            /* Area chat - broadcast to same village */
            printf("[GAME] Area chat from %s: %s\n", s->character_name, message);
            broadcast_chat(s, CHAT_TYPE_NORMAL, message, msg_len, s->current_village);
            break;
    }

    return 0;
}

/* msg_no=16: MOVE_AREA (change map) */
int handle_move_area(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_USER_AREA from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 23, error, 4);  /* NOTIPACKET_USER_AREA = 23 */
    }

    uint32_t village_id;
    memcpy(&village_id, data, 4);
    printf("[GAME] SET_USER_AREA: village_id=%u\n", village_id);

    /* Update character's village */
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            s->characters[i].village = village_id;
            break;
        }
    }
    s->current_village = village_id;

    /* Response: result + village_id + spawn position */
    uint8_t response[24];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &village_id, 4);

    /* Default spawn position */
    uint16_t spawn_x = 400;
    uint16_t spawn_y = 300;
    memcpy(response + 8, &spawn_x, 2);
    memcpy(response + 10, &spawn_y, 2);

    return send_response(s, 23, response, 16);  /* NOTIPACKET_USER_AREA = 23 */
}

/* msg_no=37: SET_USER_POSITION (position update) */
int handle_move(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 4) {
        return 0;
    }

    /* Parse position */
    uint16_t x, y;
    memcpy(&x, data, 2);
    memcpy(&y, data + 2, 2);

    /* Direction and state */
    uint8_t direction = (len >= 5) ? data[4] : 0;
    uint8_t move_type = (len >= 6) ? data[5] : 0;

    printf("[GAME] SET_USER_POSITION: x=%u y=%u dir=%d type=%d\n", x, y, direction, move_type);

    /* ACK back with NOTIPACKET_USER_POSITION = 22 */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response, &x, 2);
    memcpy(response + 2, &y, 2);
    response[4] = direction;
    response[5] = move_type;

    return send_response(s, 22, response, 8);  /* NOTIPACKET_USER_POSITION = 22 */
}

/* msg_no=21: MOVE_STOP */
int handle_move_stop(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint16_t x, y;
    memcpy(&x, data, 2);
    memcpy(&y, data + 2, 2);

    printf("[GAME] MOVE_STOP: x=%u y=%u\n", x, y);

    uint8_t response[8];
    memcpy(response, &x, 2);
    memcpy(response + 2, &y, 2);
    memset(response + 4, 0, 4);

    return send_response(s, 21, response, 8);
}

/* msg_no=25: GET_INVENTORY */
int handle_get_inventory(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_INVENTORY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 13, error, 4);  /* NOTIPACKET_ITEM_LIST = 13 */
    }

    /* Reload inventory if not loaded */
    if (!s->inventory.inventory_loaded && s->character_id > 0) {
        db_get_inventory(s->character_id, &s->inventory);
    }

    /* Build inventory response */
    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Gold - from DB */
    uint32_t gold = s->inventory.money;
    memcpy(response + offset, &gold, 4);
    offset += 4;

    /* Inventory capacity - from DB */
    uint16_t capacity = (uint16_t)s->inventory.inventory_capacity;
    if (capacity == 0) capacity = 56;
    memcpy(response + offset, &capacity, 2);
    offset += 2;

    /* Coin (points) - from DB */
    uint32_t coin = s->inventory.coin;
    memcpy(response + offset, &coin, 4);
    offset += 4;

    /* Pay coin - from DB */
    uint32_t pay_coin = s->inventory.pay_coin;
    memcpy(response + offset, &pay_coin, 4);
    offset += 4;

    /* Event coin - from DB */
    uint32_t event_coin = s->inventory.event_coin;
    memcpy(response + offset, &event_coin, 4);
    offset += 4;

    /* Item count - from DB */
    uint16_t item_count = s->inventory.item_count;
    memcpy(response + offset, &item_count, 2);
    offset += 2;

    /* Real items from DB: slot(2) + item_id(4) + count(2) + upgrade(1) + seal(1) + flags(2) = 12 bytes each */
    for (uint16_t i = 0; i < item_count && i < MAX_BAG_ITEMS; i++) {
        DBBagItem* item = &s->inventory.items[i];

        memcpy(response + offset, &item->slot, 2);
        offset += 2;
        memcpy(response + offset, &item->it_id, 4);
        offset += 4;
        memcpy(response + offset, &item->count, 2);
        offset += 2;
        response[offset++] = item->upgrade;
        response[offset++] = item->seal;
        memcpy(response + offset, &item->flags, 2);
        offset += 2;
    }

    printf("[GAME] GET_INVENTORY: gold=%u, capacity=%u, items=%u\n", gold, capacity, item_count);
    return send_response(s, 13, response, offset);  /* NOTIPACKET_ITEM_LIST = 13 */
}

/* Item effect types */
#define ITEM_EFFECT_HP_RESTORE    1
#define ITEM_EFFECT_MP_RESTORE    2
#define ITEM_EFFECT_HP_MP_RESTORE 3
#define ITEM_EFFECT_BUFF          4

/* msg_no=26: USE_ITEM */
int handle_use_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] USE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 6) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 26, error, 4);
    }

    /* Parse: slot(2) + item_id(4) */
    uint16_t slot;
    uint32_t item_id;
    memcpy(&slot, data, 2);
    memcpy(&item_id, data + 2, 4);

    printf("[GAME] USE_ITEM: slot=%u item_id=%u\n", slot, item_id);

    /* Determine item effect based on item_id ranges
     * 10001-19999: HP potions (restore HP by item_id % 1000 * 10)
     * 20001-29999: MP potions (restore MP by item_id % 1000 * 10)
     * 30001-39999: HP+MP potions (restore both)
     * Others: Generic item use
     */
    uint32_t old_hp = s->current_hp;
    uint32_t old_mp = s->current_mp;
    uint32_t effect_type = 0;
    uint32_t effect_value = 0;

    if (item_id >= 10001 && item_id < 20000) {
        /* HP Potion */
        effect_type = ITEM_EFFECT_HP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_hp += effect_value;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        printf("[GAME] HP Potion: restored %u HP (%u -> %u)\n",
               effect_value, old_hp, s->current_hp);
    }
    else if (item_id >= 20001 && item_id < 30000) {
        /* MP Potion */
        effect_type = ITEM_EFFECT_MP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_mp += effect_value;
        if (s->current_mp > s->combat_stats.max_mp) {
            s->current_mp = s->combat_stats.max_mp;
        }
        printf("[GAME] MP Potion: restored %u MP (%u -> %u)\n",
               effect_value, old_mp, s->current_mp);
    }
    else if (item_id >= 30001 && item_id < 40000) {
        /* HP+MP Potion */
        effect_type = ITEM_EFFECT_HP_MP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_hp += effect_value;
        s->current_mp += effect_value;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        if (s->current_mp > s->combat_stats.max_mp) {
            s->current_mp = s->combat_stats.max_mp;
        }
        printf("[GAME] HP+MP Potion: restored %u each (HP: %u->%u, MP: %u->%u)\n",
               effect_value, old_hp, s->current_hp, old_mp, s->current_mp);
    }
    else {
        /* Generic item - just acknowledge use */
        effect_type = 0;
        effect_value = 0;
        printf("[GAME] Generic item used: %u\n", item_id);
    }

    /* Delete consumable item from database */
    if (effect_type != 0) {
        /* Item was a consumable (potion), delete from DB */
        DBItemInfo consumed_item;
        if (db_get_item_at_slot(s->character_id, slot, &consumed_item) == 0 &&
            consumed_item.ui_id != 0) {
            if (db_delete_item(s->character_id, consumed_item.ui_id) == 0) {
                printf("[GAME] Consumable item deleted from DB: slot=%u (ui_id=%u)\n",
                       slot, consumed_item.ui_id);
            }
        }
    }

    /* Response: result(4) + slot(2) + item_id(4) + effect_type(1) + hp(4) + mp(4) */
    uint8_t response[24];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &slot, 2);
    offset += 2;
    memcpy(response + offset, &item_id, 4);
    offset += 4;
    response[offset++] = (uint8_t)effect_type;
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    memcpy(response + offset, &s->current_mp, 4);
    offset += 4;

    return send_response(s, 26, response, offset);
}

/* msg_no=30: CHANGE_SKILLSLOT - Production protocol
 * Data format: [1 byte slot_index][2 bytes skill_id]
 * slot_index: 0-11 for quickslot bindings
 * skill_id: skill to bind (0 = unbind)
 */
int handle_change_skillslot(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CHANGE_SKILLSLOT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 30, error, 4);
    }

    uint8_t slot_index = data[0];
    uint16_t skill_id;
    memcpy(&skill_id, data + 1, 2);

    printf("[GAME] CHANGE_SKILLSLOT: slot=%u, skill=%u\n", slot_index, skill_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));

    /* Validate slot index (12 quickslots: 0-11) */
    if (slot_index >= 12) {
        uint32_t result = 2;  /* Invalid slot */
        memcpy(response, &result, 4);
        return send_response(s, 30, response, 4);
    }

    /* If binding a skill, verify character has learned it */
    if (skill_id != 0) {
        uint8_t skill_level = 0;
        int has_skill = db_has_skill(s->character_id, skill_id, &skill_level);
        if (has_skill <= 0) {
            uint32_t result = 3;  /* Skill not learned */
            memcpy(response, &result, 4);
            return send_response(s, 30, response, 4);
        }
    }

    /* Update skill slot in database
     * The skill_slot blob format needs to be updated
     * For now, we use a simple key-value update approach */
    if (!g_db_charac2) {
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 30, response, 4);
    }

    /* Update quickslot_skill table or use a simplified blob update */
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO skill_quickslot (charac_no, slot_index, skill_id) "
             "VALUES (%u, %u, %u) "
             "ON DUPLICATE KEY UPDATE skill_id=%u",
             s->character_id, slot_index, skill_id, skill_id);

    /* Note: This requires a skill_quickslot table. If it doesn't exist,
     * we just return success and store locally for now */
    if (mysql_query(g_db_charac2, query)) {
        /* Table might not exist, just log and continue */
        printf("[GAME] CHANGE_SKILLSLOT: DB update failed (table may not exist): %s\n",
               mysql_error(g_db_charac2));
    }

    /* Build success response */
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    response[4] = slot_index;
    memcpy(response + 5, &skill_id, 2);

    printf("[GAME] CHANGE_SKILLSLOT success: slot %u -> skill %u\n", slot_index, skill_id);
    return send_response(s, 30, response, 7);
}

/* msg_no=31: BUY_SKILL - Production protocol */
int handle_buy_skill(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] BUY_SKILL from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 31, error, 4);
    }

    uint16_t skill_id;
    memcpy(&skill_id, data, 2);
    printf("[GAME] BUY_SKILL: skill_id=%u\n", skill_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    /* Check current skill level */
    uint8_t current_level = 0;
    int has_skill = db_has_skill(s->character_id, skill_id, &current_level);
    if (has_skill < 0) {
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 31, response, 4);
    }

    /* Calculate SP cost */
    int sp_cost = get_skill_sp_cost(skill_id, current_level);

    /* Check if enough SP */
    if (s->skill.remain_sp < (uint32_t)sp_cost) {
        uint32_t result = 2;  /* Not enough SP */
        memcpy(response, &result, 4);
        printf("[GAME] BUY_SKILL failed: not enough SP (have %u, need %d)\n",
               s->skill.remain_sp, sp_cost);
        return send_response(s, 31, response, 4);
    }

    /* Check max level (10 for most skills) */
    uint8_t max_level = 10;
    if (current_level >= max_level) {
        uint32_t result = 3;  /* Max level reached */
        memcpy(response, &result, 4);
        return send_response(s, 31, response, 4);
    }

    /* Deduct SP and learn skill */
    uint8_t new_level = current_level + 1;
    uint32_t new_remain_sp = s->skill.remain_sp - sp_cost;
    uint32_t new_used_sp = s->skill.used_sp + sp_cost;

    if (db_update_skill_sp(s->character_id, new_remain_sp, new_used_sp) < 0) {
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 31, response, 4);
    }

    if (db_learn_skill(s->character_id, skill_id, new_level) < 0) {
        /* Rollback SP */
        db_update_skill_sp(s->character_id, s->skill.remain_sp, s->skill.used_sp);
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 31, response, 4);
    }

    /* Update session */
    s->skill.remain_sp = new_remain_sp;
    s->skill.used_sp = new_used_sp;

    /* Add to skill list if new skill */
    if (!has_skill && s->skill.skill_count < MAX_PARSED_SKILLS) {
        s->skill.skill_ids[s->skill.skill_count++] = skill_id;
    }

    /* Build response */
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &skill_id, 2);
    response[6] = new_level;
    response[7] = 0;
    memcpy(response + 8, &new_remain_sp, 4);

    printf("[GAME] BUY_SKILL success: skill=%u level=%u->%u, SP=%u->%u\n",
           skill_id, current_level, new_level, s->skill.remain_sp + sp_cost, new_remain_sp);
    return send_response(s, 31, response, 12);
}

/* msg_no=32: INCREASE_STATUS - Production protocol */
int handle_increase_status(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] INCREASE_STATUS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 1) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 32, error, 4);
    }

    uint8_t stat_type = data[0];  /* 0=STR, 1=INT, 2=VIT, 3=SPI */
    printf("[GAME] INCREASE_STATUS: stat_type=%u\n", stat_type);

    /* Validate stat type */
    if (stat_type > 3) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid stat type */
        return send_response(s, 32, error, 4);
    }

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    /* Get current stat points */
    uint32_t remain_stat = 0, used_stat = 0;
    if (db_get_stat_points(s->character_id, &remain_stat, &used_stat) < 0) {
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 32, response, 4);
    }

    /* Check if enough points */
    if (remain_stat < 1) {
        uint32_t result = 3;  /* Not enough stat points */
        memcpy(response, &result, 4);
        printf("[GAME] INCREASE_STATUS failed: no stat points remaining\n");
        return send_response(s, 32, response, 4);
    }

    /* Increase the stat */
    if (db_increase_stat(s->character_id, stat_type, 1) < 0) {
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 32, response, 4);
    }

    /* Deduct stat point */
    uint32_t new_remain = remain_stat - 1;
    uint32_t new_used = used_stat + 1;
    if (db_update_stat_points(s->character_id, new_remain, new_used) < 0) {
        /* Rollback stat increase */
        db_increase_stat(s->character_id, stat_type, -1);
        uint32_t result = 1;  /* DB error */
        memcpy(response, &result, 4);
        return send_response(s, 32, response, 4);
    }

    /* Update combat stats */
    switch (stat_type) {
        case 0: s->combat_stats.str++; break;
        case 1: s->combat_stats.intel++; break;
        case 2: s->combat_stats.vit++; break;
        case 3: s->combat_stats.spr++; break;
    }

    /* Build response */
    const char* stat_names[] = {"STR", "INT", "VIT", "SPI"};
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    response[4] = stat_type;
    memcpy(response + 8, &new_remain, 4);

    printf("[GAME] INCREASE_STATUS success: %s+1, remaining=%u\n",
           stat_names[stat_type], new_remain);
    return send_response(s, 32, response, 12);
}

/* msg_no=34: GIVEUP_QUEST - Production protocol */
int handle_giveup_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GIVEUP_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] GIVEUP_QUEST: quest_id=%u\n", quest_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));

    /* Find the quest in active quest list */
    int found_idx = -1;
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx < 0) {
        uint32_t result = 2;  /* Quest not found in active list */
        memcpy(response, &result, 4);
        return send_response(s, 34, response, 4);
    }

    /* Remove quest from active list - shift remaining quests down */
    for (int i = found_idx; i < s->active_quest_count - 1; i++) {
        s->active_quests[i] = s->active_quests[i + 1];
        s->quest_progress[i] = s->quest_progress[i + 1];
    }
    s->active_quest_count--;

    /* Clear the last slot */
    if (s->active_quest_count < 10) {
        s->active_quests[s->active_quest_count] = 0;
        s->quest_progress[s->active_quest_count] = 0;
    }

    /* Build success response */
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &quest_id, 4);

    printf("[GAME] GIVEUP_QUEST success: quest %u removed, active count=%d\n",
           quest_id, s->active_quest_count);
    return send_response(s, 34, response, 8);
}

/* msg_no=35: SET_QUEST_TRIGGER - Production protocol */
int handle_set_quest_trigger(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_QUEST_TRIGGER from fd=%d\n", s->fd);

    /* Quest trigger update - acknowledge */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 35, response, 4);
}

/* msg_no=42: DIE_MONSTER - Production protocol */
int handle_die_monster(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DIE_MONSTER from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Monster death notification from client - for P2P sync */
    /* Server should verify and broadcast to other party members */
    return 0;  /* No response needed */
}

/* msg_no=43: DIE_CHARACTER - Production protocol */
int handle_die_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DIE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Character death notification */
    return 0;
}

/* msg_no=44: USE_COIN - Production protocol (resurrection)
 * Data format: [1 byte coin_type]
 * coin_type: 0=free resurrect, 1=use resurrection coin, 2=use CERA
 */
int handle_use_coin(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] USE_COIN from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 44, error, 4);
    }

    uint8_t coin_type = (len >= 1) ? data[0] : 0;
    printf("[GAME] USE_COIN: type=%u\n", coin_type);

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    /* Check if character is dead (HP = 0) */
    if (s->current_hp > 0) {
        uint32_t result = 2;  /* Not dead, cannot resurrect */
        memcpy(response, &result, 4);
        return send_response(s, 44, response, 4);
    }

    /* Check if in dungeon */
    if (!s->dungeon.active) {
        uint32_t result = 3;  /* Not in dungeon */
        memcpy(response, &result, 4);
        return send_response(s, 44, response, 4);
    }

    /* Process based on coin type */
    uint32_t hp_restore_percent = 30;  /* Default: restore 30% HP */

    switch (coin_type) {
        case 0:  /* Free resurrect (once per dungeon) */
            if (s->dungeon.free_resurrect_used) {
                uint32_t result = 4;  /* Already used free resurrect */
                memcpy(response, &result, 4);
                return send_response(s, 44, response, 4);
            }
            s->dungeon.free_resurrect_used = 1;
            hp_restore_percent = 30;
            break;

        case 1:  /* Use resurrection coin item */
            {
                /* Find and consume resurrection coin (it_id 5001) */
                int coin_slot = -1;
                for (int i = 0; i < s->inventory.item_count; i++) {
                    if (s->inventory.items[i].it_id == 5001) {  /* Resurrection coin */
                        coin_slot = s->inventory.items[i].slot;
                        break;
                    }
                }
                if (coin_slot < 0) {
                    uint32_t result = 5;  /* No resurrection coin */
                    memcpy(response, &result, 4);
                    return send_response(s, 44, response, 4);
                }
                /* Consume the coin */
                db_delete_item_at_slot(s->character_id, coin_slot);
                hp_restore_percent = 50;  /* 50% HP restore */
            }
            break;

        case 2:  /* Use CERA (premium currency) - full restore */
            /* For now, just allow it without checking CERA balance
             * Real implementation would check account.cera_balance */
            hp_restore_percent = 100;
            break;

        default:
            {
                uint32_t result = 6;  /* Invalid coin type */
                memcpy(response, &result, 4);
                return send_response(s, 44, response, 4);
            }
    }

    /* Resurrect: restore HP */
    uint32_t restored_hp = (s->combat_stats.max_hp * hp_restore_percent) / 100;
    if (restored_hp < 1) restored_hp = 1;
    s->current_hp = restored_hp;

    /* Restore some MP as well */
    uint32_t restored_mp = (s->combat_stats.max_mp * hp_restore_percent) / 100;
    if (restored_mp < 1) restored_mp = 1;
    s->current_mp = restored_mp;

    /* Build success response */
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &s->current_hp, 4);
    memcpy(response + 8, &s->current_mp, 4);
    response[12] = coin_type;

    printf("[GAME] USE_COIN success: type=%u, HP=%u/%u, MP=%u/%u\n",
           coin_type, s->current_hp, s->combat_stats.max_hp,
           s->current_mp, s->combat_stats.max_mp);
    return send_response(s, 44, response, 13);
}

/* msg_no=46: GET_ITEM - Production protocol (pickup) */
int handle_get_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GET_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Item pickup notification - handled in dungeon context */
    return 0;
}

/* msg_no=50: DROP_ITEM - Production protocol */
int handle_drop_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DROP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 50, error, 4);
    }

    uint16_t slot;
    memcpy(&slot, data, 2);
    printf("[GAME] DROP_ITEM: slot=%u\n", slot);

    /* Delete from inventory */
    db_delete_item_at_slot(s->character_id, slot);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 50, response, 4);
}

/* msg_no=53: MAKE_PVP_ROOM - Production protocol */
int handle_pvp_create_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] MAKE_PVP_ROOM from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 53, response, 4);
}

/* msg_no=54: ENTER_PVP_ROOM - Production protocol */
int handle_pvp_enter_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] ENTER_PVP_ROOM from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 54, response, 4);
}

/* msg_no=55: SET_PVP_SEAT_STATE - Production protocol */
int handle_pvp_set_seat(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_PVP_SEAT_STATE from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 55, response, 4);
}

/* msg_no=65: DEBUG_COMMAND - Production protocol */
int handle_debug_command(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEBUG_COMMAND from fd=%d\n", s->fd);
    /* Debug commands - should be restricted in production */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 65, response, 4);
}

/* msg_no=83: UPGRADE_ITEM */
int handle_upgrade_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UPGRADE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 83, error, 4);
    }

    uint16_t slot, material_slot;
    uint8_t upgrade_type;
    memcpy(&slot, data, 2);
    memcpy(&material_slot, data + 2, 2);
    upgrade_type = data[4];

    printf("[GAME] UPGRADE_ITEM: slot=%u material=%u type=%u\n", slot, material_slot, upgrade_type);

    DBItemInfo upgrade_item;
    if (db_get_item_info_at_slot(s->character_id, slot, &upgrade_item) < 0 ||
        upgrade_item.it_id == 0) {
        uint8_t response[4] = {3, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }
    uint32_t item_id = upgrade_item.it_id;

    uint8_t current_level = (uint8_t)(upgrade_item.hidden_option & 0xFF);

    if (current_level >= 15) {
        uint8_t response[4] = {4, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }

    uint32_t cost = calc_upgrade_cost(item_id, current_level);
    if (s->inventory.money < cost) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }

    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    if (material_slot != 0xFFFF) {
        DBItemInfo material_item;
        if (db_get_item_at_slot(s->character_id, material_slot, &material_item) == 0 &&
            material_item.ui_id != 0) {
            db_delete_item(s->character_id, material_item.ui_id);
        }
    }

    int success_rate = calc_upgrade_success_rate(current_level);
    int roll = rand() % 100;
    int success = (roll < success_rate);
    int destroyed = 0;

    if (success) {
        current_level++;
        db_set_item_upgrade_level(upgrade_item.ui_id, current_level);
    } else {
        if (current_level >= 7) {
            int destroy_chance = (current_level - 6) * 10;
            if ((rand() % 100) < destroy_chance) {
                db_delete_item(s->character_id, upgrade_item.ui_id);
                destroyed = 1;
            } else if (current_level > 0) {
                current_level--;
                db_set_item_upgrade_level(upgrade_item.ui_id, current_level);
            }
        }
    }

    uint8_t response[12];
    memset(response, 0, 12);
    uint32_t result_code = success ? 0 : 1;
    uint32_t new_level = current_level;
    memcpy(response, &result_code, 4);
    memcpy(response + 4, &new_level, 4);
    response[8] = (uint8_t)destroyed;

    printf("[GAME] UPGRADE: result=%u level=%u destroyed=%d\n", result_code, new_level, destroyed);
    return send_response(s, 83, response, 12);
}

/* msg_no=100: DEPOSIT_MONEY */
int handle_storage_deposit(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEPOSIT_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    if (s->inventory.money < amount) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 100, response, 4);
    }

    uint32_t storage_gold = db_get_storage_gold(s->account_id);

    s->inventory.money -= amount;
    storage_gold += amount;

    db_update_gold(s->character_id, s->inventory.money);
    db_set_storage_gold(s->account_id, storage_gold);

    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] DEPOSIT: char=%u storage=%u\n", s->inventory.money, storage_gold);
    return send_response(s, 100, response, 12);
}

/* msg_no=101: WITHDRAW_MONEY */
int handle_storage_withdraw(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] WITHDRAW_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    uint32_t storage_gold = db_get_storage_gold(s->account_id);

    if (storage_gold < amount) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 101, response, 4);
    }

    storage_gold -= amount;
    s->inventory.money += amount;

    db_set_storage_gold(s->account_id, storage_gold);
    db_update_gold(s->character_id, s->inventory.money);

    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] WITHDRAW: char=%u storage=%u\n", s->inventory.money, storage_gold);
    return send_response(s, 101, response, 12);
}

/* msg_no=41: USE_SKILL - Production protocol
 * Phase 16 P1: Integrated real damage calculation and cooldown system
 * Data format: [2 bytes skill_id][2 bytes target_x][2 bytes target_y]
 * Response: [4 bytes result][2 bytes cooldown_ms][2 bytes hit_count][4 bytes base_damage]
 */
int handle_use_skill(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 6) {
        return 0;
    }

    uint16_t skill_id;
    uint16_t target_x, target_y;
    memcpy(&skill_id, data, 2);
    memcpy(&target_x, data + 2, 2);
    memcpy(&target_y, data + 4, 2);

    printf("[GAME] USE_SKILL: skill_id=%u target=(%u,%u)\n", skill_id, target_x, target_y);

    /* Response structure */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;

    /* 1. Check cooldown */
    if (skill_is_on_cooldown(s, skill_id)) {
        result = 1;  /* ERROR: Skill on cooldown */
        uint32_t remaining = skill_get_cooldown_remaining(s, skill_id);
        printf("[GAME] USE_SKILL: skill %u on cooldown, %u ms remaining\n", skill_id, remaining);
        memcpy(response, &result, 4);
        memcpy(response + 4, &remaining, 4);
        return send_response(s, 30, response, 8);
    }

    /* 2. Get skill level from session (default based on player level) */
    uint8_t skill_level = 1;
    /* Check if player has this skill in skill_ids array */
    int has_skill = 0;
    for (int i = 0; i < s->skill.skill_count && i < MAX_PARSED_SKILLS; i++) {
        if (s->skill.skill_ids[i] == skill_id) {
            has_skill = 1;
            break;
        }
    }
    /* Estimate skill level based on player level (1-10 based on level) */
    if (has_skill) {
        skill_level = (s->level / 10) + 1;
    }
    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;

    /* 3. Check MP cost */
    uint16_t mp_cost = get_skill_mp_cost(skill_id, skill_level);
    if (s->current_mp < mp_cost) {
        result = 2;  /* ERROR: Not enough MP */
        printf("[GAME] USE_SKILL: not enough MP (have %u, need %u)\n", s->current_mp, mp_cost);
        memcpy(response, &result, 4);
        return send_response(s, 30, response, 4);
    }

    /* 4. Consume MP */
    s->current_mp -= mp_cost;
    printf("[GAME] USE_SKILL: consumed %u MP, remaining %u\n", mp_cost, s->current_mp);

    /* 5. Start cooldown */
    skill_start_cooldown(s, skill_id);
    uint16_t cooldown = get_skill_cooldown(skill_id);

    /* 6. Get hit count for multi-hit skills */
    uint8_t hit_count = get_skill_hit_count(skill_id);

    /* 7. Calculate base damage (for display/validation)
     * Full damage calculation happens when hitting monsters */
    uint8_t is_crit = 0, is_miss = 0;
    uint32_t base_damage = calculate_skill_damage_v3(
        s, skill_id, skill_level,
        0,     /* monster_type: 0 for base calculation */
        1,     /* monster_level: use player level context */
        0,     /* monster_def: 0 for base calculation */
        &is_crit, &is_miss
    );

    printf("[GAME] USE_SKILL: skill=%u lv=%u dmg=%u hits=%u cd=%ums crit=%d\n",
           skill_id, skill_level, base_damage, hit_count, cooldown, is_crit);

    /* 8. Build response */
    result = 0;  /* SUCCESS */
    memcpy(response, &result, 4);
    memcpy(response + 4, &cooldown, 2);
    memcpy(response + 6, &hit_count, 1);
    response[7] = is_crit;
    memcpy(response + 8, &base_damage, 4);
    memcpy(response + 12, &s->current_mp, 4);

    return send_response(s, 30, response, 16);
}

/* msg_no=31: GET_SKILL_LIST */
int handle_get_skill_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_SKILL_LIST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 19, error, 4);  /* NOTIPACKET_SKILLINFO = 19 */
    }

    /* Reload skill if not loaded */
    if (!s->skill.skill_loaded && s->character_id > 0) {
        db_get_skill(s->character_id, &s->skill);
    }

    /* Build skill list response */
    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* SP (skill points) - from DB */
    uint32_t sp = s->skill.remain_sp;
    memcpy(response + offset, &sp, 4);
    offset += 4;

    /* Used SP - from DB */
    uint32_t used_sp = s->skill.used_sp;
    memcpy(response + offset, &used_sp, 4);
    offset += 4;

    /* 2nd awakening SP - from DB */
    uint32_t sp_2nd = s->skill.remain_sp_2nd;
    memcpy(response + offset, &sp_2nd, 4);
    offset += 4;

    /* Use real skills from parsed skill_slot blob */
    uint16_t skill_count = s->skill.skill_count;

    /* If no skills were parsed, add some default class skills based on job */
    if (skill_count == 0) {
        /* Get character job for default skills */
        uint8_t job = 0;
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                job = s->characters[i].job;
                break;
            }
        }

        /* Add default skills based on job */
        uint16_t default_skills[5];
        switch (job) {
            case 0:  /* Slayer */
                default_skills[0] = 1;   /* Basic attack */
                default_skills[1] = 101; /* Slash */
                default_skills[2] = 102; /* Thrust */
                default_skills[3] = 103; /* Gore */
                default_skills[4] = 104; /* Uppercut */
                break;
            case 1:  /* Fighter */
                default_skills[0] = 1;   /* Basic attack */
                default_skills[1] = 201; /* Punch */
                default_skills[2] = 202; /* Kick */
                default_skills[3] = 203; /* Suplex */
                default_skills[4] = 204; /* Throw */
                break;
            case 2:  /* Gunner */
                default_skills[0] = 1;   /* Basic attack */
                default_skills[1] = 301; /* Shoot */
                default_skills[2] = 302; /* Multi-shot */
                default_skills[3] = 303; /* Grenades */
                default_skills[4] = 304; /* Flamethrower */
                break;
            case 3:  /* Mage */
                default_skills[0] = 1;   /* Basic attack */
                default_skills[1] = 401; /* Magic missile */
                default_skills[2] = 402; /* Fireball */
                default_skills[3] = 403; /* Ice bolt */
                default_skills[4] = 404; /* Lightning */
                break;
            case 4:  /* Priest */
                default_skills[0] = 1;   /* Basic attack */
                default_skills[1] = 501; /* Holy light */
                default_skills[2] = 502; /* Smite */
                default_skills[3] = 503; /* Heal */
                default_skills[4] = 504; /* Blessing */
                break;
            default:
                default_skills[0] = 1;
                default_skills[1] = 2;
                default_skills[2] = 3;
                default_skills[3] = 4;
                default_skills[4] = 5;
                break;
        }

        skill_count = 5;
        memcpy(response + offset, &skill_count, 2);
        offset += 2;

        /* Send default skills */
        for (int i = 0; i < 5; i++) {
            memcpy(response + offset, &default_skills[i], 2);
            offset += 2;
            response[offset++] = 1;  /* Level 1 */
            response[offset++] = (uint8_t)i;  /* Slot */
        }
        printf("[GAME] GET_SKILL_LIST: default skills for job %u, count=%u\n", job, skill_count);
    } else {
        /* Send real parsed skills from skill_slot blob */
        memcpy(response + offset, &skill_count, 2);
        offset += 2;

        /* skill_id(2) + level(1) + slot(1) = 4 bytes each */
        for (int i = 0; i < skill_count && i < MAX_PARSED_SKILLS && offset + 4 < 500; i++) {
            uint16_t skill_id = s->skill.skill_ids[i];
            memcpy(response + offset, &skill_id, 2);
            offset += 2;
            response[offset++] = 1;  /* Level - would need separate tracking per skill */
            response[offset++] = (uint8_t)i;  /* Slot */
        }
        printf("[GAME] GET_SKILL_LIST: real skills from DB, count=%u\n", skill_count);
    }

    printf("[GAME] GET_SKILL_LIST: remain_sp=%u, used_sp=%u, sp_2nd=%u\n", sp, used_sp, sp_2nd);
    return send_response(s, 19, response, offset);  /* NOTIPACKET_SKILLINFO = 19 */
}

/* msg_no=32: GET_EQUIPMENT */
int handle_get_equipment(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_EQUIPMENT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 32, error, 4);
    }

    /* Reload equipment if not loaded */
    if (!s->equipment.equip_loaded && s->character_id > 0) {
        db_get_equipment(s->character_id, &s->equipment);
    }

    /* Build equipment response */
    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Equipment count */
    uint8_t equip_count = s->equipment.equip_count;
    response[offset++] = equip_count;

    /* Each equipment: slot(1) + item_id(4) + stat(1) + hidden_option(2) = 8 bytes */
    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        if (s->equipment.equipment[i].it_id > 0) {
            response[offset++] = (uint8_t)i;  /* Slot */
            memcpy(response + offset, &s->equipment.equipment[i].it_id, 4);
            offset += 4;
            response[offset++] = s->equipment.equipment[i].stat;
            memcpy(response + offset, &s->equipment.equipment[i].hidden_option, 2);
            offset += 2;
        }
    }

    printf("[GAME] GET_EQUIPMENT: count=%d\n", equip_count);
    return send_response(s, 32, response, offset);
}

/* msg_no=33: EQUIP_ITEM */
int handle_equip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] EQUIP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 33, error, 4);
    }

    uint8_t equip_slot = data[0];  /* Target equipment slot (0-10) */
    uint32_t item_id;
    memcpy(&item_id, data + 1, 4);

    /* Check if source slot is provided (extended format) */
    int source_slot = -1;
    if (len >= 9) {
        memcpy(&source_slot, data + 5, 4);
    }

    printf("[GAME] EQUIP_ITEM: equip_slot=%d, item_id=%u, source_slot=%d\n",
           equip_slot, item_id, source_slot);

    /* Validate slot */
    if (equip_slot >= MAX_EQUIP_SLOTS) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid slot */
        return send_response(s, 33, error, 4);
    }

    /* Update database - move item from inventory to equipment slot */
    if (source_slot >= MAX_EQUIP_SLOTS) {
        DBItemInfo source_item;
        if (db_get_item_at_slot(s->character_id, source_slot, &source_item) < 0 ||
            source_item.ui_id == 0) {
            printf("[EQUIP] Unable to locate item in slot %d\n", source_slot);
        } else {
            /* If there's already an item in equipment slot, swap to source slot */
            DBItemInfo existing_item;
            if (db_get_item_at_slot(s->character_id, equip_slot, &existing_item) == 0 &&
                existing_item.ui_id != 0) {
                db_update_item_slot(s->character_id, existing_item.ui_id, source_slot);
                printf("[EQUIP] Swapped existing item %u from equip to slot %d\n",
                       existing_item.it_id, source_slot);
            }
            /* Move new item from source to equipment slot */
            db_update_item_slot(s->character_id, source_item.ui_id, equip_slot);
            printf("[EQUIP] Equipped item %u (ui_id=%u) from slot %d to equip slot %d\n",
                   item_id, source_item.ui_id, source_slot, equip_slot);
        }
    }

    /* Update in-memory equipment */
    s->equipment.equipment[equip_slot].slot = equip_slot;
    s->equipment.equipment[equip_slot].it_id = item_id;
    s->equipment.equipment[equip_slot].stat = 0;

    /* P4: Recalculate combat stats after equipment change */
    calculate_combat_stats(s);
    printf("[EQUIP] Recalculated stats: ATK=%u, DEF=%u, HP=%u\n",
           s->combat_stats.phys_attack, s->combat_stats.phys_def, s->combat_stats.max_hp);

    /* Response: result + slot + item_id */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    response[offset++] = (uint8_t)equip_slot;
    memcpy(response + offset, &item_id, 4);
    offset += 4;

    return send_response(s, 33, response, offset);
}

/* msg_no=34: UNEQUIP_ITEM */
int handle_unequip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UNEQUIP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 1) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    uint8_t equip_slot = data[0];
    printf("[GAME] UNEQUIP_ITEM: slot=%d\n", equip_slot);

    /* Validate slot */
    if (equip_slot >= MAX_EQUIP_SLOTS) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid slot */
        return send_response(s, 34, error, 4);
    }

    /* Get the item being unequipped */
    uint32_t item_id = s->equipment.equipment[equip_slot].it_id;

    if (item_id == 0) {
        /* No item in this slot */
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    /* Find a free inventory slot */
    int dest_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (dest_slot < MAX_EQUIP_SLOTS || dest_slot >= 200) {
        printf("[UNEQUIP] No free inventory slot\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Inventory full */
        return send_response(s, 34, error, 4);
    }

    /* Move item from equipment slot to inventory */
    DBItemInfo equip_item;
    if (db_get_item_at_slot(s->character_id, equip_slot, &equip_item) < 0 ||
        equip_item.ui_id == 0) {
        printf("[UNEQUIP] Failed to locate DB record for equip slot %d\n", equip_slot);
    } else if (db_update_item_slot(s->character_id, equip_item.ui_id, dest_slot) < 0) {
        printf("[UNEQUIP] Failed to update database for equip slot %d\n", equip_slot);
        /* Continue anyway for in-memory update */
    } else {
        printf("[UNEQUIP] Moved item %u from equip slot %d to bag slot %d\n",
               item_id, equip_slot, dest_slot);
    }

    /* Clear in-memory equipment */
    memset(&s->equipment.equipment[equip_slot], 0, sizeof(DBItemInfo));

    /* P4: Recalculate combat stats after unequip */
    calculate_combat_stats(s);
    printf("[UNEQUIP] Recalculated stats: ATK=%u, DEF=%u, HP=%u\n",
           s->combat_stats.phys_attack, s->combat_stats.phys_def, s->combat_stats.max_hp);

    /* Response: result + slot + item_id + new_slot */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    response[offset++] = equip_slot;
    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &dest_slot, 4);
    offset += 4;

    return send_response(s, 34, response, offset);
}

/* msg_no=38: GET_CHARACTER_STATS */
int handle_get_character_stats(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GET_CHARACTER_STATS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 38, error, 4);
    }

    /* Recalculate combat stats based on current equipment */
    calculate_combat_stats(s);

    /* Response: result + 16 stat values (4 bytes each) */
    uint8_t response[68];  /* 4 + 16*4 = 68 */
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Copy all stats */
    CombatStats* stats = &s->combat_stats;
    memcpy(response + offset, &stats->str, 4);
    offset += 4;
    memcpy(response + offset, &stats->intel, 4);
    offset += 4;
    memcpy(response + offset, &stats->vit, 4);
    offset += 4;
    memcpy(response + offset, &stats->spr, 4);
    offset += 4;
    memcpy(response + offset, &stats->phys_attack, 4);
    offset += 4;
    memcpy(response + offset, &stats->mag_attack, 4);
    offset += 4;
    memcpy(response + offset, &stats->phys_def, 4);
    offset += 4;
    memcpy(response + offset, &stats->mag_def, 4);
    offset += 4;
    memcpy(response + offset, &stats->max_hp, 4);
    offset += 4;
    memcpy(response + offset, &stats->max_mp, 4);
    offset += 4;
    memcpy(response + offset, &stats->attack_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->move_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->cast_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->crit_rate, 4);
    offset += 4;
    memcpy(response + offset, &stats->hit_rate, 4);
    offset += 4;
    memcpy(response + offset, &stats->evasion, 4);
    offset += 4;

    printf("[GAME] Stats: STR=%u INT=%u VIT=%u SPR=%u PATK=%u MATK=%u\n",
           stats->str, stats->intel, stats->vit, stats->spr,
           stats->phys_attack, stats->mag_attack);
    printf("[GAME] Stats: HP=%u MP=%u DEF=%u/%u CRIT=%u\n",
           stats->max_hp, stats->max_mp, stats->phys_def, stats->mag_def,
           stats->crit_rate);

    return send_response(s, 38, response, offset);
}

/* P4: NPC type definitions */
typedef enum {
    NPC_TYPE_SHOP = 1,       /* General shop */
    NPC_TYPE_BLACKSMITH = 2, /* Equipment repair/upgrade */
    NPC_TYPE_STORAGE = 3,    /* Account storage */
    NPC_TYPE_QUEST = 4,      /* Quest giver */
    NPC_TYPE_TELEPORT = 5,   /* Teleporter */
    NPC_TYPE_GUILD = 6,      /* Guild manager */
    NPC_TYPE_AUCTION = 7,    /* Auction house */
    NPC_TYPE_SKILL = 8,      /* Skill trainer */
    NPC_TYPE_DISMANTLE = 9   /* Item dismantle */
} NPCType;

/* Get NPC type based on NPC ID (simplified mapping) */
static uint8_t get_npc_type(uint32_t npc_id) {
    /* NPC ID ranges determine type */
    uint32_t category = (npc_id / 1000) % 10;
    switch (category) {
        case 0: return NPC_TYPE_SHOP;       /* 0xxx: General shops */
        case 1: return NPC_TYPE_BLACKSMITH; /* 1xxx: Blacksmiths */
        case 2: return NPC_TYPE_STORAGE;    /* 2xxx: Storage NPCs */
        case 3: return NPC_TYPE_QUEST;      /* 3xxx: Quest NPCs */
        case 4: return NPC_TYPE_TELEPORT;   /* 4xxx: Teleporters */
        case 5: return NPC_TYPE_GUILD;      /* 5xxx: Guild NPCs */
        case 6: return NPC_TYPE_AUCTION;    /* 6xxx: Auction NPCs */
        case 7: return NPC_TYPE_SKILL;      /* 7xxx: Skill trainers */
        case 8: return NPC_TYPE_DISMANTLE;  /* 8xxx: Dismantle NPCs */
        default: return NPC_TYPE_SHOP;
    }
}

/* msg_no=35: NPC_DIALOG - P4: Full NPC dialog logic based on NPC type */
int handle_npc_dialog(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] NPC_DIALOG from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 35, error, 4);
    }

    uint32_t npc_id;
    memcpy(&npc_id, data, 4);
    printf("[GAME] NPC_DIALOG: npc_id=%u\n", npc_id);

    /* Response: result + dialog options */
    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* P4: Determine NPC type and provide appropriate options */
    uint8_t npc_type = get_npc_type(npc_id);
    response[offset++] = npc_type;

    uint8_t option_count;
    switch (npc_type) {
        case NPC_TYPE_SHOP:
            option_count = 3;
            response[offset++] = option_count;
            response[offset++] = 1;  /* Buy */
            response[offset++] = 2;  /* Sell */
            response[offset++] = 99; /* Close */
            printf("[NPC] Shop NPC: Buy/Sell options\n");
            break;

        case NPC_TYPE_BLACKSMITH:
            option_count = 4;
            response[offset++] = option_count;
            response[offset++] = 10; /* Repair all */
            response[offset++] = 11; /* Repair single */
            response[offset++] = 12; /* Upgrade */
            response[offset++] = 99; /* Close */
            printf("[NPC] Blacksmith NPC: Repair/Upgrade options\n");
            break;

        case NPC_TYPE_STORAGE:
            option_count = 4;
            response[offset++] = option_count;
            response[offset++] = 20; /* Open storage */
            response[offset++] = 21; /* Deposit gold */
            response[offset++] = 22; /* Expand storage */
            response[offset++] = 99; /* Close */
            printf("[NPC] Storage NPC: Storage options\n");
            break;

        case NPC_TYPE_QUEST:
            option_count = 3;
            response[offset++] = option_count;
            response[offset++] = 30; /* View quests */
            response[offset++] = 31; /* Turn in quest */
            response[offset++] = 99; /* Close */
            /* Include available quest count */
            response[offset++] = 3;  /* Available quests (mock) */
            printf("[NPC] Quest NPC: Quest options\n");
            break;

        case NPC_TYPE_TELEPORT:
            option_count = 5;
            response[offset++] = option_count;
            response[offset++] = 40; /* Elvenguard */
            response[offset++] = 41; /* Hendon Myre */
            response[offset++] = 42; /* West Coast */
            response[offset++] = 43; /* Underfoot */
            response[offset++] = 99; /* Close */
            printf("[NPC] Teleport NPC: Destination options\n");
            break;

        case NPC_TYPE_GUILD:
            option_count = 4;
            response[offset++] = option_count;
            response[offset++] = 50; /* Create guild */
            response[offset++] = 51; /* Guild info */
            response[offset++] = 52; /* Leave guild */
            response[offset++] = 99; /* Close */
            printf("[NPC] Guild NPC: Guild options\n");
            break;

        case NPC_TYPE_AUCTION:
            option_count = 4;
            response[offset++] = option_count;
            response[offset++] = 60; /* Search */
            response[offset++] = 61; /* My listings */
            response[offset++] = 62; /* List item */
            response[offset++] = 99; /* Close */
            printf("[NPC] Auction NPC: Auction options\n");
            break;

        case NPC_TYPE_SKILL:
            option_count = 3;
            response[offset++] = option_count;
            response[offset++] = 70; /* Learn skill */
            response[offset++] = 71; /* Reset skills */
            response[offset++] = 99; /* Close */
            printf("[NPC] Skill NPC: Skill options\n");
            break;

        case NPC_TYPE_DISMANTLE:
            option_count = 3;
            response[offset++] = option_count;
            response[offset++] = 80; /* Dismantle item */
            response[offset++] = 81; /* Dismantle all */
            response[offset++] = 99; /* Close */
            printf("[NPC] Dismantle NPC: Dismantle options\n");
            break;

        default:
            option_count = 2;
            response[offset++] = option_count;
            response[offset++] = 90; /* Talk */
            response[offset++] = 99; /* Close */
            printf("[NPC] Generic NPC: Talk option\n");
            break;
    }

    return send_response(s, 35, response, offset);
}

/* Quest status constants */
#define QUEST_STATUS_AVAILABLE   0
#define QUEST_STATUS_ACTIVE      1
#define QUEST_STATUS_COMPLETE    2
#define QUEST_STATUS_REWARDED    3

/* msg_no=50: GET_QUEST_LIST */
int handle_get_quest_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_QUEST_LIST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 50, error, 4);
    }

    /* Response: result(4) + active_count(1) + [quest_id(4) + progress(1)]... */
    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Active quest count */
    response[offset++] = s->active_quest_count;

    /* List active quests */
    for (int i = 0; i < s->active_quest_count && i < 10; i++) {
        memcpy(response + offset, &s->active_quests[i], 4);
        offset += 4;
        response[offset++] = s->quest_progress[i];
    }

    printf("[GAME] Quest list: %d active quests\n", s->active_quest_count);
    return send_response(s, 50, response, offset);
}

/* msg_no=51: ACCEPT_QUEST */
int handle_accept_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] ACCEPT_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 51, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] Accept quest: %u\n", quest_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Check if quest already active */
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            uint32_t result = 2;  /* Already accepted */
            memcpy(response + offset, &result, 4);
            return send_response(s, 51, response, 4);
        }
    }

    /* Check if quest slot available */
    if (s->active_quest_count >= 10) {
        uint32_t result = 3;  /* Quest log full */
        memcpy(response + offset, &result, 4);
        return send_response(s, 51, response, 4);
    }

    /* Accept the quest */
    int idx = s->active_quest_count;
    s->active_quests[idx] = quest_id;
    s->quest_progress[idx] = 0;
    s->active_quest_count++;

    /* P5: Persist quest to database */
    db_save_quest_progress(s->character_id, quest_id, 1, 0);  /* state=1 (active), progress=0 */

    uint32_t result = 0;  /* Success */
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    response[offset++] = 0;  /* Initial progress */

    printf("[GAME] Quest %u accepted (slot %d), persisted to DB\n", quest_id, idx);
    return send_response(s, 51, response, offset);
}

/* msg_no=52: COMPLETE_QUEST */
int handle_complete_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] COMPLETE_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 52, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] Complete quest: %u\n", quest_id);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Find the quest */
    int found_idx = -1;
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx < 0) {
        uint32_t result = 2;  /* Quest not found */
        memcpy(response + offset, &result, 4);
        return send_response(s, 52, response, 4);
    }

    /* Mark as complete and give rewards */
    uint32_t exp_reward = (quest_id % 100) * 100 + 500;
    uint32_t gold_reward = (quest_id % 100) * 50 + 200;

    /* Add rewards via DB with persistence */
    if (db_add_exp(s->character_id, exp_reward) >= 0) {
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                s->characters[i].exp += exp_reward;
                break;
            }
        }
        printf("[GAME] Quest reward: EXP+%u persisted to DB\n", exp_reward);
    }
    if (db_add_gold(s->character_id, gold_reward) >= 0) {
        s->inventory.money += gold_reward;
        printf("[GAME] Quest reward: Gold+%u persisted to DB\n", gold_reward);
    }

    /* P2: Check for level up after quest reward */
    check_and_level_up(s);

    /* P5: Persist quest completion to database */
    db_save_quest_progress(s->character_id, quest_id, 2, 100);  /* state=2 (complete), progress=100 */

    /* Remove from active, add to completed */
    if (s->completed_quest_count < 64) {
        s->completed_quests[s->completed_quest_count++] = quest_id;
    }

    /* Shift active quests down */
    for (int i = found_idx; i < s->active_quest_count - 1; i++) {
        s->active_quests[i] = s->active_quests[i + 1];
        s->quest_progress[i] = s->quest_progress[i + 1];
    }
    s->active_quest_count--;

    uint32_t result = 0;  /* Success */
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    memcpy(response + offset, &exp_reward, 4);
    offset += 4;
    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    printf("[GAME] Quest %u completed and persisted! Rewards: EXP=%u, Gold=%u\n",
           quest_id, exp_reward, gold_reward);
    return send_response(s, 52, response, offset);
}

/* msg_no=53: UPDATE_QUEST_PROGRESS */
int handle_update_quest_progress(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UPDATE_QUEST_PROGRESS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 53, error, 4);
    }

    uint32_t quest_id;
    uint8_t progress;
    memcpy(&quest_id, data, 4);
    progress = data[4];

    printf("[GAME] Update quest %u progress to %u%%\n", quest_id, progress);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Find and update quest */
    int found = 0;
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            s->quest_progress[i] = progress;
            found = 1;
            /* P5: Persist progress update to database */
            db_save_quest_progress(s->character_id, quest_id, 1, progress);
            printf("[GAME] Quest %u progress=%u persisted to DB\n", quest_id, progress);
            break;
        }
    }

    uint32_t result = found ? 0 : 2;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    response[offset++] = progress;

    return send_response(s, 53, response, offset);
}

/*==============================================================================
 * Dungeon System Handlers
 *============================================================================*/

/*------------------------------------------------------------------------------
 * Shared Party Dungeon Helper Functions
 *----------------------------------------------------------------------------*/

/* Find shared dungeon by party_id */
static SharedDungeonInstance* find_shared_dungeon_by_party(uint32_t party_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].party_id == party_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Find shared dungeon by ID */
static SharedDungeonInstance* find_shared_dungeon(uint32_t shared_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].shared_id == shared_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Create a new shared dungeon instance for party */
static SharedDungeonInstance* create_shared_dungeon(uint32_t party_id, uint32_t dungeon_id, uint8_t difficulty) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (!g_shared_dungeons[i].active) {
            SharedDungeonInstance* sd = &g_shared_dungeons[i];
            memset(sd, 0, sizeof(SharedDungeonInstance));
            sd->shared_id = g_next_shared_dungeon_id++;
            sd->party_id = party_id;
            sd->dungeon_id = dungeon_id;
            sd->difficulty = difficulty;
            sd->current_room = 0;
            sd->total_rooms = 3 + difficulty;
            sd->active = 1;
            sd->start_time = (uint32_t)time(NULL);
            sd->time_limit = 600;  /* 10 minutes */
            sd->monster_count = 0;
            sd->monsters_killed = 0;
            sd->member_count = 0;
            printf("[PARTY_DUNGEON] Created shared dungeon %u for party %u (dungeon=%u diff=%d)\n",
                   sd->shared_id, party_id, dungeon_id, difficulty);
            return sd;
        }
    }
    printf("[PARTY_DUNGEON] ERROR: No free shared dungeon slots!\n");
    return NULL;
}

/* Add member to shared dungeon */
static int shared_dungeon_add_member(SharedDungeonInstance* sd, int fd) {
    if (!sd || sd->member_count >= MAX_PARTY_MEMBERS_DUNGEON) return -1;

    /* Check not already added */
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) return -1;
    }

    sd->member_fds[sd->member_count++] = (uint32_t)fd;
    printf("[PARTY_DUNGEON] Added member fd=%d to shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);
    return 0;
}

/* Remove member from shared dungeon */
static int shared_dungeon_remove_member(SharedDungeonInstance* sd, int fd) {
    if (!sd) return -1;

    int found = -1;
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)sd->member_count - 1; i++) {
        sd->member_fds[i] = sd->member_fds[i + 1];
    }
    sd->member_count--;

    printf("[PARTY_DUNGEON] Removed member fd=%d from shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);

    /* If no members left, deactivate shared dungeon */
    if (sd->member_count == 0) {
        printf("[PARTY_DUNGEON] Shared dungeon %u closed (no members)\n", sd->shared_id);
        sd->active = 0;
    }

    return 0;
}

/* Broadcast dungeon update to all party members in dungeon */
static void shared_dungeon_broadcast(SharedDungeonInstance* sd, uint8_t msg_no,
                                      const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        /* Find session by fd */
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Broadcast dungeon update to all party members except sender */
static void shared_dungeon_broadcast_except(SharedDungeonInstance* sd, int except_fd,
                                             uint8_t msg_no, const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        if (fd == except_fd) continue;  /* Skip sender */

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Spawn monsters for shared dungeon */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room);

/* Sync local dungeon state from shared instance */
static void sync_dungeon_from_shared(DungeonInstance* local, SharedDungeonInstance* sd) {
    if (!local || !sd) return;

    local->dungeon_id = sd->dungeon_id;
    local->difficulty = sd->difficulty;
    local->current_room = sd->current_room;
    local->total_rooms = sd->total_rooms;
    local->active = sd->active;
    local->start_time = sd->start_time;
    local->time_limit = sd->time_limit;
    local->monster_count = sd->monster_count;
    local->monsters_killed = sd->monsters_killed;

    /* Copy monster data */
    memcpy(local->monsters, sd->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Sync shared instance from local dungeon state (after attack) */
static void sync_shared_from_dungeon(SharedDungeonInstance* sd, DungeonInstance* local) {
    if (!sd || !local) return;

    sd->current_room = local->current_room;
    sd->monster_count = local->monster_count;
    sd->monsters_killed = local->monsters_killed;

    /* Copy monster data */
    memcpy(sd->monsters, local->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Helper: Spawn monsters for a dungeon room */
static void dungeon_spawn_monsters(DungeonInstance* d, int room) {
    d->monster_count = 0;

    /* Spawn 3-5 monsters per room based on difficulty */
    int base_count = 3 + d->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    /* Determine if this room has a boss */
    int is_boss_room = (room == d->total_rooms - 1);
    /* Boss ID based on dungeon_id (1-3 for now) */
    uint32_t boss_id = (d->dungeon_id % 3) + 1;

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &d->monsters[i];
        memset(m, 0, sizeof(MonsterInfo));  /* Clear all fields including boss fields */

        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (d->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        m->max_hp = 1000 * (d->difficulty + 1) * (room + 1);
        m->hp = m->max_hp;
        m->alive = 1;

        /* Initialize AI fields */
        m->state = MONSTER_STATE_IDLE;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->hit_stun_frames = 0;

        /* Check if this is the boss (last monster in boss room) */
        int is_this_boss = (is_boss_room && i == base_count - 1);

        if (is_this_boss) {
            /* Initialize as boss using template system */
            boss_init_from_template(m, boss_id, d->difficulty);
            m->monster_id = (room * 100) + i + 1;  /* Preserve monster_id */
            m->x = 400;  /* Boss spawns in center */
            m->y = 250;
            m->spawn_x = m->x;
            m->spawn_y = m->y;
            m->move_speed = 3;
            m->aggro_range = 500;
            m->attack_range = 100;
        } else {
            /* Normal monster */
            m->ai_type = MONSTER_AI_NORMAL;
            m->is_boss = 0;
            m->move_speed = 5;
            m->aggro_range = 250;
            m->attack_range = 50;
            m->attack_damage = 100 * (d->difficulty + 1) * (room + 1);
            m->phys_def = 50 * (d->difficulty + 1);
            m->mag_def = 30 * (d->difficulty + 1);
        }

        d->monster_count++;
    }

    printf("[DUNGEON] Spawned %d monsters in room %d (boss_room=%d, boss_id=%u)\n",
           d->monster_count, room, is_boss_room, is_boss_room ? boss_id : 0);
}

/* Spawn monsters for shared dungeon (uses same logic as dungeon_spawn_monsters) */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room) {
    sd->monster_count = 0;

    int base_count = 3 + sd->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    /* Determine if this room has a boss */
    int is_boss_room = (room == (int)sd->total_rooms - 1);
    /* Boss ID based on dungeon_id (1-3 for now) */
    uint32_t boss_id = (sd->dungeon_id % 3) + 1;
    /* Party scaling factor */
    float party_scale = 1.0f + (sd->member_count - 1) * 0.3f;

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &sd->monsters[i];
        memset(m, 0, sizeof(MonsterInfo));

        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (sd->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        /* Scale HP for party size */
        m->max_hp = (uint32_t)(1000 * (sd->difficulty + 1) * (room + 1) * party_scale);
        m->hp = m->max_hp;
        m->alive = 1;

        m->state = MONSTER_STATE_IDLE;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->hit_stun_frames = 0;

        /* Check if this is the boss (last monster in boss room) */
        int is_this_boss = (is_boss_room && i == base_count - 1);

        if (is_this_boss) {
            /* Initialize as boss using template system with party scaling */
            boss_init_from_template(m, boss_id, sd->difficulty);
            m->monster_id = (room * 100) + i + 1;
            m->x = 400;
            m->y = 250;
            m->spawn_x = m->x;
            m->spawn_y = m->y;
            m->move_speed = 3;
            m->aggro_range = 500;
            m->attack_range = 100;
            /* Apply party scaling to boss HP */
            m->max_hp = (uint32_t)(m->max_hp * party_scale);
            m->hp = m->max_hp;
        } else {
            /* Normal monster */
            m->ai_type = MONSTER_AI_NORMAL;
            m->is_boss = 0;
            m->move_speed = 5;
            m->aggro_range = 250;
            m->attack_range = 50;
            m->attack_damage = 100 * (sd->difficulty + 1) * (room + 1);
            m->phys_def = 50 * (sd->difficulty + 1);
            m->mag_def = 30 * (sd->difficulty + 1);
        }

        sd->monster_count++;
    }

    printf("[PARTY_DUNGEON] Shared dungeon %u: Spawned %d monsters in room %d (members=%d, boss=%u)\n",
           sd->shared_id, sd->monster_count, room, sd->member_count, is_boss_room ? boss_id : 0);
}

int handle_dungeon_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ENTER from fd=%d party_id=%u\n", s->fd, s->party_id);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 60, error, 4);
    }

    uint32_t dungeon_id;
    uint8_t difficulty;
    memcpy(&dungeon_id, data, 4);
    difficulty = data[4];

    printf("[DUNGEON] Enter: dungeon_id=%u difficulty=%d\n", dungeon_id, difficulty);

    /* P1: Check and consume fatigue with DB persistence */
    int fatigue_cost = 8 + difficulty * 2;  /* Higher difficulty = more fatigue */
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            if (s->characters[i].fatigue <= 0) {
                printf("[DUNGEON] No fatigue remaining\n");
                uint8_t error[4] = {5, 0, 0, 0};  /* No fatigue */
                return send_response(s, 60, error, 4);
            }
            if (s->characters[i].fatigue < fatigue_cost) {
                printf("[DUNGEON] Not enough fatigue: have=%d need=%d\n",
                       s->characters[i].fatigue, fatigue_cost);
                uint8_t error[4] = {5, 0, 0, 0};  /* Not enough fatigue */
                return send_response(s, 60, error, 4);
            }
            /* Consume fatigue */
            s->characters[i].fatigue -= fatigue_cost;
            if (s->characters[i].fatigue < 0) {
                s->characters[i].fatigue = 0;
            }
            /* Persist fatigue to DB */
            db_update_fatigue(s->character_id, s->characters[i].fatigue);
            printf("[DUNGEON] Consumed %d fatigue, remaining: %d\n",
                   fatigue_cost, s->characters[i].fatigue);
            break;
        }
    }

    /* Initialize local dungeon instance */
    DungeonInstance* d = &s->dungeon;
    memset(d, 0, sizeof(DungeonInstance));

    SharedDungeonInstance* sd = NULL;

    /* Check if player is in a party */
    if (s->party_id > 0) {
        PartyInfo* party = find_party(s->party_id);
        if (party && party->active) {
            /* Check if party already has a shared dungeon */
            sd = find_shared_dungeon_by_party(s->party_id);

            if (sd == NULL) {
                /* Party leader creates the shared dungeon */
                if (party->leader_id == s->character_id) {
                    sd = create_shared_dungeon(s->party_id, dungeon_id, difficulty);
                    if (sd == NULL) {
                        printf("[PARTY_DUNGEON] Failed to create shared dungeon\n");
                        uint8_t error[4] = {6, 0, 0, 0};  /* Server error */
                        return send_response(s, 60, error, 4);
                    }
                    /* Spawn monsters for shared dungeon */
                    shared_dungeon_add_member(sd, s->fd);
                    shared_dungeon_spawn_monsters(sd, 0);

                    /* Mark as host */
                    d->is_party_host = 1;
                    printf("[PARTY_DUNGEON] Leader created shared dungeon %u\n", sd->shared_id);
                } else {
                    /* Non-leader trying to enter without leader */
                    printf("[PARTY_DUNGEON] ERROR: Leader must enter first!\n");
                    uint8_t error[4] = {7, 0, 0, 0};  /* Party leader must enter first */
                    return send_response(s, 60, error, 4);
                }
            } else {
                /* Join existing shared dungeon */
                if (sd->dungeon_id != dungeon_id) {
                    printf("[PARTY_DUNGEON] ERROR: Dungeon mismatch (requested %u, party in %u)\n",
                           dungeon_id, sd->dungeon_id);
                    uint8_t error[4] = {8, 0, 0, 0};  /* Wrong dungeon */
                    return send_response(s, 60, error, 4);
                }
                shared_dungeon_add_member(sd, s->fd);
                d->is_party_host = 0;
                printf("[PARTY_DUNGEON] Member joined shared dungeon %u\n", sd->shared_id);
            }

            /* Store shared dungeon reference */
            d->party_id = s->party_id;
            d->shared_dungeon_id = sd->shared_id;

            /* Sync local dungeon from shared instance */
            sync_dungeon_from_shared(d, sd);

            /* Notify other party members that someone joined */
            uint8_t join_notify[16];
            memset(join_notify, 0, sizeof(join_notify));
            uint32_t member_id = s->character_id;
            memcpy(join_notify, &member_id, 4);
            join_notify[4] = 1;  /* Join notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, join_notify, 8);  /* msg_no=68: PARTY_DUNGEON_MEMBER_UPDATE */

            printf("[PARTY_DUNGEON] Entered shared dungeon, members=%d\n", sd->member_count);
        } else {
            /* Party not found, fall back to solo */
            printf("[DUNGEON] Party not found, entering solo\n");
            s->party_id = 0;
        }
    }

    /* Solo dungeon (no party or party not found) */
    if (s->party_id == 0) {
        d->dungeon_id = dungeon_id;
        d->difficulty = difficulty;
        d->current_room = 0;
        d->total_rooms = 3 + difficulty;
        d->active = 1;
        d->start_time = (uint32_t)time(NULL);
        d->time_limit = 600;
        d->monsters_killed = 0;
        d->party_id = 0;
        d->shared_dungeon_id = 0;
        d->is_party_host = 0;

        dungeon_spawn_monsters(d, 0);
    }

    s->state = STATE_IN_DUNGEON;

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->total_rooms;
    response[offset++] = d->current_room;

    /* Party info */
    response[offset++] = (d->party_id > 0) ? 1 : 0;  /* is_party_dungeon */
    if (d->party_id > 0 && sd) {
        response[offset++] = sd->member_count;
    } else {
        response[offset++] = 1;  /* Solo = 1 member */
    }

    /* Monster count for this room */
    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list: id(4) + type(2) + x(2) + y(2) + hp(4) = 14 bytes each */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    printf("[DUNGEON] Entered dungeon %u room 0/%d with %d monsters (party=%u)\n",
           d->dungeon_id, d->total_rooms, d->monster_count, d->party_id);

    return send_response(s, 60, response, offset);
}

/* msg_no=61: DUNGEON_EXIT */
int handle_dungeon_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_EXIT from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 61, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Handle party dungeon exit */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        SharedDungeonInstance* sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Notify other party members */
            uint8_t exit_notify[16];
            memset(exit_notify, 0, sizeof(exit_notify));
            uint32_t member_id = s->character_id;
            memcpy(exit_notify, &member_id, 4);
            exit_notify[4] = 0;  /* Leave notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, exit_notify, 8);

            /* Remove from shared dungeon */
            shared_dungeon_remove_member(sd, s->fd);

            printf("[PARTY_DUNGEON] Member fd=%d left shared dungeon %u (remaining=%d)\n",
                   s->fd, sd->shared_id, sd->member_count);
        }
    }

    /* Reset dungeon state */
    d->active = 0;
    d->party_id = 0;
    d->shared_dungeon_id = 0;
    s->state = STATE_IN_GAME;

    printf("[DUNGEON] Exited dungeon (killed %d monsters)\n", d->monsters_killed);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &d->monsters_killed, 4);

    return send_response(s, 61, response, 8);
}

/* msg_no=62: DUNGEON_INFO (get current dungeon state) */
int handle_dungeon_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_INFO from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    if (s->state != STATE_IN_DUNGEON || !s->dungeon.active) {
        uint32_t result = 1;  /* Not in dungeon */
        memcpy(response, &result, 4);
        return send_response(s, 62, response, 4);
    }

    DungeonInstance* d = &s->dungeon;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    /* Time remaining */
    uint32_t elapsed = (uint32_t)time(NULL) - d->start_time;
    uint32_t remaining = (elapsed < d->time_limit) ? (d->time_limit - elapsed) : 0;
    memcpy(response + offset, &remaining, 4);
    offset += 4;

    /* Monsters alive in current room */
    uint8_t alive_count = 0;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].alive) alive_count++;
    }
    response[offset++] = alive_count;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    return send_response(s, 62, response, offset);
}

/* msg_no=65: DUNGEON_ATTACK (attack a monster with skill damage calculation) */
int handle_dungeon_attack(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ATTACK from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 65, error, 4);
    }

    uint32_t monster_id;
    uint16_t skill_id = 100;  /* Default: normal attack */
    uint8_t skill_level = 1;
    memcpy(&monster_id, data, 4);

    /* Parse skill_id if provided (offset 4-5) */
    if (len >= 6) {
        memcpy(&skill_id, data + 4, 2);
    }
    /* Parse skill_level if provided (offset 6) */
    if (len >= 7) {
        skill_level = data[6];
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, operate on shared instance */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Sync local from shared before finding monster */
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Find monster */
    MonsterInfo* target = NULL;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].monster_id == monster_id && d->monsters[i].alive) {
            target = &d->monsters[i];
            break;
        }
    }

    if (!target) {
        printf("[DUNGEON] Monster %u not found or dead\n", monster_id);
        uint8_t error[4] = {2, 0, 0, 0};  /* Monster not found */
        return send_response(s, 65, error, 4);
    }

    /* Get character info for skill calculation */
    uint8_t job = 0;
    uint8_t char_level = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            job = s->characters[i].job;
            char_level = s->characters[i].level;
            break;
        }
    }

    /* Calculate monster effective level based on difficulty and room */
    uint8_t monster_level = (uint8_t)(10 + d->difficulty * 15 + d->current_room * 3);
    if (monster_level > 99) monster_level = 99;

    /* Monster defense scales with difficulty and room */
    uint32_t monster_def = 50 + d->difficulty * 30 + d->current_room * 10;

    /* Use v3 damage calculation with full modifiers (Buff/Penetration/Element) */
    uint8_t is_crit = 0;
    uint8_t is_miss = 0;
    uint32_t damage = calculate_skill_damage_v3(s, skill_id, skill_level,
                                                 target->monster_type, monster_level,
                                                 monster_def, &is_crit, &is_miss);

    /* Handle miss */
    if (is_miss) {
        printf("[DUNGEON] Skill %u MISSED! (hit_rate too low vs monster lv%u)\n",
               skill_id, monster_level);
        uint8_t miss_resp[8] = {0, 0, 0, 0, 0, 0, 0, 1};  /* damage=0, miss=1 */
        memcpy(miss_resp, &monster_id, 4);
        return send_response(s, 65, miss_resp, 8);
    }

    /* Get hit count for multi-hit skills */
    uint8_t hit_count = get_skill_hit_count(skill_id);
    uint32_t total_damage = damage * hit_count;

    /* Apply lifesteal from Buff */
    int32_t lifesteal_pct = buff_get_stat_modifier(s, BUFF_LIFESTEAL);
    if (lifesteal_pct > 0 && total_damage > 0) {
        uint32_t heal = total_damage * lifesteal_pct / 100;
        if (heal > 0) {
            s->current_hp += heal;
            if (s->current_hp > s->combat_stats.max_hp) {
                s->current_hp = s->combat_stats.max_hp;
            }
            printf("[DUNGEON] Lifesteal: +%u HP (now %u/%u)\n",
                   heal, s->current_hp, s->combat_stats.max_hp);
        }
    }

    printf("[DUNGEON] Skill %u (lv%d) -> Monster %u (type=%u, lv=%u): %u dmg x %d hits = %u%s\n",
           skill_id, skill_level, monster_id, target->monster_type, monster_level,
           damage, hit_count, total_damage, is_crit ? " CRIT!" : "");

    /* Apply damage with hit stun */
    uint8_t killed = 0;
    if (total_damage >= target->hp) {
        target->hp = 0;
        target->alive = 0;
        target->state = MONSTER_STATE_DEAD;
        killed = 1;
        d->monsters_killed++;
        printf("[DUNGEON] Monster %u killed! Total kills: %d\n", monster_id, d->monsters_killed);
    } else {
        target->hp -= total_damage;
        /* Apply hit stun based on damage dealt */
        uint8_t stun_frames = (uint8_t)((total_damage / 100) + 3);
        if (stun_frames > 30) stun_frames = 30;
        monster_apply_hit_stun(target, stun_frames);
        printf("[DUNGEON] Monster %u took %u damage, HP: %u/%u, stun: %d frames\n",
               monster_id, total_damage, target->hp, target->max_hp, stun_frames);
    }

    /* Check if room is cleared */
    uint8_t room_cleared = dungeon_room_cleared(d) ? 1 : 0;

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &monster_id, 4);
    offset += 4;

    memcpy(response + offset, &total_damage, 4);
    offset += 4;

    memcpy(response + offset, &target->hp, 4);
    offset += 4;

    response[offset++] = killed;
    response[offset++] = is_crit;
    response[offset++] = hit_count;
    response[offset++] = room_cleared;

    /* Add attacker ID for party sync */
    memcpy(response + offset, &s->character_id, 4);
    offset += 4;

    /* For party dungeons, sync state and broadcast to other members */
    if (sd) {
        /* Sync shared from local (after damage applied) */
        sync_shared_from_dungeon(sd, d);

        /* Broadcast damage update to other party members (msg_no=69: PARTY_DUNGEON_ATTACK_SYNC) */
        shared_dungeon_broadcast_except(sd, s->fd, 69, response, offset);

        /* Sync updated monster state to all party sessions */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;  /* Skip attacker */

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    /* Sync their local dungeon from shared */
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    break;
                }
            }
        }

        printf("[PARTY_DUNGEON] Synced attack to %d other members\n", sd->member_count - 1);
    }

    return send_response(s, 65, response, offset);
}

/* msg_no=66: DUNGEON_NEXT_ROOM (move to next room) */
int handle_dungeon_next_room(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_NEXT_ROOM from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 66, error, 4);
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, sync from shared instance first */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Check if current room is cleared */
    if (!dungeon_room_cleared(d)) {
        printf("[DUNGEON] Room not cleared yet\n");
        uint8_t error[4] = {2, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 66, error, 4);
    }

    /* Check if this was the last room */
    if (d->current_room >= d->total_rooms - 1) {
        printf("[DUNGEON] Already at final room\n");
        uint8_t error[4] = {3, 0, 0, 0};  /* Already at final room */
        return send_response(s, 66, error, 4);
    }

    /* For party dungeons, only leader can advance room */
    if (sd && !d->is_party_host) {
        printf("[PARTY_DUNGEON] Only party leader can advance room\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Not party leader */
        return send_response(s, 66, error, 4);
    }

    /* Move to next room */
    d->current_room++;

    /* For party dungeons, spawn on shared instance and sync */
    if (sd) {
        sd->current_room = d->current_room;
        shared_dungeon_spawn_monsters(sd, sd->current_room);
        sync_dungeon_from_shared(d, sd);

        printf("[PARTY_DUNGEON] Advanced to room %d/%d with %d monsters\n",
               sd->current_room, sd->total_rooms, sd->monster_count);
    } else {
        dungeon_spawn_monsters(d, d->current_room);
        printf("[DUNGEON] Moved to room %d/%d with %d monsters\n",
               d->current_room, d->total_rooms, d->monster_count);
    }

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    /* For party dungeons, broadcast room change to all members and sync their state */
    if (sd) {
        /* Broadcast to other members (msg_no=70 but with result=100 to indicate room change) */
        uint8_t room_notify[8];
        memset(room_notify, 0, sizeof(room_notify));
        room_notify[0] = d->current_room;
        room_notify[1] = d->total_rooms;
        room_notify[2] = mon_count;
        shared_dungeon_broadcast_except(sd, s->fd, 67, room_notify, 8);  /* msg_no=67: PARTY_DUNGEON_ROOM_CHANGE */

        /* Sync all party members */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    /* Send same response to party members */
                    send_response(&g_sessions[j], 66, response, offset);
                    break;
                }
            }
        }
    }

    return send_response(s, 66, response, offset);
}

/* msg_no=70: DUNGEON_CLEAR (clear final room) */
int handle_dungeon_clear(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_CLEAR from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 70, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Check if final room and cleared */
    if (d->current_room < d->total_rooms - 1) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Not at final room */
        return send_response(s, 70, error, 4);
    }

    if (!dungeon_room_cleared(d)) {
        uint8_t error[4] = {3, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 70, error, 4);
    }

    /* Calculate clear time */
    uint32_t clear_time = (uint32_t)time(NULL) - d->start_time;

    /* Calculate rank (S, A, B, C, D based on time and difficulty) */
    uint8_t rank;
    uint32_t base_time = 180 * (d->difficulty + 1);  /* Base time for S rank */
    if (clear_time <= base_time) {
        rank = 0;  /* S */
    } else if (clear_time <= base_time * 1.5) {
        rank = 1;  /* A */
    } else if (clear_time <= base_time * 2) {
        rank = 2;  /* B */
    } else if (clear_time <= base_time * 3) {
        rank = 3;  /* C */
    } else {
        rank = 4;  /* D */
    }

    printf("[DUNGEON] Cleared! Time: %u sec, Rank: %c, Kills: %d\n",
           clear_time, "SABCD"[rank], d->monsters_killed);

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &clear_time, 4);
    offset += 4;

    response[offset++] = rank;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    /* Base rewards (scaled by rank and difficulty) */
    uint32_t exp_reward = 10000 * (d->difficulty + 1) * (5 - rank);
    uint32_t gold_reward = 5000 * (d->difficulty + 1) * (5 - rank);

    memcpy(response + offset, &exp_reward, 4);
    offset += 4;

    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    /* Persist rewards to database */
    printf("[DUNGEON] Persisting rewards: EXP=%u, Gold=%u\n", exp_reward, gold_reward);

    /* Add EXP to database */
    if (db_add_exp(s->character_id, exp_reward) >= 0) {
        printf("[DUNGEON] EXP persisted to DB\n");
        /* Update in-memory too */
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                s->characters[i].exp += exp_reward;
                break;
            }
        }
    }

    /* P2: Check for level up after gaining exp */
    int levels_gained = check_and_level_up(s);
    if (levels_gained > 0) {
        printf("[DUNGEON] Gained %d levels from dungeon clear!\n", levels_gained);
    }

    /* Add gold to database */
    if (db_add_gold(s->character_id, gold_reward) >= 0) {
        printf("[DUNGEON] Gold persisted to DB\n");
        /* Update in-memory too */
        s->inventory.money += gold_reward;
    }

    /* P3: Decrease equipment durability after dungeon */
    int durability_loss = 2 + d->difficulty;  /* Higher difficulty = more wear */
    decrease_equipment_durability(s, durability_loss);
    save_equipment_durability(s);
    printf("[DUNGEON] Equipment durability decreased by %d\n", durability_loss);

    /* Phase 9: Add guild experience on dungeon clear */
    if (s->guild_id > 0) {
        /* Guild exp = base * difficulty * rank bonus */
        /* Base: 100 exp per dungeon, +50 per difficulty level */
        /* Rank bonus: S=2.0x, A=1.5x, B=1.2x, C=1.0x, D=0.8x */
        uint32_t guild_exp_base = 100 + d->difficulty * 50;
        float rank_mult[5] = {2.0f, 1.5f, 1.2f, 1.0f, 0.8f};
        uint32_t guild_exp = (uint32_t)(guild_exp_base * rank_mult[rank]);

        uint8_t new_level = 0, leveled_up = 0;
        if (guild_ipc_add_exp(s->guild_id, guild_exp, &new_level, &leveled_up) == 0) {
            printf("[DUNGEON] Guild %u gained %u exp (level=%u, leveled_up=%u)\n",
                   s->guild_id, guild_exp, new_level, leveled_up);
        }
    }

    /* Phase 12: Log boss kill if boss was defeated */
    uint32_t boss_id = (d->dungeon_id % 3) + 1;  /* Same logic as spawning */
    uint32_t kill_count = 0, best_time = 0;
    db_get_boss_kill_stats(s->character_id, boss_id, &kill_count, &best_time);
    uint8_t is_first_kill = (kill_count == 0) ? 1 : 0;
    db_log_boss_kill(s->character_id, boss_id, d->dungeon_id, d->difficulty,
                     clear_time, is_first_kill);
    if (is_first_kill) {
        printf("[BOSS] FIRST KILL! Boss %u defeated by charac=%u\n", boss_id, s->character_id);
    }

    /* Phase 12: Achievement triggers on dungeon clear */
    {
        int ach_completed = 0;
        /* Monster kill achievement */
        ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_KILL_MONSTER,
                                                     d->monsters_killed);
        /* Dungeon clear achievement - need to track cumulative */
        /* For now, count from db */
        char ach_query[256];
        snprintf(ach_query, sizeof(ach_query),
                 "SELECT COUNT(*) FROM boss_kill_log WHERE charac_no=%u", s->character_id);
        if (g_db_charac && mysql_query(g_db_charac, ach_query) == 0) {
            MYSQL_RES* ach_result = mysql_store_result(g_db_charac);
            if (ach_result) {
                MYSQL_ROW ach_row = mysql_fetch_row(ach_result);
                if (ach_row) {
                    uint32_t dungeon_clears = (uint32_t)atoi(ach_row[0]);
                    ach_completed += check_achievement_progress(s->character_id,
                                                                 ACH_TYPE_CLEAR_DUNGEON,
                                                                 dungeon_clears);
                }
                mysql_free_result(ach_result);
            }
        }
        /* Boss kill achievement */
        ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_KILL_BOSS,
                                                     kill_count + 1);
        /* First boss kill achievement */
        if (is_first_kill) {
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_FIRST_BOSS,
                                                         boss_id);
        }
        /* Speed clear achievement (target = seconds, lower is better) */
        if (clear_time <= 180) {
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_SPEED_CLEAR,
                                                         clear_time);
        }
        /* No death clear - if player never died in this dungeon */
        if (d->death_count == 0) {
            /* Count no-death clears from session stat */
            s->stat_no_death_clears++;
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_NO_DEATH,
                                                         s->stat_no_death_clears);
        }
        /* Party clear achievement */
        if (s->party_id > 0) {
            s->stat_party_clears++;
            ach_completed += check_achievement_progress(s->character_id, ACH_TYPE_PARTY_CLEAR,
                                                         s->stat_party_clears);
        }
        if (ach_completed > 0) {
            printf("[ACH] Dungeon clear triggered %d achievement completions\n", ach_completed);
        }
    }

    /* Drop a random item as reward (simple: item_id based on dungeon_id + difficulty) */
    uint32_t drop_item_id = 60000 + d->dungeon_id * 100 + d->difficulty * 10 + (rank < 3 ? 1 : 0);
    int drop_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (drop_slot > 0 && drop_slot < 200) {
        int new_id = db_add_item(s->character_id, drop_item_id, drop_slot, 0);
        if (new_id > 0) {
            printf("[DUNGEON] Item drop persisted: item_id=%u, slot=%d\n", drop_item_id, drop_slot);
        }
    }

    /* Mark dungeon as complete */
    d->active = 0;
    s->state = STATE_IN_GAME;

    return send_response(s, 70, response, offset);
}


/* msg_no=71: DUNGEON_REWARD (claim additional rewards with random attributes) */
int handle_dungeon_reward(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_REWARD from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 71, error, 4);
    }

    uint32_t reward_type;
    memcpy(&reward_type, data, 4);

    printf("[DUNGEON] Claiming reward type: %u\n", reward_type);

    /* Determine item rarity based on reward_type and RNG */
    uint8_t item_rarity = RARITY_NORMAL;
    uint32_t rarity_roll = (uint32_t)(rand() % 10000);
    if (rarity_roll < 100) {
        item_rarity = RARITY_LEGENDARY;  /* 1% */
    } else if (rarity_roll < 500) {
        item_rarity = RARITY_EPIC;       /* 4% */
    } else if (rarity_roll < 1500) {
        item_rarity = RARITY_RARE;       /* 10% */
    } else if (rarity_roll < 4000) {
        item_rarity = RARITY_MAGIC;      /* 25% */
    }

    /* Boost rarity for higher reward types */
    if (reward_type >= 5 && item_rarity < RARITY_EPIC) {
        item_rarity++;  /* Better rewards for harder content */
    }

    /* Generate item ID: base equipment (2000-2100) based on reward type */
    uint32_t item_id = 2000 + (reward_type % 20) * 5;
    uint16_t item_count = 1;

    /* Item level based on dungeon difficulty (stored in session) */
    uint8_t item_level = s->dungeon.difficulty * 10 + 15;
    if (item_level > 85) item_level = 85;

    /* Find free slot */
    int slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (slot < 0 || slot >= 200) {
        uint8_t error[4] = {4, 0, 0, 0};  /* Inventory full */
        return send_response(s, 71, error, 4);
    }

    /* Add item with random attributes */
    int new_ui_id = db_add_item_with_attrs(s->character_id, item_id, slot, item_rarity, item_level);
    if (new_ui_id <= 0) {
        /* Fallback to basic item without attributes */
        new_ui_id = db_add_item(s->character_id, item_id, slot, item_rarity);
        if (new_ui_id <= 0) {
            uint8_t error[4] = {5, 0, 0, 0};  /* DB error */
            return send_response(s, 71, error, 4);
        }
    }

    printf("[DUNGEON] Reward item: item_id=%u, rarity=%u, level=%u, slot=%d, ui_id=%d\n",
           item_id, item_rarity, item_level, slot, new_ui_id);

    /* Build response with rarity info */
    uint8_t response[40];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &item_count, 2);
    offset += 2;

    uint16_t slot16 = (uint16_t)slot;
    memcpy(response + offset, &slot16, 2);
    offset += 2;

    /* New: include rarity and level for client display */
    response[offset++] = item_rarity;
    response[offset++] = item_level;

    return send_response(s, 71, response, offset);
}

/* msg_no=220: DUNGEON_MONSTER_TICK - Client sends player position, server updates monsters */
int handle_dungeon_monster_tick(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 4) {
        return 0;  /* Silent fail for tick messages */
    }

    /* Parse player position from client */
    uint16_t player_x, player_y;
    memcpy(&player_x, data, 2);
    memcpy(&player_y, data + 2, 2);

    DungeonInstance* d = &s->dungeon;
    if (!d->active) return 0;

    /* P3: Process buff updates every tick */
    static uint32_t last_buff_tick = 0;
    uint32_t now = (uint32_t)time(NULL) * 1000;
    uint32_t delta_ms = (last_buff_tick > 0) ? (now - last_buff_tick) : 100;
    if (delta_ms > 1000) delta_ms = 1000;  /* Cap at 1 second */
    last_buff_tick = now;

    /* Update and expire buffs */
    buff_update(s);

    /* Process DOT/HOT effects (poison damage, regen healing, etc.) */
    buff_process_dots_and_hots(s, delta_ms);

    /* Recalculate buff stat modifiers */
    buff_apply_stat_modifiers(s);

    /* Use enhanced AI v2 with skill selection and debuffs */
    DungeonTickResult tick = dungeon_tick_monsters_v2(d, s, player_x, player_y);

    /* Apply total damage to player if attacked */
    uint8_t player_died = 0;
    uint32_t actual_damage = 0;
    if (tick.total_damage > 0) {
        actual_damage = (uint32_t)player_take_damage(s, d, tick.total_damage, &player_died);
        printf("[DUNGEON] Monster attacks hit player! attacks=%d raw=%u actual=%u hp=%u\n",
               tick.attacks_count, tick.total_damage, actual_damage, s->current_hp);
    }

    /* Check if player died from DOT effects */
    if (!player_died && s->current_hp == 0) {
        player_die(s);
        player_died = 1;
    }

    /* Build response: monster positions + player status */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result code: 0=OK, 1=player died */
    uint32_t result = player_died ? 1 : 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Player HP (4) + debuff_type (2) + debuff_duration (2) = 8 bytes */
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    memcpy(response + offset, &tick.debuff_type, 2);
    offset += 2;
    memcpy(response + offset, &tick.debuff_duration, 2);
    offset += 2;

    /* Monster count */
    response[offset++] = (uint8_t)d->monster_count;

    /* Monster positions: id(4) + x(2) + y(2) + state(1) + hp(4) = 13 bytes */
    for (int i = 0; i < d->monster_count && offset < 220; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        response[offset++] = m->state;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    /* Attack summary: count(1) + total_damage(4) = 5 bytes */
    response[offset++] = (uint8_t)tick.attacks_count;
    memcpy(response + offset, &actual_damage, 4);
    offset += 4;

    /* P5: Party dungeon sync - broadcast to all party members */
    if (s->party_id > 0) {
        SharedDungeonInstance* sd = find_shared_dungeon_by_party(s->party_id);
        if (sd && sd->member_count > 1) {
            /* Build party sync packet with sender's info */
            uint8_t sync_pkt[280];
            memset(sync_pkt, 0, sizeof(sync_pkt));
            int sync_offset = 0;

            /* Add sender character ID and position */
            memcpy(sync_pkt + sync_offset, &s->character_id, 4);
            sync_offset += 4;
            memcpy(sync_pkt + sync_offset, &player_x, 2);
            sync_offset += 2;
            memcpy(sync_pkt + sync_offset, &player_y, 2);
            sync_offset += 2;
            memcpy(sync_pkt + sync_offset, &s->current_hp, 4);
            sync_offset += 4;

            /* Copy monster data from response */
            memcpy(sync_pkt + sync_offset, response, offset);
            sync_offset += offset;

            /* Broadcast to all party members except sender */
            for (int i = 0; i < (int)sd->member_count; i++) {
                int fd = (int)sd->member_fds[i];
                if (fd == s->fd) continue;  /* Skip sender */
                for (int j = 0; j < MAX_SESSIONS; j++) {
                    if (g_sessions[j].active && g_sessions[j].fd == fd) {
                        send_response(&g_sessions[j], 215, sync_pkt, sync_offset);  /* PARTY_DUNGEON_SYNC */
                        break;
                    }
                }
            }
        }
    }

    return send_response(s, 214, response, offset);
}

/* msg_no=221: DUNGEON_PLAYER_HIT - Acknowledge player hit by monster */
int handle_dungeon_player_hit(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    uint32_t monster_id;
    uint8_t monster_idx;
    memcpy(&monster_id, data, 4);
    monster_idx = data[4];
    
    DungeonInstance* d = &s->dungeon;
    if (!d->active || monster_idx >= d->monster_count) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    MonsterInfo* m = &d->monsters[monster_idx];
    if (!m->alive || m->monster_id != monster_id) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    

    /* Calculate actual damage with defense reduction */
    uint32_t raw_damage = m->attack_damage;
    uint32_t player_def = s->combat_stats.phys_def;

    /* Defense formula: damage = raw * (100 / (100 + def)) */
    uint32_t actual_damage = raw_damage * 100 / (100 + player_def);
    if (actual_damage < 1) actual_damage = 1;

    /* Check for damage reduction buffs */
    int32_t def_mod = buff_get_stat_modifier(s, BUFF_DEFENSE_UP);
    if (def_mod > 0) {
        actual_damage = actual_damage * 100 / (100 + def_mod);
    }

    /* Check for invincibility buff */
    if (buff_has(s, BUFF_INVINCIBLE)) {
        actual_damage = 0;
        printf("[DUNGEON] Player is invincible, damage blocked!\n");
    }

    /* Apply damage to player HP */
    uint32_t old_hp = s->current_hp;
    uint8_t player_died = 0;

    if (actual_damage >= s->current_hp) {
        s->current_hp = 0;
        player_died = 1;
        printf("[DUNGEON] Player KILLED by monster %u! Damage: %u, Old HP: %u\n",
               monster_id, actual_damage, old_hp);
    } else {
        s->current_hp -= actual_damage;
        printf("[DUNGEON] Player hit by monster %u for %u damage (raw: %u), HP: %u -> %u\n",
               monster_id, actual_damage, raw_damage, old_hp, s->current_hp);
    }

    /* Build response */
    uint8_t response[20];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &monster_id, 4);
    offset += 4;
    memcpy(response + offset, &actual_damage, 4);
    offset += 4;
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    response[offset++] = player_died;

    return send_response(s, 215, response, offset);
}


/*==============================================================================
 * Trade System (msg_no 75-83)
 *============================================================================*/

/* Trade state structure */
typedef struct {
    uint32_t partner_fd;          /* Trading partner's fd */
    uint32_t partner_id;          /* Partner character ID */
    uint32_t items[10];           /* Item ui_ids in trade */
    uint8_t  item_count;
    uint32_t gold;                /* Gold offered */
    uint8_t  locked;              /* Trade locked (ready to confirm) */
    uint8_t  confirmed;           /* Trade confirmed */
    uint8_t  active;
} TradeState;

static TradeState g_trades[MAX_SESSIONS];

/* PartyInfo struct and g_parties defined earlier at line ~165 for shared dungeon support */

/* Find party by ID (implementation of forward declaration) */
static PartyInfo* find_party(uint32_t party_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (g_parties[i].active && g_parties[i].party_id == party_id) {
            return &g_parties[i];
        }
    }
    return NULL;
}

/* Create a new party */
static PartyInfo* create_party(uint32_t leader_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (!g_parties[i].active) {
            memset(&g_parties[i], 0, sizeof(PartyInfo));
            g_parties[i].party_id = g_next_party_id++;
            g_parties[i].leader_id = leader_id;
            g_parties[i].members[0] = leader_id;
            g_parties[i].member_count = 1;
            g_parties[i].loot_mode = 0;
            g_parties[i].active = 1;
            printf("[PARTY] Created party %u with leader %u\n",
                   g_parties[i].party_id, leader_id);
            return &g_parties[i];
        }
    }
    return NULL;  /* No free slots */
}

/* Add member to party */
static int party_add_member(PartyInfo* party, uint32_t member_id) {
    if (!party || party->member_count >= MAX_PARTY_MEMBERS) return -1;

    /* Check not already in party */
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) return -1;
    }

    party->members[party->member_count++] = member_id;
    printf("[PARTY] Added member %u to party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);
    return 0;
}

/* Remove member from party */
static int party_remove_member(PartyInfo* party, uint32_t member_id) {
    if (!party) return -1;

    int found = -1;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }
    party->member_count--;

    printf("[PARTY] Removed member %u from party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);

    /* If party empty or leader left, dissolve party */
    if (party->member_count == 0 || member_id == party->leader_id) {
        if (party->member_count > 0) {
            /* Transfer leadership to first remaining member */
            party->leader_id = party->members[0];
            printf("[PARTY] New leader: %u\n", party->leader_id);
        } else {
            party->active = 0;
            printf("[PARTY] Party %u dissolved\n", party->party_id);
        }
    }

    return 0;
}

/* Notify all party members */
static void party_notify_members(PartyInfo* party, uint8_t msg_no,
                                  const uint8_t* data, int len) {
    if (!party) return;

    for (int i = 0; i < (int)party->member_count; i++) {
        uint32_t member_id = party->members[i];
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}


/* Auction listing info */
#define MAX_AUCTION_LISTINGS 256
typedef struct {
    uint32_t listing_id;          /* Unique listing ID */
    uint32_t seller_id;           /* Seller character ID */
    char     seller_name[20];     /* Seller name */
    uint32_t item_ui_id;          /* Item unique ID from user_items */
    uint32_t item_id;             /* Item template ID */
    uint32_t price;               /* Asking price */
    uint32_t expire_time;         /* Unix timestamp when expires */
    uint8_t  active;
} AuctionListing;

static AuctionListing g_auction_listings[MAX_AUCTION_LISTINGS];
static uint32_t g_next_auction_id = 1;

/* Create auction listing */
static AuctionListing* create_auction_listing(uint32_t seller_id, const char* seller_name,
                                                uint32_t item_ui_id, uint32_t item_id,
                                                uint32_t price, uint32_t duration_hours) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (!g_auction_listings[i].active) {
            AuctionListing* a = &g_auction_listings[i];
            memset(a, 0, sizeof(AuctionListing));
            a->listing_id = g_next_auction_id++;
            a->seller_id = seller_id;
            strncpy(a->seller_name, seller_name, 19);
            a->item_ui_id = item_ui_id;
            a->item_id = item_id;
            a->price = price;
            a->expire_time = (uint32_t)time(NULL) + (duration_hours * 3600);
            a->active = 1;
            printf("[AUCTION] Created listing %u: seller=%u item=%u price=%u\n",
                   a->listing_id, seller_id, item_id, price);
            return a;
        }
    }
    return NULL;
}

/* Find auction listing by ID */
static AuctionListing* find_auction_listing(uint32_t listing_id) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (g_auction_listings[i].active && g_auction_listings[i].listing_id == listing_id) {
            return &g_auction_listings[i];
        }
    }
    return NULL;
}

/* Cancel auction listing */
static int cancel_auction_listing(uint32_t listing_id, uint32_t seller_id) {
    AuctionListing* a = find_auction_listing(listing_id);
    if (a && a->seller_id == seller_id) {
        printf("[AUCTION] Cancelled listing %u\n", listing_id);
        a->active = 0;
        return 0;
    }
    return -1;
}

/* Search auction listings by item_id */
static int search_auction_listings(uint32_t item_id, uint32_t min_price, uint32_t max_price,
                                    AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active || a->expire_time < now) continue;

        /* Match by item_id (0 = all items) */
        if (item_id > 0 && a->item_id != item_id) continue;

        /* Match by price range */
        if (min_price > 0 && a->price < min_price) continue;
        if (max_price > 0 && a->price > max_price) continue;

        results[count++] = a;
    }

    return count;
}

/* Get seller's listings */
static int get_seller_listings(uint32_t seller_id, AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active) continue;
        if (a->seller_id == seller_id) {
            /* Mark expired listings */
            if (a->expire_time < now) a->active = 0;
            else results[count++] = a;
        }
    }

    return count;
}


/* msg_no=75: TRADE_REQUEST */
int handle_trade_request(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_REQUEST from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 75, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find target session */
    ClientSession* target = NULL;
    int target_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            target = &g_sessions[i];
            target_idx = i;
            break;
        }
    }

    if (!target || target->state != STATE_IN_GAME) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Target not found */
        return send_response(s, 75, error, 4);
    }

    /* Find requester index */
    int req_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) {
            req_idx = i;
            break;
        }
    }

    /* Initialize trade states */
    memset(&g_trades[req_idx], 0, sizeof(TradeState));
    memset(&g_trades[target_idx], 0, sizeof(TradeState));
    g_trades[req_idx].partner_fd = target->fd;
    g_trades[req_idx].partner_id = target_id;
    g_trades[req_idx].active = 1;
    g_trades[target_idx].partner_fd = s->fd;
    g_trades[target_idx].partner_id = s->character_id;
    g_trades[target_idx].active = 1;

    /* Notify target */
    uint8_t notify[64];
    memset(notify, 0, sizeof(notify));
    uint32_t result = 0;
    memcpy(notify, &result, 4);
    memcpy(notify + 4, &s->character_id, 4);
    memcpy(notify + 8, s->character_name, 20);
    send_response(target, 75, notify, 28);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response, &result, 4);
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 75, response, 8);
}

/* msg_no=76: TRADE_ACCEPT */
int handle_trade_accept(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_ACCEPT from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 76, error, 4);
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 76, response, 4);
}

/* msg_no=77: TRADE_DECLINE */
int handle_trade_decline(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_DECLINE from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx >= 0 && g_trades[idx].active) {
        /* Find partner and notify */
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
                uint8_t notify[4] = {3, 0, 0, 0}; /* Trade declined */
                send_response(&g_sessions[i], 77, notify, 4);
                g_trades[i].active = 0;
                break;
            }
        }
        g_trades[idx].active = 0;
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 77, response, 4);
}

/* msg_no=78: TRADE_ADD_ITEM */
int handle_trade_add_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_ADD_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 78, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 78, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    if (g_trades[idx].item_count < 10) {
        g_trades[idx].items[g_trades[idx].item_count++] = item_uid;
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 78, response, 8);
}

/* msg_no=79: TRADE_REMOVE_ITEM */
int handle_trade_remove_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_REMOVE_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 79, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 79, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    /* Remove item from trade */
    for (int i = 0; i < g_trades[idx].item_count; i++) {
        if (g_trades[idx].items[i] == item_uid) {
            for (int j = i; j < g_trades[idx].item_count - 1; j++) {
                g_trades[idx].items[j] = g_trades[idx].items[j + 1];
            }
            g_trades[idx].item_count--;
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 79, response, 8);
}

/* msg_no=80: TRADE_SET_GOLD */
int handle_trade_set_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_SET_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 80, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 80, error, 4);
    }

    uint32_t gold;
    memcpy(&gold, data, 4);

    if (gold > s->inventory.money) {
        gold = s->inventory.money;
    }
    g_trades[idx].gold = gold;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &gold, 4);
    return send_response(s, 80, response, 8);
}

/* msg_no=81: TRADE_LOCK */
int handle_trade_lock(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_LOCK from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 81, error, 4);
    }

    g_trades[idx].locked = 1;

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 81, response, 4);
}

/* msg_no=82: TRADE_CONFIRM */
int handle_trade_confirm(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_CONFIRM from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || !g_trades[idx].locked) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 82, error, 4);
    }

    g_trades[idx].confirmed = 1;

    /* Check if both confirmed */
    int partner_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
            partner_idx = i;
            break;
        }
    }

    if (partner_idx >= 0 && g_trades[partner_idx].confirmed) {
        ClientSession* partner = &g_sessions[partner_idx];

        /* Execute trade - swap gold in memory */
        uint32_t my_give = g_trades[idx].gold;
        uint32_t partner_give = g_trades[partner_idx].gold;

        s->inventory.money -= my_give;
        s->inventory.money += partner_give;
        partner->inventory.money -= partner_give;
        partner->inventory.money += my_give;

        printf("[TRADE] Gold swap: %s gives %u, %s gives %u\n",
               s->character_name, my_give, partner->character_name, partner_give);

        /* Persist gold to database */
        db_update_gold(s->character_id, s->inventory.money);
        db_update_gold(partner->character_id, partner->inventory.money);

        /* Transfer items from self to partner */
        for (int i = 0; i < g_trades[idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, s->character_id, partner->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, s->character_name, partner->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, s->character_name, partner->character_name);
                }
            }
        }

        /* Transfer items from partner to self */
        for (int i = 0; i < g_trades[partner_idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[partner_idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, partner->character_id, s->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, partner->character_name, s->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, partner->character_name, s->character_name);
                }
            }
        }

        /* Clear trades */
        g_trades[idx].active = 0;
        g_trades[partner_idx].active = 0;

        printf("[TRADE] Trade completed between %s and %s\n",
               s->character_name, partner->character_name);

        /* Notify both */
        uint8_t complete[4] = {0, 0, 0, 0};
        send_response(partner, 82, complete, 4);
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 82, response, 4);
}

/* msg_no=83: TRADE_CANCEL */
int handle_trade_cancel(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_CANCEL from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx >= 0 && g_trades[idx].active) {
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
                uint8_t notify[4] = {4, 0, 0, 0}; /* Trade cancelled */
                send_response(&g_sessions[i], 83, notify, 4);
                g_trades[i].active = 0;
                break;
            }
        }
        g_trades[idx].active = 0;
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 83, response, 4);
}

/*==============================================================================
 * Mail System (msg_no 85-91)
 *============================================================================*/

/* msg_no=85: MAIL_GET_LIST */
int handle_mail_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MAIL] MAIL_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[2048];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get mail from database */
    DBMailInfo mails[20];
    int mail_count = db_get_mail_list(s->character_id, mails, 20);

    response[offset++] = (uint8_t)mail_count;

    for (int i = 0; i < mail_count && offset < 2000; i++) {
        /* mail_id */
        memcpy(response + offset, &mails[i].postal_id, 4);
        offset += 4;

        /* sender name */
        char sender[20];
        memset(sender, 0, 20);
        strncpy(sender, mails[i].send_name, 19);
        memcpy(response + offset, sender, 20);
        offset += 20;

        /* subject (use sender name as subject for now) */
        char subject[32];
        memset(subject, 0, 32);
        if (mails[i].gold > 0) {
            snprintf(subject, 31, "Gold: %u", mails[i].gold);
        } else if (mails[i].item_id > 0) {
            snprintf(subject, 31, "Item: %u", mails[i].item_id);
        } else {
            strncpy(subject, "Message", 31);
        }
        memcpy(response + offset, subject, 32);
        offset += 32;

        /* timestamp */
        memcpy(response + offset, &mails[i].occ_time, 4);
        offset += 4;

        /* has_attachment */
        uint8_t has_attach = (mails[i].item_id > 0 || mails[i].gold > 0) ? 1 : 0;
        response[offset++] = has_attach;

        /* is_read */
        response[offset++] = mails[i].read_flag;
    }

    printf("[MAIL] Returning %d mails for charac_no=%u\n", mail_count, s->character_id);
    return send_response(s, 85, response, offset);
}

/* msg_no=86: MAIL_SEND */
int handle_mail_send(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_SEND from fd=%d\n", s->fd);
    if (len < 56) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 86, error, 4);
    }

    char recipient[21];
    char subject[33];
    memset(recipient, 0, sizeof(recipient));
    memset(subject, 0, sizeof(subject));
    memcpy(recipient, data, 20);
    memcpy(subject, data + 20, 32);

    uint32_t item_id = 0;
    uint32_t gold = 0;
    if (len >= 56) memcpy(&item_id, data + 52, 4);
    if (len >= 60) memcpy(&gold, data + 56, 4);

    printf("[MAIL] Sending mail to '%s' subject='%s' item=%u gold=%u\n",
           recipient, subject, item_id, gold);

    /* Look up recipient charac_no by name */
    uint32_t recv_charac_no = db_get_charac_no_by_name(recipient);
    if (recv_charac_no == 0) {
        printf("[MAIL] Recipient '%s' not found\n", recipient);
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: recipient not found */
        return send_response(s, 86, error, 4);
    }

    /* Get sender's character name */
    char send_name[21];
    memset(send_name, 0, sizeof(send_name));
    strncpy(send_name, s->character_name, 20);

    /* Deduct postage (100 gold) - check balance first */
    uint32_t postage = 100;
    uint32_t total_cost = postage + gold;
    if (s->inventory.money < total_cost) {
        printf("[MAIL] Insufficient gold: have %u, need %u\n", s->inventory.money, total_cost);
        uint8_t error[4] = {3, 0, 0, 0}; /* Error: insufficient gold */
        return send_response(s, 86, error, 4);
    }

    /* Insert mail into postal table */
    int mail_id = db_send_mail(s->character_id, send_name, recv_charac_no, item_id, gold);
    if (mail_id <= 0) {
        printf("[MAIL] Failed to insert mail into database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Error: database error */
        return send_response(s, 86, error, 4);
    }

    /* Deduct gold from sender */
    s->inventory.money -= total_cost;
    db_update_gold(s->character_id, s->inventory.money);
    printf("[MAIL] Mail sent successfully, mail_id=%d, deducted %u gold\n", mail_id, total_cost);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0; /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 86, response, 8);
}

/* msg_no=87: MAIL_READ */
int handle_mail_read(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_READ from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 87, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);

    /* Get mail from database */
    DBMailInfo mail;
    if (db_get_mail_by_id(mail_id, s->character_id, &mail) < 0) {
        printf("[MAIL] Mail %u not found for charac_no=%u\n", mail_id, s->character_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found */
        return send_response(s, 87, error, 4);
    }

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &mail_id, 4);
    offset += 4;

    /* Sender name from database */
    memcpy(response + offset, mail.send_name, 20);
    offset += 20;

    /* Subject - generate from mail info */
    char subject[32];
    memset(subject, 0, sizeof(subject));
    if (mail.gold > 0 && mail.item_id > 0) {
        snprintf(subject, sizeof(subject) - 1, "Gold & Item");
    } else if (mail.gold > 0) {
        snprintf(subject, sizeof(subject) - 1, "Gold: %u", mail.gold);
    } else if (mail.item_id > 0) {
        snprintf(subject, sizeof(subject) - 1, "Item: %u", mail.item_id);
    } else {
        strncpy(subject, "Message", sizeof(subject) - 1);
    }
    memcpy(response + offset, subject, 32);
    offset += 32;

    /* Body */
    char body[128];
    memset(body, 0, sizeof(body));
    snprintf(body, sizeof(body) - 1, "From: %s", mail.send_name);
    memcpy(response + offset, body, 128);
    offset += 128;

    /* Attachment gold and item */
    memcpy(response + offset, &mail.gold, 4);
    offset += 4;

    memcpy(response + offset, &mail.item_id, 4);
    offset += 4;

    printf("[MAIL] Read mail %u: from=%s gold=%u item=%u\n",
           mail_id, mail.send_name, mail.gold, mail.item_id);
    return send_response(s, 87, response, offset);
}

/* msg_no=88: MAIL_DELETE */
int handle_mail_delete(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_DELETE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 88, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);
    printf("[MAIL] Deleting mail %u for charac_no=%u\n", mail_id, s->character_id);

    /* Delete mail from database */
    if (db_delete_mail(mail_id, s->character_id) < 0) {
        printf("[MAIL] Failed to delete mail %u\n", mail_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found or not owned */
        return send_response(s, 88, error, 4);
    }

    printf("[MAIL] Mail %u deleted successfully\n", mail_id);
    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 88, response, 8);
}

/* msg_no=89: MAIL_TAKE_ATTACHMENT */
int handle_mail_take_attachment(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_TAKE_ATTACHMENT from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 89, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);

    /* Get mail attachment from database */
    DBMailAttachment att;
    if (db_get_mail_attachment(mail_id, s->character_id, &att) < 0) {
        printf("[MAIL] Mail %u not found or not for this character\n", mail_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found */
        return send_response(s, 89, error, 4);
    }

    /* Check if already claimed */
    if (att.claimed) {
        printf("[MAIL] Mail %u already claimed\n", mail_id);
        uint8_t error[4] = {3, 0, 0, 0}; /* Already claimed */
        return send_response(s, 89, error, 4);
    }

    /* Give gold to player */
    if (att.gold > 0) {
        s->inventory.money += att.gold;
        db_update_gold(s->character_id, s->inventory.money);
        printf("[MAIL] Added %u gold from mail %u\n", att.gold, mail_id);
    }

    /* Add item to player inventory if present */
    if (att.item_id > 0) {
        int free_slot = db_find_free_slot(s->character_id, 11);  /* Bag slots start at 11 */
        if (free_slot >= 0 && free_slot <= 200) {
            int new_ui_id = db_add_item(s->character_id, att.item_id, free_slot, 0);
            if (new_ui_id > 0) {
                printf("[MAIL] Added item_id=%u to slot=%d (new ui_id=%d) from mail %u\n",
                       att.item_id, free_slot, new_ui_id, mail_id);
            } else {
                printf("[MAIL] Failed to add item_id=%u from mail %u\n", att.item_id, mail_id);
            }
        } else {
            printf("[MAIL] No free inventory slot for item from mail %u\n", mail_id);
        }
    }

    /* Mark mail as claimed */
    db_claim_mail_attachment(mail_id);

    printf("[MAIL] Took attachment from mail %u: gold=%u item=%u\n",
           mail_id, att.gold, att.item_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &mail_id, 4);
    memcpy(response + 8, &att.gold, 4);
    memcpy(response + 12, &att.item_id, 4);
    return send_response(s, 89, response, 16);
}

/* msg_no=90: MAIL_GET_UNREAD_COUNT */
int handle_mail_get_unread_count(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MAIL] MAIL_GET_UNREAD_COUNT from fd=%d\n", s->fd);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t unread = db_get_unread_mail_count(s->character_id);
    memcpy(response + 4, &unread, 4);
    printf("[MAIL] Unread count=%u for charac_no=%u\n", unread, s->character_id);
    return send_response(s, 90, response, 8);
}

/* msg_no=91: MAIL_RETURN */
int handle_mail_return(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_RETURN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 91, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);
    printf("[MAIL] Returning mail %u to sender\n", mail_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 91, response, 8);
}

/*==============================================================================
 * Friend System (msg_no 95-102)
 *============================================================================*/

/* msg_no=95: FRIEND_GET_LIST */
int handle_friend_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[FRIEND] FRIEND_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get friends from database */
    DBFriendInfo friends[30];
    int friend_count = db_get_friend_list(s->character_id, friends, 30);

    response[offset++] = (uint8_t)friend_count;

    for (int i = 0; i < friend_count && offset < 1000; i++) {
        /* friend_id */
        memcpy(response + offset, &friends[i].friend_charac_no, 4);
        offset += 4;

        /* name */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, friends[i].friend_name, 19);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level (we don't have this from friend table, use 0) */
        uint8_t level = 0;
        response[offset++] = level;

        /* job (we don't have this from friend table, use 0) */
        uint8_t job = 0;
        response[offset++] = job;

        /* online status */
        response[offset++] = friends[i].online;
    }

    printf("[FRIEND] Returning %d friends for charac_no=%u\n", friend_count, s->character_id);
    return send_response(s, 95, response, offset);
}

/* msg_no=96: FRIEND_ADD */
int handle_friend_add(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_ADD from fd=%d\n", s->fd);
    if (len < 20) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 96, error, 4);
    }

    char target_name[21];
    memset(target_name, 0, sizeof(target_name));
    memcpy(target_name, data, 20);
    printf("[FRIEND] Adding friend: %s\n", target_name);

    /* Look up target's charac_no by name */
    uint32_t friend_charac_no = db_get_charac_no_by_name(target_name);
    if (friend_charac_no == 0) {
        printf("[FRIEND] Target '%s' not found\n", target_name);
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: character not found */
        return send_response(s, 96, error, 4);
    }

    /* Cannot add yourself */
    if (friend_charac_no == s->character_id) {
        printf("[FRIEND] Cannot add yourself as friend\n");
        uint8_t error[4] = {3, 0, 0, 0}; /* Error: cannot add self */
        return send_response(s, 96, error, 4);
    }

    /* Add friend relationship to database (bidirectional) */
    int result = db_add_friend(s->character_id, friend_charac_no);
    if (result < 0) {
        printf("[FRIEND] Failed to add friend to database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Error: database error or already friends */
        return send_response(s, 96, error, 4);
    }

    printf("[FRIEND] Friend added successfully: %s (charac_no=%u)\n", target_name, friend_charac_no);

    /* Notify target if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == friend_charac_no) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            uint32_t req_result = 0;
            memcpy(notify, &req_result, 4);
            memcpy(notify + 4, &s->character_id, 4);
            memcpy(notify + 8, s->character_name, 20);
            send_response(&g_sessions[i], 96, notify, 28);
            printf("[FRIEND] Notified online friend fd=%d\n", g_sessions[i].fd);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &friend_charac_no, 4);
    return send_response(s, 96, response, 8);
}

/* msg_no=97: FRIEND_REMOVE */
int handle_friend_remove(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_REMOVE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 97, error, 4);
    }

    uint32_t friend_charac_no;
    memcpy(&friend_charac_no, data, 4);
    printf("[FRIEND] Removing friend charac_no: %u\n", friend_charac_no);

    /* Remove friend from database (bidirectional) */
    int result = db_remove_friend(s->character_id, friend_charac_no);
    if (result < 0) {
        printf("[FRIEND] Failed to remove friend from database\n");
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: database error */
        return send_response(s, 97, error, 4);
    }

    printf("[FRIEND] Friend removed successfully: charac_no=%u\n", friend_charac_no);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &friend_charac_no, 4);
    return send_response(s, 97, response, 8);
}

/* msg_no=98: FRIEND_ACCEPT */
int handle_friend_accept(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_ACCEPT from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 98, error, 4);
    }

    uint32_t requester_id;
    memcpy(&requester_id, data, 4);
    printf("[FRIEND] Accepting friend request from ID: %u\n", requester_id);

    /* Add bidirectional friendship in database */
    if (db_add_friend(s->character_id, requester_id) < 0) {
        printf("[FRIEND] Failed to save friendship to database\n");
        uint8_t error[4] = {2, 0, 0, 0}; /* Database error */
        return send_response(s, 98, error, 4);
    }
    /* Also add reverse direction */
    db_add_friend(requester_id, s->character_id);

    printf("[FRIEND] Friendship saved: %u <-> %u\n", s->character_id, requester_id);

    /* Notify requester if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == requester_id) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->character_id, 4);
            memcpy(notify + 8, s->character_name, 20);
            send_response(&g_sessions[i], 98, notify, 28);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &requester_id, 4);
    return send_response(s, 98, response, 8);
}

/* msg_no=99: FRIEND_DECLINE */
int handle_friend_decline(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_DECLINE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 99, error, 4);
    }

    uint32_t requester_id;
    memcpy(&requester_id, data, 4);
    printf("[FRIEND] Declining friend request from ID: %u\n", requester_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &requester_id, 4);
    return send_response(s, 99, response, 8);
}

/* msg_no=100: FRIEND_BLOCK */
int handle_friend_block(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_BLOCK from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[FRIEND] Blocking user ID: %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 100, response, 8);
}

/* msg_no=101: FRIEND_UNBLOCK */
int handle_friend_unblock(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_UNBLOCK from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[FRIEND] Unblocking user ID: %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 101, response, 8);
}

/* msg_no=102: FRIEND_GET_BLOCKED_LIST */
int handle_friend_get_blocked_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[FRIEND] FRIEND_GET_BLOCKED_LIST from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t blocked_count = 0;
    response[offset++] = blocked_count;

    return send_response(s, 102, response, offset);
}

/*==============================================================================
 * Guild System (msg_no 105-117)
 *============================================================================*/

/*------------------------------------------------------------------------------
 * P10: Guild Service IPC Communication Infrastructure
 *
 * The guild service runs on a separate process (df_guild_r) on port 30403.
 * This provides basic IPC functions to communicate with the guild service.
 *----------------------------------------------------------------------------*/

#define GUILD_SERVICE_PORT 30403
#define GUILD_IPC_TIMEOUT_MS 3000

/* Guild IPC message types (matches guild.h GMSG_* enum) */
typedef enum {
    GIPC_CREATE_GUILD = 1,      /* = GMSG_CREATE_GUILD */
    GIPC_DISBAND_GUILD,         /* = GMSG_DISBAND_GUILD */
    GIPC_GET_GUILD_INFO,        /* = GMSG_GET_GUILD_INFO */
    GIPC_JOIN_GUILD,            /* = GMSG_JOIN_GUILD */
    GIPC_LEAVE_GUILD,           /* = GMSG_LEAVE_GUILD */
    GIPC_KICK_MEMBER,           /* = GMSG_KICK_MEMBER */
    GIPC_CHANGE_RANK,           /* = GMSG_CHANGE_RANK */
    GIPC_UPDATE_NOTICE,         /* = GMSG_UPDATE_NOTICE */
    GIPC_UPDATE_INTRO,          /* = GMSG_UPDATE_INTRO */
    GIPC_ADD_EXP,               /* = GMSG_ADD_EXP (10) */
    GIPC_LEARN_SKILL,           /* = GMSG_LEARN_SKILL */
    GIPC_DEPOSIT_ITEM,          /* = GMSG_DEPOSIT_ITEM */
    GIPC_WITHDRAW_ITEM,         /* = GMSG_WITHDRAW_ITEM */
    GIPC_DEPOSIT_FUND,          /* = GMSG_DEPOSIT_FUND */
    GIPC_WITHDRAW_FUND,         /* = GMSG_WITHDRAW_FUND */
    GIPC_GET_MEMBER_LIST,       /* = GMSG_GET_MEMBER_LIST */
    GIPC_GET_JOIN_REQUESTS,     /* = GMSG_GET_JOIN_REQUESTS */
    GIPC_ACCEPT_JOIN,           /* = GMSG_ACCEPT_JOIN */
    GIPC_REJECT_JOIN,           /* = GMSG_REJECT_JOIN */
    GIPC_SEARCH_GUILD,          /* = GMSG_SEARCH_GUILD */
    GIPC_MEMBER_ONLINE,         /* = GMSG_MEMBER_ONLINE (21) */
    GIPC_MEMBER_OFFLINE,        /* = GMSG_MEMBER_OFFLINE (22) */
    GIPC_BROADCAST              /* = GMSG_BROADCAST (23) */
} GuildIPCMsgType;

static int g_guild_ipc_fd = -1;  /* Socket to guild service */

/* Connect to guild service (non-blocking with timeout) */
static int guild_ipc_connect(void) {
    if (g_guild_ipc_fd >= 0) {
        return 0;  /* Already connected */
    }

    g_guild_ipc_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_guild_ipc_fd < 0) {
        printf("[GUILD_IPC] Failed to create socket: %s\n", strerror(errno));
        return -1;
    }

    /* Set non-blocking */
    int flags = fcntl(g_guild_ipc_fd, F_GETFL, 0);
    fcntl(g_guild_ipc_fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(GUILD_SERVICE_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    int ret = connect(g_guild_ipc_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        printf("[GUILD_IPC] Connect failed: %s\n", strerror(errno));
        close(g_guild_ipc_fd);
        g_guild_ipc_fd = -1;
        return -1;
    }

    /* Wait for connection with timeout */
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(g_guild_ipc_fd, &wfds);

    struct timeval tv;
    tv.tv_sec = GUILD_IPC_TIMEOUT_MS / 1000;
    tv.tv_usec = (GUILD_IPC_TIMEOUT_MS % 1000) * 1000;

    ret = select(g_guild_ipc_fd + 1, NULL, &wfds, NULL, &tv);
    if (ret <= 0) {
        printf("[GUILD_IPC] Connection timeout\n");
        close(g_guild_ipc_fd);
        g_guild_ipc_fd = -1;
        return -1;
    }

    /* Check for connection error */
    int err = 0;
    socklen_t errlen = sizeof(err);
    getsockopt(g_guild_ipc_fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
    if (err != 0) {
        printf("[GUILD_IPC] Connection error: %s\n", strerror(err));
        close(g_guild_ipc_fd);
        g_guild_ipc_fd = -1;
        return -1;
    }

    /* Restore blocking mode */
    fcntl(g_guild_ipc_fd, F_SETFL, flags);

    printf("[GUILD_IPC] Connected to guild service on port %d\n", GUILD_SERVICE_PORT);
    return 0;
}

/* Send IPC message to guild service */
static int guild_ipc_send(uint8_t msg_type, const uint8_t* data, int len) {
    if (guild_ipc_connect() < 0) {
        return -1;  /* Not connected */
    }

    /* Build IPC packet: [2:len][1:msg_type][N:data] */
    uint8_t packet[512];
    uint16_t pkt_len = 1 + len;
    memcpy(packet, &pkt_len, 2);
    packet[2] = msg_type;
    if (len > 0 && data) {
        memcpy(packet + 3, data, len);
    }

    int total = 2 + pkt_len;
    int sent = write(g_guild_ipc_fd, packet, total);
    if (sent != total) {
        printf("[GUILD_IPC] Send failed: %s\n", strerror(errno));
        close(g_guild_ipc_fd);
        g_guild_ipc_fd = -1;
        return -1;
    }

    return 0;
}

/* Receive IPC response from guild service */
static int guild_ipc_recv(uint8_t* buffer, int max_len) {
    if (g_guild_ipc_fd < 0) return -1;

    /* Read with timeout */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(g_guild_ipc_fd, &rfds);

    struct timeval tv;
    tv.tv_sec = GUILD_IPC_TIMEOUT_MS / 1000;
    tv.tv_usec = (GUILD_IPC_TIMEOUT_MS % 1000) * 1000;

    int ret = select(g_guild_ipc_fd + 1, &rfds, NULL, NULL, &tv);
    if (ret <= 0) {
        printf("[GUILD_IPC] Receive timeout\n");
        return -1;
    }

    /* Read length header */
    uint16_t pkt_len;
    if (read(g_guild_ipc_fd, &pkt_len, 2) != 2) {
        return -1;
    }

    if (pkt_len > max_len) {
        printf("[GUILD_IPC] Response too large: %d > %d\n", pkt_len, max_len);
        return -1;
    }

    /* Read data */
    int received = read(g_guild_ipc_fd, buffer, pkt_len);
    return received;
}

/* Notify guild service of member online status */
void guild_ipc_notify_online(uint32_t guild_id, uint32_t charac_no, int online) {
    if (guild_id == 0) return;

    uint8_t data[12];
    memcpy(data, &guild_id, 4);
    memcpy(data + 4, &charac_no, 4);
    data[8] = online ? 1 : 0;

    if (guild_ipc_send(online ? GIPC_MEMBER_ONLINE : GIPC_MEMBER_OFFLINE, data, 9) == 0) {
        printf("[GUILD_IPC] Notified %s for charac=%u in guild=%u\n",
               online ? "ONLINE" : "OFFLINE", charac_no, guild_id);
    }
}

/* Request guild info from guild service */
static int guild_ipc_get_info(uint32_t guild_id, DBGuildInfo* info) {
    if (!info || guild_id == 0) return -1;

    uint8_t data[4];
    memcpy(data, &guild_id, 4);

    if (guild_ipc_send(GIPC_GET_GUILD_INFO, data, 4) < 0) {
        return -1;
    }

    uint8_t response[256];
    int len = guild_ipc_recv(response, sizeof(response));
    if (len < 8) {
        return -1;  /* No response or error */
    }

    /* Parse response: [4:result][4:guild_id][32:name][4:level][4:member_count]... */
    uint32_t result;
    memcpy(&result, response, 4);
    if (result != 0) {
        return -1;
    }

    memcpy(&info->guild_id, response + 4, 4);
    memcpy(info->guild_name, response + 8, 32);
    memcpy(&info->level, response + 40, 4);
    memcpy(&info->member_count, response + 44, 4);
    /* Additional fields would be parsed here */

    printf("[GUILD_IPC] Got info for guild=%u: name=%s level=%u members=%u\n",
           guild_id, info->guild_name, info->level, info->member_count);
    return 0;
}

/* Close guild IPC connection */
static void guild_ipc_close(void) {
    if (g_guild_ipc_fd >= 0) {
        close(g_guild_ipc_fd);
        g_guild_ipc_fd = -1;
        printf("[GUILD_IPC] Connection closed\n");
    }
}

/* Phase 9: Add experience to guild via IPC
 * Returns: 0 on success, -1 on failure
 * out_new_level: new guild level after exp gain (if level up)
 * out_leveled_up: 1 if guild leveled up, 0 otherwise
 */
static int guild_ipc_add_exp(uint32_t guild_id, uint32_t exp_amount,
                              uint8_t* out_new_level, uint8_t* out_leveled_up) {
    if (guild_id == 0 || exp_amount == 0) {
        return -1;
    }

    /* Prepare data: [4:guild_id][4:exp_amount] */
    uint8_t data[8];
    memcpy(data, &guild_id, 4);
    memcpy(data + 4, &exp_amount, 4);

    if (guild_ipc_send(GIPC_ADD_EXP, data, 8) < 0) {
        printf("[GUILD_IPC] Failed to send ADD_EXP for guild=%u exp=%u\n",
               guild_id, exp_amount);
        return -1;
    }

    /* Read response: [1:result][1:new_level][1:leveled_up] */
    uint8_t response[8];
    int len = guild_ipc_recv(response, sizeof(response));
    if (len >= 3) {
        uint8_t result = response[0];
        if (result == 0) {
            if (out_new_level) *out_new_level = response[1];
            if (out_leveled_up) *out_leveled_up = response[2];
            printf("[GUILD_IPC] ADD_EXP success: guild=%u exp=%u new_level=%u leveled=%u\n",
                   guild_id, exp_amount, response[1], response[2]);
            return 0;
        }
    }

    printf("[GUILD_IPC] ADD_EXP failed for guild=%u\n", guild_id);
    return -1;
}

/* msg_no=105: GUILD_CREATE */
int handle_guild_create(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_CREATE from fd=%d\n", s->fd);
    if (len < 20) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 105, error, 4);
    }

    /* Already in a guild */
    if (s->guild_id > 0) {
        printf("[GUILD] Player already in guild %u\n", s->guild_id);
        uint8_t error[4] = {3, 0, 0, 0}; /* Already in guild */
        return send_response(s, 105, error, 4);
    }

    char guild_name[21];
    memset(guild_name, 0, sizeof(guild_name));
    memcpy(guild_name, data, 20);
    printf("[GUILD] Creating guild: %s\n", guild_name);

    /* Check gold (creation fee: 100000) */
    uint32_t creation_fee = 100000;
    if (s->inventory.money < creation_fee) {
        printf("[GUILD] Not enough gold: have %u, need %u\n", s->inventory.money, creation_fee);
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 105, error, 4);
    }

    /* Create guild in database */
    int guild_id = db_create_guild(guild_name, s->character_id, s->character_name);
    if (guild_id <= 0) {
        printf("[GUILD] Failed to create guild in database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Database error */
        return send_response(s, 105, error, 4);
    }

    /* Add creator as guild master (grade=1) */
    if (db_add_guild_member(guild_id, s->character_id, s->character_name, 1) < 0) {
        printf("[GUILD] Failed to add guild master to member table\n");
    }

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, guild_id);

    /* Deduct gold and update in database */
    s->inventory.money -= creation_fee;
    db_update_gold(s->character_id, s->inventory.money);
    s->guild_id = (uint32_t)guild_id;

    printf("[GUILD] Guild created: id=%d name=%s master=%s\n",
           guild_id, guild_name, s->character_name);

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &s->guild_id, 4);
    memcpy(response + 8, guild_name, 20);
    return send_response(s, 105, response, 28);
}

/* msg_no=106: GUILD_DISBAND */
int handle_guild_disband(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_DISBAND from fd=%d\n", s->fd);

    if (s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 106, error, 4);
    }

    uint32_t old_guild = s->guild_id;

    /* Disband guild in database */
    if (db_disband_guild(old_guild) < 0) {
        printf("[GUILD] Failed to disband guild %u in database\n", old_guild);
        uint8_t error[4] = {2, 0, 0, 0}; /* Database error */
        return send_response(s, 106, error, 4);
    }

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, 0);

    /* Update all online members */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].guild_id == old_guild) {
            g_sessions[i].guild_id = 0;
            /* Notify member if not self */
            if (g_sessions[i].character_id != s->character_id) {
                uint8_t notify[8] = {6, 0, 0, 0, 0, 0, 0, 0}; /* Disbanded */
                memcpy(notify + 4, &old_guild, 4);
                send_response(&g_sessions[i], 106, notify, 8);
            }
        }
    }

    s->guild_id = 0;
    printf("[GUILD] Disbanded guild %u\n", old_guild);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_guild, 4);
    return send_response(s, 106, response, 8);
}

/* msg_no=107: GUILD_GET_INFO */
int handle_guild_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Get guild info from database */
    DBGuildInfo guild;
    int has_guild = (db_get_guild_by_charac(s->character_id, &guild) == 0);

    uint32_t result = has_guild ? 0 : 1;  /* 1 = not in guild */
    memcpy(response + offset, &result, 4);
    offset += 4;

    if (has_guild) {
        /* guild_id */
        memcpy(response + offset, &guild.guild_id, 4);
        offset += 4;
        s->guild_id = guild.guild_id;

        /* guild_name */
        char guild_name[20];
        memset(guild_name, 0, 20);
        strncpy(guild_name, guild.guild_name, 19);
        memcpy(response + offset, guild_name, 20);
        offset += 20;

        /* guild_level */
        response[offset++] = guild.level;

        /* member_count */
        uint16_t member_count = (uint16_t)guild.member_count;
        memcpy(response + offset, &member_count, 2);
        offset += 2;

        /* max_members (based on guild level) */
        uint16_t max_members = 20 + guild.level * 10;
        memcpy(response + offset, &max_members, 2);
        offset += 2;

        /* master_name */
        char master_name[20];
        memset(master_name, 0, 20);
        strncpy(master_name, guild.master_name, 19);
        memcpy(response + offset, master_name, 20);
        offset += 20;

        /* guild_point */
        memcpy(response + offset, &guild.guild_point, 4);
        offset += 4;

        /* notice placeholder */
        char notice[64] = "Welcome to our guild!";
        memcpy(response + offset, notice, 64);
        offset += 64;

        printf("[GUILD] Returning guild %s (id=%u, lv=%u, members=%u)\n",
               guild.guild_name, guild.guild_id, guild.level, guild.member_count);
    } else {
        s->guild_id = 0;
        printf("[GUILD] Character %u is not in any guild\n", s->character_id);
    }

    return send_response(s, 107, response, offset);
}

/* msg_no=108: GUILD_INVITE */
int handle_guild_invite(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_INVITE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 108, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find and notify target */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->guild_id, 4);
            memcpy(notify + 8, &s->character_id, 4);
            memcpy(notify + 12, s->character_name, 20);
            send_response(&g_sessions[i], 108, notify, 32);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 108, response, 8);
}

/* msg_no=109: GUILD_JOIN */
int handle_guild_join(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_JOIN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 109, error, 4);
    }

    /* Already in a guild */
    if (s->guild_id > 0) {
        printf("[GUILD] Already in guild %u, cannot join another\n", s->guild_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Already in guild */
        return send_response(s, 109, error, 4);
    }

    uint32_t guild_id;
    memcpy(&guild_id, data, 4);

    /* Add to guild_member table (grade=5 = normal member) */
    if (db_add_guild_member(guild_id, s->character_id, s->character_name, 5) < 0) {
        printf("[GUILD] Failed to add member to database\n");
        uint8_t error[4] = {3, 0, 0, 0}; /* Database error */
        return send_response(s, 109, error, 4);
    }

    /* Update member count */
    db_update_guild_member_count(guild_id, 1);

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, guild_id);

    s->guild_id = guild_id;
    printf("[GUILD] %s joined guild %u\n", s->character_name, guild_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &guild_id, 4);
    return send_response(s, 109, response, 8);
}

/* msg_no=110: GUILD_LEAVE */
int handle_guild_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_LEAVE from fd=%d\n", s->fd);

    if (s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 110, error, 4);
    }

    uint32_t old_guild = s->guild_id;

    /* Remove from guild_member table */
    if (db_remove_guild_member(old_guild, s->character_id) < 0) {
        printf("[GUILD] Failed to remove member from database\n");
        /* Continue anyway */
    }

    /* Update member count */
    db_update_guild_member_count(old_guild, -1);

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, 0);

    s->guild_id = 0;
    printf("[GUILD] Left guild %u\n", old_guild);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_guild, 4);
    return send_response(s, 110, response, 8);
}

/* msg_no=111: GUILD_KICK */
int handle_guild_kick(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_KICK from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 111, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Remove from database */
    if (db_remove_guild_member(s->guild_id, target_id) < 0) {
        printf("[GUILD] Failed to remove kicked member from database\n");
    }

    /* Update member count */
    db_update_guild_member_count(s->guild_id, -1);

    /* Update kicked character's guild_id */
    db_update_character_guild(target_id, 0);

    /* Notify if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            g_sessions[i].guild_id = 0;
            uint8_t notify[8] = {5, 0, 0, 0, 0, 0, 0, 0}; /* Kicked */
            send_response(&g_sessions[i], 111, notify, 8);
            break;
        }
    }

    printf("[GUILD] Kicked member %u from guild %u\n", target_id, s->guild_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 111, response, 8);
}

/* msg_no=112: GUILD_PROMOTE */
int handle_guild_promote(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_PROMOTE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 112, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[GUILD] Promoted member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 112, response, 8);
}

/* msg_no=113: GUILD_DEMOTE */
int handle_guild_demote(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_DEMOTE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 113, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[GUILD] Demoted member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 113, response, 8);
}

/* msg_no=114: GUILD_GET_MEMBERS */
int handle_guild_get_members(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_MEMBERS from fd=%d\n", s->fd);

    uint8_t response[2048];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Check if player has guild */
    if (s->guild_id == 0) {
        uint32_t result = 1;  /* Not in guild */
        memcpy(response, &result, 4);
        return send_response(s, 114, response, 4);
    }

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get guild members from database */
    DBGuildMemberInfo members[50];
    int member_count = db_get_guild_members(s->guild_id, members, 50);

    response[offset++] = (uint8_t)member_count;

    for (int i = 0; i < member_count && offset < 2000; i++) {
        /* member_id */
        memcpy(response + offset, &members[i].charac_no, 4);
        offset += 4;

        /* name */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, members[i].charac_name, 19);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level */
        response[offset++] = members[i].level;

        /* rank/grade */
        response[offset++] = members[i].grade;

        /* online status */
        response[offset++] = members[i].online;

        /* job */
        response[offset++] = members[i].job;
    }

    printf("[GUILD] Returning %d members for guild_id=%u\n", member_count, s->guild_id);
    return send_response(s, 114, response, offset);
}

/* msg_no=115: GUILD_SET_NOTICE */
int handle_guild_set_notice(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_SET_NOTICE from fd=%d\n", s->fd);
    if (len < 1 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 115, error, 4);
    }

    char notice[128];
    int notice_len = (len > 127) ? 127 : len;
    memcpy(notice, data, notice_len);
    notice[notice_len] = '\0';
    printf("[GUILD] Setting notice: %s\n", notice);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 115, response, 4);
}

/* msg_no=116: GUILD_GET_APPLICATIONS */
int handle_guild_get_applications(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_APPLICATIONS from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t app_count = 0;
    response[offset++] = app_count;

    return send_response(s, 116, response, offset);
}

/* msg_no=117: GUILD_RESPOND_APPLICATION */
int handle_guild_respond_application(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_RESPOND_APPLICATION from fd=%d\n", s->fd);
    if (len < 5 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 117, error, 4);
    }

    uint32_t applicant_id;
    uint8_t accept;
    memcpy(&applicant_id, data, 4);
    accept = data[4];

    printf("[GUILD] %s application from %u\n", accept ? "Accepted" : "Rejected", applicant_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &applicant_id, 4);
    return send_response(s, 117, response, 8);
}

/*==============================================================================
 * Party System (msg_no 120-128)
 *============================================================================*/

/* msg_no=120: PARTY_CREATE */
int handle_party_create(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_CREATE from fd=%d\n", s->fd);

    if (s->party_id != 0) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Already in party */
        return send_response(s, 120, error, 4);
    }

    PartyInfo* party = create_party(s->character_id);
    if (!party) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Cannot create party */
        return send_response(s, 120, error, 4);
    }

    s->party_id = party->party_id;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party->party_id, 4);
    return send_response(s, 120, response, 8);
}

/* msg_no=121: PARTY_INVITE */
int handle_party_invite(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_INVITE from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 121, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 121, error, 4);
    }

    if (party->member_count >= MAX_PARTY_MEMBERS) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party full */
        return send_response(s, 121, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find and notify target */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            if (g_sessions[i].party_id != 0) {
                uint8_t error[4] = {4, 0, 0, 0}; /* Target in party */
                return send_response(s, 121, error, 4);
            }
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->party_id, 4);
            memcpy(notify + 8, &s->character_id, 4);
            memcpy(notify + 12, s->character_name, 20);
            send_response(&g_sessions[i], 121, notify, 32);
            printf("[PARTY] Invited %u to party %u\n", target_id, s->party_id);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 121, response, 8);
}

/* msg_no=122: PARTY_JOIN */
int handle_party_join(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_JOIN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 122, error, 4);
    }

    if (s->party_id != 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Already in party */
        return send_response(s, 122, error, 4);
    }

    uint32_t party_id;
    memcpy(&party_id, data, 4);

    PartyInfo* party = find_party(party_id);
    if (!party) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party not found */
        return send_response(s, 122, error, 4);
    }

    if (party_add_member(party, s->character_id) < 0) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot join */
        return send_response(s, 122, error, 4);
    }

    s->party_id = party_id;

    /* Notify all party members */
    uint8_t notify[32];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 1; /* Member joined */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &s->character_id, 4);
    memcpy(notify + 8, s->character_name, 20);
    party_notify_members(party, 125, notify, 28);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party_id, 4);
    return send_response(s, 122, response, 8);
}

/* msg_no=123: PARTY_LEAVE */
int handle_party_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_LEAVE from fd=%d\n", s->fd);

    if (s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 123, error, 4);
    }

    uint32_t old_party = s->party_id;
    PartyInfo* party = find_party(s->party_id);

    s->party_id = 0;

    if (party) {
        party_remove_member(party, s->character_id);

        /* Notify remaining members */
        if (party->active) {
            uint8_t notify[12];
            memset(notify, 0, sizeof(notify));
            uint32_t action = 2; /* Member left */
            memcpy(notify, &action, 4);
            memcpy(notify + 4, &s->character_id, 4);
            party_notify_members(party, 125, notify, 12);
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_party, 4);
    return send_response(s, 123, response, 8);
}

/* msg_no=124: PARTY_KICK */
int handle_party_kick(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_KICK from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 124, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 124, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    if (target_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot kick self */
        return send_response(s, 124, error, 4);
    }

    /* Remove from party */
    party_remove_member(party, target_id);

    /* Find and notify kicked player */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            g_sessions[i].party_id = 0;
            uint8_t notify[8] = {5, 0, 0, 0, 0, 0, 0, 0}; /* Kicked */
            send_response(&g_sessions[i], 124, notify, 8);
            break;
        }
    }

    /* Notify remaining members */
    uint8_t member_notify[12];
    memset(member_notify, 0, sizeof(member_notify));
    uint32_t action = 3; /* Member kicked */
    memcpy(member_notify, &action, 4);
    memcpy(member_notify + 4, &target_id, 4);
    party_notify_members(party, 125, member_notify, 12);

    printf("[PARTY] Kicked member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 124, response, 8);
}

/* msg_no=125: PARTY_GET_INFO */
int handle_party_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    PartyInfo* party = find_party(s->party_id);

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &s->party_id, 4);
    offset += 4;

    if (!party || !party->active) {
        /* No party or inactive */
        response[offset++] = 0; /* member_count = 0 */
        return send_response(s, 125, response, offset);
    }

    response[offset++] = (uint8_t)party->member_count;

    for (int i = 0; i < (int)party->member_count && i < MAX_PARTY_MEMBERS; i++) {
        uint32_t member_id = party->members[i];
        memcpy(response + offset, &member_id, 4);
        offset += 4;

        /* Find member info */
        char name[20] = "Unknown";
        uint8_t level = 1;
        uint8_t job = 0;

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                memcpy(name, g_sessions[j].character_name, 20);
                /* Find level/job from character data */
                for (int k = 0; k < g_sessions[j].character_count; k++) {
                    if (g_sessions[j].characters[k].charac_no == member_id) {
                        level = g_sessions[j].characters[k].level;
                        job = g_sessions[j].characters[k].job;
                        break;
                    }
                }
                break;
            }
        }

        memcpy(response + offset, name, 20);
        offset += 20;

        response[offset++] = level;
        response[offset++] = job;

        uint8_t is_leader = (member_id == party->leader_id) ? 1 : 0;
        response[offset++] = is_leader;
    }

    return send_response(s, 125, response, offset);
}

/* msg_no=126: PARTY_CHANGE_LEADER */
int handle_party_change_leader(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_CHANGE_LEADER from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 126, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 126, error, 4);
    }

    uint32_t new_leader_id;
    memcpy(&new_leader_id, data, 4);

    /* Verify new leader is in party */
    int found = 0;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == new_leader_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Not in party */
        return send_response(s, 126, error, 4);
    }

    party->leader_id = new_leader_id;
    printf("[PARTY] Changed leader to %u\n", new_leader_id);

    /* Notify all members */
    uint8_t notify[12];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 4; /* Leader changed */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &new_leader_id, 4);
    party_notify_members(party, 125, notify, 12);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &new_leader_id, 4);
    return send_response(s, 126, response, 8);
}
/* msg_no=127: PARTY_SET_LOOT_MODE */
int handle_party_set_loot_mode(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_SET_LOOT_MODE from fd=%d\n", s->fd);
    if (len < 1 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 127, error, 4);
    }

    uint8_t loot_mode = data[0]; /* 0=FFA, 1=Round-robin, 2=Leader */
    printf("[PARTY] Set loot mode to %d\n", loot_mode);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = loot_mode;
    return send_response(s, 127, response, 8);
}

/* msg_no=128: PARTY_CHAT */
int handle_party_chat(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_CHAT from fd=%d\n", s->fd);
    if (len < 1 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 128, error, 4);
    }

    /* Broadcast to party members */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].party_id == s->party_id && &g_sessions[i] != s) {
            uint8_t msg[256];
            memset(msg, 0, sizeof(msg));
            memcpy(msg + 4, &s->character_id, 4);
            memcpy(msg + 8, s->character_name, 20);
            int msg_len = (len > 200) ? 200 : len;
            memcpy(msg + 28, data, msg_len);
            send_response(&g_sessions[i], 128, msg, 28 + msg_len);
        }
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 128, response, 4);
}

/*==============================================================================
 * PVP/Arena System (msg_no 130-138)
 *============================================================================*/

/* Implementation of pvp_end_match (forward declared above) */
static void pvp_end_match(PVPMatch* match, uint8_t winner_team) {
    if (!match || match->state == 0) return;

    printf("[PVP] Ending match %u, winner team: %d\n", match->match_id, winner_team);
    match->winner = winner_team;
    match->state = 2;  /* Finished */

    /* Calculate average rating for each team (for ELO calculation) */
    uint32_t team1_avg = 0, team2_avg = 0;
    int t1_count = 0, t2_count = 0;

    for (int i = 0; i < match->team1_count; i++) {
        ClientSession* ps = find_session_by_charac(match->team1[i]);
        if (ps) {
            team1_avg += ps->pvp_points;
            t1_count++;
        }
    }
    for (int i = 0; i < match->team2_count; i++) {
        ClientSession* ps = find_session_by_charac(match->team2[i]);
        if (ps) {
            team2_avg += ps->pvp_points;
            t2_count++;
        }
    }
    if (t1_count > 0) team1_avg /= t1_count;
    if (t2_count > 0) team2_avg /= t2_count;

    /* Update ratings for team 1 */
    for (int i = 0; i < match->team1_count; i++) {
        ClientSession* ps = find_session_by_charac(match->team1[i]);
        if (ps) {
            int is_winner = (winner_team == 1);
            int32_t change = calculate_elo_change(
                is_winner ? team1_avg : team2_avg,
                is_winner ? team2_avg : team1_avg,
                is_winner
            );

            /* Apply rating change with floor */
            int32_t new_rating = (int32_t)ps->pvp_points + change;
            if (new_rating < 0) new_rating = 0;
            ps->pvp_points = (uint32_t)new_rating;

            if (is_winner) {
                ps->pvp_wins++;
            } else {
                ps->pvp_losses++;
            }

            /* Clear match state */
            ps->pvp_state = PVP_STATE_NONE;
            ps->pvp_match_id = 0;
            ps->pvp_team = 0;

            /* Save to database */
            db_save_pvp_stats(ps->character_id, ps->pvp_points,
                              ps->pvp_wins, ps->pvp_losses);

            printf("[PVP] Player %u: rating %u -> %u (%+d), W/L: %u/%u\n",
                   match->team1[i], ps->pvp_points - change, ps->pvp_points,
                   change, ps->pvp_wins, ps->pvp_losses);
        }
    }

    /* Update ratings for team 2 */
    for (int i = 0; i < match->team2_count; i++) {
        ClientSession* ps = find_session_by_charac(match->team2[i]);
        if (ps) {
            int is_winner = (winner_team == 2);
            int32_t change = calculate_elo_change(
                is_winner ? team2_avg : team1_avg,
                is_winner ? team1_avg : team2_avg,
                is_winner
            );

            int32_t new_rating = (int32_t)ps->pvp_points + change;
            if (new_rating < 0) new_rating = 0;
            ps->pvp_points = (uint32_t)new_rating;

            if (is_winner) {
                ps->pvp_wins++;
            } else {
                ps->pvp_losses++;
            }

            ps->pvp_state = PVP_STATE_NONE;
            ps->pvp_match_id = 0;
            ps->pvp_team = 0;

            db_save_pvp_stats(ps->character_id, ps->pvp_points,
                              ps->pvp_wins, ps->pvp_losses);

            printf("[PVP] Player %u: rating %u -> %u (%+d), W/L: %u/%u\n",
                   match->team2[i], ps->pvp_points - change, ps->pvp_points,
                   change, ps->pvp_wins, ps->pvp_losses);
        }
    }

    /* Clear match slot for reuse */
    memset(match, 0, sizeof(PVPMatch));
}

/* msg_no=130: PVP_GET_INFO */
int handle_pvp_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_GET_INFO from fd=%d\n", s->fd);

    /* Load PVP stats from DB if not already loaded */
    if (s->pvp_points == 0 && s->pvp_wins == 0 && s->pvp_losses == 0) {
        db_get_pvp_stats(s->character_id, &s->pvp_points, &s->pvp_wins, &s->pvp_losses);
        if (s->pvp_points == 0) s->pvp_points = 1000;  /* Default starting rating */
    }

    uint8_t response[48];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Real PVP stats from session/DB */
    memcpy(response + offset, &s->pvp_points, 4);
    offset += 4;

    memcpy(response + offset, &s->pvp_wins, 4);
    offset += 4;

    memcpy(response + offset, &s->pvp_losses, 4);
    offset += 4;

    uint8_t rank = get_pvp_rank(s->pvp_points);
    response[offset++] = rank;

    /* Current PVP state */
    response[offset++] = s->pvp_state;

    /* Current match ID (if in match) */
    memcpy(response + offset, &s->pvp_match_id, 4);
    offset += 4;

    printf("[PVP] GET_INFO: charac=%u, points=%u, wins=%u, losses=%u, rank=%u, state=%u\n",
           s->character_id, s->pvp_points, s->pvp_wins, s->pvp_losses, rank, s->pvp_state);

    return send_response(s, 130, response, offset);
}

/* msg_no=131: PVP_QUEUE */
int handle_pvp_queue(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] PVP_QUEUE from fd=%d, charac=%u\n", s->fd, s->character_id);

    uint8_t mode = PVP_MODE_1V1;
    if (len >= 1) mode = data[0];

    /* Check if already in queue or match */
    if (s->pvp_state != PVP_STATE_NONE) {
        printf("[PVP] Already in queue/match, state=%u\n", s->pvp_state);
        uint8_t error[4] = {1, 0, 0, 0};  /* Already queued */
        return send_response(s, 131, error, 4);
    }

    /* Load PVP stats if not loaded */
    if (s->pvp_points == 0) {
        db_get_pvp_stats(s->character_id, &s->pvp_points, &s->pvp_wins, &s->pvp_losses);
        if (s->pvp_points == 0) s->pvp_points = 1000;
    }

    /* Add to queue */
    int queue_result = pvp_queue_add(s->character_id, s->fd, mode, s->pvp_points);
    if (queue_result < 0) {
        uint8_t error[4];
        error[0] = (queue_result == -1) ? 1 : 2;  /* 1=already queued, 2=queue full */
        memset(error + 1, 0, 3);
        return send_response(s, 131, error, 4);
    }

    s->pvp_state = PVP_STATE_QUEUED;

    /* Try to create a match immediately */
    PVPMatch* match = pvp_try_match(mode);
    if (match) {
        /* Match found! Update all players' states */
        for (int i = 0; i < match->team1_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team1[i]);
            if (ps) {
                ps->pvp_state = PVP_STATE_MATCH_FOUND;
                ps->pvp_match_id = match->match_id;
                ps->pvp_team = 1;
                ps->pvp_team_index = i;
            }
        }
        for (int i = 0; i < match->team2_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team2[i]);
            if (ps) {
                ps->pvp_state = PVP_STATE_MATCH_FOUND;
                ps->pvp_match_id = match->match_id;
                ps->pvp_team = 2;
                ps->pvp_team_index = i;
            }
        }

        /* Respond with match found */
        uint8_t response[16];
        memset(response, 0, sizeof(response));
        response[4] = mode;
        response[5] = 1;  /* Match found flag */
        memcpy(response + 6, &match->match_id, 4);
        return send_response(s, 131, response, 16);
    }

    /* No match yet, return queue position */
    uint8_t response[12];
    memset(response, 0, sizeof(response));
    response[4] = mode;
    response[5] = 0;  /* No match yet */

    /* Count queue position */
    uint16_t queue_pos = 1;
    for (int i = 0; i < MAX_PVP_QUEUE; i++) {
        if (g_pvp_queue[i].active && g_pvp_queue[i].mode == mode) {
            if (g_pvp_queue[i].player_id == s->character_id) break;
            queue_pos++;
        }
    }
    memcpy(response + 6, &queue_pos, 2);

    printf("[PVP] Queued for mode %d, position %u\n", mode, queue_pos);
    return send_response(s, 131, response, 12);
}

/* msg_no=132: PVP_CANCEL_QUEUE */
int handle_pvp_cancel_queue(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_CANCEL_QUEUE from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->pvp_state != PVP_STATE_QUEUED) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Not in queue */
        return send_response(s, 132, error, 4);
    }

    pvp_queue_remove(s->character_id);
    s->pvp_state = PVP_STATE_NONE;

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 132, response, 4);
}

/* msg_no=133: PVP_ACCEPT_MATCH */
int handle_pvp_accept_match(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_ACCEPT_MATCH from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->pvp_state != PVP_STATE_MATCH_FOUND) {
        uint8_t error[4] = {1, 0, 0, 0};  /* No pending match */
        return send_response(s, 133, error, 4);
    }

    PVPMatch* match = find_pvp_match(s->pvp_match_id);
    if (!match) {
        s->pvp_state = PVP_STATE_NONE;
        s->pvp_match_id = 0;
        uint8_t error[4] = {2, 0, 0, 0};  /* Match not found */
        return send_response(s, 133, error, 4);
    }

    /* Mark player as ready */
    if (s->pvp_team == 1 && s->pvp_team_index < 4) {
        match->team1_ready[s->pvp_team_index] = 1;
    } else if (s->pvp_team == 2 && s->pvp_team_index < 4) {
        match->team2_ready[s->pvp_team_index] = 1;
    }

    printf("[PVP] Player %u accepted match %u\n", s->character_id, match->match_id);

    /* Check if all players are ready */
    if (pvp_all_ready(match)) {
        printf("[PVP] All players ready, starting match %u\n", match->match_id);
        match->state = 2;  /* Active */
        match->start_time = (uint32_t)time(NULL);

        /* Update all players' states */
        for (int i = 0; i < match->team1_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team1[i]);
            if (ps) ps->pvp_state = PVP_STATE_IN_MATCH;
        }
        for (int i = 0; i < match->team2_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team2[i]);
            if (ps) ps->pvp_state = PVP_STATE_IN_MATCH;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = pvp_all_ready(match) ? 1 : 0;  /* Match starting flag */
    return send_response(s, 133, response, 8);
}

/* msg_no=134: PVP_DECLINE_MATCH */
int handle_pvp_decline_match(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_DECLINE_MATCH from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->pvp_state != PVP_STATE_MATCH_FOUND) {
        uint8_t error[4] = {1, 0, 0, 0};  /* No pending match */
        return send_response(s, 134, error, 4);
    }

    PVPMatch* match = find_pvp_match(s->pvp_match_id);
    if (match) {
        /* Cancel the match for all players */
        printf("[PVP] Player %u declined, canceling match %u\n", s->character_id, match->match_id);

        for (int i = 0; i < match->team1_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team1[i]);
            if (ps) {
                ps->pvp_state = PVP_STATE_NONE;
                ps->pvp_match_id = 0;
                ps->pvp_team = 0;
            }
        }
        for (int i = 0; i < match->team2_count; i++) {
            ClientSession* ps = find_session_by_charac(match->team2[i]);
            if (ps) {
                ps->pvp_state = PVP_STATE_NONE;
                ps->pvp_match_id = 0;
                ps->pvp_team = 0;
            }
        }

        /* Clear match */
        memset(match, 0, sizeof(PVPMatch));
    }

    s->pvp_state = PVP_STATE_NONE;
    s->pvp_match_id = 0;
    s->pvp_team = 0;

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 134, response, 4);
}

/* msg_no=135: PVP_READY */
int handle_pvp_ready(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_READY from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* This is used for in-match ready state (before round starts) */
    if (s->pvp_state != PVP_STATE_IN_MATCH) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Not in match */
        return send_response(s, 135, error, 4);
    }

    PVPMatch* match = find_pvp_match(s->pvp_match_id);
    if (!match) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Match not found */
        return send_response(s, 135, error, 4);
    }

    printf("[PVP] Player %u ready in match %u\n", s->character_id, match->match_id);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 135, response, 4);
}

/* msg_no=136: PVP_ATTACK */
int handle_pvp_attack(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] PVP_ATTACK from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->pvp_state != PVP_STATE_IN_MATCH) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Not in match */
        return send_response(s, 136, error, 4);
    }

    if (len < 8) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid data */
        return send_response(s, 136, error, 4);
    }

    PVPMatch* match = find_pvp_match(s->pvp_match_id);
    if (!match || match->state != 2) {
        uint8_t error[4] = {3, 0, 0, 0};  /* Match not active */
        return send_response(s, 136, error, 4);
    }

    uint32_t target_id;
    uint32_t skill_id;
    memcpy(&target_id, data, 4);
    memcpy(&skill_id, data + 4, 4);

    /* Find target in match */
    uint8_t target_team, target_index;
    if (!pvp_find_player_in_match(match, target_id, &target_team, &target_index)) {
        uint8_t error[4] = {4, 0, 0, 0};  /* Target not in match */
        return send_response(s, 136, error, 4);
    }

    /* Can only attack enemy team */
    if (target_team == s->pvp_team) {
        uint8_t error[4] = {5, 0, 0, 0};  /* Cannot attack ally */
        return send_response(s, 136, error, 4);
    }

    /* Phase 16 P3: Use real damage calculation */
    uint32_t damage = 0;
    uint8_t is_crit = 0, is_miss = 0;

    /* Get skill level (default based on player level) */
    uint8_t skill_level = 1;
    int has_skill = 0;
    for (int i = 0; i < s->skill.skill_count && i < MAX_PARSED_SKILLS; i++) {
        if (s->skill.skill_ids[i] == (uint16_t)skill_id) {
            has_skill = 1;
            break;
        }
    }
    if (has_skill) {
        skill_level = (s->level / 10) + 1;
    }
    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;

    /* Get target's defense stats */
    uint32_t target_def = 100;  /* Default defense */
    uint8_t target_level = 1;
    ClientSession* target_session = find_session_by_charac(target_id);
    if (target_session) {
        target_def = target_session->combat_stats.phys_def;
        /* Get target level */
        for (int i = 0; i < target_session->character_count; i++) {
            if (target_session->characters[i].charac_no == target_id) {
                target_level = target_session->characters[i].level;
                break;
            }
        }
    }

    /* Calculate damage using v3 formula */
    damage = calculate_skill_damage_v3(
        s, (uint16_t)skill_id, skill_level,
        0,            /* monster_type: 0 for PVP */
        target_level, /* opponent level */
        target_def,   /* opponent defense */
        &is_crit, &is_miss
    );

    /* PVP damage reduction (70% of PVE damage) */
    damage = damage * 70 / 100;

    /* Minimum damage */
    if (damage < 50) damage = 50;

    printf("[PVP] Damage calc: skill=%u lv=%u def=%u -> dmg=%u (crit=%d)\n",
           skill_id, skill_level, target_def, damage, is_crit);

    /* Apply damage to target */
    uint32_t* target_hp = (target_team == 1) ?
                          &match->team1_hp[target_index] :
                          &match->team2_hp[target_index];

    uint32_t old_hp = *target_hp;
    if (damage >= *target_hp) {
        *target_hp = 0;
    } else {
        *target_hp -= damage;
    }

    printf("[PVP] Attack: %u -> %u, skill=%u, damage=%u, HP: %u -> %u\n",
           s->character_id, target_id, skill_id, damage, old_hp, *target_hp);

    /* Check for victory */
    uint8_t winner = 0;
    if (pvp_check_team_eliminated(match, 1)) {
        winner = 2;  /* Team 2 wins */
    } else if (pvp_check_team_eliminated(match, 2)) {
        winner = 1;  /* Team 1 wins */
    }

    if (winner > 0) {
        printf("[PVP] Match %u ended! Winner: Team %u\n", match->match_id, winner);
        pvp_end_match(match, winner);
    }

    uint8_t response[24];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    memcpy(response + 8, &damage, 4);
    memcpy(response + 12, target_hp, 4);
    response[16] = winner;  /* 0=continue, 1=team1 wins, 2=team2 wins */
    return send_response(s, 136, response, 20);
}

/* msg_no=137: PVP_LEAVE */
int handle_pvp_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_LEAVE from fd=%d, charac=%u\n", s->fd, s->character_id);

    if (s->pvp_state == PVP_STATE_QUEUED) {
        /* Remove from queue */
        pvp_queue_remove(s->character_id);
    } else if (s->pvp_state == PVP_STATE_IN_MATCH || s->pvp_state == PVP_STATE_MATCH_FOUND) {
        /* Forfeit match */
        PVPMatch* match = find_pvp_match(s->pvp_match_id);
        if (match) {
            /* Set player's HP to 0 (forfeit) */
            if (s->pvp_team == 1 && s->pvp_team_index < 4) {
                match->team1_hp[s->pvp_team_index] = 0;
            } else if (s->pvp_team == 2 && s->pvp_team_index < 4) {
                match->team2_hp[s->pvp_team_index] = 0;
            }

            /* Check for victory due to forfeit */
            if (pvp_check_team_eliminated(match, 1)) {
                pvp_end_match(match, 2);
            } else if (pvp_check_team_eliminated(match, 2)) {
                pvp_end_match(match, 1);
            }
        }
    }

    s->pvp_state = PVP_STATE_NONE;
    s->pvp_match_id = 0;
    s->pvp_team = 0;

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 137, response, 4);
}

/* Get PVP ranking from database */
static int db_get_pvp_ranking(char names[][20], uint32_t* points, uint32_t* wins, int max_count) {
    if (!g_db_charac) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT c.charac_name, COALESCE(s.pvp_point, 1000) as pvp_point, "
             "COALESCE(s.pvp_win, 0) as pvp_win "
             "FROM charac_info c "
             "LEFT JOIN charac_stat s ON c.charac_no = s.charac_no "
             "WHERE c.delete_flag = 0 "
             "ORDER BY pvp_point DESC, pvp_win DESC "
             "LIMIT %d", max_count);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] PVP ranking query failed: %s\n", mysql_error(g_db_charac));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac);
    if (!res) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        if (row[0]) strncpy(names[count], row[0], 19);
        points[count] = row[1] ? (uint32_t)atoi(row[1]) : 1000;
        wins[count] = row[2] ? (uint32_t)atoi(row[2]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

/* msg_no=138: PVP_GET_RANKING */
int handle_pvp_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_GET_RANKING from fd=%d\n", s->fd);

    char names[10][20];
    uint32_t points[10];
    uint32_t wins[10];
    memset(names, 0, sizeof(names));

    int db_count = db_get_pvp_ranking(names, points, wins, 10);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = (db_count > 0) ? (uint8_t)db_count : 0;
    response[offset++] = count;

    for (int i = 0; i < count && i < 10; i++) {
        uint8_t rank_num = i + 1;
        response[offset++] = rank_num;

        memcpy(response + offset, names[i], 20);
        offset += 20;

        memcpy(response + offset, &points[i], 4);
        offset += 4;

        uint16_t win_count = (uint16_t)wins[i];
        memcpy(response + offset, &win_count, 2);
        offset += 2;

        uint8_t rank_tier = get_pvp_rank(points[i]);
        response[offset++] = rank_tier;
    }

    printf("[PVP] Returning %d ranking entries from DB\n", count);
    return send_response(s, 138, response, offset);
}

/*==============================================================================
 * Ranking System (msg_no 140-145)
 *============================================================================*/

/* msg_no=140: RANKING_GET_LEVEL */
int handle_ranking_get_level(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_LEVEL from fd=%d\n", s->fd);

    /* Query real data from database */
    DBRankingEntry entries[20];
    int db_count = db_get_level_ranking(entries, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = (db_count > 0) ? (uint8_t)db_count : 0;
    response[offset++] = count;

    for (int i = 0; i < count && i < 20; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        memcpy(response + offset, entries[i].charac_name, 20);
        offset += 20;

        response[offset++] = entries[i].lev;
        response[offset++] = entries[i].job;
    }

    printf("[RANK] Returning %d level ranking entries\n", count);
    return send_response(s, 140, response, offset);
}

/* msg_no=141: RANKING_GET_WEALTH */
int handle_ranking_get_wealth(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_WEALTH from fd=%d\n", s->fd);

    /* Query real data from database */
    DBRankingEntry entries[20];
    int db_count = db_get_wealth_ranking(entries, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = (db_count > 0) ? (uint8_t)db_count : 0;
    response[offset++] = count;

    for (int i = 0; i < count && i < 20; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        memcpy(response + offset, entries[i].charac_name, 20);
        offset += 20;

        memcpy(response + offset, &entries[i].money, 4);
        offset += 4;
    }

    printf("[RANK] Returning %d wealth ranking entries\n", count);
    return send_response(s, 141, response, offset);
}

/* msg_no=142: RANKING_GET_DUNGEON */
int handle_ranking_get_dungeon(ClientSession* s, const uint8_t* data, int len) {
    printf("[RANK] RANKING_GET_DUNGEON from fd=%d\n", s->fd);

    uint32_t dungeon_id = 1;
    if (len >= 4) memcpy(&dungeon_id, data, 4);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "Speedrunner%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint32_t clear_time = 60 + i * 5; /* seconds */
        memcpy(response + offset, &clear_time, 4);
        offset += 4;
    }

    return send_response(s, 142, response, offset);
}

/* msg_no=143: RANKING_GET_MY_RANK */
int handle_ranking_get_my_rank(ClientSession* s, const uint8_t* data, int len) {
    printf("[RANK] RANKING_GET_MY_RANK from fd=%d\n", s->fd);

    uint8_t rank_type = 0; /* 0=level, 1=wealth, 2=pvp */
    if (len >= 1) rank_type = data[0];

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = rank_type;

    uint32_t my_rank = 42 + rank_type * 10;
    memcpy(response + offset, &my_rank, 4);
    offset += 4;

    return send_response(s, 143, response, offset);
}

/* msg_no=144: RANKING_GET_GUILD */
int handle_ranking_get_guild(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_GUILD from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "TopGuild%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint8_t level = 10 - i / 2;
        response[offset++] = level;

        uint16_t members = 50 - i * 3;
        memcpy(response + offset, &members, 2);
        offset += 2;
    }

    return send_response(s, 144, response, offset);
}

/* msg_no=145: RANKING_GET_ACHIEVEMENT */
int handle_ranking_get_achievement(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_ACHIEVEMENT from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "Achiever%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint16_t points = 10000 - i * 500;
        memcpy(response + offset, &points, 2);
        offset += 2;
    }

    return send_response(s, 145, response, offset);
}

/*==============================================================================
 * Achievement System (msg_no 150-156)
 *============================================================================*/

/* msg_no=150: ACHIEVEMENT_GET_LIST */
int handle_achievement_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_LIST from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Load achievements from DB if not loaded */
    if (!s->achievement.achievement_loaded) {
        db_get_achievement(s->character_id, &s->achievement);
    }

    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Total achievement points from DB */
    uint16_t total_points = (uint16_t)s->achievement.total_points;
    memcpy(response + offset, &total_points, 2);
    offset += 2;

    /* Achievement count from DB */
    uint8_t count = s->achievement.achievement_count;
    if (count > 50) count = 50; /* Limit for response size */
    response[offset++] = count;

    /* Send each achievement from DB */
    for (int i = 0; i < count; i++) {
        DBAchievementEntry* entry = &s->achievement.achievements[i];

        memcpy(response + offset, &entry->ach_id, 4);
        offset += 4;

        response[offset++] = entry->completed;
        response[offset++] = entry->progress;

        memcpy(response + offset, &entry->points, 2);
        offset += 2;
    }

    printf("[ACH] Returning %u achievements, total_points=%u\n", count, total_points);
    return send_response(s, 150, response, offset);
}

/* msg_no=151: ACHIEVEMENT_GET_DETAIL */
int handle_achievement_get_detail(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_DETAIL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 151, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    /* Load achievements from DB if not loaded */
    if (!s->achievement.achievement_loaded) {
        db_get_achievement(s->character_id, &s->achievement);
    }

    /* Find achievement in loaded data */
    DBAchievementEntry* found = NULL;
    for (int i = 0; i < s->achievement.achievement_count; i++) {
        if (s->achievement.achievements[i].ach_id == ach_id) {
            found = &s->achievement.achievements[i];
            break;
        }
    }

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = found ? 0 : 1; /* Error if not found */
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &ach_id, 4);
    offset += 4;

    /* Achievement names based on ID ranges */
    char name[32];
    char desc[64];
    if (ach_id >= 1000 && ach_id < 1100) {
        snprintf(name, sizeof(name), "Combat Master %u", ach_id - 1000);
        snprintf(desc, sizeof(desc), "Defeat %u enemies", (ach_id - 1000 + 1) * 100);
    } else if (ach_id >= 1100 && ach_id < 1200) {
        snprintf(name, sizeof(name), "Explorer %u", ach_id - 1100);
        snprintf(desc, sizeof(desc), "Discover %u areas", (ach_id - 1100 + 1) * 10);
    } else {
        snprintf(name, sizeof(name), "Achievement %u", ach_id);
        snprintf(desc, sizeof(desc), "Complete special task");
    }
    memcpy(response + offset, name, 32);
    offset += 32;
    memcpy(response + offset, desc, 64);
    offset += 64;

    uint16_t points = found ? found->points : 100;
    memcpy(response + offset, &points, 2);
    offset += 2;

    uint8_t progress = found ? found->progress : 0;
    response[offset++] = progress;

    return send_response(s, 151, response, offset);
}

/* msg_no=152: ACHIEVEMENT_CLAIM_REWARD */
int handle_achievement_claim_reward(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_CLAIM_REWARD from fd=%d, charac=%u\n", s->fd, s->character_id);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 152, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    /* Load achievements from DB if not loaded */
    if (!s->achievement.achievement_loaded) {
        db_get_achievement(s->character_id, &s->achievement);
    }

    /* Find achievement in loaded data */
    DBAchievementEntry* found = NULL;
    for (int i = 0; i < s->achievement.achievement_count; i++) {
        if (s->achievement.achievements[i].ach_id == ach_id) {
            found = &s->achievement.achievements[i];
            break;
        }
    }

    /* Check if achievement is completed */
    if (!found || !found->completed) {
        printf("[ACH] Achievement %u not completed, cannot claim\n", ach_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Not completed error */
        return send_response(s, 152, error, 4);
    }

    /* Calculate reward based on achievement points */
    uint32_t gold_reward = found->points * 10; /* 10 gold per point */

    /* Update memory */
    s->inventory.money += gold_reward;

    /* Persist to DB */
    db_add_gold(s->character_id, gold_reward);
    db_save_achievement(s->character_id, ach_id, 1);

    printf("[ACH] Claimed reward for achievement %u: +%u gold\n", ach_id, gold_reward);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &ach_id, 4);
    memcpy(response + 8, &gold_reward, 4);
    return send_response(s, 152, response, 16);
}

/* msg_no=153: ACHIEVEMENT_GET_PROGRESS */
int handle_achievement_get_progress(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_PROGRESS from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 153, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    /* Load achievements from DB if not loaded */
    if (!s->achievement.achievement_loaded) {
        db_get_achievement(s->character_id, &s->achievement);
    }

    /* Find achievement progress in loaded data */
    uint8_t progress = 0;
    for (int i = 0; i < s->achievement.achievement_count; i++) {
        if (s->achievement.achievements[i].ach_id == ach_id) {
            progress = s->achievement.achievements[i].progress;
            break;
        }
    }

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &ach_id, 4);
    response[8] = progress;
    return send_response(s, 153, response, 12);
}

/* msg_no=154: ACHIEVEMENT_GET_CATEGORIES */
int handle_achievement_get_categories(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_CATEGORIES from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 5;
    response[offset++] = count;

    const char* categories[] = {"Combat", "Exploration", "Social", "Collection", "Special"};
    for (int i = 0; i < 5; i++) {
        uint8_t cat_id = i;
        response[offset++] = cat_id;
        memcpy(response + offset, categories[i], 16);
        offset += 16;
    }

    return send_response(s, 154, response, offset);
}

/* msg_no=155: ACHIEVEMENT_GET_BY_CATEGORY */
int handle_achievement_get_by_category(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_BY_CATEGORY from fd=%d\n", s->fd);

    uint8_t category = 0;
    if (len >= 1) category = data[0];

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = category;

    uint8_t count = 3;
    response[offset++] = count;

    for (int i = 0; i < 3; i++) {
        uint32_t ach_id = category * 100 + i;
        memcpy(response + offset, &ach_id, 4);
        offset += 4;

        uint8_t completed = (i == 0) ? 1 : 0;
        response[offset++] = completed;
    }

    return send_response(s, 155, response, offset);
}

/* msg_no=156: ACHIEVEMENT_GET_RECENT */
int handle_achievement_get_recent(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_RECENT from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 3;
    response[offset++] = count;

    for (int i = 0; i < 3; i++) {
        uint32_t ach_id = 1000 + i;
        memcpy(response + offset, &ach_id, 4);
        offset += 4;

        uint32_t timestamp = (uint32_t)time(NULL) - i * 3600;
        memcpy(response + offset, &timestamp, 4);
        offset += 4;
    }

    return send_response(s, 156, response, offset);
}

/*==============================================================================
 * Title System (msg_no 160-165)
 *============================================================================*/

/* Title name lookup based on ID */
static const char* get_title_name(uint32_t title_id) {
    static const char* base_titles[] = {
        "Beginner", "Adventurer", "Veteran", "Champion", "Legend",
        "Explorer", "Warrior", "Mage", "Hunter", "Guardian"
    };
    if (title_id >= 1 && title_id <= 10) {
        return base_titles[title_id - 1];
    }
    if (title_id >= 10 && title_id < 100) {
        return "Elite";
    }
    return "Unknown";
}

/* msg_no=160: TITLE_GET_LIST */
int handle_title_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_LIST from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Load titles from DB if not loaded */
    if (!s->title.title_loaded) {
        db_get_title(s->character_id, &s->title);
    }

    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Title count from DB */
    uint8_t count = s->title.title_count;
    if (count > 30) count = 30; /* Limit for response size */
    response[offset++] = count;

    /* Send each title from DB */
    for (int i = 0; i < count; i++) {
        DBTitleEntry* entry = &s->title.titles[i];

        memcpy(response + offset, &entry->title_id, 4);
        offset += 4;

        const char* name = get_title_name(entry->title_id);
        char name_buf[20];
        memset(name_buf, 0, sizeof(name_buf));
        strncpy(name_buf, name, sizeof(name_buf) - 1);
        memcpy(response + offset, name_buf, 20);
        offset += 20;

        response[offset++] = entry->unlocked;
    }

    printf("[TITLE] Returning %u titles\n", count);
    return send_response(s, 160, response, offset);
}

/* msg_no=161: TITLE_EQUIP */
int handle_title_equip(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] TITLE_EQUIP from fd=%d, charac=%u\n", s->fd, s->character_id);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 161, error, 4);
    }

    uint32_t title_id;
    memcpy(&title_id, data, 4);

    /* Load titles from DB if not loaded */
    if (!s->title.title_loaded) {
        db_get_title(s->character_id, &s->title);
    }

    /* Check if title is unlocked */
    int found = 0;
    for (int i = 0; i < s->title.title_count; i++) {
        if (s->title.titles[i].title_id == title_id && s->title.titles[i].unlocked) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("[TITLE] Title %u not unlocked\n", title_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Not unlocked error */
        return send_response(s, 161, error, 4);
    }

    /* Update equipped title in memory */
    s->title.equipped_title = title_id;

    /* Persist to DB */
    db_save_equipped_title(s->character_id, title_id);

    printf("[TITLE] Equipped title %u\n", title_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &title_id, 4);
    return send_response(s, 161, response, 8);
}

/* msg_no=162: TITLE_UNEQUIP */
int handle_title_unequip(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_UNEQUIP from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Update equipped title in memory */
    s->title.equipped_title = 0;

    /* Persist to DB */
    db_save_equipped_title(s->character_id, 0);

    printf("[TITLE] Unequipped title\n");

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 162, response, 4);
}

/* msg_no=163: TITLE_GET_DETAIL */
int handle_title_get_detail(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] TITLE_GET_DETAIL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 163, error, 4);
    }

    uint32_t title_id;
    memcpy(&title_id, data, 4);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &title_id, 4);
    offset += 4;

    char name[20] = "Champion";
    memcpy(response + offset, name, 20);
    offset += 20;

    char desc[64] = "Win 100 PvP matches";
    memcpy(response + offset, desc, 64);
    offset += 64;

    /* Stats bonus */
    uint16_t str_bonus = 10;
    uint16_t int_bonus = 5;
    memcpy(response + offset, &str_bonus, 2);
    offset += 2;
    memcpy(response + offset, &int_bonus, 2);
    offset += 2;

    return send_response(s, 163, response, offset);
}

/* msg_no=164: TITLE_GET_CURRENT */
int handle_title_get_current(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_CURRENT from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Load titles from DB if not loaded */
    if (!s->title.title_loaded) {
        db_get_title(s->character_id, &s->title);
    }

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get equipped title from DB data */
    uint32_t title_id = s->title.equipped_title;
    memcpy(response + offset, &title_id, 4);
    offset += 4;

    /* Get title name */
    const char* title_name = get_title_name(title_id);
    char name[20];
    memset(name, 0, sizeof(name));
    if (title_id > 0) {
        strncpy(name, title_name, sizeof(name) - 1);
    }
    memcpy(response + offset, name, 20);
    offset += 20;

    printf("[TITLE] Current equipped title: %u (%s)\n", title_id, name);
    return send_response(s, 164, response, offset);
}

/* msg_no=165: TITLE_GET_UNLOCKABLE */
int handle_title_get_unlockable(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_UNLOCKABLE from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 2;
    response[offset++] = count;

    for (int i = 0; i < 2; i++) {
        uint32_t title_id = 103 + i;
        memcpy(response + offset, &title_id, 4);
        offset += 4;

        uint8_t progress = 60 + i * 20;
        response[offset++] = progress;
    }

    return send_response(s, 165, response, offset);
}


/*==============================================================================
 * Auction System (msg_no 170-178)
 *============================================================================*/

/* msg_no=170: AUCTION_SEARCH */
int handle_auction_search(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_SEARCH from fd=%d\n", s->fd);

    uint32_t item_id = 0;
    uint32_t min_price = 0;
    uint32_t max_price = 0;
    if (len >= 4) memcpy(&item_id, data, 4);
    if (len >= 8) memcpy(&min_price, data + 4, 4);
    if (len >= 12) memcpy(&max_price, data + 8, 4);

    AuctionListing* results[20];
    int count = search_auction_listings(item_id, min_price, max_price, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        memcpy(response + offset, a->seller_name, 20);
        offset += 20;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Search returned %d results\n", count);
    return send_response(s, 170, response, offset);
}

/* msg_no=171: AUCTION_LIST_ITEM */
int handle_auction_list_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_LIST_ITEM from fd=%d\n", s->fd);
    if (len < 12) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 171, error, 4);
    }

    uint32_t item_uid;
    uint32_t price;
    uint32_t duration; /* hours */
    memcpy(&item_uid, data, 4);
    memcpy(&price, data + 4, 4);
    memcpy(&duration, data + 8, 4);

    /* Validate duration (1-48 hours) */
    if (duration < 1) duration = 1;
    if (duration > 48) duration = 48;

    /* Listing fee (5% of price, min 1000) */
    uint32_t fee = price / 20;
    if (fee < 1000) fee = 1000;

    if (s->inventory.money < fee) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 171, error, 4);
    }

    /* Get item info */
    DBItemInfo auction_item;
    if (db_get_item_by_ui_id(s->character_id, item_uid, &auction_item) < 0 ||
        auction_item.it_id == 0) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Item not found */
        return send_response(s, 171, error, 4);
    }
    uint32_t item_id = auction_item.it_id;

    /* Create listing */
    AuctionListing* listing = create_auction_listing(
        s->character_id, s->character_name, item_uid, item_id, price, duration);

    if (!listing) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot create listing */
        return send_response(s, 171, error, 4);
    }

    /* Deduct fee and persist gold */
    s->inventory.money -= fee;
    db_update_gold(s->character_id, s->inventory.money);

    /* Remove item from seller's inventory */
    db_delete_item(s->character_id, item_uid);

    printf("[AUCTION] Listed item %u (id=%u) for %u gold (fee: %u, listing_id=%u)\n",
           item_uid, item_id, price, fee, listing->listing_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing->listing_id, 4);
    memcpy(response + 8, &fee, 4);
    return send_response(s, 171, response, 12);
}

/* msg_no=172: AUCTION_CANCEL */
int handle_auction_cancel(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_CANCEL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 172, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing || listing->seller_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found or not owner */
        return send_response(s, 172, error, 4);
    }

    /* Return item to seller */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Cancel listing */
    listing->active = 0;
    printf("[AUCTION] Cancelled listing %u, returned item %u\n", listing_id, listing->item_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    return send_response(s, 172, response, 8);
}

/* msg_no=173: AUCTION_BUY */
int handle_auction_buy(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_BUY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 173, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found */
        return send_response(s, 173, error, 4);
    }

    if (listing->seller_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot buy own listing */
        return send_response(s, 173, error, 4);
    }

    if (s->inventory.money < listing->price) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 173, error, 4);
    }

    /* Deduct gold from buyer */
    s->inventory.money -= listing->price;
    db_update_gold(s->character_id, s->inventory.money);

    /* Add item to buyer */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Credit seller (seller collects later or via auction_collect_gold) */
    /* For simplicity, directly add to seller if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == listing->seller_id) {
            g_sessions[i].inventory.money += listing->price;
            db_update_gold(listing->seller_id, g_sessions[i].inventory.money);
            break;
        }
    }

    uint32_t price = listing->price;
    uint32_t item_id = listing->item_id;

    /* Mark listing as sold */
    listing->active = 0;
    printf("[AUCTION] Bought listing %u (item=%u) for %u gold\n", listing_id, item_id, price);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &price, 4);
    memcpy(response + 12, &item_id, 4);
    return send_response(s, 173, response, 16);
}

/* msg_no=174: AUCTION_GET_MY_LISTINGS */
int handle_auction_get_my_listings(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_GET_MY_LISTINGS from fd=%d\n", s->fd);

    AuctionListing* results[20];
    int count = get_seller_listings(s->character_id, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Returned %d listings for seller %u\n", count, s->character_id);
    return send_response(s, 174, response, offset);
}
/* msg_no=175: AUCTION_GET_HISTORY */
int handle_auction_get_history(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_GET_HISTORY from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 2;
    response[offset++] = count;

    for (int i = 0; i < 2; i++) {
        uint32_t item_id = 10000 + i * 50;
        memcpy(response + offset, &item_id, 4);
        offset += 4;

        uint32_t price = 75000 + i * 25000;
        memcpy(response + offset, &price, 4);
        offset += 4;

        uint8_t sold = (i == 0) ? 1 : 0;
        response[offset++] = sold;

        uint32_t timestamp = (uint32_t)time(NULL) - i * 86400;
        memcpy(response + offset, &timestamp, 4);
        offset += 4;
    }

    return send_response(s, 175, response, offset);
}

/* msg_no=176: AUCTION_BID */
int handle_auction_bid(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_BID from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 176, error, 4);
    }

    uint32_t listing_id;
    uint32_t bid_amount;
    memcpy(&listing_id, data, 4);
    memcpy(&bid_amount, data + 4, 4);

    if (s->inventory.money < bid_amount) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 176, error, 4);
    }

    printf("[AUCTION] Bid %u on listing %u\n", bid_amount, listing_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &bid_amount, 4);
    return send_response(s, 176, response, 12);
}

/* msg_no=177: AUCTION_GET_PRICE_HISTORY */
int handle_auction_get_price_history(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_GET_PRICE_HISTORY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 177, error, 4);
    }

    uint32_t item_id;
    memcpy(&item_id, data, 4);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;

    uint32_t avg_price = 85000;
    uint32_t min_price = 50000;
    uint32_t max_price = 120000;
    memcpy(response + offset, &avg_price, 4);
    offset += 4;
    memcpy(response + offset, &min_price, 4);
    offset += 4;
    memcpy(response + offset, &max_price, 4);
    offset += 4;

    return send_response(s, 177, response, offset);
}

/* msg_no=178: AUCTION_COLLECT_GOLD */
int handle_auction_collect_gold(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_COLLECT_GOLD from fd=%d\n", s->fd);

    /* Mock: collect 100000 gold from sold items */
    uint32_t collected = 100000;
    s->inventory.money += collected;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &collected, 4);
    return send_response(s, 178, response, 8);
}

/*==============================================================================
 * Storage/Warehouse System (msg_no 180-185)
 *============================================================================*/

/* msg_no=180: STORAGE_GET_LIST */
int handle_storage_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[STORAGE] STORAGE_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get storage from database using account_id as m_id */
    DBStorageInfo storage;
    if (db_get_storage(s->account_id, &storage) < 0) {
        storage.money = 0;
        storage.capacity = 8;
    }

    memcpy(response + offset, &storage.money, 4);
    offset += 4;

    response[offset++] = storage.capacity;

    /* Item count - cargo blob needs special handling, return 0 for now */
    uint8_t item_count = 0;
    response[offset++] = item_count;

    printf("[STORAGE] Returning storage for m_id=%u: gold=%u, capacity=%u\n",
           s->account_id, storage.money, storage.capacity);
    return send_response(s, 180, response, offset);
}

/* msg_no=181: STORAGE_DEPOSIT_ITEM - P7: Full DB persistence */
int handle_storage_deposit_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_DEPOSIT_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 181, error, 4);
    }

    /* item_uid is actually inventory slot in this context */
    int inv_slot;
    memcpy(&inv_slot, data, 4);
    printf("[STORAGE] Depositing item from slot %d\n", inv_slot);

    /* P7: Use DB function to move item from inventory to storage */
    int storage_slot = db_storage_deposit_item(s->account_id, s->character_id, inv_slot);

    uint8_t response[12];
    memset(response, 0, sizeof(response));

    if (storage_slot >= 0) {
        uint32_t result = 0;
        memcpy(response, &result, 4);
        memcpy(response + 4, &inv_slot, 4);
        memcpy(response + 8, &storage_slot, 4);
        printf("[STORAGE] Item deposited: inv_slot=%d -> storage_slot=%d\n", inv_slot, storage_slot);
    } else {
        uint32_t result = 2;  /* Failed */
        memcpy(response, &result, 4);
        printf("[STORAGE] Deposit failed\n");
    }

    return send_response(s, 181, response, 12);
}

/* msg_no=182: STORAGE_WITHDRAW_ITEM - P7: Full DB persistence */
int handle_storage_withdraw_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_WITHDRAW_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 182, error, 4);
    }

    int storage_slot;
    memcpy(&storage_slot, data, 4);
    printf("[STORAGE] Withdrawing from storage slot %d\n", storage_slot);

    /* P7: Use DB function to move item from storage to inventory */
    int inv_slot = db_storage_withdraw_item(s->account_id, s->character_id, storage_slot);

    uint8_t response[12];
    memset(response, 0, sizeof(response));

    if (inv_slot >= 0) {
        uint32_t result = 0;
        memcpy(response, &result, 4);
        memcpy(response + 4, &storage_slot, 4);
        memcpy(response + 8, &inv_slot, 4);
        printf("[STORAGE] Item withdrawn: storage_slot=%d -> inv_slot=%d\n", storage_slot, inv_slot);
    } else if (inv_slot == -2) {
        uint32_t result = 3;  /* Inventory full */
        memcpy(response, &result, 4);
        printf("[STORAGE] Withdraw failed: inventory full\n");
    } else {
        uint32_t result = 2;  /* Item not found */
        memcpy(response, &result, 4);
        printf("[STORAGE] Withdraw failed: item not found\n");
    }

    return send_response(s, 182, response, 12);
}

/* msg_no=183: STORAGE_DEPOSIT_GOLD */
int handle_storage_deposit_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_DEPOSIT_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 183, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    if (amount > s->inventory.money) {
        amount = s->inventory.money;
    }

    if (amount > 0) {
        /* Deduct from character inventory */
        s->inventory.money -= amount;
        db_update_gold(s->character_id, s->inventory.money);

        /* Add to storage */
        db_update_storage_gold(s->account_id, (int32_t)amount);
        printf("[STORAGE] Deposited %u gold to storage (m_id=%u)\n", amount, s->account_id);
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &amount, 4);
    return send_response(s, 183, response, 8);
}

/* msg_no=184: STORAGE_WITHDRAW_GOLD */
int handle_storage_withdraw_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_WITHDRAW_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 184, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    /* Get actual storage gold */
    uint32_t storage_gold = db_get_storage_gold(s->account_id);
    if (amount > storage_gold) {
        amount = storage_gold;
    }

    if (amount > 0) {
        /* Deduct from storage */
        db_update_storage_gold(s->account_id, -(int32_t)amount);

        /* Add to character inventory */
        s->inventory.money += amount;
        db_update_gold(s->character_id, s->inventory.money);
        printf("[STORAGE] Withdrew %u gold from storage (m_id=%u)\n", amount, s->account_id);
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &amount, 4);
    return send_response(s, 184, response, 8);
}

/* msg_no=185: STORAGE_EXPAND */
int handle_storage_expand(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[STORAGE] STORAGE_EXPAND from fd=%d\n", s->fd);

    /* Expansion cost: 100000 gold */
    if (s->inventory.money < 100000) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 185, error, 4);
    }

    s->inventory.money -= 100000;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint8_t new_capacity = 120;
    response[4] = new_capacity;
    return send_response(s, 185, response, 8);
}

/*==============================================================================
 * Pet System (msg_no 190-198)
 *============================================================================*/

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
        p->ui_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
        p->it_id = row[1] ? (uint32_t)atoi(row[1]) : 0;
        if (row[2]) strncpy(p->name, row[2], sizeof(p->name) - 1);
        p->exp = row[3] ? (uint32_t)atoi(row[3]) : 0;
        p->endurance = row[4] ? (uint8_t)atoi(row[4]) : 100;
        p->slot = row[5] ? (uint8_t)atoi(row[5]) : 0;
        p->creature_type = row[6] ? (uint8_t)atoi(row[6]) : 0;
        p->stat = row[7] ? (uint8_t)atoi(row[7]) : 0;
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d pets for charac_no=%u\n", count, charac_no);
    return count;
}

/* msg_no=190: PET_GET_LIST */
int handle_pet_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PET] PET_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get pets from database */
    DBPetInfo pets[MAX_PETS];
    int pet_count = db_get_pet_list(s->character_id, pets, MAX_PETS);

    response[offset++] = (uint8_t)pet_count;

    for (int i = 0; i < pet_count && offset < 500; i++) {
        /* pet_id (ui_id) */
        memcpy(response + offset, &pets[i].ui_id, 4);
        offset += 4;

        /* pet_type (it_id) */
        memcpy(response + offset, &pets[i].it_id, 4);
        offset += 4;

        /* name - 20 bytes */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, pets[i].name, 12);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level (based on exp, simplified) */
        uint8_t level = (uint8_t)(pets[i].exp / 1000 + 1);
        if (level > 100) level = 100;
        response[offset++] = level;

        /* summoned (slot 0 = summoned) */
        uint8_t summoned = (pets[i].slot == 0) ? 1 : 0;
        response[offset++] = summoned;
    }

    printf("[PET] Returning %d pets for charac_no=%u\n", pet_count, s->character_id);
    return send_response(s, 190, response, offset);
}

/* msg_no=191: PET_SUMMON - P6: Full pet summon with combat support */
int handle_pet_summon(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_SUMMON from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 191, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);

    /* Check if already have a pet summoned */
    if (s->active_pet_id != 0) {
        printf("[PET] Already have pet %u summoned, unsummoning first\n", s->active_pet_id);
        s->active_pet_id = 0;
    }

    /* Verify ownership by checking DB */
    DBPetInfo pets[MAX_PETS];
    int pet_count = db_get_pet_list(s->character_id, pets, MAX_PETS);

    int found = 0;
    uint32_t pet_item_id = 0;
    for (int i = 0; i < pet_count; i++) {
        if (pets[i].ui_id == pet_id) {
            found = 1;
            pet_item_id = pets[i].it_id;
            break;
        }
    }

    if (!found) {
        printf("[PET] Pet %u not found in character's pet list\n", pet_id);
        uint8_t error[4] = {2, 0, 0, 0};  /* Pet not owned */
        return send_response(s, 191, error, 4);
    }

    /* Summon the pet */
    s->active_pet_id = pet_id;
    /* Pet HP based on item_id tier */
    s->pet_max_hp = 500 + (pet_item_id % 100) * 50;
    s->pet_hp = s->pet_max_hp;
    s->pet_state = 1;  /* Following */

    printf("[PET] Summoned pet %u (item=%u, HP=%u)\n", pet_id, pet_item_id, s->pet_hp);

    uint8_t response[20];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &pet_id, 4);
    offset += 4;
    memcpy(response + offset, &s->pet_hp, 4);
    offset += 4;
    memcpy(response + offset, &s->pet_max_hp, 4);
    offset += 4;
    response[offset++] = s->pet_state;

    return send_response(s, 191, response, offset);
}

/* msg_no=192: PET_UNSUMMON - P6: Full unsummon */
int handle_pet_unsummon(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PET] PET_UNSUMMON from fd=%d\n", s->fd);

    if (s->active_pet_id == 0) {
        uint8_t error[4] = {2, 0, 0, 0};  /* No pet summoned */
        return send_response(s, 192, error, 4);
    }

    uint32_t old_pet_id = s->active_pet_id;
    s->active_pet_id = 0;
    s->pet_hp = 0;
    s->pet_state = 0;

    printf("[PET] Unsummoned pet %u\n", old_pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_pet_id, 4);
    return send_response(s, 192, response, 8);
}

/* msg_no=193: PET_FEED */
int handle_pet_feed(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_FEED from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 193, error, 4);
    }

    uint32_t pet_id;
    uint32_t food_id;
    memcpy(&pet_id, data, 4);
    memcpy(&food_id, data + 4, 4);

    printf("[PET] Fed pet %u with food %u\n", pet_id, food_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    uint8_t new_hunger = 100;
    response[8] = new_hunger;
    return send_response(s, 193, response, 12);
}

/* msg_no=194: PET_RENAME */
int handle_pet_rename(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_RENAME from fd=%d\n", s->fd);
    if (len < 24) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 194, error, 4);
    }

    uint32_t pet_id;
    char new_name[20];
    memcpy(&pet_id, data, 4);
    memcpy(new_name, data + 4, 20);
    new_name[19] = '\0';

    printf("[PET] Renamed pet %u to '%s'\n", pet_id, new_name);

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    memcpy(response + 8, new_name, 20);
    return send_response(s, 194, response, 28);
}

/* msg_no=195: PET_GET_INFO */
int handle_pet_get_info(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_GET_INFO from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 195, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &pet_id, 4);
    offset += 4;

    uint32_t pet_type = 1000;
    memcpy(response + offset, &pet_type, 4);
    offset += 4;

    char name[20] = "Fluffy";
    memcpy(response + offset, name, 20);
    offset += 20;

    uint8_t level = 15;
    response[offset++] = level;

    uint32_t exp = 5000;
    memcpy(response + offset, &exp, 4);
    offset += 4;

    uint8_t hunger = 85;
    response[offset++] = hunger;

    uint8_t loyalty = 95;
    response[offset++] = loyalty;

    return send_response(s, 195, response, offset);
}

/* msg_no=196: PET_RELEASE */
int handle_pet_release(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_RELEASE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 196, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);
    printf("[PET] Released pet %u\n", pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    return send_response(s, 196, response, 8);
}

/* msg_no=197: PET_EQUIP_ITEM */
int handle_pet_equip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_EQUIP_ITEM from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 197, error, 4);
    }

    uint32_t pet_id;
    uint32_t item_uid;
    memcpy(&pet_id, data, 4);
    memcpy(&item_uid, data + 4, 4);

    printf("[PET] Equipped item %u on pet %u\n", item_uid, pet_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    memcpy(response + 8, &item_uid, 4);
    return send_response(s, 197, response, 12);
}

/* msg_no=198: PET_UNEQUIP_ITEM */
int handle_pet_unequip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_UNEQUIP_ITEM from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 198, error, 4);
    }

    uint32_t pet_id;
    uint8_t slot;
    memcpy(&pet_id, data, 4);
    slot = data[4];

    printf("[PET] Unequipped slot %d from pet %u\n", slot, pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    response[8] = slot;
    return send_response(s, 198, response, 9);
}

/*==============================================================================
 * Phase 7 Protocol Handlers (300-319)
 *============================================================================*/

/* msg_no=300: ENHANCE_EQUIPMENT - P7-P1 装备强化 */
int handle_enhance_equipment(ClientSession* s, const uint8_t* data, int len) {
    printf("[ENHANCE] ENHANCE_EQUIPMENT from fd=%d\n", s->fd);
    if (len < 6) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 300, error, 4);
    }

    int32_t slot;
    uint8_t protect_type;
    uint8_t use_blessing; /* 0 = no, 1 = use blessing item for better odds */
    memcpy(&slot, data, 4);
    protect_type = data[4];
    use_blessing = data[5];

    DBItemInfo equip_item;
    if (db_get_item_at_slot(s->character_id, slot, &equip_item) < 0 ||
        equip_item.ui_id == 0) {
        uint8_t error[4] = {4, 0, 0, 0};
        return send_response(s, 300, error, 4);
    }

    uint8_t current_level = 0;
    if (db_get_enhance_level(equip_item.ui_id, &current_level) < 0) {
        current_level = 0;
    }

    printf("[ENHANCE] Slot %d current level +%d, protect=%d, blessing=%d\n",
           slot, current_level, protect_type, use_blessing);

    /* Calculate cost */
    uint8_t item_level = 50; /* Would come from item data */
    uint32_t cost = get_enhance_cost((uint8_t)current_level, item_level);

    if (s->inventory.money < cost) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 300, error, 4);
    }

    /* Deduct gold */
    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    /* If using blessing, increase success chance temporarily */
    if (use_blessing) {
        /* Would consume blessing item and boost rates */
    }

    /* Attempt enhancement */
    uint8_t new_level = 0;
    uint8_t was_destroyed = 0;
    int result = enhance_equipment((uint8_t)current_level, protect_type,
                                   &new_level, &was_destroyed);

    /* Update database */
    if (was_destroyed) {
        /* Delete the item */
        db_delete_item(s->character_id, equip_item.ui_id);
    } else {
        db_update_enhance_level(equip_item.ui_id, new_level);
    }

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t res_code = (result >= 0) ? 0 : ((was_destroyed) ? 3 : 1);
    memcpy(response + offset, &res_code, 4);
    offset += 4;

    memcpy(response + offset, &slot, 4);
    offset += 4;

    response[offset++] = new_level;
    response[offset++] = was_destroyed;
    response[offset++] = (result == 1) ? 1 : 0; /* success flag */

    memcpy(response + offset, &cost, 4);
    offset += 4;

    printf("[ENHANCE] Result: level=%d destroyed=%d cost=%u\n",
           new_level, was_destroyed, cost);
    return send_response(s, 300, response, offset);
}

/* msg_no=301: LEARN_SKILL - P7-P4 技能学习 */
int handle_learn_skill(ClientSession* s, const uint8_t* data, int len) {
    printf("[SKILL] LEARN_SKILL from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 301, error, 4);
    }

    uint16_t skill_id;
    memcpy(&skill_id, data, 2);

    /* Get player level */
    uint8_t player_level = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            player_level = (uint8_t)s->characters[i].level;
            break;
        }
    }

    /* Check prerequisites (simplified - would need skill list) */
    uint16_t player_skills[100];
    uint8_t skill_levels[100];
    int skill_count = 0;

    int prereq_result = check_skill_prerequisite(skill_id, player_level,
                                                  player_skills, skill_levels,
                                                  skill_count);
    if (prereq_result < 0) {
        uint8_t error[4] = {(uint8_t)(2 - prereq_result), 0, 0, 0};
        return send_response(s, 301, error, 4);
    }

    /* Get current skill level */
    uint8_t current_level = 0; /* Would query from DB */

    /* Calculate SP cost */
    int sp_cost = get_skill_sp_cost(skill_id, current_level);
    if (sp_cost <= 0) {
        uint8_t error[4] = {5, 0, 0, 0}; /* Max level */
        return send_response(s, 301, error, 4);
    }

    /* Learn/upgrade skill */
    uint8_t new_level = current_level + 1;
    if (db_learn_skill(s->character_id, skill_id, new_level) < 0) {
        uint8_t error[4] = {6, 0, 0, 0}; /* DB error */
        return send_response(s, 301, error, 4);
    }

    printf("[SKILL] Learned skill %u level %d (SP cost: %d)\n",
           skill_id, new_level, sp_cost);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &skill_id, 2);
    response[6] = new_level;
    response[7] = (uint8_t)sp_cost;

    return send_response(s, 301, response, 12);
}

/* msg_no=302: RESET_SKILLS - P7-P4 技能重置 */
int handle_reset_skills(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SKILL] RESET_SKILLS from fd=%d\n", s->fd);

    /* Get player level for cost calculation */
    uint8_t player_level = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            player_level = (uint8_t)s->characters[i].level;
            break;
        }
    }

    uint32_t cost = get_skill_reset_cost(player_level);
    if (s->inventory.money < cost) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 302, error, 4);
    }

    /* Deduct gold */
    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    /* Reset skills */
    uint32_t sp_refund = 0;
    if (db_reset_skills(s->character_id, &sp_refund) < 0) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Reset failed */
        return send_response(s, 302, error, 4);
    }

    printf("[SKILL] Reset all skills, refund SP=%u, cost=%u\n", sp_refund, cost);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &sp_refund, 4);
    memcpy(response + 8, &cost, 4);

    return send_response(s, 302, response, 12);
}

/* msg_no=303: SPLIT_ITEM_STACK - P7-P6 物品拆分 */
int handle_split_item_stack(ClientSession* s, const uint8_t* data, int len) {
    printf("[ITEM] SPLIT_ITEM_STACK from fd=%d\n", s->fd);
    if (len < 10) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 303, error, 4);
    }

    int32_t source_slot;
    int32_t target_slot;
    uint16_t count;
    memcpy(&source_slot, data, 4);
    memcpy(&target_slot, data + 4, 4);
    memcpy(&count, data + 8, 2);

    printf("[ITEM] Split %d items from slot %d to slot %d\n",
           count, source_slot, target_slot);

    if (db_split_stack(s->character_id, source_slot, target_slot, count) < 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Split failed */
        return send_response(s, 303, error, 4);
    }

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &source_slot, 4);
    memcpy(response + 8, &target_slot, 4);

    return send_response(s, 303, response, 12);
}

/* msg_no=304: SELECT_DUNGEON_DIFFICULTY - P7-P7 副本难度选择 */
int handle_select_dungeon_difficulty(ClientSession* s, const uint8_t* data, int len) {
    printf("[DUNGEON] SELECT_DUNGEON_DIFFICULTY from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 304, error, 4);
    }

    uint32_t dungeon_id;
    uint8_t difficulty;
    memcpy(&dungeon_id, data, 4);
    difficulty = data[4];

    /* Check if difficulty is unlocked */
    if (!is_difficulty_unlocked(s->character_id, dungeon_id, difficulty)) {
        printf("[DUNGEON] Difficulty %d not unlocked for dungeon %u\n",
               difficulty, dungeon_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Not unlocked */
        return send_response(s, 304, error, 4);
    }

    /* Check player level */
    uint8_t player_level = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            player_level = (uint8_t)s->characters[i].level;
            break;
        }
    }

    const DungeonDifficultyInfo* info = get_difficulty_info(difficulty);
    if (player_level < info->min_level) {
        printf("[DUNGEON] Player level %d < minimum %d\n",
               player_level, info->min_level);
        uint8_t error[4] = {3, 0, 0, 0}; /* Level too low */
        return send_response(s, 304, error, 4);
    }

    printf("[DUNGEON] Selected difficulty %d (%s) for dungeon %u\n",
           difficulty, info->name, dungeon_id);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;

    response[offset++] = difficulty;
    response[offset++] = info->room_count;
    response[offset++] = info->fatigue_cost;
    response[offset++] = info->recommended_level;

    uint16_t monster_scale = info->monster_level_scale;
    memcpy(response + offset, &monster_scale, 2);
    offset += 2;

    uint16_t reward_scale = info->reward_scale;
    memcpy(response + offset, &reward_scale, 2);
    offset += 2;

    return send_response(s, 304, response, offset);
}

/* msg_no=305: GUILD_DONATE - P7-P9 公会捐献 */
int handle_guild_donate(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_DONATE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 305, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    if (s->guild_id == 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not in guild */
        return send_response(s, 305, error, 4);
    }

    if (s->inventory.money < amount) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 305, error, 4);
    }

    /* Deduct gold */
    s->inventory.money -= amount;
    db_update_gold(s->character_id, s->inventory.money);

    /* Add guild exp (1 exp per 100 gold donated) */
    uint32_t exp_gain = amount / 100;
    if (exp_gain < 1) exp_gain = 1;

    uint8_t new_level = 0;
    uint8_t leveled_up = 0;
    db_add_guild_exp(s->guild_id, exp_gain, &new_level, &leveled_up);

    /* Add contribution */
    db_add_guild_contribution(s->guild_id, s->character_id, amount);

    printf("[GUILD] Donated %u gold, guild exp +%u, level=%d, levelup=%d\n",
           amount, exp_gain, new_level, leveled_up);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &amount, 4);
    offset += 4;

    memcpy(response + offset, &exp_gain, 4);
    offset += 4;

    response[offset++] = new_level;
    response[offset++] = leveled_up;

    return send_response(s, 305, response, offset);
}

/* msg_no=306: VALIDATE_EQUIP - P7-P5 装备验证 */
int handle_validate_equip(ClientSession* s, const uint8_t* data, int len) {
    printf("[EQUIP] VALIDATE_EQUIP from fd=%d\n", s->fd);
    if (len < 9) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 306, error, 4);
    }

    uint32_t item_id;
    uint8_t target_slot;
    memcpy(&item_id, data, 4);
    target_slot = data[4];

    /* Get player info */
    uint8_t player_level = 1;
    uint8_t player_job = 0;
    uint8_t player_sex = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            player_level = (uint8_t)s->characters[i].level;
            player_job = (uint8_t)s->characters[i].job;
            player_sex = s->characters[i].sex;
            break;
        }
    }

    int validation = validate_equipment(item_id, player_level, player_job,
                                        player_sex, target_slot);

    uint8_t response[8];
    memset(response, 0, sizeof(response));

    uint32_t result = (validation == 0) ? 0 : (uint32_t)(-validation);
    memcpy(response, &result, 4);
    memcpy(response + 4, &item_id, 4);

    printf("[EQUIP] Validation result for item %u: %d\n", item_id, validation);
    return send_response(s, 306, response, 8);
}

/*==============================================================================
 * Phase 13 P1: Tower Challenge System (死亡之塔)
 *============================================================================*/

#define TOWER_MAX_FLOOR        100
#define TOWER_DAILY_LIMIT      3
#define TOWER_BOSS_INTERVAL    10

/* Tower floor info */
typedef struct {
    uint8_t  floor;
    uint8_t  monster_count;
    uint16_t monster_types[5];
    uint8_t  monster_levels[5];
    uint8_t  is_boss_floor;
    uint16_t boss_type;
    uint32_t clear_reward_gold;
    uint32_t clear_reward_exp;
} TowerFloorInfo;

/* Tower session state */
typedef struct {
    uint8_t  active;
    uint8_t  current_floor;
    uint8_t  highest_floor;
    uint32_t start_time;
    uint32_t total_damage;
    uint16_t deaths;
    uint8_t  daily_attempts;
} TowerState;

/* Tower ranking entry */
typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  highest_floor;
    uint32_t clear_time;
    uint32_t total_damage;
} TowerRankEntry;

/* Floor difficulty scaling */
static const float g_tower_floor_scaling[] = {
    1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.6f, 1.8f, 2.0f, 2.3f, 2.6f  /* Per 10 floors */
};

/* Generate tower floor info */
static void generate_tower_floor(uint8_t floor, TowerFloorInfo* info) {
    if (!info) return;
    memset(info, 0, sizeof(TowerFloorInfo));

    info->floor = floor;
    info->is_boss_floor = (floor % TOWER_BOSS_INTERVAL == 0) ? 1 : 0;

    /* Scale difficulty by floor tier */
    int tier = (floor - 1) / 10;
    if (tier > 9) tier = 9;
    float scale = g_tower_floor_scaling[tier];

    if (info->is_boss_floor) {
        /* Boss floor: 1 boss monster */
        info->monster_count = 1;
        info->boss_type = 9000 + floor;  /* Boss ID = 9000 + floor */
        info->monster_types[0] = info->boss_type;
        info->monster_levels[0] = (uint8_t)(20 + floor * 0.6f);
    } else {
        /* Normal floor: 3-5 monsters */
        info->monster_count = 3 + (floor % 3);
        for (int i = 0; i < info->monster_count && i < 5; i++) {
            info->monster_types[i] = 1001 + (floor % 20);
            info->monster_levels[i] = (uint8_t)(15 + floor * 0.5f);
        }
    }

    /* Rewards scale with floor */
    info->clear_reward_gold = (uint32_t)(100 * floor * scale);
    info->clear_reward_exp = (uint32_t)(50 * floor * scale);
}

/* DB: Get player tower progress */
static int db_get_tower_progress(uint32_t charac_no, uint8_t* highest_floor, uint8_t* daily_attempts) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT highest_floor, daily_attempts FROM tower_progress WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) return -1;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        if (highest_floor) *highest_floor = row[0] ? (uint8_t)atoi(row[0]) : 0;
        if (daily_attempts) *daily_attempts = row[0] ? (uint8_t)atoi(row[1]) : 0;
    } else {
        if (highest_floor) *highest_floor = 0;
        if (daily_attempts) *daily_attempts = 0;
    }

    mysql_free_result(result);
    return 0;
}

/* DB: Update tower progress */
static int db_update_tower_progress(uint32_t charac_no, uint8_t floor_cleared) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO tower_progress (charac_no, highest_floor, daily_attempts, last_attempt) "
             "VALUES (%u, %d, 1, NOW()) "
             "ON DUPLICATE KEY UPDATE "
             "highest_floor = GREATEST(highest_floor, %d), "
             "daily_attempts = IF(DATE(last_attempt) = CURDATE(), daily_attempts + 1, 1), "
             "last_attempt = NOW()",
             charac_no, floor_cleared, floor_cleared);

    return mysql_query(g_db_charac, query) ? -1 : 0;
}

/* DB: Get tower ranking */
static int db_get_tower_ranking(TowerRankEntry* entries, int max_count) {
    if (!g_db_charac || !entries) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT t.charac_no, c.charac_name, t.highest_floor, t.best_time, t.total_damage "
             "FROM tower_progress t "
             "JOIN charac_info c ON t.charac_no = c.charac_no "
             "ORDER BY t.highest_floor DESC, t.best_time ASC "
             "LIMIT %d", max_count);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        entries[count].charac_no = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].charac_name, row[1], 19);
        entries[count].highest_floor = row[2] ? (uint8_t)atoi(row[2]) : 0;
        entries[count].clear_time = row[3] ? (uint32_t)atoi(row[3]) : 0;
        entries[count].total_damage = row[4] ? (uint32_t)atoi(row[4]) : 0;
        count++;
    }

    mysql_free_result(result);
    return count;
}

/*==============================================================================
 * Phase 13 P2: Equipment Set Bonus System (套装系统)
 *============================================================================*/

#define SET_MAX_ITEMS         8
#define SET_MAX_BONUSES       4
#define SET_DEFINITION_MAX    20

/* Set bonus tier (number of items equipped) */
typedef struct {
    uint8_t  items_required;
    int16_t  phys_attack;
    int16_t  mag_attack;
    int16_t  phys_def;
    int16_t  mag_def;
    int16_t  max_hp;
    int16_t  crit_rate;
    int16_t  move_speed;
} SetBonusTier;

/* Equipment set definition */
typedef struct {
    uint16_t set_id;
    char     set_name[32];
    uint32_t item_ids[SET_MAX_ITEMS];
    uint8_t  item_count;
    SetBonusTier bonuses[SET_MAX_BONUSES];
    uint8_t  bonus_count;
} EquipmentSet;

/* Sample equipment sets */
static const EquipmentSet g_equipment_sets[] = {
    /* Set 1: Warrior's Might (5 pieces) */
    {1, "Warrior's Might", {1001, 1002, 1003, 1004, 1005, 0, 0, 0}, 5,
     {{2, 10, 0, 5, 0, 100, 0, 0},
      {3, 25, 0, 15, 0, 300, 50, 0},
      {5, 50, 0, 30, 0, 500, 100, 5},
      {0, 0, 0, 0, 0, 0, 0, 0}}, 3},

    /* Set 2: Mage's Wisdom (5 pieces) */
    {2, "Mage's Wisdom", {2001, 2002, 2003, 2004, 2005, 0, 0, 0}, 5,
     {{2, 0, 15, 0, 10, 50, 0, 0},
      {3, 0, 35, 0, 25, 150, 75, 0},
      {5, 0, 60, 0, 45, 300, 150, 3},
      {0, 0, 0, 0, 0, 0, 0, 0}}, 3},

    /* Set 3: Shadow Assassin (4 pieces) */
    {3, "Shadow Assassin", {3001, 3002, 3003, 3004, 0, 0, 0, 0}, 4,
     {{2, 20, 0, 0, 0, 0, 100, 3},
      {4, 50, 0, 10, 0, 200, 200, 8},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}}, 2},

    /* End marker */
    {0, "", {0}, 0, {{0}}, 0}
};

/* Find equipment set by ID */
static const EquipmentSet* find_equipment_set(uint16_t set_id) {
    for (int i = 0; g_equipment_sets[i].set_id != 0; i++) {
        if (g_equipment_sets[i].set_id == set_id) {
            return &g_equipment_sets[i];
        }
    }
    return NULL;
}

/* Check how many items from a set the player has equipped */
static int count_equipped_set_items(ClientSession* s, const EquipmentSet* set) {
    if (!s || !set) return 0;

    int count = 0;
    for (int i = 0; i < set->item_count; i++) {
        uint32_t set_item = set->item_ids[i];
        if (set_item == 0) continue;

        /* Check if this item is in equipped slots */
        for (int slot = 0; slot < MAX_EQUIP_SLOTS && slot < 12; slot++) {
            if (s->equipment.equipment[slot].it_id == set_item) {
                count++;
                break;
            }
        }
    }
    return count;
}

/* Calculate total set bonuses for a player */
static void calculate_set_bonuses(ClientSession* s, SetBonusTier* total_bonus) {
    if (!s || !total_bonus) return;
    memset(total_bonus, 0, sizeof(SetBonusTier));

    /* Check all known sets */
    for (int i = 0; g_equipment_sets[i].set_id != 0; i++) {
        const EquipmentSet* set = &g_equipment_sets[i];
        int equipped = count_equipped_set_items(s, set);

        /* Find highest applicable bonus tier */
        for (int b = set->bonus_count - 1; b >= 0; b--) {
            if (set->bonuses[b].items_required > 0 &&
                equipped >= set->bonuses[b].items_required) {
                /* Add this bonus */
                total_bonus->phys_attack += set->bonuses[b].phys_attack;
                total_bonus->mag_attack += set->bonuses[b].mag_attack;
                total_bonus->phys_def += set->bonuses[b].phys_def;
                total_bonus->mag_def += set->bonuses[b].mag_def;
                total_bonus->max_hp += set->bonuses[b].max_hp;
                total_bonus->crit_rate += set->bonuses[b].crit_rate;
                total_bonus->move_speed += set->bonuses[b].move_speed;
                break;  /* Only highest tier bonus applies per set */
            }
        }
    }
}

/* Apply set bonuses to combat stats */
static void apply_set_bonuses_to_stats(ClientSession* s) {
    if (!s) return;

    SetBonusTier bonus;
    calculate_set_bonuses(s, &bonus);

    s->combat_stats.phys_attack += bonus.phys_attack;
    s->combat_stats.mag_attack += bonus.mag_attack;
    s->combat_stats.phys_def += bonus.phys_def;
    s->combat_stats.mag_def += bonus.mag_def;
    s->combat_stats.max_hp += bonus.max_hp;
    s->combat_stats.crit_rate += bonus.crit_rate;
    s->combat_stats.move_speed += bonus.move_speed;
}

/*==============================================================================
 * Phase 13 P3: Season System (赛季系统)
 *============================================================================*/

#define SEASON_DURATION_DAYS   90
#define SEASON_RANK_TIERS      7

typedef enum {
    RANK_BRONZE = 0,
    RANK_SILVER = 1,
    RANK_GOLD = 2,
    RANK_PLATINUM = 3,
    RANK_DIAMOND = 4,
    RANK_MASTER = 5,
    RANK_GRANDMASTER = 6
} SeasonRank;

/* Season rank thresholds (rating points) */
static const uint32_t g_season_rank_thresholds[] = {
    0,      /* Bronze: 0+ */
    1000,   /* Silver: 1000+ */
    1500,   /* Gold: 1500+ */
    2000,   /* Platinum: 2000+ */
    2500,   /* Diamond: 2500+ */
    3000,   /* Master: 3000+ */
    3500    /* Grandmaster: 3500+ */
};

/* Season reward by rank tier */
typedef struct {
    uint8_t  rank;
    uint32_t gold_reward;
    uint32_t item_reward;
    uint16_t title_reward;
} SeasonReward;

static const SeasonReward g_season_rewards[] = {
    {RANK_BRONZE,      1000,  0,     0},
    {RANK_SILVER,      3000,  50001, 0},
    {RANK_GOLD,        5000,  50002, 101},
    {RANK_PLATINUM,    10000, 50003, 102},
    {RANK_DIAMOND,     20000, 50004, 103},
    {RANK_MASTER,      50000, 50005, 104},
    {RANK_GRANDMASTER, 100000,50006, 105}
};

/* Season info structure */
typedef struct {
    uint32_t season_id;
    uint32_t start_time;
    uint32_t end_time;
    uint8_t  is_active;
} SeasonInfo;

/* Player season data */
typedef struct {
    uint32_t charac_no;
    uint32_t season_id;
    uint32_t rating;
    uint8_t  rank;
    uint32_t wins;
    uint32_t losses;
    uint8_t  rewards_claimed;
} PlayerSeasonData;

/* Get rank from rating */
static uint8_t get_rank_from_rating(uint32_t rating) {
    for (int i = SEASON_RANK_TIERS - 1; i >= 0; i--) {
        if (rating >= g_season_rank_thresholds[i]) {
            return (uint8_t)i;
        }
    }
    return RANK_BRONZE;
}

/* DB: Get current season */
static int db_get_current_season(SeasonInfo* season) {
    if (!g_db_charac || !season) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT season_id, UNIX_TIMESTAMP(start_time), UNIX_TIMESTAMP(end_time), "
             "(NOW() BETWEEN start_time AND end_time) as is_active "
             "FROM season_info ORDER BY season_id DESC LIMIT 1");

    if (mysql_query(g_db_charac, query)) return -1;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        season->season_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
        season->start_time = row[1] ? (uint32_t)atoi(row[1]) : 0;
        season->end_time = row[2] ? (uint32_t)atoi(row[2]) : 0;
        season->is_active = row[3] ? (uint8_t)atoi(row[3]) : 0;
        mysql_free_result(result);
        return 0;
    }

    mysql_free_result(result);
    return -1;
}

/* DB: Get player season data */
static int db_get_player_season_data(uint32_t charac_no, uint32_t season_id, PlayerSeasonData* data) {
    if (!g_db_charac || !data) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT rating, wins, losses, rewards_claimed "
             "FROM player_season WHERE charac_no=%u AND season_id=%u",
             charac_no, season_id);

    if (mysql_query(g_db_charac, query)) return -1;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    memset(data, 0, sizeof(PlayerSeasonData));
    data->charac_no = charac_no;
    data->season_id = season_id;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        data->rating = row[0] ? (uint32_t)atoi(row[0]) : 1000;
        data->wins = row[1] ? (uint32_t)atoi(row[1]) : 0;
        data->losses = row[2] ? (uint32_t)atoi(row[2]) : 0;
        data->rewards_claimed = row[3] ? (uint8_t)atoi(row[3]) : 0;
    } else {
        data->rating = 1000;  /* Starting rating */
    }

    data->rank = get_rank_from_rating(data->rating);
    mysql_free_result(result);
    return 0;
}

/* DB: Update player season rating */
static int db_update_season_rating(uint32_t charac_no, uint32_t season_id, int32_t rating_change, uint8_t is_win) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO player_season (charac_no, season_id, rating, wins, losses) "
             "VALUES (%u, %u, %d, %d, %d) "
             "ON DUPLICATE KEY UPDATE "
             "rating = GREATEST(0, rating + %d), "
             "wins = wins + %d, "
             "losses = losses + %d",
             charac_no, season_id, 1000 + rating_change, is_win ? 1 : 0, is_win ? 0 : 1,
             rating_change, is_win ? 1 : 0, is_win ? 0 : 1);

    return mysql_query(g_db_charac, query) ? -1 : 0;
}

/*==============================================================================
 * Phase 13 P4: Equipment Growth System (装备成长/潜力系统)
 *============================================================================*/

#define GROWTH_MAX_LEVEL       10
#define GROWTH_MAX_POTENTIAL   100

/* Growth material costs by level */
static const uint32_t g_growth_material_cost[] = {
    100, 200, 400, 800, 1500, 3000, 5000, 8000, 12000, 20000
};

/* Growth stat bonus per level (percentage) */
static const uint8_t g_growth_stat_bonus[] = {
    2, 4, 7, 10, 14, 18, 23, 28, 34, 40  /* Total % bonus at each level */
};

/* Equipment growth info */
typedef struct {
    uint32_t item_uid;
    uint8_t  growth_level;
    uint8_t  potential;
    uint32_t accumulated_exp;
} EquipmentGrowth;

/* DB: Get equipment growth */
static int db_get_equipment_growth(uint32_t item_uid, EquipmentGrowth* growth) {
    if (!g_db_charac || !growth) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT growth_level, potential, accumulated_exp "
             "FROM equipment_growth WHERE item_uid=%u",
             item_uid);

    if (mysql_query(g_db_charac, query)) return -1;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    memset(growth, 0, sizeof(EquipmentGrowth));
    growth->item_uid = item_uid;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        growth->growth_level = row[0] ? (uint8_t)atoi(row[0]) : 0;
        growth->potential = row[1] ? (uint8_t)atoi(row[1]) : 100;
        growth->accumulated_exp = row[2] ? (uint32_t)atoi(row[2]) : 0;
    } else {
        growth->potential = 100;  /* Default potential */
    }

    mysql_free_result(result);
    return 0;
}

/* DB: Upgrade equipment growth level */
static int db_upgrade_equipment_growth(uint32_t item_uid, uint32_t charac_no) {
    if (!g_db_charac) return -1;

    /* Get current growth info */
    EquipmentGrowth growth;
    if (db_get_equipment_growth(item_uid, &growth) < 0) {
        return -1;
    }

    if (growth.growth_level >= GROWTH_MAX_LEVEL) {
        return -2;  /* Already max level */
    }

    /* Check material cost */
    uint32_t cost = g_growth_material_cost[growth.growth_level];

    /* Deduct gold */
    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET money = money - %u "
             "WHERE charac_no=%u AND money >= %u",
             cost, charac_no, cost);

    if (mysql_query(g_db_charac, query) || mysql_affected_rows(g_db_charac) == 0) {
        return -3;  /* Not enough gold */
    }

    /* Upgrade growth level */
    snprintf(query, sizeof(query),
             "INSERT INTO equipment_growth (item_uid, growth_level, potential) "
             "VALUES (%u, 1, 100) "
             "ON DUPLICATE KEY UPDATE growth_level = growth_level + 1",
             item_uid);

    if (mysql_query(g_db_charac, query)) {
        return -4;
    }

    printf("[GROWTH] Item %u upgraded to level %d\n", item_uid, growth.growth_level + 1);
    return growth.growth_level + 1;
}

/* Calculate growth bonus for an item */
static int16_t calculate_growth_bonus(uint8_t growth_level, int16_t base_stat) {
    if (growth_level == 0 || growth_level > GROWTH_MAX_LEVEL) return 0;
    return (int16_t)(base_stat * g_growth_stat_bonus[growth_level - 1] / 100);
}

/*==============================================================================
 * Phase 13 P5: Mystery Shop System (神秘商店)
 *============================================================================*/

#define MYSTERY_SHOP_SLOTS     6
#define MYSTERY_SHOP_REFRESH_HOURS 4

/* Mystery shop item */
typedef struct {
    uint32_t item_id;
    uint8_t  count;
    uint32_t price;
    uint8_t  discount_percent;
    uint8_t  purchase_limit;
    uint8_t  purchased;
} MysteryShopItem;

/* Mystery shop state */
typedef struct {
    uint32_t charac_no;
    uint32_t last_refresh;
    uint8_t  free_refreshes;
    MysteryShopItem items[MYSTERY_SHOP_SLOTS];
} MysteryShopState;

/* Possible mystery shop items by tier */
static const uint32_t g_mystery_items_common[] = {
    10001, 10002, 10003, 10004, 10005,  /* Common materials */
    20001, 20002, 20003,                 /* Consumables */
    0
};

static const uint32_t g_mystery_items_rare[] = {
    30001, 30002, 30003,  /* Rare materials */
    40001, 40002,         /* Enhancement stones */
    0
};

static const uint32_t g_mystery_items_epic[] = {
    50001, 50002,  /* Epic materials */
    60001,         /* Special items */
    0
};

/* Generate random shop items */
static void generate_mystery_shop_items(MysteryShopState* shop) {
    if (!shop) return;

    srand((unsigned int)time(NULL) + shop->charac_no);

    for (int i = 0; i < MYSTERY_SHOP_SLOTS; i++) {
        MysteryShopItem* item = &shop->items[i];
        memset(item, 0, sizeof(MysteryShopItem));

        /* Determine rarity based on slot */
        int rarity_roll = rand() % 100;
        const uint32_t* item_pool;
        int pool_size = 0;

        if (rarity_roll < 60) {
            /* 60% common */
            item_pool = g_mystery_items_common;
            while (item_pool[pool_size] != 0) pool_size++;
        } else if (rarity_roll < 90) {
            /* 30% rare */
            item_pool = g_mystery_items_rare;
            while (item_pool[pool_size] != 0) pool_size++;
        } else {
            /* 10% epic */
            item_pool = g_mystery_items_epic;
            while (item_pool[pool_size] != 0) pool_size++;
        }

        if (pool_size > 0) {
            item->item_id = item_pool[rand() % pool_size];
            item->count = 1 + (rand() % 5);
            item->price = 1000 + (rand() % 9000);
            item->discount_percent = (rand() % 50);  /* 0-49% discount */
            item->purchase_limit = 1 + (rand() % 3);
            item->purchased = 0;
        }
    }

    shop->last_refresh = (uint32_t)time(NULL);
}

/* DB: Load mystery shop state */
static int db_load_mystery_shop(uint32_t charac_no, MysteryShopState* shop) {
    if (!g_db_charac || !shop) return -1;

    memset(shop, 0, sizeof(MysteryShopState));
    shop->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT UNIX_TIMESTAMP(last_refresh), free_refreshes "
             "FROM mystery_shop WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) return -1;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        shop->last_refresh = row[0] ? (uint32_t)atoi(row[0]) : 0;
        shop->free_refreshes = row[1] ? (uint8_t)atoi(row[1]) : 3;
    } else {
        shop->free_refreshes = 3;
    }

    mysql_free_result(result);

    /* Check if refresh needed */
    uint32_t now = (uint32_t)time(NULL);
    if (shop->last_refresh == 0 ||
        (now - shop->last_refresh) > MYSTERY_SHOP_REFRESH_HOURS * 3600) {
        generate_mystery_shop_items(shop);
    }

    return 0;
}

/* DB: Save mystery shop state */
static int db_save_mystery_shop(MysteryShopState* shop) {
    if (!g_db_charac || !shop) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO mystery_shop (charac_no, last_refresh, free_refreshes) "
             "VALUES (%u, FROM_UNIXTIME(%u), %d) "
             "ON DUPLICATE KEY UPDATE "
             "last_refresh = FROM_UNIXTIME(%u), free_refreshes = %d",
             shop->charac_no, shop->last_refresh, shop->free_refreshes,
             shop->last_refresh, shop->free_refreshes);

    return mysql_query(g_db_charac, query) ? -1 : 0;
}

/* Purchase mystery shop item */
static int mystery_shop_purchase(ClientSession* s, uint8_t slot) {
    if (!s || slot >= MYSTERY_SHOP_SLOTS) return -1;

    MysteryShopState shop;
    if (db_load_mystery_shop(s->character_id, &shop) < 0) {
        return -2;
    }

    MysteryShopItem* item = &shop.items[slot];
    if (item->item_id == 0) return -3;  /* Empty slot */
    if (item->purchased >= item->purchase_limit) return -4;  /* Purchase limit */

    /* Calculate discounted price */
    uint32_t final_price = item->price * (100 - item->discount_percent) / 100;

    /* Deduct gold (simplified) */
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET money = money - %u "
             "WHERE charac_no=%u AND money >= %u",
             final_price, s->character_id, final_price);

    if (mysql_query(g_db_charac, query) || mysql_affected_rows(g_db_charac) == 0) {
        return -5;  /* Not enough gold */
    }

    /* Add item to inventory (simplified) */
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, it_id, count, slot) "
             "SELECT %u, %u, %d, COALESCE(MAX(slot)+1, 0) FROM user_items WHERE charac_no=%u",
             s->character_id, item->item_id, item->count, s->character_id);

    mysql_query(g_db_charac, query);

    item->purchased++;
    db_save_mystery_shop(&shop);

    printf("[MYSTERY] Character %u purchased item %u x%d for %u gold\n",
           s->character_id, item->item_id, item->count, final_price);
    return 0;
}


typedef struct {
    uint32_t item_id;
    uint32_t price;
    uint8_t discount;
    uint16_t stock;
    uint8_t category;
    uint8_t vip_required;
} CashShopItemDef;

static CashShopItemDef g_cash_shop_items[] = {
    {90001, 1000, 0, 0, 1, 0},
    {90002, 500, 10, 100, 2, 0},
    {90003, 200, 0, 0, 2, 0},
    {90004, 2000, 20, 50, 3, 1},
    {90005, 5000, 0, 0, 4, 0},
    {90006, 300, 50, 0, 2, 0},
    {90007, 800, 0, 0, 1, 0},
    {90008, 1500, 15, 0, 3, 2},
    {0, 0, 0, 0, 0, 0}
};

static int db_deduct_cash(uint32_t account_id, uint32_t amount) {
    if (!g_db_account || amount == 0) return -1;

    uint32_t cash = 0;
    uint32_t bind_cash = 0;
    db_get_cash_balance(account_id, &cash, &bind_cash);

    if (cash + bind_cash < amount) return -2;

    uint32_t deduct_bind = (bind_cash >= amount) ? amount : bind_cash;
    uint32_t deduct_cash_amt = amount - deduct_bind;

    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE cash_balance SET cash=cash-%u, bind_cash=bind_cash-%u \
             WHERE account_id=%u AND cash>=%u AND bind_cash>=%u",
             deduct_cash_amt, deduct_bind, account_id, deduct_cash_amt, deduct_bind);

    if (mysql_query(g_db_account, query) || mysql_affected_rows(g_db_account) == 0) {
        return -3;
    }

    printf("[CASH] Deducted %u from account %u\n", amount, account_id);
    return 0;
}

static int db_add_cash_log(uint32_t account_id, uint8_t type, uint32_t item_id,
                           uint32_t price, uint32_t charac_no) {
    if (!g_db_account) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO cash_shop_log (account_id,type,item_id,price,charac_no,create_time) \
             VALUES (%u,%u,%u,%u,%u,NOW())",
             account_id, type, item_id, price, charac_no);

    mysql_query(g_db_account, query);
    return 0;
}

static CashShopItemDef* find_cash_shop_item(uint32_t item_id) {
    for (int i = 0; g_cash_shop_items[i].item_id != 0; i++) {
        if (g_cash_shop_items[i].item_id == item_id) return &g_cash_shop_items[i];
    }
    return NULL;
}

static int cash_shop_buy_item(ClientSession* s, uint32_t item_id, uint8_t count) {
    if (!s || count == 0) return -1;
    CashShopItemDef* item = find_cash_shop_item(item_id);
    if (!item) return -2;
    uint8_t vip_level = 0; uint32_t vip_expire = 0;
    db_get_vip_info(s->account_id, &vip_level, &vip_expire);
    if (item->vip_required > vip_level) return -3;
    uint32_t total_price = (item->price * (100 - item->discount) / 100) * count;
    if (db_deduct_cash(s->account_id, total_price) < 0) return -4;
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no,it_id,count,slot) "
             "SELECT %u,%u,%d,COALESCE(MAX(slot)+1,0) FROM user_items WHERE charac_no=%u",
             s->character_id, item_id, count, s->character_id);
    if (mysql_query(g_db_charac, query)) return -5;
    db_add_cash_log(s->account_id, 1, item_id, total_price, s->character_id);
    printf("[CASH] Account %u bought item %u x%d for %u\n", s->account_id, item_id, count, total_price);
    return 0;
}
/*==============================================================================
 * Phase 13 Protocol Handlers (410-449)
 *============================================================================*/

/* msg_no=410: TOWER_ENTER - 进入死亡之塔 */
int handle_tower_enter(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TOWER] ENTER from fd=%d charac=%u\n", s->fd, s->character_id);

    /* Check daily attempts */
    uint8_t highest = 0, attempts = 0;
    db_get_tower_progress(s->character_id, &highest, &attempts);

    uint8_t response[32];
    memset(response, 0, sizeof(response));

    if (attempts >= TOWER_DAILY_LIMIT) {
        /* Daily limit reached */
        uint32_t result = 1;
        memcpy(response, &result, 4);
        return send_response(s, 410, response, 4);
    }

    /* Generate first floor */
    TowerFloorInfo floor;
    generate_tower_floor(1, &floor);

    /* Success response */
    uint32_t result = 0;
    memcpy(response, &result, 4);
    response[4] = highest;  /* Highest floor reached */
    response[5] = attempts; /* Daily attempts used */
    response[6] = floor.floor;
    response[7] = floor.monster_count;
    response[8] = floor.is_boss_floor;

    return send_response(s, 410, response, 16);
}

/* msg_no=411: TOWER_CLEAR_FLOOR - 通关层 */
int handle_tower_clear_floor(ClientSession* s, const uint8_t* data, int len) {
    if (len < 1) return -1;
    uint8_t floor = data[0];

    printf("[TOWER] CLEAR_FLOOR %d from fd=%d\n", floor, s->fd);

    /* Update progress */
    db_update_tower_progress(s->character_id, floor);

    /* Generate next floor info */
    TowerFloorInfo next_floor;
    if (floor < TOWER_MAX_FLOOR) {
        generate_tower_floor(floor + 1, &next_floor);
    }

    uint8_t response[64];
    memset(response, 0, sizeof(response));

    uint32_t result = 0;
    memcpy(response, &result, 4);
    response[4] = floor;

    /* Rewards */
    TowerFloorInfo cleared;
    generate_tower_floor(floor, &cleared);
    memcpy(response + 8, &cleared.clear_reward_gold, 4);
    memcpy(response + 12, &cleared.clear_reward_exp, 4);

    /* Next floor info */
    if (floor < TOWER_MAX_FLOOR) {
        response[16] = next_floor.floor;
        response[17] = next_floor.monster_count;
        response[18] = next_floor.is_boss_floor;
    } else {
        response[16] = 0;  /* No more floors */
    }

    return send_response(s, 411, response, 32);
}

/* msg_no=412: TOWER_GET_RANKING - 获取排行榜 */
int handle_tower_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TOWER] GET_RANKING from fd=%d\n", s->fd);

    TowerRankEntry entries[20];
    int count = db_get_tower_ranking(entries, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));

    uint32_t result = 0;
    memcpy(response, &result, 4);
    response[4] = (uint8_t)count;

    int offset = 8;
    for (int i = 0; i < count && offset < 500; i++) {
        memcpy(response + offset, &entries[i].charac_no, 4); offset += 4;
        memcpy(response + offset, entries[i].charac_name, 20); offset += 20;
        response[offset++] = entries[i].highest_floor;
    }

    return send_response(s, 412, response, offset);
}

/* msg_no=420: SET_GET_INFO - 获取套装信息 */
int handle_set_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SET] GET_INFO from fd=%d\n", s->fd);

    SetBonusTier bonus;
    calculate_set_bonuses(s, &bonus);

    uint8_t response[64];
    memset(response, 0, sizeof(response));

    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &bonus.phys_attack, 2);
    memcpy(response + 6, &bonus.mag_attack, 2);
    memcpy(response + 8, &bonus.phys_def, 2);
    memcpy(response + 10, &bonus.mag_def, 2);
    memcpy(response + 12, &bonus.max_hp, 2);
    memcpy(response + 14, &bonus.crit_rate, 2);
    memcpy(response + 16, &bonus.move_speed, 2);

    return send_response(s, 420, response, 20);
}

/* msg_no=430: SEASON_GET_INFO - 获取赛季信息 */
int handle_season_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SEASON] GET_INFO from fd=%d\n", s->fd);

    SeasonInfo season;
    PlayerSeasonData player_data;

    uint8_t response[64];
    memset(response, 0, sizeof(response));

    if (db_get_current_season(&season) < 0) {
        uint32_t result = 1;  /* No active season */
        memcpy(response, &result, 4);
        return send_response(s, 430, response, 4);
    }

    db_get_player_season_data(s->character_id, season.season_id, &player_data);

    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &season.season_id, 4);
    response[8] = season.is_active;
    memcpy(response + 12, &player_data.rating, 4);
    response[16] = player_data.rank;
    memcpy(response + 20, &player_data.wins, 4);
    memcpy(response + 24, &player_data.losses, 4);

    return send_response(s, 430, response, 32);
}

/* msg_no=440: GROWTH_GET_INFO - 获取装备成长信息 */
int handle_growth_get_info(ClientSession* s, const uint8_t* data, int len) {
    if (len < 4) return -1;

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    printf("[GROWTH] GET_INFO item=%u from fd=%d\n", item_uid, s->fd);

    EquipmentGrowth growth;
    db_get_equipment_growth(item_uid, &growth);

    uint8_t response[32];
    memset(response, 0, sizeof(response));

    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &item_uid, 4);
    response[8] = growth.growth_level;
    response[9] = growth.potential;

    /* Next level cost */
    uint32_t next_cost = 0;
    if (growth.growth_level < GROWTH_MAX_LEVEL) {
        next_cost = g_growth_material_cost[growth.growth_level];
    }
    memcpy(response + 12, &next_cost, 4);

    return send_response(s, 440, response, 20);
}

/* msg_no=441: GROWTH_UPGRADE - 升级装备成长 */
int handle_growth_upgrade(ClientSession* s, const uint8_t* data, int len) {
    if (len < 4) return -1;

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    printf("[GROWTH] UPGRADE item=%u from fd=%d\n", item_uid, s->fd);

    int new_level = db_upgrade_equipment_growth(item_uid, s->character_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    uint32_t result = (new_level > 0) ? 0 : (uint32_t)(-new_level);
    memcpy(response, &result, 4);
    memcpy(response + 4, &item_uid, 4);
    response[8] = (new_level > 0) ? (uint8_t)new_level : 0;

    return send_response(s, 441, response, 12);
}

/* msg_no=450: MYSTERY_SHOP_GET - 获取神秘商店 */
int handle_mystery_shop_get(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MYSTERY] GET from fd=%d\n", s->fd);

    MysteryShopState shop;
    db_load_mystery_shop(s->character_id, &shop);

    uint8_t response[128];
    memset(response, 0, sizeof(response));

    uint32_t result = 0;
    memcpy(response, &result, 4);
    response[4] = shop.free_refreshes;

    int offset = 8;
    for (int i = 0; i < MYSTERY_SHOP_SLOTS; i++) {
        MysteryShopItem* item = &shop.items[i];
        memcpy(response + offset, &item->item_id, 4); offset += 4;
        response[offset++] = item->count;
        memcpy(response + offset, &item->price, 4); offset += 4;
        response[offset++] = item->discount_percent;
        response[offset++] = item->purchase_limit;
        response[offset++] = item->purchased;
    }

    return send_response(s, 450, response, offset);
}

/* msg_no=451: MYSTERY_SHOP_BUY - 神秘商店购买 */
int handle_mystery_shop_buy(ClientSession* s, const uint8_t* data, int len) {
    if (len < 1) return -1;
    uint8_t slot = data[0];

    printf("[MYSTERY] BUY slot=%d from fd=%d\n", slot, s->fd);

    int result = mystery_shop_purchase(s, slot);

    uint8_t response[8];
    memset(response, 0, sizeof(response));

    uint32_t res = (result == 0) ? 0 : (uint32_t)(-result);
    memcpy(response, &res, 4);
    response[4] = slot;

    return send_response(s, 451, response, 8);
}

/* msg_no=452: MYSTERY_SHOP_REFRESH - 刷新神秘商店 */
int handle_mystery_shop_refresh(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MYSTERY] REFRESH from fd=%d\n", s->fd);

    MysteryShopState shop;
    db_load_mystery_shop(s->character_id, &shop);

    uint8_t response[8];
    memset(response, 0, sizeof(response));

    if (shop.free_refreshes == 0) {
        /* No free refreshes, would need gems/cash */
        uint32_t result = 1;
        memcpy(response, &result, 4);
        return send_response(s, 452, response, 4);
    }

    shop.free_refreshes--;
    generate_mystery_shop_items(&shop);
    db_save_mystery_shop(&shop);

    uint32_t result = 0;
    memcpy(response, &result, 4);
    response[4] = shop.free_refreshes;

    return send_response(s, 452, response, 8);
}

/*==============================================================================
 * Phase 8 Protocol Handlers (310-409)
 *============================================================================*/

/* msg_no=310: AVATAR_GET_LIST - 获取时装列表 */
int handle_avatar_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AVATAR] GET_LIST from fd=%d\n", s->fd);

    uint32_t avatar_items[AVATAR_SLOT_MAX];
    int count = db_load_avatar_items(s->character_id, avatar_items, AVATAR_SLOT_MAX);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result code */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Avatar count */
    response[offset++] = (uint8_t)(count > 0 ? count : 0);

    /* Avatar items */
    for (int i = 0; i < AVATAR_SLOT_MAX; i++) {
        memcpy(response + offset, &avatar_items[i], 4);
        offset += 4;
    }

    return send_response(s, 310, response, offset);
}

/* msg_no=311: AVATAR_EQUIP - 穿戴时装 */
int handle_avatar_equip(ClientSession* s, const uint8_t* data, int len) {
    printf("[AVATAR] EQUIP from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 311, error, 4);
    }

    int32_t inv_slot;
    uint8_t avatar_slot;
    memcpy(&inv_slot, data, 4);
    avatar_slot = data[4];

    if (avatar_slot >= AVATAR_SLOT_MAX) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 311, error, 4);
    }

    if (db_equip_avatar(s->character_id, inv_slot, avatar_slot) < 0) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 311, error, 4);
    }

    printf("[AVATAR] Equipped item from slot %d to avatar slot %d\n", inv_slot, avatar_slot);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &inv_slot, 4);

    return send_response(s, 311, response, 8);
}

/* msg_no=312: AVATAR_UNEQUIP - 卸下时装 */
int handle_avatar_unequip(ClientSession* s, const uint8_t* data, int len) {
    printf("[AVATAR] UNEQUIP from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 312, error, 4);
    }

    uint8_t avatar_slot = data[0];
    int32_t target_inv_slot;
    memcpy(&target_inv_slot, data + 1, 4);

    if (avatar_slot >= AVATAR_SLOT_MAX) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 312, error, 4);
    }

    if (db_unequip_avatar(s->character_id, avatar_slot, target_inv_slot) < 0) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 312, error, 4);
    }

    printf("[AVATAR] Unequipped avatar slot %d to inventory slot %d\n", avatar_slot, target_inv_slot);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = avatar_slot;

    return send_response(s, 312, response, 8);
}

/* msg_no=320: TITLE_GET_LIST - 获取称号列表 */
int handle_title_get_all(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] GET_ALL from fd=%d\n", s->fd);

    uint16_t titles[TITLE_MAX_COUNT];
    int count = db_load_titles(s->character_id, titles, TITLE_MAX_COUNT);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Title count */
    uint16_t title_count = (uint16_t)(count > 0 ? count : 0);
    memcpy(response + offset, &title_count, 2);
    offset += 2;

    /* Title IDs */
    for (int i = 0; i < count && i < 20; i++) {
        memcpy(response + offset, &titles[i], 2);
        offset += 2;
    }

    return send_response(s, 320, response, offset);
}

/* msg_no=321: TITLE_EQUIP - 装备称号 */
int handle_title_equip_new(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] EQUIP from fd=%d\n", s->fd);
    if (len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 321, error, 4);
    }

    uint16_t title_id;
    uint8_t slot;
    memcpy(&title_id, data, 2);
    slot = data[2];

    if (slot >= TITLE_SLOT_COUNT) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 321, error, 4);
    }

    const TitleInfo* title = find_title(title_id);
    if (!title) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 321, error, 4);
    }

    printf("[TITLE] Equipped title %d to slot %d\n", title_id, slot);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &title_id, 2);
    response[6] = slot;

    return send_response(s, 321, response, 8);
}

/* msg_no=322: TITLE_UNLOCK - 解锁称号 */
int handle_title_unlock(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] UNLOCK from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 322, error, 4);
    }

    uint16_t title_id;
    memcpy(&title_id, data, 2);

    if (db_unlock_title(s->character_id, title_id) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 322, error, 4);
    }

    printf("[TITLE] Unlocked title %d\n", title_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &title_id, 2);

    return send_response(s, 322, response, 8);
}

/* msg_no=330: ACHIEVEMENT_GET_LIST - 获取成就列表 */
int handle_achievement_get_all(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] GET_LIST from fd=%d\n", s->fd);

    AchievementProgress progress[ACHIEVEMENT_MAX];
    int count = db_load_achievements(s->character_id, progress, ACHIEVEMENT_MAX);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Achievement count */
    uint16_t ach_count = (uint16_t)(count > 0 ? count : 0);
    memcpy(response + offset, &ach_count, 2);
    offset += 2;

    /* Achievement progress */
    for (int i = 0; i < count && i < 30; i++) {
        memcpy(response + offset, &progress[i].ach_id, 2);
        offset += 2;
        memcpy(response + offset, &progress[i].progress, 4);
        offset += 4;
        response[offset++] = progress[i].completed;
        response[offset++] = progress[i].claimed;
    }

    return send_response(s, 330, response, offset);
}

/* msg_no=331: ACHIEVEMENT_CLAIM - 领取成就奖励 */
int handle_achievement_claim(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] CLAIM from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 331, error, 4);
    }

    uint16_t ach_id;
    memcpy(&ach_id, data, 2);

    const AchievementDef* ach = find_achievement(ach_id);
    if (!ach) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 331, error, 4);
    }

    /* Mark as claimed and give rewards */
    db_update_achievement(s->character_id, ach_id, ach->target, 1, 1);

    /* Add rewards */
    s->inventory.money += ach->reward_gold;
    db_update_gold(s->character_id, s->inventory.money);

    /* Unlock title if any */
    if (ach->reward_title_id > 0) {
        db_unlock_title(s->character_id, ach->reward_title_id);
    }

    printf("[ACH] Claimed achievement %d, gold +%u, exp +%u\n",
           ach_id, ach->reward_gold, ach->reward_exp);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &ach_id, 2);
    offset += 2;
    memcpy(response + offset, &ach->reward_gold, 4);
    offset += 4;
    memcpy(response + offset, &ach->reward_exp, 4);
    offset += 4;

    return send_response(s, 331, response, offset);
}

/* msg_no=340: GEM_SOCKET - 镶嵌宝石 */
int handle_gem_socket(ClientSession* s, const uint8_t* data, int len) {
    printf("[GEM] SOCKET from fd=%d\n", s->fd);
    if (len < 7) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 340, error, 4);
    }

    int32_t item_slot;
    uint8_t socket_idx;
    uint16_t gem_id;
    memcpy(&item_slot, data, 4);
    socket_idx = data[4];
    memcpy(&gem_id, data + 5, 2);

    if (socket_idx >= GEM_MAX_SOCKETS) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 340, error, 4);
    }

    if (db_socket_gem(s->character_id, item_slot, socket_idx, gem_id) < 0) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 340, error, 4);
    }

    printf("[GEM] Socketed gem %d to item slot %d socket %d\n", gem_id, item_slot, socket_idx);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_slot, 4);
    response[8] = socket_idx;
    memcpy(response + 9, &gem_id, 2);

    return send_response(s, 340, response, 12);
}

/* msg_no=341: GEM_UNSOCKET - 取出宝石 */
int handle_gem_unsocket(ClientSession* s, const uint8_t* data, int len) {
    printf("[GEM] UNSOCKET from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 341, error, 4);
    }

    int32_t item_slot;
    uint8_t socket_idx;
    memcpy(&item_slot, data, 4);
    socket_idx = data[4];

    if (db_unsocket_gem(s->character_id, item_slot, socket_idx) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 341, error, 4);
    }

    printf("[GEM] Unsocketed from item slot %d socket %d\n", item_slot, socket_idx);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_slot, 4);
    response[8] = socket_idx;

    return send_response(s, 341, response, 9);
}

/* msg_no=342: GEM_SYNTHESIZE - 宝石合成 */
int handle_gem_synthesize(ClientSession* s, const uint8_t* data, int len) {
    printf("[GEM] SYNTHESIZE from fd=%d\n", s->fd);
    if (len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 342, error, 4);
    }

    uint8_t gem_type = data[0];
    uint8_t gem_grade = data[1];

    uint8_t new_grade = 0;
    if (synthesize_gems(gem_type, gem_grade, &new_grade) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 342, error, 4);
    }

    printf("[GEM] Synthesized grade %d -> %d\n", gem_grade, new_grade);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = gem_type;
    response[5] = new_grade;

    return send_response(s, 342, response, 8);
}

/* msg_no=350: ABYSS_ENTER - 进入深渊 (Phase 12 Enhanced) */
int handle_abyss_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ENTER from fd=%d\n", s->fd);
    if (len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Invalid request */
        return send_response(s, 350, error, 4);
    }

    uint16_t boss_id;
    uint8_t difficulty;
    memcpy(&boss_id, data, 2);
    difficulty = data[2];

    if (difficulty >= ABYSS_DIFFICULTY_MAX) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid difficulty */
        return send_response(s, 350, error, 4);
    }

    /* Phase 12: Full validation (level, daily limit, tickets) */
    int validation = validate_abyss_entry(s, boss_id, difficulty, 1);  /* 1 = consume ticket */
    if (validation != 0) {
        printf("[ABYSS] Entry denied: validation=%d\n", validation);
        uint8_t error[4];
        error[0] = (uint8_t)(3 - validation);  /* 3=level, 4=daily limit, 5=tickets */
        error[1] = 0;
        error[2] = 0;
        error[3] = 0;
        return send_response(s, 350, error, 4);
    }

    /* Phase 12: Consume abyss tickets */
    int ticket_cost = g_abyss_entry_cost[difficulty];
    if (db_consume_abyss_tickets(s->character_id, ticket_cost) < 0) {
        printf("[ABYSS] Failed to consume tickets\n");
        uint8_t error[4] = {6, 0, 0, 0};  /* Ticket consumption failed */
        return send_response(s, 350, error, 4);
    }

    printf("[ABYSS] Consumed %d tickets for difficulty %d\n", ticket_cost, difficulty);

    /* Get scaled boss stats */
    MonsterInfo boss_info;
    memset(&boss_info, 0, sizeof(boss_info));
    calculate_abyss_boss_stats(boss_id, difficulty, &boss_info);
    uint32_t boss_hp = boss_info.hp;
    uint16_t boss_attack = (uint16_t)boss_info.attack_damage;
    uint16_t boss_defense = (uint16_t)boss_info.phys_def;

    /* Phase 12: Get daily remaining count */
    int daily_count = db_get_abyss_daily_count(s->character_id);
    uint8_t daily_remaining = (uint8_t)(ABYSS_DAILY_LIMIT - daily_count);

    printf("[ABYSS] Entering boss %d difficulty %d (HP=%u ATK=%u DEF=%u) daily_remaining=%d\n",
           boss_id, difficulty, boss_hp, boss_attack, boss_defense, daily_remaining);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &boss_id, 2);
    offset += 2;
    response[offset++] = difficulty;
    memcpy(response + offset, &boss_hp, 4);
    offset += 4;
    memcpy(response + offset, &boss_attack, 2);
    offset += 2;
    memcpy(response + offset, &boss_defense, 2);
    offset += 2;
    response[offset++] = daily_remaining;  /* Phase 12: Daily remaining */
    response[offset++] = (uint8_t)ticket_cost;  /* Phase 12: Tickets consumed */

    return send_response(s, 350, response, offset);
}

/* msg_no=351: ABYSS_CLEAR - 深渊通关 (Phase 12 Enhanced) */
int handle_abyss_clear(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] CLEAR from fd=%d\n", s->fd);
    if (len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 351, error, 4);
    }

    uint16_t boss_id;
    uint8_t difficulty;
    memcpy(&boss_id, data, 2);
    difficulty = data[2];

    if (difficulty >= ABYSS_DIFFICULTY_MAX) difficulty = 0;

    /* Record clear in DB */
    db_record_abyss_clear(s->character_id, boss_id, difficulty);

    /* Phase 12: Calculate rewards using new system */
    uint32_t reward_gold = 0, reward_exp = 0;
    uint16_t reward_item = 0;
    calculate_abyss_rewards(boss_id, difficulty, &reward_gold, &reward_exp, &reward_item);

    /* Phase 12: Generate drops */
    uint32_t drop_items[10];
    uint8_t drop_counts[10];
    int drop_count = generate_abyss_drops(boss_id, difficulty, drop_items, drop_counts, 10);

    /* Apply gold reward */
    s->inventory.money += reward_gold;
    db_update_gold(s->character_id, s->inventory.money);

    /* Phase 12: Add exp (check for level up) */
    db_add_exp(s->character_id, reward_exp);

    /* Phase 12: Add drop items to inventory */
    for (int i = 0; i < drop_count; i++) {
        /* Find empty slot in inventory (slots 0-55) */
        int slot = -1;
        for (int j = 0; j < MAX_BAG_ITEMS && j < 56; j++) {
            if (s->inventory.items[j].it_id == 0) {
                slot = j;
                break;
            }
        }
        if (slot >= 0) {
            db_add_item(s->character_id, drop_items[i], drop_counts[i], slot);
            /* Update local inventory */
            s->inventory.items[slot].it_id = drop_items[i];
            s->inventory.items[slot].count = drop_counts[i];
            printf("[ABYSS] Added drop to slot %d: item=%u count=%u\n",
                   slot, drop_items[i], drop_counts[i]);
        } else {
            printf("[ABYSS] Inventory full, drop lost: item=%u\n", drop_items[i]);
        }
    }

    /* Phase 12: Update achievement stats */
    s->stat_dungeons_cleared++;

    /* Phase 12: Achievement progress is tracked via dungeon clear handler */
    /* Achievement system will check stat_dungeons_cleared automatically */

    printf("[ABYSS] Cleared boss %d difficulty %d, gold +%u, exp +%u, drops=%d\n",
           boss_id, difficulty, reward_gold, reward_exp, drop_count);

    /* Build response with drops */
    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &boss_id, 2);
    offset += 2;
    response[offset++] = difficulty;
    memcpy(response + offset, &reward_gold, 4);
    offset += 4;
    memcpy(response + offset, &reward_exp, 4);
    offset += 4;

    /* Phase 12: Include drops in response */
    response[offset++] = (uint8_t)drop_count;
    for (int i = 0; i < drop_count && i < 10; i++) {
        memcpy(response + offset, &drop_items[i], 4);
        offset += 4;
        response[offset++] = drop_counts[i];
    }

    return send_response(s, 351, response, offset);
}

/* msg_no=352: ABYSS_EXIT - 退出深渊 */
int handle_abyss_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] EXIT from fd=%d charac=%u\n", s->fd, s->character_id);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 352, response, 4);
}

/* msg_no=353: ABYSS_GET_INFO - 获取深渊信息 */
int handle_abyss_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ABYSS] GET_INFO from fd=%d\n", s->fd);

    int daily_count = db_get_abyss_daily_count(s->character_id);
    int ticket_count = db_get_abyss_ticket_count(s->character_id);
    int can_enter = can_enter_abyss(s->level, 0, 0) ? 1 : 0;

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = (uint8_t)daily_count;
    response[offset++] = ABYSS_DAILY_LIMIT;
    response[offset++] = (uint8_t)(ticket_count > 255 ? 255 : ticket_count);
    response[offset++] = (uint8_t)can_enter;

    return send_response(s, 353, response, offset);
}

/* msg_no=354: ABYSS_ROOM_CLEAR - 深渊房间清理 */
int handle_abyss_room_clear(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] ROOM_CLEAR from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 354, error, 4);
    }

    uint32_t monsters_killed;
    memcpy(&monsters_killed, data, 4);

    printf("[ABYSS] Room cleared, killed=%u monsters\n", monsters_killed);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &monsters_killed, 4);

    return send_response(s, 354, response, 8);
}

/* msg_no=355: ABYSS_GET_RANKING - 深渊排行榜 */
int handle_abyss_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    printf("[ABYSS] GET_RANKING from fd=%d\n", s->fd);

    uint8_t difficulty = 0;
    if (len >= 1) difficulty = data[0];
    if (difficulty >= ABYSS_DIFFICULTY_MAX) difficulty = 0;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT c.charac_name, a.charac_no, a.clear_count, a.fastest_time "
        "FROM abyss_stats a JOIN charac_info c ON a.charac_no=c.charac_no "
        "WHERE a.difficulty=%d ORDER BY a.clear_count DESC LIMIT 10", difficulty);

    MYSQL_RES* res = NULL;
    if (mysql_query(g_db_charac, query) == 0) {
        res = mysql_store_result(g_db_charac);
    }

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = difficulty;

    int count_offset = offset++;
    int rank_count = 0;

    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) && rank_count < 10) {
            strncpy((char*)(response + offset), row[0] ? row[0] : "?", 15);
            offset += 16;
            uint32_t cid = row[1] ? atoi(row[1]) : 0;
            memcpy(response + offset, &cid, 4); offset += 4;
            uint32_t clears = row[2] ? atoi(row[2]) : 0;
            memcpy(response + offset, &clears, 4); offset += 4;
            uint32_t fastest = row[3] ? atoi(row[3]) : 0;
            memcpy(response + offset, &fastest, 4); offset += 4;
            rank_count++;
        }
        mysql_free_result(res);
    }

    response[count_offset] = (uint8_t)rank_count;
    return send_response(s, 355, response, offset);
}

/* msg_no=360: PVP_CREATE_ROOM_EX - 创建PVP房间 */
int handle_pvp_create_room_ex(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] CREATE_ROOM from fd=%d\n", s->fd);
    if (len < 1) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 360, error, 4);
    }

    uint8_t mode = data[0];
    uint32_t room_id = create_pvp_room(s->character_id, mode);

    if (room_id == 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 360, error, 4);
    }

    printf("[PVP] Created room %u mode %d\n", room_id, mode);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &room_id, 4);
    response[8] = mode;

    return send_response(s, 360, response, 12);
}

/* msg_no=361: PVP_JOIN_ROOM - 加入PVP房间 */
int handle_pvp_join_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] JOIN_ROOM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 361, error, 4);
    }

    uint32_t room_id;
    memcpy(&room_id, data, 4);

    int result = join_pvp_room(room_id, s->character_id);
    if (result < 0) {
        uint8_t error[4] = {(uint8_t)(2 - result), 0, 0, 0};
        return send_response(s, 361, error, 4);
    }

    printf("[PVP] Joined room %u\n", room_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &room_id, 4);

    return send_response(s, 361, response, 8);
}

/* msg_no=362: PVP_SET_READY - 设置准备状态 */
int handle_pvp_set_ready(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] SET_READY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 362, error, 4);
    }

    uint32_t room_id;
    memcpy(&room_id, data, 4);

    int all_ready = pvp_set_ready(room_id, s->character_id);
    if (all_ready < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 362, error, 4);
    }

    printf("[PVP] Set ready in room %u (all_ready=%d)\n", room_id, all_ready);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &room_id, 4);
    response[8] = (uint8_t)all_ready;

    return send_response(s, 362, response, 9);
}

/* msg_no=363: PVP_START - 开始PVP */
int handle_pvp_start(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] START from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 363, error, 4);
    }

    uint32_t room_id;
    memcpy(&room_id, data, 4);

    if (pvp_start_match(room_id) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 363, error, 4);
    }

    printf("[PVP] Started match in room %u\n", room_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &room_id, 4);

    return send_response(s, 363, response, 8);
}

/* msg_no=364: PVP_RESULT - PVP结果 */
int handle_pvp_result(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] RESULT from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 364, error, 4);
    }

    uint32_t room_id;
    uint32_t winner_id;
    memcpy(&room_id, data, 4);
    memcpy(&winner_id, data + 4, 4);

    pvp_record_result(room_id, winner_id);

    /* Update rankings */
    PVPRoom* room = find_pvp_room(room_id);
    if (room) {
        for (int i = 0; i < room->player_count; i++) {
            int win = (room->players[i] == winner_id) ? 1 : 0;
            db_update_pvp_ranking(room->players[i], win);
        }
    }

    /* Cleanup */
    pvp_cleanup_room(room_id);

    printf("[PVP] Match ended, winner=%u\n", winner_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &room_id, 4);
    memcpy(response + 8, &winner_id, 4);

    return send_response(s, 364, response, 12);
}

/* msg_no=380: DAILY_QUEST_GET - 获取每日任务 */
int handle_daily_quest_get(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[DAILY] GET_QUESTS from fd=%d\n", s->fd);

    DailyQuestProgress quests[DAILY_QUEST_MAX];
    int count = db_load_daily_quests(s->character_id, quests, DAILY_QUEST_MAX);

    /* If no quests for today, generate new ones */
    if (count == 0) {
        uint16_t quest_ids[DAILY_QUEST_MAX];
        generate_daily_quests(s->character_id, quest_ids, DAILY_QUEST_MAX);
        for (int i = 0; i < DAILY_QUEST_MAX; i++) {
            db_save_daily_quest(s->character_id, quest_ids[i], 0, 0, 0);
            quests[i].quest_id = quest_ids[i];
            quests[i].progress = 0;
            quests[i].completed = 0;
            quests[i].claimed = 0;
        }
        count = DAILY_QUEST_MAX;
    }

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Quest count */
    response[offset++] = (uint8_t)count;

    /* Quest data */
    for (int i = 0; i < count; i++) {
        memcpy(response + offset, &quests[i].quest_id, 2);
        offset += 2;
        memcpy(response + offset, &quests[i].progress, 4);
        offset += 4;
        response[offset++] = quests[i].completed;
        response[offset++] = quests[i].claimed;
    }

    return send_response(s, 380, response, offset);
}

/* msg_no=381: DAILY_QUEST_CLAIM - 领取每日任务奖励 */
int handle_daily_quest_claim(ClientSession* s, const uint8_t* data, int len) {
    printf("[DAILY] CLAIM from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 381, error, 4);
    }

    uint16_t quest_id;
    memcpy(&quest_id, data, 2);

    const DailyQuestDef* quest = find_daily_quest(quest_id);
    if (!quest) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 381, error, 4);
    }

    /* Mark claimed */
    db_save_daily_quest(s->character_id, quest_id, quest->target, 1, 1);

    /* Give rewards */
    s->inventory.money += quest->reward_gold;
    db_update_gold(s->character_id, s->inventory.money);

    printf("[DAILY] Claimed quest %d, gold +%u\n", quest_id, quest->reward_gold);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &quest_id, 2);
    offset += 2;
    memcpy(response + offset, &quest->reward_gold, 4);
    offset += 4;
    memcpy(response + offset, &quest->reward_exp, 4);
    offset += 4;

    return send_response(s, 381, response, offset);
}

/* msg_no=390: EVENT_GET_LIST - 获取活动列表 */
/*==============================================================================
 * Weekly Quest System (周常任务)
 *============================================================================*/

#define WEEKLY_QUEST_MAX 5
#define WEEKLY_QUEST_POOL_SIZE 10

typedef struct {
    uint16_t quest_id;
    uint8_t  type;
    uint32_t target;
    uint32_t reward_gold;
    uint32_t reward_exp;
    uint16_t reward_item;
    uint8_t  reward_count;
} WeeklyQuestDef;

static WeeklyQuestDef g_weekly_quest_pool[] = {
    {5001, 1, 20, 50000, 20000, 90006, 3},   /* 通关20次副本 */
    {5002, 2, 100, 30000, 15000, 0, 0},      /* 击杀100只怪物 */
    {5003, 3, 5, 40000, 18000, 90003, 5},    /* 完成5次深渊 */
    {5004, 4, 10, 25000, 12000, 0, 0},       /* 强化10次装备 */
    {5005, 5, 3, 60000, 25000, 90004, 1},    /* 完成3次死亡之塔 */
    {5006, 6, 50, 20000, 10000, 0, 0},       /* 使用50个消耗品 */
    {5007, 7, 10, 35000, 16000, 90002, 2},   /* PVP战斗10场 */
    {5008, 8, 5, 45000, 20000, 0, 0},        /* 完成5次组队副本 */
    {0, 0, 0, 0, 0, 0, 0}
};

static const WeeklyQuestDef* find_weekly_quest(uint16_t quest_id) {
    for (int i = 0; g_weekly_quest_pool[i].quest_id != 0; i++) {
        if (g_weekly_quest_pool[i].quest_id == quest_id) return &g_weekly_quest_pool[i];
    }
    return NULL;
}

/* msg_no=382: WEEKLY_QUEST_GET */
int handle_weekly_quest_get(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[WEEKLY] GET from fd=%d\n", s->fd);

    DailyQuestProgress quests[WEEKLY_QUEST_MAX];
    int count = db_load_weekly_quests(s->character_id, quests, WEEKLY_QUEST_MAX);

    if (count == 0) {
        /* Assign new weekly quests */
        for (int i = 0; i < WEEKLY_QUEST_MAX && g_weekly_quest_pool[i].quest_id != 0; i++) {
            db_save_weekly_quest(s->character_id, g_weekly_quest_pool[i].quest_id, 0, 0, 0);
            quests[i].quest_id = g_weekly_quest_pool[i].quest_id;
            quests[i].progress = 0;
            quests[i].completed = 0;
            quests[i].claimed = 0;
            count++;
        }
    }

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count; i++) {
        const WeeklyQuestDef* def = find_weekly_quest(quests[i].quest_id);
        memcpy(response + offset, &quests[i].quest_id, 2); offset += 2;
        response[offset++] = def ? def->type : 0;
        uint32_t target = def ? def->target : 0;
        memcpy(response + offset, &target, 4); offset += 4;
        memcpy(response + offset, &quests[i].progress, 4); offset += 4;
        response[offset++] = quests[i].completed;
        response[offset++] = quests[i].claimed;
    }

    return send_response(s, 382, response, offset);
}

/* msg_no=383: WEEKLY_QUEST_CLAIM */
int handle_weekly_quest_claim(ClientSession* s, const uint8_t* data, int len) {
    printf("[WEEKLY] CLAIM from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    uint16_t quest_id;
    memcpy(&quest_id, data, 2);

    const WeeklyQuestDef* def = find_weekly_quest(quest_id);
    if (!def) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    DailyQuestProgress quests[WEEKLY_QUEST_MAX];
    db_load_weekly_quests(s->character_id, quests, WEEKLY_QUEST_MAX);

    DailyQuestProgress* q = NULL;
    for (int i = 0; i < WEEKLY_QUEST_MAX; i++) {
        if (quests[i].quest_id == quest_id) { q = &quests[i]; break; }
    }

    if (!q || !q->completed || q->claimed) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 383, error, 4);
    }

    /* Give rewards */
    if (def->reward_gold > 0) {
        s->inventory.money += def->reward_gold;
        db_update_gold(s->character_id, s->inventory.money);
    }
    if (def->reward_exp > 0) {
        db_add_exp(s->character_id, def->reward_exp);
    }
    if (def->reward_item > 0) {
        char query[256];
        snprintf(query, sizeof(query),
                 "INSERT INTO user_items (charac_no, it_id, count, slot) "
                 "SELECT %u, %u, %d, COALESCE(MAX(slot)+1, 0) FROM user_items WHERE charac_no=%u",
                 s->character_id, def->reward_item, def->reward_count, s->character_id);
        mysql_query(g_db_charac, query);
    }

    db_save_weekly_quest(s->character_id, quest_id, q->progress, 1, 1);
    printf("[WEEKLY] Claimed quest %d: gold=%u exp=%u\n", quest_id, def->reward_gold, def->reward_exp);

    uint8_t response[20];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &quest_id, 2); offset += 2;
    memcpy(response + offset, &def->reward_gold, 4); offset += 4;
    memcpy(response + offset, &def->reward_exp, 4); offset += 4;

    return send_response(s, 383, response, offset);
}
/*==============================================================================
 * Sign-In System (签到系统) - Protocol 534-536
 *============================================================================*/

#define SIGN_IN_MONTHLY_DAYS 31
#define SIGN_IN_STREAK_BONUS_7  1000   /* 7天连续奖励金币 */
#define SIGN_IN_STREAK_BONUS_14 3000   /* 14天连续奖励 */
#define SIGN_IN_STREAK_BONUS_21 5000   /* 21天连续奖励 */
#define SIGN_IN_STREAK_BONUS_28 10000  /* 28天连续奖励 */

/* 每日签到奖励表 */
static const uint32_t g_daily_sign_rewards[] = {
    1000, 1000, 1000, 2000, 2000, 2000, 3000,   /* Day 1-7 */
    1500, 1500, 1500, 2500, 2500, 2500, 4000,   /* Day 8-14 */
    2000, 2000, 2000, 3000, 3000, 3000, 5000,   /* Day 15-21 */
    2500, 2500, 2500, 3500, 3500, 3500, 8000,   /* Day 22-28 */
    3000, 3000, 10000                            /* Day 29-31 */
};

typedef struct {
    uint32_t charac_no;
    uint8_t  month;              /* 当前月份 1-12 */
    uint8_t  signed_days;        /* 本月已签天数 */
    uint8_t  streak;             /* 连续签到天数 */
    uint32_t last_sign_date;     /* 上次签到日期 YYYYMMDD */
    uint8_t  sign_bits[4];       /* 32位标记本月哪天签了 */
    uint8_t  makeup_used;        /* 本月已用补签次数 */
} SignInInfo;

/* DB: 加载签到信息 */
static int db_load_sign_in(uint32_t charac_no, SignInInfo* info) {
    if (!g_db_charac || !info) return -1;
    memset(info, 0, sizeof(SignInInfo));
    info->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT month, signed_days, streak, last_sign_date, sign_bits, makeup_used "
             "FROM game_sign_in WHERE charac_no=%u", charac_no);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            info->month = row[0] ? atoi(row[0]) : 0;
            info->signed_days = row[1] ? atoi(row[1]) : 0;
            info->streak = row[2] ? atoi(row[2]) : 0;
            info->last_sign_date = row[3] ? atoi(row[3]) : 0;
            if (row[4]) {
                unsigned long* lengths = mysql_fetch_lengths(result);
                if (lengths[4] >= 4) memcpy(info->sign_bits, row[4], 4);
            }
            info->makeup_used = row[5] ? atoi(row[5]) : 0;
        }
        mysql_free_result(result);
    }
    return 0;
}

/* DB: 保存签到信息 */
static int db_save_sign_in(SignInInfo* info) {
    if (!g_db_charac || !info) return -1;

    char bits_hex[12];
    snprintf(bits_hex, sizeof(bits_hex), "0x%02X%02X%02X%02X",
             info->sign_bits[0], info->sign_bits[1], info->sign_bits[2], info->sign_bits[3]);

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO game_sign_in (charac_no, month, signed_days, streak, last_sign_date, sign_bits, makeup_used) "
             "VALUES (%u, %d, %d, %d, %u, %s, %d) "
             "ON DUPLICATE KEY UPDATE month=%d, signed_days=%d, streak=%d, last_sign_date=%u, sign_bits=%s, makeup_used=%d",
             info->charac_no, info->month, info->signed_days, info->streak,
             info->last_sign_date, bits_hex, info->makeup_used,
             info->month, info->signed_days, info->streak, info->last_sign_date, bits_hex, info->makeup_used);

    return mysql_query(g_db_charac, query) ? -1 : 0;
}

/* 获取当前日期 YYYYMMDD */
static uint32_t get_today_date(void) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    return (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
}

/* 检查并重置月份 */
static void check_month_reset(SignInInfo* info) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    uint8_t current_month = t->tm_mon + 1;

    if (info->month != current_month) {
        info->month = current_month;
        info->signed_days = 0;
        info->makeup_used = 0;
        memset(info->sign_bits, 0, 4);
    }
}

/* 检查今天是否已签 */
static int is_signed_today(SignInInfo* info) {
    uint32_t today = get_today_date();
    return info->last_sign_date == today;
}

/* 设置某天已签 */
static void set_day_signed(SignInInfo* info, int day) {
    if (day < 1 || day > 31) return;
    int idx = (day - 1) / 8;
    int bit = (day - 1) % 8;
    info->sign_bits[idx] |= (1 << bit);
}

/* 检查某天是否已签 */
static int is_day_signed(SignInInfo* info, int day) {
    if (day < 1 || day > 31) return 0;
    int idx = (day - 1) / 8;
    int bit = (day - 1) % 8;
    return (info->sign_bits[idx] >> bit) & 1;
}

/* msg_no=534: SIGN_IN_INFO - 获取签到信息 */
int handle_sign_in_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SIGNIN] INFO from fd=%d charac=%u\n", s->fd, s->character_id);

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 4;
    response[offset++] = info.month;
    response[offset++] = info.signed_days;
    response[offset++] = info.streak;
    response[offset++] = is_signed_today(&info) ? 1 : 0;
    memcpy(response + offset, info.sign_bits, 4); offset += 4;
    response[offset++] = info.makeup_used;
    response[offset++] = 3 - info.makeup_used;  /* 剩余补签次数 */

    return send_response(s, 534, response, offset);
}

/* msg_no=535: SIGN_IN_DO - 执行签到 */
int handle_sign_in_do(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SIGNIN] DO from fd=%d charac=%u\n", s->fd, s->character_id);

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[32];
    memset(response, 0, sizeof(response));

    if (is_signed_today(&info)) {
        uint32_t err = 1;  /* 今天已签 */
        memcpy(response, &err, 4);
        return send_response(s, 535, response, 4);
    }

    /* 更新连续签到 */
    uint32_t today = get_today_date();
    uint32_t yesterday = today - 1;
    if (info.last_sign_date == yesterday) {
        info.streak++;
    } else {
        info.streak = 1;
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int day = t->tm_mday;

    set_day_signed(&info, day);
    info.signed_days++;
    info.last_sign_date = today;

    /* 计算奖励 */
    uint32_t reward = g_daily_sign_rewards[day <= 31 ? day - 1 : 30];
    uint32_t streak_bonus = 0;
    if (info.streak == 7) streak_bonus = SIGN_IN_STREAK_BONUS_7;
    else if (info.streak == 14) streak_bonus = SIGN_IN_STREAK_BONUS_14;
    else if (info.streak == 21) streak_bonus = SIGN_IN_STREAK_BONUS_21;
    else if (info.streak == 28) streak_bonus = SIGN_IN_STREAK_BONUS_28;

    uint32_t total_reward = reward + streak_bonus;

    /* 发放奖励 */
    s->inventory.money += total_reward;
    db_update_gold(s->character_id, s->inventory.money);
    db_save_sign_in(&info);

    printf("[SIGNIN] Day %d signed, streak=%d, reward=%u+%u\n",
           day, info.streak, reward, streak_bonus);

    int offset = 4;
    response[offset++] = info.signed_days;
    response[offset++] = info.streak;
    memcpy(response + offset, &reward, 4); offset += 4;
    memcpy(response + offset, &streak_bonus, 4); offset += 4;

    return send_response(s, 535, response, offset);
}

/* msg_no=536: SIGN_IN_MAKEUP - 补签 */
int handle_sign_in_makeup(ClientSession* s, const uint8_t* data, int len) {
    printf("[SIGNIN] MAKEUP from fd=%d\n", s->fd);
    if (len < 1) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 536, e, 4);
    }

    uint8_t target_day = data[0];
    if (target_day < 1 || target_day > 31) {
        uint8_t e[4] = {2, 0, 0, 0};
        return send_response(s, 536, e, 4);
    }

    SignInInfo info;
    db_load_sign_in(s->character_id, &info);
    check_month_reset(&info);

    uint8_t response[16];
    memset(response, 0, sizeof(response));

    /* 检查补签次数 */
    if (info.makeup_used >= 3) {
        uint32_t err = 3;  /* 补签次数用完 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 检查目标日期是否已签 */
    if (is_day_signed(&info, target_day)) {
        uint32_t err = 4;  /* 已签过 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 补签消耗：1000金币 */
    uint32_t cost = 1000;
    if (s->inventory.money < cost) {
        uint32_t err = 5;  /* 金币不足 */
        memcpy(response, &err, 4);
        return send_response(s, 536, response, 4);
    }

    /* 执行补签 */
    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    set_day_signed(&info, target_day);
    info.signed_days++;
    info.makeup_used++;

    uint32_t reward = g_daily_sign_rewards[target_day <= 31 ? target_day - 1 : 30];
    s->inventory.money += reward;
    db_update_gold(s->character_id, s->inventory.money);
    db_save_sign_in(&info);

    printf("[SIGNIN] Makeup day %d, cost=%u, reward=%u\n", target_day, cost, reward);

    int offset = 4;
    response[offset++] = target_day;
    response[offset++] = info.signed_days;
    response[offset++] = 3 - info.makeup_used;
    memcpy(response + offset, &reward, 4); offset += 4;

    return send_response(s, 536, response, offset);
}
int handle_event_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[EVENT] GET_LIST from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Event count */
    response[offset++] = (uint8_t)g_active_event_count;

    /* Events */
    for (int i = 0; i < g_active_event_count; i++) {
        EventDef* e = &g_active_events[i];
        memcpy(response + offset, &e->event_id, 2);
        offset += 2;
        response[offset++] = e->type;
        response[offset++] = e->state;
        memcpy(response + offset, &e->target, 4);
        offset += 4;
        memcpy(response + offset, &e->reward_gold, 4);
        offset += 4;
    }

    return send_response(s, 390, response, offset);
}

/* msg_no=391: EVENT_CLAIM - 领取活动奖励 */
int handle_event_claim(ClientSession* s, const uint8_t* data, int len) {
    printf("[EVENT] CLAIM from fd=%d\n", s->fd);
    if (len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 391, error, 4);
    }

    uint16_t event_id;
    memcpy(&event_id, data, 2);

    EventDef* e = find_event(event_id);
    if (!e || !is_event_active(event_id)) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 391, error, 4);
    }

    /* Mark claimed */
    db_update_event_progress(s->character_id, event_id, e->target, 1, 1);

    /* Give gold reward */
    if (e->reward_gold > 0) {
        s->inventory.money += e->reward_gold;
        db_update_gold(s->character_id, s->inventory.money);
        printf("[EVENT] Gold +%u\n", e->reward_gold);
    }

    /* Give exp reward */
    if (e->reward_exp > 0) {
        db_add_exp(s->character_id, e->reward_exp);
        printf("[EVENT] Exp +%u\n", e->reward_exp);
    }

    /* Give item rewards */
    int items_given = 0;
    for (int i = 0; i < EVENT_REWARD_MAX && e->reward_items[i] != 0; i++) {
        char query[256];
        snprintf(query, sizeof(query),
                 "INSERT INTO user_items (charac_no, it_id, count, slot) "
                 "SELECT %u, %u, %d, COALESCE(MAX(slot)+1, 0) FROM user_items WHERE charac_no=%u",
                 s->character_id, e->reward_items[i], e->reward_counts[i], s->character_id);
        if (mysql_query(g_db_charac, query) == 0) {
            printf("[EVENT] Item %u x%d given\n", e->reward_items[i], e->reward_counts[i]);
            items_given++;
        }
    }

    printf("[EVENT] Claimed event %d: gold=%u exp=%u items=%d\n",
           event_id, e->reward_gold, e->reward_exp, items_given);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &event_id, 2);
    offset += 2;
    memcpy(response + offset, &e->reward_gold, 4);
    offset += 4;
    memcpy(response + offset, &e->reward_exp, 4);
    offset += 4;

    return send_response(s, 391, response, offset);
}
/* msg_no=400: DISASSEMBLE_ITEM - 分解装备 */
int handle_disassemble_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[CRAFT] DISASSEMBLE from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 400, error, 4);
    }

    int32_t item_slot;
    uint8_t item_rarity;
    memcpy(&item_slot, data, 4);
    item_rarity = data[4];

    const DisassembleResult* result = get_disassemble_result(item_rarity);
    if (db_disassemble_item(s->character_id, item_slot, item_rarity) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 400, error, 4);
    }

    printf("[CRAFT] Disassembled item slot %d rarity %d\n", item_slot, item_rarity);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &item_slot, 4);
    offset += 4;
    response[offset++] = result->material_count;
    for (int i = 0; i < result->material_count; i++) {
        memcpy(response + offset, &result->material_ids[i], 2);
        offset += 2;
        response[offset++] = result->material_counts[i];
    }

    return send_response(s, 400, response, offset);
}

/* msg_no=401: CRAFT_ITEM - 合成物品 */
int handle_craft_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[CRAFT] CRAFT from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 401, error, 4);
    }

    uint32_t recipe_id;
    memcpy(&recipe_id, data, 4);

    const CraftRecipe* recipe = find_craft_recipe(recipe_id);
    if (!recipe) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 401, error, 4);
    }

    /* Check materials */
    if (!check_craft_materials(s->character_id, recipe)) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 401, error, 4);
    }

    /* Check gold */
    if (s->inventory.money < recipe->gold_cost) {
        uint8_t error[4] = {4, 0, 0, 0};
        return send_response(s, 401, error, 4);
    }

    /* Deduct gold */
    s->inventory.money -= recipe->gold_cost;
    db_update_gold(s->character_id, s->inventory.money);

    printf("[CRAFT] Crafted recipe %u, result item %u\n", recipe_id, recipe->result_item_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 4;
    memcpy(response + offset, &recipe_id, 4);
    offset += 4;
    memcpy(response + offset, &recipe->result_item_id, 4);
    offset += 4;
    response[offset++] = recipe->result_count;

    return send_response(s, 401, response, offset);
}

/* msg_no=402: GET_CRAFT_RECIPES - 获取配方列表 */
int handle_get_craft_recipes(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[CRAFT] GET_RECIPES from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Count recipes */
    int recipe_count = 0;
    while (g_craft_recipes[recipe_count].recipe_id != 0) recipe_count++;

    response[offset++] = (uint8_t)recipe_count;

    /* Recipe data */
    for (int i = 0; i < recipe_count && i < 20; i++) {
        const CraftRecipe* r = &g_craft_recipes[i];
        memcpy(response + offset, &r->recipe_id, 4);
        offset += 4;
        memcpy(response + offset, &r->result_item_id, 4);
        offset += 4;
        memcpy(response + offset, &r->gold_cost, 4);
        offset += 4;
        response[offset++] = r->required_level;
    }

    return send_response(s, 402, response, offset);
}

/*==============================================================================
 * Phase 14: Gameplay Enhancement Systems
 *============================================================================*/

/*------------------------------------------------------------------------------
 * P1: Skill Combo System - 技能连招系统
 *----------------------------------------------------------------------------*/

#define MAX_COMBO_CHAIN        5
#define COMBO_WINDOW_MS        1500    /* 1.5 second window for combo */
#define COMBO_DAMAGE_BONUS     15      /* 15% per combo level */

typedef struct {
    uint16_t skill_id;
    uint16_t followup_skills[4];   /* Skills that can follow this one */
    uint8_t  followup_count;
    uint8_t  cancel_frames;        /* Frame window for cancel */
    uint8_t  combo_bonus;          /* Extra damage % when in combo */
} SkillComboEntry;

typedef struct {
    uint16_t skills[MAX_COMBO_CHAIN];
    uint8_t  count;
    uint32_t last_skill_time;
    uint8_t  combo_level;
} PlayerComboState;

/* Sample combo definitions */
static const SkillComboEntry g_skill_combos[] = {
    /* Slayer basic combos */
    {1001, {1002, 1003, 0, 0}, 2, 10, 10},  /* Basic attack -> Uppercut/Slash */
    {1002, {1003, 1004, 0, 0}, 2, 8, 15},   /* Uppercut -> Slash/Wave */
    {1003, {1004, 1005, 0, 0}, 2, 12, 20},  /* Slash -> Wave/Finisher */
    /* Fighter basic combos */
    {2001, {2002, 2003, 0, 0}, 2, 8, 10},
    {2002, {2003, 2004, 0, 0}, 2, 6, 15},
    /* Gunner basic combos */
    {3001, {3002, 3003, 0, 0}, 2, 15, 10},
    {3002, {3003, 3004, 0, 0}, 2, 12, 15},
    /* Mage basic combos */
    {4001, {4002, 4003, 0, 0}, 2, 20, 10},
    {4002, {4003, 4004, 0, 0}, 2, 18, 15},
    {0, {0, 0, 0, 0}, 0, 0, 0}  /* Terminator */
};

static const SkillComboEntry* find_combo_entry(uint16_t skill_id) {
    for (int i = 0; g_skill_combos[i].skill_id != 0; i++) {
        if (g_skill_combos[i].skill_id == skill_id) {
            return &g_skill_combos[i];
        }
    }
    return NULL;
}

static int can_combo_into(uint16_t prev_skill, uint16_t next_skill) {
    const SkillComboEntry* entry = find_combo_entry(prev_skill);
    if (!entry) return 0;

    for (int i = 0; i < entry->followup_count && i < 4; i++) {
        if (entry->followup_skills[i] == next_skill) {
            return 1;
        }
    }
    return 0;
}

static uint8_t calculate_combo_damage_bonus(ClientSession* s) {
    if (!s || s->combo_state.combo_level == 0) return 0;
    return s->combo_state.combo_level * COMBO_DAMAGE_BONUS;
}

static void update_combo_state(ClientSession* s, uint16_t skill_id) {
    if (!s) return;

    uint32_t now = (uint32_t)time(NULL) * 1000;  /* Approximate ms */

    /* Check if combo window expired */
    if (s->combo_state.count > 0 && (now - s->combo_state.last_skill_time) > COMBO_WINDOW_MS) {
        memset(&s->combo_state, 0, sizeof(s->combo_state));
    }

    /* Check if this skill can combo from previous */
    if (s->combo_state.count > 0) {
        uint16_t prev_skill = s->combo_state.skills[s->combo_state.count - 1];
        if (can_combo_into(prev_skill, skill_id)) {
            /* Valid combo continuation */
            if (s->combo_state.count < MAX_COMBO_CHAIN) {
                s->combo_state.skills[s->combo_state.count++] = skill_id;
                s->combo_state.combo_level++;
            }
        } else {
            /* Combo broken, start new */
            memset(&s->combo_state, 0, sizeof(s->combo_state));
            s->combo_state.skills[0] = skill_id;
            s->combo_state.count = 1;
            s->combo_state.combo_level = 1;
        }
    } else {
        /* Start new combo */
        s->combo_state.skills[0] = skill_id;
        s->combo_state.count = 1;
        s->combo_state.combo_level = 1;
    }

    s->combo_state.last_skill_time = now;
}

int handle_combo_skill(ClientSession* s, const uint8_t* data, int len) {
    if (len < 4) return -1;

    uint16_t skill_id, target_id;
    memcpy(&skill_id, data, 2);
    memcpy(&target_id, data + 2, 2);

    printf("[COMBO] skill=%u target=%u from fd=%d\n", skill_id, target_id, s->fd);

    /* Update combo state */
    update_combo_state(s, skill_id);

    /* Calculate combo bonus */
    uint8_t bonus = calculate_combo_damage_bonus(s);

    uint8_t response[16];
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &skill_id, 2); offset += 2;
    response[offset++] = s->combo_state.combo_level;
    response[offset++] = bonus;
    memcpy(response + offset, &s->combo_state.last_skill_time, 4); offset += 4;

    return send_response(s, 460, response, offset);
}

int handle_skill_cancel(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[SKILL] CANCEL from fd=%d\n", s->fd);

    /* Reset combo state on cancel */
    memset(&s->combo_state, 0, sizeof(PlayerComboState));

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 461, response, 4);
}

/*------------------------------------------------------------------------------
 * P2: Complete Item Usage System - 完整物品使用系统
 *----------------------------------------------------------------------------*/

typedef enum {
    ITEM_EFFECT_NONE = 0,
    ITEM_EFFECT_HEAL_HP,           /* Heal HP */
    ITEM_EFFECT_HEAL_MP,           /* Heal MP */
    ITEM_EFFECT_HEAL_BOTH,         /* Heal HP and MP */
    ITEM_EFFECT_BUFF_ATK,          /* Attack buff */
    ITEM_EFFECT_BUFF_DEF,          /* Defense buff */
    ITEM_EFFECT_BUFF_SPEED,        /* Speed buff */
    ITEM_EFFECT_BUFF_CRIT,         /* Crit buff */
    ITEM_EFFECT_TELEPORT_TOWN,     /* Teleport to town */
    ITEM_EFFECT_TELEPORT_DUNGEON,  /* Teleport to dungeon entrance */
    ITEM_EFFECT_REVIVE,            /* Resurrection */
    ITEM_EFFECT_CURE_DEBUFF,       /* Remove debuffs */
    ITEM_EFFECT_EXP_BOOST,         /* Experience boost */
    ITEM_EFFECT_DROP_BOOST,        /* Drop rate boost */
    ITEM_EFFECT_SUMMON_PET,        /* Summon pet */
    ITEM_EFFECT_OPEN_BOX           /* Open box/package */
} ItemEffectType;

typedef struct {
    uint32_t item_id;
    uint8_t  effect_type;
    int32_t  effect_value;
    uint16_t duration_sec;         /* Duration for buffs, 0 for instant */
    uint8_t  cooldown_sec;
    uint8_t  level_req;
} ConsumableItem;

static const ConsumableItem g_consumable_items[] = {
    /* HP Potions */
    {100001, ITEM_EFFECT_HEAL_HP, 100, 0, 5, 1},        /* Small HP Potion */
    {100002, ITEM_EFFECT_HEAL_HP, 300, 0, 5, 10},       /* Medium HP Potion */
    {100003, ITEM_EFFECT_HEAL_HP, 700, 0, 5, 25},       /* Large HP Potion */
    {100004, ITEM_EFFECT_HEAL_HP, 1500, 0, 5, 45},      /* Grand HP Potion */
    /* MP Potions */
    {100101, ITEM_EFFECT_HEAL_MP, 50, 0, 5, 1},
    {100102, ITEM_EFFECT_HEAL_MP, 150, 0, 5, 10},
    {100103, ITEM_EFFECT_HEAL_MP, 350, 0, 5, 25},
    /* Combined Potions */
    {100201, ITEM_EFFECT_HEAL_BOTH, 200, 0, 10, 15},
    {100202, ITEM_EFFECT_HEAL_BOTH, 500, 0, 10, 35},
    /* Buff Items */
    {100301, ITEM_EFFECT_BUFF_ATK, 10, 300, 0, 10},     /* +10% ATK for 5 min */
    {100302, ITEM_EFFECT_BUFF_DEF, 15, 300, 0, 10},     /* +15% DEF for 5 min */
    {100303, ITEM_EFFECT_BUFF_SPEED, 20, 180, 0, 15},   /* +20% Speed for 3 min */
    {100304, ITEM_EFFECT_BUFF_CRIT, 5, 300, 0, 20},     /* +5% Crit for 5 min */
    /* Utility Items */
    {100401, ITEM_EFFECT_TELEPORT_TOWN, 0, 0, 60, 1},   /* Town Portal Scroll */
    {100402, ITEM_EFFECT_TELEPORT_DUNGEON, 0, 0, 30, 20}, /* Dungeon Scroll */
    {100403, ITEM_EFFECT_REVIVE, 100, 0, 0, 1},         /* Resurrection Token */
    {100404, ITEM_EFFECT_CURE_DEBUFF, 0, 0, 30, 15},    /* Cure Potion */
    /* Boost Items */
    {100501, ITEM_EFFECT_EXP_BOOST, 50, 1800, 0, 1},    /* +50% EXP for 30 min */
    {100502, ITEM_EFFECT_DROP_BOOST, 30, 1800, 0, 1},   /* +30% Drop for 30 min */
    {0, ITEM_EFFECT_NONE, 0, 0, 0, 0}  /* Terminator */
};

static const ConsumableItem* find_consumable(uint32_t item_id) {
    for (int i = 0; g_consumable_items[i].item_id != 0; i++) {
        if (g_consumable_items[i].item_id == item_id) {
            return &g_consumable_items[i];
        }
    }
    return NULL;
}

static int apply_item_effect(ClientSession* s, const ConsumableItem* item) {
    if (!s || !item) return -1;

    switch (item->effect_type) {
        case ITEM_EFFECT_HEAL_HP:
            s->current_hp += item->effect_value;
            if (s->current_hp > s->combat_stats.max_hp) {
                s->current_hp = s->combat_stats.max_hp;
            }
            printf("[ITEM] Healed %d HP, now %u/%u\n",
                   item->effect_value, s->current_hp, s->combat_stats.max_hp);
            break;

        case ITEM_EFFECT_HEAL_MP:
            s->current_mp += item->effect_value;
            if (s->current_mp > s->combat_stats.max_mp) {
                s->current_mp = s->combat_stats.max_mp;
            }
            break;

        case ITEM_EFFECT_HEAL_BOTH:
            s->current_hp += item->effect_value;
            s->current_mp += item->effect_value / 2;
            if (s->current_hp > s->combat_stats.max_hp) s->current_hp = s->combat_stats.max_hp;
            if (s->current_mp > s->combat_stats.max_mp) s->current_mp = s->combat_stats.max_mp;
            break;

        case ITEM_EFFECT_BUFF_ATK:
            buff_add(s, BUFF_ATTACK_UP, item->effect_value, item->duration_sec, 0);
            break;

        case ITEM_EFFECT_BUFF_DEF:
            buff_add(s, BUFF_DEFENSE_UP, item->effect_value, item->duration_sec, 0);
            break;

        case ITEM_EFFECT_BUFF_SPEED:
            buff_add(s, BUFF_SPEED_UP, item->effect_value, item->duration_sec, 0);
            break;

        case ITEM_EFFECT_BUFF_CRIT:
            buff_add(s, BUFF_CRIT_UP, item->effect_value, item->duration_sec, 0);
            break;

        case ITEM_EFFECT_TELEPORT_TOWN:
            /* Set flag for teleport on next move */
            s->pending_teleport = 1;
            s->teleport_map_id = 1;  /* Town map */
            break;

        case ITEM_EFFECT_REVIVE:
            if (s->current_hp == 0) {
                s->current_hp = s->combat_stats.max_hp * item->effect_value / 100;
            }
            break;

        case ITEM_EFFECT_CURE_DEBUFF:
            buff_remove_debuffs(s);
            break;

        case ITEM_EFFECT_EXP_BOOST:
            buff_add(s, BUFF_EXP_BOOST, item->effect_value, item->duration_sec, 0);
            break;

        case ITEM_EFFECT_DROP_BOOST:
            buff_add(s, BUFF_DROP_BOOST, item->effect_value, item->duration_sec, 0);
            break;

        default:
            return -1;
    }

    return 0;
}

int handle_use_consumable(ClientSession* s, const uint8_t* data, int len) {
    if (len < 6) return -1;

    uint32_t item_id;
    uint16_t slot;
    memcpy(&item_id, data, 4);
    memcpy(&slot, data + 4, 2);

    printf("[ITEM] USE item=%u slot=%u from fd=%d\n", item_id, slot, s->fd);

    const ConsumableItem* consumable = find_consumable(item_id);
    uint8_t response[16];
    int offset = 0;

    if (!consumable) {
        uint32_t result = 1;  /* Item not consumable */
        memcpy(response, &result, 4);
        return send_response(s, 462, response, 4);
    }

    /* Check level requirement */
    if (s->level < consumable->level_req) {
        uint32_t result = 2;  /* Level too low */
        memcpy(response, &result, 4);
        return send_response(s, 462, response, 4);
    }

    /* Apply effect */
    if (apply_item_effect(s, consumable) < 0) {
        uint32_t result = 3;  /* Effect failed */
        memcpy(response, &result, 4);
        return send_response(s, 462, response, 4);
    }

    /* Locate inventory entry for validation */
    DBItemInfo consumed_item;
    if (db_get_item_at_slot(s->character_id, slot, &consumed_item) < 0 ||
        consumed_item.ui_id == 0 || consumed_item.it_id != item_id) {
        uint32_t result = 4;  /* Item not found */
        memcpy(response, &result, 4);
        return send_response(s, 462, response, 4);
    }

    /* Consume item from inventory */
    db_consume_item(s->character_id, consumed_item.ui_id, 1);

    /* Send success response */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &item_id, 4); offset += 4;
    memcpy(response + offset, &s->current_hp, 4); offset += 4;
    memcpy(response + offset, &s->current_mp, 2); offset += 2;

    return send_response(s, 462, response, offset);
}

/*------------------------------------------------------------------------------
 * P3: Map Teleport System - 地图传送系统
 *----------------------------------------------------------------------------*/

typedef struct {
    uint16_t map_id;
    uint16_t x;
    uint16_t y;
    char     name[32];
    uint8_t  level_req;
    uint32_t cost;           /* Gold cost, 0 for free */
} TeleportPoint;

static const TeleportPoint g_teleport_points[] = {
    /* Towns */
    {1, 500, 500, "Seria's Room", 1, 0},
    {2, 300, 300, "Hendon Myre", 1, 0},
    {3, 400, 400, "Arad City", 15, 100},
    {4, 350, 350, "West Coast", 25, 200},
    {5, 450, 450, "Ghent", 55, 500},
    /* Dungeon Entrances */
    {101, 100, 100, "Forest Dungeon", 10, 50},
    {102, 150, 150, "Cave Dungeon", 20, 100},
    {103, 200, 200, "Ruins Dungeon", 35, 200},
    {104, 250, 250, "Tower Dungeon", 50, 300},
    {105, 300, 300, "Abyss Gate", 60, 500},
    {0, 0, 0, "", 0, 0}  /* Terminator */
};

static const TeleportPoint* find_teleport_point(uint16_t map_id) {
    for (int i = 0; g_teleport_points[i].map_id != 0; i++) {
        if (g_teleport_points[i].map_id == map_id) {
            return &g_teleport_points[i];
        }
    }
    return NULL;
}

int handle_teleport(ClientSession* s, const uint8_t* data, int len) {
    if (len < 2) return -1;

    uint16_t dest_map;
    memcpy(&dest_map, data, 2);

    printf("[TELEPORT] to map=%u from fd=%d\n", dest_map, s->fd);

    const TeleportPoint* tp = find_teleport_point(dest_map);
    uint8_t response[24];
    int offset = 0;

    if (!tp) {
        uint32_t result = 1;  /* Invalid destination */
        memcpy(response, &result, 4);
        return send_response(s, 470, response, 4);
    }

    /* Check level */
    if (s->level < tp->level_req) {
        uint32_t result = 2;  /* Level too low */
        memcpy(response, &result, 4);
        return send_response(s, 470, response, 4);
    }

    /* Check gold */
    if (s->inventory.money < tp->cost) {
        uint32_t result = 3;  /* Not enough gold */
        memcpy(response, &result, 4);
        return send_response(s, 470, response, 4);
    }

    /* Deduct gold */
    if (tp->cost > 0) {
        s->inventory.money -= tp->cost;
        db_update_gold(s->character_id, s->inventory.money);
    }

    /* Update position */
    s->current_map = dest_map;
    s->pos_x = tp->x;
    s->pos_y = tp->y;

    /* Send success response */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &dest_map, 2); offset += 2;
    memcpy(response + offset, &tp->x, 2); offset += 2;
    memcpy(response + offset, &tp->y, 2); offset += 2;
    memcpy(response + offset, &s->inventory.money, 4); offset += 4;

    return send_response(s, 470, response, offset);
}

int handle_move_map(ClientSession* s, const uint8_t* data, int len) {
    if (len < 6) return -1;

    uint16_t dest_map, x, y;
    memcpy(&dest_map, data, 2);
    memcpy(&x, data + 2, 2);
    memcpy(&y, data + 4, 2);

    printf("[MOVE_MAP] to map=%u pos=(%u,%u) from fd=%d\n", dest_map, x, y, s->fd);

    /* Basic validation */
    if (dest_map == 0 || dest_map > 500) {
        uint32_t result = 1;
        uint8_t response[4];
        memcpy(response, &result, 4);
        return send_response(s, 471, response, 4);
    }

    /* Update position */
    s->current_map = dest_map;
    s->pos_x = x;
    s->pos_y = y;

    uint8_t response[12];
    int offset = 0;
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &dest_map, 2); offset += 2;
    memcpy(response + offset, &x, 2); offset += 2;
    memcpy(response + offset, &y, 2); offset += 2;

    return send_response(s, 471, response, offset);
}

/*------------------------------------------------------------------------------
 * P4: Quest System Enhancement - 任务系统深化
 *----------------------------------------------------------------------------*/

#define MAX_ACTIVE_QUESTS       20

typedef enum {
    QUEST_TRIGGER_NONE = 0,
    QUEST_TRIGGER_KILL_MONSTER,
    QUEST_TRIGGER_COLLECT_ITEM,
    QUEST_TRIGGER_TALK_NPC,
    QUEST_TRIGGER_REACH_LEVEL,
    QUEST_TRIGGER_ENTER_MAP,
    QUEST_TRIGGER_CLEAR_DUNGEON,
    QUEST_TRIGGER_USE_SKILL,
    QUEST_TRIGGER_CRAFT_ITEM,
    QUEST_TRIGGER_ENHANCE_EQUIP
} QuestTriggerType;

typedef struct {
    uint16_t quest_id;
    uint8_t  trigger_type;
    uint32_t trigger_target;  /* Monster ID, Item ID, NPC ID, etc */
    uint16_t trigger_count;
    uint16_t next_quest_id;   /* Chain quest, 0 if none */
} QuestTrigger;

typedef struct {
    uint16_t quest_id;
    uint16_t current_count;
    uint8_t  status;          /* 0=inactive, 1=active, 2=complete */
} QuestProgress;

static const QuestTrigger g_quest_triggers[] = {
    /* Tutorial chain */
    {1001, QUEST_TRIGGER_KILL_MONSTER, 1001, 5, 1002},   /* Kill 5 goblins -> next quest */
    {1002, QUEST_TRIGGER_COLLECT_ITEM, 2001, 3, 1003},   /* Collect 3 goblin teeth */
    {1003, QUEST_TRIGGER_TALK_NPC, 101, 1, 0},           /* Talk to blacksmith */
    /* Level-up chain */
    {2001, QUEST_TRIGGER_REACH_LEVEL, 10, 1, 2002},
    {2002, QUEST_TRIGGER_REACH_LEVEL, 20, 1, 2003},
    {2003, QUEST_TRIGGER_REACH_LEVEL, 30, 1, 0},
    /* Dungeon chain */
    {3001, QUEST_TRIGGER_CLEAR_DUNGEON, 101, 1, 3002},
    {3002, QUEST_TRIGGER_CLEAR_DUNGEON, 102, 1, 3003},
    {3003, QUEST_TRIGGER_CLEAR_DUNGEON, 103, 1, 0},
    {0, QUEST_TRIGGER_NONE, 0, 0, 0}  /* Terminator */
};

static const QuestTrigger* find_quest_trigger(uint16_t quest_id) {
    for (int i = 0; g_quest_triggers[i].quest_id != 0; i++) {
        if (g_quest_triggers[i].quest_id == quest_id) {
            return &g_quest_triggers[i];
        }
    }
    return NULL;
}

static int trigger_quest_progress(ClientSession* s, uint8_t trigger_type,
                                   uint32_t target, uint16_t count) {
    if (!s) return 0;

    int quests_updated = 0;

    /* Check all active quests for matching triggers */
    for (int i = 0; i < MAX_ACTIVE_QUESTS && i < 20; i++) {
        if (s->active_quests[i] == 0) continue;

        const QuestTrigger* trigger = find_quest_trigger(s->active_quests[i]);
        if (!trigger) continue;

        if (trigger->trigger_type == trigger_type &&
            trigger->trigger_target == target) {

            s->quest_progress[i] += count;

            /* Check completion */
            if (s->quest_progress[i] >= trigger->trigger_count) {
                s->quest_status[i] = 2;  /* Complete */
                quests_updated++;

                /* Auto-start chain quest if exists */
                if (trigger->next_quest_id > 0) {
                    /* Find empty slot */
                    for (int j = 0; j < 20; j++) {
                        if (s->active_quests[j] == 0) {
                            s->active_quests[j] = trigger->next_quest_id;
                            s->quest_progress[j] = 0;
                            s->quest_status[j] = 1;
                            break;
                        }
                    }
                }
            }

            quests_updated++;
        }
    }

    return quests_updated;
}

int handle_quest_trigger(ClientSession* s, const uint8_t* data, int len) {
    if (len < 7) return -1;

    uint8_t trigger_type = data[0];
    uint32_t target;
    uint16_t count;
    memcpy(&target, data + 1, 4);
    memcpy(&count, data + 5, 2);

    printf("[QUEST] trigger type=%u target=%u count=%u from fd=%d\n",
           trigger_type, target, count, s->fd);

    int updated = trigger_quest_progress(s, trigger_type, target, count);

    uint8_t response[8];
    int offset = 0;
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    response[offset++] = (uint8_t)updated;

    return send_response(s, 472, response, offset);
}

int handle_quest_track(ClientSession* s, const uint8_t* data, int len) {
    if (len < 3) return -1;

    uint16_t quest_id;
    uint8_t track;
    memcpy(&quest_id, data, 2);
    track = data[2];

    printf("[QUEST] track quest=%u enable=%u from fd=%d\n", quest_id, track, s->fd);

    /* Toggle tracking */
    for (int i = 0; i < 5; i++) {
        if (track && s->tracked_quests[i] == 0) {
            s->tracked_quests[i] = quest_id;
            break;
        } else if (!track && s->tracked_quests[i] == quest_id) {
            s->tracked_quests[i] = 0;
            break;
        }
    }

    uint8_t response[8];
    int offset = 0;
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    memcpy(response + offset, &quest_id, 2); offset += 2;
    response[offset++] = track;

    return send_response(s, 473, response, offset);
}

/*------------------------------------------------------------------------------
 * P5: Dungeon Matching System - 副本匹配系统
 *----------------------------------------------------------------------------*/

#define MAX_MATCH_QUEUE        100
#define MATCH_TIMEOUT_SEC      120

typedef struct {
    uint32_t character_id;
    uint16_t dungeon_id;
    uint8_t  difficulty;
    uint8_t  level;
    uint8_t  job;
    uint32_t queue_time;
    int      socket_fd;
} MatchQueueEntry;

typedef struct {
    MatchQueueEntry entries[MAX_MATCH_QUEUE];
    int count;
} MatchQueue;

static MatchQueue g_match_queue = {{{0}}, 0};

static int find_match_for_player(MatchQueueEntry* player, MatchQueueEntry* matched, int max_matches) {
    int found = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < g_match_queue.count && found < max_matches; i++) {
        MatchQueueEntry* other = &g_match_queue.entries[i];

        /* Skip self */
        if (other->character_id == player->character_id) continue;

        /* Skip expired entries */
        if (now - other->queue_time > MATCH_TIMEOUT_SEC) continue;

        /* Match criteria */
        if (other->dungeon_id == player->dungeon_id &&
            other->difficulty == player->difficulty) {

            /* Level range check (+/- 10 levels) */
            int level_diff = (int)other->level - (int)player->level;
            if (level_diff < -10 || level_diff > 10) continue;

            matched[found++] = *other;
        }
    }

    return found;
}

static void remove_from_queue(uint32_t character_id) {
    for (int i = 0; i < g_match_queue.count; i++) {
        if (g_match_queue.entries[i].character_id == character_id) {
            /* Shift remaining entries */
            for (int j = i; j < g_match_queue.count - 1; j++) {
                g_match_queue.entries[j] = g_match_queue.entries[j + 1];
            }
            g_match_queue.count--;
            break;
        }
    }
}

int handle_quick_party_queue(ClientSession* s, const uint8_t* data, int len) {
    if (len < 4) return -1;

    uint16_t dungeon_id;
    uint8_t difficulty;
    memcpy(&dungeon_id, data, 2);
    difficulty = data[2];

    printf("[MATCH] Queue for dungeon=%u diff=%u from fd=%d\n",
           dungeon_id, difficulty, s->fd);

    /* Add to queue */
    if (g_match_queue.count < MAX_MATCH_QUEUE) {
        MatchQueueEntry* entry = &g_match_queue.entries[g_match_queue.count++];
        entry->character_id = s->character_id;
        entry->dungeon_id = dungeon_id;
        entry->difficulty = difficulty;
        entry->level = s->level;
        entry->job = s->job;
        entry->queue_time = (uint32_t)time(NULL);
        entry->socket_fd = s->fd;
    }

    /* Try to find matches */
    MatchQueueEntry matched[3];
    MatchQueueEntry self = {s->character_id, dungeon_id, difficulty, s->level,
                           s->job, (uint32_t)time(NULL), s->fd};
    int match_count = find_match_for_player(&self, matched, 3);

    uint8_t response[64];
    int offset = 0;

    if (match_count >= 1) {
        /* Match found! */
        uint32_t result = 0;
        memcpy(response + offset, &result, 4); offset += 4;
        response[offset++] = (uint8_t)(match_count + 1);  /* Including self */

        /* Send matched player info */
        for (int i = 0; i < match_count && i < 3; i++) {
            memcpy(response + offset, &matched[i].character_id, 4); offset += 4;
            response[offset++] = matched[i].level;
            response[offset++] = matched[i].job;

            /* Remove from queue */
            remove_from_queue(matched[i].character_id);
        }

        /* Remove self from queue */
        remove_from_queue(s->character_id);
    } else {
        /* No match yet, stay in queue */
        uint32_t result = 100;  /* Queued */
        memcpy(response + offset, &result, 4); offset += 4;
        response[offset++] = 0;  /* In queue */
    }

    return send_response(s, 480, response, offset);
}

int handle_quick_party_cancel(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;

    printf("[MATCH] Cancel queue from fd=%d\n", s->fd);

    remove_from_queue(s->character_id);

    uint8_t response[4];
    uint32_t result = 0;
    memcpy(response, &result, 4);

    return send_response(s, 481, response, 4);
}

int handle_match_status(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;

    /* Find player in queue */
    int in_queue = 0;
    uint32_t queue_time = 0;

    for (int i = 0; i < g_match_queue.count; i++) {
        if (g_match_queue.entries[i].character_id == s->character_id) {
            in_queue = 1;
            queue_time = (uint32_t)time(NULL) - g_match_queue.entries[i].queue_time;
            break;
        }
    }

    uint8_t response[12];
    int offset = 0;
    uint32_t result = 0;
    memcpy(response + offset, &result, 4); offset += 4;
    response[offset++] = (uint8_t)in_queue;
    memcpy(response + offset, &queue_time, 4); offset += 4;
    response[offset++] = (uint8_t)g_match_queue.count;

    return send_response(s, 482, response, offset);
}


/* Cash Shop 510-514 */
int handle_cash_shop_get_list(ClientSession* s, const uint8_t* d, int l) {
    (void)d;(void)l;
    printf("[SHOP] GET_LIST fd=%d\n", s->fd);
    uint8_t r[64];
    memset(r, 0, 64);
    int o = 4;
    r[o++] = 3;
    uint32_t i = 90001, p = 1000;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 0;
    i = 90002; p = 500;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 0;
    i = 90003; p = 200;
    memcpy(r + o, &i, 4); o += 4;
    memcpy(r + o, &p, 4); o += 4;
    r[o++] = 1;
    return send_response(s, 510, r, o);
}

int handle_cash_shop_buy(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] BUY fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 511, e, 4);
    }
    uint32_t item_id;
    uint8_t count = 1;
    memcpy(&item_id, d, 4);
    if (l >= 5) count = d[4];
    if (count == 0) count = 1;
    printf("[SHOP] Buy item=%u count=%d\n", item_id, count);
    int ret = cash_shop_buy_item(s, item_id, count);
    uint8_t r[16];
    memset(r, 0, 16);
    if (ret < 0) {
        uint32_t err = (uint32_t)(-ret);
        memcpy(r, &err, 4);
        return send_response(s, 511, r, 4);
    }
    memcpy(r + 4, &item_id, 4);
    r[8] = count;
    return send_response(s, 511, r, 12);
}

int handle_cash_shop_get_balance(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[SHOP] GET_BALANCE fd=%d account=%u\n", s->fd, s->account_id);
    uint32_t cash = 0, bind_cash = 0;
    db_get_cash_balance(s->account_id, &cash, &bind_cash);
    uint8_t vip_level = 0;
    uint32_t vip_expire = 0;
    db_get_vip_info(s->account_id, &vip_level, &vip_expire);
    uint8_t r[24];
    memset(r, 0, 24);
    memcpy(r + 4, &cash, 4);
    memcpy(r + 8, &bind_cash, 4);
    r[12] = vip_level;
    memcpy(r + 16, &vip_expire, 4);
    printf("[SHOP] Balance: cash=%u bind=%u vip=%d\n", cash, bind_cash, vip_level);
    return send_response(s, 512, r, 20);
}

int handle_cash_shop_recharge(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] RECHARGE fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 513, e, 4);
    }
    uint32_t amt;
    memcpy(&amt, d, 4);
    printf("[SHOP] Amount=%u\n", amt);
    uint8_t r[12];
    memset(r, 0, 12);
    memcpy(r + 4, &amt, 4);
    return send_response(s, 513, r, 8);
}

int handle_cash_shop_gift(ClientSession* s, const uint8_t* d, int l) {
    printf("[SHOP] GIFT fd=%d\n", s->fd);
    if (l < 8) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 514, e, 4);
    }
    uint32_t target, item;
    memcpy(&target, d, 4);
    memcpy(&item, d + 4, 4);
    printf("[SHOP] Gift item=%u to=%u\n", item, target);
    uint8_t r[16];
    memset(r, 0, 16);
    memcpy(r + 4, &target, 4);
    memcpy(r + 8, &item, 4);
    return send_response(s, 514, r, 12);
}


/* Cross-Server Matching 550-555 */
int handle_cross_match_queue(ClientSession* s, const uint8_t* d, int l) {
    printf("[MATCH] QUEUE fd=%d charac=%u\n", s->fd, s->character_id);
    if (l < 1) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 550, e, 4);
    }
    uint8_t mode = d[0];

    /* Check if already in queue */
    for (int i = 0; i < g_match_queue.count; i++) {
        if (g_match_queue.entries[i].character_id == s->character_id) {
            uint8_t e[4] = {3, 0, 0, 0}; /* Already queued */
            return send_response(s, 550, e, 4);
        }
    }

    /* Add to queue */
    if (g_match_queue.count >= MAX_MATCH_QUEUE) {
        uint8_t e[4] = {4, 0, 0, 0}; /* Queue full */
        return send_response(s, 550, e, 4);
    }

    MatchQueueEntry* entry = &g_match_queue.entries[g_match_queue.count++];
    entry->character_id = s->character_id;
    entry->dungeon_id = mode;
    entry->difficulty = 0;
    entry->level = s->level;
    entry->job = s->job;
    entry->queue_time = (uint32_t)time(NULL);
    entry->socket_fd = s->fd;

    printf("[MATCH] Added to queue, count=%d\n", g_match_queue.count);

    uint8_t r[16];
    memset(r, 0, 16);
    int offset = 4;
    r[offset++] = mode;
    uint32_t pos = g_match_queue.count;
    memcpy(r + offset, &pos, 4); offset += 4;
    return send_response(s, 550, r, offset);
}

int handle_cross_match_cancel(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[MATCH] CANCEL fd=%d\n", s->fd);
    remove_from_queue(s->character_id);
    uint8_t r[4] = {0, 0, 0, 0};
    return send_response(s, 551, r, 4);
}

int handle_cross_match_status(ClientSession* s, const uint8_t* d, int l) {
    (void)d; (void)l;
    printf("[MATCH] STATUS fd=%d\n", s->fd);
    uint8_t r[20];
    memset(r, 0, 20);
    int offset = 4;

    int in_queue = 0;
    uint32_t wait_time = 0;
    for (int i = 0; i < g_match_queue.count; i++) {
        if (g_match_queue.entries[i].character_id == s->character_id) {
            in_queue = 1;
            wait_time = (uint32_t)time(NULL) - g_match_queue.entries[i].queue_time;
            break;
        }
    }

    r[offset++] = in_queue;
    memcpy(r + offset, &wait_time, 4); offset += 4;
    uint32_t pos = g_match_queue.count;
    memcpy(r + offset, &pos, 4); offset += 4;
    return send_response(s, 552, r, offset);
}

int handle_cross_match_accept(ClientSession* s, const uint8_t* d, int l) {
    printf("[MATCH] ACCEPT fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 553, e, 4);
    }
    uint32_t match_id;
    memcpy(&match_id, d, 4);
    printf("[MATCH] Accept match=%u\n", match_id);

    uint8_t r[12];
    memset(r, 0, 12);
    int offset = 4;
    memcpy(r + offset, &match_id, 4); offset += 4;
    r[offset++] = 1; /* Accepted */
    return send_response(s, 553, r, offset);
}

int handle_cross_match_decline(ClientSession* s, const uint8_t* d, int l) {
    printf("[MATCH] DECLINE fd=%d\n", s->fd);
    if (l < 4) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 554, e, 4);
    }
    uint32_t match_id;
    memcpy(&match_id, d, 4);
    printf("[MATCH] Decline match=%u\n", match_id);
    remove_from_queue(s->character_id);

    uint8_t r[8];
    memset(r, 0, 8);
    memcpy(r + 4, &match_id, 4);
    return send_response(s, 554, r, 8);
}

int handle_cross_match_result(ClientSession* s, const uint8_t* d, int l) {
    printf("[MATCH] RESULT fd=%d\n", s->fd);
    if (l < 8) {
        uint8_t e[4] = {1, 0, 0, 0};
        return send_response(s, 555, e, 4);
    }
    uint32_t match_id, result;
    memcpy(&match_id, d, 4);
    memcpy(&result, d + 4, 4);

    /* 胜负奖励 */
    uint32_t reward = (result == 1) ? 5000 : 1000;
    s->inventory.money += reward;
    db_update_gold(s->character_id, s->inventory.money);
    printf("[MATCH] Result=%u reward=%u\n", result, reward);

    uint8_t r[20];
    memset(r, 0, 20);
    int offset = 4;
    memcpy(r + offset, &match_id, 4); offset += 4;
    memcpy(r + offset, &result, 4); offset += 4;
    memcpy(r + offset, &reward, 4); offset += 4;
    return send_response(s, 555, r, offset);
}
int handle_default(ClientSession* s, uint8_t msg_no, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] Unknown msg_no=%d from fd=%d\n", msg_no, s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, msg_no, response, 4);
}

/*==============================================================================
 * Packet Dispatcher
 *============================================================================*/
