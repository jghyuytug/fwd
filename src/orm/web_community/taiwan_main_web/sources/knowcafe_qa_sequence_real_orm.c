#include "knowcafe_qa_sequence_real_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeQaSequenceReal_Add(DBConnectionManager* manager, const KnowcafeQaSequenceReal* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_qa_sequence_real (sequence, cnt) "
        "VALUES (%d, %d)",
        record->sequence, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeQaSequenceReal_GetAll(DBConnectionManager* manager, KnowcafeQaSequenceReal* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sequence, cnt FROM knowcafe_qa_sequence_real");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeQaSequenceReal));
        records[count].sequence = row[0] ? atoi(row[0]) : 0;
        records[count].cnt = row[1] ? atoll(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
