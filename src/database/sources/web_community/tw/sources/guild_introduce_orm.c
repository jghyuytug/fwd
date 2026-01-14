#include "guild_introduce_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildIntroduce_Add(DBConnectionManager* manager, const GuildIntroduce* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_introduce (guild_id, server_id, introduce) "
        "VALUES (%d, %d, '%s')",
        record->guild_id, record->server_id, record->introduce);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildIntroduce_Get(DBConnectionManager* manager, int guild_id, GuildIntroduce* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, introduce FROM guild_introduce WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildIntroduce));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->introduce, row[2], sizeof(record->introduce) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildIntroduce_GetAll(DBConnectionManager* manager, GuildIntroduce* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, server_id, introduce FROM guild_introduce");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildIntroduce));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].introduce, row[2], sizeof(records[count].introduce) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
