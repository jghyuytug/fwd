/**
 * @file test_village_attacked_server_time_rank_orm.c
 * @brief Test suite for village_attacked_server_time_rank ORM
 *
 * Database: taiwan_login (DB_TYPE_LOGIN = 4)
 * Table: village_attacked_server_time_rank
 * Description: Server-wide fastest clear time rankings for village attack events
 */

#include "../headers/village_attacked_server_time_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Test configuration */
#define TEST_SERVER_INFO 1
#define TEST_OCC_DATE "2025-11-19"
#define TEST_CLEAR_TIME 3600
#define TEST_RANK 1
#define MAX_TEST_RECORDS 100

/* Test Helper Functions */
static void cleanup_test_data(DBConnectionManager* manager);
void test_add_record(DBConnectionManager* manager);
void test_get_record(DBConnectionManager* manager);
void test_exists_record(DBConnectionManager* manager);
void test_update_record(DBConnectionManager* manager);
void test_get_by_server(DBConnectionManager* manager);
void test_get_top_ranks(DBConnectionManager* manager);
void test_get_by_date_range(DBConnectionManager* manager);
void test_print_info(DBConnectionManager* manager);
void test_delete_record(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("Village Attacked Server Time Rank ORM Test Suite\n");
    printf("========================================\n\n");

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("Database connection established\n\n");

    /* Clean up any existing test data */
    cleanup_test_data(&manager);

    /* Run CRUD tests */
    printf("\n--- Basic CRUD Operations ---\n");
    test_add_record(&manager);
    test_get_record(&manager);
    test_exists_record(&manager);
    test_update_record(&manager);

    /* Run business operation tests */
    printf("\n--- Business Operations ---\n");
    test_get_by_server(&manager);
    test_get_top_ranks(&manager);
    test_get_by_date_range(&manager);

    /* Run utility tests */
    printf("\n--- Utility Functions ---\n");
    test_print_info(&manager);

    /* Cleanup tests */
    printf("\n--- Cleanup Operations ---\n");
    test_delete_record(&manager);
    cleanup_test_data(&manager);

    /* Cleanup connection */
    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("All tests passed successfully!\n");
    printf("========================================\n");

    return 0;
}

static void cleanup_test_data(DBConnectionManager* manager) {
    printf("Cleaning up test data...\n");
    VillageAttackedServerTimeRank_Delete(manager, TEST_SERVER_INFO, TEST_OCC_DATE);
    VillageAttackedServerTimeRank_Delete(manager, TEST_SERVER_INFO, "2025-11-18");
    VillageAttackedServerTimeRank_Delete(manager, TEST_SERVER_INFO, "2025-11-17");
    VillageAttackedServerTimeRank_Delete(manager, 2, TEST_OCC_DATE);
    VillageAttackedServerTimeRank_Delete(manager, 3, TEST_OCC_DATE);
}

void test_add_record(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_Add...\n");

    VillageAttackedServerTimeRank record;
    memset(&record, 0, sizeof(record));
    record.server_info = TEST_SERVER_INFO;
    strncpy(record.occ_date, TEST_OCC_DATE, sizeof(record.occ_date) - 1);
    record.occ_date[sizeof(record.occ_date) - 1] = '\0';
    record.clear_time = TEST_CLEAR_TIME;
    record.rank = TEST_RANK;

    int result = VillageAttackedServerTimeRank_Add(manager, &record);
    assert(result == 1);
    printf("  PASSED: Record added successfully\n");
}

void test_get_record(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_Get...\n");

    VillageAttackedServerTimeRank record;
    int result = VillageAttackedServerTimeRank_Get(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE, &record);

    assert(result == 1);
    assert(record.server_info == TEST_SERVER_INFO);
    assert(strcmp(record.occ_date, TEST_OCC_DATE) == 0);
    assert(record.clear_time == TEST_CLEAR_TIME);
    assert(record.rank == TEST_RANK);

    printf("  PASSED: Record retrieved successfully\n");
    printf("    Server: %u, Date: %s, Time: %u sec, Rank: %u\n",
           record.server_info, record.occ_date, record.clear_time, record.rank);
}

void test_exists_record(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_Exists...\n");

    int result = VillageAttackedServerTimeRank_Exists(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE);
    assert(result == 1);

    result = VillageAttackedServerTimeRank_Exists(
        manager, 99, "2099-12-31");
    assert(result == 0);

    printf("  PASSED: Exists check works correctly\n");
}

void test_update_record(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_Update...\n");

    VillageAttackedServerTimeRank record;
    memset(&record, 0, sizeof(record));
    record.server_info = TEST_SERVER_INFO;
    strncpy(record.occ_date, TEST_OCC_DATE, sizeof(record.occ_date) - 1);
    record.occ_date[sizeof(record.occ_date) - 1] = '\0';
    record.clear_time = 2400;
    record.rank = 2;

    int result = VillageAttackedServerTimeRank_Update(manager, &record);
    assert(result == 1);

    VillageAttackedServerTimeRank retrieved;
    VillageAttackedServerTimeRank_Get(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE, &retrieved);
    assert(retrieved.clear_time == 2400);
    assert(retrieved.rank == 2);

    printf("  PASSED: Record updated successfully\n");
    printf("    New clear time: %u sec, New rank: %u\n",
           retrieved.clear_time, retrieved.rank);
}

void test_get_by_server(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_GetByServer...\n");

    /* Add test records for multiple dates */
    VillageAttackedServerTimeRank record1;
    memset(&record1, 0, sizeof(record1));
    record1.server_info = TEST_SERVER_INFO;
    strncpy(record1.occ_date, "2025-11-18", sizeof(record1.occ_date) - 1);
    record1.occ_date[sizeof(record1.occ_date) - 1] = '\0';
    record1.clear_time = 3000;
    record1.rank = 1;
    VillageAttackedServerTimeRank_Add(manager, &record1);

    VillageAttackedServerTimeRank record2;
    memset(&record2, 0, sizeof(record2));
    record2.server_info = TEST_SERVER_INFO;
    strncpy(record2.occ_date, "2025-11-17", sizeof(record2.occ_date) - 1);
    record2.occ_date[sizeof(record2.occ_date) - 1] = '\0';
    record2.clear_time = 3200;
    record2.rank = 1;
    VillageAttackedServerTimeRank_Add(manager, &record2);

    VillageAttackedServerTimeRank records[MAX_TEST_RECORDS];
    int count = VillageAttackedServerTimeRank_GetByServer(
        manager, TEST_SERVER_INFO, records, MAX_TEST_RECORDS);

    assert(count >= 3);
    printf("  PASSED: GetByServer retrieved %d records\n", count);

    for (int i = 0; i < count && i < 5; i++) {
        printf("    [%d] Date: %s, Time: %u sec, Rank: %u\n",
               i, records[i].occ_date, records[i].clear_time, records[i].rank);
    }
}

void test_get_top_ranks(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_GetTopRanks...\n");

    /* Add multiple servers for the same date */
    VillageAttackedServerTimeRank record1;
    memset(&record1, 0, sizeof(record1));
    record1.server_info = 2;
    strncpy(record1.occ_date, TEST_OCC_DATE, sizeof(record1.occ_date) - 1);
    record1.occ_date[sizeof(record1.occ_date) - 1] = '\0';
    record1.clear_time = 2000;
    record1.rank = 1;
    VillageAttackedServerTimeRank_Add(manager, &record1);

    VillageAttackedServerTimeRank record2;
    memset(&record2, 0, sizeof(record2));
    record2.server_info = 3;
    strncpy(record2.occ_date, TEST_OCC_DATE, sizeof(record2.occ_date) - 1);
    record2.occ_date[sizeof(record2.occ_date) - 1] = '\0';
    record2.clear_time = 2500;
    record2.rank = 2;
    VillageAttackedServerTimeRank_Add(manager, &record2);

    VillageAttackedServerTimeRank records[MAX_TEST_RECORDS];
    int count = VillageAttackedServerTimeRank_GetTopRanks(
        manager, TEST_OCC_DATE, 10, records, MAX_TEST_RECORDS);

    assert(count >= 2);
    printf("  PASSED: GetTopRanks retrieved %d records\n", count);

    /* Verify records are sorted by clear_time ASC (fastest first) */
    for (int i = 0; i < count; i++) {
        printf("    [%d] Server: %u, Time: %u sec, Rank: %u\n",
               i, records[i].server_info, records[i].clear_time, records[i].rank);

        if (i > 0) {
            assert(records[i].clear_time >= records[i-1].clear_time);
        }
    }
}

void test_get_by_date_range(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_GetByDateRange...\n");

    VillageAttackedServerTimeRank records[MAX_TEST_RECORDS];
    int count = VillageAttackedServerTimeRank_GetByDateRange(
        manager, TEST_SERVER_INFO, "2025-11-17", "2025-11-19",
        records, MAX_TEST_RECORDS);

    assert(count >= 3);
    printf("  PASSED: GetByDateRange retrieved %d records\n", count);

    for (int i = 0; i < count; i++) {
        printf("    [%d] Date: %s, Time: %u sec, Rank: %u\n",
               i, records[i].occ_date, records[i].clear_time, records[i].rank);
    }
}

void test_print_info(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_PrintInfo...\n");

    VillageAttackedServerTimeRank record;
    VillageAttackedServerTimeRank_Get(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE, &record);

    printf("  Record details:\n");
    VillageAttackedServerTimeRank_PrintInfo(&record);

    printf("  PASSED: PrintInfo executed successfully\n");
}

void test_delete_record(DBConnectionManager* manager) {
    printf("Testing VillageAttackedServerTimeRank_Delete...\n");

    int result = VillageAttackedServerTimeRank_Delete(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE);
    assert(result == 1);

    int exists = VillageAttackedServerTimeRank_Exists(
        manager, TEST_SERVER_INFO, TEST_OCC_DATE);
    assert(exists == 0);

    printf("  PASSED: Record deleted successfully\n");
}
