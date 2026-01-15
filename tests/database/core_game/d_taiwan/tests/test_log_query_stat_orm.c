#include "log_query_stat_orm.h"
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
void test_aggregate_operations(DBConnectionManager* manager);
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

    printf("=== Log Query Stat ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_specialized_operations(&manager);
    test_query_operations(&manager);
    test_aggregate_operations(&manager);
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
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM log_query_stat WHERE occ_time >= '2025-01-01 00:00:00' "
        "AND occ_time <= '2025-12-31 23:59:59'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogQueryStat stat;
    LogQueryStat retrieved;

    printf("\n--- Test Basic CRUD Operations ---\n");

    /* Test 1: Add new stat */
    memset(&stat, 0, sizeof(LogQueryStat));
    strcpy(stat.occ_time, "2025-11-18 10:00:00");
    stat.q_id = 1001;
    stat.gc_no = 5;
    stat.total = 100;
    stat.response_time = 50;

    TEST_ASSERT(LogQueryStat_Add(manager, &stat) == 0,
                "Add new log stat");

    /* Test 2: Get stat */
    memset(&retrieved, 0, sizeof(LogQueryStat));
    TEST_ASSERT(LogQueryStat_Get(manager, "2025-11-18 10:00:00", 1001, 5, &retrieved) == 0,
                "Get log stat by composite key");

    /* Test 3: Verify retrieved data */
    TEST_ASSERT(strcmp(retrieved.occ_time, "2025-11-18 10:00:00") == 0,
                "Verify occ_time matches");
    TEST_ASSERT(retrieved.q_id == 1001,
                "Verify q_id matches");
    TEST_ASSERT(retrieved.gc_no == 5,
                "Verify gc_no matches");
    TEST_ASSERT(retrieved.total == 100,
                "Verify total matches");
    TEST_ASSERT(retrieved.response_time == 50,
                "Verify response_time matches");

    /* Test 4: Exists check */
    TEST_ASSERT(LogQueryStat_Exists(manager, "2025-11-18 10:00:00", 1001, 5) == 1,
                "Stat exists");
    TEST_ASSERT(LogQueryStat_Exists(manager, "2025-11-18 10:00:00", 9999, 1) == 0,
                "Non-existent stat returns 0");

    /* Test 5: Update stat */
    stat.total = 200;
    stat.response_time = 75;
    TEST_ASSERT(LogQueryStat_Update(manager, &stat) == 0,
                "Update log stat");

    memset(&retrieved, 0, sizeof(LogQueryStat));
    LogQueryStat_Get(manager, "2025-11-18 10:00:00", 1001, 5, &retrieved);
    TEST_ASSERT(retrieved.total == 200 && retrieved.response_time == 75,
                "Verify stat updated");

    /* Test 6: Delete stat */
    TEST_ASSERT(LogQueryStat_Delete(manager, "2025-11-18 10:00:00", 1001, 5) == 0,
                "Delete log stat");

    TEST_ASSERT(LogQueryStat_Exists(manager, "2025-11-18 10:00:00", 1001, 5) == 0,
                "Verify stat deleted");

    /* Test 7: REPLACE INTO behavior */
    memset(&stat, 0, sizeof(LogQueryStat));
    strcpy(stat.occ_time, "2025-11-18 11:00:00");
    stat.q_id = 1002;
    stat.gc_no = 10;
    stat.total = 50;
    stat.response_time = 30;
    LogQueryStat_Add(manager, &stat);

    stat.total = 150;
    stat.response_time = 60;
    TEST_ASSERT(LogQueryStat_Add(manager, &stat) == 0,
                "Add duplicate stat (REPLACE INTO)");

    memset(&retrieved, 0, sizeof(LogQueryStat));
    LogQueryStat_Get(manager, "2025-11-18 11:00:00", 1002, 10, &retrieved);
    TEST_ASSERT(retrieved.total == 150 && retrieved.response_time == 60,
                "Verify REPLACE INTO updated values");
}

void test_specialized_operations(DBConnectionManager* manager) {
    LogQueryStat stat;
    LogQueryStat retrieved;

    printf("\n--- Test Specialized Operations ---\n");

    /* Setup test data */
    memset(&stat, 0, sizeof(LogQueryStat));
    strcpy(stat.occ_time, "2025-11-18 12:00:00");
    stat.q_id = 1003;
    stat.gc_no = 15;
    stat.total = 100;
    stat.response_time = 40;
    LogQueryStat_Add(manager, &stat);

    /* Test 1: Update total */
    TEST_ASSERT(LogQueryStat_UpdateTotal(manager, "2025-11-18 12:00:00", 1003, 15, 250) == 0,
                "Update total");

    memset(&retrieved, 0, sizeof(LogQueryStat));
    LogQueryStat_Get(manager, "2025-11-18 12:00:00", 1003, 15, &retrieved);
    TEST_ASSERT(retrieved.total == 250,
                "Verify total updated to 250");

    /* Test 2: Update response time */
    TEST_ASSERT(LogQueryStat_UpdateResponseTime(manager, "2025-11-18 12:00:00", 1003, 15, 80) == 0,
                "Update response time");

    memset(&retrieved, 0, sizeof(LogQueryStat));
    LogQueryStat_Get(manager, "2025-11-18 12:00:00", 1003, 15, &retrieved);
    TEST_ASSERT(retrieved.response_time == 80,
                "Verify response_time updated to 80");

    /* Test 3: Increment total */
    TEST_ASSERT(LogQueryStat_IncrementTotal(manager, "2025-11-18 12:00:00", 1003, 15, 50) == 0,
                "Increment total by 50");

    memset(&retrieved, 0, sizeof(LogQueryStat));
    LogQueryStat_Get(manager, "2025-11-18 12:00:00", 1003, 15, &retrieved);
    TEST_ASSERT(retrieved.total == 300,
                "Verify total incremented to 300");
}

void test_query_operations(DBConnectionManager* manager) {
    LogQueryStat stats[10];
    int actual_count = 0;
    int i;

    printf("\n--- Test Query Operations ---\n");

    /* Setup test data */
    for (i = 0; i < 5; i++) {
        LogQueryStat stat;
        memset(&stat, 0, sizeof(LogQueryStat));
        snprintf(stat.occ_time, sizeof(stat.occ_time), "2025-11-18 %02d:00:00", 13 + i);
        stat.q_id = 2001;
        stat.gc_no = 20 + i;
        stat.total = 100 + (i * 10);
        stat.response_time = 50 + (i * 5);
        LogQueryStat_Add(manager, &stat);
    }

    /* Test 1: Get by query ID */
    memset(stats, 0, sizeof(stats));
    actual_count = 0;
    TEST_ASSERT(LogQueryStat_GetByQueryId(manager, 2001, stats, 10, &actual_count) == 0,
                "Get stats by query ID");
    TEST_ASSERT(actual_count == 5,
                "Verify got 5 stats for q_id=2001");

    /* Test 2: Get by time range */
    memset(stats, 0, sizeof(stats));
    actual_count = 0;
    TEST_ASSERT(LogQueryStat_GetByTimeRange(manager, "2025-11-18 13:00:00",
                                             "2025-11-18 15:00:00", stats, 10, &actual_count) == 0,
                "Get stats by time range");
    TEST_ASSERT(actual_count == 3,
                "Verify got 3 stats in range 13:00-15:00");

    /* Test 3: Get by channel */
    memset(stats, 0, sizeof(stats));
    actual_count = 0;
    TEST_ASSERT(LogQueryStat_GetByChannel(manager, 22, stats, 10, &actual_count) == 0,
                "Get stats by channel");
    TEST_ASSERT(actual_count == 1,
                "Verify got 1 stat for gc_no=22");

    /* Test 4: Count by query ID */
    int count = LogQueryStat_CountByQueryId(manager, 2001);
    TEST_ASSERT(count == 5,
                "Count stats by query ID returns 5");

    /* Test 5: Count by time range */
    count = LogQueryStat_CountByTimeRange(manager, "2025-11-18 13:00:00", "2025-11-18 15:00:00");
    TEST_ASSERT(count == 3,
                "Count stats by time range returns 3");

    /* Test 6: Print info utility */
    printf("Test PrintInfo utility:\n");
    LogQueryStat_PrintInfo(&stats[0]);
    TEST_ASSERT(1, "PrintInfo executed without error");
}

void test_aggregate_operations(DBConnectionManager* manager) {
    int i;

    printf("\n--- Test Aggregate Operations ---\n");

    /* Setup test data with different response times */
    for (i = 0; i < 3; i++) {
        LogQueryStat stat;
        memset(&stat, 0, sizeof(LogQueryStat));
        snprintf(stat.occ_time, sizeof(stat.occ_time), "2025-11-18 %02d:30:00", 18 + i);
        stat.q_id = 3001;
        stat.gc_no = 30 + i;
        stat.total = 100;
        stat.response_time = 100 + (i * 50);  /* 100, 150, 200 */
        LogQueryStat_Add(manager, &stat);
    }

    /* Test 1: Get average response time */
    int avg_time = LogQueryStat_GetAverageResponseTime(manager, 3001);
    TEST_ASSERT(avg_time == 150,
                "Average response time is 150ms");

    /* Test 2: Get max response time */
    int max_time = LogQueryStat_GetMaxResponseTime(manager, 3001);
    TEST_ASSERT(max_time == 200,
                "Max response time is 200ms");
}

void test_batch_operations(DBConnectionManager* manager) {
    int i;
    int count_before, count_after;

    printf("\n--- Test Batch Operations ---\n");

    /* Setup test data */
    for (i = 0; i < 5; i++) {
        LogQueryStat stat;
        memset(&stat, 0, sizeof(LogQueryStat));
        snprintf(stat.occ_time, sizeof(stat.occ_time), "2025-11-18 %02d:00:00", 20 + i);
        stat.q_id = 4001;
        stat.gc_no = 40 + i;
        stat.total = 100;
        stat.response_time = 50;
        LogQueryStat_Add(manager, &stat);
    }

    count_before = LogQueryStat_CountByQueryId(manager, 4001);

    /* Test 1: Delete by time range */
    TEST_ASSERT(LogQueryStat_DeleteByTimeRange(manager, "2025-11-18 21:00:00",
                                                "2025-11-18 23:00:00") == 0,
                "Delete stats by time range");

    count_after = LogQueryStat_CountByQueryId(manager, 4001);
    TEST_ASSERT(count_after == count_before - 3,
                "Verify 3 stats deleted from time range");

    /* Test 2: Verify specific records deleted */
    TEST_ASSERT(LogQueryStat_Exists(manager, "2025-11-18 21:00:00", 4001, 41) == 0,
                "Verify 21:00:00 record deleted");
    TEST_ASSERT(LogQueryStat_Exists(manager, "2025-11-18 20:00:00", 4001, 40) == 1,
                "Verify 20:00:00 record still exists");
}
