#include "guild_notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildNotice_Add(DBConnectionManager* manager, const GuildNotice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_notice (guild_id, notice, acc_date) "
        "VALUES (%d, '%s', %d)",
        record->guild_id, record->notice, record->acc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildNotice_Get(DBConnectionManager* manager, int guild_id, GuildNotice* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, notice, acc_date FROM guild_notice WHERE guild_id = %d",
        guild_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildNotice));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->notice, row[1], sizeof(record->notice) - 1);
    record->acc_date = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildNotice_GetAll(DBConnectionManager* manager, GuildNotice* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, notice, acc_date FROM guild_notice");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildNotice));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].notice, row[1], sizeof(records[count].notice) - 1);
        records[count].acc_date = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
