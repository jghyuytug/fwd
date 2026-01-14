#include "../headers/village_attacked_charac_point_rank_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_QUERY_LEN 1024

// ============================================================================
// Basic CRUD Operations
// ============================================================================

int VillageAttackedCharacPointRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedCharacPointRank* record
) {
    if (!manager || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "INSERT INTO village_attacked_charac_point_rank "
        "(server_info, occ_date, charac_no, hunting_point, rank) "
        "VALUES (%u, '%s', %u, %u, %u)",
        record->server_info,
        record->occ_date,
        record->charac_no,
        record->hunting_point,
        record->rank
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedCharacPointRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no,
    VillageAttackedCharacPointRank* record
) {
    if (!manager || !occ_date || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, charac_no, hunting_point, rank "
        "FROM village_attacked_charac_point_rank "
        "WHERE server_info = %u AND occ_date = '%s' AND charac_no = %u",
        server_info,
        occ_date,
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    memset(record, 0, sizeof(VillageAttackedCharacPointRank));
    record->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
        record->occ_date[sizeof(record->occ_date) - 1] = '\0';
    }
    record->charac_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
    record->hunting_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
    record->rank = row[4] ? (unsigned char)atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedCharacPointRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedCharacPointRank* record
) {
    if (!manager || !record) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "UPDATE village_attacked_charac_point_rank "
        "SET hunting_point = %u, rank = %u "
        "WHERE server_info = %u AND occ_date = '%s' AND charac_no = %u",
        record->hunting_point,
        record->rank,
        record->server_info,
        record->occ_date,
        record->charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedCharacPointRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no
) {
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "DELETE FROM village_attacked_charac_point_rank "
        "WHERE server_info = %u AND occ_date = '%s' AND charac_no = %u",
        server_info,
        occ_date,
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return 0;
    }

    DBQueryResult_Free(&result);
    return 1;
}

int VillageAttackedCharacPointRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no
) {
    if (!manager || !occ_date) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM village_attacked_charac_point_rank "
        "WHERE server_info = %u AND occ_date = '%s' AND charac_no = %u",
        server_info,
        occ_date,
        charac_no
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

int VillageAttackedCharacPointRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedCharacPointRank* records,
    int max_records
) {
    if (!manager || !occ_date || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, charac_no, hunting_point, rank "
        "FROM village_attacked_charac_point_rank "
        "WHERE server_info = %u AND occ_date = '%s' "
        "ORDER BY rank ASC",
        server_info,
        occ_date
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedCharacPointRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].charac_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].hunting_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].rank = row[4] ? (unsigned char)atoi(row[4]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedCharacPointRank_GetTopRanks(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned char top_n,
    VillageAttackedCharacPointRank* records,
    int max_records
) {
    if (!manager || !occ_date || !records || max_records <= 0 || top_n == 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, charac_no, hunting_point, rank "
        "FROM village_attacked_charac_point_rank "
        "WHERE server_info = %u AND occ_date = '%s' AND rank <= %u "
        "ORDER BY rank ASC",
        server_info,
        occ_date,
        top_n
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedCharacPointRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].charac_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].hunting_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].rank = row[4] ? (unsigned char)atoi(row[4]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int VillageAttackedCharacPointRank_GetByCharacter(
    DBConnectionManager* manager,
    unsigned int charac_no,
    VillageAttackedCharacPointRank* records,
    int max_records
) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    snprintf(query, sizeof(query),
        "SELECT server_info, occ_date, charac_no, hunting_point, rank "
        "FROM village_attacked_charac_point_rank "
        "WHERE charac_no = %u "
        "ORDER BY occ_date DESC",
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackedCharacPointRank));
        records[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].charac_no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        records[count].hunting_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        records[count].rank = row[4] ? (unsigned char)atoi(row[4]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ============================================================================
// Utility Functions
// ============================================================================

void VillageAttackedCharacPointRank_PrintInfo(const VillageAttackedCharacPointRank* record) {
    if (!record) {
        printf("VillageAttackedCharacPointRank: NULL\n");
        return;
    }

    printf("=== Village Attacked Character Point Rank ===\n");
    printf("Server Info: %u\n", record->server_info);
    printf("Date: %s\n", record->occ_date);
    printf("Character No: %u\n", record->charac_no);
    printf("Hunting Point: %u\n", record->hunting_point);
    printf("Rank: %u\n", record->rank);
    printf("==========================================\n");
}
