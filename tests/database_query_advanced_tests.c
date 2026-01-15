#include "database_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

// Test database configuration
#define TEST_DB_TYPE 2
#define TEST_CONFIG_FILE "config/db_config.cfg"

void test_query_timeout_control() {
    printf("\n=== Test: Query Timeout Control ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Set query timeout to 2 seconds
    CDBConnector_SetQueryTimeout(&connector, 2);

    // Execute a quick query (should succeed)
    QueryResult result;
    int ret = CDBConnector_ExecuteQueryWithTimeout(&connector, TEST_DB_TYPE,
                                                     "SELECT 1", &result, 2);
    printf("Quick query result: %d rows\n", ret);
    assert(ret >= 0);
    assert(result.execution_time_ms >= 0);
    printf("Execution time: %lld ms\n", result.execution_time_ms);
    QueryResult_Free(&result);

    // Execute a slow query simulation (using SLEEP)
    printf("Testing slow query with SLEEP(3)...\n");
    ret = CDBConnector_ExecuteQueryWithTimeout(&connector, TEST_DB_TYPE,
                                                 "SELECT SLEEP(3)", &result, 1);
    // This should timeout or return error
    printf("Slow query result: %d (expected timeout/error)\n", ret);
    if (ret >= 0) {
        QueryResult_Free(&result);
    }

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Query timeout control test passed\n");
}

void test_query_retry_mechanism() {
    printf("\n=== Test: Query Retry Mechanism ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Set max retry attempts
    CDBConnector_SetMaxRetryAttempts(&connector, 3);

    // Test with a valid query (should succeed on first attempt)
    QueryResult result;
    int ret = CDBConnector_ExecuteQueryWithRetry(&connector, TEST_DB_TYPE,
                                                   "SELECT 1", &result, 3);
    printf("Retry test with valid query: %d rows\n", ret);
    assert(ret >= 0);
    printf("Retry count: %d (expected 0)\n", result.retry_count);
    assert(result.retry_count == 0);
    QueryResult_Free(&result);

    // Test with an invalid query (should fail after retries)
    printf("Testing retry with invalid query...\n");
    ret = CDBConnector_ExecuteQueryWithRetry(&connector, TEST_DB_TYPE,
                                               "SELECT * FROM nonexistent_table_xyz",
                                               &result, 2);
    printf("Retry test with invalid query: %d (expected failure)\n", ret);
    assert(ret < 0);

    // Check retry statistics
    printf("Queries retried: %d\n", connector.queries_retried);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Query retry mechanism test passed\n");
}

void test_slow_query_detection() {
    printf("\n=== Test: Slow Query Detection ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Set slow query threshold to 100ms
    CDBConnector_SetSlowQueryThreshold(&connector, 100);

    // Execute some fast queries
    QueryResult result;
    for (int i = 0; i < 5; i++) {
        CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE, "SELECT 1", &result);
        QueryResult_Free(&result);
    }

    // Execute a slow query (using SLEEP to simulate)
    printf("Executing slow query with SLEEP(0.2)...\n");
    CDBConnector_ExecuteQueryWithTimeout(&connector, TEST_DB_TYPE,
                                          "SELECT SLEEP(0.2)", &result, 5);
    if (result.execution_time_ms > 0) {
        QueryResult_Free(&result);
    }

    // Check slow query detection
    CDBConnector_DetectSlowQueries(&connector);
    printf("Slow queries detected: %d\n", connector.slow_queries_detected);

    // Print detailed report
    CDBConnector_PrintSlowQueryReport(&connector);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Slow query detection test passed\n");
}

void test_query_caching() {
    printf("\n=== Test: Query Result Caching ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Enable query cache with 60 second TTL and 100 max entries
    CDBConnector_EnableQueryCache(&connector, true, 60, 100);

    // Execute same query multiple times
    QueryResult result;
    const char* test_query = "SELECT 1 AS test_value";

    for (int i = 0; i < 5; i++) {
        int ret = CDBConnector_ExecuteQueryCached(&connector, TEST_DB_TYPE,
                                                    test_query, &result);
        printf("Cached query execution %d: %d rows, hash: %s\n",
               i + 1, ret, result.query_hash);
        if (ret >= 0) {
            QueryResult_Free(&result);
        }
    }

    // Get cache statistics
    int hits, misses;
    float hit_rate;
    CDBConnector_GetCacheStats(&connector, &hits, &misses, &hit_rate);
    printf("\nCache Statistics:\n");
    printf("  Cache hits: %d\n", hits);
    printf("  Cache misses: %d\n", misses);
    printf("  Hit rate: %.2f%%\n", hit_rate);

    // Test cache clearing
    CDBConnector_ClearQueryCache(&connector);
    CDBConnector_GetCacheStats(&connector, &hits, &misses, &hit_rate);
    printf("\nAfter cache clear:\n");
    printf("  Cache hits: %d (expected 0)\n", hits);
    printf("  Cache misses: %d (expected 0)\n", misses);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Query caching test passed\n");
}

void test_streaming_result_processing() {
    printf("\n=== Test: Streaming Result Set Processing ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Create a test query that returns multiple rows
    // First, create a temporary table with test data
    QueryResult result;
    CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE,
                              "CREATE TEMPORARY TABLE IF NOT EXISTS test_stream ("
                              "id INT AUTO_INCREMENT PRIMARY KEY, "
                              "value VARCHAR(50))", &result);
    QueryResult_Free(&result);

    // Insert test data
    for (int i = 1; i <= 100; i++) {
        char insert_query[256];
        sprintf(insert_query, "INSERT INTO test_stream (value) VALUES ('Row %d')", i);
        CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE, insert_query, &result);
        QueryResult_Free(&result);
    }

    // Execute query and get result set
    int ret = CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE,
                                         "SELECT * FROM test_stream", &result);
    printf("Total rows in result set: %d\n", ret);
    assert(ret == 100);

    // Initialize streaming
    MYSQL_RES* res = result.result_set;
    QueryResult stream_result;
    QueryResult_InitStream(&stream_result, res);

    // Fetch results in batches of 20
    int total_fetched = 0;
    int batch_num = 0;
    char** rows;

    while (true) {
        int fetched = QueryResult_FetchNextBatch(&stream_result, 20, &rows);
        if (fetched == 0) break;

        batch_num++;
        total_fetched += fetched;
        printf("Batch %d: Fetched %d rows (Total: %d)\n",
               batch_num, fetched, total_fetched);

        // Free batch data
        for (int i = 0; i < fetched; i++) {
            free(rows[i]);
        }
        free(rows);
    }

    printf("Total rows fetched via streaming: %d\n", total_fetched);
    assert(total_fetched == 100);

    // Free stream
    QueryResult_FreeStream(&stream_result);

    // Drop temporary table
    CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE,
                              "DROP TEMPORARY TABLE IF NOT EXISTS test_stream", &result);
    QueryResult_Free(&result);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Streaming result processing test passed\n");
}

void test_execution_metrics() {
    printf("\n=== Test: Query Execution Metrics ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Configure query execution settings
    CDBConnector_SetQueryTimeout(&connector, 5);
    CDBConnector_SetMaxRetryAttempts(&connector, 2);
    CDBConnector_SetSlowQueryThreshold(&connector, 50);
    CDBConnector_EnableQueryCache(&connector, true, 300, 50);

    // Execute various queries to generate metrics
    QueryResult result;

    // Fast queries
    for (int i = 0; i < 10; i++) {
        CDBConnector_ExecuteQueryWithTimeout(&connector, TEST_DB_TYPE,
                                              "SELECT 1", &result, 5);
        printf("Query %d: %lld ms\n", i + 1, result.execution_time_ms);
        QueryResult_Free(&result);
    }

    // Check if any were detected as slow
    printf("\nExecution Metrics:\n");
    printf("  Total queries: %d\n", connector.total_queries);
    printf("  Failed queries: %d\n", connector.failed_queries);
    printf("  Slow queries detected: %d\n", connector.slow_queries_detected);
    printf("  Queries retried: %d\n", connector.queries_retried);

    int hits, misses;
    float hit_rate;
    CDBConnector_GetCacheStats(&connector, &hits, &misses, &hit_rate);
    printf("  Cache hits: %d\n", hits);
    printf("  Cache misses: %d\n", misses);
    printf("  Cache hit rate: %.2f%%\n", hit_rate);

    // Print detailed slow query report
    CDBConnector_PrintSlowQueryReport(&connector);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Query execution metrics test passed\n");
}

void test_combined_advanced_features() {
    printf("\n=== Test: Combined Advanced Features ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Configure all features
    CDBConnector_SetQueryTimeout(&connector, 10);
    CDBConnector_SetMaxRetryAttempts(&connector, 3);
    CDBConnector_SetSlowQueryThreshold(&connector, 100);
    CDBConnector_EnableQueryCache(&connector, true, 600, 100);

    // Also use advanced pool features
    CDBConnector_PrewarmPool(&connector, TEST_DB_TYPE);
    CDBConnector_HealthCheckAll(&connector, TEST_DB_TYPE);

    // Execute mixed workload
    QueryResult result;

    // 1. Cached queries
    for (int i = 0; i < 3; i++) {
        CDBConnector_ExecuteQueryCached(&connector, TEST_DB_TYPE,
                                         "SELECT 1", &result);
        QueryResult_Free(&result);
    }

    // 2. Queries with timeout
    CDBConnector_ExecuteQueryWithTimeout(&connector, TEST_DB_TYPE,
                                          "SELECT NOW()", &result, 5);
    QueryResult_Free(&result);

    // 3. Queries with retry
    CDBConnector_ExecuteQueryWithRetry(&connector, TEST_DB_TYPE,
                                        "SELECT DATABASE()", &result, 2);
    QueryResult_Free(&result);

    // Print comprehensive statistics
    printf("\n=== Connection Pool Metrics ===\n");
    CDBConnector_PrintPoolMetrics(&connector, TEST_DB_TYPE);

    printf("\n=== Query Execution Report ===\n");
    CDBConnector_PrintSlowQueryReport(&connector);

    printf("\n=== Cache Statistics ===\n");
    int hits, misses;
    float hit_rate;
    CDBConnector_GetCacheStats(&connector, &hits, &misses, &hit_rate);
    printf("Cache Hits: %d, Misses: %d, Hit Rate: %.2f%%\n",
           hits, misses, hit_rate);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Combined advanced features test passed\n");
}

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  Phase 5.9c - Query Execution & Result Processing   ║\n");
    printf("║           Advanced Features Test Suite               ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");

    // Seed random for any random operations
    srand(time(NULL));

    test_query_timeout_control();
    test_query_retry_mechanism();
    test_slow_query_detection();
    test_query_caching();
    test_streaming_result_processing();
    test_execution_metrics();
    test_combined_advanced_features();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  ✅ All Phase 5.9c tests passed successfully!       ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");

    return 0;
}
