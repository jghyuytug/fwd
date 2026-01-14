#include "log_query_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int LogQueryStat_Add(DBConnectionManager* manager, const LogQueryStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "REPLACE INTO log_query_stat (occ_time, q_id, gc_no, total, response_time) "
        "VALUES ('%s', %u, %u, %u, %u)",
        stat->occ_time, stat->q_id, stat->gc_no, stat->total, stat->response_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_Get(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                      unsigned int gc_no, LogQueryStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !occ_time || !stat) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time "
        "FROM log_query_stat WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(stat, 0, sizeof(LogQueryStat));
    if (row[0]) strncpy(stat->occ_time, row[0], sizeof(stat->occ_time) - 1);
    stat->q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
    stat->gc_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
    stat->total = row[3] ? (unsigned int)atoi(row[3]) : 0;
    stat->response_time = row[4] ? (unsigned int)atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_Update(DBConnectionManager* manager, const LogQueryStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_query_stat SET total = %u, response_time = %u "
        "WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        stat->total, stat->response_time, stat->occ_time, stat->q_id, stat->gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_Delete(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                         unsigned int gc_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_stat WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_Exists(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                         unsigned int gc_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !occ_time) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_query_stat WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Specialized Operations */

int LogQueryStat_UpdateTotal(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                              unsigned int gc_no, unsigned int new_total) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_query_stat SET total = %u "
        "WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        new_total, occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_UpdateResponseTime(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                                     unsigned int gc_no, unsigned int new_response_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_query_stat SET response_time = %u "
        "WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        new_response_time, occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_IncrementTotal(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                                 unsigned int gc_no, unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_query_stat SET total = total + %u "
        "WHERE occ_time = '%s' AND q_id = %u AND gc_no = %u",
        increment, occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Query Operations */

int LogQueryStat_GetByQueryId(DBConnectionManager* manager, unsigned short q_id,
                               LogQueryStat* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time "
        "FROM log_query_stat WHERE q_id = %u ORDER BY occ_time DESC",
        q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(LogQueryStat));
        if (row[0]) strncpy(stats[count].occ_time, row[0], sizeof(stats[count].occ_time) - 1);
        stats[count].q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
        stats[count].gc_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        stats[count].total = row[3] ? (unsigned int)atoi(row[3]) : 0;
        stats[count].response_time = row[4] ? (unsigned int)atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                 const char* end_time, LogQueryStat* stats, int max_count,
                                 int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !start_time || !end_time || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time "
        "FROM log_query_stat WHERE occ_time >= '%s' AND occ_time <= '%s' "
        "ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(LogQueryStat));
        if (row[0]) strncpy(stats[count].occ_time, row[0], sizeof(stats[count].occ_time) - 1);
        stats[count].q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
        stats[count].gc_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        stats[count].total = row[3] ? (unsigned int)atoi(row[3]) : 0;
        stats[count].response_time = row[4] ? (unsigned int)atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_GetByChannel(DBConnectionManager* manager, unsigned int gc_no,
                               LogQueryStat* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time "
        "FROM log_query_stat WHERE gc_no = %u ORDER BY occ_time DESC",
        gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(LogQueryStat));
        if (row[0]) strncpy(stats[count].occ_time, row[0], sizeof(stats[count].occ_time) - 1);
        stats[count].q_id = row[1] ? (unsigned short)atoi(row[1]) : 0;
        stats[count].gc_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        stats[count].total = row[3] ? (unsigned int)atoi(row[3]) : 0;
        stats[count].response_time = row[4] ? (unsigned int)atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_CountByQueryId(DBConnectionManager* manager, unsigned short q_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_query_stat WHERE q_id = %u", q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int LogQueryStat_CountByTimeRange(DBConnectionManager* manager, const char* start_time,
                                   const char* end_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager || !start_time || !end_time) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_query_stat WHERE occ_time >= '%s' AND occ_time <= '%s'",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int LogQueryStat_GetAverageResponseTime(DBConnectionManager* manager, unsigned short q_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int avg_time = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT AVG(response_time) FROM log_query_stat WHERE q_id = %u", q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        avg_time = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return avg_time;
}

int LogQueryStat_GetMaxResponseTime(DBConnectionManager* manager, unsigned short q_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int max_time = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT MAX(response_time) FROM log_query_stat WHERE q_id = %u", q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        max_time = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return max_time;
}

/* Batch Operations */

int LogQueryStat_DeleteByTimeRange(DBConnectionManager* manager, const char* start_time,
                                    const char* end_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !start_time || !end_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_stat WHERE occ_time >= '%s' AND occ_time <= '%s'",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_DeleteOlderThan(DBConnectionManager* manager, int days) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || days <= 0) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_stat WHERE occ_time < DATE_SUB(NOW(), INTERVAL %d DAY)",
        days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions */

void LogQueryStat_PrintInfo(const LogQueryStat* stat) {
    if (!stat) {
        printf("LogQueryStat: NULL\n");
        return;
    }

    printf("=== Log Query Statistics ===\n");
    printf("Time:          %s\n", stat->occ_time);
    printf("Query ID:      %u\n", stat->q_id);
    printf("Channel No:    %u\n", stat->gc_no);
    printf("Total:         %u\n", stat->total);
    printf("Response Time: %u ms\n", stat->response_time);
    printf("============================\n");
}
