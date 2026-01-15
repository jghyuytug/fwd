#include "pswd_qstion_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

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

    printf("=== PswdQstion ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("=== All Tests Completed ===\n");

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM pswd_qstion WHERE q_no >= 100");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    PswdQstion question;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add question */
    printf("  [1] Add question... ");
    memset(&question, 0, sizeof(PswdQstion));
    question.q_no = 100;
    strcpy(question.q_text, "Your pet name?");

    if (PswdQstion_Add(manager, &question) == 0) {
        printf("PASS\n");
        test_passed++;
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 2: Get question */
    printf("  [2] Get question by q_no... ");
    if (PswdQstion_Get(manager, 100, &question) == 0) {
        printf("PASS\n");
        test_passed++;
        PswdQstion_PrintInfo(&question);
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 3: Exists check */
    printf("  [3] Check if question exists... ");
    if (PswdQstion_Exists(manager, 100)) {
        printf("PASS (question exists)\n");
        test_passed++;
    } else {
        printf("FAIL (question should exist)\n");
        test_failed++;
    }

    /* Test 4: Update question */
    printf("  [4] Update question... ");
    memset(&question, 0, sizeof(PswdQstion));
    question.q_no = 100;
    strcpy(question.q_text, "Your first pet?");

    if (PswdQstion_Update(manager, &question) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify update */
        if (PswdQstion_Get(manager, 100, &question) == 0) {
            if (strcmp(question.q_text, "Your first pet?") == 0) {
                printf("    Verified: q_text updated to '%s'\n", question.q_text);
            }
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 5: Delete question */
    printf("  [5] Delete question... ");
    if (PswdQstion_Delete(manager, 100) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify deletion */
        if (!PswdQstion_Exists(manager, 100)) {
            printf("    Verified: question deleted\n");
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    PswdQstion question, questions[20];
    int count, total;

    printf("[TEST] Business Operations\n");

    /* Clean up and add test data */
    cleanup_test_data(manager);

    /* Add multiple questions */
    memset(&question, 0, sizeof(PswdQstion));
    question.q_no = 101;
    strcpy(question.q_text, "Your mother name?");
    PswdQstion_Add(manager, &question);

    question.q_no = 102;
    strcpy(question.q_text, "Your birth city?");
    PswdQstion_Add(manager, &question);

    question.q_no = 103;
    strcpy(question.q_text, "Your first school?");
    PswdQstion_Add(manager, &question);

    question.q_no = 104;
    strcpy(question.q_text, "Your favorite color?");
    PswdQstion_Add(manager, &question);

    /* Test 1: Get all questions */
    printf("  [1] Get all questions... ");
    if (PswdQstion_GetAll(manager, questions, 20, &count) == 0 && count >= 4) {
        printf("PASS (found %d questions)\n", count);
        test_passed++;
        for (int i = 0; i < count && i < 4; i++) {
            printf("    Q%d: %s\n", questions[i].q_no, questions[i].q_text);
        }
    } else {
        printf("FAIL (expected >= 4, got %d)\n", count);
        test_failed++;
    }

    /* Test 2: Get by text pattern */
    printf("  [2] Get by text pattern (name)... ");
    if (PswdQstion_GetByTextPattern(manager, "name", questions, 20, &count) == 0 && count >= 1) {
        printf("PASS (found %d questions)\n", count);
        test_passed++;
        for (int i = 0; i < count; i++) {
            printf("    Q%d: %s\n", questions[i].q_no, questions[i].q_text);
        }
    } else {
        printf("FAIL (expected >= 1, got %d)\n", count);
        test_failed++;
    }

    /* Test 3: Count total questions */
    printf("  [3] Count total questions... ");
    if (PswdQstion_Count(manager, &total) == 0 && total >= 4) {
        printf("PASS (total: %d)\n", total);
        test_passed++;
    } else {
        printf("FAIL (expected >= 4, got %d)\n", total);
        test_failed++;
    }

    printf("\n");
}
