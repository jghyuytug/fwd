#include "max_count_v2_orm.h"
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

    printf("=== MaxCountV2 ORM Test Suite ===\n");
    printf("NOTE: This table has no primary key (statistics table)\n\n");

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
        "DELETE FROM max_count_v2 WHERE server_info IN (1, 2, 3)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_insert_and_query(DBConnectionManager* manager) {
    MaxCountV2 record1, record2, record3;
    int ret;

    printf("--- Test 1: Add Records ---\n");

    /* Prepare test data 1 */
    memset(&record1, 0, sizeof(MaxCountV2));
    record1.server_info = 1;
    record1.num_occupations_charscreen = 100;
    record1.num_occupations_seriaroom = 50;
    record1.num_login_per_min = 20;
    record1.num_logout_per_min = 15;
    strcpy(record1.mc_date, "2025-11-18 10:00:00");

    ret = MaxCountV2_Add(manager, &record1);
    printf("Add record1 (server_info=1): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 2 */
    memset(&record2, 0, sizeof(MaxCountV2));
    record2.server_info = 1;
    record2.num_occupations_charscreen = 150;
    record2.num_occupations_seriaroom = 75;
    record2.num_login_per_min = 30;
    record2.num_logout_per_min = 25;
    strcpy(record2.mc_date, "2025-11-18 11:00:00");

    ret = MaxCountV2_Add(manager, &record2);
    printf("Add record2 (server_info=1): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 3 */
    memset(&record3, 0, sizeof(MaxCountV2));
    record3.server_info = 2;
    record3.num_occupations_charscreen = 200;
    record3.num_occupations_seriaroom = 100;
    record3.num_login_per_min = 40;
    record3.num_logout_per_min = 35;
    strcpy(record3.mc_date, "2025-11-18 12:00:00");

    ret = MaxCountV2_Add(manager, &record3);
    printf("Add record3 (server_info=2): %s\n", ret == 0 ? "OK" : "FAIL");

    printf("\n--- Test 2: Get All Records ---\n");
    MaxCountV2 all_records[100];
    int count = 0;

    ret = MaxCountV2_GetAll(manager, all_records, 100, &count);
    printf("GetAll: %s, Found %d records\n", ret == 0 ? "OK" : "FAIL", count);

    if (count > 0) {
        printf("First record (latest by date):\n");
        MaxCountV2_PrintInfo(&all_records[0]);
    }

    printf("\n--- Test 3: Get by server_info ---\n");
    MaxCountV2 records_by_server[10];
    int server_count = 0;

    ret = MaxCountV2_GetByServerInfo(manager, 1, records_by_server, 10, &server_count);
    printf("GetByServerInfo (server_info=1): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", server_count);

    for (int i = 0; i < server_count; i++) {
        printf("  Record %d: charscreen=%u, seriaroom=%u, login=%u, logout=%u, date=%s\n",
               i + 1, records_by_server[i].num_occupations_charscreen,
               records_by_server[i].num_occupations_seriaroom,
               records_by_server[i].num_login_per_min,
               records_by_server[i].num_logout_per_min,
               records_by_server[i].mc_date);
    }

    printf("\n--- Test 4: Get Latest Record ---\n");
    MaxCountV2 latest;

    ret = MaxCountV2_GetLatest(manager, 1, &latest);
    printf("GetLatest (server_info=1): %s\n", ret == 0 ? "OK" : "FAIL");

    if (ret == 0) {
        MaxCountV2_PrintInfo(&latest);
        if (latest.num_occupations_charscreen == 150) {
            printf("Latest record verification: OK (charscreen=150)\n");
        } else {
            printf("Latest record verification: FAIL (expected 150, got %u)\n",
                   latest.num_occupations_charscreen);
        }
    }
}

void test_business_operations(DBConnectionManager* manager) {
    MaxCountV2 test_records[3];
    int ret;

    printf("\n--- Test 5: Get by Date Range ---\n");

    /* Add more test data with specific dates */
    memset(&test_records[0], 0, sizeof(MaxCountV2));
    test_records[0].server_info = 3;
    test_records[0].num_occupations_charscreen = 300;
    test_records[0].num_occupations_seriaroom = 150;
    test_records[0].num_login_per_min = 50;
    test_records[0].num_logout_per_min = 45;
    strcpy(test_records[0].mc_date, "2025-11-17 10:00:00");
    MaxCountV2_Add(manager, &test_records[0]);

    memset(&test_records[1], 0, sizeof(MaxCountV2));
    test_records[1].server_info = 3;
    test_records[1].num_occupations_charscreen = 350;
    test_records[1].num_occupations_seriaroom = 175;
    test_records[1].num_login_per_min = 60;
    test_records[1].num_logout_per_min = 55;
    strcpy(test_records[1].mc_date, "2025-11-18 10:00:00");
    MaxCountV2_Add(manager, &test_records[1]);

    memset(&test_records[2], 0, sizeof(MaxCountV2));
    test_records[2].server_info = 3;
    test_records[2].num_occupations_charscreen = 400;
    test_records[2].num_occupations_seriaroom = 200;
    test_records[2].num_login_per_min = 70;
    test_records[2].num_logout_per_min = 65;
    strcpy(test_records[2].mc_date, "2025-11-19 10:00:00");
    MaxCountV2_Add(manager, &test_records[2]);

    /* Query by date range */
    MaxCountV2 records_by_date[10];
    int date_count = 0;

    ret = MaxCountV2_GetByDateRange(manager, "2025-11-18 00:00:00", "2025-11-18 23:59:59",
                                     records_by_date, 10, &date_count);
    printf("GetByDateRange (2025-11-18): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", date_count);

    for (int i = 0; i < date_count; i++) {
        printf("  Record %d: server_info=%u, charscreen=%u, login/min=%u\n",
               i + 1, records_by_date[i].server_info,
               records_by_date[i].num_occupations_charscreen,
               records_by_date[i].num_login_per_min);
    }

    printf("\n--- Test 6: Delete by server_info ---\n");

    ret = MaxCountV2_DeleteByServerInfo(manager, 3);
    printf("DeleteByServerInfo (server_info=3): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Verify deletion */
    MaxCountV2 verify_records[10];
    int verify_count = 0;

    ret = MaxCountV2_GetByServerInfo(manager, 3, verify_records, 10, &verify_count);
    printf("Verify deletion: %s (found %d records, expected 0)\n",
           verify_count == 0 ? "OK" : "FAIL", verify_count);

    printf("\n--- Test 7: Delete by Date Range ---\n");

    /* Add one more record for deletion test */
    MaxCountV2 temp_record;
    memset(&temp_record, 0, sizeof(MaxCountV2));
    temp_record.server_info = 2;
    temp_record.num_occupations_charscreen = 500;
    temp_record.num_occupations_seriaroom = 250;
    temp_record.num_login_per_min = 80;
    temp_record.num_logout_per_min = 75;
    strcpy(temp_record.mc_date, "2025-11-20 10:00:00");
    MaxCountV2_Add(manager, &temp_record);

    ret = MaxCountV2_DeleteByDateRange(manager, "2025-11-20 00:00:00", "2025-11-20 23:59:59");
    printf("DeleteByDateRange (2025-11-20): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Verify deletion */
    ret = MaxCountV2_GetByDateRange(manager, "2025-11-20 00:00:00", "2025-11-20 23:59:59",
                                     verify_records, 10, &verify_count);
    printf("Verify deletion: %s (found %d records, expected 0)\n",
           verify_count == 0 ? "OK" : "FAIL", verify_count);
}
