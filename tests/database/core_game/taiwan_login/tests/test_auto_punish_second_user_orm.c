#include "auto_punish_second_user_orm.h"
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

    printf("=== AutoPunishSecondUser ORM Test Suite ===\n\n");

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
        "DELETE FROM auto_punish_second_user WHERE m_id IN (99991, 99992)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishSecondUser info;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&info, 0, sizeof(AutoPunishSecondUser));
    info.m_id = 99991;
    strcpy(info.occ_time, "2025-11-19 10:00:00");
    info.total_trade_cnt = 100;
    info.trade_cnt = 10;
    info.total_trade_gold = 10000000;
    info.trade_gold = 1000000;
    info.punish_flag = 0;

    ret = AutoPunishSecondUser_Add(manager, &info);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishSecondUser_Exists(manager, 99991);
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&info, 0, sizeof(AutoPunishSecondUser));
    ret = AutoPunishSecondUser_Get(manager, 99991, &info);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishSecondUser_PrintInfo(&info);
    }

    /* Test Update */
    info.trade_cnt = 20;
    info.trade_gold = 2000000;
    info.punish_flag = 1;
    ret = AutoPunishSecondUser_Update(manager, &info);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishSecondUser_Delete(manager, 99991);
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishSecondUser infos[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishSecondUser info1, info2;
    memset(&info1, 0, sizeof(AutoPunishSecondUser));
    info1.m_id = 99991;
    strcpy(info1.occ_time, "2025-11-19 10:00:00");
    info1.total_trade_cnt = 100;
    info1.trade_cnt = 10;
    info1.total_trade_gold = 10000000;
    info1.trade_gold = 1000000;
    info1.punish_flag = 1;
    AutoPunishSecondUser_Add(manager, &info1);

    memset(&info2, 0, sizeof(AutoPunishSecondUser));
    info2.m_id = 99992;
    strcpy(info2.occ_time, "2025-11-19 11:00:00");
    info2.total_trade_cnt = 50;
    info2.trade_cnt = 5;
    info2.total_trade_gold = 5000000;
    info2.trade_gold = 500000;
    info2.punish_flag = 0;
    AutoPunishSecondUser_Add(manager, &info2);

    /* Test GetAll */
    ret = AutoPunishSecondUser_GetAll(manager, infos, 10, &actual_count);
    printf("  GetAll: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByPunishFlag */
    ret = AutoPunishSecondUser_GetByPunishFlag(manager, 1, infos, 10, &actual_count);
    printf("  GetByPunishFlag: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetTopTraders */
    ret = AutoPunishSecondUser_GetTopTraders(manager, infos, 10, &actual_count);
    printf("  GetTopTraders: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
