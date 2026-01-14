#include "guild_search_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildSearch_Add(DBConnectionManager* manager, const GuildSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_search (guild_id, server_id, guild_name, master_name, create_time, lev, member_count, guild_point_acc, guild_exp, guild_url) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, '%s')",
        record->guild_id, record->server_id, record->guild_name, record->master_name, record->create_time, record->lev, record->member_count, record->guild_point_acc, record->guild_exp, record->guild_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildSearch_Get(DBConnectionManager* manager, int guild_id, GuildSearch* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, guild_name, master_name, create_time, lev, member_count, guild_point_acc, guild_exp, guild_url FROM guild_search WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildSearch));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->guild_name, row[2], sizeof(record->guild_name) - 1);
    if (row[3]) strncpy(record->master_name, row[3], sizeof(record->master_name) - 1);
    if (row[4]) strncpy(record->create_time, row[4], sizeof(record->create_time) - 1);
    record->lev = row[5] ? atoi(row[5]) : 0;
    record->member_count = row[6] ? atoi(row[6]) : 0;
    record->guild_point_acc = row[7] ? atoi(row[7]) : 0;
    record->guild_exp = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->guild_url, row[9], sizeof(record->guild_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildSearch_GetAll(DBConnectionManager* manager, GuildSearch* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, guild_name, master_name, create_time, lev, member_count, guild_point_acc, guild_exp, guild_url FROM guild_search");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildSearch));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].guild_name, row[2], sizeof(records[count].guild_name) - 1);
        if (row[3]) strncpy(records[count].master_name, row[3], sizeof(records[count].master_name) - 1);
        if (row[4]) strncpy(records[count].create_time, row[4], sizeof(records[count].create_time) - 1);
        records[count].lev = row[5] ? atoi(row[5]) : 0;
        records[count].member_count = row[6] ? atoi(row[6]) : 0;
        records[count].guild_point_acc = row[7] ? atoi(row[7]) : 0;
        records[count].guild_exp = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].guild_url, row[9], sizeof(records[count].guild_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
