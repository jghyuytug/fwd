/**
 * test_member_passwd_mod_orm.c
 *
 * Comprehensive test suite for member_passwd_mod_orm module
 * 密码修改历史追踪表 - 测试所有17个API函数
 *
 * Test Coverage:
 * - 5 CRUD operations
 * - 9 business queries
 * - 2 bulk operations
 * - 1 utility function
 *
 * Usage: ./test_member_passwd_mod_orm <config_file>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "member_passwd_mod_orm.h"

/* Test database connection manager */
static DBConnectionManager manager;

/* Test statistics */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macros */
#define TEST_START(name) \
    do { \
        printf("\n[TEST %d] %s\n", ++tests_run, name); \
    } while(0)

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  ✓ %s\n", message); \
            tests_passed++; \
        } else { \
            printf("  ✗ %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

/**
 * Test 1: Add a new record
 */
static void Test_Add() {
    TEST_START("MemberPasswdMod_Add");

    MemberPasswdMod record = {
        .m_id = 10001,
        .first_time = "2025-01-01 10:00:00",
        .last_time = "2025-01-01 10:00:00",
        .cnt = 1
    };

    int ret = MemberPasswdMod_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Add record with m_id=10001");
}

/**
 * Test 2: Get a record
 */
static void Test_Get() {
    TEST_START("MemberPasswdMod_Get");

    MemberPasswdMod record;
    int ret = MemberPasswdMod_Get(&manager, 10001, &record);

    TEST_ASSERT(ret == 0, "Get record with m_id=10001");
    TEST_ASSERT(record.m_id == 10001, "m_id matches");
    TEST_ASSERT(strcmp(record.first_time, "2025-01-01 10:00:00") == 0, "first_time matches");
    TEST_ASSERT(record.cnt == 1, "cnt matches");
}

/**
 * Test 3: Exists check
 */
static void Test_Exists() {
    TEST_START("MemberPasswdMod_Exists");

    int exists1 = MemberPasswdMod_Exists(&manager, 10001);
    TEST_ASSERT(exists1 == 1, "Record 10001 exists");

    int exists2 = MemberPasswdMod_Exists(&manager, 99999);
    TEST_ASSERT(exists2 == 0, "Record 99999 does not exist");
}

/**
 * Test 4: Update a record
 */
static void Test_Update() {
    TEST_START("MemberPasswdMod_Update");

    MemberPasswdMod record = {
        .m_id = 10001,
        .first_time = "2025-01-01 10:00:00",
        .last_time = "2025-01-15 10:00:00",
        .cnt = 5
    };

    int ret = MemberPasswdMod_Update(&manager, &record);
    TEST_ASSERT(ret == 0, "Update record with m_id=10001");

    /* Verify update */
    MemberPasswdMod updated;
    MemberPasswdMod_Get(&manager, 10001, &updated);
    TEST_ASSERT(updated.cnt == 5, "cnt updated to 5");
    TEST_ASSERT(strcmp(updated.last_time, "2025-01-15 10:00:00") == 0, "last_time updated");
}

/**
 * Test 5: Add idempotent (REPLACE INTO)
 */
static void Test_AddIdempotent() {
    TEST_START("MemberPasswdMod_Add (Idempotent)");

    MemberPasswdMod record = {
        .m_id = 10001,
        .first_time = "2025-02-01 10:00:00",
        .last_time = "2025-02-01 10:00:00",
        .cnt = 10
    };

    int ret = MemberPasswdMod_Add(&manager, &record);
    TEST_ASSERT(ret == 0, "Re-add (replace) record with m_id=10001");

    /* Verify replacement */
    MemberPasswdMod replaced;
    MemberPasswdMod_Get(&manager, 10001, &replaced);
    TEST_ASSERT(replaced.cnt == 10, "cnt replaced to 10");
    TEST_ASSERT(strcmp(replaced.first_time, "2025-02-01 10:00:00") == 0, "first_time replaced");
}

/**
 * Test 6: IncrementCnt - atomic increment with NOW()
 */
static void Test_IncrementCnt() {
    TEST_START("MemberPasswdMod_IncrementCnt");

    /* Original cnt = 10, after increment should be 11 */
    int ret = MemberPasswdMod_IncrementCnt(&manager, 10001);
    TEST_ASSERT(ret == 0, "Increment cnt for m_id=10001");

    /* Verify increment and last_time update */
    MemberPasswdMod record;
    MemberPasswdMod_Get(&manager, 10001, &record);
    TEST_ASSERT(record.cnt == 11, "cnt incremented to 11");
    /* last_time should be updated to NOW() - just check it's not the old value */
    TEST_ASSERT(strcmp(record.last_time, "2025-02-01 10:00:00") != 0, "last_time updated to NOW()");
}

/**
 * Test 7: GetByFirstTimeRange
 */
static void Test_GetByFirstTimeRange() {
    TEST_START("MemberPasswdMod_GetByFirstTimeRange");

    /* Add more test data */
    MemberPasswdMod test_records[] = {
        {10002, "2025-01-10 10:00:00", "2025-01-10 10:00:00", 2},
        {10003, "2025-01-20 10:00:00", "2025-01-20 10:00:00", 3},
        {10004, "2025-01-30 10:00:00", "2025-01-30 10:00:00", 4}
    };
    int i;
    for (i = 0; i < 3; i++) {
        MemberPasswdMod_Add(&manager, &test_records[i]);
    }

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByFirstTimeRange(&manager,
        "2025-01-01 00:00:00", "2025-01-25 23:59:59", results, 10);

    TEST_ASSERT(count >= 2, "Found at least 2 records in range");
    /* Should be sorted by first_time DESC */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].first_time, results[1].first_time) >= 0,
                   "Results sorted by first_time DESC");
    }
}

/**
 * Test 8: GetByLastTimeRange
 */
static void Test_GetByLastTimeRange() {
    TEST_START("MemberPasswdMod_GetByLastTimeRange");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByLastTimeRange(&manager,
        "2025-01-01 00:00:00", "2025-12-31 23:59:59", results, 10);

    TEST_ASSERT(count > 0, "Found records in last_time range");
    /* Should be sorted by last_time DESC */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].last_time, results[1].last_time) >= 0,
                   "Results sorted by last_time DESC");
    }
}

/**
 * Test 9: GetByCntRange
 */
static void Test_GetByCntRange() {
    TEST_START("MemberPasswdMod_GetByCntRange");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByCntRange(&manager, 2, 10, results, 10);
    int i;

    TEST_ASSERT(count > 0, "Found records in cnt range [2, 10]");
    /* Verify all results are in range and sorted DESC */
    for (i = 0; i < count; i++) {
        TEST_ASSERT(results[i].cnt >= 2 && results[i].cnt <= 10,
                   "Record cnt in range [2, 10]");
        if (i > 0) {
            TEST_ASSERT(results[i-1].cnt >= results[i].cnt,
                       "Results sorted by cnt DESC");
        }
    }
}

/**
 * Test 10: GetFrequentModifiers
 */
static void Test_GetFrequentModifiers() {
    TEST_START("MemberPasswdMod_GetFrequentModifiers");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetFrequentModifiers(&manager, 3, results, 10);
    int i;

    TEST_ASSERT(count > 0, "Found frequent modifiers (cnt >= 3)");
    /* Verify all results meet threshold */
    for (i = 0; i < count; i++) {
        TEST_ASSERT(results[i].cnt >= 3, "Record cnt >= 3");
    }
}

/**
 * Test 11: GetRecentModifiers
 */
static void Test_GetRecentModifiers() {
    TEST_START("MemberPasswdMod_GetRecentModifiers");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetRecentModifiers(&manager, results, 10);

    TEST_ASSERT(count > 0, "Found recent modifiers");
    /* Should be sorted by last_time DESC */
    if (count >= 2) {
        TEST_ASSERT(strcmp(results[0].last_time, results[1].last_time) >= 0,
                   "Results sorted by last_time DESC");
    }
}

/**
 * Test 12: UpdateLastTime
 */
static void Test_UpdateLastTime() {
    TEST_START("MemberPasswdMod_UpdateLastTime");

    /* Get original last_time */
    MemberPasswdMod original;
    MemberPasswdMod_Get(&manager, 10002, &original);

    /* Update last_time to NOW() */
    int ret = MemberPasswdMod_UpdateLastTime(&manager, 10002);
    TEST_ASSERT(ret == 0, "Update last_time for m_id=10002");

    /* Verify last_time changed */
    MemberPasswdMod updated;
    MemberPasswdMod_Get(&manager, 10002, &updated);
    TEST_ASSERT(strcmp(updated.last_time, original.last_time) != 0,
               "last_time updated to NOW()");
}

/**
 * Test 13: GetNeverModified
 */
static void Test_GetNeverModified() {
    TEST_START("MemberPasswdMod_GetNeverModified");

    /* Add a record with cnt = 0 */
    MemberPasswdMod never_modified = {
        .m_id = 10005,
        .first_time = "0000-00-00 00:00:00",
        .last_time = "0000-00-00 00:00:00",
        .cnt = 0
    };
    MemberPasswdMod_Add(&manager, &never_modified);

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetNeverModified(&manager, results, 10);
    int i;

    TEST_ASSERT(count > 0, "Found users who never modified password");
    /* Verify all results have cnt = 0 */
    for (i = 0; i < count; i++) {
        TEST_ASSERT(results[i].cnt == 0, "Record cnt = 0");
    }
}

/**
 * Test 14: GetByMinCnt
 */
static void Test_GetByMinCnt() {
    TEST_START("MemberPasswdMod_GetByMinCnt");

    MemberPasswdMod results[10];
    int count = MemberPasswdMod_GetByMinCnt(&manager, 5, results, 10);
    int i;

    TEST_ASSERT(count >= 0, "Query executed (may return 0 records)");
    /* Verify all results meet minimum */
    for (i = 0; i < count; i++) {
        TEST_ASSERT(results[i].cnt >= 5, "Record cnt >= 5");
    }
}

/**
 * Test 15: BulkAdd
 */
static void Test_BulkAdd() {
    TEST_START("MemberPasswdMod_BulkAdd");

    MemberPasswdMod bulk_records[] = {
        {10006, "2025-03-01 10:00:00", "2025-03-01 10:00:00", 6},
        {10007, "2025-03-02 10:00:00", "2025-03-02 10:00:00", 7},
        {10008, "2025-03-03 10:00:00", "2025-03-03 10:00:00", 8}
    };

    int count = MemberPasswdMod_BulkAdd(&manager, bulk_records, 3);
    TEST_ASSERT(count == 3, "Bulk add 3 records");

    /* Verify one of the added records */
    MemberPasswdMod verify;
    int ret = MemberPasswdMod_Get(&manager, 10006, &verify);
    TEST_ASSERT(ret == 0 && verify.cnt == 6, "Bulk added record exists");
}

/**
 * Test 16: BulkDelete
 */
static void Test_BulkDelete() {
    TEST_START("MemberPasswdMod_BulkDelete");

    int ids_to_delete[] = {10006, 10007, 10008};
    int ret = MemberPasswdMod_BulkDelete(&manager, ids_to_delete, 3);
    TEST_ASSERT(ret == 0, "Bulk delete 3 records");

    /* Verify deletion */
    int exists = MemberPasswdMod_Exists(&manager, 10006);
    TEST_ASSERT(exists == 0, "Bulk deleted record does not exist");
}

/**
 * Test 17: Delete (idempotent)
 */
static void Test_Delete() {
    TEST_START("MemberPasswdMod_Delete (Idempotent)");

    int ret1 = MemberPasswdMod_Delete(&manager, 10001);
    TEST_ASSERT(ret1 == 0, "Delete record with m_id=10001");

    int ret2 = MemberPasswdMod_Delete(&manager, 10001);
    TEST_ASSERT(ret2 == 0, "Delete again (idempotent)");

    int exists = MemberPasswdMod_Exists(&manager, 10001);
    TEST_ASSERT(exists == 0, "Record 10001 no longer exists");
}

/**
 * Test 18: PrintInfo utility
 */
static void Test_PrintInfo() {
    TEST_START("MemberPasswdMod_PrintInfo");

    MemberPasswdMod record = {
        .m_id = 99999,
        .first_time = "2025-12-31 23:59:59",
        .last_time = "2025-12-31 23:59:59",
        .cnt = 255
    };

    printf("  Testing PrintInfo output:\n");
    MemberPasswdMod_PrintInfo(&record);
    TEST_ASSERT(1, "PrintInfo executed");

    printf("  Testing PrintInfo with NULL:\n");
    MemberPasswdMod_PrintInfo(NULL);
    TEST_ASSERT(1, "PrintInfo handles NULL");
}

/**
 * Initialize database connection
 */
static int Init_Database(const char* config_file) {
    printf("Initializing database connection...\n");

    if (DBConnectionManager_Initialize(&manager, config_file) < 0) {
        printf("Failed to initialize database connection manager\n");
        return -1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to test_d_taiwan database\n");
        DBConnectionManager_Cleanup(&manager);
        return -1;
    }

    printf("Database connection established\n");
    return 0;
}

/**
 * Cleanup test data and close connection
 */
static void Cleanup_Database() {
    printf("\nCleaning up test data...\n");

    /* Delete all test records */
    int test_ids[] = {10001, 10002, 10003, 10004, 10005, 10006, 10007, 10008};
    MemberPasswdMod_BulkDelete(&manager, test_ids, 8);

    DBConnectionManager_Cleanup(&manager);
    printf("Database connection closed\n");
}

/**
 * Main test runner
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        printf("Example: %s ../config/database.cfg\n", argv[0]);
        return 1;
    }

    printf("=================================================\n");
    printf("  Member Passwd Mod ORM Test Suite\n");
    printf("  Testing 17 API functions\n");
    printf("=================================================\n");

    /* Initialize database */
    if (Init_Database(argv[1]) < 0) {
        return 1;
    }

    /* Run all tests */
    Test_Add();
    Test_Get();
    Test_Exists();
    Test_Update();
    Test_AddIdempotent();
    Test_IncrementCnt();
    Test_GetByFirstTimeRange();
    Test_GetByLastTimeRange();
    Test_GetByCntRange();
    Test_GetFrequentModifiers();
    Test_GetRecentModifiers();
    Test_UpdateLastTime();
    Test_GetNeverModified();
    Test_GetByMinCnt();
    Test_BulkAdd();
    Test_BulkDelete();
    Test_Delete();
    Test_PrintInfo();

    /* Cleanup */
    Cleanup_Database();

    /* Print summary */
    printf("\n=================================================\n");
    printf("  TEST SUMMARY\n");
    printf("=================================================\n");
    printf("  Tests Run:    %d\n", tests_run);
    printf("  Assertions:   %d total\n", tests_passed + tests_failed);
    printf("  Passed:       %d ✓\n", tests_passed);
    printf("  Failed:       %d ✗\n", tests_failed);
    printf("  Success Rate: %.1f%%\n",
           (tests_passed * 100.0) / (tests_passed + tests_failed));
    printf("=================================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
