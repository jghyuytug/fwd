#include "knowcafe_writing_stats_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeWritingStats_Add(DBConnectionManager* manager, const KnowcafeWritingStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_writing_stats (date_enroll, tip_sum, tim_recom, qa_sum, qa_recom, job_sum, job_recom, skill_sum, skill_recom, tip_hit, qa_hit, job_hit, skill_hit, discussion_sum, discussion_recom, discussion_hit) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->date_enroll, record->tip_sum, record->tim_recom, record->qa_sum, record->qa_recom, record->job_sum, record->job_recom, record->skill_sum, record->skill_recom, record->tip_hit, record->qa_hit, record->job_hit, record->skill_hit, record->discussion_sum, record->discussion_recom, record->discussion_hit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeWritingStats_Get(DBConnectionManager* manager, const char* date_enroll, KnowcafeWritingStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, tip_sum, tim_recom, qa_sum, qa_recom, job_sum, job_recom, skill_sum, skill_recom, tip_hit, qa_hit, job_hit, skill_hit, discussion_sum, discussion_recom, discussion_hit FROM knowcafe_writing_stats WHERE date_enroll = '%s'",
        date_enroll);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeWritingStats));
    if (row[0]) strncpy(record->date_enroll, row[0], sizeof(record->date_enroll) - 1);
    record->tip_sum = row[1] ? atoi(row[1]) : 0;
    record->tim_recom = row[2] ? atoi(row[2]) : 0;
    record->qa_sum = row[3] ? atoi(row[3]) : 0;
    record->qa_recom = row[4] ? atoi(row[4]) : 0;
    record->job_sum = row[5] ? atoi(row[5]) : 0;
    record->job_recom = row[6] ? atoi(row[6]) : 0;
    record->skill_sum = row[7] ? atoi(row[7]) : 0;
    record->skill_recom = row[8] ? atoi(row[8]) : 0;
    record->tip_hit = row[9] ? atoi(row[9]) : 0;
    record->qa_hit = row[10] ? atoi(row[10]) : 0;
    record->job_hit = row[11] ? atoi(row[11]) : 0;
    record->skill_hit = row[12] ? atoi(row[12]) : 0;
    record->discussion_sum = row[13] ? atoi(row[13]) : 0;
    record->discussion_recom = row[14] ? atoi(row[14]) : 0;
    record->discussion_hit = row[15] ? atoi(row[15]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeWritingStats_GetAll(DBConnectionManager* manager, KnowcafeWritingStats* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, tip_sum, tim_recom, qa_sum, qa_recom, job_sum, job_recom, skill_sum, skill_recom, tip_hit, qa_hit, job_hit, skill_hit, discussion_sum, discussion_recom, discussion_hit FROM knowcafe_writing_stats");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeWritingStats));
        if (row[0]) strncpy(records[count].date_enroll, row[0], sizeof(records[count].date_enroll) - 1);
        records[count].tip_sum = row[1] ? atoi(row[1]) : 0;
        records[count].tim_recom = row[2] ? atoi(row[2]) : 0;
        records[count].qa_sum = row[3] ? atoi(row[3]) : 0;
        records[count].qa_recom = row[4] ? atoi(row[4]) : 0;
        records[count].job_sum = row[5] ? atoi(row[5]) : 0;
        records[count].job_recom = row[6] ? atoi(row[6]) : 0;
        records[count].skill_sum = row[7] ? atoi(row[7]) : 0;
        records[count].skill_recom = row[8] ? atoi(row[8]) : 0;
        records[count].tip_hit = row[9] ? atoi(row[9]) : 0;
        records[count].qa_hit = row[10] ? atoi(row[10]) : 0;
        records[count].job_hit = row[11] ? atoi(row[11]) : 0;
        records[count].skill_hit = row[12] ? atoi(row[12]) : 0;
        records[count].discussion_sum = row[13] ? atoi(row[13]) : 0;
        records[count].discussion_recom = row[14] ? atoi(row[14]) : 0;
        records[count].discussion_hit = row[15] ? atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
