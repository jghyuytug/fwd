#include "event_skill2025_entry_orm.h"
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

    printf("=== EventSkill2025Entry ORM Test Suite ===\n\n");

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
        "DELETE FROM event_skill2025_entry WHERE m_id IN (100001, 100002, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventSkill2025Entry entry, retrieved;
    int result;

    printf("[Test] Basic CRUD Operations\n");

    /* Test 1: Add new entry */
    memset(&entry, 0, sizeof(EventSkill2025Entry));
    entry.m_id = 100001;
    entry.server_id = 1;
    entry.charac_no = 5001;
    strcpy(entry.occ_time, "2025-11-17 10:00:00");

    result = EventSkill2025Entry_Add(manager, &entry);
    if (result == 0) {
        printf("  [PASS] Add entry (m_id=100001)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add entry failed\n");
        test_failed++;
    }

    /* Test 2: Exists check */
    result = EventSkill2025Entry_Exists(manager, 100001);
    if (result == 1) {
        printf("  [PASS] Exists check (found)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Exists check (should exist)\n");
        test_failed++;
    }

    /* Test 3: Get entry */
    memset(&retrieved, 0, sizeof(EventSkill2025Entry));
    result = EventSkill2025Entry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.m_id == 100001 && retrieved.server_id == 1) {
        printf("  [PASS] Get entry (verified fields)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Get entry (data mismatch)\n");
        test_failed++;
    }

    /* Test 4: Update entry */
    retrieved.charac_no = 6001;
    strcpy(retrieved.occ_time, "2025-11-17 12:00:00");
    result = EventSkill2025Entry_Update(manager, &retrieved);
    if (result == 0) {
        printf("  [PASS] Update entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Update entry failed\n");
        test_failed++;
    }

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(EventSkill2025Entry));
    result = EventSkill2025Entry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.charac_no == 6001) {
        printf("  [PASS] Verify update (charac_no=6001)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Verify update (data not updated)\n");
        test_failed++;
    }

    /* Test 6: Delete entry */
    result = EventSkill2025Entry_Delete(manager, 100001);
    if (result == 0) {
        printf("  [PASS] Delete entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Delete entry failed\n");
        test_failed++;
    }

    /* Test 7: Verify deletion */
    result = EventSkill2025Entry_Exists(manager, 100001);
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
    EventSkill2025Entry entries[10];
    EventSkill2025Entry test_data[3];
    int actual_count;
    int result;

    printf("[Test] Business Operations\n");

    /* Prepare test data */
    memset(test_data, 0, sizeof(test_data));

    test_data[0].m_id = 100001;
    test_data[0].server_id = 1;
    test_data[0].charac_no = 5001;
    strcpy(test_data[0].occ_time, "2025-11-17 10:00:00");

    test_data[1].m_id = 100002;
    test_data[1].server_id = 1;
    test_data[1].charac_no = 5002;
    strcpy(test_data[1].occ_time, "2025-11-17 11:00:00");

    test_data[2].m_id = 200001;
    test_data[2].server_id = 2;
    test_data[2].charac_no = 6001;
    strcpy(test_data[2].occ_time, "2025-11-17 12:00:00");

    /* Insert test data */
    for (int i = 0; i < 3; i++) {
        EventSkill2025Entry_Add(manager, &test_data[i]);
    }

    /* Test 1: GetByServer */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventSkill2025Entry_GetByServer(manager, 1, entries, 10, &actual_count);
    if (result == 0 && actual_count == 2) {
        printf("  [PASS] GetByServer (found 2 entries for server_id=1)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetByServer (expected 2, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 2: GetRecent */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventSkill2025Entry_GetRecent(manager, 2, entries, 10, &actual_count);
    if (result == 0 && actual_count == 2) {
        printf("  [PASS] GetRecent (found 2 most recent entries)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetRecent (expected 2, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 3: GetAll */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventSkill2025Entry_GetAll(manager, entries, 10, &actual_count);
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
        EventSkill2025Entry_PrintInfo(&entries[0]);
        printf("  [PASS] PrintInfo function\n");
        test_passed++;
    } else {
        printf("  [FAIL] PrintInfo (no data)\n");
        test_failed++;
    }

    printf("\n");
}
