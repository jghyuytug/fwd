#include "hack_cleanpad_ratio_info_orm.h"
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

    printf("=== Hack Cleanpad Ratio Info ORM Test Suite ===\n\n");

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
        "DELETE FROM hack_cleanpad_ratio_info WHERE hack_type IN (101, 102, 103)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    HackCleanpadRatioInfo info;
    HackCleanpadRatioInfo retrieved;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add with custom date */
    memset(&info, 0, sizeof(HackCleanpadRatioInfo));
    info.hack_type = 101;
    info.value = 5000;
    strncpy(info.reg_date, "2025-11-19 10:30:00", sizeof(info.reg_date) - 1);

    ret = HackCleanpadRatioInfo_Add(manager, &info);
    printf("  Add with custom date: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 2: Add with NOW() */
    memset(&info, 0, sizeof(HackCleanpadRatioInfo));
    info.hack_type = 102;
    info.value = 7500;
    info.reg_date[0] = '\0';

    ret = HackCleanpadRatioInfo_Add(manager, &info);
    printf("  Add with NOW(): %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 3: Exists */
    ret = HackCleanpadRatioInfo_Exists(manager, 101);
    printf("  Exists check: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test 4: Get */
    memset(&retrieved, 0, sizeof(HackCleanpadRatioInfo));
    ret = HackCleanpadRatioInfo_Get(manager, 101, &retrieved);
    printf("  Get info: %s\n", ret == 0 ? "PASS" : "FAIL");

    if (ret == 0) {
        int match = (retrieved.hack_type == 101 && retrieved.value == 5000);
        printf("  Verify data: %s (hack_type=%u, value=%u)\n",
               match ? "PASS" : "FAIL", retrieved.hack_type, retrieved.value);
        printf("    reg_date: %s\n", retrieved.reg_date);
    }

    /* Test 5: Update */
    retrieved.value = 9999;
    ret = HackCleanpadRatioInfo_Update(manager, &retrieved);
    printf("  Update value: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 6: Verify update */
    memset(&retrieved, 0, sizeof(HackCleanpadRatioInfo));
    ret = HackCleanpadRatioInfo_Get(manager, 101, &retrieved);
    if (ret == 0) {
        int match = (retrieved.value == 9999);
        printf("  Verify update: %s (value=%u)\n",
               match ? "PASS" : "FAIL", retrieved.value);
    }

    /* Test 7: Delete */
    ret = HackCleanpadRatioInfo_Delete(manager, 102);
    printf("  Delete info: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 8: Verify deletion */
    ret = HackCleanpadRatioInfo_Exists(manager, 102);
    printf("  Verify deletion: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    HackCleanpadRatioInfo infos[10];
    int count;
    int ret;
    int i;

    printf("[TEST] Business Operations\n");

    /* Add multiple records */
    for (i = 0; i < 3; i++) {
        HackCleanpadRatioInfo info;
        memset(&info, 0, sizeof(HackCleanpadRatioInfo));
        info.hack_type = (unsigned short)(101 + i);
        info.value = (unsigned int)(5000 + i * 1000);
        info.reg_date[0] = '\0';
        HackCleanpadRatioInfo_Add(manager, &info);
    }

    /* Test 1: GetAll */
    memset(infos, 0, sizeof(infos));
    ret = HackCleanpadRatioInfo_GetAll(manager, infos, 10, &count);
    printf("  GetAll: %s (found %d records)\n",
           ret == 0 && count >= 3 ? "PASS" : "FAIL", count);

    /* Test 2: GetCount */
    ret = HackCleanpadRatioInfo_GetCount(manager, &count);
    printf("  GetCount: %s (count=%d)\n",
           ret == 0 && count >= 3 ? "PASS" : "FAIL", count);

    /* Test 3: PrintInfo */
    if (count > 0) {
        printf("  PrintInfo test:\n");
        HackCleanpadRatioInfo_PrintInfo(&infos[0]);
    }

    printf("\n");
}
