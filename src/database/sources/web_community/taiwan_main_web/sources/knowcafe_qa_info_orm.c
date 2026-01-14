#include "knowcafe_qa_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeQaInfo_Add(DBConnectionManager* manager, const KnowcafeQaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_qa_info (no, notice, category, depth, sequence) "
        "VALUES (%d, %d, %d, %d, %f)",
        record->no, record->notice, record->category, record->depth, record->sequence);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQaInfo_Get(DBConnectionManager* manager, int no, KnowcafeQaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, depth, sequence FROM knowcafe_qa_info WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeQaInfo));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->notice = row[1] ? row[1][0] : '\0';
    record->category = row[2] ? atoi(row[2]) : 0;
    record->depth = row[3] ? atoi(row[3]) : 0;
    record->sequence = row[4] ? atof(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQaInfo_GetAll(DBConnectionManager* manager, KnowcafeQaInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, notice, category, depth, sequence FROM knowcafe_qa_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeQaInfo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].notice = row[1] ? row[1][0] : '\0';
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].depth = row[3] ? atoi(row[3]) : 0;
        records[count].sequence = row[4] ? atof(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
