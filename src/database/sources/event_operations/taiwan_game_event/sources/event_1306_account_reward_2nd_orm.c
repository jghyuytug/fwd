#include "event_1306_account_reward_2nd_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Event1306AccountReward2Nd_Add(DBConnectionManager* manager, const Event1306AccountReward2Nd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_1306_account_reward_2nd (m_id, charac_no, occ_date) "
        "VALUES (%d, %d, '%s')",
        record->m_id, record->charac_no, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1306AccountReward2Nd_Get(DBConnectionManager* manager, int m_id, const char* occ_date, Event1306AccountReward2Nd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, occ_date FROM event_1306_account_reward_2nd WHERE m_id = %d AND occ_date = '%s'",
        m_id, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Event1306AccountReward2Nd));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_date, row[2], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Event1306AccountReward2Nd_GetAll(DBConnectionManager* manager, Event1306AccountReward2Nd* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, occ_date FROM event_1306_account_reward_2nd");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Event1306AccountReward2Nd));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_date, row[2], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
