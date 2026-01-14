#include "headers/database_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>

// Check health of a single connection
int CDBConnector_CheckConnectionHealth(CDBConnector* self, DBConnection* conn) {
    if (!conn || !conn->mysql_conn) {
        return -1;
    }

    // Update last health check timestamp
    conn->last_health_check = time(NULL);

    // Try to ping the server
    int ping_result = mysql_ping(conn->mysql_conn);
    if (ping_result != 0) {
        // Connection is dead
        conn->is_healthy = false;
        conn->is_connected = false;

        DBConnectionPool* pool = &self->pools[conn->type];
        pool->health_check_failures++;

        fprintf(stderr, "[Database] Health check failed for connection type %d: %s\n",
                conn->type, mysql_error(conn->mysql_conn));
        return -1;
    }

    // Connection is healthy
    conn->is_healthy = true;
    conn->is_connected = true;
    return 0;
}

// Perform health check on all connections in a pool
int CDBConnector_HealthCheckAll(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    int healthy_count = 0;
    int unhealthy_count = 0;

    for (int i = 0; i < pool->pool_size; i++) {
        DBConnection* conn = &pool->connections[i];

        if (CDBConnector_CheckConnectionHealth(self, conn) == 0) {
            healthy_count++;
        } else {
            unhealthy_count++;

            // Try to reconnect unhealthy connections
            mysql_close(conn->mysql_conn);

            DBConfig* config = &self->configs[db_type];
            MYSQL* new_conn = mysql_init(NULL);
            if (new_conn && mysql_real_connect(new_conn, config->host, config->user,
                                               config->password, config->database,
                                               config->port, NULL, 0)) {
                conn->mysql_conn = new_conn;
                conn->is_connected = true;
                conn->is_healthy = true;
                conn->reconnect_count++;
                pool->auto_reconnects++;

                printf("[Database] Successfully reconnected connection type %d (index %d)\n",
                       db_type, i);
                healthy_count++;
                unhealthy_count--;
            } else {
                fprintf(stderr, "[Database] Failed to reconnect connection type %d (index %d)\n",
                        db_type, i);
            }
        }
    }

    printf("[Database] Health check for type %d: %d healthy, %d unhealthy\n",
           db_type, healthy_count, unhealthy_count);

    return healthy_count;
}

// Prewarm connection pool (create all connections upfront)
int CDBConnector_PrewarmPool(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    DBConfig* config = &self->configs[db_type];

    printf("[Database] Prewarming pool for type %d (target: %d connections)\n",
           db_type, config->pool_size);

    // Create connections up to pool_size
    int created = 0;
    for (int i = pool->pool_size; i < config->pool_size; i++) {
        // Allocate or reallocate connection array
        if (pool->connections == NULL) {
            pool->connections = malloc(config->pool_size * sizeof(DBConnection));
            if (!pool->connections) {
                fprintf(stderr, "[Database] Failed to allocate connection array\n");
                return -1;
            }
            memset(pool->connections, 0, config->pool_size * sizeof(DBConnection));
        }

        DBConnection* conn = &pool->connections[i];
        memset(conn, 0, sizeof(DBConnection));
        conn->type = db_type;
        conn->created_at = time(NULL);
        conn->last_used = conn->created_at;
        conn->is_healthy = false;
        conn->is_connected = false;
        conn->in_use = false;

        // Create MySQL connection
        MYSQL* mysql_conn = mysql_init(NULL);
        if (!mysql_conn) {
            fprintf(stderr, "[Database] Failed to initialize MySQL connection\n");
            continue;
        }

        if (!mysql_real_connect(mysql_conn, config->host, config->user, config->password,
                                config->database, config->port, NULL, 0)) {
            fprintf(stderr, "[Database] Failed to connect to database type %d: %s\n",
                    db_type, mysql_error(mysql_conn));
            mysql_close(mysql_conn);
            continue;
        }

        conn->mysql_conn = mysql_conn;
        conn->is_connected = true;
        conn->is_healthy = true;

        pool->pool_size++;
        pool->available++;
        pool->total_connections_created++;
        created++;

        // Update peak connections
        if (pool->pool_size > pool->peak_connections) {
            pool->peak_connections = pool->pool_size;
        }
    }

    printf("[Database] Prewarming complete: created %d connections, total: %d\n",
           created, pool->pool_size);

    return created;
}

// Detect connections that have exceeded timeout
int CDBConnector_DetectConnectionTimeout(CDBConnector* self, int db_type, int timeout_seconds) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    int current_time = time(NULL);
    int timeout_count = 0;

    for (int i = 0; i < pool->pool_size; i++) {
        DBConnection* conn = &pool->connections[i];

        // Skip connections currently in use
        if (conn->in_use) {
            continue;
        }

        // Check if connection has been idle for too long
        int idle_time = current_time - conn->last_used;
        if (idle_time > timeout_seconds) {
            printf("[Database] Connection type %d (index %d) timed out: idle for %d seconds\n",
                   db_type, i, idle_time);

            // Close timed out connection
            if (conn->mysql_conn) {
                mysql_close(conn->mysql_conn);
                conn->mysql_conn = NULL;
                conn->is_connected = false;
                conn->is_healthy = false;
                pool->total_connections_destroyed++;
            }

            timeout_count++;
        }
    }

    if (timeout_count > 0) {
        printf("[Database] Detected %d timed out connections for type %d\n",
               timeout_count, db_type);
    }

    return timeout_count;
}

// Detect connection leaks (connections marked as in_use for too long)
int CDBConnector_DetectConnectionLeaks(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];
    int current_time = time(NULL);
    int leak_count = 0;
    int leak_threshold = 300; // 5 minutes

    for (int i = 0; i < pool->pool_size; i++) {
        DBConnection* conn = &pool->connections[i];

        // Check if connection has been in use for too long
        if (conn->in_use) {
            int in_use_time = current_time - conn->last_used;
            if (in_use_time > leak_threshold) {
                fprintf(stderr, "[Database] WARNING: Potential connection leak detected!\n");
                fprintf(stderr, "  Type: %d, Index: %d, In use for: %d seconds\n",
                        db_type, i, in_use_time);
                fprintf(stderr, "  Query count: %d, Failed queries: %d\n",
                        conn->use_count, conn->failed_queries);
                leak_count++;
            }
        }
    }

    return leak_count;
}

// Print detailed pool metrics
void CDBConnector_PrintPoolMetrics(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return;
    }

    DBConnectionPool* pool = &self->pools[db_type];

    printf("\n=== Connection Pool Metrics for DB Type %d ===\n", db_type);
    printf("Current Status:\n");
    printf("  Pool Size: %d / %d (max)\n", pool->pool_size, pool->max_size);
    printf("  Available: %d\n", pool->available);
    printf("  In Use: %d\n", pool->in_use);
    printf("\nLifetime Statistics:\n");
    printf("  Total Created: %d\n", pool->total_connections_created);
    printf("  Total Destroyed: %d\n", pool->total_connections_destroyed);
    printf("  Peak Connections: %d\n", pool->peak_connections);
    printf("  Connection Waits: %d\n", pool->connection_wait_count);
    printf("\nHealth & Performance:\n");
    printf("  Health Check Failures: %d\n", pool->health_check_failures);
    printf("  Auto Reconnects: %d\n", pool->auto_reconnects);
    printf("  Total Query Time: %lld ms\n", pool->total_query_time);
    if (self->total_queries > 0) {
        printf("  Average Query Time: %.2f ms\n",
               (double)pool->total_query_time / self->total_queries);
    }
    printf("==========================================\n\n");
}

// Get detailed statistics
int CDBConnector_GetDetailedStats(CDBConnector* self, int db_type,
                                   int* total_created, int* total_destroyed,
                                   int* peak_connections, int* wait_count,
                                   long long* total_query_time) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];

    if (total_created) {
        *total_created = pool->total_connections_created;
    }
    if (total_destroyed) {
        *total_destroyed = pool->total_connections_destroyed;
    }
    if (peak_connections) {
        *peak_connections = pool->peak_connections;
    }
    if (wait_count) {
        *wait_count = pool->connection_wait_count;
    }
    if (total_query_time) {
        *total_query_time = pool->total_query_time;
    }

    return 0;
}
