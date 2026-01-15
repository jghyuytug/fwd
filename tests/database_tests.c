#include "database_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Test database configuration
#define TEST_DB_TYPE 1
#define TEST_DB_HOST "localhost"
#define TEST_DB_USER "testuser"
#define TEST_DB_PASS "testpass"
#define TEST_DB_NAME "testdb"
#define TEST_DB_POOL_SIZE 5
#define TEST_DB_TIMEOUT 30

// Test query
#define TEST_QUERY "SELECT * FROM test_table"

void test_database_connection() {
    CDBConnector connector;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Check if the connection is successful
    assert(CDBConnector_IsConnected(&connector, TEST_DB_TYPE) == true);

    // Cleanup
    CDBConnector_Cleanup(&connector);
}

void test_database_query() {
    CDBConnector connector;
    QueryResult result;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Execute the test query
    assert(CDBConnector_ExecuteQuery(&connector, TEST_DB_TYPE, TEST_QUERY, &result) >= 0);

    // Check the result
    assert(result.row_count > 0);
    assert(result.column_count > 0);

    // Clean up
    QueryResult_Free(&result);
    CDBConnector_Cleanup(&connector);
}

int main() {
    test_database_connection();
    test_database_query();
    printf("All database tests passed!\n");
    return 0;
}