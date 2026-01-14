#include "member_security_card_history_orm.h"
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

    printf("=== MemberSecurityCardHistory ORM Test Suite ===\n\n");

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
        "DELETE FROM member_security_card_history WHERE m_id IN (400001, 400002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberSecurityCardHistory history;
    MemberSecurityCardHistory retrieved;

    printf("[TEST] Basic CRUD Operations\n");

    /* Add test */
    memset(&history, 0, sizeof(MemberSecurityCardHistory));
    strcpy(history.occ_time, "2025-01-01 10:00:00");
    history.m_id = 400001;
    history.modify_type = 1;

    if (MemberSecurityCardHistory_Add(manager, &history) == 0) {
        printf("  [PASS] Add history record\n");
    } else {
        printf("  [FAIL] Add history record\n");
    }

    /* Exists test */
    if (MemberSecurityCardHistory_Exists(manager, "2025-01-01 10:00:00", 400001)) {
        printf("  [PASS] History record exists\n");
    } else {
        printf("  [FAIL] History record should exist\n");
    }

    /* Get test */
    memset(&retrieved, 0, sizeof(MemberSecurityCardHistory));
    if (MemberSecurityCardHistory_Get(manager, "2025-01-01 10:00:00", 400001, &retrieved) == 0) {
        printf("  [PASS] Get history record\n");
        if (retrieved.m_id == 400001 && retrieved.modify_type == 1) {
            printf("  [PASS] Data verification\n");
        } else {
            printf("  [FAIL] Data mismatch\n");
        }
    } else {
        printf("  [FAIL] Get history record\n");
    }

    /* Delete test */
    if (MemberSecurityCardHistory_Delete(manager, "2025-01-01 10:00:00", 400001) == 0) {
        printf("  [PASS] Delete history record\n");
    } else {
        printf("  [FAIL] Delete history record\n");
    }

    if (!MemberSecurityCardHistory_Exists(manager, "2025-01-01 10:00:00", 400001)) {
        printf("  [PASS] Verify deletion\n");
    } else {
        printf("  [FAIL] Record should not exist after deletion\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MemberSecurityCardHistory histories[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Add multiple history records */
    MemberSecurityCardHistory h1 = {"2025-01-01 10:00:00", 400002, 1};
    MemberSecurityCardHistory h2 = {"2025-01-01 11:00:00", 400002, 2};
    MemberSecurityCardHistory h3 = {"2025-01-01 12:00:00", 400002, 1};

    MemberSecurityCardHistory_Add(manager, &h1);
    MemberSecurityCardHistory_Add(manager, &h2);
    MemberSecurityCardHistory_Add(manager, &h3);

    /* Test GetByMember */
    memset(histories, 0, sizeof(histories));
    if (MemberSecurityCardHistory_GetByMember(manager, 400002, histories, 10, &count) == 0) {
        printf("  [PASS] GetByMember (found %d records)\n", count);
        if (count == 3) {
            printf("  [PASS] Correct record count\n");
        } else {
            printf("  [FAIL] Expected 3 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByMember\n");
    }

    /* Test GetByType */
    memset(histories, 0, sizeof(histories));
    if (MemberSecurityCardHistory_GetByType(manager, 1, histories, 10, &count) == 0) {
        printf("  [PASS] GetByType (found %d records)\n", count);
        if (count == 2) {
            printf("  [PASS] Correct type filter\n");
        } else {
            printf("  [FAIL] Expected 2 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByType\n");
    }

    /* Test GetRecent */
    memset(histories, 0, sizeof(histories));
    if (MemberSecurityCardHistory_GetRecent(manager, 400002, 2, histories, &count) == 0) {
        printf("  [PASS] GetRecent (found %d records)\n", count);
        if (count == 2) {
            printf("  [PASS] Correct limit applied\n");
        } else {
            printf("  [FAIL] Expected 2 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetRecent\n");
    }

    printf("\n");
}
