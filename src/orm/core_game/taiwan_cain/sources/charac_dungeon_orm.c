#include "charac_dungeon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 16384
#define MAX_FIELDS 16

/*******************************************************************************
 * 辅助函数
 ******************************************************************************/

static void bin_to_hex(const unsigned char* bin, int bin_len, char* hex) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sprintf(hex + (i * 2), "%02x", bin[i]);
    }
    hex[bin_len * 2] = '\0';
}

static int hex_to_bin(const char* hex, unsigned char* bin, int max_bin_len) {
    int hex_len = strlen(hex);
    int bin_len = hex_len / 2;
    int i;

    if (bin_len > max_bin_len) {
        bin_len = max_bin_len;
    }

    for (i = 0; i < bin_len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bin[i]);
    }

    return bin_len;
}

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacDungeon_Add(DBConnectionManager* manager, const CharacDungeon* record) {
    char query[MAX_QUERY_LEN];
    char hex_dungeon[MAX_DUNGEON_DATA_LEN * 2 + 1];
    char hex_best_clear_time[MAX_BEST_CLEAR_TIME_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->dungeon, record->dungeon_len, hex_dungeon);
    bin_to_hex(record->best_clear_time, record->best_clear_time_len, hex_best_clear_time);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_dungeon (charac_no, dungeon, best_clear_time, "
        "blue_marble_enter_count, charac_inform_notice) "
        "VALUES (%u, UNHEX('%s'), UNHEX('%s'), %u, '%s')",
        record->charac_no, hex_dungeon, hex_best_clear_time,
        record->blue_marble_enter_count, record->charac_inform_notice);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_Get(DBConnectionManager* manager, unsigned int charac_no,
                      CharacDungeon* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(dungeon), HEX(best_clear_time), "
        "blue_marble_enter_count, charac_inform_notice "
        "FROM charac_dungeon WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);

        if (values[1]) {
            record->dungeon_len = hex_to_bin(values[1], record->dungeon, MAX_DUNGEON_DATA_LEN);
        } else {
            record->dungeon_len = 0;
        }

        if (values[2]) {
            record->best_clear_time_len = hex_to_bin(values[2], record->best_clear_time, MAX_BEST_CLEAR_TIME_LEN);
        } else {
            record->best_clear_time_len = 0;
        }

        record->blue_marble_enter_count = (unsigned char)atoi(values[3]);

        if (values[4]) {
            strncpy(record->charac_inform_notice, values[4], MAX_NOTICE_LEN);
            record->charac_inform_notice[MAX_NOTICE_LEN] = '\0';
        } else {
            record->charac_inform_notice[0] = '\0';
        }

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacDungeon_Update(DBConnectionManager* manager, const CharacDungeon* record) {
    char query[MAX_QUERY_LEN];
    char hex_dungeon[MAX_DUNGEON_DATA_LEN * 2 + 1];
    char hex_best_clear_time[MAX_BEST_CLEAR_TIME_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->dungeon, record->dungeon_len, hex_dungeon);
    bin_to_hex(record->best_clear_time, record->best_clear_time_len, hex_best_clear_time);

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon SET dungeon = UNHEX('%s'), "
        "best_clear_time = UNHEX('%s'), blue_marble_enter_count = %u, "
        "charac_inform_notice = '%s' WHERE charac_no = %u",
        hex_dungeon, hex_best_clear_time, record->blue_marble_enter_count,
        record->charac_inform_notice, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_dungeon WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_dungeon WHERE charac_no = %u LIMIT 1",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int CharacDungeon_UpdateDungeonData(DBConnectionManager* manager, unsigned int charac_no,
                                    const unsigned char* dungeon, int dungeon_len) {
    char query[MAX_QUERY_LEN];
    char hex_dungeon[MAX_DUNGEON_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !dungeon || dungeon_len <= 0) {
        return -1;
    }

    bin_to_hex(dungeon, dungeon_len, hex_dungeon);

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon SET dungeon = UNHEX('%s') WHERE charac_no = %u",
        hex_dungeon, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_UpdateBestClearTime(DBConnectionManager* manager, unsigned int charac_no,
                                      const unsigned char* best_clear_time,
                                      int best_clear_time_len) {
    char query[MAX_QUERY_LEN];
    char hex_best_clear_time[MAX_BEST_CLEAR_TIME_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !best_clear_time || best_clear_time_len <= 0) {
        return -1;
    }

    bin_to_hex(best_clear_time, best_clear_time_len, hex_best_clear_time);

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon SET best_clear_time = UNHEX('%s') WHERE charac_no = %u",
        hex_best_clear_time, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_UpdateNotice(DBConnectionManager* manager, unsigned int charac_no,
                               const char* notice) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !notice) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon SET charac_inform_notice = '%s' WHERE charac_no = %u",
        notice, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeon_IncrementBlueMarbleCount(DBConnectionManager* manager,
                                           unsigned int charac_no,
                                           unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon SET blue_marble_enter_count = blue_marble_enter_count + %u "
        "WHERE charac_no = %u",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacDungeon_PrintInfo(const CharacDungeon* record) {
    int i;

    if (!record) {
        printf("CharacDungeon record is NULL\n");
        return;
    }

    printf("=== CharacDungeon Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  dungeon length: %d bytes\n", record->dungeon_len);

    if (record->dungeon_len > 0) {
        printf("  dungeon (hex, first 64 bytes): ");
        for (i = 0; i < record->dungeon_len && i < 64; i++) {
            printf("%02x", record->dungeon[i]);
        }
        printf("\n");
    }

    printf("  best_clear_time length: %d bytes\n", record->best_clear_time_len);

    if (record->best_clear_time_len > 0) {
        printf("  best_clear_time (hex, first 64 bytes): ");
        for (i = 0; i < record->best_clear_time_len && i < 64; i++) {
            printf("%02x", record->best_clear_time[i]);
        }
        printf("\n");
    }

    printf("  blue_marble_enter_count: %u\n", record->blue_marble_enter_count);
    printf("  charac_inform_notice: %s\n", record->charac_inform_notice);
    printf("==============================\n");
}
