#include "churn_reward_history_201506_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_total = 0;
static int test_passed = 0;

void check_test(const char* test_name, int condition) {
    test_total++;
    if (condition) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        printf("[FAIL] %s\n", test_name);
    }
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM churn_reward_history_201506 WHERE m_id IN (100001, 100002)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    ChurnRewardHistory201506 history;
    int ret;

    printf("\n=== Test Basic CRUD (2-field PK) ===\n");

    memset(&history, 0, sizeof(ChurnRewardHistory201506));
    history.m_id = 100001;
    strcpy(history.occ_time, "2025-01-15 10:00:00");
    history.server_id = 1;
    history.charac_no = 12345;
    history.item_id = 5000;
    history.add_info = 100;
    history.luck_point = 50;
    history.reward_order = 1;
    history.cera = 200;

    ret = ChurnRewardHistory201506_Add(manager, &history);
    check_test("Add new record", ret == 0);

    ret = ChurnRewardHistory201506_Exists(manager, 100001, "2025-01-15 10:00:00");
    check_test("Record exists (2 keys)", ret == 1);

    ChurnRewardHistory201506 retrieved;
    ret = ChurnRewardHistory201506_Get(manager, 100001, "2025-01-15 10:00:00", &retrieved);
    check_test("Get record (2 keys)", ret == 0 && retrieved.m_id == 100001);
    check_test("Verify cera", retrieved.cera == 200);
    check_test("Verify luck_point", retrieved.luck_point == 50);

    ret = ChurnRewardHistory201506_Delete(manager, 100001, "2025-01-15 10:00:00");
    check_test("Delete record (2 keys)", ret == 0);

    ret = ChurnRewardHistory201506_Exists(manager, 100001, "2025-01-15 10:00:00");
    check_test("Verify deletion", ret == 0);
}

void test_business_operations(DBConnectionManager* manager) {
    ChurnRewardHistory201506 history;
    ChurnRewardHistory201506 results[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    memset(&history, 0, sizeof(ChurnRewardHistory201506));
    history.m_id = 100002;
    strcpy(history.occ_time, "2025-01-15 10:00:00");
    history.server_id = 1;
    history.cera = 100;
    ChurnRewardHistory201506_Add(manager, &history);

    strcpy(history.occ_time, "2025-01-16 11:00:00");
    history.cera = 200;
    ChurnRewardHistory201506_Add(manager, &history);

    ret = ChurnRewardHistory201506_GetByMember(manager, 100002, results, 10, &actual_count);
    check_test("Get by member", ret == 0 && actual_count == 2);

    ret = ChurnRewardHistory201506_GetByTimeRange(manager, "2025-01-15 00:00:00",
                                                    "2025-01-17 00:00:00",
                                                    results, 10, &actual_count);
    check_test("Get by time range", ret == 0 && actual_count >= 2);
}

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

    printf("=== Churn Reward History 201506 ORM Test Suite ===\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (100.0 * test_passed / test_total) : 0.0);

    return (test_total == test_passed) ? 0 : 1;
}
