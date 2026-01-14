/**
 * Database Module - Public Interface
 * Purpose: MySQL database access layer for game server
 *
 * Components:
 * - CDBConnector: Connection management
 * - MySQL: Query execution wrapper
 * - DBMgr: Connection pool and database manager
 * - CQueryCounter: Query statistics and monitoring
 */

#ifndef SRC_DATABASE_DATABASE_INTERFACE_H_
#define SRC_DATABASE_DATABASE_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>
#include <database/headers/CDBConnector.h>

/**
 * Database Module Initialization
 *
 * Initializes:
 * - Connection pool
 * - MySQL connections (account, character, game, log databases)
 * - Query counter
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Database_Initialize(const char *config_path);

/**
 * Database Module Cleanup
 *
 * Properly closes all connections and frees resources
 */
void Database_Cleanup();

/**
 * Execute a query and get result
 *
 * Parameters:
 *   query: SQL query string
 *   result: Output buffer for result rows
 *
 * Returns: Number of rows returned, or error code if negative
 */
int Database_Query(const char *query, void **result);

/**
 * Get a connection from the pool
 *
 * Returns: Connection handle, or NULL on failure
 */
void* Database_GetConnection(int connection_id);

/**
 * Return connection to pool
 *
 * Returns: 0 on success
 */
int Database_ReleaseConnection(void *conn);

/**
 * Get query statistics
 *
 * Returns: Total number of queries executed
 */
unsigned int Database_GetQueryCount();

/**
 * Reset query counter
 */
void Database_ResetQueryCounter();

/**
 * Check if database is connected
 *
 * Returns: 1 if connected, 0 if not
 */
int Database_IsConnected();

/**
 * Database connection types
 */
typedef enum {
    DB_ACCOUNT = 0,   /* Account database */
    DB_CHARACTER = 1, /* Character database */
    DB_GAME = 2,      /* Game data database */
    DB_LOG = 3,       /* Audit/event log database */
} DatabaseType;

/**
 * Get specific database connection
 */
void* Database_GetTypedConnection(DatabaseType db_type);

#endif // SRC_DATABASE_DATABASE_INTERFACE_H_
