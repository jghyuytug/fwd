#include "charac_tower_record_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

/* ==================== CRUD 操作实现 ==================== */

int CharacTowerRecord_Add(DBConnectionManager* manager, const CharacTowerRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_tower_record "
        "(charac_no, tower_index, "
        "member_info_1, stage_1, play_time_1, occ_time_1, "
        "member_info_2, stage_2, play_time_2, occ_time_2, "
        "member_info_3, stage_3, play_time_3, occ_time_3, "
        "member_info_4, stage_4, play_time_4, occ_time_4) "
        "VALUES (%d, %u, "
        "'%s', %u, %u, '%s', "
        "'%s', %u, %u, '%s', "
        "'%s', %u, %u, '%s', "
        "'%s', %u, %u, '%s')",
        record->charac_no, (unsigned int)record->tower_index,
        record->member_info_1, (unsigned int)record->stage_1, record->play_time_1, record->occ_time_1,
        record->member_info_2, (unsigned int)record->stage_2, record->play_time_2, record->occ_time_2,
        record->member_info_3, (unsigned int)record->stage_3, record->play_time_3, record->occ_time_3,
        record->member_info_4, (unsigned int)record->stage_4, record->play_time_4, record->occ_time_4);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_Get(DBConnectionManager* manager, int charac_no, CharacTowerRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[32];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTowerRecord));

    snprintf(query, sizeof(query),
        "SELECT charac_no, tower_index, "
        "member_info_1, stage_1, play_time_1, occ_time_1, "
        "member_info_2, stage_2, play_time_2, occ_time_2, "
        "member_info_3, stage_3, play_time_3, occ_time_3, "
        "member_info_4, stage_4, play_time_4, occ_time_4 "
        "FROM charac_tower_record WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->tower_index = (unsigned char)atoi(values[1]);

        if (values[2]) strncpy(record->member_info_1, values[2], sizeof(record->member_info_1) - 1);
        record->stage_1 = (unsigned char)atoi(values[3]);
        record->play_time_1 = (unsigned int)atoi(values[4]);
        if (values[5]) strncpy(record->occ_time_1, values[5], sizeof(record->occ_time_1) - 1);

        if (values[6]) strncpy(record->member_info_2, values[6], sizeof(record->member_info_2) - 1);
        record->stage_2 = (unsigned char)atoi(values[7]);
        record->play_time_2 = (unsigned int)atoi(values[8]);
        if (values[9]) strncpy(record->occ_time_2, values[9], sizeof(record->occ_time_2) - 1);

        if (values[10]) strncpy(record->member_info_3, values[10], sizeof(record->member_info_3) - 1);
        record->stage_3 = (unsigned char)atoi(values[11]);
        record->play_time_3 = (unsigned int)atoi(values[12]);
        if (values[13]) strncpy(record->occ_time_3, values[13], sizeof(record->occ_time_3) - 1);

        if (values[14]) strncpy(record->member_info_4, values[14], sizeof(record->member_info_4) - 1);
        record->stage_4 = (unsigned char)atoi(values[15]);
        record->play_time_4 = (unsigned int)atoi(values[16]);
        if (values[17]) strncpy(record->occ_time_4, values[17], sizeof(record->occ_time_4) - 1);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerRecord_Update(DBConnectionManager* manager, const CharacTowerRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET "
        "tower_index = %u, "
        "member_info_1 = '%s', stage_1 = %u, play_time_1 = %u, occ_time_1 = '%s', "
        "member_info_2 = '%s', stage_2 = %u, play_time_2 = %u, occ_time_2 = '%s', "
        "member_info_3 = '%s', stage_3 = %u, play_time_3 = %u, occ_time_3 = '%s', "
        "member_info_4 = '%s', stage_4 = %u, play_time_4 = %u, occ_time_4 = '%s' "
        "WHERE charac_no = %d",
        (unsigned int)record->tower_index,
        record->member_info_1, (unsigned int)record->stage_1, record->play_time_1, record->occ_time_1,
        record->member_info_2, (unsigned int)record->stage_2, record->play_time_2, record->occ_time_2,
        record->member_info_3, (unsigned int)record->stage_3, record->play_time_3, record->occ_time_3,
        record->member_info_4, (unsigned int)record->stage_4, record->play_time_4, record->occ_time_4,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_record WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_record WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/* ==================== 专项操作实现 ==================== */

int CharacTowerRecord_UpdateMemberRecord(DBConnectionManager* manager, int charac_no, int member_size,
                                          const char* member_info, unsigned char stage,
                                          unsigned int play_time, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member_info || !occ_time || member_size < 1 || member_size > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET "
        "member_info_%d = '%s', stage_%d = %u, play_time_%d = %u, occ_time_%d = '%s' "
        "WHERE charac_no = %d",
        member_size, member_info, member_size, (unsigned int)stage,
        member_size, play_time, member_size, occ_time, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_UpdateMemberInfo(DBConnectionManager* manager, int charac_no, int member_size,
                                        const char* member_info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member_info || member_size < 1 || member_size > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET member_info_%d = '%s' WHERE charac_no = %d",
        member_size, member_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_UpdateStage(DBConnectionManager* manager, int charac_no, int member_size,
                                   unsigned char stage) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || member_size < 1 || member_size > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET stage_%d = %u WHERE charac_no = %d",
        member_size, (unsigned int)stage, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_UpdatePlayTime(DBConnectionManager* manager, int charac_no, int member_size,
                                      unsigned int play_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || member_size < 1 || member_size > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET play_time_%d = %u WHERE charac_no = %d",
        member_size, play_time, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRecord_UpdateTowerIndex(DBConnectionManager* manager, int charac_no, unsigned char tower_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_record SET tower_index = %u WHERE charac_no = %d",
        (unsigned int)tower_index, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacTowerRecord_GetByTowerIndex(DBConnectionManager* manager, unsigned char tower_index,
                                       CharacTowerRecord* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[32];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, tower_index, "
        "member_info_1, stage_1, play_time_1, occ_time_1, "
        "member_info_2, stage_2, play_time_2, occ_time_2, "
        "member_info_3, stage_3, play_time_3, occ_time_3, "
        "member_info_4, stage_4, play_time_4, occ_time_4 "
        "FROM charac_tower_record WHERE tower_index = %u LIMIT %d",
        (unsigned int)tower_index, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].charac_no = atoi(values[0]);
        records[count].tower_index = (unsigned char)atoi(values[1]);

        if (values[2]) strncpy(records[count].member_info_1, values[2], sizeof(records[count].member_info_1) - 1);
        records[count].stage_1 = (unsigned char)atoi(values[3]);
        records[count].play_time_1 = (unsigned int)atoi(values[4]);
        if (values[5]) strncpy(records[count].occ_time_1, values[5], sizeof(records[count].occ_time_1) - 1);

        if (values[6]) strncpy(records[count].member_info_2, values[6], sizeof(records[count].member_info_2) - 1);
        records[count].stage_2 = (unsigned char)atoi(values[7]);
        records[count].play_time_2 = (unsigned int)atoi(values[8]);
        if (values[9]) strncpy(records[count].occ_time_2, values[9], sizeof(records[count].occ_time_2) - 1);

        if (values[10]) strncpy(records[count].member_info_3, values[10], sizeof(records[count].member_info_3) - 1);
        records[count].stage_3 = (unsigned char)atoi(values[11]);
        records[count].play_time_3 = (unsigned int)atoi(values[12]);
        if (values[13]) strncpy(records[count].occ_time_3, values[13], sizeof(records[count].occ_time_3) - 1);

        if (values[14]) strncpy(records[count].member_info_4, values[14], sizeof(records[count].member_info_4) - 1);
        records[count].stage_4 = (unsigned char)atoi(values[15]);
        records[count].play_time_4 = (unsigned int)atoi(values[16]);
        if (values[17]) strncpy(records[count].occ_time_4, values[17], sizeof(records[count].occ_time_4) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerRecord_GetFastestByMemberSize(DBConnectionManager* manager, int member_size, int limit,
                                               CharacTowerRecord* records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[32];
    int count = 0;

    if (!manager || !records || limit <= 0 || member_size < 1 || member_size > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, tower_index, "
        "member_info_1, stage_1, play_time_1, occ_time_1, "
        "member_info_2, stage_2, play_time_2, occ_time_2, "
        "member_info_3, stage_3, play_time_3, occ_time_3, "
        "member_info_4, stage_4, play_time_4, occ_time_4 "
        "FROM charac_tower_record WHERE play_time_%d > 0 ORDER BY play_time_%d ASC LIMIT %d",
        member_size, member_size, limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        records[count].charac_no = atoi(values[0]);
        records[count].tower_index = (unsigned char)atoi(values[1]);

        if (values[2]) strncpy(records[count].member_info_1, values[2], sizeof(records[count].member_info_1) - 1);
        records[count].stage_1 = (unsigned char)atoi(values[3]);
        records[count].play_time_1 = (unsigned int)atoi(values[4]);
        if (values[5]) strncpy(records[count].occ_time_1, values[5], sizeof(records[count].occ_time_1) - 1);

        if (values[6]) strncpy(records[count].member_info_2, values[6], sizeof(records[count].member_info_2) - 1);
        records[count].stage_2 = (unsigned char)atoi(values[7]);
        records[count].play_time_2 = (unsigned int)atoi(values[8]);
        if (values[9]) strncpy(records[count].occ_time_2, values[9], sizeof(records[count].occ_time_2) - 1);

        if (values[10]) strncpy(records[count].member_info_3, values[10], sizeof(records[count].member_info_3) - 1);
        records[count].stage_3 = (unsigned char)atoi(values[11]);
        records[count].play_time_3 = (unsigned int)atoi(values[12]);
        if (values[13]) strncpy(records[count].occ_time_3, values[13], sizeof(records[count].occ_time_3) - 1);

        if (values[14]) strncpy(records[count].member_info_4, values[14], sizeof(records[count].member_info_4) - 1);
        records[count].stage_4 = (unsigned char)atoi(values[15]);
        records[count].play_time_4 = (unsigned int)atoi(values[16]);
        if (values[17]) strncpy(records[count].occ_time_4, values[17], sizeof(records[count].occ_time_4) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerRecord_GetCountByTowerIndex(DBConnectionManager* manager, unsigned char tower_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_record WHERE tower_index = %u",
        (unsigned int)tower_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerRecord_DeleteByTowerIndex(DBConnectionManager* manager, unsigned char tower_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_record WHERE tower_index = %u",
        (unsigned int)tower_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void CharacTowerRecord_PrintInfo(const CharacTowerRecord* record) {
    if (!record) {
        printf("CharacTowerRecord: NULL\n");
        return;
    }

    printf("CharacTowerRecord {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  tower_index: %u\n", (unsigned int)record->tower_index);
    printf("  [1人] member_info: %s, stage: %u, play_time: %u, occ_time: %s\n",
           record->member_info_1, (unsigned int)record->stage_1,
           record->play_time_1, record->occ_time_1);
    printf("  [2人] member_info: %s, stage: %u, play_time: %u, occ_time: %s\n",
           record->member_info_2, (unsigned int)record->stage_2,
           record->play_time_2, record->occ_time_2);
    printf("  [3人] member_info: %s, stage: %u, play_time: %u, occ_time: %s\n",
           record->member_info_3, (unsigned int)record->stage_3,
           record->play_time_3, record->occ_time_3);
    printf("  [4人] member_info: %s, stage: %u, play_time: %u, occ_time: %s\n",
           record->member_info_4, (unsigned int)record->stage_4,
           record->play_time_4, record->occ_time_4);
    printf("}\n");
}
