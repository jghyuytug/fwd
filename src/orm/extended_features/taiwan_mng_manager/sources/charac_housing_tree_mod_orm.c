#include "charac_housing_tree_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacHousingTreeMod_Add(DBConnectionManager* manager, const CharacHousingTreeMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_housing_tree_mod (server_info, charac_no, tree_id_old, tree_id_new, expire_date_old, expire_date_new, current_point_old, current_point_new, leaf_point_old, leaf_point_new, day_water_count_old, day_water_count_new, MNG_user_id, work_time, work_type, memo) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->server_info, record->charac_no, record->tree_id_old, record->tree_id_new, record->expire_date_old, record->expire_date_new, record->current_point_old, record->current_point_new, record->leaf_point_old, record->leaf_point_new, record->day_water_count_old, record->day_water_count_new, record->MNG_user_id, record->work_time, record->work_type, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeMod_Get(DBConnectionManager* manager, int id, CharacHousingTreeMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, tree_id_old, tree_id_new, expire_date_old, expire_date_new, current_point_old, current_point_new, leaf_point_old, leaf_point_new, day_water_count_old, day_water_count_new, MNG_user_id, work_time, work_type, memo FROM charac_housing_tree_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacHousingTreeMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->tree_id_old = row[3] ? atoi(row[3]) : 0;
    record->tree_id_new = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->expire_date_old, row[5], sizeof(record->expire_date_old) - 1);
    if (row[6]) strncpy(record->expire_date_new, row[6], sizeof(record->expire_date_new) - 1);
    record->current_point_old = row[7] ? atoi(row[7]) : 0;
    record->current_point_new = row[8] ? atoi(row[8]) : 0;
    record->leaf_point_old = row[9] ? atoi(row[9]) : 0;
    record->leaf_point_new = row[10] ? atoi(row[10]) : 0;
    record->day_water_count_old = row[11] ? atoi(row[11]) : 0;
    record->day_water_count_new = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->MNG_user_id, row[13], sizeof(record->MNG_user_id) - 1);
    if (row[14]) strncpy(record->work_time, row[14], sizeof(record->work_time) - 1);
    record->work_type = row[15] ? row[15][0] : '\0';
    if (row[16]) strncpy(record->memo, row[16], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingTreeMod_GetAll(DBConnectionManager* manager, CharacHousingTreeMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, tree_id_old, tree_id_new, expire_date_old, expire_date_new, current_point_old, current_point_new, leaf_point_old, leaf_point_new, day_water_count_old, day_water_count_new, MNG_user_id, work_time, work_type, memo FROM charac_housing_tree_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacHousingTreeMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].tree_id_old = row[3] ? atoi(row[3]) : 0;
        records[count].tree_id_new = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].expire_date_old, row[5], sizeof(records[count].expire_date_old) - 1);
        if (row[6]) strncpy(records[count].expire_date_new, row[6], sizeof(records[count].expire_date_new) - 1);
        records[count].current_point_old = row[7] ? atoi(row[7]) : 0;
        records[count].current_point_new = row[8] ? atoi(row[8]) : 0;
        records[count].leaf_point_old = row[9] ? atoi(row[9]) : 0;
        records[count].leaf_point_new = row[10] ? atoi(row[10]) : 0;
        records[count].day_water_count_old = row[11] ? atoi(row[11]) : 0;
        records[count].day_water_count_new = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].MNG_user_id, row[13], sizeof(records[count].MNG_user_id) - 1);
        if (row[14]) strncpy(records[count].work_time, row[14], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[15] ? row[15][0] : '\0';
        if (row[16]) strncpy(records[count].memo, row[16], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
