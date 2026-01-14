#include "event_quest_party_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventQuestPartyMember_Add(DBConnectionManager* manager, const EventQuestPartyMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_quest_party_member (charac_no, quest_no, member_1, member_1_name, member_1_lev, member_2, member_2_name, member_2_lev, member_3, member_3_name, member_3_lev, m_id) "
        "VALUES (%d, %d, %d, '%s', %d, %d, '%s', %d, %d, '%s', %d, %d)",
        record->charac_no, record->quest_no, record->member_1, record->member_1_name, record->member_1_lev, record->member_2, record->member_2_name, record->member_2_lev, record->member_3, record->member_3_name, record->member_3_lev, record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMember_Get(DBConnectionManager* manager, int charac_no, int quest_no, EventQuestPartyMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, quest_no, member_1, member_1_name, member_1_lev, member_2, member_2_name, member_2_lev, member_3, member_3_name, member_3_lev, m_id FROM event_quest_party_member WHERE charac_no = %d AND quest_no = %d",
        charac_no, quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventQuestPartyMember));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->quest_no = row[1] ? atoi(row[1]) : 0;
    record->member_1 = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->member_1_name, row[3], sizeof(record->member_1_name) - 1);
    record->member_1_lev = row[4] ? atoi(row[4]) : 0;
    record->member_2 = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->member_2_name, row[6], sizeof(record->member_2_name) - 1);
    record->member_2_lev = row[7] ? atoi(row[7]) : 0;
    record->member_3 = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->member_3_name, row[9], sizeof(record->member_3_name) - 1);
    record->member_3_lev = row[10] ? atoi(row[10]) : 0;
    record->m_id = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMember_GetAll(DBConnectionManager* manager, EventQuestPartyMember* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, quest_no, member_1, member_1_name, member_1_lev, member_2, member_2_name, member_2_lev, member_3, member_3_name, member_3_lev, m_id FROM event_quest_party_member");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventQuestPartyMember));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].quest_no = row[1] ? atoi(row[1]) : 0;
        records[count].member_1 = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].member_1_name, row[3], sizeof(records[count].member_1_name) - 1);
        records[count].member_1_lev = row[4] ? atoi(row[4]) : 0;
        records[count].member_2 = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].member_2_name, row[6], sizeof(records[count].member_2_name) - 1);
        records[count].member_2_lev = row[7] ? atoi(row[7]) : 0;
        records[count].member_3 = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].member_3_name, row[9], sizeof(records[count].member_3_name) - 1);
        records[count].member_3_lev = row[10] ? atoi(row[10]) : 0;
        records[count].m_id = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
