#include "max_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add new max count record */
int MaxCount_Add(DBConnectionManager* manager, const MaxCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count (server_info, mc_max, mc_date) VALUES (%u, %u, '%s')",
        record->server_info, record->mc_max, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get by server_info (indexed field) */
int MaxCount_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                               MaxCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count "
        "WHERE server_info = %u ORDER BY mc_date DESC", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCount));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].mc_max = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(records[count].mc_date, row[2], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get by date range (indexed field) */
int MaxCount_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                             MaxCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count || !start_date || !end_date) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count "
        "WHERE mc_date >= '%s' AND mc_date <= '%s' ORDER BY mc_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCount));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].mc_max = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(records[count].mc_date, row[2], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get latest record for a server */
int MaxCount_GetLatest(DBConnectionManager* manager, unsigned char server_info,
                        MaxCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count "
        "WHERE server_info = %u ORDER BY mc_date DESC LIMIT 1", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MaxCount));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    record->mc_max = row[1] ? (unsigned int)atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(record->mc_date, row[2], sizeof(record->mc_date) - 1);
        record->mc_date[sizeof(record->mc_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get all records */
int MaxCount_GetAll(DBConnectionManager* manager, MaxCount* records,
                     int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count "
        "ORDER BY mc_date DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCount));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].mc_max = row[1] ? (unsigned int)atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(records[count].mc_date, row[2], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by server_info */
int MaxCount_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count WHERE server_info = %u", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by date range */
int MaxCount_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !start_date || !end_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count WHERE mc_date >= '%s' AND mc_date <= '%s'",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print max count information */
void MaxCount_PrintInfo(const MaxCount* record) {
    if (!record) {
        printf("MaxCount: NULL\n");
        return;
    }

    printf("MaxCount:\n");
    printf("  server_info: %u\n", record->server_info);
    printf("  mc_max: %u\n", record->mc_max);
    printf("  mc_date: %s\n", record->mc_date);
}
