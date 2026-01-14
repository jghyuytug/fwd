#include "auto_punish_second_log_orm.h"
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

    printf("=== AutoPunishSecondLog ORM Test Suite ===\n\n");

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
        "DELETE FROM auto_punish_second_log WHERE m_id IN (99991, 99992)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishSecondLog info;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&info, 0, sizeof(AutoPunishSecondLog));
    info.m_id = 99991;
    info.hack_m_id = 88881;
    strcpy(info.occ_time, "2025-11-19 10:00:00");
    info.trade_cnt = 5;
    info.trade_gold = 1000000;

    ret = AutoPunishSecondLog_Add(manager, &info);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishSecondLog_Exists(manager, 99991, 88881);
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&info, 0, sizeof(AutoPunishSecondLog));
    ret = AutoPunishSecondLog_Get(manager, 99991, 88881, &info);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishSecondLog_PrintInfo(&info);
    }

    /* Test Update */
    info.trade_cnt = 10;
    info.trade_gold = 2000000;
    ret = AutoPunishSecondLog_Update(manager, &info);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishSecondLog_Delete(manager, 99991, 88881);
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishSecondLog infos[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishSecondLog info1, info2;
    memset(&info1, 0, sizeof(AutoPunishSecondLog));
    info1.m_id = 99991;
    info1.hack_m_id = 88881;
    strcpy(info1.occ_time, "2025-11-19 10:00:00");
    info1.trade_cnt = 5;
    info1.trade_gold = 1000000;
    AutoPunishSecondLog_Add(manager, &info1);

    memset(&info2, 0, sizeof(AutoPunishSecondLog));
    info2.m_id = 99992;
    info2.hack_m_id = 88881;
    strcpy(info2.occ_time, "2025-11-19 11:00:00");
    info2.trade_cnt = 3;
    info2.trade_gold = 500000;
    AutoPunishSecondLog_Add(manager, &info2);

    /* Test GetAll */
    ret = AutoPunishSecondLog_GetAll(manager, infos, 10, &actual_count);
    printf("  GetAll: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByMemberId */
    ret = AutoPunishSecondLog_GetByMemberId(manager, 99991, infos, 10, &actual_count);
    printf("  GetByMemberId: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByHackMemberId */
    ret = AutoPunishSecondLog_GetByHackMemberId(manager, 88881, infos, 10, &actual_count);
    printf("  GetByHackMemberId: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
