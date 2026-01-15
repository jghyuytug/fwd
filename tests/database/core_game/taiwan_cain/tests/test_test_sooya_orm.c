#include "test_sooya_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_crud(DBConnectionManager* manager);
void cleanup(DBConnectionManager* manager);

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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Test Sooya ORM Test Suite ===\n\n");

    cleanup(&manager);
    test_crud(&manager);
    cleanup(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void test_crud(DBConnectionManager* manager) {
    TestSooya record;

    printf("Test 1: Add record...\n");
    memset(&record, 0, sizeof(TestSooya));
    record.m_id = 100001;
    record.charac_no = 200001;
    record.lev = 60;

    if (TestSooya_Add(manager, &record) == 0) {
        printf("  PASS: Record added\n");
    } else {
        printf("  FAIL: Could not add record\n");
    }

    printf("Test 2: Get record...\n");
    memset(&record, 0, sizeof(TestSooya));
    if (TestSooya_Get(manager, 100001, 200001, &record) == 0) {
        printf("  PASS: Record retrieved\n");
        TestSooya_PrintInfo(&record);
    } else {
        printf("  FAIL: Could not get record\n");
    }

    printf("Test 3: Update record...\n");
    record.lev = 85;
    if (TestSooya_Update(manager, &record) == 0) {
        printf("  PASS: Record updated\n");
    } else {
        printf("  FAIL: Could not update record\n");
    }

    printf("Test 4: Exists check...\n");
    if (TestSooya_Exists(manager, 100001, 200001)) {
        printf("  PASS: Record exists\n");
    } else {
        printf("  FAIL: Record should exist\n");
    }

    printf("Test 5: Delete record...\n");
    if (TestSooya_Delete(manager, 100001, 200001) == 0) {
        printf("  PASS: Record deleted\n");
    } else {
        printf("  FAIL: Could not delete record\n");
    }
}

void cleanup(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM test_sooya WHERE m_id IN (100001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}
