#include "churn_member_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        "DELETE FROM churn_member_info WHERE m_id IN (100001, 100002, 100003)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    ChurnMemberInfo info;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("\n=== Test Basic CRUD ===\n");

    /* Add */
    memset(&info, 0, sizeof(ChurnMemberInfo));
    info.m_id = 100001;
    info.accrue_cera = 1000;
    strcpy(info.play_info, "test_play_info");
    info.first_reward_time = current_time;
    info.last_reward_time = current_time;
    info.server_id = 1;
    info.charac_no = 12345;
    info.item_id = 5000;
    info.add_info = 1;
    info.luck_point = 100;
    info.last_update_time = current_time;
    info.second_reward_time = 0;
    info.quest_time = 0;

    ret = ChurnMemberInfo_Add(manager, &info);
    check_test("Add new record", ret == 0);

    /* Exists */
    ret = ChurnMemberInfo_Exists(manager, 100001);
    check_test("Record exists", ret == 1);

    /* Get */
    ChurnMemberInfo retrieved;
    ret = ChurnMemberInfo_Get(manager, 100001, &retrieved);
    check_test("Get record", ret == 0 && retrieved.m_id == 100001);
    check_test("Verify accrue_cera", retrieved.accrue_cera == 1000);
    check_test("Verify luck_point", retrieved.luck_point == 100);

    /* Update */
    retrieved.accrue_cera = 2000;
    retrieved.luck_point = 200;
    ret = ChurnMemberInfo_Update(manager, &retrieved);
    check_test("Update record", ret == 0);

    /* Verify update */
    ChurnMemberInfo updated;
    ret = ChurnMemberInfo_Get(manager, 100001, &updated);
    check_test("Verify update - accrue_cera", updated.accrue_cera == 2000);
    check_test("Verify update - luck_point", updated.luck_point == 200);

    /* Delete */
    ret = ChurnMemberInfo_Delete(manager, 100001);
    check_test("Delete record", ret == 0);
    ret = ChurnMemberInfo_Exists(manager, 100001);
    check_test("Verify deletion", ret == 0);
}

void test_business_operations(DBConnectionManager* manager) {
    ChurnMemberInfo info;
    ChurnMemberInfo results[10];
    int actual_count;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("\n=== Test Business Operations ===\n");

    /* Add test data */
    memset(&info, 0, sizeof(ChurnMemberInfo));
    info.m_id = 100002;
    info.accrue_cera = 500;
    strcpy(info.play_info, "server1");
    info.server_id = 1;
    info.last_update_time = current_time;
    ChurnMemberInfo_Add(manager, &info);

    info.m_id = 100003;
    info.server_id = 2;
    info.last_update_time = current_time - 3600;
    ChurnMemberInfo_Add(manager, &info);

    /* Get by server */
    ret = ChurnMemberInfo_GetByServer(manager, 1, results, 10, &actual_count);
    check_test("Get by server", ret == 0 && actual_count >= 1);

    /* Get active members */
    ret = ChurnMemberInfo_GetActiveMembers(manager, current_time - 7200,
                                            results, 10, &actual_count);
    check_test("Get active members", ret == 0 && actual_count >= 2);

    /* Update reward time */
    ret = ChurnMemberInfo_UpdateRewardTime(manager, 100002, current_time + 100);
    check_test("Update reward time", ret == 0);
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

    printf("=== Churn Member Info ORM Test Suite ===\n");

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
