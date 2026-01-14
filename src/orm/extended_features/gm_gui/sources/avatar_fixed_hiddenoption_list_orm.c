#include "avatar_fixed_hiddenoption_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AvatarFixedHiddenoptionList_Add(DBConnectionManager* manager, const AvatarFixedHiddenoptionList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO avatar_fixed_hiddenoption_list (Id, Type, Name, MinValue, MaxValue) "
        "VALUES (%d, %d, '%s', %f, %f)",
        record->Id, record->Type, record->Name, record->MinValue, record->MaxValue);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarFixedHiddenoptionList_Get(DBConnectionManager* manager, short Id, signed char Type, AvatarFixedHiddenoptionList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Type, Name, MinValue, MaxValue FROM avatar_fixed_hiddenoption_list WHERE Id = %d AND Type = %d",
        Id, Type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AvatarFixedHiddenoptionList));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Type = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->Name, row[2], sizeof(record->Name) - 1);
    record->MinValue = row[3] ? atof(row[3]) : 0;
    record->MaxValue = row[4] ? atof(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarFixedHiddenoptionList_GetAll(DBConnectionManager* manager, AvatarFixedHiddenoptionList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Type, Name, MinValue, MaxValue FROM avatar_fixed_hiddenoption_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AvatarFixedHiddenoptionList));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Type = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].Name, row[2], sizeof(records[count].Name) - 1);
        records[count].MinValue = row[3] ? atof(row[3]) : 0;
        records[count].MaxValue = row[4] ? atof(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
