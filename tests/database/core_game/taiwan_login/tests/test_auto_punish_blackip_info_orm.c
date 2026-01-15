#include "auto_punish_blackip_info_orm.h"
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

    printf("=== AutoPunishBlackipInfo ORM Test Suite ===\n\n");

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
        "DELETE FROM auto_punish_blackip_info WHERE ip IN ('192.168.1', '10.0.0')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishBlackipInfo info;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&info, 0, sizeof(AutoPunishBlackipInfo));
    strcpy(info.ip, "192.168.1");
    info.start_ip = 10;
    info.end_ip = 20;
    strcpy(info.reg_date, "2025-11-19 10:00:00");
    info.apply_flag = 1;

    ret = AutoPunishBlackipInfo_Add(manager, &info);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishBlackipInfo_Exists(manager, "192.168.1", 10, 20);
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&info, 0, sizeof(AutoPunishBlackipInfo));
    ret = AutoPunishBlackipInfo_Get(manager, "192.168.1", 10, 20, &info);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishBlackipInfo_PrintInfo(&info);
    }

    /* Test Update */
    info.apply_flag = 0;
    ret = AutoPunishBlackipInfo_Update(manager, &info);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishBlackipInfo_Delete(manager, "192.168.1", 10, 20);
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishBlackipInfo infos[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishBlackipInfo info1, info2;
    memset(&info1, 0, sizeof(AutoPunishBlackipInfo));
    strcpy(info1.ip, "192.168.1");
    info1.start_ip = 1;
    info1.end_ip = 10;
    strcpy(info1.reg_date, "2025-11-19 10:00:00");
    info1.apply_flag = 1;
    AutoPunishBlackipInfo_Add(manager, &info1);

    memset(&info2, 0, sizeof(AutoPunishBlackipInfo));
    strcpy(info2.ip, "10.0.0");
    info2.start_ip = 1;
    info2.end_ip = 255;
    strcpy(info2.reg_date, "2025-11-19 11:00:00");
    info2.apply_flag = 0;
    AutoPunishBlackipInfo_Add(manager, &info2);

    /* Test GetAll */
    ret = AutoPunishBlackipInfo_GetAll(manager, infos, 10, &actual_count);
    printf("  GetAll: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByApplyFlag */
    ret = AutoPunishBlackipInfo_GetByApplyFlag(manager, 1, infos, 10, &actual_count);
    printf("  GetByApplyFlag(1): %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
