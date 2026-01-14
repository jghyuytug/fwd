#ifndef VILLAGE_ATTACKED_CHARAC_POINT_RANK_ORM_H
#define VILLAGE_ATTACKED_CHARAC_POINT_RANK_ORM_H

#include "db_connection_manager.h"

/**
 * ORM structure for village_attacked_charac_point_rank table
 *
 * Table: village_attacked_charac_point_rank
 * Database: DB_TYPE_LOGIN (4)
 * Primary Key: Composite (server_info, occ_date, charac_no)
 *
 * Purpose: Track character hunting points and rankings during village attack events
 */
typedef struct {
    unsigned char server_info;      // Server identifier
    char occ_date[11];              // Event date (YYYY-MM-DD format)
    unsigned int charac_no;         // Character number
    unsigned int hunting_point;     // Accumulated hunting points
    unsigned char rank;             // Character's rank position
} VillageAttackedCharacPointRank;

// ============================================================================
// Basic CRUD Operations
// ============================================================================

/**
 * Add a new village attacked character point rank record
 *
 * @param manager DBConnectionManager handle
 * @param record Pointer to VillageAttackedCharacPointRank structure
 * @return 1 on success, 0 on failure
 */
int VillageAttackedCharacPointRank_Add(
    DBConnectionManager* manager,
    const VillageAttackedCharacPointRank* record
);

/**
 * Get a village attacked character point rank by composite primary key
 *
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Event date (YYYY-MM-DD format)
 * @param charac_no Character number
 * @param record Pointer to store retrieved record
 * @return 1 on success, 0 if not found or error
 */
int VillageAttackedCharacPointRank_Get(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no,
    VillageAttackedCharacPointRank* record
);

/**
 * Update a village attacked character point rank record
 *
 * @param manager DBConnectionManager handle
 * @param record Pointer to VillageAttackedCharacPointRank structure with updated values
 * @return 1 on success, 0 on failure
 */
int VillageAttackedCharacPointRank_Update(
    DBConnectionManager* manager,
    const VillageAttackedCharacPointRank* record
);

/**
 * Delete a village attacked character point rank by composite primary key
 *
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Event date (YYYY-MM-DD format)
 * @param charac_no Character number
 * @return 1 on success, 0 on failure
 */
int VillageAttackedCharacPointRank_Delete(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no
);

/**
 * Check if a village attacked character point rank exists by composite primary key
 *
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Event date (YYYY-MM-DD format)
 * @param charac_no Character number
 * @return 1 if exists, 0 if not exists or error
 */
int VillageAttackedCharacPointRank_Exists(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned int charac_no
);

// ============================================================================
// Business Operations
// ============================================================================

/**
 * Get all character point ranks for a specific server and date
 *
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Event date (YYYY-MM-DD format)
 * @param records Pointer to array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedCharacPointRank_GetByServer(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    VillageAttackedCharacPointRank* records,
    int max_records
);

/**
 * Get top N ranked characters for a specific server and date
 *
 * @param manager DBConnectionManager handle
 * @param server_info Server identifier
 * @param occ_date Event date (YYYY-MM-DD format)
 * @param top_n Number of top ranks to retrieve
 * @param records Pointer to array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedCharacPointRank_GetTopRanks(
    DBConnectionManager* manager,
    unsigned char server_info,
    const char* occ_date,
    unsigned char top_n,
    VillageAttackedCharacPointRank* records,
    int max_records
);

/**
 * Get character's rank history across all dates
 *
 * @param manager DBConnectionManager handle
 * @param charac_no Character number
 * @param records Pointer to array to store retrieved records
 * @param max_records Maximum number of records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int VillageAttackedCharacPointRank_GetByCharacter(
    DBConnectionManager* manager,
    unsigned int charac_no,
    VillageAttackedCharacPointRank* records,
    int max_records
);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Print village attacked character point rank information
 *
 * @param record Pointer to VillageAttackedCharacPointRank structure
 */
void VillageAttackedCharacPointRank_PrintInfo(const VillageAttackedCharacPointRank* record);

#endif // VILLAGE_ATTACKED_CHARAC_POINT_RANK_ORM_H
