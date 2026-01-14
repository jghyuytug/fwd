#ifndef LOG_QUERY_DBMW_REF_ORM_H
#define LOG_QUERY_DBMW_REF_ORM_H

#include "db_connection_manager.h"

/**
 * LogQueryDbmwRef - Query reference for DBMW (Database Middleware)
 * Maps to taiwan_login.log_query_dbmw_ref table
 * Primary Key: q_id (auto_increment)
 * Unique Key: query_hash
 */
typedef struct {
    char query_hash[17];       /* VARCHAR(16) - UNIQUE KEY */
    unsigned short q_id;       /* SMALLINT UNSIGNED - AUTO_INCREMENT PRIMARY KEY */
    char query[4096];          /* TEXT field (max 4KB) */
} LogQueryDbmwRef;

/* ========== CRUD Operations (AUTO_INCREMENT Primary Key) ========== */

/**
 * Add a new log_query_dbmw_ref record
 * Note: q_id is auto-generated, do not set it in the input record
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_Add(DBConnectionManager* manager, const LogQueryDbmwRef* record);

/**
 * Get a log_query_dbmw_ref record by primary key (q_id)
 * @param q_id Query ID (auto_increment primary key)
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_Get(DBConnectionManager* manager,
                         unsigned short q_id,
                         LogQueryDbmwRef* record);

/**
 * Get a log_query_dbmw_ref record by unique key (query_hash)
 * @param query_hash Query hash (unique key)
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_GetByHash(DBConnectionManager* manager,
                                const char* query_hash,
                                LogQueryDbmwRef* record);

/**
 * Update a log_query_dbmw_ref record
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_Update(DBConnectionManager* manager, const LogQueryDbmwRef* record);

/**
 * Delete a log_query_dbmw_ref record by primary key
 * @param q_id Query ID
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_Delete(DBConnectionManager* manager, unsigned short q_id);

/**
 * Delete a log_query_dbmw_ref record by unique key
 * @param query_hash Query hash
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_DeleteByHash(DBConnectionManager* manager, const char* query_hash);

/**
 * Check if a log_query_dbmw_ref record exists by primary key
 * @param q_id Query ID
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int LogQueryDbmwRef_Exists(DBConnectionManager* manager, unsigned short q_id);

/**
 * Check if a query_hash already exists
 * @param query_hash Query hash
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int LogQueryDbmwRef_HashExists(DBConnectionManager* manager, const char* query_hash);

/* ========== Business Operations ========== */

/**
 * Get all query references
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_GetAll(DBConnectionManager* manager,
                            LogQueryDbmwRef* records,
                            int max_count,
                            int* actual_count);

/**
 * Get the latest query reference
 * @return 0 on success, -1 on failure
 */
int LogQueryDbmwRef_GetLatest(DBConnectionManager* manager, LogQueryDbmwRef* record);

/**
 * Count total number of query references
 * @return count (>= 0) on success, -1 on failure
 */
int LogQueryDbmwRef_Count(DBConnectionManager* manager);

/* ========== Utility Functions ========== */

/**
 * Print log_query_dbmw_ref record information
 */
void LogQueryDbmwRef_PrintInfo(const LogQueryDbmwRef* record);

#endif /* LOG_QUERY_DBMW_REF_ORM_H */
