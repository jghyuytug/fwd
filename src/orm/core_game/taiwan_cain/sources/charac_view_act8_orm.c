#include "charac_view_act8_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 16
#define MAX_INFO_LEN 65535

/* ==================== CRUD 操作 ==================== */

int CharacViewAct8_Add(DBConnectionManager* manager, const CharacViewAct8* record) {
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
        "REPLACE INTO charac_view_act8 "
        "(m_id, info, slot_effect_count, charac_slot_limit, hash_key) "
        "VALUES (%llu, '%s', %u, %u, '%s')",
        record->m_id,
        escaped_info ? escaped_info : "",
        record->slot_effect_count,
        record->charac_slot_limit,
        record->hash_key);

    if (escaped_info) {
        free(escaped_info);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacViewAct8_Get(DBConnectionManager* manager, unsigned long long m_id, CharacViewAct8* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacViewAct8));

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key "
        "FROM charac_view_act8 WHERE m_id = %llu",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = strtoull(values[0], NULL, 10);

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

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacViewAct8_Update(DBConnectionManager* manager, const CharacViewAct8* record) {
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
        "UPDATE charac_view_act8 SET "
        "info = '%s', slot_effect_count = %u, charac_slot_limit = %u, hash_key = '%s' "
        "WHERE m_id = %llu",
        escaped_info ? escaped_info : "",
        record->slot_effect_count,
        record->charac_slot_limit,
        record->hash_key,
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

int CharacViewAct8_Delete(DBConnectionManager* manager, unsigned long long m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_view_act8 WHERE m_id = %llu",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacViewAct8_Exists(DBConnectionManager* manager, unsigned long long m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_view_act8 WHERE m_id = %llu",
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

int CharacViewAct8_UpdateSlotEffectCount(DBConnectionManager* manager, unsigned long long m_id, unsigned char count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view_act8 SET slot_effect_count = %u WHERE m_id = %llu",
        count, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacViewAct8_UpdateCharacSlotLimit(DBConnectionManager* manager, unsigned long long m_id, unsigned char limit) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view_act8 SET charac_slot_limit = %u WHERE m_id = %llu",
        limit, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacViewAct8_UpdateHashKey(DBConnectionManager* manager, unsigned long long m_id, const char* hash_key) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !hash_key) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_view_act8 SET hash_key = '%s' WHERE m_id = %llu",
        hash_key, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int CharacViewAct8_GetByMinSlotLimit(DBConnectionManager* manager, unsigned char min_limit,
                                     CharacViewAct8* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key "
        "FROM charac_view_act8 WHERE charac_slot_limit >= %u ORDER BY charac_slot_limit DESC LIMIT %d",
        min_limit, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = strtoull(values[0], NULL, 10);

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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacViewAct8_GetBySlotEffectCount(DBConnectionManager* manager, unsigned char count_val,
                                        CharacViewAct8* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, info, slot_effect_count, charac_slot_limit, hash_key "
        "FROM charac_view_act8 WHERE slot_effect_count = %u LIMIT %d",
        count_val, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].m_id = strtoull(values[0], NULL, 10);

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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void CharacViewAct8_PrintInfo(const CharacViewAct8* record) {
    if (!record) {
        printf("CharacViewAct8: NULL\n");
        return;
    }

    printf("\nCharacViewAct8 {\n");
    printf("  m_id: %llu\n", record->m_id);
    printf("  info_len: %d bytes\n", record->info_len);
    printf("  slot_effect_count: %u\n", record->slot_effect_count);
    printf("  charac_slot_limit: %u\n", record->charac_slot_limit);
    printf("  hash_key: %s\n", record->hash_key);
    printf("}\n");
}

void CharacViewAct8_Free(CharacViewAct8* record) {
    if (record && record->info) {
        free(record->info);
        record->info = NULL;
        record->info_len = 0;
    }
}
