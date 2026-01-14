#include "event_dnf_3year_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventDnf3YearComment_Add(DBConnectionManager* manager, const EventDnf3YearComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_dnf_3year_comment (m_id, server_id, charac_no, job, comment, feel_cnt, open_flag) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d)",
        record->m_id, record->server_id, record->charac_no, record->job, record->comment, record->feel_cnt, record->open_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventDnf3YearComment_Get(DBConnectionManager* manager, int no, EventDnf3YearComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, server_id, charac_no, job, comment, feel_cnt, open_flag FROM event_dnf_3year_comment WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventDnf3YearComment));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->job = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->comment, row[5], sizeof(record->comment) - 1);
    record->feel_cnt = row[6] ? atoi(row[6]) : 0;
    record->open_flag = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventDnf3YearComment_GetAll(DBConnectionManager* manager, EventDnf3YearComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, server_id, charac_no, job, comment, feel_cnt, open_flag FROM event_dnf_3year_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventDnf3YearComment));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].job = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].comment, row[5], sizeof(records[count].comment) - 1);
        records[count].feel_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].open_flag = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
