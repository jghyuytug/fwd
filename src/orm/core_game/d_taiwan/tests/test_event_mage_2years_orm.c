#include "event_mage_2years_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void test_soft_delete(DBConnectionManager* manager);
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Event Mage 2years ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);
    test_soft_delete(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    return test_failed > 0 ? 1 : 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_mage_2years WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventMage2years record;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add record */
    memset(&record, 0, sizeof(EventMage2years));
    record.m_id = 100001;
    record.server_info = 1;
    strcpy(record.charac_name, "TestMage");
    strcpy(record.create_time, "2025-01-01 10:00:00");
    strcpy(record.delete_time, "0000-00-00 00:00:00");
    record.delete_flag = 0;

    ret = EventMage2years_Add(manager, &record);
    TEST_ASSERT(ret == 0, "Add record");

    /* Note: charac_no is auto_increment, need to get it */
    EventMage2years records[10];
    int actual_count;
    ret = EventMage2years_GetByMemberId(manager, 100001, records, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count > 0, "Get added record to retrieve charac_no");

    int charac_no = records[0].charac_no;

    /* Test 2: Exists check */
    ret = EventMage2years_Exists(manager, 100001, 1, charac_no);
    TEST_ASSERT(ret == 1, "Record exists after add");

    /* Test 3: Get record */
    memset(&record, 0, sizeof(EventMage2years));
    ret = EventMage2years_Get(manager, 100001, 1, charac_no, &record);
    TEST_ASSERT(ret == 0, "Get record");
    TEST_ASSERT(record.m_id == 100001, "Get record - m_id correct");
    TEST_ASSERT(record.server_info == 1, "Get record - server_info correct");
    TEST_ASSERT(strcmp(record.charac_name, "TestMage") == 0, "Get record - charac_name correct");

    /* Test 4: Update record */
    strcpy(record.charac_name, "UpdatedMage");
    record.delete_flag = 0;
    ret = EventMage2years_Update(manager, &record);
    TEST_ASSERT(ret == 0, "Update record");

    /* Test 5: Verify update */
    memset(&record, 0, sizeof(EventMage2years));
    ret = EventMage2years_Get(manager, 100001, 1, charac_no, &record);
    TEST_ASSERT(ret == 0, "Get updated record");
    TEST_ASSERT(strcmp(record.charac_name, "UpdatedMage") == 0, "Update - charac_name changed");

    /* Test 6: Delete record */
    ret = EventMage2years_Delete(manager, 100001, 1, charac_no);
    TEST_ASSERT(ret == 0, "Delete record");

    /* Test 7: Verify delete */
    ret = EventMage2years_Exists(manager, 100001, 1, charac_no);
    TEST_ASSERT(ret == 0, "Record not exists after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventMage2years records[10];
    EventMage2years test_record;
    int actual_count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data */
    memset(&test_record, 0, sizeof(EventMage2years));
    test_record.m_id = 100002;
    test_record.server_info = 1;
    strcpy(test_record.charac_name, "Mage1");
    strcpy(test_record.create_time, "2025-01-01 10:00:00");
    strcpy(test_record.delete_time, "0000-00-00 00:00:00");
    test_record.delete_flag = 0;
    EventMage2years_Add(manager, &test_record);

    test_record.m_id = 100002;
    test_record.server_info = 2;
    strcpy(test_record.charac_name, "Mage2");
    EventMage2years_Add(manager, &test_record);

    test_record.m_id = 100003;
    test_record.server_info = 1;
    strcpy(test_record.charac_name, "Mage3");
    EventMage2years_Add(manager, &test_record);

    /* Test 1: GetByMemberId */
    memset(records, 0, sizeof(records));
    ret = EventMage2years_GetByMemberId(manager, 100002, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByMemberId operation");
    TEST_ASSERT(actual_count == 2, "GetByMemberId - found 2 records for m_id=100002");

    /* Test 2: GetByServerInfo */
    memset(records, 0, sizeof(records));
    ret = EventMage2years_GetByServerInfo(manager, 1, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByServerInfo operation");
    TEST_ASSERT(actual_count >= 2, "GetByServerInfo - found at least 2 records for server_info=1");

    /* Test 3: GetActiveCharacters (delete_flag = 0) */
    memset(records, 0, sizeof(records));
    ret = EventMage2years_GetActiveCharacters(manager, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetActiveCharacters operation");
    TEST_ASSERT(actual_count >= 3, "GetActiveCharacters - found at least 3 active characters");

    printf("\n");
}

void test_soft_delete(DBConnectionManager* manager) {
    EventMage2years record;
    EventMage2years records[10];
    int actual_count;
    int ret;

    printf("--- Test: Soft Delete ---\n");

    /* Add test data */
    memset(&record, 0, sizeof(EventMage2years));
    record.m_id = 100003;
    record.server_info = 3;
    strcpy(record.charac_name, "ToDelete");
    strcpy(record.create_time, "2025-01-01 10:00:00");
    strcpy(record.delete_time, "0000-00-00 00:00:00");
    record.delete_flag = 0;
    EventMage2years_Add(manager, &record);

    /* Get charac_no - find the one with server_info=3 */
    ret = EventMage2years_GetByMemberId(manager, 100003, records, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count > 0, "Get record to soft delete");

    int charac_no = 0;
    for (int i = 0; i < actual_count; i++) {
        if (records[i].server_info == 3) {
            charac_no = records[i].charac_no;
            break;
        }
    }
    TEST_ASSERT(charac_no > 0, "Found record with server_info=3");

    /* Test 1: Soft delete */
    ret = EventMage2years_SoftDelete(manager, 100003, 3, charac_no, "2025-01-02 12:00:00");
    TEST_ASSERT(ret == 0, "Soft delete operation");

    /* Test 2: Verify soft delete - record still exists */
    ret = EventMage2years_Exists(manager, 100003, 3, charac_no);
    TEST_ASSERT(ret == 1, "Record still exists after soft delete");

    /* Test 3: Verify delete_flag is set */
    memset(&record, 0, sizeof(EventMage2years));
    ret = EventMage2years_Get(manager, 100003, 3, charac_no, &record);
    TEST_ASSERT(ret == 0, "Get soft deleted record");
    TEST_ASSERT(record.delete_flag == 1, "Soft delete - delete_flag set to 1");
    TEST_ASSERT(strcmp(record.delete_time, "2025-01-02 12:00:00") == 0, "Soft delete - delete_time updated");

    /* Test 4: Verify not in active characters */
    memset(records, 0, sizeof(records));
    ret = EventMage2years_GetActiveCharacters(manager, records, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetActiveCharacters after soft delete");

    int found_deleted = 0;
    for (int i = 0; i < actual_count; i++) {
        if (records[i].m_id == 100003 && records[i].server_info == 3 && records[i].charac_no == charac_no) {
            found_deleted = 1;
            break;
        }
    }
    TEST_ASSERT(found_deleted == 0, "Soft deleted record not in active characters");

    printf("\n");
}
