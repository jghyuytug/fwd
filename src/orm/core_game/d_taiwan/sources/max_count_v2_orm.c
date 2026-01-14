#include "max_count_v2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add new max count v2 record */
int MaxCountV2_Add(DBConnectionManager* manager, const MaxCountV2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count_v2 (server_info, num_occupations_charscreen, "
        "num_occupations_seriaroom, num_login_per_min, num_logout_per_min, mc_date) "
        "VALUES (%u, %u, %u, %u, %u, '%s')",
        record->server_info, record->num_occupations_charscreen,
        record->num_occupations_seriaroom, record->num_login_per_min,
        record->num_logout_per_min, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get by server_info */
int MaxCountV2_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                MaxCountV2* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, num_occupations_charscreen, num_occupations_seriaroom, "
        "num_login_per_min, num_logout_per_min, mc_date FROM max_count_v2 "
        "WHERE server_info = %u ORDER BY mc_date DESC", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountV2));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].num_occupations_charscreen = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].num_occupations_seriaroom = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].num_login_per_min = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].num_logout_per_min = row[4] ? (unsigned int)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].mc_date, row[5], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get by date range */
int MaxCountV2_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                               MaxCountV2* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count || !start_date || !end_date) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, num_occupations_charscreen, num_occupations_seriaroom, "
        "num_login_per_min, num_logout_per_min, mc_date FROM max_count_v2 "
        "WHERE mc_date >= '%s' AND mc_date <= '%s' ORDER BY mc_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountV2));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].num_occupations_charscreen = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].num_occupations_seriaroom = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].num_login_per_min = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].num_logout_per_min = row[4] ? (unsigned int)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].mc_date, row[5], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get latest record for a server */
int MaxCountV2_GetLatest(DBConnectionManager* manager, unsigned char server_info,
                          MaxCountV2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, num_occupations_charscreen, num_occupations_seriaroom, "
        "num_login_per_min, num_logout_per_min, mc_date FROM max_count_v2 "
        "WHERE server_info = %u ORDER BY mc_date DESC LIMIT 1", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MaxCountV2));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    record->num_occupations_charscreen = row[1] ? (unsigned int)atoi(row[1]) : 0;
    record->num_occupations_seriaroom = row[2] ? (unsigned int)atoi(row[2]) : 0;
    record->num_login_per_min = row[3] ? (unsigned int)atoi(row[3]) : 0;
    record->num_logout_per_min = row[4] ? (unsigned int)atoi(row[4]) : 0;
    if (row[5]) {
        strncpy(record->mc_date, row[5], sizeof(record->mc_date) - 1);
        record->mc_date[sizeof(record->mc_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get all records */
int MaxCountV2_GetAll(DBConnectionManager* manager, MaxCountV2* records,
                       int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, num_occupations_charscreen, num_occupations_seriaroom, "
        "num_login_per_min, num_logout_per_min, mc_date FROM max_count_v2 "
        "ORDER BY mc_date DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountV2));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        records[count].num_occupations_charscreen = row[1] ? (unsigned int)atoi(row[1]) : 0;
        records[count].num_occupations_seriaroom = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].num_login_per_min = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].num_logout_per_min = row[4] ? (unsigned int)atoi(row[4]) : 0;
        if (row[5]) {
            strncpy(records[count].mc_date, row[5], sizeof(records[count].mc_date) - 1);
            records[count].mc_date[sizeof(records[count].mc_date) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by server_info */
int MaxCountV2_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count_v2 WHERE server_info = %u", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by date range */
int MaxCountV2_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !start_date || !end_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count_v2 WHERE mc_date >= '%s' AND mc_date <= '%s'",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print max count v2 information */
void MaxCountV2_PrintInfo(const MaxCountV2* record) {
    if (!record) {
        printf("MaxCountV2: NULL\n");
        return;
    }

    printf("MaxCountV2:\n");
    printf("  server_info: %u\n", record->server_info);
    printf("  num_occupations_charscreen: %u\n", record->num_occupations_charscreen);
    printf("  num_occupations_seriaroom: %u\n", record->num_occupations_seriaroom);
    printf("  num_login_per_min: %u\n", record->num_login_per_min);
    printf("  num_logout_per_min: %u\n", record->num_logout_per_min);
    printf("  mc_date: %s\n", record->mc_date);
}
