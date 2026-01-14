#include "member_mousepass_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberMousepass_Add(DBConnectionManager* manager, const MemberMousepass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_mousepass "
        "(m_id, mousepass, occ_time, fail_cnt, cancel_cnt, version_info, "
        "validity_time, reward_time, enable_flag) "
        "VALUES (%d, '%s', '%s', %u, %u, '%c', %d, %d, '%c')",
        record->m_id, record->mousepass, record->occ_time,
        record->fail_cnt, record->cancel_cnt, record->version_info,
        record->validity_time, record->reward_time, record->enable_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_Get(DBConnectionManager* manager, int m_id, MemberMousepass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, mousepass, occ_time, fail_cnt, cancel_cnt, version_info, "
        "validity_time, reward_time, enable_flag "
        "FROM member_mousepass WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberMousepass));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->mousepass, row[1], sizeof(record->mousepass) - 1);
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);
    record->fail_cnt = row[3] ? (unsigned char)atoi(row[3]) : 0;
    record->cancel_cnt = row[4] ? (unsigned short)atoi(row[4]) : 0;
    record->version_info = row[5] ? row[5][0] : '1';
    record->validity_time = row[6] ? atoi(row[6]) : 0;
    record->reward_time = row[7] ? atoi(row[7]) : 0;
    record->enable_flag = row[8] ? row[8][0] : '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_Update(DBConnectionManager* manager, const MemberMousepass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_mousepass SET "
        "mousepass = '%s', occ_time = '%s', fail_cnt = %u, cancel_cnt = %u, "
        "version_info = '%c', validity_time = %d, reward_time = %d, enable_flag = '%c' "
        "WHERE m_id = %d",
        record->mousepass, record->occ_time, record->fail_cnt, record->cancel_cnt,
        record->version_info, record->validity_time, record->reward_time,
        record->enable_flag, record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_mousepass WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_mousepass WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int MemberMousepass_GetByEnableFlag(DBConnectionManager* manager, char enable_flag,
                                     MemberMousepass* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, mousepass, occ_time, fail_cnt, cancel_cnt, version_info, "
        "validity_time, reward_time, enable_flag "
        "FROM member_mousepass WHERE enable_flag = '%c' ORDER BY m_id", enable_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepass));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].mousepass, row[1], sizeof(records[count].mousepass) - 1);
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        records[count].fail_cnt = row[3] ? (unsigned char)atoi(row[3]) : 0;
        records[count].cancel_cnt = row[4] ? (unsigned short)atoi(row[4]) : 0;
        records[count].version_info = row[5] ? row[5][0] : '1';
        records[count].validity_time = row[6] ? atoi(row[6]) : 0;
        records[count].reward_time = row[7] ? atoi(row[7]) : 0;
        records[count].enable_flag = row[8] ? row[8][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_UpdateFailCount(DBConnectionManager* manager, int m_id, unsigned char fail_cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_mousepass SET fail_cnt = %u WHERE m_id = %d",
        fail_cnt, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_ResetFailCount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_mousepass SET fail_cnt = 0 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepass_IncrementCancelCount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_mousepass SET cancel_cnt = cancel_cnt + 1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberMousepass_PrintInfo(const MemberMousepass* record) {
    if (!record) return;

    printf("Member Mouse Password Info:\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  mousepass: %s\n", record->mousepass);
    printf("  occ_time: %s\n", record->occ_time);
    printf("  fail_cnt: %u\n", record->fail_cnt);
    printf("  cancel_cnt: %u\n", record->cancel_cnt);
    printf("  version_info: %c\n", record->version_info);
    printf("  validity_time: %d\n", record->validity_time);
    printf("  reward_time: %d\n", record->reward_time);
    printf("  enable_flag: %c\n", record->enable_flag);
}
