#include "member_doubt_trade_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_total = 0;
static int test_passed = 0;

void test_basic_operations(DBConnectionManager* manager);
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

    printf("=== MemberDoubtTrade ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("\n=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_operations(DBConnectionManager* manager) {
    MemberDoubtTrade trade, retrieved;
    MemberDoubtTrade trades[10];
    int actual_count;

    printf("[TEST] Basic Operations\n");

    /* Test 1: Add record */
    test_total++;
    memset(&trade, 0, sizeof(MemberDoubtTrade));
    trade.m_id = 100001;
    trade.over_count = 0;

    if (MemberDoubtTrade_Add(manager, &trade) == 0) {
        printf("  [PASS] Add doubt trade record\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add doubt trade record\n");
    }

    /* Test 2: Check exists */
    test_total++;
    if (MemberDoubtTrade_Exists(manager, 100001) == 1) {
        printf("  [PASS] Record exists\n");
        test_passed++;
    } else {
        printf("  [FAIL] Record should exist\n");
    }

    /* Test 3: Get record */
    test_total++;
    memset(&retrieved, 0, sizeof(MemberDoubtTrade));

    if (MemberDoubtTrade_Get(manager, 100001, &retrieved) == 0) {
        if (retrieved.m_id == 100001 && retrieved.over_count == 0) {
            printf("  [PASS] Get record with correct data\n");
            test_passed++;
        } else {
            printf("  [FAIL] Get record with incorrect data\n");
        }
    } else {
        printf("  [FAIL] Get record\n");
    }

    /* Test 4: Increment count */
    test_total++;
    if (MemberDoubtTrade_IncrementCount(manager, 100001) == 0) {
        memset(&retrieved, 0, sizeof(MemberDoubtTrade));
        MemberDoubtTrade_Get(manager, 100001, &retrieved);
        if (retrieved.over_count == 1) {
            printf("  [PASS] Increment count (count=%u)\n", retrieved.over_count);
            test_passed++;
        } else {
            printf("  [FAIL] Increment count - expected 1, got %u\n", retrieved.over_count);
        }
    } else {
        printf("  [FAIL] Increment count\n");
    }

    /* Test 5: Update record */
    test_total++;
    trade.over_count = 5;
    strcpy(trade.last_update_time, "2025-11-19 10:00:00");

    if (MemberDoubtTrade_Update(manager, &trade) == 0) {
        memset(&retrieved, 0, sizeof(MemberDoubtTrade));
        MemberDoubtTrade_Get(manager, 100001, &retrieved);
        if (retrieved.over_count == 5) {
            printf("  [PASS] Update record (count=%u)\n", retrieved.over_count);
            test_passed++;
        } else {
            printf("  [FAIL] Update record - expected 5, got %u\n", retrieved.over_count);
        }
    } else {
        printf("  [FAIL] Update record\n");
    }

    /* Test 6: Add more records */
    test_total++;
    memset(&trade, 0, sizeof(MemberDoubtTrade));
    trade.m_id = 100002;
    trade.over_count = 3;
    MemberDoubtTrade_Add(manager, &trade);

    trade.m_id = 100003;
    trade.over_count = 10;

    if (MemberDoubtTrade_Add(manager, &trade) == 0) {
        printf("  [PASS] Add multiple records\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add multiple records\n");
    }

    /* Test 7: Get by minimum count */
    test_total++;
    memset(trades, 0, sizeof(trades));

    if (MemberDoubtTrade_GetByMinCount(manager, 5, trades, 10, &actual_count) == 0) {
        if (actual_count == 2) {
            printf("  [PASS] Get by min count (count=%d)\n", actual_count);
            test_passed++;
        } else {
            printf("  [FAIL] Get by min count - expected 2, got %d\n", actual_count);
        }
    } else {
        printf("  [FAIL] Get by min count\n");
    }

    /* Test 8: Delete record */
    test_total++;
    if (MemberDoubtTrade_Delete(manager, 100001) == 0) {
        if (MemberDoubtTrade_Exists(manager, 100001) == 0) {
            printf("  [PASS] Delete record\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete record - still exists\n");
        }
    } else {
        printf("  [FAIL] Delete record\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    MemberDoubtTrade_Delete(manager, 100001);
    MemberDoubtTrade_Delete(manager, 100002);
    MemberDoubtTrade_Delete(manager, 100003);
}
