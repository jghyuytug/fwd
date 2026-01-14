#include "guild_halloffame_html_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildHalloffameHtml_Add(DBConnectionManager* manager, const GuildHalloffameHtml* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_halloffame_html (fame_id, title, html) "
        "VALUES (%d, '%s', '%s')",
        record->fame_id, record->title, record->html);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffameHtml_Get(DBConnectionManager* manager, int fame_id, GuildHalloffameHtml* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, title, html FROM guild_halloffame_html WHERE fame_id = %d",
        fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildHalloffameHtml));
    record->fame_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    if (row[2]) strncpy(record->html, row[2], sizeof(record->html) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildHalloffameHtml_GetAll(DBConnectionManager* manager, GuildHalloffameHtml* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, title, html FROM guild_halloffame_html");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildHalloffameHtml));
        records[count].fame_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        if (row[2]) strncpy(records[count].html, row[2], sizeof(records[count].html) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
