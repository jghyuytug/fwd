#include "event_1207_growthweapon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Event1207Growthweapon_Add(DBConnectionManager* manager, const Event1207Growthweapon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_1207_growthweapon (m_id, server_id, charac_no, growthweapon_id, timepiece, occ_date, infinityweapon_id, reward_occ_date) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d, '%s')",
        record->m_id, record->server_id, record->charac_no, record->growthweapon_id, record->timepiece, record->occ_date, record->infinityweapon_id, record->reward_occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Event1207Growthweapon_Get(DBConnectionManager* manager, int m_id, Event1207Growthweapon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, growthweapon_id, timepiece, occ_date, infinityweapon_id, reward_occ_date FROM event_1207_growthweapon WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Event1207Growthweapon));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->growthweapon_id = row[3] ? atoi(row[3]) : 0;
    record->timepiece = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->occ_date, row[5], sizeof(record->occ_date) - 1);
    record->infinityweapon_id = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->reward_occ_date, row[7], sizeof(record->reward_occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Event1207Growthweapon_GetAll(DBConnectionManager* manager, Event1207Growthweapon* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, growthweapon_id, timepiece, occ_date, infinityweapon_id, reward_occ_date FROM event_1207_growthweapon");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Event1207Growthweapon));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].growthweapon_id = row[3] ? atoi(row[3]) : 0;
        records[count].timepiece = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].occ_date, row[5], sizeof(records[count].occ_date) - 1);
        records[count].infinityweapon_id = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].reward_occ_date, row[7], sizeof(records[count].reward_occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
