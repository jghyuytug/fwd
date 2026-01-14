#ifndef VILLAGE_ATTACKED_SERVER_POINT_RANK_ORM_H
#define VILLAGE_ATTACKED_SERVER_POINT_RANK_ORM_H

#include "db_connection_manager.h"

/**
 * Village Attacked Server Point Rank ORM
 * Table: village_attacked_server_point_rank
 * Database: DB_TYPE_LOGIN (4)
 * Primary Key: (server_info, occ_date) - 2-field composite key
 * Description: Tracks server ranking points for village attacked event by date
 */

typedef struct {
    unsigned char server_info;      // Server identifier (PRIMARY KEY 1/2)
    char occ_date[11];              // Occurrence date YYYY-MM-DD format (PRIMARY KEY 2/2)
    unsigned int hunting_point;     // Hunting points accumulated
    unsigned char rank;             // Server rank for this date
} VillageAttackedServerPointRank;

// ============================================================================
// Basic CRUD Operations
// ============================================================================

/**
 * Add new village attacked server point rank record
 * @param manager DBConnectionManager handle
 * @param rank Pointer to VillageAttackedServerPointRank structure
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerPointRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedServerPointRank* rank
);

/**
 * Get village attacked server point rank by composite primary key
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Occurrence date (format: "YYYY-MM-DD")
 * @param out_rank Output structure to store result
 * @return 1 if found, 0 if not found or error
 */
int VillageAttackedServerPointRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedServerPointRank* out_rank
);

/**
 * Update existing village attacked server point rank record
 * @param manager DBConnectionManager handle
 * @param rank Pointer to VillageAttackedServerPointRank structure with updated data
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerPointRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedServerPointRank* rank
);

/**
 * Delete village attacked server point rank by composite primary key
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Occurrence date (format: "YYYY-MM-DD")
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerPointRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
);

/**
 * Check if village attacked server point rank exists by composite primary key
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Occurrence date (format: "YYYY-MM-DD")
 * @return 1 if exists, 0 otherwise
 */
int VillageAttackedServerPointRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
);

// ============================================================================
// Business Operations
// ============================================================================

/**
 * Get all ranking records for a specific server
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param out_ranks Output array to store results
 * @param max_count Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerPointRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
);

/**
 * Get top N ranked servers for a specific date
 * @param manager DBConnectionManager handle
 * @param occ_date Occurrence date (format: "YYYY-MM-DD")
 * @param top_n Number of top servers to retrieve
 * @param out_ranks Output array to store results
 * @param max_count Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerPointRank_GetTopRanks(
    DBConnectionManager* manager,
    const char* occ_date,
    int top_n,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
);

/**
 * Get ranking records for a date range
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier (0 for all servers)
 * @param start_date Start date (format: "YYYY-MM-DD")
 * @param end_date End date (format: "YYYY-MM-DD")
 * @param out_ranks Output array to store results
 * @param max_count Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerPointRank_GetByDateRange(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* start_date,
    const char* end_date,
    VillageAttackedServerPointRank* out_ranks,
    int max_count
);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Print village attacked server point rank information (for debugging)
 * @param rank Pointer to VillageAttackedServerPointRank structure
 */
void VillageAttackedServerPointRank_PrintInfo(const VillageAttackedServerPointRank* rank);

#endif // VILLAGE_ATTACKED_SERVER_POINT_RANK_ORM_H
