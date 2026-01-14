#include "guild_halloffame_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildHalloffame_Add(DBConnectionManager* manager, const GuildHalloffame* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_halloffame (fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag) "
        "VALUES (%d, %d, %d, '%s', '%s', %d, %d)",
        record->fame_id, record->server_id, record->guild_id, record->guild_name, record->file_url, record->open_flag, record->main_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_Get(DBConnectionManager* manager, int fame_id, signed char server_id, GuildHalloffame* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag FROM guild_halloffame WHERE fame_id = %d AND server_id = %d",
        fame_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildHalloffame));
    record->fame_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->guild_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->guild_name, row[3], sizeof(record->guild_name) - 1);
    if (row[4]) strncpy(record->file_url, row[4], sizeof(record->file_url) - 1);
    record->open_flag = row[5] ? atoi(row[5]) : 0;
    record->main_flag = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffame_GetAll(DBConnectionManager* manager, GuildHalloffame* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, server_id, guild_id, guild_name, file_url, open_flag, main_flag FROM guild_halloffame");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildHalloffame));
        records[count].fame_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].guild_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].guild_name, row[3], sizeof(records[count].guild_name) - 1);
        if (row[4]) strncpy(records[count].file_url, row[4], sizeof(records[count].file_url) - 1);
        records[count].open_flag = row[5] ? atoi(row[5]) : 0;
        records[count].main_flag = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
