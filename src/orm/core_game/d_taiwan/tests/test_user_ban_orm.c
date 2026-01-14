#include "user_ban_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

/* Test helper macros */
#define TEST_START(name) \
    do { \
        printf("\n[TEST] %s\n", name); \
        test_total++; \
    } while(0)

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  [PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("  [FAIL] %s\n", message); \
        } \
    } while(0)

/* Forward declarations */
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

    printf("=== User Ban ORM Test Suite ===\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_crud(DBConnectionManager* manager) {
    UserBan ban;
    UserBan retrieved;
    unsigned int current_time = (unsigned int)time(NULL);

    TEST_START("Basic CRUD Operations");

    /* Prepare test data */
    memset(&ban, 0, sizeof(UserBan));
    ban.category = 1;
    ban.m_id = 100001;
    ban.ban_term = 7;
    ban.ban_reason = 5;
    strncpy(ban.detail_reason, "Test ban for cheating", sizeof(ban.detail_reason) - 1);
    ban.ban_date = current_time;
    ban.admin_id = 9999;
    ban.status = 1;
    strncpy(ban.first_ssn, "123456", sizeof(ban.first_ssn) - 1);
    strncpy(ban.second_ssn, "7890123", sizeof(ban.second_ssn) - 1);

    /* Test Add */
    TEST_ASSERT(UserBan_Add(manager, &ban) == 0, "Add new ban record");

    /* Get the inserted record - query last inserted */
    DBQueryResult result;
    char query[256];
    char* row[1];
    unsigned int inserted_no = 0;

    snprintf(query, sizeof(query),
             "SELECT no FROM user_ban WHERE m_id = 100001 ORDER BY no DESC LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) == 0) {
        if (DBQueryResult_FetchRow(&result, row) > 0) {
            inserted_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        }
        DBQueryResult_Free(&result);
    }

    /* Test Get */
    memset(&retrieved, 0, sizeof(UserBan));
    TEST_ASSERT(UserBan_Get(manager, inserted_no, &retrieved) == 0, "Get ban record");
    TEST_ASSERT(retrieved.m_id == 100001, "Verify m_id");
    TEST_ASSERT(retrieved.ban_reason == 5, "Verify ban_reason");
    TEST_ASSERT(retrieved.status == 1, "Verify status");

    /* Test Exists */
    TEST_ASSERT(UserBan_Exists(manager, inserted_no) == 1, "Check ban exists");

    /* Test Update */
    retrieved.ban_term = 14;
    retrieved.status = 2;
    strncpy(retrieved.cancel_reason, "Test cancel", sizeof(retrieved.cancel_reason) - 1);
    TEST_ASSERT(UserBan_Update(manager, &retrieved) == 0, "Update ban record");

    /* Verify update */
    memset(&retrieved, 0, sizeof(UserBan));
    UserBan_Get(manager, inserted_no, &retrieved);
    TEST_ASSERT(retrieved.ban_term == 14, "Verify updated ban_term");
    TEST_ASSERT(retrieved.status == 2, "Verify updated status");

    /* Test Delete */
    TEST_ASSERT(UserBan_Delete(manager, inserted_no) == 0, "Delete ban record");
    TEST_ASSERT(UserBan_Exists(manager, inserted_no) == 0, "Verify deletion");
}

void test_business_operations(DBConnectionManager* manager) {
    UserBan ban1, ban2, ban3;
    UserBan results[10];
    int count = 0;
    unsigned int current_time = (unsigned int)time(NULL);

    TEST_START("Business Operations");

    /* Add multiple test records */
    memset(&ban1, 0, sizeof(UserBan));
    ban1.category = 1;
    ban1.m_id = 200001;
    ban1.ban_term = 7;
    ban1.ban_reason = 1;
    strncpy(ban1.detail_reason, "First ban", sizeof(ban1.detail_reason) - 1);
    ban1.ban_date = current_time;
    ban1.admin_id = 9999;
    ban1.status = 1;
    strncpy(ban1.first_ssn, "111111", sizeof(ban1.first_ssn) - 1);
    strncpy(ban1.second_ssn, "1111111", sizeof(ban1.second_ssn) - 1);
    UserBan_Add(manager, &ban1);

    memset(&ban2, 0, sizeof(UserBan));
    ban2.category = 1;
    ban2.m_id = 200001;
    ban2.ban_term = 30;
    ban2.ban_reason = 2;
    strncpy(ban2.detail_reason, "Second ban", sizeof(ban2.detail_reason) - 1);
    ban2.ban_date = current_time + 1000;
    ban2.admin_id = 9999;
    ban2.status = 2;
    strncpy(ban2.first_ssn, "111111", sizeof(ban2.first_ssn) - 1);
    strncpy(ban2.second_ssn, "1111111", sizeof(ban2.second_ssn) - 1);
    UserBan_Add(manager, &ban2);

    memset(&ban3, 0, sizeof(UserBan));
    ban3.category = 2;
    ban3.m_id = 300001;
    ban3.ban_term = 1;
    ban3.ban_reason = 3;
    strncpy(ban3.detail_reason, "Third ban", sizeof(ban3.detail_reason) - 1);
    ban3.ban_date = current_time;
    ban3.admin_id = 8888;
    ban3.status = 1;
    strncpy(ban3.first_ssn, "222222", sizeof(ban3.first_ssn) - 1);
    strncpy(ban3.second_ssn, "2222222", sizeof(ban3.second_ssn) - 1);
    UserBan_Add(manager, &ban3);

    /* Test GetByMemberId */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(UserBan_GetByMemberId(manager, 200001, results, 10, &count) == 0,
                "Get bans by member ID");
    TEST_ASSERT(count == 2, "Verify ban count for member 200001");

    /* Test GetByStatus */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(UserBan_GetByStatus(manager, 1, results, 10, &count) == 0,
                "Get bans by status");
    TEST_ASSERT(count >= 2, "Verify active bans exist");

    /* Test GetActiveByMemberId */
    memset(&ban1, 0, sizeof(UserBan));
    TEST_ASSERT(UserBan_GetActiveByMemberId(manager, 200001, &ban1) == 0,
                "Get active ban for member");
    TEST_ASSERT(ban1.status == 1, "Verify active ban status");

    /* Test GetByFirstSSN */
    count = 0;
    memset(results, 0, sizeof(results));
    TEST_ASSERT(UserBan_GetByFirstSSN(manager, "111111", results, 10, &count) == 0,
                "Get bans by first SSN");
    TEST_ASSERT(count == 2, "Verify ban count by SSN");

    /* Test UpdateStatus */
    if (count > 0) {
        TEST_ASSERT(UserBan_UpdateStatus(manager, results[0].no, 3) == 0,
                    "Update ban status");

        memset(&ban1, 0, sizeof(UserBan));
        UserBan_Get(manager, results[0].no, &ban1);
        TEST_ASSERT(ban1.status == 3, "Verify status updated");
    }

    /* Test CancelBan */
    DBQueryResult result;
    char query[256];
    char* row[1];
    unsigned int test_no = 0;

    snprintf(query, sizeof(query),
             "SELECT no FROM user_ban WHERE m_id = 300001 LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) == 0) {
        if (DBQueryResult_FetchRow(&result, row) > 0) {
            test_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        }
        DBQueryResult_Free(&result);
    }

    if (test_no > 0) {
        TEST_ASSERT(UserBan_CancelBan(manager, test_no, "Admin cancelled", 7777) == 0,
                    "Cancel ban");

        memset(&ban3, 0, sizeof(UserBan));
        UserBan_Get(manager, test_no, &ban3);
        TEST_ASSERT(ban3.status == 2, "Verify ban cancelled");
        TEST_ASSERT(ban3.cancel_date > 0, "Verify cancel date set");
    }

    /* Test PrintInfo */
    if (count > 0) {
        printf("\n  [INFO] Sample ban record:\n");
        UserBan_PrintInfo(&results[0]);
    }
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM user_ban WHERE m_id IN (100001, 200001, 300001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
