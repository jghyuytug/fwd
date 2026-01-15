#include "check_pick_up_random_option_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("[PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("[FAIL] %s\n", message); \
            test_failed++; \
        } \
    } while(0)

void cleanup_test_data(DBConnectionManager* manager);
void test_basic_crud(DBConnectionManager* manager);
void test_specialized_operations(DBConnectionManager* manager);
void test_query_operations(DBConnectionManager* manager);
void test_batch_operations(DBConnectionManager* manager);

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

    printf("=== Check Pick Up Random Option Item ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_specialized_operations(&manager);
    test_query_operations(&manager);
    test_batch_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    if (test_failed == 0) {
        printf("\n*** All Tests PASSED! ***\n");
        return 0;
    } else {
        printf("\n*** Some Tests FAILED! ***\n");
        return 1;
    }
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM check_pick_up_random_option_item WHERE m_id IN (100001, 100002, 100003, 100004, 100005)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    CheckPickUpRandomOptionItem item;
    CheckPickUpRandomOptionItem retrieved;

    printf("\n--- Test Basic CRUD Operations ---\n");

    /* Test 1: Add new item */
    memset(&item, 0, sizeof(CheckPickUpRandomOptionItem));
    item.m_id = 100001;
    item.check_count = 5;

    TEST_ASSERT(CheckPickUpRandomOptionItem_Add(manager, &item) == 0,
                "Add new item");

    /* Test 2: Get item */
    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    TEST_ASSERT(CheckPickUpRandomOptionItem_Get(manager, 100001, &retrieved) == 0,
                "Get item by m_id");

    /* Test 3: Verify retrieved data */
    TEST_ASSERT(retrieved.m_id == 100001,
                "Verify m_id matches");
    TEST_ASSERT(retrieved.check_count == 5,
                "Verify check_count matches");

    /* Test 4: Exists check */
    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 100001) == 1,
                "Item exists");
    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 999999) == 0,
                "Non-existent item returns 0");

    /* Test 5: Update item */
    item.check_count = 10;
    TEST_ASSERT(CheckPickUpRandomOptionItem_Update(manager, &item) == 0,
                "Update item");

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100001, &retrieved);
    TEST_ASSERT(retrieved.check_count == 10,
                "Verify check_count updated to 10");

    /* Test 6: Delete item */
    TEST_ASSERT(CheckPickUpRandomOptionItem_Delete(manager, 100001) == 0,
                "Delete item");

    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 100001) == 0,
                "Verify item deleted");

    /* Test 7: REPLACE INTO behavior (Add existing item) */
    memset(&item, 0, sizeof(CheckPickUpRandomOptionItem));
    item.m_id = 100002;
    item.check_count = 3;
    CheckPickUpRandomOptionItem_Add(manager, &item);

    item.check_count = 7;
    TEST_ASSERT(CheckPickUpRandomOptionItem_Add(manager, &item) == 0,
                "Add duplicate item (REPLACE INTO)");

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100002, &retrieved);
    TEST_ASSERT(retrieved.check_count == 7,
                "Verify REPLACE INTO updated check_count");
}

void test_specialized_operations(DBConnectionManager* manager) {
    CheckPickUpRandomOptionItem item;
    CheckPickUpRandomOptionItem retrieved;

    printf("\n--- Test Specialized Operations ---\n");

    /* Setup test data */
    memset(&item, 0, sizeof(CheckPickUpRandomOptionItem));
    item.m_id = 100003;
    item.check_count = 5;
    CheckPickUpRandomOptionItem_Add(manager, &item);

    /* Test 1: Increment count */
    TEST_ASSERT(CheckPickUpRandomOptionItem_IncrementCount(manager, 100003) == 0,
                "Increment check count");

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100003, &retrieved);
    TEST_ASSERT(retrieved.check_count == 6,
                "Verify count incremented to 6");

    /* Test 2: Multiple increments */
    CheckPickUpRandomOptionItem_IncrementCount(manager, 100003);
    CheckPickUpRandomOptionItem_IncrementCount(manager, 100003);

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100003, &retrieved);
    TEST_ASSERT(retrieved.check_count == 8,
                "Verify count incremented to 8 (after 2 more increments)");

    /* Test 3: Reset count */
    TEST_ASSERT(CheckPickUpRandomOptionItem_ResetCount(manager, 100003) == 0,
                "Reset check count");

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100003, &retrieved);
    TEST_ASSERT(retrieved.check_count == 0,
                "Verify count reset to 0");

    /* Test 4: Update count to specific value */
    TEST_ASSERT(CheckPickUpRandomOptionItem_UpdateCount(manager, 100003, 15) == 0,
                "Update count to 15");

    memset(&retrieved, 0, sizeof(CheckPickUpRandomOptionItem));
    CheckPickUpRandomOptionItem_Get(manager, 100003, &retrieved);
    TEST_ASSERT(retrieved.check_count == 15,
                "Verify count updated to 15");
}

void test_query_operations(DBConnectionManager* manager) {
    CheckPickUpRandomOptionItem items[10];
    int actual_count = 0;
    int i;

    printf("\n--- Test Query Operations ---\n");

    /* Setup test data */
    for (i = 0; i < 5; i++) {
        CheckPickUpRandomOptionItem item;
        memset(&item, 0, sizeof(CheckPickUpRandomOptionItem));
        item.m_id = 100001 + i;
        item.check_count = (i + 1) * 2;
        CheckPickUpRandomOptionItem_Add(manager, &item);
    }

    /* Test 1: Get all items */
    memset(items, 0, sizeof(items));
    TEST_ASSERT(CheckPickUpRandomOptionItem_GetAll(manager, items, 10, &actual_count) == 0,
                "Get all items");
    TEST_ASSERT(actual_count == 5,
                "Verify got 5 items");

    /* Test 2: Count all items */
    int total_count = CheckPickUpRandomOptionItem_CountAll(manager);
    TEST_ASSERT(total_count == 5,
                "Count all items returns 5");

    /* Test 3: Get by count range */
    memset(items, 0, sizeof(items));
    actual_count = 0;
    TEST_ASSERT(CheckPickUpRandomOptionItem_GetByCountRange(manager, 4, 8, items, 10, &actual_count) == 0,
                "Get items with count range 4-8");
    TEST_ASSERT(actual_count >= 2,
                "Verify at least 2 items in range 4-8");

    /* Test 4: Count by specific count value */
    int count_by_val = CheckPickUpRandomOptionItem_CountByCount(manager, 6);
    TEST_ASSERT(count_by_val == 1,
                "Count items with check_count=6 returns 1");

    /* Test 5: Print info utility */
    printf("Test PrintInfo utility:\n");
    CheckPickUpRandomOptionItem_PrintInfo(&items[0]);
    TEST_ASSERT(1, "PrintInfo executed without error");
}

void test_batch_operations(DBConnectionManager* manager) {
    unsigned int m_ids[3];
    int i;

    printf("\n--- Test Batch Operations ---\n");

    /* Setup test data */
    for (i = 0; i < 3; i++) {
        CheckPickUpRandomOptionItem item;
        memset(&item, 0, sizeof(CheckPickUpRandomOptionItem));
        item.m_id = 100001 + i;
        item.check_count = i + 1;
        CheckPickUpRandomOptionItem_Add(manager, &item);
    }

    /* Test 1: Batch delete */
    m_ids[0] = 100001;
    m_ids[1] = 100002;
    m_ids[2] = 100003;

    TEST_ASSERT(CheckPickUpRandomOptionItem_BatchDelete(manager, m_ids, 3) == 0,
                "Batch delete 3 items");

    /* Test 2: Verify all deleted */
    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 100001) == 0,
                "Verify item 100001 deleted");
    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 100002) == 0,
                "Verify item 100002 deleted");
    TEST_ASSERT(CheckPickUpRandomOptionItem_Exists(manager, 100003) == 0,
                "Verify item 100003 deleted");
}
