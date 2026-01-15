#include "event_charac_mov_1th_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("[PASS] %s\n", message); \
        test_passed++; \
    } else { \
        printf("[FAIL] %s\n", message); \
        test_failed++; \
    } \
} while(0)

/* Function prototypes */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

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

    printf("=== Event Charac Mov 1th Entry ORM Test Suite ===\n\n");

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
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    return test_failed > 0 ? 1 : 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_charac_mov_1th_entry WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventCharacMov1thEntry entry;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add entry */
    memset(&entry, 0, sizeof(EventCharacMov1thEntry));
    entry.m_id = 100001;
    entry.occ_time = 1700000000;
    entry.it_no = 5001;
    entry.item_check = 1;

    ret = EventCharacMov1thEntry_Add(manager, &entry);
    TEST_ASSERT(ret == 0, "Add entry");

    /* Test 2: Exists check */
    ret = EventCharacMov1thEntry_Exists(manager, 100001);
    TEST_ASSERT(ret == 1, "Entry exists after add");

    /* Test 3: Get entry */
    memset(&entry, 0, sizeof(EventCharacMov1thEntry));
    ret = EventCharacMov1thEntry_Get(manager, 100001, &entry);
    TEST_ASSERT(ret == 0, "Get entry");
    TEST_ASSERT(entry.m_id == 100001, "Get entry - m_id correct");
    TEST_ASSERT(entry.occ_time == 1700000000, "Get entry - occ_time correct");
    TEST_ASSERT(entry.it_no == 5001, "Get entry - it_no correct");
    TEST_ASSERT(entry.item_check == 1, "Get entry - item_check correct");

    /* Test 4: Update entry */
    entry.occ_time = 1700000100;
    entry.it_no = 5002;
    entry.item_check = 0;
    ret = EventCharacMov1thEntry_Update(manager, &entry);
    TEST_ASSERT(ret == 0, "Update entry");

    /* Test 5: Verify update */
    memset(&entry, 0, sizeof(EventCharacMov1thEntry));
    ret = EventCharacMov1thEntry_Get(manager, 100001, &entry);
    TEST_ASSERT(ret == 0, "Get updated entry");
    TEST_ASSERT(entry.occ_time == 1700000100, "Update - occ_time changed");
    TEST_ASSERT(entry.it_no == 5002, "Update - it_no changed");
    TEST_ASSERT(entry.item_check == 0, "Update - item_check changed");

    /* Test 6: Delete entry */
    ret = EventCharacMov1thEntry_Delete(manager, 100001);
    TEST_ASSERT(ret == 0, "Delete entry");

    /* Test 7: Verify delete */
    ret = EventCharacMov1thEntry_Exists(manager, 100001);
    TEST_ASSERT(ret == 0, "Entry not exists after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventCharacMov1thEntry entries[10];
    int actual_count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data */
    EventCharacMov1thEntry test_data[3] = {
        {100001, 1700000000, 5001, 1},
        {100002, 1700000100, 5001, 0},
        {100003, 1700000200, 5002, 1}
    };

    for (int i = 0; i < 3; i++) {
        EventCharacMov1thEntry_Add(manager, &test_data[i]);
    }

    /* Test 1: GetAll */
    memset(entries, 0, sizeof(entries));
    ret = EventCharacMov1thEntry_GetAll(manager, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetAll operation");
    TEST_ASSERT(actual_count >= 3, "GetAll - returned at least 3 entries");

    /* Test 2: GetByItemNo - it_no = 5001 */
    memset(entries, 0, sizeof(entries));
    ret = EventCharacMov1thEntry_GetByItemNo(manager, 5001, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByItemNo operation");
    TEST_ASSERT(actual_count == 2, "GetByItemNo - found 2 entries with it_no=5001");
    if (actual_count > 0) {
        TEST_ASSERT(entries[0].it_no == 5001, "GetByItemNo - first entry it_no correct");
    }

    /* Test 3: GetByItemNo - it_no = 5002 */
    memset(entries, 0, sizeof(entries));
    ret = EventCharacMov1thEntry_GetByItemNo(manager, 5002, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByItemNo (5002) operation");
    TEST_ASSERT(actual_count == 1, "GetByItemNo - found 1 entry with it_no=5002");

    /* Test 4: GetByItemNo - non-existent it_no */
    memset(entries, 0, sizeof(entries));
    ret = EventCharacMov1thEntry_GetByItemNo(manager, 9999, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByItemNo (non-existent) operation");
    TEST_ASSERT(actual_count == 0, "GetByItemNo - no entries with it_no=9999");

    printf("\n");
}
