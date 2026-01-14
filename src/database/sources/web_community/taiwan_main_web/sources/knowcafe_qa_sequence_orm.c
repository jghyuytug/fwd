#include "knowcafe_qa_sequence_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeQaSequence_Add(DBConnectionManager* manager, const KnowcafeQaSequence* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_qa_sequence (no, sequence) "
        "VALUES (%d, %d)",
        record->no, record->sequence);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQaSequence_GetAll(DBConnectionManager* manager, KnowcafeQaSequence* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, sequence FROM knowcafe_qa_sequence");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeQaSequence));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].sequence = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
