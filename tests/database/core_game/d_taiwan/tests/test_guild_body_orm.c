#include "guild_body_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test data cleanup */
void cleanup_test_data(DBConnectionManager* manager);

/* Test functions */
void test_add_guild(DBConnectionManager* manager);
void test_get_guild(DBConnectionManager* manager);
void test_update_guild(DBConnectionManager* manager);
void test_exists_guild(DBConnectionManager* manager);
void test_delete_guild(DBConnectionManager* manager);

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

    printf("=== Guild Body ORM Test Suite ===\n\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_add_guild(&manager);
    test_get_guild(&manager);
    test_exists_guild(&manager);
    test_update_guild(&manager);
    test_delete_guild(&manager);

    /* Clean up after tests */
    cleanup_test_data(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_body WHERE gno IN (100001, 100002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_guild(DBConnectionManager* manager) {
    GuildBody* guild = GuildBody_Allocate();

    printf("[TEST] Add guild body\n");

    if (!guild) {
        printf("  FAIL: Failed to allocate GuildBody\n");
        test_failed++;
        return;
    }

    guild->gno = 100001;
    guild->body = (char*)malloc(1024);
    if (!guild->body) {
        printf("  FAIL: Failed to allocate body\n");
        GuildBody_Free(guild);
        free(guild);
        test_failed++;
        return;
    }
    strcpy(guild->body, "This is test guild body content. It can contain long text.");

    if (GuildBody_Add(manager, guild) == 0) {
        printf("  PASS: Guild body added (gno=%d)\n", guild->gno);
        test_passed++;
    } else {
        printf("  FAIL: Failed to add guild body\n");
        test_failed++;
    }

    GuildBody_Free(guild);
    free(guild);
}

void test_get_guild(DBConnectionManager* manager) {
    GuildBody* guild = GuildBody_Allocate();

    printf("[TEST] Get guild body\n");

    if (!guild) {
        printf("  FAIL: Failed to allocate GuildBody\n");
        test_failed++;
        return;
    }

    if (GuildBody_Get(manager, 100001, guild) == 0) {
        printf("  PASS: Retrieved guild body (gno=%d)\n", guild->gno);
        printf("  Body content: %.50s...\n", guild->body ? guild->body : "(null)");
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve guild body\n");
        test_failed++;
    }

    GuildBody_Free(guild);
    free(guild);
}

void test_exists_guild(DBConnectionManager* manager) {
    printf("[TEST] Check guild body exists\n");

    if (GuildBody_Exists(manager, 100001)) {
        printf("  PASS: Guild body exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Guild body should exist\n");
        test_failed++;
    }
}

void test_update_guild(DBConnectionManager* manager) {
    GuildBody* guild = GuildBody_Allocate();

    printf("[TEST] Update guild body\n");

    if (!guild) {
        printf("  FAIL: Failed to allocate GuildBody\n");
        test_failed++;
        return;
    }

    guild->gno = 100001;
    guild->body = (char*)malloc(1024);
    if (!guild->body) {
        printf("  FAIL: Failed to allocate body\n");
        GuildBody_Free(guild);
        free(guild);
        test_failed++;
        return;
    }
    strcpy(guild->body, "Updated guild body content with new information.");

    if (GuildBody_Update(manager, guild) == 0) {
        printf("  PASS: Guild body updated\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to update guild body\n");
        test_failed++;
    }

    GuildBody_Free(guild);
    free(guild);
}

void test_delete_guild(DBConnectionManager* manager) {
    printf("[TEST] Delete guild body\n");

    if (GuildBody_Delete(manager, 100001) == 0) {
        printf("  PASS: Guild body deleted\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete guild body\n");
        test_failed++;
    }
}
