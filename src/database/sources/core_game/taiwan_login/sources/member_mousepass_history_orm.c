#include "member_mousepass_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberMousepassHistory_Add(DBConnectionManager* manager, const MemberMousepassHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_mousepass_history "
        "(occ_time, m_id, pre_mousepass, modify_type) "
        "VALUES ('%s', %d, '%s', %u)",
        record->occ_time, record->m_id, record->pre_mousepass, record->modify_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                          MemberMousepassHistory* records,
                                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type "
        "FROM member_mousepass_history WHERE m_id = %d ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory_GetByTimeRange(DBConnectionManager* manager,
                                           const char* start_time, const char* end_time,
                                           MemberMousepassHistory* records,
                                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type "
        "FROM member_mousepass_history "
        "WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory_GetByModifyType(DBConnectionManager* manager,
                                            unsigned char modify_type,
                                            MemberMousepassHistory* records,
                                            int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type "
        "FROM member_mousepass_history WHERE modify_type = %u ORDER BY occ_time DESC",
        modify_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory_DeleteOldRecords(DBConnectionManager* manager, const char* before_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !before_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_mousepass_history WHERE occ_time < '%s'", before_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberMousepassHistory_PrintInfo(const MemberMousepassHistory* record) {
    if (!record) return;

    printf("Mouse Password History:\n");
    printf("  occ_time: %s\n", record->occ_time);
    printf("  m_id: %d\n", record->m_id);
    printf("  pre_mousepass: %s\n", record->pre_mousepass);
    printf("  modify_type: %u\n", record->modify_type);
}
