#include "pvp_grade_expand_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== CRUD 操作 ==================== */

int PvpGradeExpand_Add(DBConnectionManager* manager, const PvpGradeExpand* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "REPLACE INTO pvp_grade_expand (charac_no, pvp_grade, pvp_point, last_play_time) "
        "VALUES (%d, %d, %d, '%s')",
        record->charac_no,
        record->pvp_grade,
        record->pvp_point,
        record->last_play_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_Get(DBConnectionManager* manager, int charac_no, PvpGradeExpand* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT charac_no, pvp_grade, pvp_point, last_play_time "
        "FROM pvp_grade_expand "
        "WHERE charac_no = %d",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[4];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->pvp_grade = atoi(values[1]);
        record->pvp_point = atoi(values[2]);

        strncpy(record->last_play_time, values[3], sizeof(record->last_play_time) - 1);
        record->last_play_time[sizeof(record->last_play_time) - 1] = '\0';

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int PvpGradeExpand_Update(DBConnectionManager* manager, const PvpGradeExpand* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_expand "
        "SET pvp_grade = %d, pvp_point = %d, last_play_time = '%s' "
        "WHERE charac_no = %d",
        record->pvp_grade,
        record->pvp_point,
        record->last_play_time,
        record->charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_Delete(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM pvp_grade_expand WHERE charac_no = %d",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_Exists(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pvp_grade_expand WHERE charac_no = %d",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int PvpGradeExpand_UpdatePoints(DBConnectionManager* manager, int charac_no, int point_delta) {
    if (!manager) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_expand "
        "SET pvp_point = pvp_point + %d, last_play_time = NOW() "
        "WHERE charac_no = %d",
        point_delta,
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_UpdateGrade(DBConnectionManager* manager, int charac_no, int new_grade) {
    if (!manager) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_expand "
        "SET pvp_grade = %d, last_play_time = NOW() "
        "WHERE charac_no = %d",
        new_grade,
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_expand SET last_play_time = NOW() WHERE charac_no = %d",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvpGradeExpand_Reset(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE pvp_grade_expand "
        "SET pvp_grade = 0, pvp_point = 0, last_play_time = NOW() "
        "WHERE charac_no = %d",
        charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int PvpGradeExpand_GetTopRanking(DBConnectionManager* manager, int limit, PvpGradeExpand* records) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT charac_no, pvp_grade, pvp_point, last_play_time "
        "FROM pvp_grade_expand "
        "ORDER BY pvp_point DESC, pvp_grade DESC "
        "LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[4];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        records[count].charac_no = atoi(values[0]);
        records[count].pvp_grade = atoi(values[1]);
        records[count].pvp_point = atoi(values[2]);

        strncpy(records[count].last_play_time, values[3], sizeof(records[count].last_play_time) - 1);
        records[count].last_play_time[sizeof(records[count].last_play_time) - 1] = '\0';

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeExpand_CountByGrade(DBConnectionManager* manager, int pvp_grade) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pvp_grade_expand WHERE pvp_grade = %d",
        pvp_grade);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeExpand_CountInactivePlayers(DBConnectionManager* manager, int days) {
    if (!manager || days <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pvp_grade_expand "
        "WHERE last_play_time < DATE_SUB(NOW(), INTERVAL %d DAY)",
        days);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeExpand_CountByGradeRange(DBConnectionManager* manager, int min_grade, int max_grade) {
    if (!manager) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pvp_grade_expand "
        "WHERE pvp_grade >= %d AND pvp_grade <= %d",
        min_grade, max_grade);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeExpand_CountByPointRange(DBConnectionManager* manager, int min_point, int max_point) {
    if (!manager) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pvp_grade_expand "
        "WHERE pvp_point >= %d AND pvp_point <= %d",
        min_point, max_point);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int PvpGradeExpand_CountAll(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pvp_grade_expand");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void PvpGradeExpand_PrintInfo(const PvpGradeExpand* record) {
    if (!record) {
        printf("  [PvpGradeExpand] NULL record\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  角色编号: %d\n", record->charac_no);
    printf("  PVP等级: %d\n", record->pvp_grade);
    printf("  PVP积分: %d\n", record->pvp_point);
    printf("  最后游戏时间: %s\n", record->last_play_time);
    printf("  ----------------------------------------\n");
}
