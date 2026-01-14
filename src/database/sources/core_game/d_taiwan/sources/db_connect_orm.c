/**
 * member_passwd_mod_orm.c
 *
 * ORM implementation for member_passwd_mod table
 * 密码修改历史追踪表 - 实现会员密码修改记录的数据库操作
 *
 * Table: member_passwd_mod (4 fields)
 * Primary Key: m_id
 *
 * Generated: 2025-11-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_passwd_mod_orm.h"

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 4

/**
 * Add a new member_passwd_mod record (idempotent with REPLACE INTO)
 */
int MemberPasswdMod_Add(DBConnectionManager* manager, const MemberPasswdMod* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_passwd_mod (m_id, first_time, last_time, cnt) "
        "VALUES (%d, '%s', '%s', %u)",
        record->m_id, record->first_time, record->last_time, record->cnt);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Get a member_passwd_mod record by m_id
 */
int MemberPasswdMod_Get(DBConnectionManager* manager, int m_id, MemberPasswdMod* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = atoi(values[0]);
        strncpy(record->first_time, values[1], sizeof(record->first_time) - 1);
        record->first_time[sizeof(record->first_time) - 1] = '\0';
        strncpy(record->last_time, values[2], sizeof(record->last_time) - 1);
        record->last_time[sizeof(record->last_time) - 1] = '\0';
        record->cnt = (unsigned char)atoi(values[3]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * Update an existing member_passwd_mod record
 */
int MemberPasswdMod_Update(DBConnectionManager* manager, const MemberPasswdMod* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_passwd_mod SET first_time = '%s', last_time = '%s', cnt = %u "
        "WHERE m_id = %d",
        record->first_time, record->last_time, record->cnt, record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Delete a member_passwd_mod record by m_id (idempotent)
 */
int MemberPasswdMod_Delete(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_passwd_mod WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Check if a member_passwd_mod record exists
 */
int MemberPasswdMod_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_passwd_mod WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int exists = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/**
 * Atomically increment cnt and update last_time to NOW()
 */
int MemberPasswdMod_IncrementCnt(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_passwd_mod SET cnt = cnt + 1, last_time = NOW() WHERE m_id = %d",
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

/**
 * Get records by first_time range
 */
int MemberPasswdMod_GetByFirstTimeRange(DBConnectionManager* manager,
                                         const char* start_time, const char* end_time,
                                         MemberPasswdMod* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE first_time BETWEEN '%s' AND '%s' ORDER BY first_time DESC LIMIT %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by last_time range
 */
int MemberPasswdMod_GetByLastTimeRange(DBConnectionManager* manager,
                                        const char* start_time, const char* end_time,
                                        MemberPasswdMod* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE last_time BETWEEN '%s' AND '%s' ORDER BY last_time DESC LIMIT %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by cnt range
 */
int MemberPasswdMod_GetByCntRange(DBConnectionManager* manager,
                                   int min_cnt, int max_cnt,
                                   MemberPasswdMod* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE cnt BETWEEN %d AND %d ORDER BY cnt DESC LIMIT %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get frequent modifiers (cnt >= threshold)
 */
int MemberPasswdMod_GetFrequentModifiers(DBConnectionManager* manager,
                                          int threshold,
                                          MemberPasswdMod* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE cnt >= %d ORDER BY cnt DESC LIMIT %d",
        threshold, max_records);

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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get recent modifiers (sorted by last_time DESC)
 */
int MemberPasswdMod_GetRecentModifiers(DBConnectionManager* manager,
                                        MemberPasswdMod* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "ORDER BY last_time DESC LIMIT %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Update last_time to NOW()
 */
int MemberPasswdMod_UpdateLastTime(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_passwd_mod SET last_time = NOW() WHERE m_id = %d",
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

/**
 * Get users who have never modified their password (cnt = 0)
 */
int MemberPasswdMod_GetNeverModified(DBConnectionManager* manager,
                                      MemberPasswdMod* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE cnt = 0 LIMIT %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get users with cnt >= min_cnt
 */
int MemberPasswdMod_GetByMinCnt(DBConnectionManager* manager,
                                 int min_cnt,
                                 MemberPasswdMod* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt FROM member_passwd_mod "
        "WHERE cnt >= %d ORDER BY cnt DESC LIMIT %d",
        min_cnt, max_records);

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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        records[count].cnt = (unsigned char)atoi(values[3]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Bulk add member_passwd_mod records
 */
int MemberPasswdMod_BulkAdd(DBConnectionManager* manager,
                             const MemberPasswdMod* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int i;
    int success_count = 0;
    for (i = 0; i < count; i++) {
        if (MemberPasswdMod_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

/**
 * Bulk delete member_passwd_mod records
 */
int MemberPasswdMod_BulkDelete(DBConnectionManager* manager,
                                const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        MemberPasswdMod_Delete(manager, m_ids[i]);
    }

    return 0;
}

/**
 * Print member_passwd_mod record information
 */
void MemberPasswdMod_PrintInfo(const MemberPasswdMod* record) {
    if (!record) {
        printf("MemberPasswdMod: NULL\n");
        return;
    }

    printf("=== MemberPasswdMod Record ===\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  first_time: %s\n", record->first_time);
    printf("  last_time: %s\n", record->last_time);
    printf("  cnt: %u\n", record->cnt);
    printf("==============================\n");
}
