#include "max_count_pvp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

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

    printf("=== MaxCountPvp ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("=== All Tests Completed ===\n");

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM max_count_pvp WHERE server_info IN (10, 20, 30)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MaxCountPvp record, records[10];
    int count;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add records */
    printf("  [1] Add records... ");
    memset(&record, 0, sizeof(MaxCountPvp));
    record.server_info = 10;
    record.mc_max = 1000;
    strcpy(record.mc_date, "2025-11-18 10:00:00");

    if (MaxCountPvp_Add(manager, &record) == 0) {
        printf("PASS\n");
        test_passed++;
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 2: Add another record with NOW() */
    printf("  [2] Add record with NOW()... ");
    memset(&record, 0, sizeof(MaxCountPvp));
    record.server_info = 20;
    record.mc_max = 2000;
    record.mc_date[0] = '\0';  /* Use NOW() */

    if (MaxCountPvp_Add(manager, &record) == 0) {
        printf("PASS\n");
        test_passed++;
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 3: Get by server_info */
    printf("  [3] Get by server_info... ");
    if (MaxCountPvp_GetByServerInfo(manager, 10, records, 10, &count) == 0 && count == 1) {
        printf("PASS (found %d record)\n", count);
        test_passed++;
        MaxCountPvp_PrintInfo(&records[0]);
    } else {
        printf("FAIL (expected 1, got %d)\n", count);
        test_failed++;
    }

    /* Test 4: Update by server_info */
    printf("  [4] Update by server_info... ");
    memset(&record, 0, sizeof(MaxCountPvp));
    record.server_info = 10;
    record.mc_max = 1500;
    strcpy(record.mc_date, "2025-11-18 12:00:00");

    if (MaxCountPvp_UpdateByServerInfo(manager, 10, &record) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify update */
        if (MaxCountPvp_GetByServerInfo(manager, 10, records, 10, &count) == 0 && count == 1) {
            if (records[0].mc_max == 1500) {
                printf("    Verified: mc_max updated to %u\n", records[0].mc_max);
            }
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 5: Delete by server_info */
    printf("  [5] Delete by server_info... ");
    if (MaxCountPvp_DeleteByServerInfo(manager, 20) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify deletion */
        if (MaxCountPvp_GetByServerInfo(manager, 20, records, 10, &count) == 0 && count == 0) {
            printf("    Verified: record deleted\n");
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MaxCountPvp record, records[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Clean up before adding test data */
    cleanup_test_data(manager);

    /* Add test data */
    memset(&record, 0, sizeof(MaxCountPvp));
    record.server_info = 10;
    record.mc_max = 1000;
    strcpy(record.mc_date, "2025-11-18 10:00:00");
    MaxCountPvp_Add(manager, &record);

    record.server_info = 20;
    record.mc_max = 2000;
    strcpy(record.mc_date, "2025-11-18 11:00:00");
    MaxCountPvp_Add(manager, &record);

    record.server_info = 30;
    record.mc_max = 3000;
    strcpy(record.mc_date, "2025-11-18 12:00:00");
    MaxCountPvp_Add(manager, &record);

    /* Test 1: Get all records */
    printf("  [1] Get all records... ");
    if (MaxCountPvp_GetAll(manager, records, 10, &count) == 0 && count >= 3) {
        printf("PASS (found %d records)\n", count);
        test_passed++;
        for (int i = 0; i < count && i < 3; i++) {
            printf("    Record %d: server_info=%u, mc_max=%u, mc_date=%s\n",
                   i+1, records[i].server_info, records[i].mc_max, records[i].mc_date);
        }
    } else {
        printf("FAIL (expected >= 3, got %d)\n", count);
        test_failed++;
    }

    /* Test 2: Get by date range */
    printf("  [2] Get by date range... ");
    if (MaxCountPvp_GetByDateRange(manager, "2025-11-18 10:30:00", "2025-11-18 12:30:00",
                                    records, 10, &count) == 0 && count == 2) {
        printf("PASS (found %d records)\n", count);
        test_passed++;
    } else {
        printf("FAIL (expected 2, got %d)\n", count);
        test_failed++;
    }

    /* Test 3: Get latest records */
    printf("  [3] Get latest records (limit 2)... ");
    if (MaxCountPvp_GetLatest(manager, records, 2, &count) == 0 && count == 2) {
        printf("PASS (found %d records)\n", count);
        test_passed++;
        printf("    Latest: server_info=%u, mc_date=%s\n",
               records[0].server_info, records[0].mc_date);
    } else {
        printf("FAIL (expected 2, got %d)\n", count);
        test_failed++;
    }

    printf("\n");
}
