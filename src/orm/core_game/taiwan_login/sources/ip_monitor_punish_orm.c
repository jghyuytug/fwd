#include "ip_monitor_punish_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int IpMonitorPunish_Add(DBConnectionManager* manager, const IpMonitorPunish* punish) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !punish) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ip_monitor_punish (ip, type, m_id_cnt, start_time, end_time) "
        "VALUES ('%s', %d, %u, '%s', '%s')",
        punish->ip, punish->type, punish->m_id_cnt,
        punish->start_time[0] ? punish->start_time : "0000-00-00 00:00:00",
        punish->end_time[0] ? punish->end_time : "0000-00-00 00:00:00");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_Get(DBConnectionManager* manager, const char* ip, signed char type, IpMonitorPunish* punish) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !ip || !punish) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, type, m_id_cnt, start_time, end_time "
        "FROM ip_monitor_punish WHERE ip = '%s' AND type = %d",
        ip, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(punish, 0, sizeof(IpMonitorPunish));
    if (row[0]) strncpy(punish->ip, row[0], sizeof(punish->ip) - 1);
    punish->type = row[1] ? (signed char)atoi(row[1]) : 0;
    punish->m_id_cnt = row[2] ? (unsigned short)atoi(row[2]) : 0;
    if (row[3]) strncpy(punish->start_time, row[3], sizeof(punish->start_time) - 1);
    if (row[4]) strncpy(punish->end_time, row[4], sizeof(punish->end_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_Update(DBConnectionManager* manager, const IpMonitorPunish* punish) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !punish) return -1;

    snprintf(query, sizeof(query),
        "UPDATE ip_monitor_punish SET m_id_cnt = %u, start_time = '%s', end_time = '%s' "
        "WHERE ip = '%s' AND type = %d",
        punish->m_id_cnt,
        punish->start_time[0] ? punish->start_time : "0000-00-00 00:00:00",
        punish->end_time[0] ? punish->end_time : "0000-00-00 00:00:00",
        punish->ip, punish->type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_Delete(DBConnectionManager* manager, const char* ip, signed char type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !ip) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM ip_monitor_punish WHERE ip = '%s' AND type = %d", ip, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_Exists(DBConnectionManager* manager, const char* ip, signed char type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !ip) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_monitor_punish WHERE ip = '%s' AND type = %d", ip, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        int count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_GetAll(DBConnectionManager* manager,
                            IpMonitorPunish* punishes,
                            int max_count,
                            int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !punishes || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, type, m_id_cnt, start_time, end_time "
        "FROM ip_monitor_punish ORDER BY ip, type");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&punishes[count], 0, sizeof(IpMonitorPunish));
        if (row[0]) strncpy(punishes[count].ip, row[0], sizeof(punishes[count].ip) - 1);
        punishes[count].type = row[1] ? (signed char)atoi(row[1]) : 0;
        punishes[count].m_id_cnt = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(punishes[count].start_time, row[3], sizeof(punishes[count].start_time) - 1);
        if (row[4]) strncpy(punishes[count].end_time, row[4], sizeof(punishes[count].end_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_GetByIp(DBConnectionManager* manager,
                             const char* ip,
                             IpMonitorPunish* punishes,
                             int max_count,
                             int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !ip || !punishes || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, type, m_id_cnt, start_time, end_time "
        "FROM ip_monitor_punish WHERE ip = '%s' ORDER BY type", ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&punishes[count], 0, sizeof(IpMonitorPunish));
        if (row[0]) strncpy(punishes[count].ip, row[0], sizeof(punishes[count].ip) - 1);
        punishes[count].type = row[1] ? (signed char)atoi(row[1]) : 0;
        punishes[count].m_id_cnt = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(punishes[count].start_time, row[3], sizeof(punishes[count].start_time) - 1);
        if (row[4]) strncpy(punishes[count].end_time, row[4], sizeof(punishes[count].end_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_GetByType(DBConnectionManager* manager,
                               signed char type,
                               IpMonitorPunish* punishes,
                               int max_count,
                               int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !punishes || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, type, m_id_cnt, start_time, end_time "
        "FROM ip_monitor_punish WHERE type = %d ORDER BY ip", type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&punishes[count], 0, sizeof(IpMonitorPunish));
        if (row[0]) strncpy(punishes[count].ip, row[0], sizeof(punishes[count].ip) - 1);
        punishes[count].type = row[1] ? (signed char)atoi(row[1]) : 0;
        punishes[count].m_id_cnt = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(punishes[count].start_time, row[3], sizeof(punishes[count].start_time) - 1);
        if (row[4]) strncpy(punishes[count].end_time, row[4], sizeof(punishes[count].end_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int IpMonitorPunish_DeleteExpired(DBConnectionManager* manager, const char* current_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !current_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM ip_monitor_punish WHERE end_time < '%s'", current_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void IpMonitorPunish_PrintInfo(const IpMonitorPunish* punish) {
    if (!punish) return;

    printf("IP Monitor Punish:\n");
    printf("  IP Address: %s\n", punish->ip);
    printf("  Type: %d\n", punish->type);
    printf("  Member ID Count: %u\n", punish->m_id_cnt);
    printf("  Start Time: %s\n", punish->start_time);
    printf("  End Time: %s\n", punish->end_time);
}
