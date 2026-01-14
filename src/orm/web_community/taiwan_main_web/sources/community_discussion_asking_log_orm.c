#include "community_discussion_asking_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussionAskingLog_Add(DBConnectionManager* manager, const CommunityDiscussionAskingLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion_asking_log (ask_no, ask_list_no, m_id, reg_date) "
        "VALUES (%d, %d, %d, %d)",
        record->ask_no, record->ask_list_no, record->m_id, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAskingLog_Get(DBConnectionManager* manager, int no, CommunityDiscussionAskingLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, ask_list_no, m_id, reg_date FROM community_discussion_asking_log WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussionAskingLog));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->ask_no = row[1] ? atoi(row[1]) : 0;
    record->ask_list_no = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    record->reg_date = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionAskingLog_GetAll(DBConnectionManager* manager, CommunityDiscussionAskingLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, ask_list_no, m_id, reg_date FROM community_discussion_asking_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussionAskingLog));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].ask_no = row[1] ? atoi(row[1]) : 0;
        records[count].ask_list_no = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        records[count].reg_date = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
