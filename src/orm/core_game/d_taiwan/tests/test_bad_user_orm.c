#include "bad_user_orm.h"
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

    printf("=== BadUser ORM Test Suite ===\n\n");

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
    char query[512];

    /* Delete test records by m_id */
    snprintf(query, sizeof(query),
        "DELETE FROM bad_user WHERE m_id IN (100001, 100002, 100003, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    BadUser user1, user2;
    int ret;

    printf("--- Test 1: Add Bad Users ---\n");

    /* Prepare test data 1 */
    memset(&user1, 0, sizeof(BadUser));
    user1.m_id = 100001;
    user1.bad_code = 10;
    user1.create_day = 20251117;
    user1.exit_day = 20251217;
    user1.admin_n = 1;

    ret = BadUser_Add(manager, &user1);
    printf("Add user1 (m_id=100001): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 2 */
    memset(&user2, 0, sizeof(BadUser));
    user2.m_id = 100002;
    user2.bad_code = 20;
    user2.create_day = 20251118;
    user2.exit_day = 20251218;
    user2.admin_n = 2;

    ret = BadUser_Add(manager, &user2);
    printf("Add user2 (m_id=100002): %s\n", ret == 0 ? "OK" : "FAIL");

    printf("\n--- Test 2: Get All Bad Users ---\n");
    BadUser all_users[100];
    int count = 0;

    ret = BadUser_GetAll(manager, all_users, 100, &count);
    printf("GetAll: %s, Found %d records\n", ret == 0 ? "OK" : "FAIL", count);

    if (count > 0) {
        printf("First record:\n");
        BadUser_PrintInfo(&all_users[0]);
    }

    printf("\n--- Test 3: Get by m_id ---\n");
    BadUser users_by_mid[10];
    int mid_count = 0;

    ret = BadUser_GetByMId(manager, 100001, users_by_mid, 10, &mid_count);
    printf("GetByMId (m_id=100001): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", mid_count);

    if (mid_count > 0) {
        BadUser_PrintInfo(&users_by_mid[0]);
    }

    printf("\n--- Test 4: Update Bad User ---\n");
    if (mid_count > 0) {
        users_by_mid[0].bad_code = 15;
        users_by_mid[0].exit_day = 20251225;

        ret = BadUser_Update(manager, &users_by_mid[0]);
        printf("Update user (no=%u): %s\n", users_by_mid[0].no,
               ret == 0 ? "OK" : "FAIL");

        /* Verify update */
        BadUser updated;
        ret = BadUser_Get(manager, users_by_mid[0].no, &updated);
        printf("Verify update: %s, bad_code=%d, exit_day=%d\n",
               ret == 0 ? "OK" : "FAIL", updated.bad_code, updated.exit_day);
    }

    printf("\n--- Test 5: Exists Check ---\n");
    if (mid_count > 0) {
        int exists = BadUser_Exists(manager, users_by_mid[0].no);
        printf("Exists (no=%u): %s\n", users_by_mid[0].no,
               exists ? "YES" : "NO");

        int not_exists = BadUser_Exists(manager, 999999);
        printf("Exists (no=999999): %s\n", not_exists ? "YES" : "NO");
    }

    printf("\n--- Test 6: Delete Bad User ---\n");
    if (mid_count > 0) {
        ret = BadUser_Delete(manager, users_by_mid[0].no);
        printf("Delete user (no=%u): %s\n", users_by_mid[0].no,
               ret == 0 ? "OK" : "FAIL");

        /* Verify deletion */
        int exists = BadUser_Exists(manager, users_by_mid[0].no);
        printf("Verify deletion: %s\n", exists ? "FAIL (still exists)" : "OK (deleted)");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    BadUser test_users[3];
    int ret;

    printf("\n--- Test 7: Business Query - GetByBadCode ---\n");

    /* Add test data with same bad_code */
    memset(&test_users[0], 0, sizeof(BadUser));
    test_users[0].m_id = 200001;
    test_users[0].bad_code = 30;
    test_users[0].create_day = 20251117;
    test_users[0].exit_day = 20251217;
    test_users[0].admin_n = 3;
    BadUser_Add(manager, &test_users[0]);

    memset(&test_users[1], 0, sizeof(BadUser));
    test_users[1].m_id = 100003;
    test_users[1].bad_code = 30;
    test_users[1].create_day = 20251118;
    test_users[1].exit_day = 20251218;
    test_users[1].admin_n = 3;
    BadUser_Add(manager, &test_users[1]);

    /* Query by bad_code */
    BadUser users_by_code[10];
    int code_count = 0;

    ret = BadUser_GetByBadCode(manager, 30, users_by_code, 10, &code_count);
    printf("GetByBadCode (bad_code=30): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", code_count);

    for (int i = 0; i < code_count; i++) {
        printf("  Record %d: m_id=%d, bad_code=%d\n",
               i + 1, users_by_code[i].m_id, users_by_code[i].bad_code);
    }

    printf("\n--- Test 8: Business Query - GetByExitDay ---\n");
    BadUser users_by_exit[10];
    int exit_count = 0;

    ret = BadUser_GetByExitDay(manager, 20251217, users_by_exit, 10, &exit_count);
    printf("GetByExitDay (exit_day=20251217): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", exit_count);

    for (int i = 0; i < exit_count; i++) {
        printf("  Record %d: m_id=%d, exit_day=%d\n",
               i + 1, users_by_exit[i].m_id, users_by_exit[i].exit_day);
    }
}
