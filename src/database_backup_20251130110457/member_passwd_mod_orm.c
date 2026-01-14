#include "member_passwd_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberPasswdMod_Add(DBConnectionManager* manager, const MemberPasswdMod* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_passwd_mod "
        "(m_id, first_time, last_time, cnt) "
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

int MemberPasswdMod_Get(DBConnectionManager* manager, int m_id, MemberPasswdMod* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->m_id = atoi(values[0]);
        strncpy(record->first_time, values[1], sizeof(record->first_time) - 1);
        strncpy(record->last_time, values[2], sizeof(record->last_time) - 1);
        record->cnt = (unsigned char)atoi(values[3]);
        record->first_time[sizeof(record->first_time) - 1] = '\0';
        record->last_time[sizeof(record->last_time) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

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

int MemberPasswdMod_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_passwd_mod WHERE m_id = %d", m_id);

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

int MemberPasswdMod_IncrementCount(DBConnectionManager* manager, int m_id, const char* current_time) {
    if (!manager || !current_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_passwd_mod SET cnt = cnt + 1, last_time = '%s' "
        "WHERE m_id = %d",
        current_time, m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPasswdMod_UpdateLastTime(DBConnectionManager* manager, int m_id, const char* last_time) {
    if (!manager || !last_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_passwd_mod SET last_time = '%s' WHERE m_id = %d",
        last_time, m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPasswdMod_GetByCountRange(DBConnectionManager* manager, int min_cnt, int max_cnt,
                                     MemberPasswdMod* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod WHERE cnt BETWEEN %d AND %d LIMIT %d",
        min_cnt, max_cnt, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].cnt = (unsigned char)atoi(values[3]);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPasswdMod_GetByFirstTimeRange(DBConnectionManager* manager, const char* start_time,
                                         const char* end_time, MemberPasswdMod* records,
                                         int max_count) {
    if (!manager || !start_time || !end_time || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod WHERE first_time BETWEEN '%s' AND '%s' LIMIT %d",
        start_time, end_time, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].cnt = (unsigned char)atoi(values[3]);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPasswdMod_GetByLastTimeRange(DBConnectionManager* manager, const char* start_time,
                                        const char* end_time, MemberPasswdMod* records,
                                        int max_count) {
    if (!manager || !start_time || !end_time || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod WHERE last_time BETWEEN '%s' AND '%s' LIMIT %d",
        start_time, end_time, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].cnt = (unsigned char)atoi(values[3]);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPasswdMod_GetRecentModified(DBConnectionManager* manager, MemberPasswdMod* records,
                                       int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod ORDER BY last_time DESC LIMIT %d",
        limit);

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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].cnt = (unsigned char)atoi(values[3]);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPasswdMod_GetAll(DBConnectionManager* manager, MemberPasswdMod* records,
                            int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, first_time, last_time, cnt "
        "FROM member_passwd_mod LIMIT %d, %d",
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
        strncpy(records[count].first_time, values[1], sizeof(records[count].first_time) - 1);
        strncpy(records[count].last_time, values[2], sizeof(records[count].last_time) - 1);
        records[count].cnt = (unsigned char)atoi(values[3]);
        records[count].first_time[sizeof(records[count].first_time) - 1] = '\0';
        records[count].last_time[sizeof(records[count].last_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPasswdMod_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_passwd_mod");

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

int MemberPasswdMod_BulkAdd(DBConnectionManager* manager, const MemberPasswdMod* records,
                             int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberPasswdMod_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberPasswdMod_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (MemberPasswdMod_Delete(manager, m_ids[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberPasswdMod_PrintInfo(const MemberPasswdMod* record) {
    if (!record) {
        printf("MemberPasswdMod is NULL\n");
        return;
    }

    printf("=== Member Password Modification ===\n");
    printf("  m_id:        %d\n", record->m_id);
    printf("  first_time:  %s\n", record->first_time);
    printf("  last_time:   %s\n", record->last_time);
    printf("  cnt:         %u\n", record->cnt);
    printf("====================================\n");
}
