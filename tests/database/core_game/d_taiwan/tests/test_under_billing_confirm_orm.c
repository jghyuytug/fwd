#include "under_billing_confirm_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;

/* Test result reporting */
void test_result(const char* test_name, int passed) {
    test_total++;
    if (passed) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        test_failed++;
        printf("[FAIL] %s\n", test_name);
    }
}

/* Cleanup test data */
void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM under_billing_confirm WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

/* Test basic CRUD operations */
void test_basic_crud(DBConnectionManager* manager) {
    UnderBillingConfirm confirm;
    int ret;

    printf("\n=== Test Basic CRUD Operations ===\n");

    /* Test Add */
    memset(&confirm, 0, sizeof(UnderBillingConfirm));
    confirm.m_id = 100001;
    strcpy(confirm.parent_name, "Kim");
    confirm.parent_jumin = 7012341234567ULL;
    confirm.parent_phone1 = 10;
    confirm.parent_phone2 = 1234;
    confirm.parent_phone3 = 5678;
    strcpy(confirm.parent_email, "parent@example.com");
    confirm.parent_consent_type = 1;
    confirm.create_date = (unsigned int)time(NULL);
    confirm.consent_date = (unsigned int)time(NULL);
    confirm.consent_yn = 1;

    ret = UnderBillingConfirm_Add(manager, &confirm);
    test_result("UnderBillingConfirm_Add", ret == 0);

    /* Test Exists */
    ret = UnderBillingConfirm_Exists(manager, 100001);
    test_result("UnderBillingConfirm_Exists (should exist)", ret == 1);

    /* Test Get */
    UnderBillingConfirm retrieved;
    memset(&retrieved, 0, sizeof(UnderBillingConfirm));
    ret = UnderBillingConfirm_Get(manager, 100001, &retrieved);
    test_result("UnderBillingConfirm_Get", ret == 0 && retrieved.m_id == 100001);
    test_result("Verify parent_name", strcmp(retrieved.parent_name, "Kim") == 0);
    test_result("Verify parent_email", strcmp(retrieved.parent_email, "parent@example.com") == 0);

    if (ret == 0) {
        printf("\nRetrieved confirm info:\n");
        UnderBillingConfirm_PrintInfo(&retrieved);
    }

    /* Test Update */
    retrieved.consent_yn = 2;
    strcpy(retrieved.parent_email, "newemail@example.com");

    ret = UnderBillingConfirm_Update(manager, &retrieved);
    test_result("UnderBillingConfirm_Update", ret == 0);

    /* Verify update */
    UnderBillingConfirm updated;
    memset(&updated, 0, sizeof(UnderBillingConfirm));
    ret = UnderBillingConfirm_Get(manager, 100001, &updated);
    test_result("Verify Update (consent_yn)", ret == 0 && updated.consent_yn == 2);
    test_result("Verify Update (parent_email)", strcmp(updated.parent_email, "newemail@example.com") == 0);

    /* Test Delete */
    ret = UnderBillingConfirm_Delete(manager, 100001);
    test_result("UnderBillingConfirm_Delete", ret == 0);

    /* Verify deletion */
    ret = UnderBillingConfirm_Exists(manager, 100001);
    test_result("UnderBillingConfirm_Exists (after delete)", ret == 0);
}

/* Test business operations */
void test_business_operations(DBConnectionManager* manager) {
    UnderBillingConfirm confirms[10];
    int actual_count;
    int ret;
    unsigned int now = (unsigned int)time(NULL);

    printf("\n=== Test Business Operations ===\n");

    /* Add test data */
    for (int i = 0; i < 3; i++) {
        UnderBillingConfirm confirm;
        memset(&confirm, 0, sizeof(UnderBillingConfirm));

        confirm.m_id = 100001 + i;
        strcpy(confirm.parent_name, "Kim");
        confirm.parent_jumin = 7012341234567ULL + i;
        confirm.parent_phone1 = 10;
        confirm.parent_phone2 = 1234 + i;
        confirm.parent_phone3 = 5678;
        strcpy(confirm.parent_email, i == 0 ? "test@example.com" : "other@example.com");
        confirm.parent_consent_type = 1;
        confirm.create_date = now + i * 100;
        confirm.consent_date = now + i * 100;
        confirm.consent_yn = (i % 2 == 0) ? 1 : 0;

        UnderBillingConfirm_Add(manager, &confirm);
    }

    /* Test GetAll */
    memset(confirms, 0, sizeof(confirms));
    ret = UnderBillingConfirm_GetAll(manager, confirms, 10, &actual_count);
    test_result("GetAll", ret == 0 && actual_count >= 3);
    printf("  Found %d records (ordered by create_date DESC)\n", actual_count);

    /* Test GetByParentName */
    memset(confirms, 0, sizeof(confirms));
    ret = UnderBillingConfirm_GetByParentName(manager, "Kim", confirms, 10, &actual_count);
    test_result("GetByParentName", ret == 0 && actual_count >= 3);
    printf("  Found %d records with parent_name 'Kim'\n", actual_count);

    /* Test GetByParentEmail */
    memset(confirms, 0, sizeof(confirms));
    ret = UnderBillingConfirm_GetByParentEmail(manager, "test@example.com", confirms, 10, &actual_count);
    test_result("GetByParentEmail", ret == 0 && actual_count >= 1);
    printf("  Found %d records with email 'test@example.com'\n", actual_count);

    /* Test GetByConsentStatus */
    memset(confirms, 0, sizeof(confirms));
    ret = UnderBillingConfirm_GetByConsentStatus(manager, 1, confirms, 10, &actual_count);
    test_result("GetByConsentStatus (approved)", ret == 0);
    printf("  Found %d approved consents\n", actual_count);

    /* Test GetByDateRange */
    memset(confirms, 0, sizeof(confirms));
    ret = UnderBillingConfirm_GetByDateRange(manager, now - 1000, now + 1000, confirms, 10, &actual_count);
    test_result("GetByDateRange", ret == 0 && actual_count >= 3);
    printf("  Found %d records in date range\n", actual_count);
}

/* Main test function */
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

    printf("=== UnderBillingConfirm ORM Test Suite ===\n");

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
    printf("Total: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}
