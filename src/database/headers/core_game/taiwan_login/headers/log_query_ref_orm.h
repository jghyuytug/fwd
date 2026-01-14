#ifndef LOG_QUERY_REF_ORM_H
#define LOG_QUERY_REF_ORM_H

#include "db_connection_manager.h"

/**
 * LogQueryRef - Query reference for Query Reference
 * Maps to taiwan_login.log_query_ref table
 * Primary Key: q_id (auto_increment)
 * Unique Key: query_hash
 */
typedef struct {
    char query_hash[17];       /* VARCHAR(16) - UNIQUE KEY */
    unsigned short q_id;       /* SMALLINT UNSIGNED - AUTO_INCREMENT PRIMARY KEY */
    char query[4096];          /* TEXT field (max 4KB) */
} LogQueryRef;

/* ========== CRUD Operations (AUTO_INCREMENT Primary Key) ========== */

/**
 * Add a new log_query_ref record
 * Note: q_id is auto-generated, do not set it in the input record
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_Add(DBConnectionManager* manager, const LogQueryRef* record);

/**
 * Get a log_query_ref record by primary key (q_id)
 * @param q_id Query ID (auto_increment primary key)
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_Get(DBConnectionManager* manager,
                         unsigned short q_id,
                         LogQueryRef* record);

/**
 * Get a log_query_ref record by unique key (query_hash)
 * @param query_hash Query hash (unique key)
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_GetByHash(DBConnectionManager* manager,
                                const char* query_hash,
                                LogQueryRef* record);

/**
 * Update a log_query_ref record
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_Update(DBConnectionManager* manager, const LogQueryRef* record);

/**
 * Delete a log_query_ref record by primary key
 * @param q_id Query ID
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_Delete(DBConnectionManager* manager, unsigned short q_id);

/**
 * Delete a log_query_ref record by unique key
 * @param query_hash Query hash
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_DeleteByHash(DBConnectionManager* manager, const char* query_hash);

/**
 * Check if a log_query_ref record exists by primary key
 * @param q_id Query ID
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int LogQueryRef_Exists(DBConnectionManager* manager, unsigned short q_id);

/**
 * Check if a query_hash already exists
 * @param query_hash Query hash
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int LogQueryRef_HashExists(DBConnectionManager* manager, const char* query_hash);

/* ========== Business Operations ========== */

/**
 * Get all query references
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_GetAll(DBConnectionManager* manager,
                            LogQueryRef* records,
                            int max_count,
                            int* actual_count);

/**
 * Get the latest query reference
 * @return 0 on success, -1 on failure
 */
int LogQueryRef_GetLatest(DBConnectionManager* manager, LogQueryRef* record);

/**
 * Count total number of query references
 * @return count (>= 0) on success, -1 on failure
 */
int LogQueryRef_Count(DBConnectionManager* manager);

/* ========== Utility Functions ========== */

/**
 * Print log_query_ref record information
 */
void LogQueryRef_PrintInfo(const LogQueryRef* record);

#endif /* LOG_QUERY_REF_ORM_H */
