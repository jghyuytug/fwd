#include "event_quest_party_member_web_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventQuestPartyMemberWeb_Add(DBConnectionManager* manager, const EventQuestPartyMemberWeb* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_quest_party_member_web "
        "(m_id, server_id, charac_no, quest_no, occ_time, send_charac_no) "
        "VALUES (%d, %u, %d, %d, '%s', %d)",
        member->m_id, member->server_id, member->charac_no, member->quest_no,
        member->occ_time, member->send_charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_Get(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                  int charac_no, int quest_no, EventQuestPartyMemberWeb* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, quest_no, occ_time, send_charac_no "
        "FROM event_quest_party_member_web "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d AND quest_no = %d",
        m_id, server_id, charac_no, quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(EventQuestPartyMemberWeb));
    member->m_id = row[0] ? atoi(row[0]) : 0;
    member->server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    member->charac_no = row[2] ? atoi(row[2]) : 0;
    member->quest_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(member->occ_time, row[4], sizeof(member->occ_time) - 1);
    member->send_charac_no = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_Update(DBConnectionManager* manager, const EventQuestPartyMemberWeb* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_quest_party_member_web "
        "SET occ_time = '%s', send_charac_no = %d "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d AND quest_no = %d",
        member->occ_time, member->send_charac_no,
        member->m_id, member->server_id, member->charac_no, member->quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_Delete(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                     int charac_no, int quest_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_quest_party_member_web "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d AND quest_no = %d",
        m_id, server_id, charac_no, quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_Exists(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                     int charac_no, int quest_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_quest_party_member_web "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d AND quest_no = %d",
        m_id, server_id, charac_no, quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0 ? 1 : 0;
}

int EventQuestPartyMemberWeb_GetByMember(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                          int charac_no, EventQuestPartyMemberWeb* members,
                                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !members || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, quest_no, occ_time, send_charac_no "
        "FROM event_quest_party_member_web "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d "
        "ORDER BY quest_no",
        m_id, server_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(EventQuestPartyMemberWeb));
        members[count].m_id = row[0] ? atoi(row[0]) : 0;
        members[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        members[count].charac_no = row[2] ? atoi(row[2]) : 0;
        members[count].quest_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(members[count].occ_time, row[4], sizeof(members[count].occ_time) - 1);
        members[count].send_charac_no = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_GetByQuest(DBConnectionManager* manager, int quest_no,
                                         EventQuestPartyMemberWeb* members,
                                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !members || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, quest_no, occ_time, send_charac_no "
        "FROM event_quest_party_member_web "
        "WHERE quest_no = %d "
        "ORDER BY m_id, server_id, charac_no",
        quest_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(EventQuestPartyMemberWeb));
        members[count].m_id = row[0] ? atoi(row[0]) : 0;
        members[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        members[count].charac_no = row[2] ? atoi(row[2]) : 0;
        members[count].quest_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(members[count].occ_time, row[4], sizeof(members[count].occ_time) - 1);
        members[count].send_charac_no = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventQuestPartyMemberWeb_DeleteByMember(DBConnectionManager* manager, int m_id,
                                             unsigned char server_id, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_quest_party_member_web "
        "WHERE m_id = %d AND server_id = %u AND charac_no = %d",
        m_id, server_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventQuestPartyMemberWeb_PrintInfo(const EventQuestPartyMemberWeb* member) {
    if (!member) {
        printf("EventQuestPartyMemberWeb: NULL\n");
        return;
    }

    printf("EventQuestPartyMemberWeb:\n");
    printf("  m_id: %d\n", member->m_id);
    printf("  server_id: %u\n", member->server_id);
    printf("  charac_no: %d\n", member->charac_no);
    printf("  quest_no: %d\n", member->quest_no);
    printf("  occ_time: %s\n", member->occ_time);
    printf("  send_charac_no: %d\n", member->send_charac_no);
}
