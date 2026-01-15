#include "auto_punish_hack_full_ip_orm.h"
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

    printf("=== AutoPunishHackFullIp ORM Test Suite ===\n\n");

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
        "DELETE FROM auto_punish_hack_full_ip WHERE full_ip IN ('192.168.1.100', '10.0.0.1')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    AutoPunishHackFullIp info;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test Add */
    memset(&info, 0, sizeof(AutoPunishHackFullIp));
    strcpy(info.occ_date, "2025-11-19");
    info.hack_type = 1;
    info.hack_sub_type = 2;
    strcpy(info.full_ip, "192.168.1.100");
    info.cnt = 5;

    ret = AutoPunishHackFullIp_Add(manager, &info);
    printf("  Add: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Exists */
    ret = AutoPunishHackFullIp_Exists(manager, "2025-11-19", 1, 2, "192.168.1.100");
    printf("  Exists: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test Get */
    memset(&info, 0, sizeof(AutoPunishHackFullIp));
    ret = AutoPunishHackFullIp_Get(manager, "2025-11-19", 1, 2, "192.168.1.100", &info);
    printf("  Get: %s\n", ret == 0 ? "PASS" : "FAIL");
    if (ret == 0) {
        AutoPunishHackFullIp_PrintInfo(&info);
    }

    /* Test Update */
    info.cnt = 10;
    ret = AutoPunishHackFullIp_Update(manager, &info);
    printf("  Update: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test Delete */
    ret = AutoPunishHackFullIp_Delete(manager, "2025-11-19", 1, 2, "192.168.1.100");
    printf("  Delete: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    AutoPunishHackFullIp infos[10];
    int actual_count;
    int ret;

    printf("[TEST] Business Operations\n");

    /* Add test data */
    AutoPunishHackFullIp info1, info2;
    memset(&info1, 0, sizeof(AutoPunishHackFullIp));
    strcpy(info1.occ_date, "2025-11-19");
    info1.hack_type = 1;
    info1.hack_sub_type = 2;
    strcpy(info1.full_ip, "192.168.1.100");
    info1.cnt = 5;
    AutoPunishHackFullIp_Add(manager, &info1);

    memset(&info2, 0, sizeof(AutoPunishHackFullIp));
    strcpy(info2.occ_date, "2025-11-18");
    info2.hack_type = 2;
    info2.hack_sub_type = 3;
    strcpy(info2.full_ip, "10.0.0.1");
    info2.cnt = 10;
    AutoPunishHackFullIp_Add(manager, &info2);

    /* Test GetAll */
    ret = AutoPunishHackFullIp_GetAll(manager, infos, 10, &actual_count);
    printf("  GetAll: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByDate */
    ret = AutoPunishHackFullIp_GetByDate(manager, "2025-11-19", infos, 10, &actual_count);
    printf("  GetByDate: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    /* Test GetByHackType */
    ret = AutoPunishHackFullIp_GetByHackType(manager, 1, infos, 10, &actual_count);
    printf("  GetByHackType: %s (count=%d)\n", ret == 0 ? "PASS" : "FAIL", actual_count);

    printf("\n");
}
