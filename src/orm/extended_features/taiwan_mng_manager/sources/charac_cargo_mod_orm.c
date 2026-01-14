#include "charac_cargo_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacCargoMod_Add(DBConnectionManager* manager, const CharacCargoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_cargo_mod (server_info, charac_no, cargo_capa_old, cargo_capa_new, MNG_user_id, work_time, work_type, memo) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->server_info, record->charac_no, record->cargo_capa_old, record->cargo_capa_new, record->MNG_user_id, record->work_time, record->work_type, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacCargoMod_Get(DBConnectionManager* manager, int id, CharacCargoMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, cargo_capa_old, cargo_capa_new, MNG_user_id, work_time, work_type, memo FROM charac_cargo_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacCargoMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->cargo_capa_old = row[3] ? atoi(row[3]) : 0;
    record->cargo_capa_new = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->MNG_user_id, row[5], sizeof(record->MNG_user_id) - 1);
    if (row[6]) strncpy(record->work_time, row[6], sizeof(record->work_time) - 1);
    record->work_type = row[7] ? row[7][0] : '\0';
    if (row[8]) strncpy(record->memo, row[8], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacCargoMod_GetAll(DBConnectionManager* manager, CharacCargoMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, cargo_capa_old, cargo_capa_new, MNG_user_id, work_time, work_type, memo FROM charac_cargo_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacCargoMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].cargo_capa_old = row[3] ? atoi(row[3]) : 0;
        records[count].cargo_capa_new = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].MNG_user_id, row[5], sizeof(records[count].MNG_user_id) - 1);
        if (row[6]) strncpy(records[count].work_time, row[6], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[7] ? row[7][0] : '\0';
        if (row[8]) strncpy(records[count].memo, row[8], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
