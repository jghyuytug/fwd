#include "event_reserving_charac_name_20130328_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventReservingCharacName20130328_Add(DBConnectionManager* manager, const EventReservingCharacName20130328* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_reserving_charac_name_20130328 (user_id, server_info, charac_name) "
        "VALUES ('%s', %d, '%s')",
        record->user_id, record->server_info, record->charac_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventReservingCharacName20130328_GetAll(DBConnectionManager* manager, EventReservingCharacName20130328* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT user_id, server_info, charac_name FROM event_reserving_charac_name_20130328");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventReservingCharacName20130328));
        if (row[0]) strncpy(records[count].user_id, row[0], sizeof(records[count].user_id) - 1);
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].charac_name, row[2], sizeof(records[count].charac_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
