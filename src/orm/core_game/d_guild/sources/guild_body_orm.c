#include "guild_body_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildBody_Add(DBConnectionManager* manager, const GuildBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_body (gno, body) "
        "VALUES (%d, '%s')",
        record->gno, record->body);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_Get(DBConnectionManager* manager, int gno, GuildBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, body FROM guild_body WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildBody));
    record->gno = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->body, row[1], sizeof(record->body) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBody_GetAll(DBConnectionManager* manager, GuildBody* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, body FROM guild_body");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildBody));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].body, row[1], sizeof(records[count].body) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
