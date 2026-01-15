#include "gm_manifest_notuse_orm.h"
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

    printf("=== GmManifestNotuse ORM Test Suite ===\n\n");

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

    /* Delete test records by m_id */
    snprintf(query, sizeof(query),
        "DELETE FROM gm_manifest_notuse WHERE m_id IN (100001, 100002, 100003, 200001, 200002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GmManifestNotuse gm1, gm2;
    int ret;

    printf("--- Test 1: Add GM Manifest Records ---\n");

    /* Prepare test data 1 */
    memset(&gm1, 0, sizeof(GmManifestNotuse));
    gm1.m_id = 100001;
    gm1.level = 5;

    ret = GmManifestNotuse_Add(manager, &gm1);
    printf("Add gm1 (m_id=100001, level=5): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 2 */
    memset(&gm2, 0, sizeof(GmManifestNotuse));
    gm2.m_id = 100002;
    gm2.level = 10;

    ret = GmManifestNotuse_Add(manager, &gm2);
    printf("Add gm2 (m_id=100002, level=10): %s\n", ret == 0 ? "OK" : "FAIL");

    printf("\n--- Test 2: Get GM Manifest by m_id ---\n");
    GmManifestNotuse retrieved_gm;

    ret = GmManifestNotuse_Get(manager, 100001, &retrieved_gm);
    printf("Get (m_id=100001): %s\n", ret == 0 ? "OK" : "FAIL");

    if (ret == 0) {
        GmManifestNotuse_PrintInfo(&retrieved_gm);
        if (retrieved_gm.m_id == 100001 && retrieved_gm.level == 5) {
            printf("Data verification: OK\n");
        } else {
            printf("Data verification: FAIL\n");
        }
    }

    printf("\n--- Test 3: Get All GM Manifests ---\n");
    GmManifestNotuse all_gms[100];
    int count = 0;

    ret = GmManifestNotuse_GetAll(manager, all_gms, 100, &count);
    printf("GetAll: %s, Found %d records\n", ret == 0 ? "OK" : "FAIL", count);

    if (count > 0) {
        printf("First record:\n");
        GmManifestNotuse_PrintInfo(&all_gms[0]);
    }

    printf("\n--- Test 4: Update GM Manifest ---\n");
    retrieved_gm.level = 15;

    ret = GmManifestNotuse_Update(manager, &retrieved_gm);
    printf("Update (m_id=%d, new level=%u): %s\n",
           retrieved_gm.m_id, retrieved_gm.level, ret == 0 ? "OK" : "FAIL");

    /* Verify update */
    GmManifestNotuse updated_gm;
    ret = GmManifestNotuse_Get(manager, 100001, &updated_gm);
    printf("Verify update: %s, level=%u\n",
           ret == 0 ? "OK" : "FAIL", updated_gm.level);

    printf("\n--- Test 5: Exists Check ---\n");
    int exists = GmManifestNotuse_Exists(manager, 100001);
    printf("Exists (m_id=100001): %s\n", exists ? "YES" : "NO");

    int not_exists = GmManifestNotuse_Exists(manager, 999999);
    printf("Exists (m_id=999999): %s\n", not_exists ? "YES" : "NO");

    printf("\n--- Test 6: Delete GM Manifest ---\n");
    ret = GmManifestNotuse_Delete(manager, 100001);
    printf("Delete (m_id=100001): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Verify deletion */
    exists = GmManifestNotuse_Exists(manager, 100001);
    printf("Verify deletion: %s\n", exists ? "FAIL (still exists)" : "OK (deleted)");
}

void test_business_operations(DBConnectionManager* manager) {
    GmManifestNotuse test_gms[3];
    int ret;

    printf("\n--- Test 7: Business Query - GetByLevel ---\n");

    /* Add test data with same level */
    memset(&test_gms[0], 0, sizeof(GmManifestNotuse));
    test_gms[0].m_id = 200001;
    test_gms[0].level = 20;
    GmManifestNotuse_Add(manager, &test_gms[0]);

    memset(&test_gms[1], 0, sizeof(GmManifestNotuse));
    test_gms[1].m_id = 200002;
    test_gms[1].level = 20;
    GmManifestNotuse_Add(manager, &test_gms[1]);

    memset(&test_gms[2], 0, sizeof(GmManifestNotuse));
    test_gms[2].m_id = 100003;
    test_gms[2].level = 25;
    GmManifestNotuse_Add(manager, &test_gms[2]);

    /* Query by level */
    GmManifestNotuse gms_by_level[10];
    int level_count = 0;

    ret = GmManifestNotuse_GetByLevel(manager, 20, gms_by_level, 10, &level_count);
    printf("GetByLevel (level=20): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", level_count);

    for (int i = 0; i < level_count; i++) {
        printf("  Record %d: m_id=%d, level=%u\n",
               i + 1, gms_by_level[i].m_id, gms_by_level[i].level);
    }

    printf("\n--- Test 8: Edge Cases ---\n");

    /* Test GetByLevel with non-existent level */
    int no_result_count = 0;
    ret = GmManifestNotuse_GetByLevel(manager, 99, gms_by_level, 10, &no_result_count);
    printf("GetByLevel (level=99, non-existent): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", no_result_count);
}
