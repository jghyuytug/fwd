#include "guild_event_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildEvent_Add(DBConnectionManager* manager, const GuildEvent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_event (gno, stt_date, end_date, ann_date, page_url) "
        "VALUES (%d, '%s', '%s', '%s', '%s')",
        record->gno, record->stt_date, record->end_date, record->ann_date, record->page_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildEvent_Get(DBConnectionManager* manager, int gno, GuildEvent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url FROM guild_event WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildEvent));
    record->gno = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->stt_date, row[1], sizeof(record->stt_date) - 1);
    if (row[2]) strncpy(record->end_date, row[2], sizeof(record->end_date) - 1);
    if (row[3]) strncpy(record->ann_date, row[3], sizeof(record->ann_date) - 1);
    if (row[4]) strncpy(record->page_url, row[4], sizeof(record->page_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildEvent_GetAll(DBConnectionManager* manager, GuildEvent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url FROM guild_event");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildEvent));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].stt_date, row[1], sizeof(records[count].stt_date) - 1);
        if (row[2]) strncpy(records[count].end_date, row[2], sizeof(records[count].end_date) - 1);
        if (row[3]) strncpy(records[count].ann_date, row[3], sizeof(records[count].ann_date) - 1);
        if (row[4]) strncpy(records[count].page_url, row[4], sizeof(records[count].page_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
