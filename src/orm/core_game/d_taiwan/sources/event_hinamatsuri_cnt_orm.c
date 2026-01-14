#include "event_hinamatsuri_cnt_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventHinamatsuriCnt_Get(DBConnectionManager* manager, int* cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !cnt) return -1;

    snprintf(query, sizeof(query),
        "SELECT cnt FROM event_hinamatsuri_cnt LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *cnt = row[0] ? atoi(row[0]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventHinamatsuriCnt_Set(DBConnectionManager* manager, int cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    /* Delete all rows first, then insert the new value */
    snprintf(query, sizeof(query),
        "DELETE FROM event_hinamatsuri_cnt");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);

    snprintf(query, sizeof(query),
        "INSERT INTO event_hinamatsuri_cnt (cnt) VALUES (%d)", cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventHinamatsuriCnt_Increment(DBConnectionManager* manager, int delta) {
    int current_cnt = 0;

    if (!manager) return -1;

    /* Get current value */
    if (EventHinamatsuriCnt_Get(manager, &current_cnt) < 0) {
        /* If no row exists, initialize to 0 */
        current_cnt = 0;
    }

    /* Set new value */
    return EventHinamatsuriCnt_Set(manager, current_cnt + delta);
}

int EventHinamatsuriCnt_Reset(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_hinamatsuri_cnt");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventHinamatsuriCnt_PrintInfo(const EventHinamatsuriCnt* counter) {
    if (!counter) {
        printf("EventHinamatsuriCnt: NULL\n");
        return;
    }

    printf("EventHinamatsuriCnt:\n");
    printf("  cnt: %d\n", counter->cnt);
}
