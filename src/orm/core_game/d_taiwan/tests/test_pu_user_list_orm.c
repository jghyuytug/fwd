#include "pu_user_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

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

    printf("=== PU User List ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM pu_user_list WHERE m_id IN (100001, 100002, 100003, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    PuUserList user;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    /* Test 1.1: Add new user */
    printf("1.1 Adding user (m_id=100001)... ");
    memset(&user, 0, sizeof(PuUserList));
    user.m_id = 100001;
    ret = PuUserList_Add(manager, &user);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.2: Check if user exists */
    printf("1.2 Checking if user exists... ");
    ret = PuUserList_Exists(manager, 100001);
    if (ret == 1) {
        printf("OK (exists)\n");
    } else {
        printf("FAILED (not found)\n");
    }

    /* Test 1.3: Get user */
    printf("1.3 Getting user (m_id=100001)... ");
    memset(&user, 0, sizeof(PuUserList));
    ret = PuUserList_Get(manager, 100001, &user);
    if (ret == 0 && user.m_id == 100001) {
        printf("OK\n");
        printf("    ");
        PuUserList_PrintInfo(&user);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.4: Delete user */
    printf("1.4 Deleting user (m_id=100001)... ");
    ret = PuUserList_Delete(manager, 100001);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.5: Verify deletion */
    printf("1.5 Verifying deletion... ");
    ret = PuUserList_Exists(manager, 100001);
    if (ret == 0) {
        printf("OK (not exists)\n");
    } else {
        printf("FAILED (still exists)\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    PuUserList users[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    /* Test 2.1: Add multiple users */
    printf("2.1 Adding multiple users... ");
    for (unsigned int i = 0; i < 3; i++) {
        PuUserList user;
        memset(&user, 0, sizeof(PuUserList));
        user.m_id = 200001 + i;
        PuUserList_Add(manager, &user);
    }
    printf("OK (added 3 users)\n");

    /* Test 2.2: Get all users */
    printf("2.2 Getting all users... ");
    memset(users, 0, sizeof(users));
    ret = PuUserList_GetAll(manager, users, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d users)\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            printf("    [%d] ", i + 1);
            PuUserList_PrintInfo(&users[i]);
        }
    } else {
        printf("FAILED\n");
    }

    printf("\n");
}
