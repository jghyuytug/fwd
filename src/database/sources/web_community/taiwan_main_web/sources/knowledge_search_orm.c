#include "knowledge_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowledgeSearch_Add(DBConnectionManager* manager, const KnowledgeSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowledge_search (no, kn_id, word) "
        "VALUES (%d, %d, '%s')",
        record->no, record->kn_id, record->word);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowledgeSearch_GetAll(DBConnectionManager* manager, KnowledgeSearch* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, kn_id, word FROM knowledge_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowledgeSearch));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].kn_id = row[1] ? atoll(row[1]) : 0;
        if (row[2]) strncpy(records[count].word, row[2], sizeof(records[count].word) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
