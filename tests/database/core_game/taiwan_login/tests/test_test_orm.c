#include "test_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

void test_basic_operations(DBConnectionManager* manager);
void test_null_handling(DBConnectionManager* manager);
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

    printf("=== Test Table ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_operations(&manager);
    test_null_handling(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    Test_Clear(manager);
}

void test_basic_operations(DBConnectionManager* manager) {
    TestRecord record;
    TestRecord records[100];
    int count = 0;

    printf("[Test 1] Add Test Record\n");
    memset(&record, 0, sizeof(TestRecord));
    record.a = 100;
    record.a_is_null = 0;
    strcpy(record.b, "2025-11-18 10:00:00");
    record.b_is_null = 0;

    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add test record\n");
        return;
    }
    printf("  PASSED: Test record added successfully\n");

    printf("\n[Test 2] Add Multiple Records\n");
    memset(&record, 0, sizeof(TestRecord));
    record.a = 200;
    record.a_is_null = 0;
    strcpy(record.b, "2025-11-18 11:00:00");
    record.b_is_null = 0;
    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add second record\n");
        return;
    }

    memset(&record, 0, sizeof(TestRecord));
    record.a = 300;
    record.a_is_null = 0;
    strcpy(record.b, "2025-11-18 12:00:00");
    record.b_is_null = 0;
    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add third record\n");
        return;
    }
    printf("  PASSED: Added 3 test records successfully\n");

    printf("\n[Test 3] Get All Records\n");
    if (Test_GetAll(manager, records, 100, &count) < 0) {
        printf("  FAILED: Could not get all records\n");
        return;
    }
    printf("  PASSED: Retrieved %d records\n", count);

    if (count >= 3) {
        printf("  First 3 records:\n");
        for (int i = 0; i < 3 && i < count; i++) {
            printf("    %d. a=%d, b=%s\n", i + 1, records[i].a, records[i].b);
        }
    }

    printf("\n[Test 4] Count Records\n");
    int total_count = 0;
    if (Test_Count(manager, &total_count) < 0) {
        printf("  FAILED: Could not count records\n");
        return;
    }
    printf("  PASSED: Total records count: %d\n", total_count);

    if (total_count != count) {
        printf("  WARNING: Count mismatch (GetAll: %d, Count: %d)\n", count, total_count);
    }

    printf("\n[Test 5] Get By A Value\n");
    memset(records, 0, sizeof(records));
    if (Test_GetByA(manager, 100, records, 100, &count) < 0) {
        printf("  FAILED: Could not get records by a value\n");
        return;
    }
    printf("  PASSED: Retrieved %d records with a=100\n", count);
    if (count > 0) {
        printf("  Record: a=%d, b=%s\n", records[0].a, records[0].b);
    }

    printf("\n[Test 6] Delete By A Value\n");
    if (Test_DeleteByA(manager, 200) < 0) {
        printf("  FAILED: Could not delete records\n");
        return;
    }
    printf("  PASSED: Records deleted successfully\n");

    if (Test_Count(manager, &total_count) < 0) {
        printf("  FAILED: Could not verify deletion\n");
        return;
    }
    printf("  PASSED: Count after deletion: %d\n", total_count);

    printf("\n[Test 7] Clear All Records\n");
    if (Test_Clear(manager) < 0) {
        printf("  FAILED: Could not clear all records\n");
        return;
    }
    printf("  PASSED: All records cleared successfully\n");

    if (Test_Count(manager, &total_count) < 0) {
        printf("  FAILED: Could not verify clear\n");
        return;
    }
    if (total_count != 0) {
        printf("  FAILED: Records still exist after clear (count=%d)\n", total_count);
        return;
    }
    printf("  PASSED: Clear verification successful (count=0)\n");
}

void test_null_handling(DBConnectionManager* manager) {
    TestRecord record;
    TestRecord records[100];
    int count = 0;

    printf("\n[Test 8] Add Record with NULL a\n");
    memset(&record, 0, sizeof(TestRecord));
    record.a_is_null = 1;
    strcpy(record.b, "2025-11-18 13:00:00");
    record.b_is_null = 0;

    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add record with NULL a\n");
        return;
    }
    printf("  PASSED: Record with NULL a added successfully\n");

    printf("\n[Test 9] Add Record with NULL b\n");
    memset(&record, 0, sizeof(TestRecord));
    record.a = 400;
    record.a_is_null = 0;
    record.b_is_null = 1;

    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add record with NULL b\n");
        return;
    }
    printf("  PASSED: Record with NULL b added successfully\n");

    printf("\n[Test 10] Add Record with Both NULL\n");
    memset(&record, 0, sizeof(TestRecord));
    record.a_is_null = 1;
    record.b_is_null = 1;

    if (Test_Add(manager, &record) < 0) {
        printf("  FAILED: Could not add record with both NULL\n");
        return;
    }
    printf("  PASSED: Record with both NULL added successfully\n");

    printf("\n[Test 11] Verify NULL Handling\n");
    if (Test_GetAll(manager, records, 100, &count) < 0) {
        printf("  FAILED: Could not retrieve records\n");
        return;
    }
    printf("  PASSED: Retrieved %d records with NULL values\n", count);

    if (count >= 3) {
        printf("  Records with NULL handling:\n");
        for (int i = 0; i < count && i < 3; i++) {
            printf("    %d. ", i + 1);
            if (records[i].a_is_null) {
                printf("a=NULL");
            } else {
                printf("a=%d", records[i].a);
            }
            printf(", ");
            if (records[i].b_is_null) {
                printf("b=NULL");
            } else {
                printf("b=%s", records[i].b);
            }
            printf("\n");
        }
    }
}
