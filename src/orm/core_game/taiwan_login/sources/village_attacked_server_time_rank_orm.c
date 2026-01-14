/**
 * @file village_attacked_server_time_rank_orm.c
 * @brief ORM implementation for village_attacked_server_time_rank table
 *
 * Database: taiwan_login (DB_TYPE_LOGIN = 4)
 * Table: village_attacked_server_time_rank
 * Description: Server-wide fastest clear time rankings for village attack events
 */

#include "../headers/village_attacked_server_time_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024

/* ========================================================================
 * Basic CRUD Operations
 * ======================================================================== */

int VillageAttackedServerTimeRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedServerTimeRank* record)
{
    if (!manager || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "INSERT INTO village_attacked_server_time_rank "
        "(server_info, occ_date, clear_time, rank) "
        "VALUES (%u, '%s', %u, %u)",
        record->server_info,
        record->occ_date,
        record->clear_time,
        record->rank
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerTimeRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedServerTimeRank* record)
{
    if (!manager || !occ_date || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, clear_time, rank "
        "FROM village_attacked_server_time_rank "
        "WHERE server_info = %u AND occ_date = '%s'",
        server_info,
        occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    memset(record, 0, sizeof(VillageAttackedServerTimeRank));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
        record->occ_date[sizeof(record->occ_date) - 1] = '\0';
    }
    record->clear_time = row[2] ? (unsigned int)atol(row[2]) : 0;
    record->rank = row[3] ? (unsigned char)atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerTimeRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedServerTimeRank* record)
{
    if (!manager || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "UPDATE village_attacked_server_time_rank "
        "SET clear_time = %u, rank = %u "
        "WHERE server_info = %u AND occ_date = '%s'",
        record->clear_time,
        record->rank,
        record->server_info,
        record->occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerTimeRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date)
{
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "DELETE FROM village_attacked_server_time_rank "
        "WHERE server_info = %u AND occ_date = '%s'",
        server_info,
        occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerTimeRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date)
{
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM village_attacked_server_time_rank "
        "WHERE server_info = %u AND occ_date = '%s'",
        server_info,
        occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ========================================================================
 * Business Operations
 * ======================================================================== */

int VillageAttackedServerTimeRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    VillageAttackedServerTimeRank* records,
    int max_records)
{
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, clear_time, rank "
        "FROM village_attacked_server_time_rank "
        "WHERE server_info = %u "
        "ORDER BY occ_date DESC",
        server_info
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedServerTimeRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].clear_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        records[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedServerTimeRank_GetTopRanks(
    DBConnectionManager* manager,
    const char* occ_date,
    int top_n,
    VillageAttackedServerTimeRank* records,
    int max_records)
{
    if (!manager || !occ_date || !records || top_n <= 0 || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, clear_time, rank "
        "FROM village_attacked_server_time_rank "
        "WHERE occ_date = '%s' "
        "ORDER BY clear_time ASC "
        "LIMIT %d",
        occ_date,
        top_n
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedServerTimeRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].clear_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        records[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedServerTimeRank_GetByDateRange(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* start_date,
    const char* end_date,
    VillageAttackedServerTimeRank* records,
    int max_records)
{
    if (!manager || !start_date || !end_date || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, clear_time, rank "
        "FROM village_attacked_server_time_rank "
        "WHERE server_info = %u AND occ_date BETWEEN '%s' AND '%s' "
        "ORDER BY occ_date DESC",
        server_info,
        start_date,
        end_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedServerTimeRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].clear_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        records[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

void VillageAttackedServerTimeRank_PrintInfo(
    const VillageAttackedServerTimeRank* record)
{
    if (!record) {
        printf("VillageAttackedServerTimeRank: NULL\n");
        return;
    }

    printf("=== Village Attacked Server Time Rank ===\n");
    printf("Server Info:  %u\n", record->server_info);
    printf("Occ Date:     %s\n", record->occ_date);
    printf("Clear Time:   %u seconds\n", record->clear_time);
    printf("Rank:         %u\n", record->rank);
    printf("==========================================\n");
}
