#include "avatar_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AvatarMod_Add(DBConnectionManager* manager, const AvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO avatar_mod (server_info, charac_no, avatar_id, MNG_user_id, work_time, work_type, reg_date, expire_date, ability_no, memo) "
        "VALUES (%d, %d, %d, '%s', '%s', %d, '%s', '%s', %d, '%s')",
        record->server_info, record->charac_no, record->avatar_id, record->MNG_user_id, record->work_time, record->work_type, record->reg_date, record->expire_date, record->ability_no, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarMod_Get(DBConnectionManager* manager, int id, AvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, avatar_id, MNG_user_id, work_time, work_type, reg_date, expire_date, ability_no, memo FROM avatar_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AvatarMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->avatar_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->MNG_user_id, row[4], sizeof(record->MNG_user_id) - 1);
    if (row[5]) strncpy(record->work_time, row[5], sizeof(record->work_time) - 1);
    record->work_type = row[6] ? row[6][0] : '\0';
    if (row[7]) strncpy(record->reg_date, row[7], sizeof(record->reg_date) - 1);
    if (row[8]) strncpy(record->expire_date, row[8], sizeof(record->expire_date) - 1);
    record->ability_no = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->memo, row[10], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AvatarMod_GetAll(DBConnectionManager* manager, AvatarMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, avatar_id, MNG_user_id, work_time, work_type, reg_date, expire_date, ability_no, memo FROM avatar_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AvatarMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].avatar_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].MNG_user_id, row[4], sizeof(records[count].MNG_user_id) - 1);
        if (row[5]) strncpy(records[count].work_time, row[5], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[6] ? row[6][0] : '\0';
        if (row[7]) strncpy(records[count].reg_date, row[7], sizeof(records[count].reg_date) - 1);
        if (row[8]) strncpy(records[count].expire_date, row[8], sizeof(records[count].expire_date) - 1);
        records[count].ability_no = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].memo, row[10], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
