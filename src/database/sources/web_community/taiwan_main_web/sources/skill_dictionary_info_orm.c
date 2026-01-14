#include "skill_dictionary_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SkillDictionaryInfo_Add(DBConnectionManager* manager, const SkillDictionaryInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO skill_dictionary_info (charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, comment_cnt, estimate_cnt) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d)",
        record->charac_idx, record->skill_group, record->skill_idx, record->dungeon_cnt, record->challenge_cnt, record->comment_cnt, record->estimate_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SkillDictionaryInfo_Get(DBConnectionManager* manager, int no, SkillDictionaryInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, comment_cnt, estimate_cnt FROM skill_dictionary_info WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SkillDictionaryInfo));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->charac_idx = row[1] ? atoi(row[1]) : 0;
    record->skill_group = row[2] ? atoi(row[2]) : 0;
    record->skill_idx = row[3] ? atoi(row[3]) : 0;
    record->dungeon_cnt = row[4] ? atoi(row[4]) : 0;
    record->challenge_cnt = row[5] ? atoi(row[5]) : 0;
    record->comment_cnt = row[6] ? atoi(row[6]) : 0;
    record->estimate_cnt = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SkillDictionaryInfo_GetAll(DBConnectionManager* manager, SkillDictionaryInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, charac_idx, skill_group, skill_idx, dungeon_cnt, challenge_cnt, comment_cnt, estimate_cnt FROM skill_dictionary_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SkillDictionaryInfo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].charac_idx = row[1] ? atoi(row[1]) : 0;
        records[count].skill_group = row[2] ? atoi(row[2]) : 0;
        records[count].skill_idx = row[3] ? atoi(row[3]) : 0;
        records[count].dungeon_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].challenge_cnt = row[5] ? atoi(row[5]) : 0;
        records[count].comment_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].estimate_cnt = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
