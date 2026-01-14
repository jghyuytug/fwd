#include "sphinx_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SphinxInfo_Add(DBConnectionManager* manager, const SphinxInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO sphinx_info (search_id, last_insert_id, last_update_time) "
        "VALUES ('%s', %d, %d)",
        record->search_id, record->last_insert_id, record->last_update_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SphinxInfo_Get(DBConnectionManager* manager, const char* search_id, SphinxInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT search_id, last_insert_id, last_update_time FROM sphinx_info WHERE search_id = '%s'",
        search_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SphinxInfo));
    if (row[0]) strncpy(record->search_id, row[0], sizeof(record->search_id) - 1);
    record->last_insert_id = row[1] ? atoll(row[1]) : 0;
    record->last_update_time = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SphinxInfo_GetAll(DBConnectionManager* manager, SphinxInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT search_id, last_insert_id, last_update_time FROM sphinx_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SphinxInfo));
        if (row[0]) strncpy(records[count].search_id, row[0], sizeof(records[count].search_id) - 1);
        records[count].last_insert_id = row[1] ? atoll(row[1]) : 0;
        records[count].last_update_time = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
