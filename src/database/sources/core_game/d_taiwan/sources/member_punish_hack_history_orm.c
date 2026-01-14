#include "member_punish_hack_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberPunishHackHistory_Add(DBConnectionManager* manager, const MemberPunishHackHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_hack_history "
        "(m_id, occ_time, period, now_flag, auto_flag, reason) "
        "VALUES (%u, %u, %u, %d, %d, '%s')",
        record->m_id, record->occ_time, record->period,
        record->now_flag, record->auto_flag, record->reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_DeleteByMId(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_hack_history WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_DeleteByMIdAndTime(DBConnectionManager* manager,
                                                 unsigned int m_id,
                                                 unsigned int occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_hack_history WHERE m_id = %u AND occ_time = %u",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_GetByMId(DBConnectionManager* manager,
                                      unsigned int m_id,
                                      MemberPunishHackHistory* records,
                                      int max_count,
                                      int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason "
        "FROM member_punish_hack_history WHERE m_id = %u ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishHackHistory));
        records[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].period = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].now_flag = row[3] ? (signed char)atoi(row[3]) : 0;
        records[count].auto_flag = row[4] ? (signed char)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].reason, row[5], sizeof(records[count].reason) - 1);
            records[count].reason[sizeof(records[count].reason) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_GetByTimeRange(DBConnectionManager* manager,
                                            unsigned int start_time,
                                            unsigned int end_time,
                                            MemberPunishHackHistory* records,
                                            int max_count,
                                            int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason "
        "FROM member_punish_hack_history "
        "WHERE occ_time >= %u AND occ_time <= %u "
        "ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishHackHistory));
        records[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].period = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].now_flag = row[3] ? (signed char)atoi(row[3]) : 0;
        records[count].auto_flag = row[4] ? (signed char)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].reason, row[5], sizeof(records[count].reason) - 1);
            records[count].reason[sizeof(records[count].reason) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_GetActive(DBConnectionManager* manager,
                                       unsigned int m_id,
                                       MemberPunishHackHistory* records,
                                       int max_count,
                                       int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason "
        "FROM member_punish_hack_history "
        "WHERE m_id = %u AND now_flag = 1 "
        "ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishHackHistory));
        records[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].period = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].now_flag = row[3] ? (signed char)atoi(row[3]) : 0;
        records[count].auto_flag = row[4] ? (signed char)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].reason, row[5], sizeof(records[count].reason) - 1);
            records[count].reason[sizeof(records[count].reason) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_CountByMId(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_punish_hack_history WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPunishHackHistory_UpdateFlag(DBConnectionManager* manager,
                                        unsigned int m_id,
                                        unsigned int occ_time,
                                        signed char now_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_punish_hack_history SET now_flag = %d "
        "WHERE m_id = %u AND occ_time = %u",
        now_flag, m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void MemberPunishHackHistory_PrintInfo(const MemberPunishHackHistory* record) {
    if (!record) {
        printf("NULL record\n");
        return;
    }

    printf("Member Punish Hack History:\n");
    printf("  Member ID: %u\n", record->m_id);
    printf("  Occurrence Time: %u\n", record->occ_time);
    printf("  Period: %u\n", record->period);
    printf("  Now Flag: %d\n", record->now_flag);
    printf("  Auto Flag: %d\n", record->auto_flag);
    printf("  Reason: %s\n", record->reason);
}
