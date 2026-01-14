#include "event_pandora_entry_200905_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    printf("=== Event Pandora Entry 200905 ORM Test Suite ===\n\n");

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
    EventPandoraEntry200905 entry, retrieved;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add new entry */
    memset(&entry, 0, sizeof(EventPandoraEntry200905));
    entry.m_id = 100001;
    strcpy(entry.occ_date, "2025-11-17");
    entry.server_id = 1;
    entry.charac_no = 500001;

    ret = EventPandoraEntry200905_Add(manager, &entry);
    TEST_ASSERT(ret == 0, "Add new pandora entry");

    /* Test 2: Check existence */
    ret = EventPandoraEntry200905_Exists(manager, 100001, "2025-11-17", 1);
    TEST_ASSERT(ret == 1, "Entry exists after add");

    /* Test 3: Get entry */
    ret = EventPandoraEntry200905_Get(manager, 100001, "2025-11-17", 1, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.m_id == 100001, "Get entry by composite key");
    TEST_ASSERT(strcmp(retrieved.occ_date, "2025-11-17") == 0, "Verify occ_date");
    TEST_ASSERT(retrieved.charac_no == 500001, "Verify charac_no");

    /* Test 4: Update entry */
    entry.charac_no = 500002;
    ret = EventPandoraEntry200905_Update(manager, &entry);
    TEST_ASSERT(ret == 0, "Update entry");

    ret = EventPandoraEntry200905_Get(manager, 100001, "2025-11-17", 1, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.charac_no == 500002, "Verify updated charac_no");

    /* Test 5: Delete entry */
    ret = EventPandoraEntry200905_Delete(manager, 100001, "2025-11-17", 1);
    TEST_ASSERT(ret == 0, "Delete entry");

    ret = EventPandoraEntry200905_Exists(manager, 100001, "2025-11-17", 1);
    TEST_ASSERT(ret == 0, "Entry does not exist after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventPandoraEntry200905 entry;
    EventPandoraEntry200905 entries[10];
    int actual_count = 0;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data - same member, different dates */
    for (int i = 0; i < 3; i++) {
        memset(&entry, 0, sizeof(EventPandoraEntry200905));
        entry.m_id = 200001;
        snprintf(entry.occ_date, sizeof(entry.occ_date), "2025-11-%02d", 15 + i);
        entry.server_id = 2;
        entry.charac_no = 600001;
        EventPandoraEntry200905_Add(manager, &entry);
    }

    /* Test 1: Get by member (should return 3 entries) */
    ret = EventPandoraEntry200905_GetByMember(manager, 200001, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all entries for a member");

    /* Add more entries for the same date */
    for (int i = 0; i < 2; i++) {
        memset(&entry, 0, sizeof(EventPandoraEntry200905));
        entry.m_id = 200002 + i;
        strcpy(entry.occ_date, "2025-11-15");
        entry.server_id = 2 + i;
        entry.charac_no = 600010 + i;
        EventPandoraEntry200905_Add(manager, &entry);
    }

    /* Test 2: Get by date (should return 3 entries for 2025-11-15) */
    ret = EventPandoraEntry200905_GetByDate(manager, "2025-11-15", entries, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all entries for a date");

    /* Add entry with same character on different server */
    memset(&entry, 0, sizeof(EventPandoraEntry200905));
    entry.m_id = 200004;
    strcpy(entry.occ_date, "2025-11-20");
    entry.server_id = 2;
    entry.charac_no = 600001;
    EventPandoraEntry200905_Add(manager, &entry);

    /* Test 3: Get by character (should return 4 entries for charac 600001 on server 2) */
    ret = EventPandoraEntry200905_GetByCharac(manager, 2, 600001, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 4, "Get all entries for a character");

    /* Test 4: Delete by member (should delete all 3 dates for member 200001) */
    ret = EventPandoraEntry200905_DeleteByMember(manager, 200001);
    TEST_ASSERT(ret == 0, "Delete all entries for a member");

    ret = EventPandoraEntry200905_GetByMember(manager, 200001, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 0, "Verify member entries deleted");

    /* Now 2025-11-15 should only have 2 entries left */
    ret = EventPandoraEntry200905_GetByDate(manager, "2025-11-15", entries, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Verify date entries count after delete");

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_pandora_entry_200905 WHERE m_id >= 100000");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
