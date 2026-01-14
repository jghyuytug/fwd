#include "limit_create_character_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD 操作实现 */
int LimitCreateCharacter_Add(DBConnectionManager* manager, const LimitCreateCharacter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO limit_create_character (m_id, count, last_access_time) VALUES (%u, %u, '%s')",
        record->m_id,
        record->count,
        record->last_access_time);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_Get(DBConnectionManager* manager, unsigned int m_id, LimitCreateCharacter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int ret;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->m_id = atoi(values[0]);
    record->count = atoi(values[1]);
    strncpy(record->last_access_time, values[2] ? values[2] : "", sizeof(record->last_access_time) - 1);
    record->last_access_time[sizeof(record->last_access_time) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_Update(DBConnectionManager* manager, const LimitCreateCharacter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character SET count = %u, last_access_time = '%s' WHERE m_id = %u",
        record->count,
        record->last_access_time,
        record->m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limit_create_character WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int exists = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limit_create_character WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* 专项操作 */
int LimitCreateCharacter_IncrementCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character SET count = count + 1, last_access_time = NOW() WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_ResetCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character SET count = 0, last_access_time = NOW() WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_UpdateAccessTime(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character SET last_access_time = NOW() WHERE m_id = %u",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 查询操作 */
int LimitCreateCharacter_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM limit_create_character");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int LimitCreateCharacter_GetAll(DBConnectionManager* manager, LimitCreateCharacter** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character ORDER BY m_id");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = result.row_count;
    if (*count == 0) {
        *records = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (LimitCreateCharacter*)malloc(sizeof(LimitCreateCharacter) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].m_id = atoi(values[0]);
        (*records)[i].count = atoi(values[1]);
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_GetByCountRange(DBConnectionManager* manager, unsigned int min_count, unsigned int max_count,
                                         LimitCreateCharacter** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character "
        "WHERE count BETWEEN %u AND %u ORDER BY count DESC",
        min_count, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = result.row_count;
    if (*count == 0) {
        *records = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (LimitCreateCharacter*)malloc(sizeof(LimitCreateCharacter) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].m_id = atoi(values[0]);
        (*records)[i].count = atoi(values[1]);
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_GetRecentAccess(DBConnectionManager* manager, int days,
                                         LimitCreateCharacter** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[3];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character "
        "WHERE last_access_time >= DATE_SUB(NOW(), INTERVAL %d DAY) "
        "ORDER BY last_access_time DESC",
        days);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = result.row_count;
    if (*count == 0) {
        *records = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (LimitCreateCharacter*)malloc(sizeof(LimitCreateCharacter) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].m_id = atoi(values[0]);
        (*records)[i].count = atoi(values[1]);
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 批量操作 */
int LimitCreateCharacter_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM limit_create_character");

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_BatchDelete(DBConnectionManager* manager, const unsigned int* m_ids, int count) {
    char query[MAX_QUERY_LEN];
    char ids[2048];
    DBQueryResult result;

    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    ids[0] = '\0';
    for (int i = 0; i < count; i++) {
        char id_str[32];
        snprintf(id_str, sizeof(id_str), "%s%u", (i > 0) ? "," : "", m_ids[i]);
        strcat(ids, id_str);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limit_create_character WHERE m_id IN (%s)", ids);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_BatchResetCount(DBConnectionManager* manager, const unsigned int* m_ids, int count) {
    char query[MAX_QUERY_LEN];
    char ids[2048];
    DBQueryResult result;

    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    ids[0] = '\0';
    for (int i = 0; i < count; i++) {
        char id_str[32];
        snprintf(id_str, sizeof(id_str), "%s%u", (i > 0) ? "," : "", m_ids[i]);
        strcat(ids, id_str);
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character SET count = 0, last_access_time = NOW() WHERE m_id IN (%s)", ids);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数 */
void LimitCreateCharacter_PrintInfo(const LimitCreateCharacter* record) {
    if (!record) {
        printf("LimitCreateCharacter: NULL\n");
        return;
    }

    printf("=== LimitCreateCharacter Info ===\n");
    printf("Member ID: %u\n", record->m_id);
    printf("Create Count: %u\n", record->count);
    printf("Last Access Time: %s\n", record->last_access_time);
    printf("=================================\n");
}
