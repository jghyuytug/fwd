#include "dnf_gamein_notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static void test_basic_crud(DBConnectionManager* manager);
static void test_business_operations(DBConnectionManager* manager);
static void cleanup_test_data(DBConnectionManager* manager);

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

    printf("=== DNF Game-in Notice ORM Test Suite ===\n\n");

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

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_gamein_notice WHERE server_id IN (127, 200, 201)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    DnfGameinNotice notice;
    int result;

    printf("[Test 1] CRUD Operations\n");

    /* Test Add with NOW() */
    printf("  1.1 Add notice (auto timestamp)... ");
    memset(&notice, 0, sizeof(DnfGameinNotice));
    strcpy(notice.img_name, "test_notice_1.png");
    notice.server_id = 200;
    notice.reg_time[0] = '\0';  /* Empty string triggers NOW() */
    notice.open_flag = 'y';

    result = DnfGameinNotice_Add(manager, &notice);
    if (result == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test Add with custom timestamp */
    printf("  1.2 Add notice (custom timestamp)... ");
    memset(&notice, 0, sizeof(DnfGameinNotice));
    strcpy(notice.img_name, "test_notice_2.png");
    notice.server_id = 200;
    strcpy(notice.reg_time, "2025-11-18 10:00:00");
    notice.open_flag = 'n';

    result = DnfGameinNotice_Add(manager, &notice);
    if (result == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test Add for server_id = 201 */
    printf("  1.3 Add notice for server 201... ");
    memset(&notice, 0, sizeof(DnfGameinNotice));
    strcpy(notice.img_name, "test_notice_3.png");
    notice.server_id = 201;
    strcpy(notice.reg_time, "2025-11-18 11:00:00");
    notice.open_flag = 'y';

    result = DnfGameinNotice_Add(manager, &notice);
    if (result == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    DnfGameinNotice notices[10];
    int actual_count;
    int result;
    unsigned char actual_server_id;

    printf("[Test 2] Business Operations\n");

    /* First get all to determine actual server_id */
    result = DnfGameinNotice_GetAll(manager, notices, 10, &actual_count);
    if (result == 0 && actual_count > 0) {
        actual_server_id = notices[0].server_id;
    } else {
        actual_server_id = 127; /* fallback */
    }

    /* Test GetByServerId with actual server_id */
    printf("  2.1 Get notices by server_id=%u... ", actual_server_id);
    result = DnfGameinNotice_GetByServerId(manager, actual_server_id, notices, 10, &actual_count);
    if (result == 0 && actual_count >= 1) {
        printf("PASS (found %d notices)\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            printf("      - Notice %u: %s (flag=%c)\n",
                   notices[i].no, notices[i].img_name, notices[i].open_flag);
        }
    } else {
        printf("FAIL (result=%d, count=%d)\n", result, actual_count);
    }

    /* Test GetByOpenFlag */
    printf("  2.2 Get notices by open_flag='y'... ");
    result = DnfGameinNotice_GetByOpenFlag(manager, 'y', notices, 10, &actual_count);
    if (result == 0 && actual_count >= 1) {
        printf("PASS (found %d notices)\n", actual_count);
    } else {
        printf("FAIL (result=%d, count=%d)\n", result, actual_count);
    }

    /* Test GetAll */
    printf("  2.3 Get all notices... ");
    result = DnfGameinNotice_GetAll(manager, notices, 10, &actual_count);
    if (result == 0 && actual_count >= 3) {
        printf("PASS (found %d notices)\n", actual_count);
    } else {
        printf("FAIL (result=%d, count=%d)\n", result, actual_count);
    }

    /* Test PrintInfo */
    if (actual_count > 0) {
        printf("  2.4 Print first notice info:\n");
        DnfGameinNotice_PrintInfo(&notices[0]);
    }

    printf("\n");
}
