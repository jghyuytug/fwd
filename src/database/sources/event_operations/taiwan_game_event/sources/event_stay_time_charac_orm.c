#include "event_stay_time_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventStayTimeCharac_Add(DBConnectionManager* manager, const EventStayTimeCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_stay_time_charac (m_id, reward_count, reward_flag, mod_date) "
        "VALUES (%d, %d, %d, '%s')",
        record->m_id, record->reward_count, record->reward_flag, record->mod_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventStayTimeCharac_Get(DBConnectionManager* manager, int m_id, EventStayTimeCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reward_count, reward_flag, mod_date FROM event_stay_time_charac WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventStayTimeCharac));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->reward_count = row[1] ? atoi(row[1]) : 0;
    record->reward_flag = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->mod_date, row[3], sizeof(record->mod_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventStayTimeCharac_GetAll(DBConnectionManager* manager, EventStayTimeCharac* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reward_count, reward_flag, mod_date FROM event_stay_time_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventStayTimeCharac));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].reward_count = row[1] ? atoi(row[1]) : 0;
        records[count].reward_flag = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].mod_date, row[3], sizeof(records[count].mod_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
