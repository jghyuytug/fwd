#include "guild_event_orm.h"
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

    printf("=== Guild Event ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_event WHERE gno IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildEvent event;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add guild event */
    memset(&event, 0, sizeof(GuildEvent));
    event.gno = 100001;
    strcpy(event.stt_date, "2025-01-01");
    strcpy(event.end_date, "2025-01-31");
    strcpy(event.ann_date, "2024-12-25");
    strcpy(event.page_url, "http://event.dnf.com/guild_2025_jan");

    ret = GuildEvent_Add(manager, &event);
    TEST_ASSERT(ret == 0, "Add guild event (gno=100001)");

    /* Test 2: Check existence */
    ret = GuildEvent_Exists(manager, 100001);
    TEST_ASSERT(ret == 1, "Event exists (gno=100001)");

    ret = GuildEvent_Exists(manager, 999999);
    TEST_ASSERT(ret == 0, "Event not exists (gno=999999)");

    /* Test 3: Get guild event */
    memset(&event, 0, sizeof(GuildEvent));
    ret = GuildEvent_Get(manager, 100001, &event);
    TEST_ASSERT(ret == 0, "Get guild event (gno=100001)");
    TEST_ASSERT(event.gno == 100001, "Verify gno = 100001");
    TEST_ASSERT(strcmp(event.stt_date, "2025-01-01") == 0, "Verify stt_date = 2025-01-01");
    TEST_ASSERT(strcmp(event.end_date, "2025-01-31") == 0, "Verify end_date = 2025-01-31");
    TEST_ASSERT(strcmp(event.ann_date, "2024-12-25") == 0, "Verify ann_date = 2024-12-25");
    TEST_ASSERT(strcmp(event.page_url, "http://event.dnf.com/guild_2025_jan") == 0,
                "Verify page_url");

    /* Test 4: Update guild event */
    strcpy(event.end_date, "2025-02-28");
    strcpy(event.page_url, "http://event.dnf.com/guild_extended");

    ret = GuildEvent_Update(manager, &event);
    TEST_ASSERT(ret == 0, "Update guild event (gno=100001)");

    /* Verify update */
    memset(&event, 0, sizeof(GuildEvent));
    ret = GuildEvent_Get(manager, 100001, &event);
    TEST_ASSERT(strcmp(event.end_date, "2025-02-28") == 0,
                "Verify updated end_date = 2025-02-28");
    TEST_ASSERT(strcmp(event.page_url, "http://event.dnf.com/guild_extended") == 0,
                "Verify updated page_url");

    /* Test 5: Delete guild event */
    ret = GuildEvent_Delete(manager, 100001);
    TEST_ASSERT(ret == 0, "Delete guild event (gno=100001)");

    ret = GuildEvent_Exists(manager, 100001);
    TEST_ASSERT(ret == 0, "Verify event deleted (not exists)");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    GuildEvent events[10];
    GuildEvent event;
    int count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Setup test data */
    memset(&event, 0, sizeof(GuildEvent));
    event.gno = 100001;
    strcpy(event.stt_date, "2025-01-01");
    strcpy(event.end_date, "2025-01-31");
    strcpy(event.ann_date, "2024-12-20");
    strcpy(event.page_url, "http://event1.dnf.com");
    GuildEvent_Add(manager, &event);

    event.gno = 100002;
    strcpy(event.stt_date, "2025-02-01");
    strcpy(event.end_date, "2025-02-28");
    strcpy(event.ann_date, "2025-01-25");
    strcpy(event.page_url, "http://event2.dnf.com");
    GuildEvent_Add(manager, &event);

    event.gno = 100003;
    strcpy(event.stt_date, "2025-03-01");
    strcpy(event.end_date, "2025-03-31");
    strcpy(event.ann_date, "2025-02-25");
    strcpy(event.page_url, "http://event3.dnf.com");
    GuildEvent_Add(manager, &event);

    /* Test 1: Get all events */
    ret = GuildEvent_GetAll(manager, events, 10, &count);
    TEST_ASSERT(ret == 0, "Get all guild events");
    TEST_ASSERT(count >= 3, "Count >= 3 events");

    /* Test 2: Get active events (current date: 2025-01-15) */
    ret = GuildEvent_GetActive(manager, "2025-01-15", events, 10, &count);
    TEST_ASSERT(ret == 0, "Get active events (2025-01-15)");
    TEST_ASSERT(count >= 1, "At least 1 active event in January");
    if (count > 0) {
        TEST_ASSERT(events[0].gno == 100001, "Active event is gno=100001");
    }

    /* Test 3: Get events by date range */
    ret = GuildEvent_GetByDateRange(manager, "2025-01-01", "2025-02-28",
                                    events, 10, &count);
    TEST_ASSERT(ret == 0, "Get events by date range (Jan-Feb)");
    TEST_ASSERT(count >= 2, "At least 2 events in Jan-Feb range");

    /* Test 4: Get active events (current date: 2025-03-15) */
    ret = GuildEvent_GetActive(manager, "2025-03-15", events, 10, &count);
    TEST_ASSERT(ret == 0, "Get active events (2025-03-15)");
    TEST_ASSERT(count >= 1, "At least 1 active event in March");
    if (count > 0) {
        TEST_ASSERT(events[0].gno == 100003, "Active event is gno=100003");
    }

    /* Test 5: PrintInfo utility */
    printf("\n[INFO] Testing GuildEvent_PrintInfo:\n");
    memset(&event, 0, sizeof(GuildEvent));
    if (GuildEvent_Get(manager, 100001, &event) == 0) {
        GuildEvent_PrintInfo(&event);
    }

    printf("\n");
}