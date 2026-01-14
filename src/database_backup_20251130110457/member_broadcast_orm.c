#include "member_broadcast_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberBroadcast_Add(DBConnectionManager* manager, const MemberBroadcast* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_broadcast "
        "(event_id, m_id, server_id, charac_no, charac_name, start_time, end_time) "
        "VALUES (%d, %d, %u, %d, '%s', '%s', '%s')",
        record->event_id, record->m_id, record->server_id, record->charac_no,
        record->charac_name, record->start_time, record->end_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBroadcast_Get(DBConnectionManager* manager, int event_id, int m_id,
                        unsigned char server_id, int charac_no, const char* start_time,
                        MemberBroadcast* record) {
    if (!manager || !start_time || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE event_id = %d AND m_id = %d AND server_id = %u "
        "AND charac_no = %d AND start_time = '%s'",
        event_id, m_id, server_id, charac_no, start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->event_id = atoi(values[0]);
        record->m_id = atoi(values[1]);
        record->server_id = (unsigned char)atoi(values[2]);
        record->charac_no = atoi(values[3]);
        strncpy(record->charac_name, values[4], sizeof(record->charac_name) - 1);
        record->charac_name[sizeof(record->charac_name) - 1] = '\0';
        strncpy(record->start_time, values[5], sizeof(record->start_time) - 1);
        record->start_time[sizeof(record->start_time) - 1] = '\0';
        strncpy(record->end_time, values[6], sizeof(record->end_time) - 1);
        record->end_time[sizeof(record->end_time) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberBroadcast_Update(DBConnectionManager* manager, const MemberBroadcast* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_broadcast SET charac_name = '%s', end_time = '%s' "
        "WHERE event_id = %d AND m_id = %d AND server_id = %u AND charac_no = %d "
        "AND start_time = '%s'",
        record->charac_name, record->end_time,
        record->event_id, record->m_id, record->server_id, record->charac_no,
        record->start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBroadcast_Delete(DBConnectionManager* manager, int event_id, int m_id,
                           unsigned char server_id, int charac_no, const char* start_time) {
    if (!manager || !start_time) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_broadcast WHERE event_id = %d AND m_id = %d "
        "AND server_id = %u AND charac_no = %d AND start_time = '%s'",
        event_id, m_id, server_id, charac_no, start_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBroadcast_Exists(DBConnectionManager* manager, int event_id, int m_id,
                           unsigned char server_id, int charac_no, const char* start_time) {
    if (!manager || !start_time) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_broadcast WHERE event_id = %d AND m_id = %d "
        "AND server_id = %u AND charac_no = %d AND start_time = '%s'",
        event_id, m_id, server_id, charac_no, start_time);

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

int MemberBroadcast_GetByEventId(DBConnectionManager* manager, int event_id,
                                  MemberBroadcast* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE event_id = %d LIMIT %d",
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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   MemberBroadcast* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE m_id = %d LIMIT %d",
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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetByServerId(DBConnectionManager* manager, unsigned char server_id,
                                   MemberBroadcast* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE server_id = %u LIMIT %d",
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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetByCharacName(DBConnectionManager* manager, const char* charac_name,
                                     MemberBroadcast* record) {
    if (!manager || !charac_name || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE charac_name = '%s'",
        charac_name);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->event_id = atoi(values[0]);
        record->m_id = atoi(values[1]);
        record->server_id = (unsigned char)atoi(values[2]);
        record->charac_no = atoi(values[3]);
        strncpy(record->charac_name, values[4], sizeof(record->charac_name) - 1);
        record->charac_name[sizeof(record->charac_name) - 1] = '\0';
        strncpy(record->start_time, values[5], sizeof(record->start_time) - 1);
        record->start_time[sizeof(record->start_time) - 1] = '\0';
        strncpy(record->end_time, values[6], sizeof(record->end_time) - 1);
        record->end_time[sizeof(record->end_time) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberBroadcast_GetByTimeRange(DBConnectionManager* manager, const char* start_begin,
                                    const char* start_end, MemberBroadcast* records,
                                    int max_records) {
    if (!manager || !start_begin || !start_end || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE start_time BETWEEN '%s' AND '%s' LIMIT %d",
        start_begin, start_end, max_records);

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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetActive(DBConnectionManager* manager, const char* current_time,
                               MemberBroadcast* records, int max_records) {
    if (!manager || !current_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast WHERE '%s' BETWEEN start_time AND end_time LIMIT %d",
        current_time, max_records);

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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetAll(DBConnectionManager* manager, MemberBroadcast* records,
                            int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time "
        "FROM member_broadcast LIMIT %d, %d",
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
        records[count].m_id = atoi(values[1]);
        records[count].server_id = (unsigned char)atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        strncpy(records[count].charac_name, values[4], sizeof(records[count].charac_name) - 1);
        records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        strncpy(records[count].start_time, values[5], sizeof(records[count].start_time) - 1);
        records[count].start_time[sizeof(records[count].start_time) - 1] = '\0';
        strncpy(records[count].end_time, values[6], sizeof(records[count].end_time) - 1);
        records[count].end_time[sizeof(records[count].end_time) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBroadcast_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_broadcast");

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

int MemberBroadcast_BulkAdd(DBConnectionManager* manager, const MemberBroadcast* records,
                             int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberBroadcast_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberBroadcast_BulkDeleteByEvent(DBConnectionManager* manager, const int* event_ids,
                                       int count) {
    if (!manager || !event_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_broadcast WHERE event_id = %d", event_ids[i]);

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

void MemberBroadcast_PrintInfo(const MemberBroadcast* record) {
    if (!record) {
        printf("MemberBroadcast is NULL\n");
        return;
    }

    printf("=== Member Broadcast Info ===\n");
    printf("  event_id:     %d\n", record->event_id);
    printf("  m_id:         %d\n", record->m_id);
    printf("  server_id:    %u\n", record->server_id);
    printf("  charac_no:    %d\n", record->charac_no);
    printf("  charac_name:  %s\n", record->charac_name);
    printf("  start_time:   %s\n", record->start_time);
    printf("  end_time:     %s\n", record->end_time);
    printf("=============================\n");
}
