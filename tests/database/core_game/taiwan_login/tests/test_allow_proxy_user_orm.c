#include "allow_proxy_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

void test_basic_operations(DBConnectionManager* manager);
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== AllowProxyUser ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("\n=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_operations(DBConnectionManager* manager) {
    AllowProxyUser users[10];
    int actual_count;

    printf("[TEST] Basic Operations\n");

    /* Test 1: Add user */
    test_total++;
    if (AllowProxyUser_Add(manager, 100001) == 0) {
        printf("  [PASS] Add user (m_id=100001)\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add user\n");
    }

    /* Test 2: Check exists */
    test_total++;
    if (AllowProxyUser_Exists(manager, 100001) == 1) {
        printf("  [PASS] User exists\n");
        test_passed++;
    } else {
        printf("  [FAIL] User should exist\n");
    }

    /* Test 3: Add multiple users */
    test_total++;
    AllowProxyUser_Add(manager, 100002);
    AllowProxyUser_Add(manager, 100003);

    int count = AllowProxyUser_Count(manager);
    if (count == 3) {
        printf("  [PASS] Add multiple users (count=%d)\n", count);
        test_passed++;
    } else {
        printf("  [FAIL] Add multiple users - expected 3, got %d\n", count);
    }

    /* Test 4: Get all users */
    test_total++;
    memset(users, 0, sizeof(users));

    if (AllowProxyUser_GetAll(manager, users, 10, &actual_count) == 0) {
        if (actual_count == 3) {
            printf("  [PASS] Get all users (count=%d)\n", actual_count);
            test_passed++;
        } else {
            printf("  [FAIL] Get all users - expected 3, got %d\n", actual_count);
        }
    } else {
        printf("  [FAIL] Get all users\n");
    }

    /* Test 5: Delete single user */
    test_total++;
    if (AllowProxyUser_Delete(manager, 100001) == 0) {
        if (AllowProxyUser_Exists(manager, 100001) == 0) {
            printf("  [PASS] Delete user\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete user - still exists\n");
        }
    } else {
        printf("  [FAIL] Delete user\n");
    }

    /* Test 6: Count after deletion */
    test_total++;
    count = AllowProxyUser_Count(manager);
    if (count == 2) {
        printf("  [PASS] Count after deletion (count=%d)\n", count);
        test_passed++;
    } else {
        printf("  [FAIL] Count after deletion - expected 2, got %d\n", count);
    }

    /* Test 7: Delete all */
    test_total++;
    if (AllowProxyUser_DeleteAll(manager) == 0) {
        count = AllowProxyUser_Count(manager);
        if (count == 0) {
            printf("  [PASS] Delete all users\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete all - %d users remain\n", count);
        }
    } else {
        printf("  [FAIL] Delete all users\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    AllowProxyUser_Delete(manager, 100001);
    AllowProxyUser_Delete(manager, 100002);
    AllowProxyUser_Delete(manager, 100003);
}
