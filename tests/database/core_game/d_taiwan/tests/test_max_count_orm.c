#include "max_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

void test_insert_and_query(DBConnectionManager* manager);
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

    printf("=== MaxCount ORM Test Suite ===\n");
    printf("NOTE: This table has no primary key\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_insert_and_query(&manager);
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

    /* Delete test records by server_info */
    snprintf(query, sizeof(query),
        "DELETE FROM max_count WHERE server_info IN (1, 2, 3)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_insert_and_query(DBConnectionManager* manager) {
    MaxCount record1, record2, record3;
    int ret;

    printf("--- Test 1: Add Records ---\n");

    /* Prepare test data 1 */
    memset(&record1, 0, sizeof(MaxCount));
    record1.server_info = 1;
    record1.mc_max = 1000;
    strcpy(record1.mc_date, "2025-11-18 10:00:00");

    ret = MaxCount_Add(manager, &record1);
    printf("Add record1 (server_info=1, mc_max=1000): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 2 */
    memset(&record2, 0, sizeof(MaxCount));
    record2.server_info = 1;
    record2.mc_max = 1500;
    strcpy(record2.mc_date, "2025-11-18 11:00:00");

    ret = MaxCount_Add(manager, &record2);
    printf("Add record2 (server_info=1, mc_max=1500): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 3 */
    memset(&record3, 0, sizeof(MaxCount));
    record3.server_info = 2;
    record3.mc_max = 2000;
    strcpy(record3.mc_date, "2025-11-18 12:00:00");

    ret = MaxCount_Add(manager, &record3);
    printf("Add record3 (server_info=2, mc_max=2000): %s\n", ret == 0 ? "OK" : "FAIL");

    printf("\n--- Test 2: Get All Records ---\n");
    MaxCount all_records[100];
    int count = 0;

    ret = MaxCount_GetAll(manager, all_records, 100, &count);
    printf("GetAll: %s, Found %d records\n", ret == 0 ? "OK" : "FAIL", count);

    if (count > 0) {
        printf("First record (latest by date):\n");
        MaxCount_PrintInfo(&all_records[0]);
    }

    printf("\n--- Test 3: Get by server_info ---\n");
    MaxCount records_by_server[10];
    int server_count = 0;

    ret = MaxCount_GetByServerInfo(manager, 1, records_by_server, 10, &server_count);
    printf("GetByServerInfo (server_info=1): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", server_count);

    for (int i = 0; i < server_count; i++) {
        printf("  Record %d: mc_max=%u, mc_date=%s\n",
               i + 1, records_by_server[i].mc_max, records_by_server[i].mc_date);
    }

    printf("\n--- Test 4: Get Latest Record ---\n");
    MaxCount latest;

    ret = MaxCount_GetLatest(manager, 1, &latest);
    printf("GetLatest (server_info=1): %s\n", ret == 0 ? "OK" : "FAIL");

    if (ret == 0) {
        MaxCount_PrintInfo(&latest);
        if (latest.mc_max == 1500) {
            printf("Latest record verification: OK (mc_max=1500)\n");
        } else {
            printf("Latest record verification: FAIL (expected 1500, got %u)\n", latest.mc_max);
        }
    }
}

void test_business_operations(DBConnectionManager* manager) {
    MaxCount test_records[3];
    int ret;

    printf("\n--- Test 5: Get by Date Range ---\n");

    /* Add more test data with specific dates */
    memset(&test_records[0], 0, sizeof(MaxCount));
    test_records[0].server_info = 3;
    test_records[0].mc_max = 3000;
    strcpy(test_records[0].mc_date, "2025-11-17 10:00:00");
    MaxCount_Add(manager, &test_records[0]);

    memset(&test_records[1], 0, sizeof(MaxCount));
    test_records[1].server_info = 3;
    test_records[1].mc_max = 3500;
    strcpy(test_records[1].mc_date, "2025-11-18 10:00:00");
    MaxCount_Add(manager, &test_records[1]);

    memset(&test_records[2], 0, sizeof(MaxCount));
    test_records[2].server_info = 3;
    test_records[2].mc_max = 4000;
    strcpy(test_records[2].mc_date, "2025-11-19 10:00:00");
    MaxCount_Add(manager, &test_records[2]);

    /* Query by date range */
    MaxCount records_by_date[10];
    int date_count = 0;

    ret = MaxCount_GetByDateRange(manager, "2025-11-18 00:00:00", "2025-11-18 23:59:59",
                                   records_by_date, 10, &date_count);
    printf("GetByDateRange (2025-11-18): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", date_count);

    for (int i = 0; i < date_count; i++) {
        printf("  Record %d: server_info=%u, mc_max=%u, mc_date=%s\n",
               i + 1, records_by_date[i].server_info,
               records_by_date[i].mc_max, records_by_date[i].mc_date);
    }

    printf("\n--- Test 6: Delete by server_info ---\n");

    ret = MaxCount_DeleteByServerInfo(manager, 3);
    printf("DeleteByServerInfo (server_info=3): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Verify deletion */
    MaxCount verify_records[10];
    int verify_count = 0;

    ret = MaxCount_GetByServerInfo(manager, 3, verify_records, 10, &verify_count);
    printf("Verify deletion: %s (found %d records, expected 0)\n",
           verify_count == 0 ? "OK" : "FAIL", verify_count);

    printf("\n--- Test 7: Delete by Date Range ---\n");

    /* Add one more record for deletion test */
    MaxCount temp_record;
    memset(&temp_record, 0, sizeof(MaxCount));
    temp_record.server_info = 2;
    temp_record.mc_max = 5000;
    strcpy(temp_record.mc_date, "2025-11-20 10:00:00");
    MaxCount_Add(manager, &temp_record);

    ret = MaxCount_DeleteByDateRange(manager, "2025-11-20 00:00:00", "2025-11-20 23:59:59");
    printf("DeleteByDateRange (2025-11-20): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Verify deletion */
    ret = MaxCount_GetByDateRange(manager, "2025-11-20 00:00:00", "2025-11-20 23:59:59",
                                   verify_records, 10, &verify_count);
    printf("Verify deletion: %s (found %d records, expected 0)\n",
           verify_count == 0 ? "OK" : "FAIL", verify_count);
}
