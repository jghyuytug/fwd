#include "options_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Options_Add(DBConnectionManager* manager, const Options* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO options (RechargeOption_Ratio, PostalTitle, PostalText, CharacUpgradeLevelExpTable, AdminId) "
        "VALUES (%d, '%s', '%s', '%s', '%s')",
        record->RechargeOption_Ratio, record->PostalTitle, record->PostalText, record->CharacUpgradeLevelExpTable, record->AdminId);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Options_Get(DBConnectionManager* manager, int Id, Options* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, RechargeOption_Ratio, PostalTitle, PostalText, CharacUpgradeLevelExpTable, AdminId FROM options WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Options));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->RechargeOption_Ratio = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->PostalTitle, row[2], sizeof(record->PostalTitle) - 1);
    if (row[3]) strncpy(record->PostalText, row[3], sizeof(record->PostalText) - 1);
    if (row[4]) strncpy(record->CharacUpgradeLevelExpTable, row[4], sizeof(record->CharacUpgradeLevelExpTable) - 1);
    if (row[5]) strncpy(record->AdminId, row[5], sizeof(record->AdminId) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Options_GetAll(DBConnectionManager* manager, Options* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, RechargeOption_Ratio, PostalTitle, PostalText, CharacUpgradeLevelExpTable, AdminId FROM options");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Options));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].RechargeOption_Ratio = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].PostalTitle, row[2], sizeof(records[count].PostalTitle) - 1);
        if (row[3]) strncpy(records[count].PostalText, row[3], sizeof(records[count].PostalText) - 1);
        if (row[4]) strncpy(records[count].CharacUpgradeLevelExpTable, row[4], sizeof(records[count].CharacUpgradeLevelExpTable) - 1);
        if (row[5]) strncpy(records[count].AdminId, row[5], sizeof(records[count].AdminId) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
