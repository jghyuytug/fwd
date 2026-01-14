#include "quest_category_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int QuestCategory_Add(DBConnectionManager* manager, const QuestCategory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO quest_category (quest_idx, quest_name) VALUES (%d, '%s')",
        record->quest_idx, record->quest_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int QuestCategory_Get(DBConnectionManager* manager, int quest_idx, QuestCategory* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(QuestCategory));

    snprintf(query, sizeof(query),
        "SELECT quest_idx, quest_name FROM quest_category WHERE quest_idx = %d",
        quest_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->quest_idx = atoi(values[0]);
        if (values[1]) {
            strncpy(record->quest_name, values[1], MAX_QUEST_NAME_LEN);
            record->quest_name[MAX_QUEST_NAME_LEN] = '\0';
        }
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int QuestCategory_Update(DBConnectionManager* manager, const QuestCategory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE quest_category SET quest_name = '%s' WHERE quest_idx = %d",
        record->quest_name, record->quest_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int QuestCategory_Delete(DBConnectionManager* manager, int quest_idx) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM quest_category WHERE quest_idx = %d",
        quest_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int QuestCategory_Exists(DBConnectionManager* manager, int quest_idx) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM quest_category WHERE quest_idx = %d",
        quest_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int QuestCategory_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM quest_category");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int QuestCategory_GetAll(DBConnectionManager* manager, QuestCategory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT quest_idx, quest_name FROM quest_category ORDER BY quest_idx LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].quest_idx = atoi(values[0]);
        if (values[1]) {
            strncpy(records[count].quest_name, values[1], MAX_QUEST_NAME_LEN);
            records[count].quest_name[MAX_QUEST_NAME_LEN] = '\0';
        } else {
            records[count].quest_name[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int QuestCategory_BatchDelete(DBConnectionManager* manager, const int* quest_idxs, int count) {
    char query[MAX_QUERY_LEN];
    char in_clause[MAX_QUERY_LEN];
    DBQueryResult result;
    int i, offset = 0;

    if (!manager || !quest_idxs || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 构建 IN 子句 */
    for (i = 0; i < count && offset < sizeof(in_clause) - 20; i++) {
        if (i > 0) {
            offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, ",");
        }
        offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, "%d", quest_idxs[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM quest_category WHERE quest_idx IN (%s)",
        in_clause);

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

void QuestCategory_PrintInfo(const QuestCategory* record) {
    if (!record) {
        printf("QuestCategory: NULL\n");
        return;
    }

    printf("=== Quest Category ===\n");
    printf("  quest_idx: %d\n", record->quest_idx);
    printf("  quest_name: %s\n", record->quest_name);
    printf("======================\n");
}
