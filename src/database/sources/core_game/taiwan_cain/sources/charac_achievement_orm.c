#include "charac_achievement_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 12288  /* Larger for BLOB HEX encoding */
#define MAX_FIELDS 10

/* Helper function to convert binary data to hex string */
static void bin_to_hex(const unsigned char* bin, int bin_len, char* hex) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sprintf(hex + (i * 2), "%02x", bin[i]);
    }
    hex[bin_len * 2] = '\0';
}

/* Helper function to convert hex string to binary data */
static int hex_to_bin(const char* hex, unsigned char* bin, int max_bin_len) {
    int hex_len = strlen(hex);
    int bin_len = hex_len / 2;
    int i;

    if (bin_len > max_bin_len) {
        return -1;
    }

    for (i = 0; i < bin_len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bin[i]);
    }

    return bin_len;
}

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacAchievement_Add(DBConnectionManager* manager, const CharacAchievement* record) {
    char query[MAX_QUERY_LEN];
    char hex_achievement[MAX_ACHIEVEMENT_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(hex_achievement, 0, sizeof(hex_achievement));

    bin_to_hex(record->achievement, record->achievement_len, hex_achievement);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_achievement (charac_no, achievement, last_update_time) "
        "VALUES (%u, UNHEX('%s'), '%s')",
        record->charac_no, hex_achievement, record->last_update_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAchievement_Get(DBConnectionManager* manager, unsigned int charac_no, CharacAchievement* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(CharacAchievement));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            record->achievement_len = hex_to_bin(values[1], record->achievement, MAX_ACHIEVEMENT_DATA_LEN);
        } else {
            record->achievement_len = 0;
        }
        if (values[2]) {
            strncpy(record->last_update_time, values[2], sizeof(record->last_update_time) - 1);
        }
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacAchievement_Update(DBConnectionManager* manager, unsigned int charac_no,
                               const unsigned char* new_achievement, int new_achievement_len) {
    char query[MAX_QUERY_LEN];
    char hex_achievement[MAX_ACHIEVEMENT_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !new_achievement || new_achievement_len < 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(hex_achievement, 0, sizeof(hex_achievement));

    bin_to_hex(new_achievement, new_achievement_len, hex_achievement);

    snprintf(query, sizeof(query),
        "UPDATE charac_achievement SET achievement = UNHEX('%s'), last_update_time = NOW() WHERE charac_no = %u",
        hex_achievement, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAchievement_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM charac_achievement WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAchievement_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int exists = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_achievement WHERE charac_no = %u LIMIT 1",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务查询实现
 ******************************************************************************/

int CharacAchievement_GetByLastUpdateTimeRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 CharacAchievement* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement "
        "WHERE last_update_time BETWEEN '%s' AND '%s' LIMIT %d",
        start_time, end_time, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            records[count].achievement_len = hex_to_bin(values[1], records[count].achievement, MAX_ACHIEVEMENT_DATA_LEN);
        } else {
            records[count].achievement_len = 0;
        }
        if (values[2]) {
            strncpy(records[count].last_update_time, values[2], sizeof(records[count].last_update_time) - 1);
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_GetRecentUpdates(DBConnectionManager* manager,
                                         CharacAchievement* records, int top_n) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || top_n <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement "
        "ORDER BY last_update_time DESC LIMIT %d",
        top_n);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < top_n && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            records[count].achievement_len = hex_to_bin(values[1], records[count].achievement, MAX_ACHIEVEMENT_DATA_LEN);
        } else {
            records[count].achievement_len = 0;
        }
        if (values[2]) {
            strncpy(records[count].last_update_time, values[2], sizeof(records[count].last_update_time) - 1);
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_GetEmptyAchievements(DBConnectionManager* manager,
                                             CharacAchievement* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement "
        "WHERE LENGTH(achievement) = 0 LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].achievement_len = 0;
        if (values[2]) {
            strncpy(records[count].last_update_time, values[2], sizeof(records[count].last_update_time) - 1);
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_GetNonEmptyAchievements(DBConnectionManager* manager,
                                                CharacAchievement* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement "
        "WHERE LENGTH(achievement) > 0 LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            records[count].achievement_len = hex_to_bin(values[1], records[count].achievement, MAX_ACHIEVEMENT_DATA_LEN);
        } else {
            records[count].achievement_len = 0;
        }
        if (values[2]) {
            strncpy(records[count].last_update_time, values[2], sizeof(records[count].last_update_time) - 1);
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_GetAchievementSize(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int size = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT LENGTH(achievement) FROM charac_achievement WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        size = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return size;
}

int CharacAchievement_GetByAchievementSize(DBConnectionManager* manager,
                                             int min_size, int max_size,
                                             CharacAchievement* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(achievement), last_update_time FROM charac_achievement "
        "WHERE LENGTH(achievement) BETWEEN %d AND %d LIMIT %d",
        min_size, max_size, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            records[count].achievement_len = hex_to_bin(values[1], records[count].achievement, MAX_ACHIEVEMENT_DATA_LEN);
        } else {
            records[count].achievement_len = 0;
        }
        if (values[2]) {
            strncpy(records[count].last_update_time, values[2], sizeof(records[count].last_update_time) - 1);
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_ClearAchievement(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_achievement SET achievement = '', last_update_time = NOW() WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAchievement_CountNonEmptyAchievements(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_achievement WHERE LENGTH(achievement) > 0");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacAchievement_UpdateTimestamp(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_achievement SET last_update_time = NOW() WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int CharacAchievement_BulkAdd(DBConnectionManager* manager,
                               const CharacAchievement* records, int count) {
    int i;
    int success_count = 0;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (CharacAchievement_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int CharacAchievement_BulkDeleteByCharacNos(DBConnectionManager* manager,
                                              const unsigned int* charac_nos, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;
    char id_list[2048];
    int offset = 0;

    if (!manager || !charac_nos || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(id_list, 0, sizeof(id_list));

    /* 构建 IN 子句的 ID 列表 */
    for (i = 0; i < count && offset < (int)sizeof(id_list) - 20; i++) {
        if (i > 0) {
            offset += snprintf(id_list + offset, sizeof(id_list) - offset, ",");
        }
        offset += snprintf(id_list + offset, sizeof(id_list) - offset, "%u", charac_nos[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_achievement WHERE charac_no IN (%s)",
        id_list);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacAchievement_PrintInfo(const CharacAchievement* record) {
    int i;

    if (!record) {
        printf("CharacAchievement record is NULL\n");
        return;
    }

    printf("=== CharacAchievement Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  achievement length: %d bytes\n", record->achievement_len);
    if (record->achievement_len > 0) {
        printf("  achievement data (hex, first 32 bytes): ");
        for (i = 0; i < record->achievement_len && i < 32; i++) {
            printf("%02x", record->achievement[i]);
        }
        if (record->achievement_len > 32) {
            printf("...");
        }
        printf("\n");
    } else {
        printf("  achievement data: (empty)\n");
    }
    printf("  last_update_time: %s\n", record->last_update_time);
    printf("================================\n");
}
