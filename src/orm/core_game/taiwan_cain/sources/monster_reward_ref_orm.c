#include "monster_reward_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int MonsterRewardRef_Add(DBConnectionManager* manager, const MonsterRewardRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO monster_reward_ref (level, exp) VALUES (%d, %d)",
        record->level, record->exp);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonsterRewardRef_Get(DBConnectionManager* manager, short level, MonsterRewardRef* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(MonsterRewardRef));

    snprintf(query, sizeof(query),
        "SELECT level, exp FROM monster_reward_ref WHERE level = %d",
        level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->level = (short)atoi(values[0]);
        record->exp = atoi(values[1]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MonsterRewardRef_Update(DBConnectionManager* manager, const MonsterRewardRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE monster_reward_ref SET exp = %d WHERE level = %d",
        record->exp, record->level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonsterRewardRef_Delete(DBConnectionManager* manager, short level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM monster_reward_ref WHERE level = %d",
        level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonsterRewardRef_Exists(DBConnectionManager* manager, short level) {
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
        "SELECT COUNT(*) FROM monster_reward_ref WHERE level = %d",
        level);

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

int MonsterRewardRef_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM monster_reward_ref");

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

int MonsterRewardRef_GetAll(DBConnectionManager* manager, MonsterRewardRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT level, exp FROM monster_reward_ref ORDER BY level LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].level = (short)atoi(values[0]);
        records[count].exp = atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int MonsterRewardRef_BatchDelete(DBConnectionManager* manager, const short* levels, int count) {
    char query[MAX_QUERY_LEN];
    char in_clause[MAX_QUERY_LEN];
    DBQueryResult result;
    int i, offset = 0;

    if (!manager || !levels || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 构建 IN 子句 */
    for (i = 0; i < count && offset < sizeof(in_clause) - 20; i++) {
        if (i > 0) {
            offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, ",");
        }
        offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, "%d", levels[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM monster_reward_ref WHERE level IN (%s)",
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

void MonsterRewardRef_PrintInfo(const MonsterRewardRef* record) {
    if (!record) {
        printf("MonsterRewardRef: NULL\n");
        return;
    }

    printf("=== Monster Reward Ref ===\n");
    printf("  level: %d\n", record->level);
    printf("  exp: %d\n", record->exp);
    printf("==========================\n");
}
