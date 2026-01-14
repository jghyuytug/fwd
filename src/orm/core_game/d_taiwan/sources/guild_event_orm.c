#include "guild_event_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild event */
int GuildEvent_Add(DBConnectionManager* manager, const GuildEvent* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_event (gno, stt_date, end_date, ann_date, page_url) "
        "VALUES (%d, '%s', '%s', '%s', '%s')",
        event->gno, event->stt_date, event->end_date,
        event->ann_date, event->page_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild event by gno */
int GuildEvent_Get(DBConnectionManager* manager, int gno, GuildEvent* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url "
        "FROM guild_event WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(event, 0, sizeof(GuildEvent));
    event->gno = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(event->stt_date, row[1], sizeof(event->stt_date) - 1);
    if (row[2]) strncpy(event->end_date, row[2], sizeof(event->end_date) - 1);
    if (row[3]) strncpy(event->ann_date, row[3], sizeof(event->ann_date) - 1);
    if (row[4]) strncpy(event->page_url, row[4], sizeof(event->page_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update guild event */
int GuildEvent_Update(DBConnectionManager* manager, const GuildEvent* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_event SET stt_date = '%s', end_date = '%s', "
        "ann_date = '%s', page_url = '%s' WHERE gno = %d",
        event->stt_date, event->end_date, event->ann_date,
        event->page_url, event->gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild event */
int GuildEvent_Delete(DBConnectionManager* manager, int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_event WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild event exists */
int GuildEvent_Exists(DBConnectionManager* manager, int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_event WHERE gno = %d", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all guild events */
int GuildEvent_GetAll(DBConnectionManager* manager, GuildEvent* events,
                      int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url "
        "FROM guild_event ORDER BY gno");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(GuildEvent));
        events[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(events[count].stt_date, row[1], sizeof(events[count].stt_date) - 1);
        if (row[2]) strncpy(events[count].end_date, row[2], sizeof(events[count].end_date) - 1);
        if (row[3]) strncpy(events[count].ann_date, row[3], sizeof(events[count].ann_date) - 1);
        if (row[4]) strncpy(events[count].page_url, row[4], sizeof(events[count].page_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get active guild events (current_date between stt_date and end_date) */
int GuildEvent_GetActive(DBConnectionManager* manager, const char* current_date,
                         GuildEvent* events, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !current_date || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url "
        "FROM guild_event WHERE '%s' BETWEEN stt_date AND end_date "
        "ORDER BY gno", current_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(GuildEvent));
        events[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(events[count].stt_date, row[1], sizeof(events[count].stt_date) - 1);
        if (row[2]) strncpy(events[count].end_date, row[2], sizeof(events[count].end_date) - 1);
        if (row[3]) strncpy(events[count].ann_date, row[3], sizeof(events[count].ann_date) - 1);
        if (row[4]) strncpy(events[count].page_url, row[4], sizeof(events[count].page_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild events by date range */
int GuildEvent_GetByDateRange(DBConnectionManager* manager,
                               const char* start_date, const char* end_date,
                               GuildEvent* events, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !start_date || !end_date || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, stt_date, end_date, ann_date, page_url "
        "FROM guild_event WHERE stt_date >= '%s' AND end_date <= '%s' "
        "ORDER BY stt_date", start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(GuildEvent));
        events[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(events[count].stt_date, row[1], sizeof(events[count].stt_date) - 1);
        if (row[2]) strncpy(events[count].end_date, row[2], sizeof(events[count].end_date) - 1);
        if (row[3]) strncpy(events[count].ann_date, row[3], sizeof(events[count].ann_date) - 1);
        if (row[4]) strncpy(events[count].page_url, row[4], sizeof(events[count].page_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild event information */
void GuildEvent_PrintInfo(const GuildEvent* event) {
    if (!event) return;

    printf("Guild Event Info:\n");
    printf("  GNO: %d\n", event->gno);
    printf("  Start Date: %s\n", event->stt_date);
    printf("  End Date: %s\n", event->end_date);
    printf("  Announce Date: %s\n", event->ann_date);
    printf("  Page URL: %s\n", event->page_url);
}