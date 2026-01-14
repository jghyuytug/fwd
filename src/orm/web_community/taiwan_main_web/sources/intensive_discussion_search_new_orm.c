#include "intensive_discussion_search_new_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int IntensiveDiscussionSearchNew_Add(DBConnectionManager* manager, const IntensiveDiscussionSearchNew* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO intensive_discussion_search_new (no, category, class, word) "
        "VALUES (%d, %d, %d, '%s')",
        record->no, record->category, record->class, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int IntensiveDiscussionSearchNew_GetAll(DBConnectionManager* manager, IntensiveDiscussionSearchNew* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, class, word FROM intensive_discussion_search_new");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(IntensiveDiscussionSearchNew));
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
