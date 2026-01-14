#include "ch_status_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作 ==================== */

int ChStatus_Add(DBConnectionManager* manager, const ChStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 由于表无主键，先删除可能存在的记录 */
    snprintf(query, sizeof(query),
        "DELETE FROM ch_status WHERE gc_group = %u",
        record->gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }
    DBQueryResult_Free(&result);

    /* 然后插入新记录 */
    snprintf(query, sizeof(query),
        "INSERT INTO ch_status (gc_group, gc_status) VALUES (%u, %u)",
        record->gc_group, record->gc_status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_Get(DBConnectionManager* manager, unsigned char gc_group, ChStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT gc_group, gc_status FROM ch_status WHERE gc_group = %u",
        gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->gc_group = (unsigned char)atoi(values[0]);
    record->gc_status = (unsigned char)atoi(values[1]);

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_Update(DBConnectionManager* manager, const ChStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE ch_status SET gc_status = %u WHERE gc_group = %u",
        record->gc_status, record->gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_Delete(DBConnectionManager* manager, unsigned char gc_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM ch_status WHERE gc_group = %u",
        gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_Exists(DBConnectionManager* manager, unsigned char gc_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) as cnt FROM ch_status WHERE gc_group = %u",
        gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int ChStatus_UpdateStatus(DBConnectionManager* manager, unsigned char gc_group, unsigned char gc_status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE ch_status SET gc_status = %u WHERE gc_group = %u",
        gc_status, gc_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_SetStatus(DBConnectionManager* manager, unsigned char gc_group, unsigned char gc_status) {
    ChStatus record;
    record.gc_group = gc_group;
    record.gc_status = gc_status;
    return ChStatus_Add(manager, &record);
}

/* ==================== 查询操作 ==================== */

int ChStatus_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) as cnt FROM ch_status");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    char* values[MAX_FIELDS];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int ChStatus_GetAll(DBConnectionManager* manager, ChStatus** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT gc_group, gc_status FROM ch_status ORDER BY gc_group");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (ChStatus*)malloc(sizeof(ChStatus) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        (*records)[*count].gc_group = (unsigned char)atoi(values[0]);
        (*records)[*count].gc_status = (unsigned char)atoi(values[1]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_GetByStatus(DBConnectionManager* manager, unsigned char gc_status, ChStatus** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT gc_group, gc_status FROM ch_status WHERE gc_status = %u ORDER BY gc_group",
        gc_status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (ChStatus*)malloc(sizeof(ChStatus) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        (*records)[*count].gc_group = (unsigned char)atoi(values[0]);
        (*records)[*count].gc_status = (unsigned char)atoi(values[1]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_GetByGroupRange(DBConnectionManager* manager, unsigned char min_group, unsigned char max_group,
                              ChStatus** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT gc_group, gc_status FROM ch_status WHERE gc_group >= %u AND gc_group <= %u ORDER BY gc_group",
        min_group, max_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (ChStatus*)malloc(sizeof(ChStatus) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        (*records)[*count].gc_group = (unsigned char)atoi(values[0]);
        (*records)[*count].gc_status = (unsigned char)atoi(values[1]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 批量操作 ==================== */

int ChStatus_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM ch_status");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_BatchDelete(DBConnectionManager* manager, const unsigned char* gc_groups, int count) {
    char query[MAX_QUERY_LEN];
    char ids[1024];
    DBQueryResult result;

    if (!manager || !gc_groups || count <= 0) {
        return -1;
    }

    /* 构建 IN 子句 */
    int offset = 0;
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            offset += snprintf(ids + offset, sizeof(ids) - offset, ",");
        }
        offset += snprintf(ids + offset, sizeof(ids) - offset, "%u", gc_groups[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM ch_status WHERE gc_group IN (%s)",
        ids);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChStatus_BatchSetStatus(DBConnectionManager* manager, const unsigned char* gc_groups, int count,
                             unsigned char gc_status) {
    char query[MAX_QUERY_LEN];
    char ids[1024];
    DBQueryResult result;

    if (!manager || !gc_groups || count <= 0) {
        return -1;
    }

    /* 构建 IN 子句 */
    int offset = 0;
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            offset += snprintf(ids + offset, sizeof(ids) - offset, ",");
        }
        offset += snprintf(ids + offset, sizeof(ids) - offset, "%u", gc_groups[i]);
    }

    snprintf(query, sizeof(query),
        "UPDATE ch_status SET gc_status = %u WHERE gc_group IN (%s)",
        gc_status, ids);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数 ==================== */

void ChStatus_PrintInfo(const ChStatus* record) {
    if (!record) {
        printf("ChStatus record is NULL\n");
        return;
    }

    printf("========================================\n");
    printf("ChStatus 信息:\n");
    printf("  分组ID: %u\n", record->gc_group);
    printf("  状态值: %u\n", record->gc_status);
    printf("========================================\n");
}
