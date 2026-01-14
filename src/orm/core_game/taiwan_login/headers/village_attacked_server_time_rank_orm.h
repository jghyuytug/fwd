/**
 * @file village_attacked_server_time_rank_orm.h
 * @brief ORM header for village_attacked_server_time_rank table
 *
 * Database: taiwan_login (DB_TYPE_LOGIN = 4)
 * Table: village_attacked_server_time_rank
 * Description: Server-wide fastest clear time rankings for village attack events
 * Primary Key: (server_info, occ_date) - 2-field composite key
 */

#ifndef VILLAGE_ATTACKED_SERVER_TIME_RANK_ORM_H
#define VILLAGE_ATTACKED_SERVER_TIME_RANK_ORM_H

#include "db_connection_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct VillageAttackedServerTimeRank
 * @brief Structure representing village_attacked_server_time_rank table
 *
 * Tracks the fastest clear time for each server on a specific date
 */
typedef struct {
    unsigned char server_info;      /**< Server identifier (Primary Key 1) */
    char occ_date[11];              /**< Event occurrence date 'YYYY-MM-DD' (Primary Key 2) */
    unsigned int clear_time;        /**< Clear time in seconds (lower is better) */
    unsigned char rank;             /**< Rank for this record (1 = fastest) */
} VillageAttackedServerTimeRank;

/* ========================================================================
 * Basic CRUD Operations
 * ======================================================================== */

/**
 * @brief Add a new server time rank record
 * @param manager DBConnectionManager handle
 * @param record Pointer to VillageAttackedServerTimeRank structure
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerTimeRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedServerTimeRank* record
);

/**
 * @brief Get a server time rank record by composite primary key
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier (Primary Key 1)
 * @param occ_date Event occurrence date (Primary Key 2)
 * @param record Pointer to store retrieved record
 * @return 1 on success, 0 if not found or error
 */
int VillageAttackedServerTimeRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedServerTimeRank* record
);

/**
 * @brief Update an existing server time rank record
 * @param manager DBConnectionManager handle
 * @param record Pointer to VillageAttackedServerTimeRank structure with updated values
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerTimeRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedServerTimeRank* record
);

/**
 * @brief Delete a server time rank record by composite primary key
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier (Primary Key 1)
 * @param occ_date Event occurrence date (Primary Key 2)
 * @return 1 on success, 0 on failure
 */
int VillageAttackedServerTimeRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
);

/**
 * @brief Check if a server time rank record exists
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier (Primary Key 1)
 * @param occ_date Event occurrence date (Primary Key 2)
 * @return 1 if exists, 0 otherwise
 */
int VillageAttackedServerTimeRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date
);

/* ========================================================================
 * Business Operations
 * ======================================================================== */

/**
 * @brief Get all time rank records for a specific server
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param records Array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerTimeRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    VillageAttackedServerTimeRank* records,
    int max_records
);

/**
 * @brief Get top N fastest clear times for a specific date
 * @param manager DBConnectionManager handle
 * @param occ_date Event occurrence date
 * @param top_n Number of top records to retrieve
 * @param records Array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerTimeRank_GetTopRanks(
    DBConnectionManager* manager,
    const char* occ_date,
    int top_n,
    VillageAttackedServerTimeRank* records,
    int max_records
);

/**
 * @brief Get time rank records for a server within a date range
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param start_date Start date (inclusive)
 * @param end_date End date (inclusive)
 * @param records Array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedServerTimeRank_GetByDateRange(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* start_date,
    const char* end_date,
    VillageAttackedServerTimeRank* records,
    int max_records
);

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

/**
 * @brief Print server time rank record information
 * @param record Pointer to VillageAttackedServerTimeRank structure
 */
void VillageAttackedServerTimeRank_PrintInfo(
    const VillageAttackedServerTimeRank* record
);

#ifdef __cplusplus
}
#endif

#endif /* VILLAGE_ATTACKED_SERVER_TIME_RANK_ORM_H */
