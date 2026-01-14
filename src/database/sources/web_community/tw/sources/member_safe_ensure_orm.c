#include "member_safe_ensure_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberSafeEnsure_Add(DBConnectionManager* manager, const MemberSafeEnsure* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_safe_ensure (m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id) "
        "VALUES (%d, '%s', '%s', %d, %d, %d, '%s', '%s')",
        record->m_id, record->occ_time, record->mobile_no, record->service_flag, record->type1_flag, record->type2_flag, record->expire_time, record->settle_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsure_GetAll(DBConnectionManager* manager, MemberSafeEnsure* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id FROM member_safe_ensure");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberSafeEnsure));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        if (row[2]) strncpy(records[count].mobile_no, row[2], sizeof(records[count].mobile_no) - 1);
        records[count].service_flag = row[3] ? atoi(row[3]) : 0;
        records[count].type1_flag = row[4] ? atoi(row[4]) : 0;
        records[count].type2_flag = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].expire_time, row[6], sizeof(records[count].expire_time) - 1);
        if (row[7]) strncpy(records[count].settle_id, row[7], sizeof(records[count].settle_id) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
