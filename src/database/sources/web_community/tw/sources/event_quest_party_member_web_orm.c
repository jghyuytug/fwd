#include "event_quest_party_member_web_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventQuestPartyMemberWeb_Add(DBConnectionManager* manager, const EventQuestPartyMemberWeb* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_quest_party_member_web (m_id, server_id, charac_no, quest_no, occ_time, send_charac_no) "
        "VALUES (%d, %d, %d, %d, '%s', %d)",
        record->m_id, record->server_id, record->charac_no, record->quest_no, record->occ_time, record->send_charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_Get(DBConnectionManager* manager, int m_id, signed char server_id, int charac_no, int quest_no, EventQuestPartyMemberWeb* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, quest_no, occ_time, send_charac_no FROM event_quest_party_member_web WHERE m_id = %d AND server_id = %d AND charac_no = %d AND quest_no = %d",
        m_id, server_id, charac_no, quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventQuestPartyMemberWeb));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->quest_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->occ_time, row[4], sizeof(record->occ_time) - 1);
    record->send_charac_no = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_GetAll(DBConnectionManager* manager, EventQuestPartyMemberWeb* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, quest_no, occ_time, send_charac_no FROM event_quest_party_member_web");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuestPartyMemberWeb));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].quest_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].occ_time, row[4], sizeof(records[count].occ_time) - 1);
        records[count].send_charac_no = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
