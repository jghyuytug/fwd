#include "event_1112_ontime_reward_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Event1112OntimeRewardUser_Add(DBConnectionManager* manager, const Event1112OntimeRewardUser* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_1112_ontime_reward_user (m_id, recv_no) "
        "VALUES (%d, %d)",
        record->m_id, record->recv_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SE_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1112OntimeRewardUser_GetAll(DBConnectionManager* manager, Event1112OntimeRewardUser* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, recv_no FROM event_1112_ontime_reward_user");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SE_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Event1112OntimeRewardUser));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].recv_no = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
