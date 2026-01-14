#include "event_webmoneystamp_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

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

    printf("=== EventWebmoneystampEntry ORM Test Suite ===\n\n");

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

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    /* Delete test records */
    snprintf(query, sizeof(query),
        "DELETE FROM event_webmoneystamp_entry WHERE m_id IN (100001, 100002, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventWebmoneystampEntry entry, retrieved;
    int result;

    printf("[Test] Basic CRUD Operations\n");

    /* Test 1: Add new entry */
    memset(&entry, 0, sizeof(EventWebmoneystampEntry));
    entry.m_id = 100001;
    strcpy(entry.occ_time, "2025-11-17 10:00:00");
    entry.attend_point = 50;
    strcpy(entry.last_attend_time, "2025-11-17 11:00:00");
    entry.return_flag = 1;
    entry.entry_item = 5;

    result = EventWebmoneystampEntry_Add(manager, &entry);
    if (result == 0) {
        printf("  [PASS] Add entry (m_id=100001)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add entry failed\n");
        test_failed++;
    }

    /* Test 2: Exists check */
    result = EventWebmoneystampEntry_Exists(manager, 100001);
    if (result == 1) {
        printf("  [PASS] Exists check (found)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Exists check (should exist)\n");
        test_failed++;
    }

    /* Test 3: Get entry */
    memset(&retrieved, 0, sizeof(EventWebmoneystampEntry));
    result = EventWebmoneystampEntry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.m_id == 100001 && retrieved.attend_point == 50) {
        printf("  [PASS] Get entry (verified fields)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Get entry (data mismatch)\n");
        test_failed++;
    }

    /* Test 4: Update entry */
    retrieved.attend_point = 100;
    strcpy(retrieved.last_attend_time, "2025-11-17 12:00:00");
    result = EventWebmoneystampEntry_Update(manager, &retrieved);
    if (result == 0) {
        printf("  [PASS] Update entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Update entry failed\n");
        test_failed++;
    }

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(EventWebmoneystampEntry));
    result = EventWebmoneystampEntry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.attend_point == 100) {
        printf("  [PASS] Verify update (attend_point=100)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Verify update (data not updated)\n");
        test_failed++;
    }

    /* Test 6: Delete entry */
    result = EventWebmoneystampEntry_Delete(manager, 100001);
    if (result == 0) {
        printf("  [PASS] Delete entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Delete entry failed\n");
        test_failed++;
    }

    /* Test 7: Verify deletion */
    result = EventWebmoneystampEntry_Exists(manager, 100001);
    if (result == 0) {
        printf("  [PASS] Verify deletion (not found)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Verify deletion (still exists)\n");
        test_failed++;
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventWebmoneystampEntry entries[10];
    EventWebmoneystampEntry test_data[3];
    int actual_count;
    int result;

    printf("[Test] Business Operations\n");

    /* Prepare test data */
    memset(test_data, 0, sizeof(test_data));

    test_data[0].m_id = 100001;
    strcpy(test_data[0].occ_time, "2025-11-17 10:00:00");
    test_data[0].attend_point = 30;
    strcpy(test_data[0].last_attend_time, "2025-11-17 11:00:00");
    test_data[0].return_flag = 1;
    test_data[0].entry_item = 3;

    test_data[1].m_id = 100002;
    strcpy(test_data[1].occ_time, "2025-11-17 12:00:00");
    test_data[1].attend_point = 60;
    strcpy(test_data[1].last_attend_time, "2025-11-17 13:00:00");
    test_data[1].return_flag = 1;
    test_data[1].entry_item = 6;

    test_data[2].m_id = 200001;
    strcpy(test_data[2].occ_time, "2025-11-17 14:00:00");
    test_data[2].attend_point = 90;
    strcpy(test_data[2].last_attend_time, "2025-11-17 15:00:00");
    test_data[2].return_flag = 0;
    test_data[2].entry_item = 9;

    /* Insert test data */
    for (int i = 0; i < 3; i++) {
        EventWebmoneystampEntry_Add(manager, &test_data[i]);
    }

    /* Test 1: GetByReturnFlag */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventWebmoneystampEntry_GetByReturnFlag(manager, 1, entries, 10, &actual_count);
    if (result == 0 && actual_count == 2) {
        printf("  [PASS] GetByReturnFlag (found 2 entries with return_flag=1)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetByReturnFlag (expected 2, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 2: GetByPointRange */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventWebmoneystampEntry_GetByPointRange(manager, 50, 100, entries, 10, &actual_count);
    if (result == 0 && actual_count == 2) {
        printf("  [PASS] GetByPointRange (found 2 entries in range 50-100)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetByPointRange (expected 2, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 3: GetAll */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventWebmoneystampEntry_GetAll(manager, entries, 10, &actual_count);
    if (result == 0 && actual_count == 3) {
        printf("  [PASS] GetAll (found 3 total entries)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetAll (expected 3, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 4: Print first entry info */
    if (actual_count > 0) {
        printf("  [INFO] Sample entry details:\n");
        EventWebmoneystampEntry_PrintInfo(&entries[0]);
        printf("  [PASS] PrintInfo function\n");
        test_passed++;
    } else {
        printf("  [FAIL] PrintInfo (no data)\n");
        test_failed++;
    }

    printf("\n");
}
