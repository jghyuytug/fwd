#include "dnf_event_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_total = 0;
static int test_passed = 0;

void cleanup_test_data(DBConnectionManager* manager);
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== DNF Event Entry ORM Test Suite ===\n\n");

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
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("\n=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_entry WHERE event_id IN (100001, 100002) OR m_id IN (200001, 200002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    DnfEventEntry entry, retrieved;
    int ret;

    printf("--- Test Basic CRUD Operations ---\n");

    /* Test 1: Add entry */
    test_total++;
    memset(&entry, 0, sizeof(DnfEventEntry));
    entry.event_id = 100001;
    entry.m_id = 200001;
    strcpy(entry.occ_date, "2025-11-19 10:00:00");
    entry.server_id = 1;
    entry.charac_no = 300001;
    strcpy(entry.obtain_date, "2025-11-19 10:30:00");

    ret = DnfEventEntry_Add(manager, &entry);
    if (ret == 0) {
        printf("[PASS] Test 1: Add entry\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 1: Add entry (returned %d)\n", ret);
    }

    /* Test 2: Get entry */
    test_total++;
    memset(&retrieved, 0, sizeof(DnfEventEntry));
    ret = DnfEventEntry_Get(manager, 100001, 200001, &retrieved);
    if (ret == 0 && retrieved.event_id == 100001 && retrieved.m_id == 200001) {
        printf("[PASS] Test 2: Get entry\n");
        test_passed++;
        DnfEventEntry_PrintInfo(&retrieved);
    } else {
        printf("[FAIL] Test 2: Get entry (returned %d)\n", ret);
    }

    /* Test 3: Exists check */
    test_total++;
    ret = DnfEventEntry_Exists(manager, 100001, 200001);
    if (ret > 0) {
        printf("[PASS] Test 3: Exists check (count=%d)\n", ret);
        test_passed++;
    } else {
        printf("[FAIL] Test 3: Exists check (returned %d)\n", ret);
    }

    /* Test 4: Update entry */
    test_total++;
    entry.server_id = 2;
    entry.charac_no = 300002;
    strcpy(entry.obtain_date, "2025-11-19 11:00:00");
    ret = DnfEventEntry_Update(manager, &entry);
    if (ret == 0) {
        printf("[PASS] Test 4: Update entry\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 4: Update entry (returned %d)\n", ret);
    }

    /* Test 5: Verify update */
    test_total++;
    memset(&retrieved, 0, sizeof(DnfEventEntry));
    ret = DnfEventEntry_Get(manager, 100001, 200001, &retrieved);
    if (ret == 0 && retrieved.server_id == 2 && retrieved.charac_no == 300002) {
        printf("[PASS] Test 5: Verify update\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 5: Verify update (server_id=%u, charac_no=%d)\n",
               retrieved.server_id, retrieved.charac_no);
    }

    /* Test 6: Delete entry */
    test_total++;
    ret = DnfEventEntry_Delete(manager, 100001, 200001);
    if (ret == 0) {
        printf("[PASS] Test 6: Delete entry\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 6: Delete entry (returned %d)\n", ret);
    }

    /* Test 7: Verify deletion */
    test_total++;
    ret = DnfEventEntry_Exists(manager, 100001, 200001);
    if (ret == 0) {
        printf("[PASS] Test 7: Verify deletion\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 7: Verify deletion (still exists: %d)\n", ret);
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    DnfEventEntry entries[10];
    int actual_count;
    int ret;

    printf("--- Test Business Operations ---\n");

    /* Add test data */
    DnfEventEntry e1, e2, e3;

    memset(&e1, 0, sizeof(DnfEventEntry));
    e1.event_id = 100001;
    e1.m_id = 200001;
    strcpy(e1.occ_date, "2025-11-19 10:00:00");
    e1.server_id = 1;
    e1.charac_no = 300001;
    strcpy(e1.obtain_date, "2025-11-19 10:30:00");
    DnfEventEntry_Add(manager, &e1);

    memset(&e2, 0, sizeof(DnfEventEntry));
    e2.event_id = 100002;
    e2.m_id = 200001;
    strcpy(e2.occ_date, "2025-11-19 11:00:00");
    e2.server_id = 1;
    e2.charac_no = 300002;
    strcpy(e2.obtain_date, "2025-11-19 11:30:00");
    DnfEventEntry_Add(manager, &e2);

    memset(&e3, 0, sizeof(DnfEventEntry));
    e3.event_id = 100001;
    e3.m_id = 200002;
    strcpy(e3.occ_date, "2025-11-19 12:00:00");
    e3.server_id = 2;
    e3.charac_no = 300001;
    strcpy(e3.obtain_date, "2025-11-19 12:30:00");
    DnfEventEntry_Add(manager, &e3);

    /* Test 8: Get by Member ID */
    test_total++;
    ret = DnfEventEntry_GetByMemberId(manager, 200001, entries, 10, &actual_count);
    if (ret == 0 && actual_count == 2) {
        printf("[PASS] Test 8: Get by Member ID (found %d entries)\n", actual_count);
        test_passed++;
    } else {
        printf("[FAIL] Test 8: Get by Member ID (returned %d, count %d)\n", ret, actual_count);
    }

    /* Test 9: Get by Event ID */
    test_total++;
    ret = DnfEventEntry_GetByEventId(manager, 100001, entries, 10, &actual_count);
    if (ret == 0 && actual_count == 2) {
        printf("[PASS] Test 9: Get by Event ID (found %d entries)\n", actual_count);
        test_passed++;
    } else {
        printf("[FAIL] Test 9: Get by Event ID (returned %d, count %d)\n", ret, actual_count);
    }

    /* Test 10: Get by Character */
    test_total++;
    ret = DnfEventEntry_GetByCharacter(manager, 300001, entries, 10, &actual_count);
    if (ret == 0 && actual_count == 2) {
        printf("[PASS] Test 10: Get by Character (found %d entries)\n", actual_count);
        test_passed++;
    } else {
        printf("[FAIL] Test 10: Get by Character (returned %d, count %d)\n", ret, actual_count);
    }

    /* Test 11: Get All */
    test_total++;
    ret = DnfEventEntry_GetAll(manager, entries, 10, &actual_count);
    if (ret == 0 && actual_count >= 3) {
        printf("[PASS] Test 11: Get All (found %d entries)\n", actual_count);
        test_passed++;
    } else {
        printf("[FAIL] Test 11: Get All (returned %d, count %d)\n", ret, actual_count);
    }

    printf("\n");
}
