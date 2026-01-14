#include "charac_avatar_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacAvatarMod_Add(DBConnectionManager* manager, const CharacAvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_avatar_mod (server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, jewel_socket_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, jewel_socket_new, MNG_user_id, work_time, work_type, memo, hidden_option_old, emblem_endurance_old, hidden_option_new, emblem_endurance_new, color1_old, color2_old, color1_new, color2_new, trade_restrict_old, trade_restrict_new) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, '%s', %d, %d, '%s', '%s', %d, '%s', '%s', '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->server_info, record->charac_no, record->ui_id_old, record->slot_old, record->reg_date_old, record->expire_date_old, record->ability_no_old, record->jewel_socket_old, record->ui_id_new, record->slot_new, record->reg_date_new, record->expire_date_new, record->ability_no_new, record->jewel_socket_new, record->MNG_user_id, record->work_time, record->work_type, record->memo, record->hidden_option_old, record->emblem_endurance_old, record->hidden_option_new, record->emblem_endurance_new, record->color1_old, record->color2_old, record->color1_new, record->color2_new, record->trade_restrict_old, record->trade_restrict_new);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAvatarMod_Get(DBConnectionManager* manager, int id, CharacAvatarMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, jewel_socket_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, jewel_socket_new, MNG_user_id, work_time, work_type, memo, hidden_option_old, emblem_endurance_old, hidden_option_new, emblem_endurance_new, color1_old, color2_old, color1_new, color2_new, trade_restrict_old, trade_restrict_new FROM charac_avatar_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacAvatarMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->ui_id_old = row[3] ? atoi(row[3]) : 0;
    record->slot_old = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->reg_date_old, row[5], sizeof(record->reg_date_old) - 1);
    if (row[6]) strncpy(record->expire_date_old, row[6], sizeof(record->expire_date_old) - 1);
    record->ability_no_old = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->jewel_socket_old, row[8], sizeof(record->jewel_socket_old) - 1);
    record->ui_id_new = row[9] ? atoi(row[9]) : 0;
    record->slot_new = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->reg_date_new, row[11], sizeof(record->reg_date_new) - 1);
    if (row[12]) strncpy(record->expire_date_new, row[12], sizeof(record->expire_date_new) - 1);
    record->ability_no_new = row[13] ? atoi(row[13]) : 0;
    if (row[14]) strncpy(record->jewel_socket_new, row[14], sizeof(record->jewel_socket_new) - 1);
    if (row[15]) strncpy(record->MNG_user_id, row[15], sizeof(record->MNG_user_id) - 1);
    if (row[16]) strncpy(record->work_time, row[16], sizeof(record->work_time) - 1);
    record->work_type = row[17] ? row[17][0] : '\0';
    if (row[18]) strncpy(record->memo, row[18], sizeof(record->memo) - 1);
    record->hidden_option_old = row[19] ? atoi(row[19]) : 0;
    record->emblem_endurance_old = row[20] ? atoi(row[20]) : 0;
    record->hidden_option_new = row[21] ? atoi(row[21]) : 0;
    record->emblem_endurance_new = row[22] ? atoi(row[22]) : 0;
    record->color1_old = row[23] ? atoi(row[23]) : 0;
    record->color2_old = row[24] ? atoi(row[24]) : 0;
    record->color1_new = row[25] ? atoi(row[25]) : 0;
    record->color2_new = row[26] ? atoi(row[26]) : 0;
    record->trade_restrict_old = row[27] ? atoi(row[27]) : 0;
    record->trade_restrict_new = row[28] ? atoi(row[28]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAvatarMod_GetAll(DBConnectionManager* manager, CharacAvatarMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, ui_id_old, slot_old, reg_date_old, expire_date_old, ability_no_old, jewel_socket_old, ui_id_new, slot_new, reg_date_new, expire_date_new, ability_no_new, jewel_socket_new, MNG_user_id, work_time, work_type, memo, hidden_option_old, emblem_endurance_old, hidden_option_new, emblem_endurance_new, color1_old, color2_old, color1_new, color2_new, trade_restrict_old, trade_restrict_new FROM charac_avatar_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacAvatarMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].ui_id_old = row[3] ? atoi(row[3]) : 0;
        records[count].slot_old = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reg_date_old, row[5], sizeof(records[count].reg_date_old) - 1);
        if (row[6]) strncpy(records[count].expire_date_old, row[6], sizeof(records[count].expire_date_old) - 1);
        records[count].ability_no_old = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].jewel_socket_old, row[8], sizeof(records[count].jewel_socket_old) - 1);
        records[count].ui_id_new = row[9] ? atoi(row[9]) : 0;
        records[count].slot_new = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].reg_date_new, row[11], sizeof(records[count].reg_date_new) - 1);
        if (row[12]) strncpy(records[count].expire_date_new, row[12], sizeof(records[count].expire_date_new) - 1);
        records[count].ability_no_new = row[13] ? atoi(row[13]) : 0;
        if (row[14]) strncpy(records[count].jewel_socket_new, row[14], sizeof(records[count].jewel_socket_new) - 1);
        if (row[15]) strncpy(records[count].MNG_user_id, row[15], sizeof(records[count].MNG_user_id) - 1);
        if (row[16]) strncpy(records[count].work_time, row[16], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[17] ? row[17][0] : '\0';
        if (row[18]) strncpy(records[count].memo, row[18], sizeof(records[count].memo) - 1);
        records[count].hidden_option_old = row[19] ? atoi(row[19]) : 0;
        records[count].emblem_endurance_old = row[20] ? atoi(row[20]) : 0;
        records[count].hidden_option_new = row[21] ? atoi(row[21]) : 0;
        records[count].emblem_endurance_new = row[22] ? atoi(row[22]) : 0;
        records[count].color1_old = row[23] ? atoi(row[23]) : 0;
        records[count].color2_old = row[24] ? atoi(row[24]) : 0;
        records[count].color1_new = row[25] ? atoi(row[25]) : 0;
        records[count].color2_new = row[26] ? atoi(row[26]) : 0;
        records[count].trade_restrict_old = row[27] ? atoi(row[27]) : 0;
        records[count].trade_restrict_new = row[28] ? atoi(row[28]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
