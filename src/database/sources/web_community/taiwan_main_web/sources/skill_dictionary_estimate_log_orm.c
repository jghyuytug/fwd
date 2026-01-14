#include "skill_dictionary_estimate_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SkillDictionaryEstimateLog_Add(DBConnectionManager* manager, const SkillDictionaryEstimateLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO skill_dictionary_estimate_log (m_id, m_user_id, charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, create_day) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d)",
        record->m_id, record->m_user_id, record->charac_idx, record->skill_group, record->skill_idx, record->dungeon_cnt, record->challenge_cnt, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SkillDictionaryEstimateLog_Get(DBConnectionManager* manager, int no, SkillDictionaryEstimateLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, m_user_id, charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, create_day FROM skill_dictionary_estimate_log WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SkillDictionaryEstimateLog));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->m_user_id, row[2], sizeof(record->m_user_id) - 1);
    record->charac_idx = row[3] ? atoi(row[3]) : 0;
    record->skill_group = row[4] ? atoi(row[4]) : 0;
    record->skill_idx = row[5] ? atoi(row[5]) : 0;
    record->dungeon_cnt = row[6] ? atoi(row[6]) : 0;
    record->challenge_cnt = row[7] ? atoi(row[7]) : 0;
    record->create_day = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SkillDictionaryEstimateLog_GetAll(DBConnectionManager* manager, SkillDictionaryEstimateLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, m_user_id, charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, create_day FROM skill_dictionary_estimate_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SkillDictionaryEstimateLog));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].m_user_id, row[2], sizeof(records[count].m_user_id) - 1);
        records[count].charac_idx = row[3] ? atoi(row[3]) : 0;
        records[count].skill_group = row[4] ? atoi(row[4]) : 0;
        records[count].skill_idx = row[5] ? atoi(row[5]) : 0;
        records[count].dungeon_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].challenge_cnt = row[7] ? atoi(row[7]) : 0;
        records[count].create_day = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
