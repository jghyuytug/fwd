#include "repl_comment_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ReplComment_Add(DBConnectionManager* manager, const ReplComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO repl_comment (rno, imtc_no, m_id, reg_id, pawd, reg_date, comment) "
        "VALUES (%d, %d, %d, '%s', '%s', %d, '%s')",
        record->rno, record->imtc_no, record->m_id, record->reg_id, record->pawd, record->reg_date, record->comment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ReplComment_Get(DBConnectionManager* manager, int rno, short comm_no, ReplComment* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, comm_no, imtc_no, m_id, reg_id, pawd, reg_date, comment FROM repl_comment WHERE rno = %d AND comm_no = %d",
        rno, comm_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ReplComment));
    record->rno = row[0] ? atoi(row[0]) : 0;
    record->comm_no = row[1] ? atoi(row[1]) : 0;
    record->imtc_no = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->reg_id, row[4], sizeof(record->reg_id) - 1);
    if (row[5]) strncpy(record->pawd, row[5], sizeof(record->pawd) - 1);
    record->reg_date = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->comment, row[7], sizeof(record->comment) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ReplComment_GetAll(DBConnectionManager* manager, ReplComment* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, comm_no, imtc_no, m_id, reg_id, pawd, reg_date, comment FROM repl_comment");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ReplComment));
        records[count].rno = row[0] ? atoi(row[0]) : 0;
        records[count].comm_no = row[1] ? atoi(row[1]) : 0;
        records[count].imtc_no = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].reg_id, row[4], sizeof(records[count].reg_id) - 1);
        if (row[5]) strncpy(records[count].pawd, row[5], sizeof(records[count].pawd) - 1);
        records[count].reg_date = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].comment, row[7], sizeof(records[count].comment) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
