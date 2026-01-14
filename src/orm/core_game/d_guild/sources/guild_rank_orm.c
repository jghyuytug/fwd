#include "guild_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildRank_Add(DBConnectionManager* manager, const GuildRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_rank (server_id, guild_id, guild_name, guild_Rank, guild_point, guild_acc_point, guild_visit, guild_acc_visit, guild_member, guild_acc_member, guild_avg_lev) "
        "VALUES (%d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->server_id, record->guild_id, record->guild_name, record->guild_Rank, record->guild_point, record->guild_acc_point, record->guild_visit, record->guild_acc_visit, record->guild_member, record->guild_acc_member, record->guild_avg_lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildRank_Get(DBConnectionManager* manager, signed char server_id, int guild_id, GuildRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, guild_acc_point, guild_visit, guild_acc_visit, guild_member, guild_acc_member, guild_avg_lev FROM guild_rank WHERE server_id = %d AND guild_id = %d",
        server_id, guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildRank));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->guild_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->guild_name, row[2], sizeof(record->guild_name) - 1);
    record->guild_Rank = row[3] ? atoi(row[3]) : 0;
    record->guild_point = row[4] ? atoi(row[4]) : 0;
    record->guild_acc_point = row[5] ? atoi(row[5]) : 0;
    record->guild_visit = row[6] ? atoi(row[6]) : 0;
    record->guild_acc_visit = row[7] ? atoi(row[7]) : 0;
    record->guild_member = row[8] ? atoi(row[8]) : 0;
    record->guild_acc_member = row[9] ? atoi(row[9]) : 0;
    record->guild_avg_lev = row[10] ? atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildRank_GetAll(DBConnectionManager* manager, GuildRank* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, guild_id, guild_name, guild_Rank, guild_point, guild_acc_point, guild_visit, guild_acc_visit, guild_member, guild_acc_member, guild_avg_lev FROM guild_rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildRank));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].guild_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].guild_name, row[2], sizeof(records[count].guild_name) - 1);
        records[count].guild_Rank = row[3] ? atoi(row[3]) : 0;
        records[count].guild_point = row[4] ? atoi(row[4]) : 0;
        records[count].guild_acc_point = row[5] ? atoi(row[5]) : 0;
        records[count].guild_visit = row[6] ? atoi(row[6]) : 0;
        records[count].guild_acc_visit = row[7] ? atoi(row[7]) : 0;
        records[count].guild_member = row[8] ? atoi(row[8]) : 0;
        records[count].guild_acc_member = row[9] ? atoi(row[9]) : 0;
        records[count].guild_avg_lev = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
