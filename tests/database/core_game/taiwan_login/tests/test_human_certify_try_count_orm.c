#include "human_certify_try_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Human Certify Try Count ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM human_certify_try_count WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    HumanCertifyTryCount cert;
    HumanCertifyTryCount retrieved;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add */
    memset(&cert, 0, sizeof(HumanCertifyTryCount));
    cert.m_id = 100001;
    cert.count = 5;

    ret = HumanCertifyTryCount_Add(manager, &cert);
    printf("  Add record (m_id=100001, count=5): %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 2: Exists */
    ret = HumanCertifyTryCount_Exists(manager, 100001);
    printf("  Exists check: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test 3: Get */
    memset(&retrieved, 0, sizeof(HumanCertifyTryCount));
    ret = HumanCertifyTryCount_Get(manager, 100001, &retrieved);
    printf("  Get record: %s\n", ret == 0 ? "PASS" : "FAIL");

    if (ret == 0) {
        int match = (retrieved.m_id == 100001 && retrieved.count == 5);
        printf("  Verify data: %s (m_id=%d, count=%u)\n",
               match ? "PASS" : "FAIL", retrieved.m_id, retrieved.count);
    }

    /* Test 4: Update */
    retrieved.count = 10;
    ret = HumanCertifyTryCount_Update(manager, &retrieved);
    printf("  Update count to 10: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(HumanCertifyTryCount));
    ret = HumanCertifyTryCount_Get(manager, 100001, &retrieved);
    if (ret == 0) {
        int match = (retrieved.count == 10);
        printf("  Verify update: %s (count=%u)\n",
               match ? "PASS" : "FAIL", retrieved.count);
    }

    /* Test 6: Delete */
    ret = HumanCertifyTryCount_Delete(manager, 100001);
    printf("  Delete record: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 7: Verify deletion */
    ret = HumanCertifyTryCount_Exists(manager, 100001);
    printf("  Verify deletion: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    HumanCertifyTryCount cert;
    HumanCertifyTryCount certs[10];
    int count;
    int ret;
    int i;

    printf("[TEST] Business Operations\n");

    /* Test 1: Increment operation */
    memset(&cert, 0, sizeof(HumanCertifyTryCount));
    cert.m_id = 100001;
    cert.count = 0;
    HumanCertifyTryCount_Add(manager, &cert);

    for (i = 0; i < 5; i++) {
        HumanCertifyTryCount_Increment(manager, 100001);
    }

    memset(&cert, 0, sizeof(HumanCertifyTryCount));
    ret = HumanCertifyTryCount_Get(manager, 100001, &cert);
    printf("  Increment 5 times: %s (count=%u)\n",
           ret == 0 && cert.count == 5 ? "PASS" : "FAIL", cert.count);

    /* Test 2: Reset operation */
    ret = HumanCertifyTryCount_Reset(manager, 100001);
    printf("  Reset count: %s\n", ret == 0 ? "PASS" : "FAIL");

    memset(&cert, 0, sizeof(HumanCertifyTryCount));
    ret = HumanCertifyTryCount_Get(manager, 100001, &cert);
    printf("  Verify reset: %s (count=%u)\n",
           ret == 0 && cert.count == 0 ? "PASS" : "FAIL", cert.count);

    /* Test 3: GetAll */
    for (i = 0; i < 3; i++) {
        HumanCertifyTryCount c;
        memset(&c, 0, sizeof(HumanCertifyTryCount));
        c.m_id = 100001 + i;
        c.count = (unsigned int)(i * 2);
        HumanCertifyTryCount_Add(manager, &c);
    }

    memset(certs, 0, sizeof(certs));
    ret = HumanCertifyTryCount_GetAll(manager, certs, 10, &count);
    printf("  GetAll: %s (found %d records)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 4: PrintInfo */
    if (count > 0) {
        printf("  PrintInfo test:\n");
        HumanCertifyTryCount_PrintInfo(&certs[0]);
    }

    printf("\n");
}
