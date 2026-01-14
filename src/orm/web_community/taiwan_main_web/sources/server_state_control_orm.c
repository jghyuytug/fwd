#include "server_state_control_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ServerStateControl_Add(DBConnectionManager* manager, const ServerStateControl* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO server_state_control (server_id, title, flag, link_url, pop_up) "
        "VALUES (%d, '%s', %d, '%s', %d)",
        record->server_id, record->title, record->flag, record->link_url, record->pop_up);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateControl_Get(DBConnectionManager* manager, signed char server_id, ServerStateControl* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, title, flag, link_url, pop_up FROM server_state_control WHERE server_id = %d",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ServerStateControl));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->title, row[1], sizeof(record->title) - 1);
    record->flag = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->link_url, row[3], sizeof(record->link_url) - 1);
    record->pop_up = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateControl_GetAll(DBConnectionManager* manager, ServerStateControl* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, title, flag, link_url, pop_up FROM server_state_control");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ServerStateControl));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].title, row[1], sizeof(records[count].title) - 1);
        records[count].flag = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].link_url, row[3], sizeof(records[count].link_url) - 1);
        records[count].pop_up = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
