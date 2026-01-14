#include "community_discussion_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussion_Add(DBConnectionManager* manager, const CommunityDiscussion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion (ask_no, notice, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, modi_day, comment, view, recom, no_recom, sequence) "
        "VALUES (%d, '%s', %d, '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %f)",
        record->ask_no, record->notice, record->m_id, record->m_user_id, record->m_sex, record->server_id, record->charac_no, record->charac_name, record->job, record->title, record->create_day, record->modi_day, record->comment, record->view, record->recom, record->no_recom, record->sequence);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussion_Get(DBConnectionManager* manager, int no, CommunityDiscussion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, notice, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, modi_day, comment, view, recom, no_recom, sequence FROM community_discussion WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussion));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->ask_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->notice, row[2], sizeof(record->notice) - 1);
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->m_user_id, row[4], sizeof(record->m_user_id) - 1);
    if (row[5]) strncpy(record->m_sex, row[5], sizeof(record->m_sex) - 1);
    record->server_id = row[6] ? atoi(row[6]) : 0;
    record->charac_no = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->charac_name, row[8], sizeof(record->charac_name) - 1);
    record->job = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->title, row[10], sizeof(record->title) - 1);
    record->create_day = row[11] ? atoi(row[11]) : 0;
    record->modi_day = row[12] ? atoi(row[12]) : 0;
    record->comment = row[13] ? atoi(row[13]) : 0;
    record->view = row[14] ? atoi(row[14]) : 0;
    record->recom = row[15] ? atoi(row[15]) : 0;
    record->no_recom = row[16] ? atoi(row[16]) : 0;
    record->sequence = row[17] ? atof(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussion_GetAll(DBConnectionManager* manager, CommunityDiscussion* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, ask_no, notice, m_id, m_user_id, m_sex, server_id, charac_no, charac_name, job, title, create_day, modi_day, comment, view, recom, no_recom, sequence FROM community_discussion");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussion));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].ask_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].notice, row[2], sizeof(records[count].notice) - 1);
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].m_user_id, row[4], sizeof(records[count].m_user_id) - 1);
        if (row[5]) strncpy(records[count].m_sex, row[5], sizeof(records[count].m_sex) - 1);
        records[count].server_id = row[6] ? atoi(row[6]) : 0;
        records[count].charac_no = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].charac_name, row[8], sizeof(records[count].charac_name) - 1);
        records[count].job = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].title, row[10], sizeof(records[count].title) - 1);
        records[count].create_day = row[11] ? atoi(row[11]) : 0;
        records[count].modi_day = row[12] ? atoi(row[12]) : 0;
        records[count].comment = row[13] ? atoi(row[13]) : 0;
        records[count].view = row[14] ? atoi(row[14]) : 0;
        records[count].recom = row[15] ? atoi(row[15]) : 0;
        records[count].no_recom = row[16] ? atoi(row[16]) : 0;
        records[count].sequence = row[17] ? atof(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
