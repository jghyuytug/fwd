#include "guild_halloffame_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test functions */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

/* Test statistics */
static int test_passed = 0;
static int test_failed = 0;
static int test_total = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        test_total++; \
        if (condition) { \
            printf("[PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("[FAIL] %s\n", message); \
            test_failed++; \
        } \
    } while(0)

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
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Hall of Fame ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n", test_total, test_passed, test_failed);
    printf("Success Rate: %.1f%%\n", test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildHalloffame fame, retrieved;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add new hall of fame entry */
    memset(&fame, 0, sizeof(GuildHalloffame));
    fame.fame_id = 100001;
    fame.server_id = 1;
    fame.guild_id = 50001;
    strcpy(fame.guild_name, "TestGuild");
    strcpy(fame.file_url, "http://example.com/guild.html");
    fame.open_flag = 1;
    fame.main_flag = 0;

    ret = GuildHalloffame_Add(manager, &fame);
    TEST_ASSERT(ret == 0, "Add new hall of fame entry");

    /* Test 2: Check existence */
    ret = GuildHalloffame_Exists(manager, 100001, 1);
    TEST_ASSERT(ret == 1, "Entry exists after add");

    /* Test 3: Get entry */
    ret = GuildHalloffame_Get(manager, 100001, 1, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.fame_id == 100001, "Get entry by composite key");
    TEST_ASSERT(strcmp(retrieved.guild_name, "TestGuild") == 0, "Verify guild_name");
    TEST_ASSERT(retrieved.guild_id == 50001, "Verify guild_id");
    TEST_ASSERT(retrieved.open_flag == 1, "Verify open_flag");

    /* Test 4: Update entry */
    strcpy(fame.guild_name, "UpdatedGuild");
    fame.open_flag = 0;
    fame.main_flag = 1;
    ret = GuildHalloffame_Update(manager, &fame);
    TEST_ASSERT(ret == 0, "Update entry");

    ret = GuildHalloffame_Get(manager, 100001, 1, &retrieved);
    TEST_ASSERT(ret == 0 && strcmp(retrieved.guild_name, "UpdatedGuild") == 0,
                "Verify updated guild_name");
    TEST_ASSERT(retrieved.open_flag == 0 && retrieved.main_flag == 1,
                "Verify updated flags");

    /* Test 5: Delete entry */
    ret = GuildHalloffame_Delete(manager, 100001, 1);
    TEST_ASSERT(ret == 0, "Delete entry");

    ret = GuildHalloffame_Exists(manager, 100001, 1);
    TEST_ASSERT(ret == 0, "Entry does not exist after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    GuildHalloffame fame;
    GuildHalloffame fames[10];
    int actual_count = 0;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data - same guild, different fame IDs */
    for (int i = 0; i < 3; i++) {
        memset(&fame, 0, sizeof(GuildHalloffame));
        fame.fame_id = 200001 + i;
        fame.server_id = 2;
        fame.guild_id = 60001;
        snprintf(fame.guild_name, sizeof(fame.guild_name), "Guild%d", i + 1);
        snprintf(fame.file_url, sizeof(fame.file_url), "http://example.com/guild%d.html", i + 1);
        fame.open_flag = 1;
        fame.main_flag = 0;
        GuildHalloffame_Add(manager, &fame);
    }

    /* Test 1: Get by guild (should return 3 entries) */
    ret = GuildHalloffame_GetByGuild(manager, 60001, fames, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all entries for a guild");

    /* Add entries on different server */
    for (int i = 0; i < 2; i++) {
        memset(&fame, 0, sizeof(GuildHalloffame));
        fame.fame_id = 200010 + i;
        fame.server_id = 2;
        fame.guild_id = 60002 + i;
        snprintf(fame.guild_name, sizeof(fame.guild_name), "ServerGuild%d", i + 1);
        strcpy(fame.file_url, "http://example.com/server.html");
        fame.open_flag = 1;
        fame.main_flag = 1;
        GuildHalloffame_Add(manager, &fame);
    }

    /* Test 2: Get by server (should return 5 entries for server 2) */
    ret = GuildHalloffame_GetByServer(manager, 2, fames, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 5, "Get all entries for a server");

    /* Test 3: Get by flags (should return 2 entries with open_flag=1, main_flag=1) */
    ret = GuildHalloffame_GetByFlags(manager, 1, 1, fames, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Get all entries with specific flags");

    /* Test 4: Delete by guild (should delete all 3 entries for guild 60001) */
    ret = GuildHalloffame_DeleteByGuild(manager, 60001);
    TEST_ASSERT(ret == 0, "Delete all entries for a guild");

    ret = GuildHalloffame_GetByGuild(manager, 60001, fames, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 0, "Verify guild entries deleted");

    /* Now server 2 should only have 2 entries left */
    ret = GuildHalloffame_GetByServer(manager, 2, fames, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Verify server entries count after delete");

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_halloffame WHERE fame_id >= 100000");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}
