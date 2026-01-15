#include "database_interface.h"
#include "events_interface.h"
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

void test_events_persistence() {
    CDBConnector connector;
    CEventBase event;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Create a new event
    assert(CEventBase_Create(&event, "test_event", 1000, 100) == 0);

    // Save the event to the database
    assert(CEventBase_Save(&event, TEST_DB_TYPE) == 0);

    // Load the event from the database
    assert(CEventBase_Load(&event, TEST_DB_TYPE, "test_event") == 0);

    // Verify the event data
    assert(strcmp(event.name, "test_event") == 0);
    assert(event.start_time == 1000);
    assert(event.duration == 100);

    // Update the event
    event.start_time = 2000;
    event.duration = 200;
    assert(CEventBase_Save(&event, TEST_DB_TYPE) == 0);

    // Load the updated event
    assert(CEventBase_Load(&event, TEST_DB_TYPE, "test_event") == 0);
    assert(event.start_time == 2000);
    assert(event.duration == 200);

    // Clean up
    CEventBase_Destroy(&event);
    CDBConnector_Cleanup(&connector);
}

int main() {
    test_events_persistence();
    printf("All events DB integration tests passed!\n");
    return 0;
}