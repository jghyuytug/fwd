#include "tcg_relay_entry_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgRelayEntryCount_Add(DBConnectionManager* manager, const TcgRelayEntryCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_relay_entry_count (event_seq, coupon_count) "
        "VALUES (%d, %d)",
        record->event_seq, record->coupon_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgRelayEntryCount_Get(DBConnectionManager* manager, int event_seq, TcgRelayEntryCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_seq, coupon_count FROM tcg_relay_entry_count WHERE event_seq = %d",
        event_seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TcgRelayEntryCount));
    record->event_seq = row[0] ? atoi(row[0]) : 0;
    record->coupon_count = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int TcgRelayEntryCount_GetAll(DBConnectionManager* manager, TcgRelayEntryCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_seq, coupon_count FROM tcg_relay_entry_count");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgRelayEntryCount));
        records[count].event_seq = row[0] ? atoi(row[0]) : 0;
        records[count].coupon_count = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
