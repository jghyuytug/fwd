#include "member_handicap_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberHandicap_Add(DBConnectionManager* manager, const MemberHandicap* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_handicap (event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value) "
        "VALUES (%d, %u, %d, %d, '%s', '%s', %d)",
        record->event_id, record->cap_type, record->server_id, record->m_id,
        record->start_time, record->end_time, record->handicap_value);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberHandicap_Get(DBConnectionManager* manager, int event_id, int cap_type,
                        int server_id, int m_id, const char* start_time, MemberHandicap* record) {
    if (!manager || !start_time || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap "
        "WHERE event_id = %d AND cap_type = %d AND server_id = %d AND m_id = %d AND start_time = '%s'",
        event_id, cap_type, server_id, m_id, start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->event_id = atoi(values[0]);
        record->cap_type = (unsigned char)atoi(values[1]);
        record->server_id = (char)atoi(values[2]);
        record->m_id = atoi(values[3]);
        strncpy(record->start_time, values[4], sizeof(record->start_time) - 1);
        record->start_time[sizeof(record->start_time) - 1] = '\0';
        strncpy(record->end_time, values[5], sizeof(record->end_time) - 1);
        record->end_time[sizeof(record->end_time) - 1] = '\0';
        record->handicap_value = atoi(values[6]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberHandicap_Update(DBConnectionManager* manager, const MemberHandicap* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_handicap SET end_time = '%s', handicap_value = %d "
        "WHERE event_id = %d AND cap_type = %u AND server_id = %d AND m_id = %d AND start_time = '%s'",
        record->end_time, record->handicap_value,
        record->event_id, record->cap_type, record->server_id, record->m_id, record->start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberHandicap_Delete(DBConnectionManager* manager, int event_id, int cap_type,
                           int server_id, int m_id, const char* start_time) {
    if (!manager || !start_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_handicap "
        "WHERE event_id = %d AND cap_type = %d AND server_id = %d AND m_id = %d AND start_time = '%s'",
        event_id, cap_type, server_id, m_id, start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberHandicap_Exists(DBConnectionManager* manager, int event_id, int cap_type,
                           int server_id, int m_id, const char* start_time) {
    if (!manager || !start_time) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_handicap "
        "WHERE event_id = %d AND cap_type = %d AND server_id = %d AND m_id = %d AND start_time = '%s'",
        event_id, cap_type, server_id, m_id, start_time);

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

int MemberHandicap_GetByEventId(DBConnectionManager* manager, int event_id,
                                  MemberHandicap* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap WHERE event_id = %d LIMIT %d",
        event_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   MemberHandicap* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap WHERE m_id = %d LIMIT %d",
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
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetByServerId(DBConnectionManager* manager, int server_id,
                                   MemberHandicap* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap WHERE server_id = %d LIMIT %d",
        server_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetByCapType(DBConnectionManager* manager, int cap_type,
                                  MemberHandicap* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap WHERE cap_type = %d LIMIT %d",
        cap_type, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetActiveHandicaps(DBConnectionManager* manager, MemberHandicap* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap "
        "WHERE NOW() BETWEEN start_time AND end_time "
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
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                    const char* end_time, MemberHandicap* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap "
        "WHERE start_time BETWEEN '%s' AND '%s' "
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
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetAll(DBConnectionManager* manager, MemberHandicap* records,
                           int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value "
        "FROM member_handicap LIMIT %d, %d",
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
        records[count].event_id = atoi(values[0]);
        records[count].cap_type = (unsigned char)atoi(values[1]);
        records[count].server_id = (char)atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        strncpy(records[count].start_time, values[4], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[5], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        records[count].handicap_value = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberHandicap_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_handicap");

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

int MemberHandicap_BulkAdd(DBConnectionManager* manager, const MemberHandicap* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberHandicap_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberHandicap_BulkDeleteByEventIds(DBConnectionManager* manager, const int* event_ids, int count) {
    if (!manager || !event_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_handicap WHERE event_id = %d", event_ids[i]);

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

void MemberHandicap_PrintInfo(const MemberHandicap* record) {
    if (!record) {
        printf("MemberHandicap is NULL\n");
        return;
    }

    printf("=== Member Handicap Info ===\n");
    printf("  event_id:       %d\n", record->event_id);
    printf("  cap_type:       %u\n", record->cap_type);
    printf("  server_id:      %d\n", record->server_id);
    printf("  m_id:           %d\n", record->m_id);
    printf("  start_time:     %s\n", record->start_time);
    printf("  end_time:       %s\n", record->end_time);
    printf("  handicap_value: %d\n", record->handicap_value);
    printf("============================\n");
}
