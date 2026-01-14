#include "member_mousepass_history_201506_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberMousepassHistory201506_Add(DBConnectionManager* manager, const MemberMousepassHistory201506* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_mousepass_history_201506 "
        "(occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) "
        "VALUES ('%s', %d, '%s', %u, '%s', '%s')",
        record->occ_time, record->m_id, record->pre_mousepass,
        record->modify_type, record->ip_info, record->port_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory201506_Get(DBConnectionManager* manager,
                                      const char* occ_time, int m_id,
                                      MemberMousepassHistory201506* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !occ_time || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info "
        "FROM member_mousepass_history_201506 WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberMousepassHistory201506));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->pre_mousepass, row[2], sizeof(record->pre_mousepass) - 1);
    record->modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
    if (row[4]) strncpy(record->ip_info, row[4], sizeof(record->ip_info) - 1);
    if (row[5]) strncpy(record->port_info, row[5], sizeof(record->port_info) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory201506_Delete(DBConnectionManager* manager,
                                         const char* occ_time, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_mousepass_history_201506 WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory201506_Exists(DBConnectionManager* manager,
                                         const char* occ_time, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !occ_time) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_mousepass_history_201506 "
        "WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

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

int MemberMousepassHistory201506_GetByMemberId(DBConnectionManager* manager, int m_id,
                                                MemberMousepassHistory201506* records,
                                                int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info "
        "FROM member_mousepass_history_201506 WHERE m_id = %d ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory201506));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].ip_info, row[4], sizeof(records[count].ip_info) - 1);
        if (row[5]) strncpy(records[count].port_info, row[5], sizeof(records[count].port_info) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory201506_GetByIP(DBConnectionManager* manager, const char* ip_info,
                                          MemberMousepassHistory201506* records,
                                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !ip_info || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info "
        "FROM member_mousepass_history_201506 WHERE ip_info = '%s' ORDER BY occ_time DESC",
        ip_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory201506));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].ip_info, row[4], sizeof(records[count].ip_info) - 1);
        if (row[5]) strncpy(records[count].port_info, row[5], sizeof(records[count].port_info) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberMousepassHistory201506_GetByTimeRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 MemberMousepassHistory201506* records,
                                                 int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info "
        "FROM member_mousepass_history_201506 "
        "WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberMousepassHistory201506));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].pre_mousepass, row[2], sizeof(records[count].pre_mousepass) - 1);
        records[count].modify_type = row[3] ? (unsigned char)atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].ip_info, row[4], sizeof(records[count].ip_info) - 1);
        if (row[5]) strncpy(records[count].port_info, row[5], sizeof(records[count].port_info) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void MemberMousepassHistory201506_PrintInfo(const MemberMousepassHistory201506* record) {
    if (!record) return;

    printf("Mouse Password History 201506:\n");
    printf("  occ_time: %s\n", record->occ_time);
    printf("  m_id: %d\n", record->m_id);
    printf("  pre_mousepass: %s\n", record->pre_mousepass);
    printf("  modify_type: %u\n", record->modify_type);
    printf("  ip_info: %s\n", record->ip_info);
    printf("  port_info: %s\n", record->port_info);
}
