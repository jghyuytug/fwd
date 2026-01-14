/**
 * Database Module Structure Definitions
 * Structures for database connections and queries
 */

#ifndef INCLUDE_DATABASE_STRUCTURES_H_
#define INCLUDE_DATABASE_STRUCTURES_H_

#include <common/ida_types.h>
#include <mysql/mysql.h>

/* Database connection info */
typedef struct {
    char hostname[256];
    char username[64];
    char password[64];
    char database[64];
    int port;
} DBConnConfig;

/* Connection pool entry */
typedef struct {
    MYSQL* connection;
    int is_in_use;
    time_t last_used;
    uint32_t query_count;
} DBConnection;

/* Connection pool */
typedef struct {
    DBConnection* connections;
    int pool_size;
    int available_connections;
    DBConnConfig config;
} DBConnPool;

/* Query result wrapper */
typedef struct {
    MYSQL_RES* result;
    MYSQL_ROW row;
    uint32_t num_rows;
} QueryResult;

#endif // INCLUDE_DATABASE_STRUCTURES_H_
