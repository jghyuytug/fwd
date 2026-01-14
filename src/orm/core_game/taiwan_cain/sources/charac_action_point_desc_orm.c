#include "charac_action_point_desc_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacActionPointDesc_Add(DBConnectionManager* manager, const CharacActionPointDesc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_action_point_desc (action_group_index, action_index, action_group_name) VALUES (%d, %d, '%s')",
        record->action_group_index, record->action_index, record->action_group_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPointDesc_Get(DBConnectionManager* manager, int action_group_index, int action_index, CharacActionPointDesc* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(CharacActionPointDesc));

    snprintf(query, sizeof(query),
        "SELECT action_group_index, action_index, action_group_name FROM charac_action_point_desc WHERE action_group_index = %d AND action_index = %d",
        action_group_index, action_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->action_group_index = atoi(values[0]);
        record->action_index = atoi(values[1]);
        if (values[2]) {
            strncpy(record->action_group_name, values[2], sizeof(record->action_group_name) - 1);
            record->action_group_name[sizeof(record->action_group_name) - 1] = '\0';
        }
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacActionPointDesc_Update(DBConnectionManager* manager, const CharacActionPointDesc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_action_point_desc SET action_group_name = '%s' WHERE action_group_index = %d AND action_index = %d",
        record->action_group_name, record->action_group_index, record->action_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPointDesc_Delete(DBConnectionManager* manager, int action_group_index, int action_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM charac_action_point_desc WHERE action_group_index = %d AND action_index = %d",
        action_group_index, action_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPointDesc_Exists(DBConnectionManager* manager, int action_group_index, int action_index) {
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
        "SELECT COUNT(*) FROM charac_action_point_desc WHERE action_group_index = %d AND action_index = %d",
        action_group_index, action_index);

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

int CharacActionPointDesc_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_action_point_desc");

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

int CharacActionPointDesc_GetAll(DBConnectionManager* manager, CharacActionPointDesc* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT action_group_index, action_index, action_group_name FROM charac_action_point_desc ORDER BY action_group_index, action_index LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].action_group_index = atoi(values[0]);
        records[count].action_index = atoi(values[1]);
        if (values[2]) {
            strncpy(records[count].action_group_name, values[2], sizeof(records[count].action_group_name) - 1);
            records[count].action_group_name[sizeof(records[count].action_group_name) - 1] = '\0';
        } else {
            records[count].action_group_name[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacActionPointDesc_GetByGroupIndex(DBConnectionManager* manager, int action_group_index, CharacActionPointDesc* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT action_group_index, action_index, action_group_name FROM charac_action_point_desc WHERE action_group_index = %d ORDER BY action_index LIMIT %d",
        action_group_index, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].action_group_index = atoi(values[0]);
        records[count].action_index = atoi(values[1]);
        if (values[2]) {
            strncpy(records[count].action_group_name, values[2], sizeof(records[count].action_group_name) - 1);
            records[count].action_group_name[sizeof(records[count].action_group_name) - 1] = '\0';
        } else {
            records[count].action_group_name[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacActionPointDesc_CountByGroup(DBConnectionManager* manager, int action_group_index) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_action_point_desc WHERE action_group_index = %d",
        action_group_index);

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

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int CharacActionPointDesc_BatchDelete(DBConnectionManager* manager, const CharacActionPointDesc* keys, int count) {
    char query[MAX_QUERY_LEN];
    char condition[MAX_QUERY_LEN];
    DBQueryResult result;
    int i, offset = 0;

    if (!manager || !keys || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 构建 WHERE 条件 */
    for (i = 0; i < count && offset < sizeof(condition) - 100; i++) {
        if (i > 0) {
            offset += snprintf(condition + offset, sizeof(condition) - offset, " OR ");
        }
        offset += snprintf(condition + offset, sizeof(condition) - offset,
            "(action_group_index = %d AND action_index = %d)",
            keys[i].action_group_index, keys[i].action_index);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_action_point_desc WHERE %s",
        condition);

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

void CharacActionPointDesc_PrintInfo(const CharacActionPointDesc* record) {
    if (!record) {
        printf("CharacActionPointDesc: NULL\n");
        return;
    }

    printf("=== Charac Action Point Desc ===\n");
    printf("  action_group_index: %d\n", record->action_group_index);
    printf("  action_index: %d\n", record->action_index);
    printf("  action_group_name: %s\n", record->action_group_name);
    printf("================================\n");
}
