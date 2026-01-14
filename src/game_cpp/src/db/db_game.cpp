#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_server_context.h"
#include "db/db_game.h"

static int run_query(int db_type, const char* query, DBQueryResult* result) {
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, db_type, query, result) < 0) {
        return -1;
    }
    return 0;
}

static int run_non_query(int db_type, const char* query) {
    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(db_type, query, &result) < 0) {
        return -1;
    }
    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

static void bytes_to_hex(const unsigned char* data, int len, char* out) {
    static const char hex_table[] = "0123456789ABCDEF";
    for (int i = 0; i < len; i++) {
        out[i * 2] = hex_table[(data[i] >> 4) & 0xF];
        out[i * 2 + 1] = hex_table[data[i] & 0xF];
    }
    out[len * 2] = '\0';
}

#ifndef ABYSS_TICKET_ITEM_ID
#define ABYSS_TICKET_ITEM_ID 99001
#endif


// Duplicate removed: uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur) {
// Duplicate removed:     uint32_t lvl = (item_id / 1000) % 100;
// Duplicate removed:     if (lvl == 0) lvl = 1;
// Duplicate removed:     return (100 - cur_dur) * lvl * 10;
// Duplicate removed: }

int db_get_abyss_daily_count(uint32_t charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM abyss_clears "
             "WHERE charac_no = %u AND DATE(clear_time) = CURDATE()",
             charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    int count = 0;
    char* row[1] = {0};
    if (DBQueryResult_FetchRow(&result, row) == 1 && row[0]) {
        count = atoi(row[0]);
    }
    DBQueryResult_Free(&result);

    return count;
}

int db_get_abyss_ticket_count(uint32_t charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT add_info FROM user_items "
             "WHERE charac_no = %u AND it_id = %u AND slot < 56",
             charac_no, ABYSS_TICKET_ITEM_ID);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    char* row[1] = {0};
    int count = 0;
    while (DBQueryResult_FetchRow(&result, row) == 1) {
        if (row[0]) {
            count += atoi(row[0]);
        }
    }
    DBQueryResult_Free(&result);

    return count;
}

int db_learn_skill(uint32_t charac_no, uint16_t skill_id, uint8_t new_level) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT skill_slot FROM skill WHERE charac_no=%u", charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        printf("[DB] Learn skill query failed for charac_no=%u\n", charac_no);
        return -1;
    }

    unsigned char new_blob[1024];
    int new_len = 0;
    int found = 0;

    char* row[1] = {0};
    unsigned long* lengths = NULL;
    if (DBQueryResult_FetchRowWithLengths(&result, row, &lengths) > 0 &&
        row[0] && lengths) {
        const unsigned char* old_blob = (const unsigned char*)row[0];
        unsigned long old_len = lengths[0];

        for (unsigned long i = 0; i + 4 <= old_len && new_len + 4 < sizeof(new_blob); i += 4) {
            uint16_t sid = old_blob[i] | (old_blob[i + 1] << 8);
            if (sid == skill_id) {
                new_blob[new_len++] = skill_id & 0xFF;
                new_blob[new_len++] = (skill_id >> 8) & 0xFF;
                new_blob[new_len++] = new_level;
                new_blob[new_len++] = 0;
                found = 1;
            } else {
                memcpy(new_blob + new_len, old_blob + i, 4);
                new_len += 4;
            }
        }
    }

    if (!found && new_len + 4 < sizeof(new_blob)) {
        new_blob[new_len++] = skill_id & 0xFF;
        new_blob[new_len++] = (skill_id >> 8) & 0xFF;
        new_blob[new_len++] = new_level;
        new_blob[new_len++] = 0;
    }

    DBQueryResult_Free(&result);

    char hex_blob[2049];
    bytes_to_hex(new_blob, new_len, hex_blob);

    snprintf(query, sizeof(query),
             "UPDATE skill SET skill_slot=UNHEX('%s') WHERE charac_no=%u",
             hex_blob, charac_no);

    if (run_non_query(DB_TYPE_CAIN, query) < 0) {
        printf("[DB] Update skill_slot failed for charac_no=%u\n", charac_no);
        return -1;
    }

    printf("[DB] Learned skill %u level %u for charac_no=%u\n", skill_id, new_level, charac_no);
    return 0;
}

int db_update_achievement(uint32_t charac_no, uint16_t ach_id, uint32_t progress,
                          uint8_t completed, uint8_t claimed) {
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO charac_achievements (charac_no, ach_id, progress, completed, claimed) "
             "VALUES (%u, %d, %u, %d, %d) "
             "ON DUPLICATE KEY UPDATE progress=%u, completed=%d, claimed=%d",
             charac_no, ach_id, progress, completed, claimed,
             progress, completed, claimed);

    return run_non_query(DB_TYPE_CAIN, query) < 0 ? -1 : 0;
}
