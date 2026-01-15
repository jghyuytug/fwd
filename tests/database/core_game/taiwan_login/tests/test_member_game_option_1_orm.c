#include "member_game_option_1_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_total = 0;
static int test_passed = 0;

void cleanup_test_data(DBConnectionManager* manager);
void test_basic_crud(DBConnectionManager* manager);

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

    printf("=== Member Game Option 1 ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);

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
        "DELETE FROM member_game_option_1 WHERE m_id IN (300001, 300002, 300003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberGameOption1 option, retrieved;
    int ret;

    printf("--- Test Basic CRUD Operations ---\n");

    /* Test 1: Add option */
    test_total++;
    MemberGameOption1_Init(&option);
    option.m_id = 300001;

    ret = MemberGameOption1_Add(manager, &option);
    if (ret == 0) {
        printf("[PASS] Test 1: Add option\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 1: Add option (returned %d)\n", ret);
    }

    /* Test 2: Get option */
    test_total++;
    MemberGameOption1_Init(&retrieved);
    ret = MemberGameOption1_Get(manager, 300001, &retrieved);
    if (ret == 0 && retrieved.m_id == 300001) {
        printf("[PASS] Test 2: Get option\n");
        test_passed++;
        MemberGameOption1_PrintInfo(&retrieved);
    } else {
        printf("[FAIL] Test 2: Get option (returned %d)\n", ret);
    }
    MemberGameOption1_Free(&retrieved);

    /* Test 3: Exists check */
    test_total++;
    ret = MemberGameOption1_Exists(manager, 300001);
    if (ret > 0) {
        printf("[PASS] Test 3: Exists check (count=%d)\n", ret);
        test_passed++;
    } else {
        printf("[FAIL] Test 3: Exists check (returned %d)\n", ret);
    }

    /* Test 4: Update option */
    test_total++;
    ret = MemberGameOption1_Update(manager, &option);
    if (ret == 0) {
        printf("[PASS] Test 4: Update option\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 4: Update option (returned %d)\n", ret);
    }

    /* Test 5: Delete option */
    test_total++;
    ret = MemberGameOption1_Delete(manager, 300001);
    if (ret == 0) {
        printf("[PASS] Test 5: Delete option\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 5: Delete option (returned %d)\n", ret);
    }

    /* Test 6: Verify deletion */
    test_total++;
    ret = MemberGameOption1_Exists(manager, 300001);
    if (ret == 0) {
        printf("[PASS] Test 6: Verify deletion\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 6: Verify deletion (still exists: %d)\n", ret);
    }

    /* Test 7: Add multiple options */
    test_total++;
    MemberGameOption1_Init(&option);
    option.m_id = 300002;
    MemberGameOption1_Add(manager, &option);

    option.m_id = 300003;
    MemberGameOption1_Add(manager, &option);

    ret = MemberGameOption1_Exists(manager, 300002);
    if (ret > 0 && MemberGameOption1_Exists(manager, 300003) > 0) {
        printf("[PASS] Test 7: Add multiple options\n");
        test_passed++;
    } else {
        printf("[FAIL] Test 7: Add multiple options\n");
    }

    MemberGameOption1_Free(&option);

    printf("\n");
}
