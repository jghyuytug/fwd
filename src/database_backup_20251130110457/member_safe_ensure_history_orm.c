#include "member_safe_ensure_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 12

/* ======================================== CRUD Operations ======================================== */

int MemberSafeEnsureHistory_Add(DBConnectionManager* manager, const MemberSafeEnsureHistory* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_safe_ensure_history (mod_flag, mod_time, m_id, occ_time, mobile_no, "
        "service_flag, type1_flag, type2_flag, expire_time, settle_id) "
        "VALUES (%d, '%s', %u, '%s', '%s', %d, %d, %d, '%s', '%s')",
        record->mod_flag, record->mod_time, record->m_id, record->occ_time, record->mobile_no,
        record->service_flag, record->type1_flag, record->type2_flag, record->expire_time, record->settle_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsureHistory_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                  const char* mod_time, MemberSafeEnsureHistory* record) {
    if (!manager || !occ_time || !mod_time || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history "
        "WHERE m_id = %u AND occ_time = '%s' AND mod_time = '%s'",
        m_id, occ_time, mod_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->mod_flag = (char)atoi(values[0]);
        strncpy(record->mod_time, values[1], sizeof(record->mod_time) - 1);
        record->mod_time[sizeof(record->mod_time) - 1] = '\0';
        record->m_id = (unsigned int)atoi(values[2]);
        strncpy(record->occ_time, values[3], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
        strncpy(record->mobile_no, values[4], sizeof(record->mobile_no) - 1);
        record->mobile_no[sizeof(record->mobile_no) - 1] = '\0';
        record->service_flag = (char)atoi(values[5]);
        record->type1_flag = (char)atoi(values[6]);
        record->type2_flag = (char)atoi(values[7]);
        strncpy(record->expire_time, values[8], sizeof(record->expire_time) - 1);
        record->expire_time[sizeof(record->expire_time) - 1] = '\0';
        strncpy(record->settle_id, values[9], sizeof(record->settle_id) - 1);
        record->settle_id[sizeof(record->settle_id) - 1] = '\0';
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberSafeEnsureHistory_Update(DBConnectionManager* manager, const MemberSafeEnsureHistory* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_safe_ensure_history SET mod_flag = %d, mobile_no = '%s', service_flag = %d, "
        "type1_flag = %d, type2_flag = %d, expire_time = '%s', settle_id = '%s' "
        "WHERE m_id = %u AND occ_time = '%s' AND mod_time = '%s'",
        record->mod_flag, record->mobile_no, record->service_flag, record->type1_flag,
        record->type2_flag, record->expire_time, record->settle_id,
        record->m_id, record->occ_time, record->mod_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsureHistory_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                     const char* mod_time) {
    if (!manager || !occ_time || !mod_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_safe_ensure_history WHERE m_id = %u AND occ_time = '%s' AND mod_time = '%s'",
        m_id, occ_time, mod_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSafeEnsureHistory_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                     const char* mod_time) {
    if (!manager || !occ_time || !mod_time) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_safe_ensure_history "
        "WHERE m_id = %u AND occ_time = '%s' AND mod_time = '%s'",
        m_id, occ_time, mod_time);

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

int MemberSafeEnsureHistory_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                            MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history WHERE m_id = %u "
        "ORDER BY mod_time DESC LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetByMobileNo(DBConnectionManager* manager, const char* mobile_no,
                                            MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !mobile_no || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history WHERE mobile_no = '%s' LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetBySettleId(DBConnectionManager* manager, const char* settle_id,
                                            MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !settle_id || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history WHERE settle_id = '%s' LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetByModFlag(DBConnectionManager* manager, int mod_flag,
                                           MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history WHERE mod_flag = %d LIMIT %d",
        mod_flag, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetByModTimeRange(DBConnectionManager* manager, const char* start_time,
                                                const char* end_time, MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history "
        "WHERE mod_time BETWEEN '%s' AND '%s' LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetByOccTimeRange(DBConnectionManager* manager, const char* start_time,
                                                const char* end_time, MemberSafeEnsureHistory* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history "
        "WHERE occ_time BETWEEN '%s' AND '%s' LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetRecentModifications(DBConnectionManager* manager, MemberSafeEnsureHistory* records,
                                                     int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history "
        "ORDER BY mod_time DESC LIMIT %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetAll(DBConnectionManager* manager, MemberSafeEnsureHistory* records,
                                     int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT mod_flag, mod_time, m_id, occ_time, mobile_no, service_flag, type1_flag, type2_flag, "
        "expire_time, settle_id FROM member_safe_ensure_history LIMIT %d, %d",
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
        records[count].mod_flag = (char)atoi(values[0]);
        strncpy(records[count].mod_time, values[1], sizeof(records[count].mod_time) - 1);
        records[count].mod_time[sizeof(records[count].mod_time) - 1] = '\0';
        records[count].m_id = (unsigned int)atoi(values[2]);
        strncpy(records[count].occ_time, values[3], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        strncpy(records[count].mobile_no, values[4], sizeof(records[count].mobile_no) - 1);
        records[count].mobile_no[sizeof(records[count].mobile_no) - 1] = '\0';
        records[count].service_flag = (char)atoi(values[5]);
        records[count].type1_flag = (char)atoi(values[6]);
        records[count].type2_flag = (char)atoi(values[7]);
        strncpy(records[count].expire_time, values[8], sizeof(records[count].expire_time) - 1);
        records[count].expire_time[sizeof(records[count].expire_time) - 1] = '\0';
        strncpy(records[count].settle_id, values[9], sizeof(records[count].settle_id) - 1);
        records[count].settle_id[sizeof(records[count].settle_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSafeEnsureHistory_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_safe_ensure_history");

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

int MemberSafeEnsureHistory_BulkAdd(DBConnectionManager* manager, const MemberSafeEnsureHistory* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberSafeEnsureHistory_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberSafeEnsureHistory_BulkDeleteByMemberIds(DBConnectionManager* manager, const unsigned int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_safe_ensure_history WHERE m_id = %u", m_ids[i]);

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

void MemberSafeEnsureHistory_PrintInfo(const MemberSafeEnsureHistory* record) {
    if (!record) {
        printf("MemberSafeEnsureHistory is NULL\n");
        return;
    }

    printf("=== Member Safe Ensure History Info ===\n");
    printf("  mod_flag:     %d\n", record->mod_flag);
    printf("  mod_time:     %s\n", record->mod_time);
    printf("  m_id:         %u\n", record->m_id);
    printf("  occ_time:     %s\n", record->occ_time);
    printf("  mobile_no:    %s\n", record->mobile_no);
    printf("  service_flag: %d\n", record->service_flag);
    printf("  type1_flag:   %d\n", record->type1_flag);
    printf("  type2_flag:   %d\n", record->type2_flag);
    printf("  expire_time:  %s\n", record->expire_time);
    printf("  settle_id:    %s\n", record->settle_id);
    printf("========================================\n");
}
