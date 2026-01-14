#include "event_valentine_vote_info_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventValentineVoteInfoOld_Add(DBConnectionManager* manager, const EventValentineVoteInfoOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_valentine_vote_info_old (m_id, server_id, vote_A, vote_B, vote_C) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->m_id, record->server_id, record->vote_A, record->vote_B, record->vote_C);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteInfoOld_Get(DBConnectionManager* manager, int m_id, signed char server_id, EventValentineVoteInfoOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, vote_A, vote_B, vote_C FROM event_valentine_vote_info_old WHERE m_id = %d AND server_id = %d",
        m_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventValentineVoteInfoOld));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->vote_A = row[2] ? atoi(row[2]) : 0;
    record->vote_B = row[3] ? atoi(row[3]) : 0;
    record->vote_C = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventValentineVoteInfoOld_GetAll(DBConnectionManager* manager, EventValentineVoteInfoOld* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, vote_A, vote_B, vote_C FROM event_valentine_vote_info_old");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventValentineVoteInfoOld));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].vote_A = row[2] ? atoi(row[2]) : 0;
        records[count].vote_B = row[3] ? atoi(row[3]) : 0;
        records[count].vote_C = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
