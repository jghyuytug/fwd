#include "pvp_grade_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int PvpGradeRef_Add(DBConnectionManager* manager, const PvpGradeRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO pvp_grade_ref (grade, limit_pts) VALUES (%d, %d)",
        record->grade, record->limit_pts);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeRef_Get(DBConnectionManager* manager, int grade, PvpGradeRef* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(PvpGradeRef));

    snprintf(query, sizeof(query),
        "SELECT grade, limit_pts FROM pvp_grade_ref WHERE grade = %d",
        grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->grade = atoi(values[0]);
        record->limit_pts = atoi(values[1]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int PvpGradeRef_Update(DBConnectionManager* manager, const PvpGradeRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_ref SET limit_pts = %d WHERE grade = %d",
        record->limit_pts, record->grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeRef_Delete(DBConnectionManager* manager, int grade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM pvp_grade_ref WHERE grade = %d",
        grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeRef_Exists(DBConnectionManager* manager, int grade) {
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
        "SELECT COUNT(*) FROM pvp_grade_ref WHERE grade = %d",
        grade);

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

int PvpGradeRef_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pvp_grade_ref");

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

int PvpGradeRef_GetAll(DBConnectionManager* manager, PvpGradeRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT grade, limit_pts FROM pvp_grade_ref ORDER BY grade LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].grade = atoi(values[0]);
        records[count].limit_pts = atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeRef_GetByPoints(DBConnectionManager* manager, int points, PvpGradeRef* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(PvpGradeRef));

    /* 查找积分所能达到的最高等级 */
    snprintf(query, sizeof(query),
        "SELECT grade, limit_pts FROM pvp_grade_ref WHERE limit_pts <= %d ORDER BY grade DESC LIMIT 1",
        points);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->grade = atoi(values[0]);
        record->limit_pts = atoi(values[1]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void PvpGradeRef_PrintInfo(const PvpGradeRef* record) {
    if (!record) {
        printf("PvpGradeRef: NULL\n");
        return;
    }

    printf("=== PVP Grade Ref ===\n");
    printf("  grade: %d\n", record->grade);
    printf("  limit_pts: %d\n", record->limit_pts);
    printf("=====================\n");
}
