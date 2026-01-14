#include "guild_halloffame_html_orm.h"
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

    printf("=== Guild Hall of Fame HTML ORM Test Suite ===\n\n");

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
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_halloffame_html WHERE fame_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildHalloffameHtml* hall;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Create structure */
    hall = GuildHalloffameHtml_Create();
    TEST_ASSERT(hall != NULL, "Create GuildHalloffameHtml structure");
    TEST_ASSERT(hall->html != NULL, "HTML buffer allocated");

    /* Test 2: Add hall of fame HTML */
    hall->fame_id = 100001;
    strcpy(hall->title, "Top Guild Rankings 2025");
    strcpy(hall->html, "<html><body><h1>Top Guild Rankings</h1><p>Guild rankings for 2025...</p></body></html>");
    hall->html_length = strlen(hall->html);

    ret = GuildHalloffameHtml_Add(manager, hall);
    TEST_ASSERT(ret == 0, "Add hall of fame HTML (fame_id=100001)");

    GuildHalloffameHtml_Free(hall);

    /* Test 3: Check existence */
    ret = GuildHalloffameHtml_Exists(manager, 100001);
    TEST_ASSERT(ret == 1, "Hall of fame exists (fame_id=100001)");

    ret = GuildHalloffameHtml_Exists(manager, 999999);
    TEST_ASSERT(ret == 0, "Hall of fame not exists (fame_id=999999)");

    /* Test 4: Get hall of fame HTML */
    hall = GuildHalloffameHtml_Create();
    ret = GuildHalloffameHtml_Get(manager, 100001, hall);
    TEST_ASSERT(ret == 0, "Get hall of fame HTML (fame_id=100001)");
    TEST_ASSERT(hall->fame_id == 100001, "Verify fame_id = 100001");
    TEST_ASSERT(strcmp(hall->title, "Top Guild Rankings 2025") == 0, "Verify title");
    TEST_ASSERT(strstr(hall->html, "Top Guild Rankings") != NULL, "Verify HTML content");
    TEST_ASSERT(hall->html_length > 0, "Verify HTML length > 0");

    /* Test 5: Update hall of fame HTML */
    strcpy(hall->title, "Updated Rankings 2025");
    strcpy(hall->html, "<html><body><h1>Updated Rankings</h1><p>New content...</p></body></html>");
    hall->html_length = strlen(hall->html);

    ret = GuildHalloffameHtml_Update(manager, hall);
    TEST_ASSERT(ret == 0, "Update hall of fame HTML (fame_id=100001)");

    GuildHalloffameHtml_Free(hall);

    /* Verify update */
    hall = GuildHalloffameHtml_Create();
    ret = GuildHalloffameHtml_Get(manager, 100001, hall);
    TEST_ASSERT(strcmp(hall->title, "Updated Rankings 2025") == 0, "Verify updated title");
    TEST_ASSERT(strstr(hall->html, "Updated Rankings") != NULL, "Verify updated HTML");

    GuildHalloffameHtml_Free(hall);

    /* Test 6: Update only HTML */
    ret = GuildHalloffameHtml_UpdateHtml(manager, 100001,
        "<html><body><h1>Quick Update</h1></body></html>");
    TEST_ASSERT(ret == 0, "Update only HTML content");

    hall = GuildHalloffameHtml_Create();
    ret = GuildHalloffameHtml_Get(manager, 100001, hall);
    TEST_ASSERT(strstr(hall->html, "Quick Update") != NULL, "Verify HTML updated");

    GuildHalloffameHtml_Free(hall);

    /* Test 7: Delete hall of fame HTML */
    ret = GuildHalloffameHtml_Delete(manager, 100001);
    TEST_ASSERT(ret == 0, "Delete hall of fame HTML (fame_id=100001)");

    ret = GuildHalloffameHtml_Exists(manager, 100001);
    TEST_ASSERT(ret == 0, "Verify hall of fame deleted");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    GuildHalloffameHtml* halls[10];
    GuildHalloffameHtml* hall;
    int count;
    int ret;
    int i;

    printf("--- Test: Business Operations ---\n");

    /* Allocate array of halls */
    for (i = 0; i < 10; i++) {
        halls[i] = GuildHalloffameHtml_Create();
    }

    /* Setup test data */
    hall = GuildHalloffameHtml_Create();

    hall->fame_id = 100001;
    strcpy(hall->title, "Season 1 Rankings");
    strcpy(hall->html, "<html><body><h1>Season 1</h1><p>First season rankings...</p></body></html>");
    hall->html_length = strlen(hall->html);
    GuildHalloffameHtml_Add(manager, hall);

    hall->fame_id = 100002;
    strcpy(hall->title, "Season 2 Rankings");
    strcpy(hall->html, "<html><body><h1>Season 2</h1><p>Second season rankings...</p></body></html>");
    hall->html_length = strlen(hall->html);
    GuildHalloffameHtml_Add(manager, hall);

    hall->fame_id = 100003;
    strcpy(hall->title, "Special Event");
    strcpy(hall->html, "<html><body><h1>Special Event</h1><p>Event rankings...</p></body></html>");
    hall->html_length = strlen(hall->html);
    GuildHalloffameHtml_Add(manager, hall);

    GuildHalloffameHtml_Free(hall);

    /* Test 1: Get all halls */
    ret = GuildHalloffameHtml_GetAll(manager, halls, 10, &count);
    TEST_ASSERT(ret == 0, "Get all hall of fame HTMLs");
    TEST_ASSERT(count >= 3, "Count >= 3 halls");

    /* Test 2: Get by title */
    hall = GuildHalloffameHtml_Create();
    ret = GuildHalloffameHtml_GetByTitle(manager, "Season 2 Rankings", hall);
    TEST_ASSERT(ret == 0, "Get hall by title");
    TEST_ASSERT(hall->fame_id == 100002, "Verify fame_id = 100002");
    TEST_ASSERT(strstr(hall->html, "Second season") != NULL, "Verify HTML content");

    /* Test 3: PrintInfo utility */
    printf("\n[INFO] Testing GuildHalloffameHtml_PrintInfo:\n");
    GuildHalloffameHtml_PrintInfo(hall);

    GuildHalloffameHtml_Free(hall);

    /* Free array of halls */
    for (i = 0; i < 10; i++) {
        GuildHalloffameHtml_Free(halls[i]);
    }

    printf("\n");
}
