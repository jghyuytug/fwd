#include "dnf_league_bbs_comments_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfLeagueBbsComments_Add(DBConnectionManager* manager, const DnfLeagueBbsComments* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_league_bbs_comments (bbs_id, m_id, server_id, charac_no, charac_name, job, comments, reg_time, delete_flag) "
        "VALUES (%d, %d, %d, %d, '%s', %d, '%s', %d, %d)",
        record->bbs_id, record->m_id, record->server_id, record->charac_no, record->charac_name, record->job, record->comments, record->reg_time, record->delete_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsComments_Get(DBConnectionManager* manager, int comment_id, DnfLeagueBbsComments* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT comment_id, bbs_id, m_id, server_id, charac_no, charac_name, job, comments, reg_time, delete_flag FROM dnf_league_bbs_comments WHERE comment_id = %d",
        comment_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfLeagueBbsComments));
    record->comment_id = row[0] ? atoi(row[0]) : 0;
    record->bbs_id = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->charac_name, row[5], sizeof(record->charac_name) - 1);
    record->job = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->comments, row[7], sizeof(record->comments) - 1);
    record->reg_time = row[8] ? atoi(row[8]) : 0;
    record->delete_flag = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfLeagueBbsComments_GetAll(DBConnectionManager* manager, DnfLeagueBbsComments* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT comment_id, bbs_id, m_id, server_id, charac_no, charac_name, job, comments, reg_time, delete_flag FROM dnf_league_bbs_comments");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfLeagueBbsComments));
        records[count].comment_id = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_id = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].charac_name, row[5], sizeof(records[count].charac_name) - 1);
        records[count].job = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].comments, row[7], sizeof(records[count].comments) - 1);
        records[count].reg_time = row[8] ? atoi(row[8]) : 0;
        records[count].delete_flag = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
