#include "member_safe_ensure_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberSafeEnsureHistory_Add(DBConnectionManager* manager, const MemberSafeEnsureHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_safe_ensure_history (mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id) "
        "VALUES (%d, '%s', %d, '%s', '%s', %d, %d, %d, '%s', '%s')",
        record->mod_flag, record->mod_time, record->m_id, record->occ_time, record->mobile_no, record->service_flag, record->type1_flag, record->type2_flag, record->expire_time, record->settle_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsureHistory_GetAll(DBConnectionManager* manager, MemberSafeEnsureHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id FROM member_safe_ensure_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberSafeEnsureHistory));
        records[count].mod_flag = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].mod_time, row[1], sizeof(records[count].mod_time) - 1);
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        if (row[4]) strncpy(records[count].mobile_no, row[4], sizeof(records[count].mobile_no) - 1);
        records[count].service_flag = row[5] ? atoi(row[5]) : 0;
        records[count].type1_flag = row[6] ? atoi(row[6]) : 0;
        records[count].type2_flag = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].expire_time, row[8], sizeof(records[count].expire_time) - 1);
        if (row[9]) strncpy(records[count].settle_id, row[9], sizeof(records[count].settle_id) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
