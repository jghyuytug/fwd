#include "member_premium_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberPremiumOld_Add(DBConnectionManager* manager, const MemberPremiumOld* premium) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !premium) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_premium_old (event_id, pre_type, m_id, service_start, service_end, server_id) "
        "VALUES (%d, %u, %d, '%s', '%s', %u)",
        premium->event_id, premium->pre_type, premium->m_id,
        premium->service_start, premium->service_end, premium->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumOld_Get(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                          unsigned char server_id, int m_id, const char* service_start,
                          MemberPremiumOld* premium) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !premium || !service_start) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_old "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(premium, 0, sizeof(MemberPremiumOld));
    premium->event_id = row[0] ? atoi(row[0]) : 0;
    premium->pre_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
    premium->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(premium->service_start, row[3], sizeof(premium->service_start) - 1);
    if (row[4]) strncpy(premium->service_end, row[4], sizeof(premium->service_end) - 1);
    premium->server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumOld_Update(DBConnectionManager* manager, const MemberPremiumOld* premium) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !premium) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_premium_old SET service_end = '%s' "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        premium->service_end, premium->event_id, premium->pre_type,
        premium->server_id, premium->m_id, premium->service_start);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumOld_Delete(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                             unsigned char server_id, int m_id, const char* service_start) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !service_start) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_premium_old "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumOld_Exists(DBConnectionManager* manager, int event_id, unsigned char pre_type,
                             unsigned char server_id, int m_id, const char* service_start) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !service_start) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM member_premium_old "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s' LIMIT 1",
        event_id, pre_type, server_id, m_id, service_start);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int MemberPremiumOld_GetByMember(DBConnectionManager* manager, int m_id,
                                  MemberPremiumOld* premiums, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !premiums || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_old WHERE m_id = %d ORDER BY service_start", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&premiums[count], 0, sizeof(MemberPremiumOld));
        premiums[count].event_id = row[0] ? atoi(row[0]) : 0;
        premiums[count].pre_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        premiums[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(premiums[count].service_start, row[3], sizeof(premiums[count].service_start) - 1);
        if (row[4]) strncpy(premiums[count].service_end, row[4], sizeof(premiums[count].service_end) - 1);
        premiums[count].server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumOld_GetByEvent(DBConnectionManager* manager, int event_id,
                                 MemberPremiumOld* premiums, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !premiums || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_old WHERE event_id = %d ORDER BY m_id", event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&premiums[count], 0, sizeof(MemberPremiumOld));
        premiums[count].event_id = row[0] ? atoi(row[0]) : 0;
        premiums[count].pre_type = row[1] ? (unsigned char)atoi(row[1]) : 0;
        premiums[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(premiums[count].service_start, row[3], sizeof(premiums[count].service_start) - 1);
        if (row[4]) strncpy(premiums[count].service_end, row[4], sizeof(premiums[count].service_end) - 1);
        premiums[count].server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void MemberPremiumOld_PrintInfo(const MemberPremiumOld* premium) {
    if (!premium) return;

    printf("MemberPremiumOld {\n");
    printf("  event_id: %d\n", premium->event_id);
    printf("  pre_type: %u\n", premium->pre_type);
    printf("  m_id: %d\n", premium->m_id);
    printf("  service_start: %s\n", premium->service_start);
    printf("  service_end: %s\n", premium->service_end);
    printf("  server_id: %u\n", premium->server_id);
    printf("}\n");
}
