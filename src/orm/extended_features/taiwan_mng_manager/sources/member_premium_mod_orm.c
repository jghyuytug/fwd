#include "member_premium_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPremiumMod_Add(DBConnectionManager* manager, const MemberPremiumMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_premium_mod (m_id, event_id_old, server_id_old, service_start_old, service_end_old, service_end_new, MNG_user_id, work_time, pre_type) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', '%s', %d)",
        record->m_id, record->event_id_old, record->server_id_old, record->service_start_old, record->service_end_old, record->service_end_new, record->MNG_user_id, record->work_time, record->pre_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumMod_Get(DBConnectionManager* manager, int id, MemberPremiumMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, event_id_old, server_id_old, service_start_old, service_end_old, service_end_new, MNG_user_id, work_time, pre_type FROM member_premium_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPremiumMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->event_id_old = row[2] ? atoi(row[2]) : 0;
    record->server_id_old = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->service_start_old, row[4], sizeof(record->service_start_old) - 1);
    if (row[5]) strncpy(record->service_end_old, row[5], sizeof(record->service_end_old) - 1);
    if (row[6]) strncpy(record->service_end_new, row[6], sizeof(record->service_end_new) - 1);
    if (row[7]) strncpy(record->MNG_user_id, row[7], sizeof(record->MNG_user_id) - 1);
    if (row[8]) strncpy(record->work_time, row[8], sizeof(record->work_time) - 1);
    record->pre_type = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumMod_GetAll(DBConnectionManager* manager, MemberPremiumMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, event_id_old, server_id_old, service_start_old, service_end_old, service_end_new, MNG_user_id, work_time, pre_type FROM member_premium_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPremiumMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].event_id_old = row[2] ? atoi(row[2]) : 0;
        records[count].server_id_old = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].service_start_old, row[4], sizeof(records[count].service_start_old) - 1);
        if (row[5]) strncpy(records[count].service_end_old, row[5], sizeof(records[count].service_end_old) - 1);
        if (row[6]) strncpy(records[count].service_end_new, row[6], sizeof(records[count].service_end_new) - 1);
        if (row[7]) strncpy(records[count].MNG_user_id, row[7], sizeof(records[count].MNG_user_id) - 1);
        if (row[8]) strncpy(records[count].work_time, row[8], sizeof(records[count].work_time) - 1);
        records[count].pre_type = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
