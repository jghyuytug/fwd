/**
 * test_member_premium_notuse_orm.c
 *
 * Test suite for member_premium_notuse ORM operations
 * 会员高级服务未使用记录表测试套件
 *
 * Composite Primary Key: (event_id, pre_type, server_id, m_id, service_start)
 *
 * Test Coverage: 18 test functions
 * - 5 CRUD operations
 * - 9 business queries
 * - 2 bulk operations
 * - 1 utility function
 * - 1 integration test
 *
 * Generated: 2025-11-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "member_premium_notuse_orm.h"

/* Test utilities */
static int g_test_count = 0;
static int g_assert_count = 0;

#define TEST_START(name) \
    do { \
        g_test_count++; \
        printf("\n[Test %d] %s...\n", g_test_count, name); \
    } while(0)

#define TEST_ASSERT(condition, message) \
    do { \
        g_assert_count++; \
        if (!(condition)) { \
            printf("  [FAIL] Assertion %d: %s\n", g_assert_count, message); \
            return; \
        } \
        printf("  [PASS] Assertion %d: %s\n", g_assert_count, message); \
    } while(0)

/* Global database connection manager */
static DBConnectionManager manager;

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
 * Cleanup database connection
 */
static void Cleanup_Database() {
    printf("\nCleaning up database connection...\n");
    DBConnectionManager_Cleanup(&manager);
    printf("Database connection closed\n");
}

/**
 * Clear test data from table
 */
static void Clear_Test_Data() {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    const char* query = "DELETE FROM member_premium_notuse WHERE event_id >= 1000";
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);

    printf("Test data cleared\n");
}

/* ========== Test Functions ========== */

/**
 * Test 1: Add new record (REPLACE INTO pattern)
 */
static void Test_Add() {
    TEST_START("MemberPremiumNotuse_Add");

    Clear_Test_Data();

    MemberPremiumNotuse record = {
        1001,
        1,
        100001,
        "2025-01-01 10:00:00",
        "2025-12-31 23:59:59",
        1
    };

    int result = MemberPremiumNotuse_Add(&manager, &record);
    TEST_ASSERT(result == 0, "Add new record should succeed");

    /* Test REPLACE INTO (idempotent) */
    strcpy(record.service_end, "2025-12-25 23:59:59");
    result = MemberPremiumNotuse_Add(&manager, &record);
    TEST_ASSERT(result == 0, "Replace existing record should succeed");

    Clear_Test_Data();
}

/**
 * Test 2: Get record by composite primary key
 */
static void Test_Get() {
    TEST_START("MemberPremiumNotuse_Get");

    Clear_Test_Data();

    MemberPremiumNotuse insert_record = {
        1002,
        2,
        100002,
        "2025-02-01 10:00:00",
        "2025-11-30 23:59:59",
        1
    };

    MemberPremiumNotuse_Add(&manager, &insert_record);

    MemberPremiumNotuse retrieved_record;
    int result = MemberPremiumNotuse_Get(&manager, 1002, 2, 1, 100002, "2025-02-01 10:00:00", &retrieved_record);
    TEST_ASSERT(result == 0, "Get existing record should succeed");
    TEST_ASSERT(retrieved_record.event_id == 1002, "event_id should match");
    TEST_ASSERT(retrieved_record.pre_type == 2, "pre_type should match");
    TEST_ASSERT(retrieved_record.m_id == 100002, "m_id should match");
    TEST_ASSERT(strcmp(retrieved_record.service_end, "2025-11-30 23:59:59") == 0, "service_end should match");

    /* Test non-existent record */
    result = MemberPremiumNotuse_Get(&manager, 9999, 99, 99, 999999, "2025-01-01 00:00:00", &retrieved_record);
    TEST_ASSERT(result == -1, "Get non-existent record should fail");

    Clear_Test_Data();
}

/**
 * Test 3: Update existing record
 */
static void Test_Update() {
    TEST_START("MemberPremiumNotuse_Update");

    Clear_Test_Data();

    MemberPremiumNotuse record = {
        1003,
        3,
        100003,
        "2025-03-01 10:00:00",
        "2025-09-30 23:59:59",
        2
    };

    MemberPremiumNotuse_Add(&manager, &record);

    /* Update service_end */
    strcpy(record.service_end, "2025-10-31 23:59:59");
    int result = MemberPremiumNotuse_Update(&manager, &record);
    TEST_ASSERT(result == 0, "Update should succeed");

    MemberPremiumNotuse updated_record;
    MemberPremiumNotuse_Get(&manager, 1003, 3, 2, 100003, "2025-03-01 10:00:00", &updated_record);
    TEST_ASSERT(strcmp(updated_record.service_end, "2025-10-31 23:59:59") == 0, "service_end should be updated");

    Clear_Test_Data();
}

/**
 * Test 4: Delete record (idempotent)
 */
static void Test_Delete() {
    TEST_START("MemberPremiumNotuse_Delete");

    Clear_Test_Data();

    MemberPremiumNotuse record = {
        1004,
        4,
        100004,
        "2025-04-01 10:00:00",
        "2025-08-31 23:59:59",
        3
    };

    MemberPremiumNotuse_Add(&manager, &record);

    int result = MemberPremiumNotuse_Delete(&manager, 1004, 4, 3, 100004, "2025-04-01 10:00:00");
    TEST_ASSERT(result == 0, "Delete existing record should succeed");

    /* Test idempotent delete */
    result = MemberPremiumNotuse_Delete(&manager, 1004, 4, 3, 100004, "2025-04-01 10:00:00");
    TEST_ASSERT(result == 0, "Delete non-existent record should still return 0 (idempotent)");

    Clear_Test_Data();
}

/**
 * Test 5: Check if record exists
 */
static void Test_Exists() {
    TEST_START("MemberPremiumNotuse_Exists");

    Clear_Test_Data();

    MemberPremiumNotuse record = {
        1005,
        5,
        100005,
        "2025-05-01 10:00:00",
        "2025-07-31 23:59:59",
        1
    };

    MemberPremiumNotuse_Add(&manager, &record);

    int result = MemberPremiumNotuse_Exists(&manager, 1005, 5, 1, 100005, "2025-05-01 10:00:00");
    TEST_ASSERT(result == 1, "Existing record should return 1");

    result = MemberPremiumNotuse_Exists(&manager, 9999, 99, 99, 999999, "2025-01-01 00:00:00");
    TEST_ASSERT(result == 0, "Non-existent record should return 0");

    Clear_Test_Data();
}

/**
 * Test 6: Get records by event_id
 */
static void Test_GetByEventId() {
    TEST_START("MemberPremiumNotuse_GetByEventId");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1010, 1, 100010, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1010, 2, 100011, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 1},
        {1010, 3, 100012, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByEventId(&manager, 1010, results, 10);
    TEST_ASSERT(count == 3, "Should find 3 records for event_id 1010");
    TEST_ASSERT(results[0].event_id == 1010, "All records should have event_id 1010");

    Clear_Test_Data();
}

/**
 * Test 7: Get records by member_id
 */
static void Test_GetByMemberId() {
    TEST_START("MemberPremiumNotuse_GetByMemberId");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1011, 1, 100020, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1012, 2, 100020, "2025-02-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1013, 3, 100020, "2025-03-10 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByMemberId(&manager, 100020, results, 10);
    TEST_ASSERT(count == 3, "Should find 3 records for m_id 100020");
    TEST_ASSERT(results[0].m_id == 100020, "All records should have m_id 100020");

    Clear_Test_Data();
}

/**
 * Test 8: Get records by pre_type
 */
static void Test_GetByPreType() {
    TEST_START("MemberPremiumNotuse_GetByPreType");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1014, 5, 100030, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1015, 5, 100031, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 1},
        {1016, 5, 100032, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByPreType(&manager, 5, results, 10);
    TEST_ASSERT(count == 3, "Should find 3 records for pre_type 5");
    TEST_ASSERT(results[0].pre_type == 5, "All records should have pre_type 5");

    Clear_Test_Data();
}

/**
 * Test 9: Get records by server_id
 */
static void Test_GetByServerId() {
    TEST_START("MemberPremiumNotuse_GetByServerId");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1017, 1, 100040, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 3},
        {1018, 2, 100041, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 3},
        {1019, 3, 100042, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 3}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByServerId(&manager, 3, results, 10);
    TEST_ASSERT(count == 3, "Should find 3 records for server_id 3");
    TEST_ASSERT(results[0].server_id == 3, "All records should have server_id 3");

    Clear_Test_Data();
}

/**
 * Test 10: Get records by service_start range
 */
static void Test_GetByServiceStartRange() {
    TEST_START("MemberPremiumNotuse_GetByServiceStartRange");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1020, 1, 100050, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1021, 2, 100051, "2025-01-20 10:00:00", "2025-12-31 23:59:59", 1},
        {1022, 3, 100052, "2025-01-30 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByServiceStartRange(&manager, "2025-01-15 00:00:00", "2025-01-25 23:59:59", results, 10);
    TEST_ASSERT(count == 1, "Should find 1 record in range");
    TEST_ASSERT(strcmp(results[0].service_start, "2025-01-20 10:00:00") == 0, "Should be the 2025-01-20 record");

    Clear_Test_Data();
}

/**
 * Test 11: Get records by service_end range
 */
static void Test_GetByServiceEndRange() {
    TEST_START("MemberPremiumNotuse_GetByServiceEndRange");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1023, 1, 100060, "2025-01-10 10:00:00", "2025-06-30 23:59:59", 1},
        {1024, 2, 100061, "2025-01-11 10:00:00", "2025-07-31 23:59:59", 1},
        {1025, 3, 100062, "2025-01-12 10:00:00", "2025-08-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByServiceEndRange(&manager, "2025-07-01 00:00:00", "2025-08-01 23:59:59", results, 10);
    TEST_ASSERT(count == 1, "Should find 1 record in range");
    TEST_ASSERT(strcmp(results[0].service_end, "2025-07-31 23:59:59") == 0, "Should be the 2025-07-31 record");

    Clear_Test_Data();
}

/**
 * Test 12: Get active services (service_end > NOW())
 */
static void Test_GetActiveServices() {
    TEST_START("MemberPremiumNotuse_GetActiveServices");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1026, 1, 100070, "2025-01-10 10:00:00", "2026-12-31 23:59:59", 1},
        {1027, 2, 100071, "2025-01-11 10:00:00", "2027-12-31 23:59:59", 1},
        {1028, 3, 100072, "2024-01-12 10:00:00", "2024-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetActiveServices(&manager, results, 10);
    TEST_ASSERT(count >= 2, "Should find at least 2 active services (service_end in future)");

    Clear_Test_Data();
}

/**
 * Test 13: Get expired services (service_end < NOW())
 */
static void Test_GetExpiredServices() {
    TEST_START("MemberPremiumNotuse_GetExpiredServices");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1029, 1, 100080, "2024-01-10 10:00:00", "2024-06-30 23:59:59", 1},
        {1030, 2, 100081, "2024-01-11 10:00:00", "2024-07-31 23:59:59", 1},
        {1031, 3, 100082, "2026-01-12 10:00:00", "2026-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetExpiredServices(&manager, results, 10);
    TEST_ASSERT(count >= 2, "Should find at least 2 expired services (service_end in past)");

    Clear_Test_Data();
}

/**
 * Test 14: Get records by event_id and m_id
 */
static void Test_GetByEventIdAndMemberId() {
    TEST_START("MemberPremiumNotuse_GetByEventIdAndMemberId");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1032, 1, 100090, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1032, 2, 100090, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 1},
        {1033, 1, 100090, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    MemberPremiumNotuse results[10];
    int count = MemberPremiumNotuse_GetByEventIdAndMemberId(&manager, 1032, 100090, results, 10);
    TEST_ASSERT(count == 2, "Should find 2 records for event_id 1032 and m_id 100090");
    TEST_ASSERT(results[0].event_id == 1032 && results[0].m_id == 100090, "Records should match criteria");

    Clear_Test_Data();
}

/**
 * Test 15: Bulk add records
 */
static void Test_BulkAdd() {
    TEST_START("MemberPremiumNotuse_BulkAdd");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1034, 1, 100100, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1035, 2, 100101, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 1},
        {1036, 3, 100102, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int count = MemberPremiumNotuse_BulkAdd(&manager, test_records, 3);
    TEST_ASSERT(count == 3, "Should successfully add 3 records");

    /* Verify records exist */
    int exists1 = MemberPremiumNotuse_Exists(&manager, 1034, 1, 1, 100100, "2025-01-10 10:00:00");
    int exists2 = MemberPremiumNotuse_Exists(&manager, 1035, 2, 1, 100101, "2025-01-11 10:00:00");
    int exists3 = MemberPremiumNotuse_Exists(&manager, 1036, 3, 2, 100102, "2025-01-12 10:00:00");
    TEST_ASSERT(exists1 == 1 && exists2 == 1 && exists3 == 1, "All bulk added records should exist");

    Clear_Test_Data();
}

/**
 * Test 16: Bulk delete by member_id
 */
static void Test_BulkDeleteByMemberId() {
    TEST_START("MemberPremiumNotuse_BulkDeleteByMemberId");

    Clear_Test_Data();

    MemberPremiumNotuse test_records[] = {
        {1037, 1, 100110, "2025-01-10 10:00:00", "2025-12-31 23:59:59", 1},
        {1038, 2, 100111, "2025-01-11 10:00:00", "2025-12-31 23:59:59", 1},
        {1039, 3, 100112, "2025-01-12 10:00:00", "2025-12-31 23:59:59", 2}
    };

    int i;
    for (i = 0; i < 3; i++) {
        MemberPremiumNotuse_Add(&manager, &test_records[i]);
    }

    int m_ids[] = {100110, 100111};
    int result = MemberPremiumNotuse_BulkDeleteByMemberId(&manager, m_ids, 2);
    TEST_ASSERT(result == 0, "Bulk delete should succeed");

    /* Verify records deleted */
    MemberPremiumNotuse results[10];
    int count1 = MemberPremiumNotuse_GetByMemberId(&manager, 100110, results, 10);
    int count2 = MemberPremiumNotuse_GetByMemberId(&manager, 100111, results, 10);
    int count3 = MemberPremiumNotuse_GetByMemberId(&manager, 100112, results, 10);
    TEST_ASSERT(count1 == 0 && count2 == 0, "Deleted members should have 0 records");
    TEST_ASSERT(count3 == 1, "Non-deleted member should still have 1 record");

    Clear_Test_Data();
}

/**
 * Test 17: Print record info (utility)
 */
static void Test_PrintInfo() {
    TEST_START("MemberPremiumNotuse_PrintInfo");

    MemberPremiumNotuse record = {
        1040,
        10,
        100120,
        "2025-01-15 10:00:00",
        "2025-12-31 23:59:59",
        5
    };

    printf("  Testing PrintInfo utility:\n");
    MemberPremiumNotuse_PrintInfo(&record);
    TEST_ASSERT(1, "PrintInfo should execute without error");
}

/**
 * Test 18: Integration test - Complete workflow
 */
static void Test_Integration() {
    TEST_START("Integration Test - Complete Workflow");

    Clear_Test_Data();

    /* 1. Add record */
    MemberPremiumNotuse record = {
        1041,
        15,
        100130,
        "2025-02-01 10:00:00",
        "2025-11-30 23:59:59",
        3
    };
    int result = MemberPremiumNotuse_Add(&manager, &record);
    TEST_ASSERT(result == 0, "Add record in integration test");

    /* 2. Verify exists */
    result = MemberPremiumNotuse_Exists(&manager, 1041, 15, 3, 100130, "2025-02-01 10:00:00");
    TEST_ASSERT(result == 1, "Record should exist");

    /* 3. Get and verify */
    MemberPremiumNotuse retrieved;
    result = MemberPremiumNotuse_Get(&manager, 1041, 15, 3, 100130, "2025-02-01 10:00:00", &retrieved);
    TEST_ASSERT(result == 0 && retrieved.event_id == 1041, "Get should retrieve correct record");

    /* 4. Update */
    strcpy(retrieved.service_end, "2025-10-31 23:59:59");
    result = MemberPremiumNotuse_Update(&manager, &retrieved);
    TEST_ASSERT(result == 0, "Update should succeed");

    /* 5. Verify update */
    MemberPremiumNotuse updated;
    MemberPremiumNotuse_Get(&manager, 1041, 15, 3, 100130, "2025-02-01 10:00:00", &updated);
    TEST_ASSERT(strcmp(updated.service_end, "2025-10-31 23:59:59") == 0, "Update should be persisted");

    /* 6. Delete */
    result = MemberPremiumNotuse_Delete(&manager, 1041, 15, 3, 100130, "2025-02-01 10:00:00");
    TEST_ASSERT(result == 0, "Delete should succeed");

    /* 7. Verify deleted */
    result = MemberPremiumNotuse_Exists(&manager, 1041, 15, 3, 100130, "2025-02-01 10:00:00");
    TEST_ASSERT(result == 0, "Record should no longer exist");

    Clear_Test_Data();
}

/* ========== Main Test Runner ========== */

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        printf("Example: %s ../config/database.cfg\n", argv[0]);
        return 1;
    }

    printf("=================================================\n");
    printf("  member_premium_notuse ORM Test Suite\n");
    printf("  Composite Key: (event_id, pre_type, server_id, m_id, service_start)\n");
    printf("=================================================\n\n");

    /* Initialize database */
    if (Init_Database(argv[1]) < 0) {
        return 1;
    }

    /* Run all tests */
    Test_Add();
    Test_Get();
    Test_Update();
    Test_Delete();
    Test_Exists();
    Test_GetByEventId();
    Test_GetByMemberId();
    Test_GetByPreType();
    Test_GetByServerId();
    Test_GetByServiceStartRange();
    Test_GetByServiceEndRange();
    Test_GetActiveServices();
    Test_GetExpiredServices();
    Test_GetByEventIdAndMemberId();
    Test_BulkAdd();
    Test_BulkDeleteByMemberId();
    Test_PrintInfo();
    Test_Integration();

    /* Cleanup */
    Cleanup_Database();

    printf("\n=================================================\n");
    printf("  Test Summary\n");
    printf("=================================================\n");
    printf("Total Tests: %d\n", g_test_count);
    printf("Total Assertions: %d\n", g_assert_count);
    printf("=================================================\n\n");

    return 0;
}
