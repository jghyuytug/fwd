#include "max_count_pvp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MaxCountPvp_Add(DBConnectionManager* manager, const MaxCountPvp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->mc_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO max_count_pvp (server_info, mc_max, mc_date) "
            "VALUES (%u, %u, '%s')",
            record->server_info, record->mc_max, record->mc_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO max_count_pvp (server_info, mc_max, mc_date) "
            "VALUES (%u, %u, NOW())",
            record->server_info, record->mc_max);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountPvp_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                 MaxCountPvp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count_pvp "
        "WHERE server_info = %u ORDER BY mc_date DESC",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountPvp));
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

int MaxCountPvp_UpdateByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                    const MaxCountPvp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->mc_date[0]) {
        snprintf(query, sizeof(query),
            "UPDATE max_count_pvp SET mc_max = %u, mc_date = '%s' "
            "WHERE server_info = %u",
            record->mc_max, record->mc_date, server_info);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE max_count_pvp SET mc_max = %u, mc_date = NOW() "
            "WHERE server_info = %u",
            record->mc_max, server_info);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountPvp_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM max_count_pvp WHERE server_info = %u", server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCountPvp_GetAll(DBConnectionManager* manager, MaxCountPvp* records,
                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count_pvp "
        "ORDER BY mc_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountPvp));
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

int MaxCountPvp_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                const char* end_date, MaxCountPvp* records,
                                int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count_pvp "
        "WHERE mc_date BETWEEN '%s' AND '%s' ORDER BY mc_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountPvp));
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

int MaxCountPvp_GetLatest(DBConnectionManager* manager, MaxCountPvp* records,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count_pvp "
        "ORDER BY mc_date DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCountPvp));
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

void MaxCountPvp_PrintInfo(const MaxCountPvp* record) {
    if (!record) {
        printf("NULL record\n");
        return;
    }

    printf("MaxCountPvp Info:\n");
    printf("  Server Info: %u\n", record->server_info);
    printf("  MC Max: %u\n", record->mc_max);
    printf("  MC Date: %s\n", record->mc_date);
}
