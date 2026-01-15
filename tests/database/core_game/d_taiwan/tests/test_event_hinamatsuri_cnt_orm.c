#include "event_hinamatsuri_cnt_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test functions */
void test_counter_operations(DBConnectionManager* manager);
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

    printf("=== Event Hinamatsuri Counter ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_counter_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n", test_total, test_passed, test_failed);
    printf("Success Rate: %.1f%%\n", test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}

void test_counter_operations(DBConnectionManager* manager) {
    int cnt = 0;
    int ret;

    printf("--- Test: Counter Operations ---\n");

    /* Test 1: Set initial value */
    ret = EventHinamatsuriCnt_Set(manager, 100);
    TEST_ASSERT(ret == 0, "Set counter to 100");

    /* Test 2: Get counter value */
    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == 100, "Get counter value (expected 100)");

    /* Test 3: Increment by 50 */
    ret = EventHinamatsuriCnt_Increment(manager, 50);
    TEST_ASSERT(ret == 0, "Increment counter by 50");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == 150, "Verify counter value (expected 150)");

    /* Test 4: Decrement by 30 (negative increment) */
    ret = EventHinamatsuriCnt_Increment(manager, -30);
    TEST_ASSERT(ret == 0, "Decrement counter by 30");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == 120, "Verify counter value (expected 120)");

    /* Test 5: Set to zero */
    ret = EventHinamatsuriCnt_Set(manager, 0);
    TEST_ASSERT(ret == 0, "Set counter to 0");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == 0, "Verify counter value (expected 0)");

    /* Test 6: Set negative value (edge case) */
    ret = EventHinamatsuriCnt_Set(manager, -999);
    TEST_ASSERT(ret == 0, "Set counter to -999");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == -999, "Verify counter value (expected -999)");

    /* Test 7: Reset counter (delete all rows) */
    ret = EventHinamatsuriCnt_Reset(manager);
    TEST_ASSERT(ret == 0, "Reset counter (delete all rows)");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret < 0, "Verify counter is empty after reset");

    /* Test 8: Increment on empty table (should initialize to 0 + delta) */
    ret = EventHinamatsuriCnt_Increment(manager, 77);
    TEST_ASSERT(ret == 0, "Increment empty counter by 77");

    ret = EventHinamatsuriCnt_Get(manager, &cnt);
    TEST_ASSERT(ret == 0 && cnt == 77, "Verify counter value (expected 77)");

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_hinamatsuri_cnt");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
