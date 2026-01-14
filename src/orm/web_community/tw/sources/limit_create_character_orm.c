#include "limit_create_character_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LimitCreateCharacter_Add(DBConnectionManager* manager, const LimitCreateCharacter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO limit_create_character (m_id, count, last_access_time) "
        "VALUES (%d, %d, '%s')",
        record->m_id, record->count, record->last_access_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_Get(DBConnectionManager* manager, int m_id, LimitCreateCharacter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LimitCreateCharacter));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->count = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->last_access_time, row[2], sizeof(record->last_access_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacter_GetAll(DBConnectionManager* manager, LimitCreateCharacter* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, count, last_access_time FROM limit_create_character");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LimitCreateCharacter));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].count = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].last_access_time, row[2], sizeof(records[count].last_access_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
