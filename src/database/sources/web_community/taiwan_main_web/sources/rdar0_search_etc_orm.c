#include "rdar0_search_etc_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Rdar0SearchEtc_Add(DBConnectionManager* manager, const Rdar0SearchEtc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO rdar0_search_etc (no, category, class, word) "
        "VALUES (%d, %d, %d, '%s')",
        record->no, record->category, record->class, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Rdar0SearchEtc_GetAll(DBConnectionManager* manager, Rdar0SearchEtc* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, class, word FROM rdar0_search_etc");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Rdar0SearchEtc));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        records[count].class = row[2] ? row[2][0] : '\0';
        if (row[3]) strncpy(records[count].word, row[3], sizeof(records[count].word) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
