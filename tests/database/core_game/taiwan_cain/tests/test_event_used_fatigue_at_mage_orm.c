#include "event_used_fatigue_at_mage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;

/* Test result reporting */
void test_result(const char* test_name, int passed) {
    test_total++;
    if (passed) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        test_failed++;
        printf("[FAIL] %s\n", test_name);
    }
}

/* Cleanup test data */
void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_used_fatigue_at_mage WHERE charac_no IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}

/* Test basic CRUD operations */
void test_basic_crud(DBConnectionManager* manager) {
    EventUsedFatigueAtMage event;
    int ret;

    printf("\n=== Test Basic CRUD Operations ===\n");

    /* Test Add */
    memset(&event, 0, sizeof(EventUsedFatigueAtMage));
    event.charac_no = 100001;
    event.fatigue_quantity = 156;

    ret = EventUsedFatigueAtMage_Add(manager, &event);
    test_result("EventUsedFatigueAtMage_Add", ret == 0);

    /* Test Exists */
    ret = EventUsedFatigueAtMage_Exists(manager, 100001);
    test_result("EventUsedFatigueAtMage_Exists (should exist)", ret == 1);

    /* Test Get */
    EventUsedFatigueAtMage retrieved;
    memset(&retrieved, 0, sizeof(EventUsedFatigueAtMage));
    ret = EventUsedFatigueAtMage_Get(manager, 100001, &retrieved);
    test_result("EventUsedFatigueAtMage_Get", ret == 0 && retrieved.charac_no == 100001);
    test_result("Verify fatigue_quantity", retrieved.fatigue_quantity == 156);

    if (ret == 0) {
        printf("\nRetrieved event info:\n");
        EventUsedFatigueAtMage_PrintInfo(&retrieved);
    }

    /* Test Update */
    retrieved.fatigue_quantity = 312;

    ret = EventUsedFatigueAtMage_Update(manager, &retrieved);
    test_result("EventUsedFatigueAtMage_Update", ret == 0);

    /* Verify update */
    EventUsedFatigueAtMage updated;
    memset(&updated, 0, sizeof(EventUsedFatigueAtMage));
    ret = EventUsedFatigueAtMage_Get(manager, 100001, &updated);
    test_result("Verify Update (fatigue_quantity)", ret == 0 && updated.fatigue_quantity == 312);

    /* Test Delete */
    ret = EventUsedFatigueAtMage_Delete(manager, 100001);
    test_result("EventUsedFatigueAtMage_Delete", ret == 0);

    /* Verify deletion */
    ret = EventUsedFatigueAtMage_Exists(manager, 100001);
    test_result("EventUsedFatigueAtMage_Exists (after delete)", ret == 0);
}

/* Test business operations */
void test_business_operations(DBConnectionManager* manager) {
    EventUsedFatigueAtMage events[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    /* Add test data */
    for (int i = 0; i < 3; i++) {
        EventUsedFatigueAtMage event;
        memset(&event, 0, sizeof(EventUsedFatigueAtMage));

        event.charac_no = 100001 + i;
        event.fatigue_quantity = 100 + (i * 50);

        EventUsedFatigueAtMage_Add(manager, &event);
    }

    /* Test GetAll */
    memset(events, 0, sizeof(events));
    ret = EventUsedFatigueAtMage_GetAll(manager, events, 10, &actual_count);
    test_result("GetAll", ret == 0 && actual_count >= 3);
    printf("  Found %d records (ordered by fatigue DESC)\n", actual_count);

    /* Test GetByFatigueRange */
    memset(events, 0, sizeof(events));
    ret = EventUsedFatigueAtMage_GetByFatigueRange(manager, 120, 180, events, 10, &actual_count);
    test_result("GetByFatigueRange (120-180)", ret == 0);
    printf("  Found %d records with fatigue in range 120-180\n", actual_count);

    /* Test IncreaseFatigue */
    ret = EventUsedFatigueAtMage_IncreaseFatigue(manager, 100001, 50);
    test_result("IncreaseFatigue", ret == 0);

    /* Verify fatigue increased */
    EventUsedFatigueAtMage event;
    memset(&event, 0, sizeof(EventUsedFatigueAtMage));
    ret = EventUsedFatigueAtMage_Get(manager, 100001, &event);
    test_result("Verify fatigue increased", ret == 0 && event.fatigue_quantity == 150);

    /* Test GetTotalFatigue */
    unsigned long long total = 0;
    ret = EventUsedFatigueAtMage_GetTotalFatigue(manager, &total);
    test_result("GetTotalFatigue", ret == 0 && total > 0);
    printf("  Total fatigue across all characters: %llu\n", total);
}

/* Main test function */
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== EventUsedFatigueAtMage ORM Test Suite ===\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Total: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}
