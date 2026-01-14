#include "charac_postal_add_large_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacPostalAddLarge_Add(DBConnectionManager* manager, const CharacPostalAddLarge* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_postal_add_large (MNG_user_id, work_time, memo, end_flag) "
        "VALUES ('%s', '%s', '%s', %d)",
        record->MNG_user_id, record->work_time, record->memo, record->end_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacPostalAddLarge_Get(DBConnectionManager* manager, int group_id, CharacPostalAddLarge* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT group_id, MNG_user_id, work_time, memo, end_flag FROM charac_postal_add_large WHERE group_id = %d",
        group_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacPostalAddLarge));
    record->group_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->MNG_user_id, row[1], sizeof(record->MNG_user_id) - 1);
    if (row[2]) strncpy(record->work_time, row[2], sizeof(record->work_time) - 1);
    if (row[3]) strncpy(record->memo, row[3], sizeof(record->memo) - 1);
    record->end_flag = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacPostalAddLarge_GetAll(DBConnectionManager* manager, CharacPostalAddLarge* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT group_id, MNG_user_id, work_time, memo, end_flag FROM charac_postal_add_large");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacPostalAddLarge));
        records[count].group_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].MNG_user_id, row[1], sizeof(records[count].MNG_user_id) - 1);
        if (row[2]) strncpy(records[count].work_time, row[2], sizeof(records[count].work_time) - 1);
        if (row[3]) strncpy(records[count].memo, row[3], sizeof(records[count].memo) - 1);
        records[count].end_flag = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
