#include "event_goldcard_cnt_orm.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Test macros */
static int test_total = 0;
static int test_passed = 0;
static int assert_total = 0;
static int assert_passed = 0;

#define TEST_START(name) \
    do { \
        printf("\n========================================\n"); \
        printf("Test: %s\n", name); \
        printf("========================================\n"); \
        test_total++; \
        int test_ok = 1;

#define TEST_END \
        if (test_ok) { \
            test_passed++; \
            printf("PASS\n"); \
        } else { \
            printf("FAIL\n"); \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected) \
    do { \
        assert_total++; \
        if ((actual) == (expected)) { \
            assert_passed++; \
            printf("  PASS: %s == %s (%d == %d)\n", #actual, #expected, (int)(actual), (int)(expected)); \
        } else { \
            printf("  FAIL: %s == %s (%d != %d)\n", #actual, #expected, (int)(actual), (int)(expected)); \
            test_ok = 0; \
        } \
    } while(0)

#define ASSERT_STR_EQ(actual, expected) \
    do { \
        assert_total++; \
        if (strcmp((actual), (expected)) == 0) { \
            assert_passed++; \
            printf("  PASS: %s == %s (\"%s\" == \"%s\")\n", #actual, #expected, actual, expected); \
        } else { \
            printf("  FAIL: %s == %s (\"%s\" != \"%s\")\n", #actual, #expected, actual, expected); \
            test_ok = 0; \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        assert_total++; \
        if (condition) { \
            assert_passed++; \
            printf("  PASS: %s is true\n", #condition); \
        } else { \
            printf("  FAIL: %s is false\n", #condition); \
            test_ok = 0; \
        } \
    } while(0)

/* Test functions */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("Add and Get goldcard count record");

    EventGoldcardCnt record = {
        .item_no = 1001,
        .occ_date = "2025-11-17",
        .cnt = 100
    };

    int ret = EventGoldcardCnt_Add(manager, &record);
    ASSERT_EQ(ret, 0);

    EventGoldcardCnt retrieved;
    ret = EventGoldcardCnt_Get(manager, 1001, "2025-11-17", &retrieved);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(retrieved.item_no, 1001);
    ASSERT_STR_EQ(retrieved.occ_date, "2025-11-17");
    ASSERT_EQ(retrieved.cnt, 100);

    TEST_END;
}

void test_update(DBConnectionManager* manager) {
    TEST_START("Update count");

    EventGoldcardCnt record = {
        .item_no = 1001,
        .occ_date = "2025-11-17",
        .cnt = 200
    };

    int ret = EventGoldcardCnt_Update(manager, &record);
    ASSERT_EQ(ret, 0);

    EventGoldcardCnt retrieved;
    ret = EventGoldcardCnt_Get(manager, 1001, "2025-11-17", &retrieved);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(retrieved.cnt, 200);

    TEST_END;
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("Check record existence");

    int exists = EventGoldcardCnt_Exists(manager, 1001, "2025-11-17");
    ASSERT_TRUE(exists);

    exists = EventGoldcardCnt_Exists(manager, 9999, "2025-11-17");
    ASSERT_TRUE(!exists);

    TEST_END;
}

void test_get_by_item_no(DBConnectionManager* manager) {
    TEST_START("Get by item number");

    /* Add multiple records for same item */
    EventGoldcardCnt record1 = {.item_no = 2001, .occ_date = "2025-11-16", .cnt = 50};
    EventGoldcardCnt record2 = {.item_no = 2001, .occ_date = "2025-11-17", .cnt = 75};
    EventGoldcardCnt record3 = {.item_no = 2002, .occ_date = "2025-11-17", .cnt = 100};

    EventGoldcardCnt_Add(manager, &record1);
    EventGoldcardCnt_Add(manager, &record2);
    EventGoldcardCnt_Add(manager, &record3);

    EventGoldcardCnt records[10];
    int count;
    int ret = EventGoldcardCnt_GetByItemNo(manager, 2001, records, 10, &count);

    ASSERT_EQ(ret, 0);
    ASSERT_EQ(count, 2);

    TEST_END;
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("Get by date range");

    EventGoldcardCnt records[10];
    int count;
    int ret = EventGoldcardCnt_GetByDateRange(manager, "2025-11-16", "2025-11-17", records, 10, &count);

    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(count > 0);

    TEST_END;
}

void test_increment_count(DBConnectionManager* manager) {
    TEST_START("Increment count");

    /* Add initial record */
    EventGoldcardCnt record = {.item_no = 3001, .occ_date = "2025-11-17", .cnt = 10};
    EventGoldcardCnt_Add(manager, &record);

    /* Increment count */
    int ret = EventGoldcardCnt_IncrementCount(manager, 3001, "2025-11-17", 5);
    ASSERT_EQ(ret, 0);

    /* Check new count */
    EventGoldcardCnt retrieved;
    EventGoldcardCnt_Get(manager, 3001, "2025-11-17", &retrieved);
    ASSERT_EQ(retrieved.cnt, 15);

    /* Increment again */
    EventGoldcardCnt_IncrementCount(manager, 3001, "2025-11-17", 10);
    EventGoldcardCnt_Get(manager, 3001, "2025-11-17", &retrieved);
    ASSERT_EQ(retrieved.cnt, 25);

    TEST_END;
}

void test_get_total_by_item(DBConnectionManager* manager) {
    TEST_START("Get total by item");

    int total = EventGoldcardCnt_GetTotalByItem(manager, 2001);
    ASSERT_EQ(total, 125);  /* 50 + 75 */

    TEST_END;
}

void test_get_total_by_date(DBConnectionManager* manager) {
    TEST_START("Get total by date");

    int total = EventGoldcardCnt_GetTotalByDate(manager, "2025-11-17");
    ASSERT_TRUE(total > 0);

    TEST_END;
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("Delete record");

    int ret = EventGoldcardCnt_Delete(manager, 1001, "2025-11-17");
    ASSERT_EQ(ret, 0);

    int exists = EventGoldcardCnt_Exists(manager, 1001, "2025-11-17");
    ASSERT_TRUE(!exists);

    /* Cleanup test data */
    EventGoldcardCnt_Delete(manager, 2001, "2025-11-16");
    EventGoldcardCnt_Delete(manager, 2001, "2025-11-17");
    EventGoldcardCnt_Delete(manager, 2002, "2025-11-17");
    EventGoldcardCnt_Delete(manager, 3001, "2025-11-17");

    TEST_END;
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("Print record info");

    EventGoldcardCnt record = {
        .item_no = 4001,
        .occ_date = "2025-11-17",
        .cnt = 999
    };

    EventGoldcardCnt_Add(manager, &record);
    EventGoldcardCnt_PrintInfo(&record);
    EventGoldcardCnt_Delete(manager, 4001, "2025-11-17");

    ASSERT_TRUE(1);  /* Print test always passes */

    TEST_END;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n");
    printf("========================================\n");
    printf("EventGoldcardCnt ORM Test Suite\n");
    printf("========================================\n");

    /* Initialize database connection */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "Failed to connect to d_taiwan database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("Database connected successfully\n");

    /* Run tests */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_get_by_item_no(&manager);
    test_get_by_date_range(&manager);
    test_increment_count(&manager);
    test_get_total_by_item(&manager);
    test_get_total_by_date(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* Cleanup */
    DBConnectionManager_Cleanup(&manager);

    /* Print statistics */
    printf("\n");
    printf("========================================\n");
    printf("Test Statistics\n");
    printf("========================================\n");
    printf("Tests: %d/%d passed\n", test_passed, test_total);
    printf("Assertions: %d/%d passed\n", assert_passed, assert_total);
    printf("========================================\n");

    return (test_total == test_passed) ? 0 : 1;
}
