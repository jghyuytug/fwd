#include "oneclick_charac_avatar_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OneclickCharacAvatarMod_Add(DBConnectionManager* manager, const OneclickCharacAvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO oneclick_charac_avatar_mod (server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, color1_old, color2_old, trade_restrict_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, color1_new, color2_new, trade_restrict_new, MNG_user_id, work_time, work_type, memo) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->server_info, record->charac_no, record->ui_id_old, record->slot_old, record->reg_date_old, record->expire_date_old, record->ability_no_old, record->color1_old, record->color2_old, record->trade_restrict_old, record->ui_id_new, record->slot_new, record->reg_date_new, record->expire_date_new, record->ability_no_new, record->color1_new, record->color2_new, record->trade_restrict_new, record->MNG_user_id, record->work_time, record->work_type, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OneclickCharacAvatarMod_Get(DBConnectionManager* manager, int id, OneclickCharacAvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, color1_old, color2_old, trade_restrict_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, color1_new, color2_new, trade_restrict_new, MNG_user_id, work_time, work_type, memo FROM oneclick_charac_avatar_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OneclickCharacAvatarMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->ui_id_old = row[3] ? atoi(row[3]) : 0;
    record->slot_old = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->reg_date_old, row[5], sizeof(record->reg_date_old) - 1);
    if (row[6]) strncpy(record->expire_date_old, row[6], sizeof(record->expire_date_old) - 1);
    record->ability_no_old = row[7] ? atoi(row[7]) : 0;
    record->color1_old = row[8] ? atoi(row[8]) : 0;
    record->color2_old = row[9] ? atoi(row[9]) : 0;
    record->trade_restrict_old = row[10] ? atoi(row[10]) : 0;
    record->ui_id_new = row[11] ? atoi(row[11]) : 0;
    record->slot_new = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->reg_date_new, row[13], sizeof(record->reg_date_new) - 1);
    if (row[14]) strncpy(record->expire_date_new, row[14], sizeof(record->expire_date_new) - 1);
    record->ability_no_new = row[15] ? atoi(row[15]) : 0;
    record->color1_new = row[16] ? atoi(row[16]) : 0;
    record->color2_new = row[17] ? atoi(row[17]) : 0;
    record->trade_restrict_new = row[18] ? atoi(row[18]) : 0;
    if (row[19]) strncpy(record->MNG_user_id, row[19], sizeof(record->MNG_user_id) - 1);
    if (row[20]) strncpy(record->work_time, row[20], sizeof(record->work_time) - 1);
    record->work_type = row[21] ? row[21][0] : '\0';
    if (row[22]) strncpy(record->memo, row[22], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int OneclickCharacAvatarMod_GetAll(DBConnectionManager* manager, OneclickCharacAvatarMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, color1_old, color2_old, trade_restrict_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, color1_new, color2_new, trade_restrict_new, MNG_user_id, work_time, work_type, memo FROM oneclick_charac_avatar_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OneclickCharacAvatarMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].ui_id_old = row[3] ? atoi(row[3]) : 0;
        records[count].slot_old = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reg_date_old, row[5], sizeof(records[count].reg_date_old) - 1);
        if (row[6]) strncpy(records[count].expire_date_old, row[6], sizeof(records[count].expire_date_old) - 1);
        records[count].ability_no_old = row[7] ? atoi(row[7]) : 0;
        records[count].color1_old = row[8] ? atoi(row[8]) : 0;
        records[count].color2_old = row[9] ? atoi(row[9]) : 0;
        records[count].trade_restrict_old = row[10] ? atoi(row[10]) : 0;
        records[count].ui_id_new = row[11] ? atoi(row[11]) : 0;
        records[count].slot_new = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].reg_date_new, row[13], sizeof(records[count].reg_date_new) - 1);
        if (row[14]) strncpy(records[count].expire_date_new, row[14], sizeof(records[count].expire_date_new) - 1);
        records[count].ability_no_new = row[15] ? atoi(row[15]) : 0;
        records[count].color1_new = row[16] ? atoi(row[16]) : 0;
        records[count].color2_new = row[17] ? atoi(row[17]) : 0;
        records[count].trade_restrict_new = row[18] ? atoi(row[18]) : 0;
        if (row[19]) strncpy(records[count].MNG_user_id, row[19], sizeof(records[count].MNG_user_id) - 1);
        if (row[20]) strncpy(records[count].work_time, row[20], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[21] ? row[21][0] : '\0';
        if (row[22]) strncpy(records[count].memo, row[22], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
