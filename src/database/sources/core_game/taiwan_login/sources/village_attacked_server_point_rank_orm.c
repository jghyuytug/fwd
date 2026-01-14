#include "../headers/village_attacked_server_point_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024

// ============================================================================
// Basic CRUD Operations
// ============================================================================

int VillageAttackedServerPointRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedServerPointRank* rank
) {
    if (!manager || !rank) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "INSERT INTO village_attacked_server_point_rank "
        "(server_info, occ_date, hunting_point, rank) "
        "VALUES (%u, '%s', %u, %u)",
        rank->server_info,
        rank->occ_date,
        rank->hunting_point,
        rank->rank
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerPointRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedServerPointRank* out_rank
) {
    if (!manager || !occ_date || !out_rank) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, hunting_point, rank "
        "FROM village_attacked_server_point_rank "
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

    memset(out_rank, 0, sizeof(VillageAttackedServerPointRank));
    out_rank->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(out_rank->occ_date, row[1], sizeof(out_rank->occ_date) - 1);
        out_rank->occ_date[sizeof(out_rank->occ_date) - 1] = '\0';
    }
    out_rank->hunting_point = row[2] ? (unsigned int)atol(row[2]) : 0;
    out_rank->rank = row[3] ? (unsigned char)atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerPointRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedServerPointRank* rank
) {
    if (!manager || !rank) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "UPDATE village_attacked_server_point_rank "
        "SET hunting_point = %u, rank = %u "
        "WHERE server_info = %u AND occ_date = '%s'",
        rank->hunting_point,
        rank->rank,
        rank->server_info,
        rank->occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedServerPointRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
) {
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "DELETE FROM village_attacked_server_point_rank "
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

int VillageAttackedServerPointRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
) {
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM village_attacked_server_point_rank "
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

// ============================================================================
// Business Operations
// ============================================================================

int VillageAttackedServerPointRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
) {
    if (!manager || !out_ranks || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, hunting_point, rank "
        "FROM village_attacked_server_point_rank "
        "WHERE server_info = %u "
        "ORDER BY occ_date DESC "
        "LIMIT %d",
        server_info,
        max_count
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&out_ranks[count], 0, sizeof(VillageAttackedServerPointRank));
        out_ranks[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(out_ranks[count].occ_date, row[1], sizeof(out_ranks[count].occ_date) - 1);
            out_ranks[count].occ_date[sizeof(out_ranks[count].occ_date) - 1] = '\0';
        }
        out_ranks[count].hunting_point = row[2] ? (unsigned int)atol(row[2]) : 0;
        out_ranks[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedServerPointRank_GetTopRanks(
    DBConnectionManager* manager,
    const char* occ_date,
    int top_n,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
) {
    if (!manager || !occ_date || !out_ranks || max_count <= 0 || top_n <= 0) {
        return -1;
    }

    int limit = (top_n < max_count) ? top_n : max_count;
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, hunting_point, rank "
        "FROM village_attacked_server_point_rank "
        "WHERE occ_date = '%s' "
        "ORDER BY rank ASC "
        "LIMIT %d",
        occ_date,
        limit
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&out_ranks[count], 0, sizeof(VillageAttackedServerPointRank));
        out_ranks[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(out_ranks[count].occ_date, row[1], sizeof(out_ranks[count].occ_date) - 1);
            out_ranks[count].occ_date[sizeof(out_ranks[count].occ_date) - 1] = '\0';
        }
        out_ranks[count].hunting_point = row[2] ? (unsigned int)atol(row[2]) : 0;
        out_ranks[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedServerPointRank_GetByDateRange(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* start_date,
    const char* end_date,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
) {
    if (!manager || !start_date || !end_date || !out_ranks || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (server_info == 0) {
        snprintf(query, sizeof(query),
            "SELECT server_info, occ_date, hunting_point, rank "
            "FROM village_attacked_server_point_rank "
            "WHERE occ_date BETWEEN '%s' AND '%s' "
            "ORDER BY occ_date DESC, rank ASC "
            "LIMIT %d",
            start_date,
            end_date,
            max_count
        );
    } else {
        snprintf(query, sizeof(query),
            "SELECT server_info, occ_date, hunting_point, rank "
            "FROM village_attacked_server_point_rank "
            "WHERE server_info = %u AND occ_date BETWEEN '%s' AND '%s' "
            "ORDER BY occ_date DESC "
            "LIMIT %d",
            server_info,
            start_date,
            end_date,
            max_count
        );
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&out_ranks[count], 0, sizeof(VillageAttackedServerPointRank));
        out_ranks[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(out_ranks[count].occ_date, row[1], sizeof(out_ranks[count].occ_date) - 1);
            out_ranks[count].occ_date[sizeof(out_ranks[count].occ_date) - 1] = '\0';
        }
        out_ranks[count].hunting_point = row[2] ? (unsigned int)atol(row[2]) : 0;
        out_ranks[count].rank = row[3] ? (unsigned char)atoi(row[3]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ============================================================================
// Utility Functions
// ============================================================================

void VillageAttackedServerPointRank_PrintInfo(const VillageAttackedServerPointRank* rank) {
    if (!rank) {
        printf("VillageAttackedServerPointRank is NULL\n");
        return;
    }

    printf("=== Village Attacked Server Point Rank Info ===\n");
    printf("Server Info:    %u\n", rank->server_info);
    printf("Date:           %s\n", rank->occ_date);
    printf("Hunting Point:  %u\n", rank->hunting_point);
    printf("Rank:           %u\n", rank->rank);
    printf("============================================\n");
}
