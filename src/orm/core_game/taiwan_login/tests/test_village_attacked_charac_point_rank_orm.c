#include "../headers/village_attacked_charac_point_rank_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Test configuration - NO Chinese characters allowed */
#define TEST_SERVER_INFO 1
#define TEST_OCC_DATE "2025-11-19"
#define TEST_CHARAC_NO 100001
#define TEST_HUNTING_POINT 5000
#define TEST_RANK 1

/* Test helper functions */
void test_add(DBConnectionManager* manager);
void test_get(DBConnectionManager* manager);
void test_exists(DBConnectionManager* manager);
void test_update(DBConnectionManager* manager);
void test_get_by_server(DBConnectionManager* manager);
void test_get_top_ranks(DBConnectionManager* manager);
void test_get_by_character(DBConnectionManager* manager);
void test_delete(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("Village Attacked Character Point Rank ORM Test Suite\n");
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

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests in sequence */
    test_add(&manager);
    test_get(&manager);
    test_exists(&manager);
    test_update(&manager);
    test_get_by_server(&manager);
    test_get_top_ranks(&manager);
    test_get_by_character(&manager);
    test_delete(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("All tests passed successfully!\n");
    printf("========================================\n");

    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    VillageAttackedCharacPointRank_Delete(manager, TEST_SERVER_INFO, TEST_OCC_DATE, TEST_CHARAC_NO);
    VillageAttackedCharacPointRank_Delete(manager, TEST_SERVER_INFO, TEST_OCC_DATE, 100002);
}

void test_add(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_Add\n");

    VillageAttackedCharacPointRank record;
    memset(&record, 0, sizeof(record));
    record.server_info = TEST_SERVER_INFO;
    strncpy(record.occ_date, TEST_OCC_DATE, sizeof(record.occ_date) - 1);
    record.occ_date[sizeof(record.occ_date) - 1] = '\0';
    record.charac_no = TEST_CHARAC_NO;
    record.hunting_point = TEST_HUNTING_POINT;
    record.rank = TEST_RANK;

    int result = VillageAttackedCharacPointRank_Add(manager, &record);
    assert(result == 1);
    printf("  PASS: Record added successfully\n\n");
}

void test_get(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_Get\n");

    VillageAttackedCharacPointRank record;
    memset(&record, 0, sizeof(record));

    int result = VillageAttackedCharacPointRank_Get(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        TEST_CHARAC_NO,
        &record
    );

    assert(result == 1);
    assert(record.server_info == TEST_SERVER_INFO);
    assert(strcmp(record.occ_date, TEST_OCC_DATE) == 0);
    assert(record.charac_no == TEST_CHARAC_NO);
    assert(record.hunting_point == TEST_HUNTING_POINT);
    assert(record.rank == TEST_RANK);

    printf("  PASS: Record retrieved successfully\n");
    VillageAttackedCharacPointRank_PrintInfo(&record);
    printf("\n");
}

void test_exists(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_Exists\n");

    int result = VillageAttackedCharacPointRank_Exists(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        TEST_CHARAC_NO
    );

    assert(result == 1);
    printf("  PASS: Record exists\n\n");
}

void test_update(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_Update\n");

    VillageAttackedCharacPointRank record;
    memset(&record, 0, sizeof(record));
    record.server_info = TEST_SERVER_INFO;
    strncpy(record.occ_date, TEST_OCC_DATE, sizeof(record.occ_date) - 1);
    record.occ_date[sizeof(record.occ_date) - 1] = '\0';
    record.charac_no = TEST_CHARAC_NO;
    record.hunting_point = 8000;
    record.rank = 2;

    int result = VillageAttackedCharacPointRank_Update(manager, &record);
    assert(result == 1);

    /* Verify update */
    VillageAttackedCharacPointRank retrieved;
    VillageAttackedCharacPointRank_Get(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        TEST_CHARAC_NO,
        &retrieved
    );

    assert(retrieved.hunting_point == 8000);
    assert(retrieved.rank == 2);

    printf("  PASS: Record updated successfully\n");
    VillageAttackedCharacPointRank_PrintInfo(&retrieved);
    printf("\n");
}

void test_get_by_server(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_GetByServer\n");

    /* Add another record */
    VillageAttackedCharacPointRank record2;
    memset(&record2, 0, sizeof(record2));
    record2.server_info = TEST_SERVER_INFO;
    strncpy(record2.occ_date, TEST_OCC_DATE, sizeof(record2.occ_date) - 1);
    record2.occ_date[sizeof(record2.occ_date) - 1] = '\0';
    record2.charac_no = 100002;
    record2.hunting_point = 6000;
    record2.rank = 1;
    VillageAttackedCharacPointRank_Add(manager, &record2);

    VillageAttackedCharacPointRank records[100];
    memset(records, 0, sizeof(records));

    int count = VillageAttackedCharacPointRank_GetByServer(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        records,
        100
    );

    assert(count >= 0);
    printf("  Found %d records for server %u on date %s\n",
           count, TEST_SERVER_INFO, TEST_OCC_DATE);

    for (int i = 0; i < count && i < 5; i++) {
        printf("    [%d] CharNo=%u, Points=%u, Rank=%u\n",
               i + 1,
               records[i].charac_no,
               records[i].hunting_point,
               records[i].rank);
    }

    printf("  PASS: GetByServer works correctly\n\n");
}

void test_get_top_ranks(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_GetTopRanks\n");

    VillageAttackedCharacPointRank records[10];
    memset(records, 0, sizeof(records));

    int count = VillageAttackedCharacPointRank_GetTopRanks(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        10,
        records,
        10
    );

    assert(count >= 0);
    printf("  Found %d top-ranked records\n", count);

    for (int i = 0; i < count; i++) {
        printf("    Rank %u: CharNo=%u, Points=%u\n",
               records[i].rank,
               records[i].charac_no,
               records[i].hunting_point);
    }

    printf("  PASS: GetTopRanks works correctly\n\n");
}

void test_get_by_character(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_GetByCharacter\n");

    VillageAttackedCharacPointRank records[100];
    memset(records, 0, sizeof(records));

    int count = VillageAttackedCharacPointRank_GetByCharacter(
        manager,
        TEST_CHARAC_NO,
        records,
        100
    );

    assert(count >= 0);
    printf("  Found %d records for character %u\n", count, TEST_CHARAC_NO);

    for (int i = 0; i < count && i < 5; i++) {
        printf("    [%d] Date=%s, Server=%u, Points=%u, Rank=%u\n",
               i + 1,
               records[i].occ_date,
               records[i].server_info,
               records[i].hunting_point,
               records[i].rank);
    }

    printf("  PASS: GetByCharacter works correctly\n\n");
}

void test_delete(DBConnectionManager* manager) {
    printf("TEST: VillageAttackedCharacPointRank_Delete\n");

    int result = VillageAttackedCharacPointRank_Delete(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        TEST_CHARAC_NO
    );

    assert(result == 1);

    /* Verify deletion */
    int exists = VillageAttackedCharacPointRank_Exists(
        manager,
        TEST_SERVER_INFO,
        TEST_OCC_DATE,
        TEST_CHARAC_NO
    );

    assert(exists == 0);
    printf("  PASS: Record deleted successfully\n\n");
}
