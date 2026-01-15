#include "database_interface.h"
#include "inventory_interface.h"
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

void test_inventory_basic_operations() {
    CDBConnector connector;
    CInventory inventory;

    // Initialize the database connector
    assert(CDBConnector_Initialize(&connector, "db_config.cfg") == 0);

    // Connect to the test database
    assert(CDBConnector_Connect(&connector, TEST_DB_TYPE) == 0);

    // Initialize the inventory
    assert(CInventory_Initialize(&inventory, TEST_DB_TYPE) == 0);

    // Add some items to the inventory
    assert(CInventory_AddItem(&inventory, "item1", 10) == 0);
    assert(CInventory_AddItem(&inventory, "item2", 5) == 0);

    // Verify the items were added correctly
    assert(CInventory_GetItemCount(&inventory, "item1") == 10);
    assert(CInventory_GetItemCount(&inventory, "item2") == 5);

    // Update the item counts
    assert(CInventory_UpdateItem(&inventory, "item1", 20) == 0);
    assert(CInventory_UpdateItem(&inventory, "item2", 0) == 0);

    // Verify the updates
    assert(CInventory_GetItemCount(&inventory, "item1") == 20);
    assert(CInventory_GetItemCount(&inventory, "item2") == 0);

    // Remove an item
    assert(CInventory_RemoveItem(&inventory, "item1", 5) == 0);
    assert(CInventory_GetItemCount(&inventory, "item1") == 15);

    // Clean up
    CInventory_Destroy(&inventory);
    CDBConnector_Cleanup(&connector);
}

int main() {
    test_inventory_basic_operations();
    printf("All inventory DB integration tests passed!\n");
    return 0;
}