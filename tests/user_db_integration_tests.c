#include "database_interface.h"
#include "user_interface.h"
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

void test_user_create_and_load() {
    CDBConnector connector;
    CUser user;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Create a new user
    assert(CUser_Create(&user, "testuser", "password123") == 0);

    // Load the user from the database
    assert(CUser_Load(&user, TEST_DB_TYPE) == 0);

    // Verify user data
    assert(strcmp(user.username, "testuser") == 0);
    assert(strcmp(user.password, "password123") == 0);

    // Clean up
    CUser_Destroy(&user);
    CDBConnector_Cleanup(&connector);
}

void test_user_update_and_delete() {
    CDBConnector connector;
    CUser user;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Create a new user
    assert(CUser_Create(&user, "testuser2", "password456") == 0);

    // Update the user
    assert(CUser_Update(&user, "newusername", "newpassword") == 0);

    // Load the updated user
    assert(CUser_Load(&user, TEST_DB_TYPE) == 0);
    assert(strcmp(user.username, "newusername") == 0);
    assert(strcmp(user.password, "newpassword") == 0);

    // Delete the user
    assert(CUser_Delete(&user, TEST_DB_TYPE) == 0);

    // Verify the user is no longer in the database
    assert(CUser_Load(&user, TEST_DB_TYPE) != 0);

    // Clean up
    CUser_Destroy(&user);
    CDBConnector_Cleanup(&connector);
}

int main() {
    test_user_create_and_load();
    test_user_update_and_delete();
    printf("All user DB integration tests passed!\n");
    return 0;
}