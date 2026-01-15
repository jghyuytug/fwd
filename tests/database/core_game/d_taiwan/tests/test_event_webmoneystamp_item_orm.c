#include "event_webmoneystamp_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test functions */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

/* Test statistics */
static int test_passed = 0;
static int test_failed = 0;
static int test_total = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        test_total++; \
        if (condition) { \
            printf("[PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("[FAIL] %s\n", message); \
            test_failed++; \
        } \
    } while(0)

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Event Webmoneystamp Item ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n", test_total, test_passed, test_failed);
    printf("Success Rate: %.1f%%\n", test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    EventWebmoneystampItem item, retrieved;
    int ret;
    int test_time = (int)time(NULL);

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add new item */
    memset(&item, 0, sizeof(EventWebmoneystampItem));
    item.m_id = 100001;
    item.occ_time = test_time;
    item.server_id = 1;
    item.charac_no = 5;
    item.item_no = 90001;
    item.item_check = 12345;

    ret = EventWebmoneystampItem_Add(manager, &item);
    TEST_ASSERT(ret == 0, "Add new webmoneystamp item");

    /* Test 2: Check existence */
    ret = EventWebmoneystampItem_Exists(manager, 100001, test_time);
    TEST_ASSERT(ret == 1, "Item exists after add");

    /* Test 3: Get item */
    ret = EventWebmoneystampItem_Get(manager, 100001, test_time, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.m_id == 100001, "Get item by composite key");
    TEST_ASSERT(retrieved.occ_time == test_time, "Verify occ_time");
    TEST_ASSERT(retrieved.item_no == 90001, "Verify item_no");
    TEST_ASSERT(retrieved.item_check == 12345, "Verify item_check");

    /* Test 4: Update item */
    item.item_no = 90002;
    item.item_check = 54321;
    ret = EventWebmoneystampItem_Update(manager, &item);
    TEST_ASSERT(ret == 0, "Update item");

    ret = EventWebmoneystampItem_Get(manager, 100001, test_time, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.item_no == 90002, "Verify updated item_no");
    TEST_ASSERT(retrieved.item_check == 54321, "Verify updated item_check");

    /* Test 5: Delete item */
    ret = EventWebmoneystampItem_Delete(manager, 100001, test_time);
    TEST_ASSERT(ret == 0, "Delete item");

    ret = EventWebmoneystampItem_Exists(manager, 100001, test_time);
    TEST_ASSERT(ret == 0, "Item does not exist after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventWebmoneystampItem item;
    EventWebmoneystampItem items[10];
    int actual_count = 0;
    int ret;
    int base_time = (int)time(NULL);

    printf("--- Test: Business Operations ---\n");

    /* Add test data - same member, different times */
    for (int i = 0; i < 3; i++) {
        memset(&item, 0, sizeof(EventWebmoneystampItem));
        item.m_id = 200001;
        item.occ_time = base_time + i * 100;
        item.server_id = 2;
        item.charac_no = 5;
        item.item_no = 90010 + i;
        item.item_check = 1000 + i;
        EventWebmoneystampItem_Add(manager, &item);
    }

    /* Test 1: Get by member (should return 3 items) */
    ret = EventWebmoneystampItem_GetByMember(manager, 200001, items, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all items for a member");

    /* Add items with same character on same server */
    for (int i = 0; i < 2; i++) {
        memset(&item, 0, sizeof(EventWebmoneystampItem));
        item.m_id = 200002 + i;
        item.occ_time = base_time + 1000 + i;
        item.server_id = 2;
        item.charac_no = 5;
        item.item_no = 90020 + i;
        item.item_check = 2000 + i;
        EventWebmoneystampItem_Add(manager, &item);
    }

    /* Test 2: Get by character (should return 5 items for charac 5 on server 2) */
    ret = EventWebmoneystampItem_GetByCharac(manager, 2, 5, items, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 5, "Get all items for a character");

    /* Add item with specific item_no */
    memset(&item, 0, sizeof(EventWebmoneystampItem));
    item.m_id = 200004;
    item.occ_time = base_time + 2000;
    item.server_id = 3;
    item.charac_no = 6;
    item.item_no = 90010;
    item.item_check = 3000;
    EventWebmoneystampItem_Add(manager, &item);

    /* Test 3: Get by item_no (should return 2 items with item_no 90010) */
    ret = EventWebmoneystampItem_GetByItemNo(manager, 90010, items, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Get all items with specific item_no");

    /* Test 4: Delete by member (should delete all 3 items for member 200001) */
    ret = EventWebmoneystampItem_DeleteByMember(manager, 200001);
    TEST_ASSERT(ret == 0, "Delete all items for a member");

    ret = EventWebmoneystampItem_GetByMember(manager, 200001, items, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 0, "Verify member items deleted");

    /* Now character should have 2 items left (200002 and 200003) */
    ret = EventWebmoneystampItem_GetByCharac(manager, 2, 5, items, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Verify character items count after delete");

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_webmoneystamp_item WHERE m_id >= 100000");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
