#include "m_withdraw_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    printf("=== Member Withdraw ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_specialized_operations(&manager);
    test_query_operations(&manager);
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
        "DELETE FROM m_withdraw WHERE user_id LIKE 'test%%'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MWithdraw withdraw;
    MWithdraw retrieved;
    int current_time = (int)time(NULL);

    printf("\n--- Test Basic CRUD Operations ---\n");

    /* Test 1: Add new withdraw record */
    memset(&withdraw, 0, sizeof(MWithdraw));
    strcpy(withdraw.user_id, "test01");
    strcpy(withdraw.user_name, "TestU");
    strcpy(withdraw.first_ssn, "123");
    strcpy(withdraw.second_ssn, "456");
    strcpy(withdraw.passwd, "pass1234");
    strcpy(withdraw.mobile_no, "1234567");
    withdraw.reg_date = current_time;
    strcpy(withdraw.email, "test@example.com");
    withdraw.q_no = 1;
    strcpy(withdraw.q_answer, "answer1");
    withdraw.state = 0;
    withdraw.w_type = 1;
    strcpy(withdraw.w_cause, "User requested");
    withdraw.w_date = current_time;
    strcpy(withdraw.nickname, "TestNick");

    TEST_ASSERT(MWithdraw_Add(manager, &withdraw) == 0,
                "Add new withdraw record");

    /* Test 2: Get by user_id */
    memset(&retrieved, 0, sizeof(MWithdraw));
    TEST_ASSERT(MWithdraw_GetByUserId(manager, "test01", &retrieved) == 0,
                "Get withdraw by user_id");

    /* Test 3: Verify retrieved data */
    TEST_ASSERT(strcmp(retrieved.user_id, "test01") == 0,
                "Verify user_id matches");
    TEST_ASSERT(strcmp(retrieved.user_name, "TestU") == 0,
                "Verify user_name matches");
    TEST_ASSERT(strcmp(retrieved.email, "test@example.com") == 0,
                "Verify email matches");
    TEST_ASSERT(retrieved.state == 0,
                "Verify state matches");
    TEST_ASSERT(retrieved.w_type == 1,
                "Verify w_type matches");

    /* Test 4: Get by m_id */
    unsigned int test_m_id = retrieved.m_id;
    memset(&retrieved, 0, sizeof(MWithdraw));
    TEST_ASSERT(MWithdraw_Get(manager, test_m_id, &retrieved) == 0,
                "Get withdraw by m_id");

    /* Test 5: Exists check */
    TEST_ASSERT(MWithdraw_Exists(manager, test_m_id) == 1,
                "Withdraw record exists");
    TEST_ASSERT(MWithdraw_Exists(manager, 999999) == 0,
                "Non-existent record returns 0");

    /* Test 6: Update withdraw record */
    strcpy(retrieved.email, "updated@example.com");
    retrieved.state = 1;
    TEST_ASSERT(MWithdraw_Update(manager, &retrieved) == 0,
                "Update withdraw record");

    memset(&withdraw, 0, sizeof(MWithdraw));
    MWithdraw_Get(manager, test_m_id, &withdraw);
    TEST_ASSERT(strcmp(withdraw.email, "updated@example.com") == 0,
                "Verify email updated");
    TEST_ASSERT(withdraw.state == 1,
                "Verify state updated");

    /* Test 7: Delete withdraw record */
    TEST_ASSERT(MWithdraw_Delete(manager, test_m_id) == 0,
                "Delete withdraw record");

    TEST_ASSERT(MWithdraw_Exists(manager, test_m_id) == 0,
                "Verify record deleted");
}

void test_specialized_operations(DBConnectionManager* manager) {
    MWithdraw withdraw;
    MWithdraw retrieved;
    int current_time = (int)time(NULL);

    printf("\n--- Test Specialized Operations ---\n");

    /* Setup test data */
    memset(&withdraw, 0, sizeof(MWithdraw));
    strcpy(withdraw.user_id, "test02");
    strcpy(withdraw.user_name, "TestU");
    strcpy(withdraw.first_ssn, "123");
    strcpy(withdraw.second_ssn, "456");
    strcpy(withdraw.passwd, "pass1234");
    strcpy(withdraw.mobile_no, "1234567");
    withdraw.reg_date = current_time;
    strcpy(withdraw.email, "test2@example.com");
    withdraw.q_no = 1;
    strcpy(withdraw.q_answer, "answer1");
    withdraw.state = 0;
    withdraw.w_type = 1;
    strcpy(withdraw.w_cause, "Initial cause");
    withdraw.w_date = current_time;
    strcpy(withdraw.nickname, "Test2");
    MWithdraw_Add(manager, &withdraw);

    MWithdraw_GetByUserId(manager, "test02", &retrieved);
    unsigned int test_m_id = retrieved.m_id;

    /* Test 1: Update state */
    TEST_ASSERT(MWithdraw_UpdateState(manager, test_m_id, 2) == 0,
                "Update state");

    memset(&retrieved, 0, sizeof(MWithdraw));
    MWithdraw_Get(manager, test_m_id, &retrieved);
    TEST_ASSERT(retrieved.state == 2,
                "Verify state updated to 2");

    /* Test 2: Update withdraw info */
    TEST_ASSERT(MWithdraw_UpdateWithdrawInfo(manager, test_m_id, 3,
                                              "Account closure", current_time + 100) == 0,
                "Update withdraw info");

    memset(&retrieved, 0, sizeof(MWithdraw));
    MWithdraw_Get(manager, test_m_id, &retrieved);
    TEST_ASSERT(retrieved.w_type == 3,
                "Verify w_type updated to 3");
    TEST_ASSERT(strcmp(retrieved.w_cause, "Account closure") == 0,
                "Verify w_cause updated");

    /* Test 3: Update email */
    TEST_ASSERT(MWithdraw_UpdateEmail(manager, test_m_id, "newemail@test.com") == 0,
                "Update email");

    memset(&retrieved, 0, sizeof(MWithdraw));
    MWithdraw_Get(manager, test_m_id, &retrieved);
    TEST_ASSERT(strcmp(retrieved.email, "newemail@test.com") == 0,
                "Verify email updated");
}

void test_query_operations(DBConnectionManager* manager) {
    MWithdraw withdraws[10];
    int actual_count = 0;
    int current_time = (int)time(NULL);
    int i;

    printf("\n--- Test Query Operations ---\n");

    /* Setup test data */
    for (i = 0; i < 5; i++) {
        MWithdraw withdraw;
        memset(&withdraw, 0, sizeof(MWithdraw));
        snprintf(withdraw.user_id, sizeof(withdraw.user_id), "test%02d", 10 + i);
        strcpy(withdraw.user_name, "TestU");
        strcpy(withdraw.first_ssn, "123");
        strcpy(withdraw.second_ssn, "456");
        strcpy(withdraw.passwd, "pass1234");
        strcpy(withdraw.mobile_no, "1234567");
        withdraw.reg_date = current_time + (i * 86400);  /* Different days */
        snprintf(withdraw.email, sizeof(withdraw.email), "test%d@example.com", 10 + i);
        withdraw.q_no = 1;
        strcpy(withdraw.q_answer, "answer");
        withdraw.state = (i % 3);  /* States: 0, 1, 2, 0, 1 */
        withdraw.w_type = (i % 2) + 1;  /* Types: 1, 2, 1, 2, 1 */
        strcpy(withdraw.w_cause, "Test cause");
        withdraw.w_date = current_time + (i * 3600);
        strcpy(withdraw.nickname, "TestN");
        MWithdraw_Add(manager, &withdraw);
    }

    /* Test 1: Get by reg_date range */
    memset(withdraws, 0, sizeof(withdraws));
    actual_count = 0;
    TEST_ASSERT(MWithdraw_GetByRegDateRange(manager, current_time,
                                             current_time + (2 * 86400), withdraws, 10, &actual_count) == 0,
                "Get by reg_date range");
    TEST_ASSERT(actual_count == 3,
                "Verify got 3 records in date range");

    /* Test 2: Get by state */
    memset(withdraws, 0, sizeof(withdraws));
    actual_count = 0;
    TEST_ASSERT(MWithdraw_GetByState(manager, 0, withdraws, 10, &actual_count) == 0,
                "Get by state");
    TEST_ASSERT(actual_count == 2,
                "Verify got 2 records with state=0");

    /* Test 3: Get by withdraw type */
    memset(withdraws, 0, sizeof(withdraws));
    actual_count = 0;
    TEST_ASSERT(MWithdraw_GetByWithdrawType(manager, 1, withdraws, 10, &actual_count) == 0,
                "Get by withdraw type");
    TEST_ASSERT(actual_count == 3,
                "Verify got 3 records with w_type=1");

    /* Test 4: Count by state */
    int count = MWithdraw_CountByState(manager, 1);
    TEST_ASSERT(count == 2,
                "Count by state returns 2");

    /* Test 5: Count by withdraw type */
    count = MWithdraw_CountByWithdrawType(manager, 2);
    TEST_ASSERT(count == 2,
                "Count by withdraw type returns 2");

    /* Test 6: Count by reg_date range */
    count = MWithdraw_CountByRegDateRange(manager, current_time, current_time + (4 * 86400));
    TEST_ASSERT(count == 5,
                "Count by reg_date range returns 5");

    /* Test 7: Print info utility */
    printf("Test PrintInfo utility:\n");
    MWithdraw_PrintInfo(&withdraws[0]);
    TEST_ASSERT(1, "PrintInfo executed without error");
}

void test_batch_operations(DBConnectionManager* manager) {
    int current_time = (int)time(NULL);
    int old_time = current_time - (30 * 86400);  /* 30 days ago */
    int i;
    int count_before, count_after;

    printf("\n--- Test Batch Operations ---\n");

    /* Setup test data with old dates */
    for (i = 0; i < 3; i++) {
        MWithdraw withdraw;
        memset(&withdraw, 0, sizeof(MWithdraw));
        snprintf(withdraw.user_id, sizeof(withdraw.user_id), "old%02d", i);
        strcpy(withdraw.user_name, "OldU");
        strcpy(withdraw.first_ssn, "123");
        strcpy(withdraw.second_ssn, "456");
        strcpy(withdraw.passwd, "pass1234");
        strcpy(withdraw.mobile_no, "1234567");
        withdraw.reg_date = old_time + (i * 86400);
        strcpy(withdraw.email, "old@example.com");
        withdraw.q_no = 1;
        strcpy(withdraw.q_answer, "answer");
        withdraw.state = 0;
        withdraw.w_type = 1;
        strcpy(withdraw.w_cause, "Old record");
        withdraw.w_date = old_time;
        strcpy(withdraw.nickname, "OldN");
        MWithdraw_Add(manager, &withdraw);
    }

    count_before = MWithdraw_CountByRegDateRange(manager, old_time, old_time + (2 * 86400));

    /* Test 1: Delete by reg_date range */
    TEST_ASSERT(MWithdraw_DeleteByRegDateRange(manager, old_time,
                                                old_time + (1 * 86400)) == 0,
                "Delete by reg_date range");

    count_after = MWithdraw_CountByRegDateRange(manager, old_time, old_time + (2 * 86400));
    TEST_ASSERT(count_after == count_before - 2,
                "Verify 2 records deleted");

    /* Test 2: Verify specific records deleted */
    MWithdraw temp;
    TEST_ASSERT(MWithdraw_GetByUserId(manager, "old00", &temp) < 0,
                "Verify old00 deleted");
    TEST_ASSERT(MWithdraw_GetByUserId(manager, "old01", &temp) < 0,
                "Verify old01 deleted");
    TEST_ASSERT(MWithdraw_GetByUserId(manager, "old02", &temp) == 0,
                "Verify old02 still exists");
}
