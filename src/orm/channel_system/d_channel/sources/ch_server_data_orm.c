#include "ch_server_data_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ChServerData_Add(DBConnectionManager* manager, const ChServerData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ch_server_data (server_id, is_use, channel_number, channel_name, channel_kind, kind_name, exp_rate, exp_1, exp_2, exp_3, exp_4, exp_5, exp_6, exp_7, exp_8, exp_9, exp_10) "
        "VALUES (%d, %d, %d, '%s', %d, '%s', %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
        record->server_id, record->is_use, record->channel_number, record->channel_name, record->channel_kind, record->kind_name, record->exp_rate, record->exp_1, record->exp_2, record->exp_3, record->exp_4, record->exp_5, record->exp_6, record->exp_7, record->exp_8, record->exp_9, record->exp_10);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChServerData_Get(DBConnectionManager* manager, int id, ChServerData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_id, is_use, channel_number, channel_name, channel_kind, kind_name, exp_rate, exp_1, exp_2, exp_3, exp_4, exp_5, exp_6, exp_7, exp_8, exp_9, exp_10 FROM ch_server_data WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ChServerData));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->is_use = row[2] ? atoi(row[2]) : 0;
    record->channel_number = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->channel_name, row[4], sizeof(record->channel_name) - 1);
    record->channel_kind = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->kind_name, row[6], sizeof(record->kind_name) - 1);
    record->exp_rate = row[7] ? atof(row[7]) : 0;
    record->exp_1 = row[8] ? atof(row[8]) : 0;
    record->exp_2 = row[9] ? atof(row[9]) : 0;
    record->exp_3 = row[10] ? atof(row[10]) : 0;
    record->exp_4 = row[11] ? atof(row[11]) : 0;
    record->exp_5 = row[12] ? atof(row[12]) : 0;
    record->exp_6 = row[13] ? atof(row[13]) : 0;
    record->exp_7 = row[14] ? atof(row[14]) : 0;
    record->exp_8 = row[15] ? atof(row[15]) : 0;
    record->exp_9 = row[16] ? atof(row[16]) : 0;
    record->exp_10 = row[17] ? atof(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ChServerData_GetAll(DBConnectionManager* manager, ChServerData* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_id, is_use, channel_number, channel_name, channel_kind, kind_name, exp_rate, exp_1, exp_2, exp_3, exp_4, exp_5, exp_6, exp_7, exp_8, exp_9, exp_10 FROM ch_server_data");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ChServerData));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].is_use = row[2] ? atoi(row[2]) : 0;
        records[count].channel_number = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].channel_name, row[4], sizeof(records[count].channel_name) - 1);
        records[count].channel_kind = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].kind_name, row[6], sizeof(records[count].kind_name) - 1);
        records[count].exp_rate = row[7] ? atof(row[7]) : 0;
        records[count].exp_1 = row[8] ? atof(row[8]) : 0;
        records[count].exp_2 = row[9] ? atof(row[9]) : 0;
        records[count].exp_3 = row[10] ? atof(row[10]) : 0;
        records[count].exp_4 = row[11] ? atof(row[11]) : 0;
        records[count].exp_5 = row[12] ? atof(row[12]) : 0;
        records[count].exp_6 = row[13] ? atof(row[13]) : 0;
        records[count].exp_7 = row[14] ? atof(row[14]) : 0;
        records[count].exp_8 = row[15] ? atof(row[15]) : 0;
        records[count].exp_9 = row[16] ? atof(row[16]) : 0;
        records[count].exp_10 = row[17] ? atof(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
