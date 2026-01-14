#include "gm_message_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GmMessage_Add(DBConnectionManager* manager, const GmMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gm_message (event_id, server_id, msg_order, send_time, msg_type, msg, send_charac_name) "
        "VALUES (%d, %d, %d, %d, %d, '%s', '%s')",
        record->event_id, record->server_id, record->msg_order, record->send_time, record->msg_type, record->msg, record->send_charac_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GmMessage_Get(DBConnectionManager* manager, signed char event_id, signed char server_id, signed char msg_order, int send_time, GmMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, server_id, msg_order, send_time, msg_type, msg, send_charac_name FROM gm_message WHERE event_id = %d AND server_id = %d AND msg_order = %d AND send_time = %d",
        event_id, server_id, msg_order, send_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GmMessage));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->msg_order = row[2] ? atoi(row[2]) : 0;
    record->send_time = row[3] ? atoi(row[3]) : 0;
    record->msg_type = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->msg, row[5], sizeof(record->msg) - 1);
    if (row[6]) strncpy(record->send_charac_name, row[6], sizeof(record->send_charac_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GmMessage_GetAll(DBConnectionManager* manager, GmMessage* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, server_id, msg_order, send_time, msg_type, msg, send_charac_name FROM gm_message");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GmMessage));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].msg_order = row[2] ? atoi(row[2]) : 0;
        records[count].send_time = row[3] ? atoi(row[3]) : 0;
        records[count].msg_type = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].msg, row[5], sizeof(records[count].msg) - 1);
        if (row[6]) strncpy(records[count].send_charac_name, row[6], sizeof(records[count].send_charac_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
