#include "slang_list_name_orm.h"
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

    printf("=== Slang List Name ORM Test Suite ===\n\n");

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
        "DELETE FROM slang_list_name WHERE slang IN ('PersonName1', 'PersonName2', 'TestName', 'Username')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    printf("[Test 1] Add Slang Name\n");
    if (SlangListName_Add(manager, "PersonName1") < 0) {
        printf("  FAILED: Could not add slang name\n");
        return;
    }
    printf("  PASSED: Slang name added successfully\n");

    printf("\n[Test 2] Check Slang Name Exists\n");
    if (!SlangListName_Exists(manager, "PersonName1")) {
        printf("  FAILED: Slang name should exist\n");
        return;
    }
    printf("  PASSED: Slang name exists check successful\n");

    if (SlangListName_Exists(manager, "NonExistentName")) {
        printf("  FAILED: Non-existent slang name should not exist\n");
        return;
    }
    printf("  PASSED: Non-existent slang name check successful\n");

    printf("\n[Test 3] Delete Slang Name\n");
    if (SlangListName_Delete(manager, "PersonName1") < 0) {
        printf("  FAILED: Could not delete slang name\n");
        return;
    }
    printf("  PASSED: Slang name deleted successfully\n");

    if (SlangListName_Exists(manager, "PersonName1")) {
        printf("  FAILED: Slang name should not exist after deletion\n");
        return;
    }
    printf("  PASSED: Delete verification successful\n");

    printf("\n[Test 4] Add Multiple Slang Names\n");
    if (SlangListName_Add(manager, "PersonName1") < 0) {
        printf("  FAILED: Could not add PersonName1\n");
        return;
    }
    if (SlangListName_Add(manager, "PersonName2") < 0) {
        printf("  FAILED: Could not add PersonName2\n");
        return;
    }
    if (SlangListName_Add(manager, "TestName") < 0) {
        printf("  FAILED: Could not add TestName\n");
        return;
    }
    printf("  PASSED: Added 3 slang names successfully\n");
}

void test_business_operations(DBConnectionManager* manager) {
    SlangListName slang_list[100];
    int count = 0;

    printf("\n[Test 5] Get All Slang Names\n");
    if (SlangListName_GetAll(manager, slang_list, 100, &count) < 0) {
        printf("  FAILED: Could not get all slang names\n");
        return;
    }
    printf("  PASSED: Retrieved %d slang names\n", count);

    if (count >= 3) {
        printf("  First 3 slang names:\n");
        for (int i = 0; i < 3 && i < count; i++) {
            printf("    %d. %s\n", i + 1, slang_list[i].slang);
        }
    }

    printf("\n[Test 6] Count Slang Names\n");
    int total_count = 0;
    if (SlangListName_Count(manager, &total_count) < 0) {
        printf("  FAILED: Could not count slang names\n");
        return;
    }
    printf("  PASSED: Total slang names count: %d\n", total_count);

    if (total_count != count) {
        printf("  WARNING: Count mismatch (GetAll: %d, Count: %d)\n", count, total_count);
    }

    printf("\n[Test 7] Content Filtering Test\n");
    const char* clean_text = "This is a normal message";
    const char* dirty_text = "This contains PersonName1 in it";

    if (SlangListName_IsFiltered(manager, clean_text)) {
        printf("  FAILED: Clean text should not be filtered\n");
        return;
    }
    printf("  PASSED: Clean text not filtered\n");

    if (!SlangListName_IsFiltered(manager, dirty_text)) {
        printf("  FAILED: Text with slang name should be filtered\n");
        return;
    }
    printf("  PASSED: Text with slang name correctly filtered\n");

    printf("\n[Test 8] Add Duplicate Slang Name\n");
    /* This should fail due to PRIMARY KEY constraint */
    if (SlangListName_Add(manager, "PersonName1") >= 0) {
        printf("  WARNING: Duplicate slang name added (should have failed)\n");
    } else {
        printf("  PASSED: Duplicate slang name correctly rejected\n");
    }

    printf("\n[Test 9] Test Empty String\n");
    if (SlangListName_Add(manager, "") >= 0) {
        printf("  WARNING: Empty string accepted (should have failed)\n");
    } else {
        printf("  PASSED: Empty string correctly rejected\n");
    }

    printf("\n[Test 10] Test NULL Parameter\n");
    if (SlangListName_Add(manager, NULL) >= 0) {
        printf("  WARNING: NULL parameter accepted (should have failed)\n");
    } else {
        printf("  PASSED: NULL parameter correctly rejected\n");
    }

    printf("\n[Test 11] Test Special Characters\n");
    if (SlangListName_Add(manager, "Username") < 0) {
        printf("  FAILED: Could not add name with special case\n");
        return;
    }
    printf("  PASSED: Name with special characters added\n");

    if (!SlangListName_Exists(manager, "Username")) {
        printf("  FAILED: Name with special characters not found\n");
        return;
    }
    printf("  PASSED: Name with special characters exists\n");
}
