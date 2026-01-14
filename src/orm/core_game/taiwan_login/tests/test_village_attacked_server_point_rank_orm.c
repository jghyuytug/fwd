#include "../headers/village_attacked_server_point_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Test configuration */
#define TEST_SERVER_1 1
#define TEST_SERVER_2 2
#define TEST_DATE_1 "2025-11-19"
#define TEST_DATE_2 "2025-11-20"
#define TEST_DATE_3 "2025-11-21"
#define MAX_RESULTS 100

/* Test helper functions */
void print_separator(void);
void test_add_records(DBConnectionManager* manager);
void test_get_by_keys(DBConnectionManager* manager);
void test_exists(DBConnectionManager* manager);
void test_update(DBConnectionManager* manager);
void test_get_by_server(DBConnectionManager* manager);
void test_get_top_ranks(DBConnectionManager* manager);
void test_get_by_date_range(DBConnectionManager* manager);
void test_print_info(DBConnectionManager* manager);
void test_delete(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("Village Attacked Server Point Rank ORM Test Suite\n");
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

    printf("MySQL connection initialized\n\n");

    /* Run all tests */
    test_add_records(&manager);
    test_get_by_keys(&manager);
    test_exists(&manager);
    test_update(&manager);
    test_get_by_server(&manager);
    test_get_top_ranks(&manager);
    test_get_by_date_range(&manager);
    test_print_info(&manager);
    test_delete(&manager);

    /* Cleanup */
    DBConnectionManager_Cleanup(&manager);

    print_separator();
    printf("ALL TESTS PASSED!\n");
    printf("========================================\n");

    return 0;
}

void print_separator(void) {
    printf("========================================\n");
}

void test_add_records(DBConnectionManager* manager) {
    printf("\n[Test 1] Add Records\n");
    print_separator();

    VillageAttackedServerPointRank rank;

    /* Add record 1 */
    memset(&rank, 0, sizeof(rank));
    rank.server_info = TEST_SERVER_1;
    strncpy(rank.occ_date, TEST_DATE_1, sizeof(rank.occ_date) - 1);
    rank.occ_date[sizeof(rank.occ_date) - 1] = '\0';
    rank.hunting_point = 50000;
    rank.rank = 1;

    int result = VillageAttackedServerPointRank_Add(manager, &rank);
    assert(result == 1);
    printf("Added: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    /* Add record 2 (same server, different date) */
    strncpy(rank.occ_date, TEST_DATE_2, sizeof(rank.occ_date) - 1);
    rank.occ_date[sizeof(rank.occ_date) - 1] = '\0';
    rank.hunting_point = 45000;
    rank.rank = 2;
    result = VillageAttackedServerPointRank_Add(manager, &rank);
    assert(result == 1);
    printf("Added: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    /* Add record 3 (different server, same first date) */
    rank.server_info = TEST_SERVER_2;
    strncpy(rank.occ_date, TEST_DATE_1, sizeof(rank.occ_date) - 1);
    rank.occ_date[sizeof(rank.occ_date) - 1] = '\0';
    rank.hunting_point = 48000;
    rank.rank = 2;
    result = VillageAttackedServerPointRank_Add(manager, &rank);
    assert(result == 1);
    printf("Added: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    /* Add record 4 (second server, different date) */
    strncpy(rank.occ_date, TEST_DATE_3, sizeof(rank.occ_date) - 1);
    rank.occ_date[sizeof(rank.occ_date) - 1] = '\0';
    rank.hunting_point = 52000;
    rank.rank = 1;
    result = VillageAttackedServerPointRank_Add(manager, &rank);
    assert(result == 1);
    printf("Added: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    printf("PASSED: All records added successfully\n");
}

void test_get_by_keys(DBConnectionManager* manager) {
    printf("\n[Test 2] Get by Composite Primary Key\n");
    print_separator();

    VillageAttackedServerPointRank rank;

    /* Get existing record */
    int result = VillageAttackedServerPointRank_Get(manager, TEST_SERVER_1, TEST_DATE_1, &rank);
    assert(result == 1);
    assert(rank.server_info == TEST_SERVER_1);
    assert(strcmp(rank.occ_date, TEST_DATE_1) == 0);
    printf("Retrieved: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    /* Get another existing record */
    result = VillageAttackedServerPointRank_Get(manager, TEST_SERVER_2, TEST_DATE_3, &rank);
    assert(result == 1);
    assert(rank.server_info == TEST_SERVER_2);
    assert(strcmp(rank.occ_date, TEST_DATE_3) == 0);
    printf("Retrieved: Server %u, Date %s, Points %u, Rank %u\n",
           rank.server_info, rank.occ_date, rank.hunting_point, rank.rank);

    /* Try to get non-existent record */
    result = VillageAttackedServerPointRank_Get(manager, 99, "2025-01-01", &rank);
    assert(result == 0);
    printf("Non-existent record correctly returned 0\n");

    printf("PASSED: Get by composite key works correctly\n");
}

void test_exists(DBConnectionManager* manager) {
    printf("\n[Test 3] Exists Check\n");
    print_separator();

    /* Check existing record */
    int exists = VillageAttackedServerPointRank_Exists(manager, TEST_SERVER_1, TEST_DATE_1);
    assert(exists == 1);
    printf("Record exists: Server %u, Date %s\n", TEST_SERVER_1, TEST_DATE_1);

    /* Check non-existent record */
    exists = VillageAttackedServerPointRank_Exists(manager, 99, "2025-01-01");
    assert(exists == 0);
    printf("Non-existent record correctly returned 0\n");

    printf("PASSED: Exists check works correctly\n");
}

void test_update(DBConnectionManager* manager) {
    printf("\n[Test 4] Update Record\n");
    print_separator();

    VillageAttackedServerPointRank rank;

    /* Get original record */
    VillageAttackedServerPointRank_Get(manager, TEST_SERVER_1, TEST_DATE_1, &rank);
    printf("Original: Points %u, Rank %u\n", rank.hunting_point, rank.rank);

    /* Update the record */
    rank.hunting_point = 60000;
    rank.rank = 3;
    int result = VillageAttackedServerPointRank_Update(manager, &rank);
    assert(result == 1);

    /* Verify update */
    VillageAttackedServerPointRank updated;
    VillageAttackedServerPointRank_Get(manager, TEST_SERVER_1, TEST_DATE_1, &updated);
    assert(updated.hunting_point == 60000);
    assert(updated.rank == 3);
    printf("Updated: Points %u, Rank %u\n", updated.hunting_point, updated.rank);

    printf("PASSED: Update works correctly\n");
}

void test_get_by_server(DBConnectionManager* manager) {
    printf("\n[Test 5] Get by Server\n");
    print_separator();

    VillageAttackedServerPointRank ranks[MAX_RESULTS];
    int count = VillageAttackedServerPointRank_GetByServer(manager, TEST_SERVER_1, ranks, MAX_RESULTS);

    assert(count > 0);
    printf("Found %d records for Server %u:\n", count, TEST_SERVER_1);
    for (int i = 0; i < count; i++) {
        printf("  [%d] Date: %s, Points: %u, Rank: %u\n",
               i + 1, ranks[i].occ_date, ranks[i].hunting_point, ranks[i].rank);
    }

    printf("PASSED: Get by server works correctly\n");
}

void test_get_top_ranks(DBConnectionManager* manager) {
    printf("\n[Test 6] Get Top Ranks for Date\n");
    print_separator();

    VillageAttackedServerPointRank ranks[MAX_RESULTS];
    int count = VillageAttackedServerPointRank_GetTopRanks(manager, TEST_DATE_1, 10, ranks, MAX_RESULTS);

    assert(count > 0);
    printf("Top %d ranks for Date %s:\n", count, TEST_DATE_1);
    for (int i = 0; i < count; i++) {
        printf("  [%d] Server: %u, Points: %u, Rank: %u\n",
               i + 1, ranks[i].server_info, ranks[i].hunting_point, ranks[i].rank);
        /* Verify ranks are in ascending order */
        if (i > 0) {
            assert(ranks[i].rank >= ranks[i-1].rank);
        }
    }

    printf("PASSED: Get top ranks works correctly\n");
}

void test_get_by_date_range(DBConnectionManager* manager) {
    printf("\n[Test 7] Get by Date Range\n");
    print_separator();

    VillageAttackedServerPointRank ranks[MAX_RESULTS];

    /* Get all servers in date range */
    int count = VillageAttackedServerPointRank_GetByDateRange(
        manager, 0, TEST_DATE_1, TEST_DATE_3, ranks, MAX_RESULTS);
    assert(count > 0);
    printf("Found %d records for all servers between %s and %s:\n",
           count, TEST_DATE_1, TEST_DATE_3);
    for (int i = 0; i < count && i < 5; i++) {
        printf("  [%d] Server: %u, Date: %s, Points: %u, Rank: %u\n",
               i + 1, ranks[i].server_info, ranks[i].occ_date,
               ranks[i].hunting_point, ranks[i].rank);
    }

    /* Get specific server in date range */
    count = VillageAttackedServerPointRank_GetByDateRange(
        manager, TEST_SERVER_1, TEST_DATE_1, TEST_DATE_3, ranks, MAX_RESULTS);
    assert(count > 0);
    printf("\nFound %d records for Server %u between %s and %s:\n",
           count, TEST_SERVER_1, TEST_DATE_1, TEST_DATE_3);
    for (int i = 0; i < count; i++) {
        printf("  [%d] Date: %s, Points: %u, Rank: %u\n",
               i + 1, ranks[i].occ_date, ranks[i].hunting_point, ranks[i].rank);
    }

    printf("PASSED: Get by date range works correctly\n");
}

void test_print_info(DBConnectionManager* manager) {
    printf("\n[Test 8] Print Info Utility\n");
    print_separator();

    VillageAttackedServerPointRank rank;
    VillageAttackedServerPointRank_Get(manager, TEST_SERVER_1, TEST_DATE_1, &rank);
    VillageAttackedServerPointRank_PrintInfo(&rank);

    printf("PASSED: Print info works correctly\n");
}

void test_delete(DBConnectionManager* manager) {
    printf("\n[Test 9] Delete Records\n");
    print_separator();

    /* Delete first record */
    int result = VillageAttackedServerPointRank_Delete(manager, TEST_SERVER_1, TEST_DATE_1);
    assert(result == 1);
    printf("Deleted: Server %u, Date %s\n", TEST_SERVER_1, TEST_DATE_1);

    /* Verify deletion */
    int exists = VillageAttackedServerPointRank_Exists(manager, TEST_SERVER_1, TEST_DATE_1);
    assert(exists == 0);
    printf("Verified: Record no longer exists\n");

    /* Delete remaining test records */
    VillageAttackedServerPointRank_Delete(manager, TEST_SERVER_1, TEST_DATE_2);
    VillageAttackedServerPointRank_Delete(manager, TEST_SERVER_2, TEST_DATE_1);
    VillageAttackedServerPointRank_Delete(manager, TEST_SERVER_2, TEST_DATE_3);
    printf("Cleaned up all test records\n");

    printf("PASSED: Delete works correctly\n");
}
