#include "member_safe_ensure_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberSafeEnsure_Add(DBConnectionManager* manager, const MemberSafeEnsure* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_safe_ensure (m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id) "
        "VALUES (%u, '%s', '%s', %d, %d, %d, '%s', '%s')",
        record->m_id, record->occ_time, record->mobile_no, record->service_flag,
        record->type1_flag, record->type2_flag, record->expire_time, record->settle_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsure_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_time, MemberSafeEnsure* record) {
    if (!manager || !occ_time || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure "
        "WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        strncpy(record->occ_time, values[1], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
        strncpy(record->mobile_no, values[2], sizeof(record->mobile_no) - 1);
        record->mobile_no[sizeof(record->mobile_no) - 1] = '\0';
        record->service_flag = (char)atoi(values[3]);
        record->type1_flag = (char)atoi(values[4]);
        record->type2_flag = (char)atoi(values[5]);
        strncpy(record->expire_time, values[6], sizeof(record->expire_time) - 1);
        record->expire_time[sizeof(record->expire_time) - 1] = '\0';
        strncpy(record->settle_id, values[7], sizeof(record->settle_id) - 1);
        record->settle_id[sizeof(record->settle_id) - 1] = '\0';
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberSafeEnsure_Update(DBConnectionManager* manager, const MemberSafeEnsure* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_safe_ensure SET mobile_no = '%s', service_flag = %d, type1_flag = %d, "
        "type2_flag = %d, expire_time = '%s', settle_id = '%s' "
        "WHERE m_id = %u AND occ_time = '%s'",
        record->mobile_no, record->service_flag, record->type1_flag, record->type2_flag,
        record->expire_time, record->settle_id, record->m_id, record->occ_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsure_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time) {
    if (!manager || !occ_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_safe_ensure WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsure_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time) {
    if (!manager || !occ_time) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_safe_ensure WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberSafeEnsure_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                    MemberSafeEnsure* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure WHERE m_id = %u "
        "ORDER BY occ_time DESC LIMIT %d",
        m_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetByMobileNo(DBConnectionManager* manager, const char* mobile_no,
                                    MemberSafeEnsure* records, int max_records) {
    if (!manager || !mobile_no || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure WHERE mobile_no = '%s' LIMIT %d",
        mobile_no, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetBySettleId(DBConnectionManager* manager, const char* settle_id,
                                    MemberSafeEnsure* records, int max_records) {
    if (!manager || !settle_id || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure WHERE settle_id = '%s' LIMIT %d",
        settle_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetActiveServices(DBConnectionManager* manager, MemberSafeEnsure* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure "
        "WHERE expire_time > NOW() "
        "LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetExpiredServices(DBConnectionManager* manager, MemberSafeEnsure* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure "
        "WHERE expire_time <= NOW() "
        "ORDER BY expire_time DESC "
        "LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetByServiceFlag(DBConnectionManager* manager, int service_flag,
                                       MemberSafeEnsure* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure WHERE service_flag = %d LIMIT %d",
        service_flag, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                     const char* end_time, MemberSafeEnsure* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure "
        "WHERE occ_time BETWEEN '%s' AND '%s' "
        "LIMIT %d",
        start_time, end_time, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetAll(DBConnectionManager* manager, MemberSafeEnsure* records,
                             int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, expire_time, settle_id "
        "FROM member_safe_ensure LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[2], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[3]);
        records[count].type1_flag = (char)atoi(values[4]);
        records[count].type2_flag = (char)atoi(values[5]);
        strncpy(records[count].expire_time, values[6], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[7], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsure_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_safe_ensure");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberSafeEnsure_BulkAdd(DBConnectionManager* manager, const MemberSafeEnsure* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberSafeEnsure_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberSafeEnsure_BulkDeleteByMemberIds(DBConnectionManager* manager, const unsigned int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_safe_ensure WHERE m_id = %u", m_ids[i]);

        DBQueryResult result;
        memset(&result, 0, sizeof(DBQueryResult));

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberSafeEnsure_PrintInfo(const MemberSafeEnsure* record) {
    if (!record) {
        printf("MemberSafeEnsure is NULL\n");
        return;
    }

    printf("=== Member Safe Ensure Info ===\n");
    printf("  m_id:         %u\n", record->m_id);
    printf("  occ_time:     %s\n", record->occ_time);
    printf("  mobile_no:    %s\n", record->mobile_no);
    printf("  service_flag: %d\n", record->service_flag);
    printf("  type1_flag:   %d\n", record->type1_flag);
    printf("  type2_flag:   %d\n", record->type2_flag);
    printf("  expire_time:  %s\n", record->expire_time);
    printf("  settle_id:    %s\n", record->settle_id);
    printf("================================\n");
}
