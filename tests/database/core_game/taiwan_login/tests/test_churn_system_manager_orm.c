#include "churn_system_manager_orm.h"
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
        "DELETE FROM churn_system_manager WHERE admin_id IN (99991, 99992)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    ChurnSystemManager config;
    ChurnSystemManager latest;
    int ret;

    printf("\n=== Test Basic CRUD (AUTO_INCREMENT) ===\n");

    memset(&config, 0, sizeof(ChurnSystemManager));
    config.weekday_var_a = 10;
    config.weekday_var_b = 20;
    config.weekday_var_c = 30;
    config.weekend_var_x = 15;
    config.weekend_var_y = 25;
    config.weekend_var_z = 35;
    config.next_reward_day = 7;
    config.admin_id = 99991;
    config.state_flag = 1;
    strcpy(config.reg_time, "");  /* Use NOW() */

    ret = ChurnSystemManager_Add(manager, &config);
    check_test("Add new record (AUTO_INCREMENT)", ret == 0);

    ret = ChurnSystemManager_GetLatest(manager, &latest);
    check_test("Get latest record", ret == 0);
    check_test("Verify admin_id", latest.admin_id == 99991);
    check_test("Verify weekday_var_a", latest.weekday_var_a == 10);
    check_test("Verify state_flag", latest.state_flag == 1);

    unsigned int no = latest.no;

    ret = ChurnSystemManager_Exists(manager, no);
    check_test("Record exists", ret == 1);

    ChurnSystemManager retrieved;
    ret = ChurnSystemManager_Get(manager, no, &retrieved);
    check_test("Get record by no", ret == 0 && retrieved.no == no);

    retrieved.weekday_var_a = 100;
    retrieved.state_flag = 0;
    ret = ChurnSystemManager_Update(manager, &retrieved);
    check_test("Update record", ret == 0);

    ChurnSystemManager updated;
    ret = ChurnSystemManager_Get(manager, no, &updated);
    check_test("Verify update - weekday_var_a", updated.weekday_var_a == 100);
    check_test("Verify update - state_flag", updated.state_flag == 0);

    ret = ChurnSystemManager_Delete(manager, no);
    check_test("Delete record", ret == 0);
    ret = ChurnSystemManager_Exists(manager, no);
    check_test("Verify deletion", ret == 0);
}

void test_business_operations(DBConnectionManager* manager) {
    ChurnSystemManager config;
    ChurnSystemManager results[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    memset(&config, 0, sizeof(ChurnSystemManager));
    config.admin_id = 99992;
    config.state_flag = 1;
    config.weekday_var_a = 50;
    strcpy(config.reg_time, "2025-01-15 10:00:00");
    ChurnSystemManager_Add(manager, &config);

    config.state_flag = 0;
    config.weekday_var_a = 60;
    strcpy(config.reg_time, "2025-01-16 11:00:00");
    ChurnSystemManager_Add(manager, &config);

    ret = ChurnSystemManager_GetActive(manager, results, 10, &actual_count);
    check_test("Get active configs", ret == 0);

    ret = ChurnSystemManager_GetByAdmin(manager, 99992, results, 10, &actual_count);
    check_test("Get by admin", ret == 0 && actual_count == 2);
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

    printf("=== Churn System Manager ORM Test Suite ===\n");

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
