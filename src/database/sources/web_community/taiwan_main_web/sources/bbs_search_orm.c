#include "bbs_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BbsSearch_Add(DBConnectionManager* manager, const BbsSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO bbs_search (bbs_class, f_key, s_key, subject, body, reg_date) "
        "VALUES (%d, %d, %d, '%s', '%s', %d)",
        record->bbs_class, record->f_key, record->s_key, record->subject, record->body, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BbsSearch_GetAll(DBConnectionManager* manager, BbsSearch* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_class, f_key, s_key, subject, body, reg_date FROM bbs_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BbsSearch));
        records[count].bbs_class = row[0] ? atoi(row[0]) : 0;
        records[count].f_key = row[1] ? atoi(row[1]) : 0;
        records[count].s_key = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].subject, row[3], sizeof(records[count].subject) - 1);
        if (row[4]) strncpy(records[count].body, row[4], sizeof(records[count].body) - 1);
        records[count].reg_date = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
