#include "member_premium_old_orm.h"
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

    printf("=== MemberPremiumOld ORM Test Suite ===\n\n");

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
        "DELETE FROM member_premium_old WHERE m_id IN (200001, 200002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberPremiumOld premium;
    MemberPremiumOld retrieved;

    printf("[TEST] Basic CRUD Operations\n");

    /* Add test */
    memset(&premium, 0, sizeof(MemberPremiumOld));
    premium.event_id = 2001;
    premium.pre_type = 1;
    premium.m_id = 200001;
    strcpy(premium.service_start, "2024-01-01 00:00:00");
    strcpy(premium.service_end, "2024-12-31 23:59:59");
    premium.server_id = 20;

    if (MemberPremiumOld_Add(manager, &premium) == 0) {
        printf("  [PASS] Add premium service\n");
    } else {
        printf("  [FAIL] Add premium service\n");
    }

    /* Exists test */
    if (MemberPremiumOld_Exists(manager, 2001, 1, 20, 200001, "2024-01-01 00:00:00")) {
        printf("  [PASS] Premium service exists\n");
    } else {
        printf("  [FAIL] Premium service should exist\n");
    }

    /* Get test */
    memset(&retrieved, 0, sizeof(MemberPremiumOld));
    if (MemberPremiumOld_Get(manager, 2001, 1, 20, 200001, "2024-01-01 00:00:00", &retrieved) == 0) {
        printf("  [PASS] Get premium service\n");
        if (retrieved.event_id == 2001 && retrieved.m_id == 200001) {
            printf("  [PASS] Data verification\n");
        } else {
            printf("  [FAIL] Data mismatch\n");
        }
    } else {
        printf("  [FAIL] Get premium service\n");
    }

    /* Update test */
    strcpy(retrieved.service_end, "2025-12-31 23:59:59");
    if (MemberPremiumOld_Update(manager, &retrieved) == 0) {
        printf("  [PASS] Update premium service\n");
    } else {
        printf("  [FAIL] Update premium service\n");
    }

    /* Delete test */
    if (MemberPremiumOld_Delete(manager, 2001, 1, 20, 200001, "2024-01-01 00:00:00") == 0) {
        printf("  [PASS] Delete premium service\n");
    } else {
        printf("  [FAIL] Delete premium service\n");
    }

    if (!MemberPremiumOld_Exists(manager, 2001, 1, 20, 200001, "2024-01-01 00:00:00")) {
        printf("  [PASS] Verify deletion\n");
    } else {
        printf("  [FAIL] Record should not exist after deletion\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MemberPremiumOld premiums[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Add multiple premium services */
    MemberPremiumOld p1 = {2001, 1, 200002, "2024-01-01 00:00:00", "2024-06-30 23:59:59", 20};
    MemberPremiumOld p2 = {2001, 2, 200002, "2024-07-01 00:00:00", "2024-12-31 23:59:59", 20};
    MemberPremiumOld p3 = {2002, 1, 200002, "2024-01-01 00:00:00", "2024-12-31 23:59:59", 20};

    MemberPremiumOld_Add(manager, &p1);
    MemberPremiumOld_Add(manager, &p2);
    MemberPremiumOld_Add(manager, &p3);

    /* Test GetByMember */
    memset(premiums, 0, sizeof(premiums));
    if (MemberPremiumOld_GetByMember(manager, 200002, premiums, 10, &count) == 0) {
        printf("  [PASS] GetByMember (found %d records)\n", count);
        if (count == 3) {
            printf("  [PASS] Correct record count\n");
        } else {
            printf("  [FAIL] Expected 3 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByMember\n");
    }

    /* Test GetByEvent */
    memset(premiums, 0, sizeof(premiums));
    if (MemberPremiumOld_GetByEvent(manager, 2001, premiums, 10, &count) == 0) {
        printf("  [PASS] GetByEvent (found %d records)\n", count);
        if (count == 2) {
            printf("  [PASS] Correct event filter\n");
        } else {
            printf("  [FAIL] Expected 2 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByEvent\n");
    }

    printf("\n");
}
