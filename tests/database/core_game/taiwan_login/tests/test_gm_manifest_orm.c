#include "gm_manifest_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test function declarations */
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== GM Manifest ORM Test Suite ===\n\n");

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
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM gm_manifest WHERE m_id IN (100001, 100002, 100003, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GmManifest gm;
    GmManifest retrieved;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add */
    memset(&gm, 0, sizeof(GmManifest));
    gm.m_id = 100001;
    gm.level = 5;

    ret = GmManifest_Add(manager, &gm);
    printf("  Add GM (m_id=100001, level=5): %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 2: Exists */
    ret = GmManifest_Exists(manager, 100001);
    printf("  Exists check: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test 3: Get */
    memset(&retrieved, 0, sizeof(GmManifest));
    ret = GmManifest_Get(manager, 100001, &retrieved);
    printf("  Get GM: %s\n", ret == 0 ? "PASS" : "FAIL");

    if (ret == 0) {
        int match = (retrieved.m_id == 100001 && retrieved.level == 5);
        printf("  Verify data: %s (m_id=%d, level=%u)\n",
               match ? "PASS" : "FAIL", retrieved.m_id, retrieved.level);
    }

    /* Test 4: Update */
    retrieved.level = 10;
    ret = GmManifest_Update(manager, &retrieved);
    printf("  Update level to 10: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(GmManifest));
    ret = GmManifest_Get(manager, 100001, &retrieved);
    if (ret == 0) {
        int match = (retrieved.level == 10);
        printf("  Verify update: %s (level=%u)\n",
               match ? "PASS" : "FAIL", retrieved.level);
    }

    /* Test 6: Delete */
    ret = GmManifest_Delete(manager, 100001);
    printf("  Delete GM: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 7: Verify deletion */
    ret = GmManifest_Exists(manager, 100001);
    printf("  Verify deletion: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    GmManifest gms[10];
    int count;
    int ret;
    int i;

    printf("[TEST] Business Operations\n");

    /* Add multiple GMs for testing */
    for (i = 0; i < 3; i++) {
        GmManifest gm;
        memset(&gm, 0, sizeof(GmManifest));
        gm.m_id = 100001 + i;
        gm.level = (unsigned char)(5 + i * 5);
        GmManifest_Add(manager, &gm);
    }

    /* Test 1: GetAll */
    memset(gms, 0, sizeof(gms));
    ret = GmManifest_GetAll(manager, gms, 10, &count);
    printf("  GetAll: %s (found %d GMs)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 2: GetCount */
    ret = GmManifest_GetCount(manager, &count);
    printf("  GetCount: %s (count=%d)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 3: GetByLevel */
    memset(gms, 0, sizeof(gms));
    ret = GmManifest_GetByLevel(manager, 10, gms, 10, &count);
    printf("  GetByLevel (level=10): %s (found %d GMs)\n",
           ret == 0 && count == 1 ? "PASS" : "FAIL", count);

    if (count > 0) {
        printf("    GM m_id=%d, level=%u\n", gms[0].m_id, gms[0].level);
    }

    /* Test 4: PrintInfo */
    if (count > 0) {
        printf("  PrintInfo test:\n");
        GmManifest_PrintInfo(&gms[0]);
    }

    printf("\n");
}
