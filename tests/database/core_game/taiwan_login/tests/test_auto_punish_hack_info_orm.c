#include "auto_punish_hack_info_orm.h"
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

    printf("=== AutoPunishHackInfo ORM Test Suite ===\n\n");

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
        "DELETE FROM auto_punish_hack_info WHERE hack_type IN (100, 200)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishHackInfo info;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&info, 0, sizeof(AutoPunishHackInfo));
    info.hack_type = 100;
    info.cnt = 10;
    info.etc = 12345;
    strcpy(info.reg_date, "2025-11-19 10:00:00");
    info.apply_flag = 1;
    info.hack_sub_type = 5;
    info.hack_sub_cnt = 3;
    info.ip_cnt = 2;

    ret = AutoPunishHackInfo_Add(manager, &info);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishHackInfo_Exists(manager, 100, 1, 5);
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&info, 0, sizeof(AutoPunishHackInfo));
    ret = AutoPunishHackInfo_Get(manager, 100, 1, 5, &info);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishHackInfo_PrintInfo(&info);
    }

    /* Test Update */
    info.cnt = 20;
    info.ip_cnt = 5;
    ret = AutoPunishHackInfo_Update(manager, &info);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishHackInfo_Delete(manager, 100, 1, 5);
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishHackInfo infos[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishHackInfo info1, info2;
    memset(&info1, 0, sizeof(AutoPunishHackInfo));
    info1.hack_type = 100;
    info1.cnt = 10;
    info1.etc = 12345;
    strcpy(info1.reg_date, "2025-11-19 10:00:00");
    info1.apply_flag = 1;
    info1.hack_sub_type = 5;
    info1.hack_sub_cnt = 3;
    info1.ip_cnt = 2;
    AutoPunishHackInfo_Add(manager, &info1);

    memset(&info2, 0, sizeof(AutoPunishHackInfo));
    info2.hack_type = 200;
    info2.cnt = 5;
    info2.etc = 67890;
    strcpy(info2.reg_date, "2025-11-19 11:00:00");
    info2.apply_flag = 0;
    info2.hack_sub_type = 3;
    info2.hack_sub_cnt = 1;
    info2.ip_cnt = 1;
    AutoPunishHackInfo_Add(manager, &info2);

    /* Test GetAll */
    ret = AutoPunishHackInfo_GetAll(manager, infos, 10, &actual_count);
    printf("  GetAll: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByHackType */
    ret = AutoPunishHackInfo_GetByHackType(manager, 100, infos, 10, &actual_count);
    printf("  GetByHackType: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByApplyFlag */
    ret = AutoPunishHackInfo_GetByApplyFlag(manager, 1, infos, 10, &actual_count);
    printf("  GetByApplyFlag: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
