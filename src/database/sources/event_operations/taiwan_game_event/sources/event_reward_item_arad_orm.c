#include "event_reward_item_arad_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventRewardItemArad_Add(DBConnectionManager* manager, const EventRewardItemArad* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_reward_item_arad (event_id, m_id, charac_no, item_id, end_date, occ_date) "
        "VALUES (%d, %d, %d, %d, '%s', '%s')",
        record->event_id, record->m_id, record->charac_no, record->item_id, record->end_date, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventRewardItemArad_Get(DBConnectionManager* manager, int event_id, int m_id, int charac_no, EventRewardItemArad* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, charac_no, item_id, end_date, occ_date FROM event_reward_item_arad WHERE event_id = %d AND m_id = %d AND charac_no = %d",
        event_id, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventRewardItemArad));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->item_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->end_date, row[4], sizeof(record->end_date) - 1);
    if (row[5]) strncpy(record->occ_date, row[5], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventRewardItemArad_GetAll(DBConnectionManager* manager, EventRewardItemArad* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, charac_no, item_id, end_date, occ_date FROM event_reward_item_arad");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventRewardItemArad));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].item_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].end_date, row[4], sizeof(records[count].end_date) - 1);
        if (row[5]) strncpy(records[count].occ_date, row[5], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
