#include "guild_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("[PASS] %s\n", message); \
        test_passed++; \
    } else { \
        printf("[FAIL] %s\n", message); \
        test_failed++; \
    } \
} while(0)

/* Function prototypes */
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) < 0) {
        printf("Failed to connect to GUILD database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Rank ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_rank WHERE server_id = 1 AND guild_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildRank rank;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add guild rank */
    memset(&rank, 0, sizeof(GuildRank));
    rank.server_id = 1;
    rank.guild_id = 100001;
    strcpy(rank.guild_name, "Dragon Warriors");
    rank.guild_Rank = 1;
    rank.guild_point = 15000;
    rank.guild_acc_point = 50000;
    rank.guild_visit = 1200;
    rank.guild_acc_visit = 5000;
    rank.guild_member = 50;
    rank.guild_acc_member = 80;
    rank.guild_avg_lev = 60;

    ret = GuildRank_Add(manager, &rank);
    TEST_ASSERT(ret == 0, "Add guild rank (server=1, guild=100001)");

    /* Test 2: Check existence */
    ret = GuildRank_Exists(manager, 1, 100001);
    TEST_ASSERT(ret == 1, "Rank exists (server=1, guild=100001)");

    ret = GuildRank_Exists(manager, 1, 999999);
    TEST_ASSERT(ret == 0, "Rank not exists (server=1, guild=999999)");

    /* Test 3: Get guild rank */
    memset(&rank, 0, sizeof(GuildRank));
    ret = GuildRank_Get(manager, 1, 100001, &rank);
    TEST_ASSERT(ret == 0, "Get guild rank (server=1, guild=100001)");
    TEST_ASSERT(rank.server_id == 1, "Verify server_id = 1");
    TEST_ASSERT(rank.guild_id == 100001, "Verify guild_id = 100001");
    TEST_ASSERT(strcmp(rank.guild_name, "Dragon Warriors") == 0, "Verify guild_name");
    TEST_ASSERT(rank.guild_Rank == 1, "Verify rank = 1");
    TEST_ASSERT(rank.guild_point == 15000, "Verify guild_point = 15000");
    TEST_ASSERT(rank.guild_member == 50, "Verify guild_member = 50");
    TEST_ASSERT(rank.guild_avg_lev == 60, "Verify guild_avg_lev = 60");

    /* Test 4: Update guild rank */
    rank.guild_Rank = 2;
    rank.guild_point = 18000;
    rank.guild_member = 55;

    ret = GuildRank_Update(manager, &rank);
    TEST_ASSERT(ret == 0, "Update guild rank (server=1, guild=100001)");

    /* Verify update */
    memset(&rank, 0, sizeof(GuildRank));
    ret = GuildRank_Get(manager, 1, 100001, &rank);
    TEST_ASSERT(rank.guild_Rank == 2, "Verify updated rank = 2");
    TEST_ASSERT(rank.guild_point == 18000, "Verify updated point = 18000");
    TEST_ASSERT(rank.guild_member == 55, "Verify updated member = 55");

    /* Test 5: Delete guild rank */
    ret = GuildRank_Delete(manager, 1, 100001);
    TEST_ASSERT(ret == 0, "Delete guild rank (server=1, guild=100001)");

    ret = GuildRank_Exists(manager, 1, 100001);
    TEST_ASSERT(ret == 0, "Verify rank deleted (not exists)");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    GuildRank ranks[10];
    GuildRank rank;
    int count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Setup test data */
    memset(&rank, 0, sizeof(GuildRank));
    rank.server_id = 1;
    rank.guild_id = 100001;
    strcpy(rank.guild_name, "Top Guild");
    rank.guild_Rank = 1;
    rank.guild_point = 25000;
    rank.guild_acc_point = 100000;
    rank.guild_visit = 2000;
    rank.guild_acc_visit = 10000;
    rank.guild_member = 80;
    rank.guild_acc_member = 120;
    rank.guild_avg_lev = 70;
    GuildRank_Add(manager, &rank);

    rank.guild_id = 100002;
    strcpy(rank.guild_name, "Second Guild");
    rank.guild_Rank = 2;
    rank.guild_point = 20000;
    rank.guild_acc_point = 80000;
    rank.guild_visit = 1500;
    rank.guild_acc_visit = 8000;
    rank.guild_member = 65;
    rank.guild_acc_member = 100;
    rank.guild_avg_lev = 65;
    GuildRank_Add(manager, &rank);

    rank.guild_id = 100003;
    strcpy(rank.guild_name, "Third Guild");
    rank.guild_Rank = 3;
    rank.guild_point = 15000;
    rank.guild_acc_point = 60000;
    rank.guild_visit = 1000;
    rank.guild_acc_visit = 5000;
    rank.guild_member = 50;
    rank.guild_acc_member = 80;
    rank.guild_avg_lev = 60;
    GuildRank_Add(manager, &rank);

    /* Test 1: Get all ranks */
    ret = GuildRank_GetAll(manager, ranks, 10, &count);
    TEST_ASSERT(ret == 0, "Get all guild ranks");
    TEST_ASSERT(count >= 3, "Count >= 3 ranks");

    /* Test 2: Get ranks by server */
    ret = GuildRank_GetByServer(manager, 1, ranks, 10, &count);
    TEST_ASSERT(ret == 0, "Get ranks by server (server=1)");
    TEST_ASSERT(count >= 3, "At least 3 ranks in server 1");

    /* Test 3: Get top 2 ranks */
    ret = GuildRank_GetTopRanks(manager, 1, 2, ranks, 10, &count);
    TEST_ASSERT(ret == 0, "Get top 2 ranks");
    TEST_ASSERT(count >= 2, "Got at least 2 top ranks");
    if (count >= 2) {
        TEST_ASSERT(ranks[0].guild_Rank == 1, "First rank is 1");
        TEST_ASSERT(ranks[1].guild_Rank == 2, "Second rank is 2");
        TEST_ASSERT(strcmp(ranks[0].guild_name, "Top Guild") == 0, "First is Top Guild");
    }

    /* Test 4: Get ranks by point range */
    ret = GuildRank_GetByPointRange(manager, 1, 15000, 22000, ranks, 10, &count);
    TEST_ASSERT(ret == 0, "Get ranks by point range (15000-22000)");
    TEST_ASSERT(count >= 2, "At least 2 guilds in point range");

    /* Test 5: PrintInfo utility */
    printf("\n[INFO] Testing GuildRank_PrintInfo:\n");
    memset(&rank, 0, sizeof(GuildRank));
    if (GuildRank_Get(manager, 1, 100001, &rank) == 0) {
        GuildRank_PrintInfo(&rank);
    }

    printf("\n");
}
