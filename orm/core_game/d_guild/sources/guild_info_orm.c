#include "guild_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildInfo_Add(DBConnectionManager* manager, const GuildInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_info (server_id, guild_name, master_id, master_no, master_name, guild_url, guild_icon, create_time, lev, ability, expire_flag, expire_time, member_secede_time, member_count, recommend_flag, recommend_time, guild_point, guild_point_acc, guild_point_prev, guild_rank, guild_war_point, final_entry, final_win, guild_icon_auth, guild_exp, power_side, guild_agit_flag, lev_up_time, power_secede_time, power_war_point, power_join_count, guild_fund) "
        "VALUES (%d, '%s', %d, %d, '%s', '%s', %d, '%s', %d, %d, %d, '%s', '%s', %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', %d, %d, %d)",
        record->server_id, record->guild_name, record->master_id, record->master_no, record->master_name, record->guild_url, record->guild_icon, record->create_time, record->lev, record->ability, record->expire_flag, record->expire_time, record->member_secede_time, record->member_count, record->recommend_flag, record->recommend_time, record->guild_point, record->guild_point_acc, record->guild_point_prev, record->guild_rank, record->guild_war_point, record->final_entry, record->final_win, record->guild_icon_auth, record->guild_exp, record->power_side, record->guild_agit_flag, record->lev_up_time, record->power_secede_time, record->power_war_point, record->power_join_count, record->guild_fund);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildInfo_Get(DBConnectionManager* manager, int guild_id, GuildInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[33];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, guild_name, master_id, master_no, master_name, guild_url, guild_icon, create_time, lev, ability, expire_flag, expire_time, member_secede_time, member_count, recommend_flag, recommend_time, guild_point, guild_point_acc, guild_point_prev, guild_rank, guild_war_point, final_entry, final_win, guild_icon_auth, guild_exp, power_side, guild_agit_flag, lev_up_time, power_secede_time, power_war_point, power_join_count, guild_fund FROM guild_info WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildInfo));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->guild_name, row[2], sizeof(record->guild_name) - 1);
    record->master_id = row[3] ? atoi(row[3]) : 0;
    record->master_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->master_name, row[5], sizeof(record->master_name) - 1);
    if (row[6]) strncpy(record->guild_url, row[6], sizeof(record->guild_url) - 1);
    record->guild_icon = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->create_time, row[8], sizeof(record->create_time) - 1);
    record->lev = row[9] ? atoi(row[9]) : 0;
    record->ability = row[10] ? atoi(row[10]) : 0;
    record->expire_flag = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->expire_time, row[12], sizeof(record->expire_time) - 1);
    if (row[13]) strncpy(record->member_secede_time, row[13], sizeof(record->member_secede_time) - 1);
    record->member_count = row[14] ? atoi(row[14]) : 0;
    record->recommend_flag = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->recommend_time, row[16], sizeof(record->recommend_time) - 1);
    record->guild_point = row[17] ? atoi(row[17]) : 0;
    record->guild_point_acc = row[18] ? atoi(row[18]) : 0;
    record->guild_point_prev = row[19] ? atoi(row[19]) : 0;
    record->guild_rank = row[20] ? atoi(row[20]) : 0;
    record->guild_war_point = row[21] ? atoi(row[21]) : 0;
    record->final_entry = row[22] ? atoi(row[22]) : 0;
    record->final_win = row[23] ? atoi(row[23]) : 0;
    record->guild_icon_auth = row[24] ? atoi(row[24]) : 0;
    record->guild_exp = row[25] ? atoi(row[25]) : 0;
    record->power_side = row[26] ? atoi(row[26]) : 0;
    record->guild_agit_flag = row[27] ? atoi(row[27]) : 0;
    if (row[28]) strncpy(record->lev_up_time, row[28], sizeof(record->lev_up_time) - 1);
    if (row[29]) strncpy(record->power_secede_time, row[29], sizeof(record->power_secede_time) - 1);
    record->power_war_point = row[30] ? atoi(row[30]) : 0;
    record->power_join_count = row[31] ? atoi(row[31]) : 0;
    record->guild_fund = row[32] ? atoi(row[32]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildInfo_GetAll(DBConnectionManager* manager, GuildInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[33];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, guild_name, master_id, master_no, master_name, guild_url, guild_icon, create_time, lev, ability, expire_flag, expire_time, member_secede_time, member_count, recommend_flag, recommend_time, guild_point, guild_point_acc, guild_point_prev, guild_rank, guild_war_point, final_entry, final_win, guild_icon_auth, guild_exp, power_side, guild_agit_flag, lev_up_time, power_secede_time, power_war_point, power_join_count, guild_fund FROM guild_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildInfo));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].guild_name, row[2], sizeof(records[count].guild_name) - 1);
        records[count].master_id = row[3] ? atoi(row[3]) : 0;
        records[count].master_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].master_name, row[5], sizeof(records[count].master_name) - 1);
        if (row[6]) strncpy(records[count].guild_url, row[6], sizeof(records[count].guild_url) - 1);
        records[count].guild_icon = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].create_time, row[8], sizeof(records[count].create_time) - 1);
        records[count].lev = row[9] ? atoi(row[9]) : 0;
        records[count].ability = row[10] ? atoi(row[10]) : 0;
        records[count].expire_flag = row[11] ? atoi(row[11]) : 0;
        if (row[12]) strncpy(records[count].expire_time, row[12], sizeof(records[count].expire_time) - 1);
        if (row[13]) strncpy(records[count].member_secede_time, row[13], sizeof(records[count].member_secede_time) - 1);
        records[count].member_count = row[14] ? atoi(row[14]) : 0;
        records[count].recommend_flag = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].recommend_time, row[16], sizeof(records[count].recommend_time) - 1);
        records[count].guild_point = row[17] ? atoi(row[17]) : 0;
        records[count].guild_point_acc = row[18] ? atoi(row[18]) : 0;
        records[count].guild_point_prev = row[19] ? atoi(row[19]) : 0;
        records[count].guild_rank = row[20] ? atoi(row[20]) : 0;
        records[count].guild_war_point = row[21] ? atoi(row[21]) : 0;
        records[count].final_entry = row[22] ? atoi(row[22]) : 0;
        records[count].final_win = row[23] ? atoi(row[23]) : 0;
        records[count].guild_icon_auth = row[24] ? atoi(row[24]) : 0;
        records[count].guild_exp = row[25] ? atoi(row[25]) : 0;
        records[count].power_side = row[26] ? atoi(row[26]) : 0;
        records[count].guild_agit_flag = row[27] ? atoi(row[27]) : 0;
        if (row[28]) strncpy(records[count].lev_up_time, row[28], sizeof(records[count].lev_up_time) - 1);
        if (row[29]) strncpy(records[count].power_secede_time, row[29], sizeof(records[count].power_secede_time) - 1);
        records[count].power_war_point = row[30] ? atoi(row[30]) : 0;
        records[count].power_join_count = row[31] ? atoi(row[31]) : 0;
        records[count].guild_fund = row[32] ? atoi(row[32]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Extended operations */
int GuildInfo_AddEx(DBConnectionManager* manager, const GuildInfo* record, unsigned long long* out_guild_id) {
    char query[MAX_QUERY_LEN];

    if (!manager || !record || !out_guild_id) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_info (server_id, guild_name, master_id, master_no, master_name, guild_url, guild_icon, create_time, lev, ability, expire_flag, expire_time, member_secede_time, member_count, recommend_flag, recommend_time, guild_point, guild_point_acc, guild_point_prev, guild_rank, guild_war_point, final_entry, final_win, guild_icon_auth, guild_exp, power_side, guild_agit_flag, lev_up_time, power_secede_time, power_war_point, power_join_count, guild_fund) "
        "VALUES (%d, '%s', %d, %d, '%s', '%s', %d, '%s', %d, %d, %d, '%s', '%s', %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', %d, %d, %d)",
        record->server_id, record->guild_name, record->master_id, record->master_no, record->master_name, record->guild_url, record->guild_icon, record->create_time, record->lev, record->ability, record->expire_flag, record->expire_time, record->member_secede_time, record->member_count, record->recommend_flag, record->recommend_time, record->guild_point, record->guild_point_acc, record->guild_point_prev, record->guild_rank, record->guild_war_point, record->final_entry, record->final_win, record->guild_icon_auth, record->guild_exp, record->power_side, record->guild_agit_flag, record->lev_up_time, record->power_secede_time, record->power_war_point, record->power_join_count, record->guild_fund);

    if (DBConnectionManager_ExecuteInsert(manager, DB_TYPE_GUILD, query, out_guild_id) < 0) {
        return -1;
    }

    return 0;
}

int GuildInfo_UpdateMemberCount(DBConnectionManager* manager, int guild_id, int delta) {
    char query[512];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET member_count = member_count + %d WHERE guild_id = %d",
             delta, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected > 0 ? 0 : -1;
}

int GuildInfo_SetExpireFlag(DBConnectionManager* manager, int guild_id, int flag) {
    char query[512];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET expire_flag = %d WHERE guild_id = %d",
             flag, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected > 0 ? 0 : -1;
}
