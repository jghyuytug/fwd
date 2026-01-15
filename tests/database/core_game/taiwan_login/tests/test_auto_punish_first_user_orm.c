#include "auto_punish_first_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== AutoPunishFirstUser ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_first_user WHERE m_id IN (100001, 100002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishFirstUser user;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&user, 0, sizeof(AutoPunishFirstUser));
    user.m_id = 100001;
    strcpy(user.occ_time, "2025-11-19 10:00:00");
    strcpy(user.ip, "192.168.1.100");
    user.hack_type = 1;
    user.cnt = 5;
    user.punish_flag = 0;
    user.hack_sub_type = 10;
    user.hack_sub_cnt = 3;

    ret = AutoPunishFirstUser_Add(manager, &user);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishFirstUser_Exists(manager, 100001, 1, 10);
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&user, 0, sizeof(AutoPunishFirstUser));
    ret = AutoPunishFirstUser_Get(manager, 100001, 1, 10, &user);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishFirstUser_PrintInfo(&user);
    }

    /* Test Update */
    user.cnt = 10;
    user.punish_flag = 1;
    ret = AutoPunishFirstUser_Update(manager, &user);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishFirstUser_Delete(manager, 100001, 1, 10);
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishFirstUser users[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishFirstUser user1, user2;
    memset(&user1, 0, sizeof(AutoPunishFirstUser));
    user1.m_id = 100001;
    strcpy(user1.occ_time, "2025-11-19 10:00:00");
    strcpy(user1.ip, "192.168.1.100");
    user1.hack_type = 1;
    user1.cnt = 5;
    user1.punish_flag = 1;
    user1.hack_sub_type = 10;
    user1.hack_sub_cnt = 3;
    AutoPunishFirstUser_Add(manager, &user1);

    memset(&user2, 0, sizeof(AutoPunishFirstUser));
    user2.m_id = 100002;
    strcpy(user2.occ_time, "2025-11-19 11:00:00");
    strcpy(user2.ip, "192.168.1.101");
    user2.hack_type = 2;
    user2.cnt = 8;
    user2.punish_flag = 0;
    user2.hack_sub_type = 20;
    user2.hack_sub_cnt = 4;
    AutoPunishFirstUser_Add(manager, &user2);

    /* Test GetByMId */
    ret = AutoPunishFirstUser_GetByMId(manager, 100001, users, 10, &actual_count);
    printf("  GetByMId(100001): %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByPunishFlag */
    ret = AutoPunishFirstUser_GetByPunishFlag(manager, 1, users, 10, &actual_count);
    printf("  GetByPunishFlag(1): %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
