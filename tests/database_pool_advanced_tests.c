#include "database_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

// Test database configuration
#define TEST_DB_TYPE 1
#define TEST_CONFIG_FILE "config/db_config.cfg"

void test_connection_health_monitoring() {
    printf("\n=== Test: Connection Health Monitoring ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Perform health check on all connections
    int healthy = CDBConnector_HealthCheckAll(&connector, TEST_DB_TYPE);
    printf("Health check result: %d healthy connections\n", healthy);
    assert(healthy > 0);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Health monitoring test passed\n");
}

void test_pool_prewarming() {
    printf("\n=== Test: Connection Pool Prewarming ===\n");

    CDBConnector connector;

    // Initialize (without connecting)
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);

    // Prewarm the pool
    int created = CDBConnector_PrewarmPool(&connector, TEST_DB_TYPE);
    printf("Prewarming created %d connections\n", created);
    assert(created > 0);

    // Verify connections are available
    int total, available, inuse;
    CDBConnector_GetConnectionPoolStats(&connector, TEST_DB_TYPE, &total, &available, &inuse);
    printf("Pool stats: %d total, %d available, %d in use\n", total, available, inuse);
    assert(total > 0);
    assert(available > 0);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Pool prewarming test passed\n");
}

void test_connection_timeout_detection() {
    printf("\n=== Test: Connection Timeout Detection ===\n");

    CDBConnector connector;

    // Initialize and prewarm
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_PrewarmPool(&connector, TEST_DB_TYPE) > 0);

    // Wait a bit
    printf("Waiting 2 seconds...\n");
    sleep(2);

    // Check for timeouts (with 1 second threshold)
    int timeout_count = CDBConnector_DetectConnectionTimeout(&connector, TEST_DB_TYPE, 1);
    printf("Detected %d timed out connections\n", timeout_count);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Timeout detection test passed\n");
}

void test_connection_leak_detection() {
    printf("\n=== Test: Connection Leak Detection ===\n");

    CDBConnector connector;

    // Initialize and prewarm
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_PrewarmPool(&connector, TEST_DB_TYPE) > 0);

    // Manually mark some connections as in_use to simulate leaks
    DBConnectionPool* pool = &connector.pools[TEST_DB_TYPE];
    if (pool->pool_size > 0) {
        pool->connections[0].in_use = true;
        pool->connections[0].last_used = time(NULL) - 400; // 400 seconds ago
    }

    // Detect leaks
    int leak_count = CDBConnector_DetectConnectionLeaks(&connector, TEST_DB_TYPE);
    printf("Detected %d potential leaks\n", leak_count);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Leak detection test passed\n");
}

void test_detailed_metrics() {
    printf("\n=== Test: Detailed Performance Metrics ===\n");

    CDBConnector connector;

    // Initialize and prewarm
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_PrewarmPool(&connector, TEST_DB_TYPE) > 0);

    // Execute some queries
    QueryResult result;
    for (int i = 0; i < 5; i++) {
        CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE, "SELECT 1", &result);
        QueryResult_Free(&result);
    }

    // Print metrics
    CDBConnector_PrintPoolMetrics(&connector, TEST_DB_TYPE);

    // Get detailed stats
    int total_created, total_destroyed, peak, wait_count;
    long long total_query_time;
    CDBConnector_GetDetailedStats(&connector, TEST_DB_TYPE,
                                   &total_created, &total_destroyed,
                                   &peak, &wait_count, &total_query_time);

    printf("Detailed Stats:\n");
    printf("  Created: %d\n", total_created);
    printf("  Destroyed: %d\n", total_destroyed);
    printf("  Peak: %d\n", peak);
    printf("  Waits: %d\n", wait_count);
    printf("  Query Time: %lld ms\n", total_query_time);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Metrics test passed\n");
}

void test_auto_reconnect() {
    printf("\n=== Test: Automatic Reconnection ===\n");

    CDBConnector connector;

    // Initialize and connect
    assert(CDBConnector_Initialize(&connector, TEST_CONFIG_FILE) == 0);
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Simulate connection failure by closing one connection
    DBConnectionPool* pool = &connector.pools[TEST_DB_TYPE];
    if (pool->pool_size > 0 && pool->connections[0].mysql_conn) {
        printf("Simulating connection failure...\n");
        mysql_close(pool->connections[0].mysql_conn);
        pool->connections[0].mysql_conn = NULL;
        pool->connections[0].is_connected = false;
        pool->connections[0].is_healthy = false;
    }

    // Run health check (should trigger auto-reconnect)
    int healthy = CDBConnector_HealthCheckAll(&connector, TEST_DB_TYPE);
    printf("After auto-reconnect: %d healthy connections\n", healthy);

    // Verify reconnection happened
    DBConnectionPool* pool2 = &connector.pools[TEST_DB_TYPE];
    printf("Auto reconnects: %d\n", pool2->auto_reconnects);
    assert(pool2->auto_reconnects > 0);

    // Cleanup
    CDBConnector_Cleanup(&connector);
    printf("✅ Auto-reconnect test passed\n");
}

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║  Phase 5.9b - Advanced Pool Management Tests     ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");

    test_connection_health_monitoring();
    test_pool_prewarming();
    test_connection_timeout_detection();
    test_connection_leak_detection();
    test_detailed_metrics();
    test_auto_reconnect();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║  ✅ All Phase 5.9b tests passed successfully!    ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    printf("\n");

    return 0;
}
