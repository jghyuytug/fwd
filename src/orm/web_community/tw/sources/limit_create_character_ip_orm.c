#include "limit_create_character_ip_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LimitCreateCharacterIp_Add(DBConnectionManager* manager, const LimitCreateCharacterIp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO limit_create_character_ip (ip, ip_str, last_access_time, count, last_access_mid) "
        "VALUES (%d, '%s', '%s', %d, %d)",
        record->ip, record->ip_str, record->last_access_time, record->count, record->last_access_mid);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIp_Get(DBConnectionManager* manager, int ip, LimitCreateCharacterIp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid FROM limit_create_character_ip WHERE ip = %d",
        ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LimitCreateCharacterIp));
    record->ip = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->ip_str, row[1], sizeof(record->ip_str) - 1);
    if (row[2]) strncpy(record->last_access_time, row[2], sizeof(record->last_access_time) - 1);
    record->count = row[3] ? atoi(row[3]) : 0;
    record->last_access_mid = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIp_GetAll(DBConnectionManager* manager, LimitCreateCharacterIp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid FROM limit_create_character_ip");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LimitCreateCharacterIp));
        records[count].ip = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].ip_str, row[1], sizeof(records[count].ip_str) - 1);
        if (row[2]) strncpy(records[count].last_access_time, row[2], sizeof(records[count].last_access_time) - 1);
        records[count].count = row[3] ? atoi(row[3]) : 0;
        records[count].last_access_mid = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
