#include "repl_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ReplBbs_Add(DBConnectionManager* manager, const ReplBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO repl_bbs (bd_id, thread_id, rev_thread, depth, reg_date, mod_date, hits, recomm, de_recom, comment, body_type, m_id, reg_id, subject) "
        "VALUES (%d, %f, %f, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', '%s')",
        record->bd_id, record->thread_id, record->rev_thread, record->depth, record->reg_date, record->mod_date, record->hits, record->recomm, record->de_recom, record->comment, record->body_type, record->m_id, record->reg_id, record->subject);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ReplBbs_Get(DBConnectionManager* manager, int rno, ReplBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, bd_id, thread_id, rev_thread, depth, reg_date, mod_date, hits, recomm, de_recom, comment, body_type, m_id, reg_id, subject FROM repl_bbs WHERE rno = %d",
        rno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ReplBbs));
    record->rno = row[0] ? atoi(row[0]) : 0;
    record->bd_id = row[1] ? atoi(row[1]) : 0;
    record->thread_id = row[2] ? atof(row[2]) : 0;
    record->rev_thread = row[3] ? atof(row[3]) : 0;
    record->depth = row[4] ? atoi(row[4]) : 0;
    record->reg_date = row[5] ? atoi(row[5]) : 0;
    record->mod_date = row[6] ? atoi(row[6]) : 0;
    record->hits = row[7] ? atoi(row[7]) : 0;
    record->recomm = row[8] ? atoi(row[8]) : 0;
    record->de_recom = row[9] ? atoi(row[9]) : 0;
    record->comment = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->body_type, row[11], sizeof(record->body_type) - 1);
    record->m_id = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->reg_id, row[13], sizeof(record->reg_id) - 1);
    if (row[14]) strncpy(record->subject, row[14], sizeof(record->subject) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ReplBbs_GetAll(DBConnectionManager* manager, ReplBbs* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, bd_id, thread_id, rev_thread, depth, reg_date, mod_date, hits, recomm, de_recom, comment, body_type, m_id, reg_id, subject FROM repl_bbs");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ReplBbs));
        records[count].rno = row[0] ? atoi(row[0]) : 0;
        records[count].bd_id = row[1] ? atoi(row[1]) : 0;
        records[count].thread_id = row[2] ? atof(row[2]) : 0;
        records[count].rev_thread = row[3] ? atof(row[3]) : 0;
        records[count].depth = row[4] ? atoi(row[4]) : 0;
        records[count].reg_date = row[5] ? atoi(row[5]) : 0;
        records[count].mod_date = row[6] ? atoi(row[6]) : 0;
        records[count].hits = row[7] ? atoi(row[7]) : 0;
        records[count].recomm = row[8] ? atoi(row[8]) : 0;
        records[count].de_recom = row[9] ? atoi(row[9]) : 0;
        records[count].comment = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].body_type, row[11], sizeof(records[count].body_type) - 1);
        records[count].m_id = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].reg_id, row[13], sizeof(records[count].reg_id) - 1);
        if (row[14]) strncpy(records[count].subject, row[14], sizeof(records[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
