#include "realradio_search_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RealradioSearchContent_Add(DBConnectionManager* manager, const RealradioSearchContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO realradio_search_content (no, bbs_type, category, class, word) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->no, record->bbs_type, record->category, record->class, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioSearchContent_GetAll(DBConnectionManager* manager, RealradioSearchContent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_type, category, class, word FROM realradio_search_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RealradioSearchContent));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_type = row[1] ? atoi(row[1]) : 0;
        records[count].category = row[2] ? atoi(row[2]) : 0;
        records[count].class = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(records[count].word, row[4], sizeof(records[count].word) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
