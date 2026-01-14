#include "sign_in_punish_config_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SignInPunishConfig_Add(DBConnectionManager* manager, const SignInPunishConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO sign_in_punish_config (Type, MinValue, MaxValue) "
        "VALUES (%d, %d, %d)",
        record->Type, record->MinValue, record->MaxValue);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SignInPunishConfig_Get(DBConnectionManager* manager, int Id, SignInPunishConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Type, MinValue, MaxValue FROM sign_in_punish_config WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SignInPunishConfig));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Type = row[1] ? atoi(row[1]) : 0;
    record->MinValue = row[2] ? atoi(row[2]) : 0;
    record->MaxValue = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SignInPunishConfig_GetAll(DBConnectionManager* manager, SignInPunishConfig* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Type, MinValue, MaxValue FROM sign_in_punish_config");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SignInPunishConfig));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Type = row[1] ? atoi(row[1]) : 0;
        records[count].MinValue = row[2] ? atoi(row[2]) : 0;
        records[count].MaxValue = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
