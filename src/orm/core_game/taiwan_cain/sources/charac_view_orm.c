#include "charac_view_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 16
#define MAX_INFO_LEN 65535

/* ==================== CRUD 操作 ==================== */

int CharacView_Add(DBConnectionManager* manager, const CharacView* record) {
    char query[MAX_QUERY_LEN];
    char* escaped_info = NULL;
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 为blob字段分配转义缓冲区 */
    if (record->info && record->info_len > 0) {
        escaped_info = (char*)malloc(record->info_len * 2 + 1);
        if (!escaped_info) {
            return -1;
        }
        /* 注意: 实际使用时需要mysql_real_escape_string */
        memcpy(escaped_info, record->info, record->info_len);
        escaped_info[record->info_len] = '\0';
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_view "
        "(m_id, info, slot_effect_count, charac_slot_limit, hash_key, charac_count) "
        "VALUES (%u, '%s', %u, %u, '%s', %u)",
        record->m_id,
        escaped_info ? escaped_info : "",
        record->slot_effect_count,
        record->charac_slot_limit,
        record->hash_key,
        record->charac_count);

    if (escaped_info) {
        free(escaped_info);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_Get(DBConnectionManager* manager, unsigned int m_id, CharacView* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacView));

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key, charac_count "
        "FROM charac_view WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = (unsigned int)atoi(values[0]);

        /* 处理blob字段 */
        if (values[1]) {
            record->info_len = strlen(values[1]);
            if (record->info_len > 0) {
                record->info = (unsigned char*)malloc(record->info_len);
                if (record->info) {
                    memcpy(record->info, values[1], record->info_len);
                }
            }
        }

        record->slot_effect_count = (unsigned char)atoi(values[2]);
        record->charac_slot_limit = (unsigned char)atoi(values[3]);
        if (values[4]) strncpy(record->hash_key, values[4], sizeof(record->hash_key) - 1);
        record->charac_count = (unsigned char)atoi(values[5]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacView_Update(DBConnectionManager* manager, const CharacView* record) {
    char query[MAX_QUERY_LEN];
    char* escaped_info = NULL;
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 为blob字段分配转义缓冲区 */
    if (record->info && record->info_len > 0) {
        escaped_info = (char*)malloc(record->info_len * 2 + 1);
        if (!escaped_info) {
            return -1;
        }
        memcpy(escaped_info, record->info, record->info_len);
        escaped_info[record->info_len] = '\0';
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET "
        "info = '%s', slot_effect_count = %u, charac_slot_limit = %u, "
        "hash_key = '%s', charac_count = %u "
        "WHERE m_id = %u",
        escaped_info ? escaped_info : "",
        record->slot_effect_count,
        record->charac_slot_limit,
        record->hash_key,
        record->charac_count,
        record->m_id);

    if (escaped_info) {
        free(escaped_info);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_view WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_view WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 专项操作 ==================== */

int CharacView_UpdateSlotEffectCount(DBConnectionManager* manager, unsigned int m_id, unsigned char count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET slot_effect_count = %u WHERE m_id = %u",
        count, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_UpdateCharacSlotLimit(DBConnectionManager* manager, unsigned int m_id, unsigned char limit) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET charac_slot_limit = %u WHERE m_id = %u",
        limit, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_UpdateHashKey(DBConnectionManager* manager, unsigned int m_id, const char* hash_key) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !hash_key) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET hash_key = '%s' WHERE m_id = %u",
        hash_key, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_UpdateCharacCount(DBConnectionManager* manager, unsigned int m_id, unsigned char count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET charac_count = %u WHERE m_id = %u",
        count, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_IncrementCharacCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET charac_count = charac_count + 1 WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacView_DecrementCharacCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view SET charac_count = IF(charac_count > 0, charac_count - 1, 0) WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int CharacView_GetByMinCharacCount(DBConnectionManager* manager, unsigned char min_count,
                                   CharacView* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key, charac_count "
        "FROM charac_view WHERE charac_count >= %u ORDER BY charac_count DESC LIMIT %d",
        min_count, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);

        /* 处理blob字段 */
        records[count].info = NULL;
        records[count].info_len = 0;
        if (values[1]) {
            records[count].info_len = strlen(values[1]);
            if (records[count].info_len > 0) {
                records[count].info = (unsigned char*)malloc(records[count].info_len);
                if (records[count].info) {
                    memcpy(records[count].info, values[1], records[count].info_len);
                }
            }
        }

        records[count].slot_effect_count = (unsigned char)atoi(values[2]);
        records[count].charac_slot_limit = (unsigned char)atoi(values[3]);
        if (values[4]) strncpy(records[count].hash_key, values[4], sizeof(records[count].hash_key) - 1);
        records[count].charac_count = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacView_GetByCharacCount(DBConnectionManager* manager, unsigned char count_val,
                                CharacView* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key, charac_count "
        "FROM charac_view WHERE charac_count = %u LIMIT %d",
        count_val, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);

        /* 处理blob字段 */
        records[count].info = NULL;
        records[count].info_len = 0;
        if (values[1]) {
            records[count].info_len = strlen(values[1]);
            if (records[count].info_len > 0) {
                records[count].info = (unsigned char*)malloc(records[count].info_len);
                if (records[count].info) {
                    memcpy(records[count].info, values[1], records[count].info_len);
                }
            }
        }

        records[count].slot_effect_count = (unsigned char)atoi(values[2]);
        records[count].charac_slot_limit = (unsigned char)atoi(values[3]);
        if (values[4]) strncpy(records[count].hash_key, values[4], sizeof(records[count].hash_key) - 1);
        records[count].charac_count = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacView_GetByMinSlotLimit(DBConnectionManager* manager, unsigned char min_limit,
                                 CharacView* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key, charac_count "
        "FROM charac_view WHERE charac_slot_limit >= %u ORDER BY charac_slot_limit DESC LIMIT %d",
        min_limit, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = (unsigned int)atoi(values[0]);

        /* 处理blob字段 */
        records[count].info = NULL;
        records[count].info_len = 0;
        if (values[1]) {
            records[count].info_len = strlen(values[1]);
            if (records[count].info_len > 0) {
                records[count].info = (unsigned char*)malloc(records[count].info_len);
                if (records[count].info) {
                    memcpy(records[count].info, values[1], records[count].info_len);
                }
            }
        }

        records[count].slot_effect_count = (unsigned char)atoi(values[2]);
        records[count].charac_slot_limit = (unsigned char)atoi(values[3]);
        if (values[4]) strncpy(records[count].hash_key, values[4], sizeof(records[count].hash_key) - 1);
        records[count].charac_count = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void CharacView_PrintInfo(const CharacView* record) {
    if (!record) {
        printf("CharacView: NULL\n");
        return;
    }

    printf("\nCharacView {\n");
    printf("  m_id: %u\n", record->m_id);
    printf("  info_len: %d bytes\n", record->info_len);
    printf("  slot_effect_count: %u\n", record->slot_effect_count);
    printf("  charac_slot_limit: %u\n", record->charac_slot_limit);
    printf("  hash_key: %s\n", record->hash_key);
    printf("  charac_count: %u\n", record->charac_count);
    printf("}\n");
}

void CharacView_Free(CharacView* record) {
    if (record && record->info) {
        free(record->info);
        record->info = NULL;
        record->info_len = 0;
    }
}
