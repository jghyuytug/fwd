#include "member_mouse_sms_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberMouseSms_Add(DBConnectionManager* manager, const MemberMouseSms* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_mouse_sms (m_id, occ_time, cnt) VALUES (%d, '%s', %u)",
        record->m_id, record->occ_time, record->cnt);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMouseSms_Get(DBConnectionManager* manager, int m_id, MemberMouseSms* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = atoi(values[0]);
        strncpy(record->occ_time, values[1], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
        record->cnt = (unsigned char)atoi(values[2]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberMouseSms_Update(DBConnectionManager* manager, const MemberMouseSms* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_mouse_sms SET occ_time = '%s', cnt = %u WHERE m_id = %d",
        record->occ_time, record->cnt, record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMouseSms_Delete(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_mouse_sms WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberMouseSms_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_mouse_sms WHERE m_id = %d",
        m_id);

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

int MemberMouseSms_GetByOccTimeRange(DBConnectionManager* manager, const char* start_time,
                                      const char* end_time, MemberMouseSms* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms "
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
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMouseSms_GetByCntRange(DBConnectionManager* manager, int min_cnt,
                                  int max_cnt, MemberMouseSms* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms "
        "WHERE cnt BETWEEN %d AND %d LIMIT %d",
        min_cnt, max_cnt, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMouseSms_GetRecent(DBConnectionManager* manager, MemberMouseSms* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms "
        "ORDER BY occ_time DESC LIMIT %d",
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
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMouseSms_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                  MemberMouseSms* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms "
        "WHERE occ_time < DATE_SUB(NOW(), INTERVAL %d DAY) "
        "ORDER BY occ_time ASC LIMIT %d",
        days_ago, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMouseSms_GetAll(DBConnectionManager* manager, MemberMouseSms* records,
                           int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, cnt FROM member_mouse_sms LIMIT %d, %d",
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
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].occ_time, values[1], sizeof(records[count].occ_time) - 1);
        records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberMouseSms_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_mouse_sms");

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

int MemberMouseSms_IncrementCnt(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_mouse_sms SET cnt = cnt + 1 WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ======================================== Bulk Operations ======================================== */

int MemberMouseSms_BulkAdd(DBConnectionManager* manager, const MemberMouseSms* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberMouseSms_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberMouseSms_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_mouse_sms WHERE m_id = %d", m_ids[i]);

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

void MemberMouseSms_PrintInfo(const MemberMouseSms* record) {
    if (!record) {
        printf("MemberMouseSms is NULL\n");
        return;
    }

    printf("=== Member Mouse SMS Info ===\n");
    printf("  m_id:      %d\n", record->m_id);
    printf("  occ_time:  %s\n", record->occ_time);
    printf("  cnt:       %u\n", record->cnt);
    printf("============================\n");
}
