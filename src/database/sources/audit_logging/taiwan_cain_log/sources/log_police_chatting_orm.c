#include "log_police_chatting_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogPoliceChatting_Add(DBConnectionManager* manager, const LogPoliceChatting* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_police_chatting (charac_no, msg_type, occ_time, message) "
        "VALUES (%d, %d, %d, '%s')",
        record->charac_no, record->msg_type, record->occ_time, record->message);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogPoliceChatting_Get(DBConnectionManager* manager, int no, LogPoliceChatting* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, charac_no, msg_type, occ_time, message FROM log_police_chatting WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogPoliceChatting));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->msg_type = row[2] ? atoi(row[2]) : 0;
    record->occ_time = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->message, row[4], sizeof(record->message) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogPoliceChatting_GetAll(DBConnectionManager* manager, LogPoliceChatting* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, charac_no, msg_type, occ_time, message FROM log_police_chatting");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPoliceChatting));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].msg_type = row[2] ? atoi(row[2]) : 0;
        records[count].occ_time = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].message, row[4], sizeof(records[count].message) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
