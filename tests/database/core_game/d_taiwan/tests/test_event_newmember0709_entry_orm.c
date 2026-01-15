#include "event_newmember0709_entry_orm.h"
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

    printf("=== EventNewmember0709Entry ORM Test Suite ===\n\n");

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
        "DELETE FROM event_newmember0709_entry WHERE m_id IN (100001, 100002, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventNewmember0709Entry entry, retrieved;
    int result;

    printf("[Test] Basic CRUD Operations\n");

    /* Test 1: Add new entry */
    memset(&entry, 0, sizeof(EventNewmember0709Entry));
    entry.m_id = 100001;
    entry.occ_date = 20090701;
    entry.server_id = 1;
    entry.charac_no = 5001;
    entry.item1_no = 10001;
    entry.item1_check = 1;
    entry.item2_no = 10002;
    entry.item2_check = 1;

    result = EventNewmember0709Entry_Add(manager, &entry);
    if (result == 0) {
        printf("  [PASS] Add entry (m_id=100001)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add entry failed\n");
        test_failed++;
    }

    /* Test 2: Exists check */
    result = EventNewmember0709Entry_Exists(manager, 100001);
    if (result == 1) {
        printf("  [PASS] Exists check (found)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Exists check (should exist)\n");
        test_failed++;
    }

    /* Test 3: Get entry */
    memset(&retrieved, 0, sizeof(EventNewmember0709Entry));
    result = EventNewmember0709Entry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.m_id == 100001 && retrieved.occ_date == 20090701) {
        printf("  [PASS] Get entry (verified fields)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Get entry (data mismatch)\n");
        test_failed++;
    }

    /* Test 4: Update entry */
    retrieved.item1_no = 20001;
    retrieved.item1_check = 2;
    retrieved.item2_no = 20002;
    retrieved.item2_check = 2;
    result = EventNewmember0709Entry_Update(manager, &retrieved);
    if (result == 0) {
        printf("  [PASS] Update entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Update entry failed\n");
        test_failed++;
    }

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(EventNewmember0709Entry));
    result = EventNewmember0709Entry_Get(manager, 100001, &retrieved);
    if (result == 0 && retrieved.item1_no == 20001 && retrieved.item2_no == 20002) {
        printf("  [PASS] Verify update (items updated)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Verify update (data not updated)\n");
        test_failed++;
    }

    /* Test 6: Delete entry */
    result = EventNewmember0709Entry_Delete(manager, 100001);
    if (result == 0) {
        printf("  [PASS] Delete entry\n");
        test_passed++;
    } else {
        printf("  [FAIL] Delete entry failed\n");
        test_failed++;
    }

    /* Test 7: Verify deletion */
    result = EventNewmember0709Entry_Exists(manager, 100001);
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
    EventNewmember0709Entry entries[10];
    EventNewmember0709Entry test_data[3];
    int actual_count;
    int result;

    printf("[Test] Business Operations\n");

    /* Prepare test data */
    memset(test_data, 0, sizeof(test_data));

    test_data[0].m_id = 100001;
    test_data[0].occ_date = 20090701;
    test_data[0].server_id = 1;
    test_data[0].charac_no = 5001;
    test_data[0].item1_no = 10001;
    test_data[0].item1_check = 1;
    test_data[0].item2_no = 10002;
    test_data[0].item2_check = 1;

    test_data[1].m_id = 100002;
    test_data[1].occ_date = 20090701;
    test_data[1].server_id = 1;
    test_data[1].charac_no = 5002;
    test_data[1].item1_no = 10003;
    test_data[1].item1_check = 1;
    test_data[1].item2_no = 10004;
    test_data[1].item2_check = 1;

    test_data[2].m_id = 200001;
    test_data[2].occ_date = 20090702;
    test_data[2].server_id = 2;
    test_data[2].charac_no = 6001;
    test_data[2].item1_no = 10005;
    test_data[2].item1_check = 1;
    test_data[2].item2_no = 10006;
    test_data[2].item2_check = 1;

    /* Insert test data */
    for (int i = 0; i < 3; i++) {
        EventNewmember0709Entry_Add(manager, &test_data[i]);
    }

    /* Test 1: GetByDate */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventNewmember0709Entry_GetByDate(manager, 20090701, entries, 10, &actual_count);
    if (result == 0 && actual_count == 2) {
        printf("  [PASS] GetByDate (found 2 entries for date=20090701)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetByDate (expected 2, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 2: GetByDateRange */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventNewmember0709Entry_GetByDateRange(manager, 20090701, 20090702, entries, 10, &actual_count);
    if (result == 0 && actual_count == 3) {
        printf("  [PASS] GetByDateRange (found 3 entries in range)\n");
        test_passed++;
    } else {
        printf("  [FAIL] GetByDateRange (expected 3, got %d)\n", actual_count);
        test_failed++;
    }

    /* Test 3: GetAll */
    actual_count = 0;
    memset(entries, 0, sizeof(entries));
    result = EventNewmember0709Entry_GetAll(manager, entries, 10, &actual_count);
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
        EventNewmember0709Entry_PrintInfo(&entries[0]);
        printf("  [PASS] PrintInfo function\n");
        test_passed++;
    } else {
        printf("  [FAIL] PrintInfo (no data)\n");
        test_failed++;
    }

    printf("\n");
}
