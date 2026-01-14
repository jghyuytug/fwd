#include "nexon_none_memner_restriction_orm.h"
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

    printf("=== NexonNoneMemnerRestriction ORM Test Suite ===\n\n");

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
        "DELETE FROM nexon_none_memner_restriction WHERE m_id IN (500001, 500002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    NexonNoneMemnerRestriction restriction;
    NexonNoneMemnerRestriction retrieved;

    printf("[TEST] Basic CRUD Operations\n");

    /* Add test */
    memset(&restriction, 0, sizeof(NexonNoneMemnerRestriction));
    restriction.m_id = 500001;
    restriction.charac_id = 1001;
    strcpy(restriction.last_trade_time, "2025-01-01 10:00:00");
    restriction.total_trade_gold = 1000000;
    restriction.trade_count = 10;
    restriction.nexon_user = 0;

    if (NexonNoneMemnerRestriction_Add(manager, &restriction) == 0) {
        printf("  [PASS] Add restriction record\n");
    } else {
        printf("  [FAIL] Add restriction record\n");
    }

    /* Exists test */
    if (NexonNoneMemnerRestriction_Exists(manager, 500001, 1001)) {
        printf("  [PASS] Restriction record exists\n");
    } else {
        printf("  [FAIL] Restriction record should exist\n");
    }

    /* Get test */
    memset(&retrieved, 0, sizeof(NexonNoneMemnerRestriction));
    if (NexonNoneMemnerRestriction_Get(manager, 500001, 1001, &retrieved) == 0) {
        printf("  [PASS] Get restriction record\n");
        if (retrieved.m_id == 500001 && retrieved.total_trade_gold == 1000000) {
            printf("  [PASS] Data verification\n");
        } else {
            printf("  [FAIL] Data mismatch\n");
        }
    } else {
        printf("  [FAIL] Get restriction record\n");
    }

    /* Update test */
    retrieved.total_trade_gold = 2000000;
    retrieved.nexon_user = 1;
    if (NexonNoneMemnerRestriction_Update(manager, &retrieved) == 0) {
        printf("  [PASS] Update restriction record\n");
    } else {
        printf("  [FAIL] Update restriction record\n");
    }

    /* Delete test */
    if (NexonNoneMemnerRestriction_Delete(manager, 500001, 1001) == 0) {
        printf("  [PASS] Delete restriction record\n");
    } else {
        printf("  [FAIL] Delete restriction record\n");
    }

    if (!NexonNoneMemnerRestriction_Exists(manager, 500001, 1001)) {
        printf("  [PASS] Verify deletion\n");
    } else {
        printf("  [FAIL] Record should not exist after deletion\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    NexonNoneMemnerRestriction restrictions[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Add multiple restriction records */
    NexonNoneMemnerRestriction r1 = {500002, 1001, "2025-01-01 10:00:00", 500000, 5, 0};
    NexonNoneMemnerRestriction r2 = {500002, 1002, "2025-01-01 11:00:00", 1500000, 15, 0};
    NexonNoneMemnerRestriction r3 = {500002, 1003, "2025-01-01 12:00:00", 3000000, 30, 1};

    NexonNoneMemnerRestriction_Add(manager, &r1);
    NexonNoneMemnerRestriction_Add(manager, &r2);
    NexonNoneMemnerRestriction_Add(manager, &r3);

    /* Test GetByMember */
    memset(restrictions, 0, sizeof(restrictions));
    if (NexonNoneMemnerRestriction_GetByMember(manager, 500002, restrictions, 10, &count) == 0) {
        printf("  [PASS] GetByMember (found %d records)\n", count);
        if (count == 3) {
            printf("  [PASS] Correct record count\n");
        } else {
            printf("  [FAIL] Expected 3 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByMember\n");
    }

    /* Test GetByNexonStatus */
    memset(restrictions, 0, sizeof(restrictions));
    if (NexonNoneMemnerRestriction_GetByNexonStatus(manager, 0, restrictions, 10, &count) == 0) {
        printf("  [PASS] GetByNexonStatus (found %d records)\n", count);
        if (count == 2) {
            printf("  [PASS] Correct status filter\n");
        } else {
            printf("  [FAIL] Expected 2 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByNexonStatus\n");
    }

    /* Test UpdateTradeInfo */
    if (NexonNoneMemnerRestriction_UpdateTradeInfo(manager, 500002, 1001, 100000) == 0) {
        printf("  [PASS] UpdateTradeInfo\n");

        NexonNoneMemnerRestriction rec;
        if (NexonNoneMemnerRestriction_Get(manager, 500002, 1001, &rec) == 0 &&
            rec.total_trade_gold == 600000 && rec.trade_count == 6) {
            printf("  [PASS] Verify trade info updated\n");
        } else {
            printf("  [FAIL] Trade info not updated correctly (gold=%u, count=%u)\n",
                   rec.total_trade_gold, rec.trade_count);
        }
    } else {
        printf("  [FAIL] UpdateTradeInfo\n");
    }

    /* Test GetHighTraders */
    memset(restrictions, 0, sizeof(restrictions));
    if (NexonNoneMemnerRestriction_GetHighTraders(manager, 1000000, restrictions, 10, &count) == 0) {
        printf("  [PASS] GetHighTraders (found %d records)\n", count);
        if (count >= 2) {
            printf("  [PASS] High traders found\n");
        }
    } else {
        printf("  [FAIL] GetHighTraders\n");
    }

    printf("\n");
}
