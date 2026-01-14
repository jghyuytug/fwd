#include "charac_blood_best_record_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacBloodBestRecord_Add(DBConnectionManager* manager, const CharacBloodBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_blood_best_record "
        "(charac_no, dungeon_index, best_round, best_time) VALUES "
        "(%u, %u, %u, %d)",
        record->charac_no, record->dungeon_index, record->best_round, record->best_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodBestRecord_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index,
                               CharacBloodBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[4];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_index, best_round, best_time "
        "FROM charac_blood_best_record "
        "WHERE charac_no = %u AND dungeon_index = %u",
        charac_no, dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->dungeon_index = (unsigned int)atoi(values[1]);
        record->best_round = (unsigned char)atoi(values[2]);
        record->best_time = atoi(values[3]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacBloodBestRecord_Update(DBConnectionManager* manager, const CharacBloodBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_blood_best_record SET "
        "best_round = %u, best_time = %d "
        "WHERE charac_no = %u AND dungeon_index = %u",
        record->best_round, record->best_time,
        record->charac_no, record->dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodBestRecord_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_blood_best_record "
        "WHERE charac_no = %u AND dungeon_index = %u",
        charac_no, dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodBestRecord_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_best_record "
        "WHERE charac_no = %u AND dungeon_index = %u",
        charac_no, dungeon_index);

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

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int CharacBloodBestRecord_GetByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                      CharacBloodBestRecord* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[4];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_index, best_round, best_time "
        "FROM charac_blood_best_record "
        "WHERE charac_no = %u "
        "ORDER BY dungeon_index LIMIT %d",
        charac_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].dungeon_index = (unsigned int)atoi(values[1]);
        records[count].best_round = (unsigned char)atoi(values[2]);
        records[count].best_time = atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodBestRecord_GetByDungeon(DBConnectionManager* manager, unsigned int dungeon_index,
                                       CharacBloodBestRecord* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[4];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_index, best_round, best_time "
        "FROM charac_blood_best_record "
        "WHERE dungeon_index = %u "
        "ORDER BY best_time ASC LIMIT %d",
        dungeon_index, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].dungeon_index = (unsigned int)atoi(values[1]);
        records[count].best_round = (unsigned char)atoi(values[2]);
        records[count].best_time = atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodBestRecord_GetTopByTime(DBConnectionManager* manager, unsigned int dungeon_index, int limit,
                                       CharacBloodBestRecord* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[4];
    int count = 0;

    if (!manager || !records || max_records <= 0 || limit <= 0) {
        return -1;
    }

    int actual_limit = (limit < max_records) ? limit : max_records;

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_index, best_round, best_time "
        "FROM charac_blood_best_record "
        "WHERE dungeon_index = %u "
        "ORDER BY best_time ASC LIMIT %d",
        dungeon_index, actual_limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].dungeon_index = (unsigned int)atoi(values[1]);
        records[count].best_round = (unsigned char)atoi(values[2]);
        records[count].best_time = atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodBestRecord_GetTopByRound(DBConnectionManager* manager, unsigned int dungeon_index, int limit,
                                        CharacBloodBestRecord* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[4];
    int count = 0;

    if (!manager || !records || max_records <= 0 || limit <= 0) {
        return -1;
    }

    int actual_limit = (limit < max_records) ? limit : max_records;

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_index, best_round, best_time "
        "FROM charac_blood_best_record "
        "WHERE dungeon_index = %u "
        "ORDER BY best_round DESC LIMIT %d",
        dungeon_index, actual_limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].dungeon_index = (unsigned int)atoi(values[1]);
        records[count].best_round = (unsigned char)atoi(values[2]);
        records[count].best_time = atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodBestRecord_UpdateIfBetter(DBConnectionManager* manager, const CharacBloodBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    CharacBloodBestRecord existing;

    if (!manager || !record) {
        return -1;
    }

    /* 检查是否存在记录 */
    int exists = CharacBloodBestRecord_Get(manager, record->charac_no, record->dungeon_index, &existing);

    if (exists < 0) {
        /* 不存在，直接插入 */
        return CharacBloodBestRecord_Add(manager, record);
    } else {
        /* 存在，比较是否更好 */
        if (record->best_time < existing.best_time || record->best_round > existing.best_round) {
            /* 新记录更好，更新 */
            return CharacBloodBestRecord_Update(manager, record);
        }
        /* 旧记录更好，不更新 */
        return 0;
    }
}

/*******************************************************************************
 * 统计操作实现
 ******************************************************************************/

int CharacBloodBestRecord_CountByCharac(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_best_record WHERE charac_no = %u",
        charac_no);

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

int CharacBloodBestRecord_CountByDungeon(DBConnectionManager* manager, unsigned int dungeon_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_best_record WHERE dungeon_index = %u",
        dungeon_index);

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

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacBloodBestRecord_PrintInfo(const CharacBloodBestRecord* record) {
    if (!record) {
        printf("CharacBloodBestRecord: NULL\n");
        return;
    }

    printf("\n========== 角色血色副本最佳记录 ==========\n");
    printf("角色编号:       %u\n", record->charac_no);
    printf("副本索引:       %u\n", record->dungeon_index);
    printf("最佳回合数:     %u\n", record->best_round);
    printf("最佳时间:       %d ms (%.2f 秒)\n", record->best_time, record->best_time / 1000.0);
    printf("========================================\n\n");
}
