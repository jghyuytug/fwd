#include "community_discussion_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommunityDiscussionSearch_Add(DBConnectionManager* manager, const CommunityDiscussionSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO community_discussion_search (no, category, class, word) "
        "VALUES (%d, %d, '%s', '%s')",
        record->no, record->category, record->class, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionSearch_Get(DBConnectionManager* manager, int no, signed char category, const char* class, const char* word, CommunityDiscussionSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, class, word FROM community_discussion_search WHERE no = %d AND category = %d AND class = '%s' AND word = '%s'",
        no, category, class, word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommunityDiscussionSearch));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->category = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->class, row[2], sizeof(record->class) - 1);
    if (row[3]) strncpy(record->word, row[3], sizeof(record->word) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CommunityDiscussionSearch_GetAll(DBConnectionManager* manager, CommunityDiscussionSearch* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, class, word FROM community_discussion_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommunityDiscussionSearch));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].class, row[2], sizeof(records[count].class) - 1);
        if (row[3]) strncpy(records[count].word, row[3], sizeof(records[count].word) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
