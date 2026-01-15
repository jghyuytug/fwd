#include "churn_reward_manager_orm.h"
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
        "DELETE FROM churn_reward_manager WHERE quest_id IN (100, 101)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    ChurnRewardManager reward;
    int ret;

    printf("\n=== Test Basic CRUD (5-field PK) ===\n");

    memset(&reward, 0, sizeof(ChurnRewardManager));
    reward.min_day = 1;
    reward.max_day = 7;
    reward.min_val = 100;
    reward.max_val = 500;
    reward.quest_id = 100;
    reward.item_id = 5000;
    reward.add_info = 200;
    reward.luck_point = 50;

    ret = ChurnRewardManager_Add(manager, &reward);
    check_test("Add new record", ret == 0);

    ret = ChurnRewardManager_Exists(manager, 1, 7, 100, 500, 100);
    check_test("Record exists (5 keys)", ret == 1);

    ChurnRewardManager retrieved;
    ret = ChurnRewardManager_Get(manager, 1, 7, 100, 500, 100, &retrieved);
    check_test("Get record (5 keys)", ret == 0 && retrieved.quest_id == 100);
    check_test("Verify item_id", retrieved.item_id == 5000);
    check_test("Verify luck_point", retrieved.luck_point == 50);

    ret = ChurnRewardManager_Delete(manager, 1, 7, 100, 500, 100);
    check_test("Delete record (5 keys)", ret == 0);

    ret = ChurnRewardManager_Exists(manager, 1, 7, 100, 500, 100);
    check_test("Verify deletion", ret == 0);
}

void test_business_operations(DBConnectionManager* manager) {
    ChurnRewardManager reward;
    ChurnRewardManager results[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    memset(&reward, 0, sizeof(ChurnRewardManager));
    reward.min_day = 1;
    reward.max_day = 7;
    reward.min_val = 100;
    reward.max_val = 500;
    reward.quest_id = 101;
    reward.item_id = 5001;
    ChurnRewardManager_Add(manager, &reward);

    reward.min_val = 501;
    reward.max_val = 1000;
    reward.item_id = 5002;
    ChurnRewardManager_Add(manager, &reward);

    ret = ChurnRewardManager_GetByDayRange(manager, 1, 7, results, 10, &actual_count);
    check_test("Get by day range", ret == 0 && actual_count >= 2);

    ret = ChurnRewardManager_GetByQuest(manager, 101, results, 10, &actual_count);
    check_test("Get by quest", ret == 0 && actual_count == 2);

    ret = ChurnRewardManager_GetAll(manager, results, 10, &actual_count);
    check_test("Get all rewards", ret == 0 && actual_count >= 2);
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

    printf("=== Churn Reward Manager ORM Test Suite ===\n");

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
