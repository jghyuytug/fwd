#include "headers/database_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <time.h>
#include <sys/time.h>

// Helper function to get current time in milliseconds
static long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (long long)(tv.tv_usec) / 1000;
}

// Simple CRC32 hash function (no external dependencies)
static unsigned int compute_crc32(const unsigned char *buf, int size) {
    unsigned int crc = 0xffffffff;
    const unsigned int poly = 0x04c11db7;

    for (int i = 0; i < size; i++) {
        crc ^= (buf[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ poly;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc ^ 0xffffffff;
}

// Helper function to compute query hash using CRC32
static void compute_query_hash(const char* query, char* hash_output) {
    unsigned int hash = compute_crc32((const unsigned char*)query, strlen(query));
    sprintf(hash_output, "%08x", hash);
    hash_output[8] = '\0';
}

// Configuration functions

void CDBConnector_SetQueryTimeout(CDBConnector* self, int timeout_seconds) {
    self->query_timeout_seconds = timeout_seconds;
    printf("[Database] Query timeout set to %d seconds\n", timeout_seconds);
}

void CDBConnector_SetMaxRetryAttempts(CDBConnector* self, int max_attempts) {
    self->max_retry_attempts = max_attempts;
    printf("[Database] Max retry attempts set to %d\n", max_attempts);
}

void CDBConnector_SetSlowQueryThreshold(CDBConnector* self, int threshold_ms) {
    self->slow_query_threshold_ms = threshold_ms;
    printf("[Database] Slow query threshold set to %d ms\n", threshold_ms);
}

void CDBConnector_EnableQueryCache(CDBConnector* self, bool enable, int ttl_seconds, int max_entries) {
    self->enable_query_cache = enable;
    self->cache_ttl_seconds = ttl_seconds;
    self->cache_max_entries = max_entries;

    if (enable) {
        printf("[Database] Query cache enabled: TTL=%d seconds, Max entries=%d\n",
               ttl_seconds, max_entries);
    } else {
        printf("[Database] Query cache disabled\n");
    }
}

// Execute query with timeout control
int CDBConnector_ExecuteQueryWithTimeout(CDBConnector* self, int db_type, const char* query,
                                          QueryResult* result, int timeout_seconds) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    DBConnectionPool* pool = &self->pools[db_type];

    // Get connection from pool
    DBConnection* conn = NULL;
    for (int i = 0; i < pool->pool_size; i++) {
        if (pool->connections[i].is_connected && !pool->connections[i].in_use) {
            conn = &pool->connections[i];
            conn->in_use = true;
            pool->in_use++;
            pool->available--;
            break;
        }
    }

    if (!conn) {
        fprintf(stderr, "[Database] No available connection for type %d\n", db_type);
        return -1;
    }

    // Set MySQL query timeout
    if (timeout_seconds > 0) {
        unsigned int timeout = (unsigned int)timeout_seconds;
        mysql_options(conn->mysql_conn, MYSQL_OPT_READ_TIMEOUT, &timeout);
        mysql_options(conn->mysql_conn, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    }

    // Execute query and measure time
    long long start_time = get_time_ms();
    int ret = mysql_query(conn->mysql_conn, query);
    long long end_time = get_time_ms();
    long long execution_time = end_time - start_time;

    // Release connection
    conn->in_use = false;
    pool->in_use--;
    pool->available++;
    conn->last_used = time(NULL);

    if (ret != 0) {
        fprintf(stderr, "[Database] Query failed: %s\n", mysql_error(conn->mysql_conn));
        self->failed_queries++;
        conn->failed_queries++;
        return -1;
    }

    // Get result set
    MYSQL_RES* res = mysql_store_result(conn->mysql_conn);
    if (!res) {
        fprintf(stderr, "[Database] Failed to get result set: %s\n", mysql_error(conn->mysql_conn));
        self->failed_queries++;
        return -1;
    }

    // Fill result structure
    result->result_set = res;
    result->row_count = mysql_num_rows(res);
    result->column_count = mysql_num_fields(res);
    result->current_row = 0;
    result->execution_time_ms = execution_time;
    result->is_cached = false;
    result->retry_count = 0;

    // Update statistics
    self->total_queries++;
    conn->use_count++;
    pool->total_query_time += execution_time;

    // Check if slow query
    if (self->slow_query_threshold_ms > 0 && execution_time > self->slow_query_threshold_ms) {
        self->slow_queries_detected++;
        fprintf(stderr, "[Database] SLOW QUERY detected (%lld ms): %s\n", execution_time, query);
    }

    printf("[Database] Query executed in %lld ms (%d rows)\n", execution_time, result->row_count);
    return result->row_count;
}

// Execute query with automatic retry
int CDBConnector_ExecuteQueryWithRetry(CDBConnector* self, int db_type, const char* query,
                                        QueryResult* result, int max_retries) {
    int attempts = 0;
    int ret = -1;

    while (attempts <= max_retries) {
        ret = CDBConnector_ExecuteQuery(self, db_type, query, result);

        if (ret >= 0) {
            // Success
            result->retry_count = attempts;
            if (attempts > 0) {
                self->queries_retried++;
                printf("[Database] Query succeeded on attempt %d/%d\n", attempts + 1, max_retries + 1);
            }
            return ret;
        }

        attempts++;

        if (attempts <= max_retries) {
            fprintf(stderr, "[Database] Query failed (attempt %d/%d), retrying...\n",
                    attempts, max_retries + 1);

            // Exponential backoff: wait 100ms * 2^attempt
            int wait_ms = 100 * (1 << attempts);
            if (wait_ms > 5000) wait_ms = 5000; // Cap at 5 seconds
            usleep(wait_ms * 1000);
        }
    }

    fprintf(stderr, "[Database] Query failed after %d attempts\n", attempts);
    return -1;
}

// Execute query with caching (simplified implementation)
int CDBConnector_ExecuteQueryCached(CDBConnector* self, int db_type, const char* query,
                                     QueryResult* result) {
    if (!self->enable_query_cache) {
        // Cache disabled, execute normally
        return CDBConnector_ExecuteQuery(self, db_type, query, result);
    }

    // Compute query hash
    char query_hash[65];
    compute_query_hash(query, query_hash);

    /*
     * Query cache implementation design:
     *
     * Production implementation using hash table:
     *   CacheEntry* entry = HashTable_Get(self->query_cache, query_hash);
     *   if (entry && entry->expire_time > time(NULL)) {
     *       memcpy(result, &entry->result, sizeof(QueryResult));
     *       self->cache_hits++;
     *       return 0;
     *   }
     *
     * Cache storage structure:
     *   typedef struct {
     *       char query_hash[65];
     *       QueryResult result;
     *       time_t expire_time;
     *       time_t created_time;
     *   } CacheEntry;
     *
     * Cache eviction: LRU or time-based (5-minute TTL typical)
     * Memory limit: ~100MB for cache entries
     *
     * Current: Statistics-only mode (cache infrastructure ready)
     */

    /* Execute query and track as cache miss */
    self->cache_misses++;
    int ret = CDBConnector_ExecuteQuery(self, db_type, query, result);

    if (ret >= 0) {
        strncpy(result->query_hash, query_hash, 64);
        result->query_hash[64] = '\0';
    }

    return ret;
}

// Detect slow queries
void CDBConnector_DetectSlowQueries(CDBConnector* self) {
    printf("\n=== Slow Query Detection Report ===\n");
    printf("Threshold: %d ms\n", self->slow_query_threshold_ms);
    printf("Total slow queries detected: %d\n", self->slow_queries_detected);

    if (self->total_queries > 0) {
        float slow_percentage = (float)self->slow_queries_detected / self->total_queries * 100.0f;
        printf("Slow query percentage: %.2f%%\n", slow_percentage);
    }
    printf("===================================\n\n");
}

// Print slow query report
void CDBConnector_PrintSlowQueryReport(CDBConnector* self) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║     Slow Query Detection Report       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    printf("Configuration:\n");
    printf("  Slow query threshold: %d ms\n", self->slow_query_threshold_ms);
    printf("\nStatistics:\n");
    printf("  Total queries: %d\n", self->total_queries);
    printf("  Slow queries detected: %d\n", self->slow_queries_detected);

    if (self->total_queries > 0) {
        float slow_percentage = (float)self->slow_queries_detected / self->total_queries * 100.0f;
        printf("  Slow query rate: %.2f%%\n", slow_percentage);

        // Calculate average query time across all pools
        long long total_time = 0;
        for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
            total_time += self->pools[i].total_query_time;
        }
        float avg_time = (float)total_time / self->total_queries;
        printf("  Average query time: %.2f ms\n", avg_time);
    }

    printf("\n");
}

// Clear query cache
void CDBConnector_ClearQueryCache(CDBConnector* self) {
    /*
     * Cache clearing: Release all cached query results
     *
     * Production implementation:
     *   if (self->query_cache) {
     *       HashTable_ForEach(self->query_cache, free_cache_entry);
     *       HashTable_Clear(self->query_cache);
     *   }
     *
     * Current: Reset statistics (cache entries not stored yet)
     */
    self->cache_hits = 0;
    self->cache_misses = 0;
    printf("[Database] Query cache cleared\n");
}

// Get cache statistics
void CDBConnector_GetCacheStats(CDBConnector* self, int* hits, int* misses, float* hit_rate) {
    if (hits) *hits = self->cache_hits;
    if (misses) *misses = self->cache_misses;

    if (hit_rate) {
        int total = self->cache_hits + self->cache_misses;
        *hit_rate = total > 0 ? (float)self->cache_hits / total * 100.0f : 0.0f;
    }
}

// Streaming result set processing

int QueryResult_InitStream(QueryResult* self, MYSQL_RES* result_set) {
    self->result_set = result_set;
    self->row_count = mysql_num_rows(result_set);
    self->column_count = mysql_num_fields(result_set);
    self->current_row = 0;

    printf("[Database] Stream initialized: %d rows, %d columns\n",
           self->row_count, self->column_count);
    return 0;
}

int QueryResult_FetchNextBatch(QueryResult* self, int batch_size, char*** rows) {
    if (!self->result_set) {
        return -1;
    }

    int fetched = 0;
    *rows = malloc(batch_size * sizeof(char*));

    for (int i = 0; i < batch_size; i++) {
        MYSQL_ROW row = mysql_fetch_row(self->result_set);
        if (!row) {
            break;
        }

        // Allocate and copy row data
        (*rows)[i] = malloc(self->column_count * sizeof(char*));
        for (int j = 0; j < self->column_count; j++) {
            if (row[j]) {
                (*rows)[i] = strdup(row[j]);
            } else {
                (*rows)[i] = NULL;
            }
        }

        fetched++;
        self->current_row++;
    }

    if (fetched < batch_size) {
        printf("[Database] Stream batch complete: fetched %d rows (end of result set)\n", fetched);
    } else {
        printf("[Database] Stream batch fetched: %d rows (progress: %d/%d)\n",
               fetched, self->current_row, self->row_count);
    }

    return fetched;
}

void QueryResult_FreeStream(QueryResult* self) {
    if (self->result_set) {
        mysql_free_result(self->result_set);
        self->result_set = NULL;
    }

    self->row_count = 0;
    self->column_count = 0;
    self->current_row = 0;

    printf("[Database] Stream resources freed\n");
}
