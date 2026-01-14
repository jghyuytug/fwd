#include "random_option_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RandomOptionRef_Add(DBConnectionManager* manager, const RandomOptionRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO random_option_ref (random_option_index, random_option_value, random_option_name) "
        "VALUES (%d, %d, '%s')",
        record->random_option_index, record->random_option_value, record->random_option_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RandomOptionRef_Get(DBConnectionManager* manager, signed char random_option_index, signed char random_option_value, RandomOptionRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT random_option_index, random_option_value, random_option_name FROM random_option_ref WHERE random_option_index = %d AND random_option_value = %d",
        random_option_index, random_option_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(RandomOptionRef));
    record->random_option_index = row[0] ? atoi(row[0]) : 0;
    record->random_option_value = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->random_option_name, row[2], sizeof(record->random_option_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int RandomOptionRef_GetAll(DBConnectionManager* manager, RandomOptionRef* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT random_option_index, random_option_value, random_option_name FROM random_option_ref");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RandomOptionRef));
        records[count].random_option_index = row[0] ? atoi(row[0]) : 0;
        records[count].random_option_value = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].random_option_name, row[2], sizeof(records[count].random_option_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
