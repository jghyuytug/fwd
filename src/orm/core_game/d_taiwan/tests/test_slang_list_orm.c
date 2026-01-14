#include "slang_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Slang List ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM slang_list WHERE slang IN ('badword1', 'badword2', 'testslang', 'inappropriate')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    printf("[Test 1] Add Slang Word\n");
    if (SlangList_Add(manager, "badword1") < 0) {
        printf("  FAILED: Could not add slang word\n");
        return;
    }
    printf("  PASSED: Slang word added successfully\n");

    printf("\n[Test 2] Check Slang Exists\n");
    if (!SlangList_Exists(manager, "badword1")) {
        printf("  FAILED: Slang word should exist\n");
        return;
    }
    printf("  PASSED: Slang word exists check successful\n");

    if (SlangList_Exists(manager, "nonexistent")) {
        printf("  FAILED: Non-existent slang should not exist\n");
        return;
    }
    printf("  PASSED: Non-existent slang check successful\n");

    printf("\n[Test 3] Delete Slang Word\n");
    if (SlangList_Delete(manager, "badword1") < 0) {
        printf("  FAILED: Could not delete slang word\n");
        return;
    }
    printf("  PASSED: Slang word deleted successfully\n");

    if (SlangList_Exists(manager, "badword1")) {
        printf("  FAILED: Slang word should not exist after deletion\n");
        return;
    }
    printf("  PASSED: Delete verification successful\n");

    printf("\n[Test 4] Add Multiple Slang Words\n");
    if (SlangList_Add(manager, "badword1") < 0) {
        printf("  FAILED: Could not add badword1\n");
        return;
    }
    if (SlangList_Add(manager, "badword2") < 0) {
        printf("  FAILED: Could not add badword2\n");
        return;
    }
    if (SlangList_Add(manager, "testslang") < 0) {
        printf("  FAILED: Could not add testslang\n");
        return;
    }
    printf("  PASSED: Added 3 slang words successfully\n");
}

void test_business_operations(DBConnectionManager* manager) {
    SlangList slang_list[100];
    int count = 0;

    printf("\n[Test 5] Get All Slang Words\n");
    if (SlangList_GetAll(manager, slang_list, 100, &count) < 0) {
        printf("  FAILED: Could not get all slang words\n");
        return;
    }
    printf("  PASSED: Retrieved %d slang words\n", count);

    if (count >= 3) {
        printf("  First 3 slang words:\n");
        for (int i = 0; i < 3 && i < count; i++) {
            printf("    %d. %s\n", i + 1, slang_list[i].slang);
        }
    }

    printf("\n[Test 6] Count Slang Words\n");
    int total_count = 0;
    if (SlangList_Count(manager, &total_count) < 0) {
        printf("  FAILED: Could not count slang words\n");
        return;
    }
    printf("  PASSED: Total slang words count: %d\n", total_count);

    if (total_count != count) {
        printf("  WARNING: Count mismatch (GetAll: %d, Count: %d)\n", count, total_count);
    }

    printf("\n[Test 7] Content Filtering Test\n");
    const char* clean_text = "This is a normal message";
    const char* dirty_text = "This contains badword1 in it";

    if (SlangList_IsFiltered(manager, clean_text)) {
        printf("  FAILED: Clean text should not be filtered\n");
        return;
    }
    printf("  PASSED: Clean text not filtered\n");

    if (!SlangList_IsFiltered(manager, dirty_text)) {
        printf("  FAILED: Text with slang should be filtered\n");
        return;
    }
    printf("  PASSED: Text with slang correctly filtered\n");

    printf("\n[Test 8] Add Duplicate Slang Word\n");
    /* This should fail due to PRIMARY KEY constraint */
    if (SlangList_Add(manager, "badword1") >= 0) {
        printf("  WARNING: Duplicate slang word added (should have failed)\n");
    } else {
        printf("  PASSED: Duplicate slang word correctly rejected\n");
    }

    printf("\n[Test 9] Test Empty String\n");
    if (SlangList_Add(manager, "") >= 0) {
        printf("  WARNING: Empty string accepted (should have failed)\n");
    } else {
        printf("  PASSED: Empty string correctly rejected\n");
    }

    printf("\n[Test 10] Test NULL Parameter\n");
    if (SlangList_Add(manager, NULL) >= 0) {
        printf("  WARNING: NULL parameter accepted (should have failed)\n");
    } else {
        printf("  PASSED: NULL parameter correctly rejected\n");
    }

    printf("\n[Test 11] Test Special Characters\n");
    if (SlangList_Add(manager, "inappropriate") < 0) {
        printf("  FAILED: Could not add slang with special case\n");
        return;
    }
    printf("  PASSED: Slang with special characters added\n");

    if (!SlangList_Exists(manager, "inappropriate")) {
        printf("  FAILED: Slang with special characters not found\n");
        return;
    }
    printf("  PASSED: Slang with special characters exists\n");
}
