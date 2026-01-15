#include "database_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

// Test database configuration
#define TEST_DB_TYPE 1
#define TEST_DB_HOST "localhost"
#define TEST_DB_USER "testuser"
#define TEST_DB_PASS "testpass"
#define TEST_DB_NAME "testdb"
#define TEST_DB_POOL_SIZE 20
#define TEST_DB_TIMEOUT 30

#define NUM_THREADS 50
#define NUM_QUERIES 1000

void* run_queries(void* arg) {
    CDBConnector connector;
    QueryResult result;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    for (int i = 0; i < NUM_QUERIES; i++) {
        // Execute a simple query
        assert(CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE, "SELECT 1", &result) >= 0);
        QueryResult_Free(&result);
    }

    CDBConnector_Cleanup(&connector);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Initialize the database connector with a larger pool size
    CDBConnector connector;
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Create the test threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, run_queries, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    CDBConnector_Cleanup(&connector);
    printf("All database performance tests passed!\n");
    return 0;
}