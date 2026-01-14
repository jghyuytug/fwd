#include "charac_dimension_inout_20130613_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int CharacDimensionInout20130613_Add(DBConnectionManager* manager, const CharacDimensionInout20130613* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_dimension_inout_20130613 "
        "(charac_no, dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, "
        "dungeon6, dungeon7, dungeon8, dungeon9, dungeon10) "
        "VALUES (%u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no,
        (int)record->dungeon1, (int)record->dungeon2, (int)record->dungeon3,
        (int)record->dungeon4, (int)record->dungeon5, (int)record->dungeon6,
        (int)record->dungeon7, (int)record->dungeon8, (int)record->dungeon9,
        (int)record->dungeon10);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_Get(DBConnectionManager* manager, unsigned int charac_no, CharacDimensionInout20130613* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, "
        "dungeon6, dungeon7, dungeon8, dungeon9, dungeon10 "
        "FROM charac_dimension_inout_20130613 WHERE charac_no = %u",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  /* No rows or error */
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[11];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    record->charac_no = (unsigned int)atoi(values[0]);
    record->dungeon1 = (char)atoi(values[1]);
    record->dungeon2 = (char)atoi(values[2]);
    record->dungeon3 = (char)atoi(values[3]);
    record->dungeon4 = (char)atoi(values[4]);
    record->dungeon5 = (char)atoi(values[5]);
    record->dungeon6 = (char)atoi(values[6]);
    record->dungeon7 = (char)atoi(values[7]);
    record->dungeon8 = (char)atoi(values[8]);
    record->dungeon9 = (char)atoi(values[9]);
    record->dungeon10 = (char)atoi(values[10]);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_Update(DBConnectionManager* manager, const CharacDimensionInout20130613* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout_20130613 SET "
        "dungeon1=%d, dungeon2=%d, dungeon3=%d, dungeon4=%d, dungeon5=%d, "
        "dungeon6=%d, dungeon7=%d, dungeon8=%d, dungeon9=%d, dungeon10=%d "
        "WHERE charac_no = %u",
        (int)record->dungeon1, (int)record->dungeon2, (int)record->dungeon3,
        (int)record->dungeon4, (int)record->dungeon5, (int)record->dungeon6,
        (int)record->dungeon7, (int)record->dungeon8, (int)record->dungeon9,
        (int)record->dungeon10,
        record->charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_dimension_inout_20130613 WHERE charac_no = %u",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_dimension_inout_20130613 WHERE charac_no = %u LIMIT 1",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作实现 ==================== */

int CharacDimensionInout20130613_SetDungeonStatus(DBConnectionManager* manager, unsigned int charac_no, int dungeon_index, char status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || dungeon_index < DIMENSION_DUNGEON_MIN || dungeon_index > DIMENSION_DUNGEON_MAX) {
        return -1;
    }

    /* 构建动态字段名 */
    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout_20130613 SET dungeon%d = %d WHERE charac_no = %u",
        dungeon_index, (int)status, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_GetDungeonStatus(DBConnectionManager* manager, unsigned int charac_no, int dungeon_index, char* status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !status || dungeon_index < DIMENSION_DUNGEON_MIN || dungeon_index > DIMENSION_DUNGEON_MAX) {
        return -1;
    }

    /* 构建动态字段名 */
    snprintf(query, sizeof(query),
        "SELECT dungeon%d FROM charac_dimension_inout_20130613 WHERE charac_no = %u",
        dungeon_index, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  /* No rows or error */
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *status = (char)atoi(values[0]);
    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_SetAllDungeons(DBConnectionManager* manager, unsigned int charac_no, char status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout_20130613 SET "
        "dungeon1=%d, dungeon2=%d, dungeon3=%d, dungeon4=%d, dungeon5=%d, "
        "dungeon6=%d, dungeon7=%d, dungeon8=%d, dungeon9=%d, dungeon10=%d "
        "WHERE charac_no = %u",
        (int)status, (int)status, (int)status, (int)status, (int)status,
        (int)status, (int)status, (int)status, (int)status, (int)status,
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout20130613_ClearAllDungeons(DBConnectionManager* manager, unsigned int charac_no) {
    return CharacDimensionInout20130613_SetAllDungeons(manager, charac_no, DUNGEON_STATUS_NOT_ENTERED);
}

int CharacDimensionInout20130613_GetCompletedCount(DBConnectionManager* manager, unsigned int charac_no, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT (dungeon1 + dungeon2 + dungeon3 + dungeon4 + dungeon5 + "
        "dungeon6 + dungeon7 + dungeon8 + dungeon9 + dungeon10) AS completed_count "
        "FROM charac_dimension_inout_20130613 WHERE charac_no = %u",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  /* No rows or error */
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacDimensionInout20130613_CountByCompletedDungeons(DBConnectionManager* manager, int completed_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || completed_count < 0 || completed_count > DIMENSION_DUNGEON_COUNT) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dimension_inout_20130613 "
        "WHERE (dungeon1 + dungeon2 + dungeon3 + dungeon4 + dungeon5 + "
        "dungeon6 + dungeon7 + dungeon8 + dungeon9 + dungeon10) = %d",
        completed_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int CharacDimensionInout20130613_CountByDungeonStatus(DBConnectionManager* manager, int dungeon_index, char status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || dungeon_index < DIMENSION_DUNGEON_MIN || dungeon_index > DIMENSION_DUNGEON_MAX) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dimension_inout_20130613 WHERE dungeon%d = %d",
        dungeon_index, (int)status);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int CharacDimensionInout20130613_CountTotal(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dimension_inout_20130613");

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int CharacDimensionInout20130613_CountFullyCompleted(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dimension_inout_20130613 "
        "WHERE (dungeon1 + dungeon2 + dungeon3 + dungeon4 + dungeon5 + "
        "dungeon6 + dungeon7 + dungeon8 + dungeon9 + dungeon10) = %d",
        DIMENSION_DUNGEON_COUNT);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void CharacDimensionInout20130613_PrintInfo(const CharacDimensionInout20130613* record) {
    if (!record) {
        printf("CharacDimensionInout20130613: NULL\n");
        return;
    }

    printf("CharacDimensionInout20130613:\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  副本状态: [");

    /* 打印所有副本状态 */
    printf("D1:%d D2:%d D3:%d D4:%d D5:%d D6:%d D7:%d D8:%d D9:%d D10:%d",
        (int)record->dungeon1, (int)record->dungeon2, (int)record->dungeon3,
        (int)record->dungeon4, (int)record->dungeon5, (int)record->dungeon6,
        (int)record->dungeon7, (int)record->dungeon8, (int)record->dungeon9,
        (int)record->dungeon10);
    printf("]\n");

    /* 统计已完成数量 */
    int completed = (int)record->dungeon1 + (int)record->dungeon2 + (int)record->dungeon3 +
                    (int)record->dungeon4 + (int)record->dungeon5 + (int)record->dungeon6 +
                    (int)record->dungeon7 + (int)record->dungeon8 + (int)record->dungeon9 +
                    (int)record->dungeon10;
    printf("  已完成副本数: %d/%d (%.1f%%)\n",
        completed, DIMENSION_DUNGEON_COUNT, (completed * 100.0) / DIMENSION_DUNGEON_COUNT);
}
