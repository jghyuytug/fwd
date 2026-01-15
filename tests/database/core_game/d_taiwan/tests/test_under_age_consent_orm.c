#include "under_age_consent_orm.h"
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
        "DELETE FROM under_age_consent WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

/* Test basic CRUD operations */
void test_basic_crud(DBConnectionManager* manager) {
    UnderAgeConsent consent;
    int ret;

    printf("\n=== Test Basic CRUD Operations ===\n");

    /* Test Add */
    memset(&consent, 0, sizeof(UnderAgeConsent));
    consent.m_id = 100001;
    consent.consent_type = 1;
    consent.limit_money = 50000;
    strcpy(consent.parent_name, "Kim");
    consent.parent_jumin = 7012341234567ULL;
    consent.parent_phone1 = 10;
    consent.parent_phone2 = 1234;
    consent.parent_phone3 = 5678;
    strcpy(consent.parent_email, "parent@example.com");
    consent.parent_consent_type = 1;
    consent.notice_type = 1;
    strcpy(consent.notice_addr, "Seoul, Korea");
    consent.create_date = (unsigned int)time(NULL);
    consent.consent_date = (unsigned int)time(NULL);
    consent.consent_yn = 1;
    consent.history_yn = 0;

    ret = UnderAgeConsent_Add(manager, &consent);
    test_result("UnderAgeConsent_Add", ret == 0);

    /* Test Exists */
    ret = UnderAgeConsent_Exists(manager, 100001, consent.create_date);
    test_result("UnderAgeConsent_Exists (should exist)", ret == 1);

    /* Test Get */
    UnderAgeConsent retrieved;
    memset(&retrieved, 0, sizeof(UnderAgeConsent));
    ret = UnderAgeConsent_Get(manager, 100001, consent.create_date, &retrieved);
    test_result("UnderAgeConsent_Get", ret == 0 && retrieved.m_id == 100001);

    if (ret == 0) {
        printf("\nRetrieved consent info:\n");
        UnderAgeConsent_PrintInfo(&retrieved);
    }

    /* Test Update */
    retrieved.limit_money = 100000;
    retrieved.consent_yn = 1;
    strcpy(retrieved.notice_addr, "Busan, Korea");

    ret = UnderAgeConsent_Update(manager, &retrieved);
    test_result("UnderAgeConsent_Update", ret == 0);

    /* Verify update */
    UnderAgeConsent updated;
    memset(&updated, 0, sizeof(UnderAgeConsent));
    ret = UnderAgeConsent_Get(manager, 100001, consent.create_date, &updated);
    test_result("Verify Update (limit_money)", ret == 0 && updated.limit_money == 100000);
    test_result("Verify Update (notice_addr)", strcmp(updated.notice_addr, "Busan, Korea") == 0);

    /* Test Delete */
    ret = UnderAgeConsent_Delete(manager, 100001, consent.create_date);
    test_result("UnderAgeConsent_Delete", ret == 0);

    /* Verify deletion */
    ret = UnderAgeConsent_Exists(manager, 100001, consent.create_date);
    test_result("UnderAgeConsent_Exists (after delete)", ret == 0);
}

/* Test business operations */
void test_business_operations(DBConnectionManager* manager) {
    UnderAgeConsent consents[10];
    int actual_count;
    int ret;
    unsigned int now = (unsigned int)time(NULL);

    printf("\n=== Test Business Operations ===\n");

    /* Add test data */
    for (int i = 0; i < 3; i++) {
        UnderAgeConsent consent;
        memset(&consent, 0, sizeof(UnderAgeConsent));

        consent.m_id = 100001 + i;
        consent.consent_type = 1;
        consent.limit_money = 50000 + (i * 10000);
        strcpy(consent.parent_name, "Kim");
        consent.parent_jumin = 7012341234567ULL + i;
        consent.parent_phone1 = 10;
        consent.parent_phone2 = 1234 + i;
        consent.parent_phone3 = 5678;
        strcpy(consent.parent_email, i == 0 ? "test@example.com" : "other@example.com");
        consent.parent_consent_type = 1;
        consent.notice_type = 1;
        strcpy(consent.notice_addr, "Test Address");
        consent.create_date = now + i;
        consent.consent_date = now + i;
        consent.consent_yn = (i % 2 == 0) ? 1 : 0;
        consent.history_yn = 0;

        UnderAgeConsent_Add(manager, &consent);
    }

    /* Test GetByMemberId */
    memset(consents, 0, sizeof(consents));
    ret = UnderAgeConsent_GetByMemberId(manager, 100001, consents, 10, &actual_count);
    test_result("GetByMemberId", ret == 0 && actual_count >= 1);
    printf("  Found %d records for member 100001\n", actual_count);

    /* Test GetByParentName */
    memset(consents, 0, sizeof(consents));
    ret = UnderAgeConsent_GetByParentName(manager, "Kim", consents, 10, &actual_count);
    test_result("GetByParentName", ret == 0 && actual_count >= 3);
    printf("  Found %d records with parent name 'Kim'\n", actual_count);

    /* Test GetByParentEmail */
    memset(consents, 0, sizeof(consents));
    ret = UnderAgeConsent_GetByParentEmail(manager, "test@example.com", consents, 10, &actual_count);
    test_result("GetByParentEmail", ret == 0 && actual_count >= 1);
    printf("  Found %d records with email 'test@example.com'\n", actual_count);

    /* Test GetByConsentStatus */
    memset(consents, 0, sizeof(consents));
    ret = UnderAgeConsent_GetByConsentStatus(manager, 1, consents, 10, &actual_count);
    test_result("GetByConsentStatus (approved)", ret == 0);
    printf("  Found %d approved consents\n", actual_count);

    /* Test GetAll */
    memset(consents, 0, sizeof(consents));
    ret = UnderAgeConsent_GetAll(manager, consents, 10, &actual_count);
    test_result("GetAll", ret == 0 && actual_count >= 3);
    printf("  Total records: %d\n", actual_count);
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

    printf("=== UnderAgeConsent ORM Test Suite ===\n");

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
