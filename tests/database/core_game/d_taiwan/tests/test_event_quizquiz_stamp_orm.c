#include "event_quizquiz_stamp_orm.h"
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
void test_stamp_increment(DBConnectionManager* manager);
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

    printf("=== Event Quizquiz Stamp ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);
    test_stamp_increment(&manager);

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
        "DELETE FROM event_quizquiz_stamp WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventQuizquizStamp record;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add record */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    record.m_id = 100001;
    record.degree = 1;
    record.stamp = 10;
    strcpy(record.occ_time, "2025-01-01 10:00:00");

    ret = EventQuizquizStamp_Add(manager, &record);
    TEST_ASSERT(ret == 0, "Add record");

    /* Test 2: Exists check */
    ret = EventQuizquizStamp_Exists(manager, 100001, 1);
    TEST_ASSERT(ret == 1, "Record exists after add");

    /* Test 3: Get record */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    ret = EventQuizquizStamp_Get(manager, 100001, 1, &record);
    TEST_ASSERT(ret == 0, "Get record");
    TEST_ASSERT(record.m_id == 100001, "Get record - m_id correct");
    TEST_ASSERT(record.degree == 1, "Get record - degree correct");
    TEST_ASSERT(record.stamp == 10, "Get record - stamp correct");

    /* Test 4: Update record */
    record.stamp = 20;
    strcpy(record.occ_time, "2025-01-02 10:00:00");
    ret = EventQuizquizStamp_Update(manager, &record);
    TEST_ASSERT(ret == 0, "Update record");

    /* Test 5: Verify update */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    ret = EventQuizquizStamp_Get(manager, 100001, 1, &record);
    TEST_ASSERT(ret == 0, "Get updated record");
    TEST_ASSERT(record.stamp == 20, "Update - stamp changed");
    TEST_ASSERT(strcmp(record.occ_time, "2025-01-02 10:00:00") == 0, "Update - occ_time changed");

    /* Test 6: Delete record */
    ret = EventQuizquizStamp_Delete(manager, 100001, 1);
    TEST_ASSERT(ret == 0, "Delete record");

    /* Test 7: Verify delete */
    ret = EventQuizquizStamp_Exists(manager, 100001, 1);
    TEST_ASSERT(ret == 0, "Record not exists after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventQuizquizStamp records[10];
    EventQuizquizStamp test_record;
    int actual_count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data - multiple degrees for same member */
    memset(&test_record, 0, sizeof(EventQuizquizStamp));
    test_record.m_id = 100002;
    test_record.degree = 1;
    test_record.stamp = 5;
    strcpy(test_record.occ_time, "2025-01-01 10:00:00");
    EventQuizquizStamp_Add(manager, &test_record);

    test_record.degree = 2;
    test_record.stamp = 10;
    EventQuizquizStamp_Add(manager, &test_record);

    test_record.degree = 3;
    test_record.stamp = 15;
    EventQuizquizStamp_Add(manager, &test_record);

    /* Different member same degree */
    test_record.m_id = 100003;
    test_record.degree = 1;
    test_record.stamp = 8;
    EventQuizquizStamp_Add(manager, &test_record);

    /* Test 1: GetByMemberId */
    memset(records, 0, sizeof(records));
    ret = EventQuizquizStamp_GetByMemberId(manager, 100002, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByMemberId operation");
    TEST_ASSERT(actual_count == 3, "GetByMemberId - found 3 degrees for m_id=100002");
    if (actual_count == 3) {
        TEST_ASSERT(records[0].degree == 1 && records[0].stamp == 5, "GetByMemberId - degree 1 data correct");
        TEST_ASSERT(records[1].degree == 2 && records[1].stamp == 10, "GetByMemberId - degree 2 data correct");
        TEST_ASSERT(records[2].degree == 3 && records[2].stamp == 15, "GetByMemberId - degree 3 data correct");
    }

    /* Test 2: GetByDegree */
    memset(records, 0, sizeof(records));
    ret = EventQuizquizStamp_GetByDegree(manager, 1, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByDegree operation");
    TEST_ASSERT(actual_count == 2, "GetByDegree - found 2 members with degree=1");

    /* Test 3: GetAll */
    memset(records, 0, sizeof(records));
    ret = EventQuizquizStamp_GetAll(manager, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetAll operation");
    TEST_ASSERT(actual_count >= 4, "GetAll - found at least 4 records");

    printf("\n");
}

void test_stamp_increment(DBConnectionManager* manager) {
    EventQuizquizStamp record;
    int ret;

    printf("--- Test: Stamp Increment ---\n");

    /* Add test record */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    record.m_id = 100003;
    record.degree = 5;
    record.stamp = 100;
    strcpy(record.occ_time, "2025-01-01 10:00:00");
    EventQuizquizStamp_Add(manager, &record);

    /* Test 1: Increment stamp */
    ret = EventQuizquizStamp_UpdateStamp(manager, 100003, 5, 50);
    TEST_ASSERT(ret == 0, "UpdateStamp operation");

    /* Test 2: Verify increment */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    ret = EventQuizquizStamp_Get(manager, 100003, 5, &record);
    TEST_ASSERT(ret == 0, "Get record after stamp increment");
    TEST_ASSERT(record.stamp == 150, "UpdateStamp - stamp incremented correctly (100 + 50 = 150)");

    /* Test 3: Negative increment (decrement) */
    ret = EventQuizquizStamp_UpdateStamp(manager, 100003, 5, -30);
    TEST_ASSERT(ret == 0, "UpdateStamp with negative value");

    /* Test 4: Verify decrement */
    memset(&record, 0, sizeof(EventQuizquizStamp));
    ret = EventQuizquizStamp_Get(manager, 100003, 5, &record);
    TEST_ASSERT(ret == 0, "Get record after stamp decrement");
    TEST_ASSERT(record.stamp == 120, "UpdateStamp - stamp decremented correctly (150 - 30 = 120)");

    printf("\n");
}
