#include "charac_postal_add_20160429_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacPostalAdd20160429_Add(DBConnectionManager* manager, const CharacPostalAdd20160429* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_postal_add_20160429 (server_info, postal_id, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, extend_info, random_option, seperate_upgrade, upgrade, gold, avata_flag, unlimit_flag, seal_flag, creature_flag, creature_stomach, creature_exp, creature_no_charge, avatar_expire_date, avatar_ability_no, avatar_jewel_socket, MNG_user_id, work_time, memo, group_id, send_flag) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', '%s', '%s', '%s', %d, %d)",
        record->server_info, record->postal_id, record->send_charac_no, record->send_charac_name, record->receive_charac_no, record->item_id, record->add_info, record->endurance, record->extend_info, record->random_option, record->seperate_upgrade, record->upgrade, record->gold, record->avata_flag, record->unlimit_flag, record->seal_flag, record->creature_flag, record->creature_stomach, record->creature_exp, record->creature_no_charge, record->avatar_expire_date, record->avatar_ability_no, record->avatar_jewel_socket, record->MNG_user_id, record->work_time, record->memo, record->group_id, record->send_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacPostalAdd20160429_Get(DBConnectionManager* manager, int id, CharacPostalAdd20160429* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, postal_id, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, extend_info, random_option, seperate_upgrade, upgrade, gold, avata_flag, unlimit_flag, seal_flag, creature_flag, creature_stomach, creature_exp, creature_no_charge, avatar_expire_date, avatar_ability_no, avatar_jewel_socket, MNG_user_id, work_time, memo, group_id, send_flag FROM charac_postal_add_20160429 WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacPostalAdd20160429));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->postal_id = row[2] ? atoi(row[2]) : 0;
    record->send_charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->send_charac_name, row[4], sizeof(record->send_charac_name) - 1);
    record->receive_charac_no = row[5] ? atoi(row[5]) : 0;
    record->item_id = row[6] ? atoi(row[6]) : 0;
    record->add_info = row[7] ? atoi(row[7]) : 0;
    record->endurance = row[8] ? atoi(row[8]) : 0;
    record->extend_info = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->random_option, row[10], sizeof(record->random_option) - 1);
    record->seperate_upgrade = row[11] ? atoi(row[11]) : 0;
    record->upgrade = row[12] ? atoi(row[12]) : 0;
    record->gold = row[13] ? atoi(row[13]) : 0;
    record->avata_flag = row[14] ? atoi(row[14]) : 0;
    record->unlimit_flag = row[15] ? atoi(row[15]) : 0;
    record->seal_flag = row[16] ? atoi(row[16]) : 0;
    record->creature_flag = row[17] ? atoi(row[17]) : 0;
    record->creature_stomach = row[18] ? atoi(row[18]) : 0;
    record->creature_exp = row[19] ? atoi(row[19]) : 0;
    record->creature_no_charge = row[20] ? atoi(row[20]) : 0;
    if (row[21]) strncpy(record->avatar_expire_date, row[21], sizeof(record->avatar_expire_date) - 1);
    record->avatar_ability_no = row[22] ? atoi(row[22]) : 0;
    if (row[23]) memcpy(record->avatar_jewel_socket, row[23], sizeof(record->avatar_jewel_socket));
    if (row[24]) strncpy(record->MNG_user_id, row[24], sizeof(record->MNG_user_id) - 1);
    if (row[25]) strncpy(record->work_time, row[25], sizeof(record->work_time) - 1);
    if (row[26]) strncpy(record->memo, row[26], sizeof(record->memo) - 1);
    record->group_id = row[27] ? atoi(row[27]) : 0;
    record->send_flag = row[28] ? atoi(row[28]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacPostalAdd20160429_GetAll(DBConnectionManager* manager, CharacPostalAdd20160429* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, postal_id, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, extend_info, random_option, seperate_upgrade, upgrade, gold, avata_flag, unlimit_flag, seal_flag, creature_flag, creature_stomach, creature_exp, creature_no_charge, avatar_expire_date, avatar_ability_no, avatar_jewel_socket, MNG_user_id, work_time, memo, group_id, send_flag FROM charac_postal_add_20160429");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacPostalAdd20160429));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].postal_id = row[2] ? atoi(row[2]) : 0;
        records[count].send_charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].send_charac_name, row[4], sizeof(records[count].send_charac_name) - 1);
        records[count].receive_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].item_id = row[6] ? atoi(row[6]) : 0;
        records[count].add_info = row[7] ? atoi(row[7]) : 0;
        records[count].endurance = row[8] ? atoi(row[8]) : 0;
        records[count].extend_info = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].random_option, row[10], sizeof(records[count].random_option) - 1);
        records[count].seperate_upgrade = row[11] ? atoi(row[11]) : 0;
        records[count].upgrade = row[12] ? atoi(row[12]) : 0;
        records[count].gold = row[13] ? atoi(row[13]) : 0;
        records[count].avata_flag = row[14] ? atoi(row[14]) : 0;
        records[count].unlimit_flag = row[15] ? atoi(row[15]) : 0;
        records[count].seal_flag = row[16] ? atoi(row[16]) : 0;
        records[count].creature_flag = row[17] ? atoi(row[17]) : 0;
        records[count].creature_stomach = row[18] ? atoi(row[18]) : 0;
        records[count].creature_exp = row[19] ? atoi(row[19]) : 0;
        records[count].creature_no_charge = row[20] ? atoi(row[20]) : 0;
        if (row[21]) strncpy(records[count].avatar_expire_date, row[21], sizeof(records[count].avatar_expire_date) - 1);
        records[count].avatar_ability_no = row[22] ? atoi(row[22]) : 0;
        if (row[23]) memcpy(records[count].avatar_jewel_socket, row[23], sizeof(records[count].avatar_jewel_socket));
        if (row[24]) strncpy(records[count].MNG_user_id, row[24], sizeof(records[count].MNG_user_id) - 1);
        if (row[25]) strncpy(records[count].work_time, row[25], sizeof(records[count].work_time) - 1);
        if (row[26]) strncpy(records[count].memo, row[26], sizeof(records[count].memo) - 1);
        records[count].group_id = row[27] ? atoi(row[27]) : 0;
        records[count].send_flag = row[28] ? atoi(row[28]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
