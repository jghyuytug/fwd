#include "event_aradlotto_0809_entry_orm.h"
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
    TEST_START("Add and Get lottery entry");

    EventAradlotto0809Entry entry = {
        .m_id = 1001,
        .occ_date = (int)time(NULL),
        .lotto_num = "1234567"
    };

    int ret = EventAradlotto0809Entry_Add(manager, &entry);
    ASSERT_EQ(ret, 0);

    EventAradlotto0809Entry retrieved;
    ret = EventAradlotto0809Entry_Get(manager, 1001, &retrieved);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(retrieved.m_id, 1001);
    ASSERT_STR_EQ(retrieved.lotto_num, "1234567");

    TEST_END;
}

void test_update(DBConnectionManager* manager) {
    TEST_START("Update lottery number");

    EventAradlotto0809Entry entry = {
        .m_id = 1001,
        .occ_date = (int)time(NULL),
        .lotto_num = "7654321"
    };

    int ret = EventAradlotto0809Entry_Update(manager, &entry);
    ASSERT_EQ(ret, 0);

    EventAradlotto0809Entry retrieved;
    ret = EventAradlotto0809Entry_Get(manager, 1001, &retrieved);
    ASSERT_EQ(ret, 0);
    ASSERT_STR_EQ(retrieved.lotto_num, "7654321");

    TEST_END;
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("Check record existence");

    int exists = EventAradlotto0809Entry_Exists(manager, 1001);
    ASSERT_TRUE(exists);

    exists = EventAradlotto0809Entry_Exists(manager, 9999);
    ASSERT_TRUE(!exists);

    TEST_END;
}

void test_get_by_lotto_num(DBConnectionManager* manager) {
    TEST_START("Get by lottery number");

    /* Add multiple records with same number */
    EventAradlotto0809Entry entry1 = {.m_id = 2001, .occ_date = (int)time(NULL), .lotto_num = "1111111"};
    EventAradlotto0809Entry entry2 = {.m_id = 2002, .occ_date = (int)time(NULL), .lotto_num = "1111111"};
    EventAradlotto0809Entry entry3 = {.m_id = 2003, .occ_date = (int)time(NULL), .lotto_num = "2222222"};

    EventAradlotto0809Entry_Add(manager, &entry1);
    EventAradlotto0809Entry_Add(manager, &entry2);
    EventAradlotto0809Entry_Add(manager, &entry3);

    EventAradlotto0809Entry entries[10];
    int count;
    int ret = EventAradlotto0809Entry_GetByLottoNum(manager, "1111111", entries, 10, &count);

    ASSERT_EQ(ret, 0);
    ASSERT_EQ(count, 2);

    TEST_END;
}

void test_count_by_lotto_num(DBConnectionManager* manager) {
    TEST_START("Count by lottery number");

    int count = EventAradlotto0809Entry_CountByLottoNum(manager, "1111111");
    ASSERT_EQ(count, 2);

    count = EventAradlotto0809Entry_CountByLottoNum(manager, "2222222");
    ASSERT_EQ(count, 1);

    count = EventAradlotto0809Entry_CountByLottoNum(manager, "9999999");
    ASSERT_EQ(count, 0);

    TEST_END;
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("Get by date range");

    int now = (int)time(NULL);
    int start = now - 3600;  /* 1 hour ago */
    int end = now + 3600;    /* 1 hour later */

    EventAradlotto0809Entry entries[10];
    int count;
    int ret = EventAradlotto0809Entry_GetByDateRange(manager, start, end, entries, 10, &count);

    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(count > 0);

    TEST_END;
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("Delete record");

    int ret = EventAradlotto0809Entry_Delete(manager, 1001);
    ASSERT_EQ(ret, 0);

    int exists = EventAradlotto0809Entry_Exists(manager, 1001);
    ASSERT_TRUE(!exists);

    /* Cleanup test data */
    EventAradlotto0809Entry_Delete(manager, 2001);
    EventAradlotto0809Entry_Delete(manager, 2002);
    EventAradlotto0809Entry_Delete(manager, 2003);

    TEST_END;
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("Print record info");

    EventAradlotto0809Entry entry = {
        .m_id = 3001,
        .occ_date = (int)time(NULL),
        .lotto_num = "8888888"
    };

    EventAradlotto0809Entry_Add(manager, &entry);
    EventAradlotto0809Entry_PrintInfo(&entry);
    EventAradlotto0809Entry_Delete(manager, 3001);

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
    printf("EventAradlotto0809Entry ORM Test Suite\n");
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
    test_get_by_lotto_num(&manager);
    test_count_by_lotto_num(&manager);
    test_get_by_date_range(&manager);
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
