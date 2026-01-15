/**
 * CharacFriends ORM 测试程序
 */

#include "../src/database/charac_friends_orm.h"
#include "../src/database/db_connection_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 测试用角色和好友ID
#define TEST_CHARAC_1  100001
#define TEST_CHARAC_2  100002
#define TEST_CHARAC_3  100003
#define TEST_FRIEND_1  200001
#define TEST_FRIEND_2  200002
#define TEST_FRIEND_3  200003

// 测试计数器
static int tests_passed = 0;
static int tests_failed = 0;

// 测试辅助宏
#define TEST_ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("✓ Test %d: %s - PASSED\n", tests_passed + tests_failed + 1, test_name); \
            tests_passed++; \
        } else { \
            printf("✗ Test %d: %s - FAILED\n", tests_passed + tests_failed + 1, test_name); \
            tests_failed++; \
        } \
    } while(0)

// 清理测试数据
void cleanup_test_data(DBConnectionManager* manager) {
    char query[512];
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    // 删除所有测试数据
    snprintf(query, sizeof(query),
             "DELETE FROM charac_friends WHERE "
             "charac_no IN (%d, %d, %d) OR friend_no IN (%d, %d, %d)",
             TEST_CHARAC_1, TEST_CHARAC_2, TEST_CHARAC_3,
             TEST_FRIEND_1, TEST_FRIEND_2, TEST_FRIEND_3);

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}

// 测试1: 添加好友关系
void test_add_friend(DBConnectionManager* manager) {
    CharacFriend friend;
    CharacFriend_Init(&friend);
    friend.charac_no = TEST_CHARAC_1;
    friend.friend_no = TEST_FRIEND_1;

    int ret = CharacFriends_Add(manager, &friend);
    TEST_ASSERT(ret == 0, "Add Friend");
}

// 测试2: 查询好友关系
void test_is_friend(DBConnectionManager* manager) {
    bool is_friend = CharacFriends_IsFriend(manager, TEST_CHARAC_1, TEST_FRIEND_1);
    TEST_ASSERT(is_friend == true, "Check Friend Exists");
}

// 测试3: 获取好友数量
void test_get_count(DBConnectionManager* manager) {
    int count = CharacFriends_GetCount(manager, TEST_CHARAC_1);
    TEST_ASSERT(count == 1, "Get Friend Count");
}

// 测试4: 添加多个好友
void test_add_multiple_friends(DBConnectionManager* manager) {
    CharacFriend friend2, friend3;
    CharacFriend_Init(&friend2);
    CharacFriend_Init(&friend3);

    friend2.charac_no = TEST_CHARAC_1;
    friend2.friend_no = TEST_FRIEND_2;

    friend3.charac_no = TEST_CHARAC_1;
    friend3.friend_no = TEST_FRIEND_3;

    int ret1 = CharacFriends_Add(manager, &friend2);
    int ret2 = CharacFriends_Add(manager, &friend3);

    TEST_ASSERT(ret1 == 0 && ret2 == 0, "Add Multiple Friends");
}

// 测试5: 获取好友列表
void test_get_friend_list(DBConnectionManager* manager) {
    CharacFriend friends[10];
    int count = CharacFriends_GetList(manager, TEST_CHARAC_1, friends, 10);

    TEST_ASSERT(count == 3, "Get Friend List (should have 3 friends)");
}

// 测试6: 重复添加好友（幂等性测试）
void test_add_duplicate_friend(DBConnectionManager* manager) {
    CharacFriend friend;
    CharacFriend_Init(&friend);
    friend.charac_no = TEST_CHARAC_1;
    friend.friend_no = TEST_FRIEND_1;

    int ret = CharacFriends_Add(manager, &friend);  // 重复添加
    int count = CharacFriends_GetCount(manager, TEST_CHARAC_1);

    TEST_ASSERT(ret == 0 && count == 3, "Add Duplicate Friend (Idempotent)");
}

// 测试7: 删除好友关系
void test_remove_friend(DBConnectionManager* manager) {
    int ret = CharacFriends_Remove(manager, TEST_CHARAC_1, TEST_FRIEND_1);
    bool is_friend = CharacFriends_IsFriend(manager, TEST_CHARAC_1, TEST_FRIEND_1);
    int count = CharacFriends_GetCount(manager, TEST_CHARAC_1);

    TEST_ASSERT(ret == 0 && is_friend == false && count == 2, "Remove Friend");
}

// 测试8: 删除不存在的好友关系（幂等性测试）
void test_remove_nonexistent_friend(DBConnectionManager* manager) {
    int ret = CharacFriends_Remove(manager, TEST_CHARAC_1, 999999);
    TEST_ASSERT(ret == 0, "Remove Non-existent Friend (Idempotent)");
}

// 测试9: 双向好友关系
void test_mutual_friends(DBConnectionManager* manager) {
    CharacFriend friend1, friend2;
    CharacFriend_Init(&friend1);
    CharacFriend_Init(&friend2);

    // A加B为好友
    friend1.charac_no = TEST_CHARAC_2;
    friend1.friend_no = TEST_CHARAC_3;
    CharacFriends_Add(manager, &friend1);

    // B加A为好友
    friend2.charac_no = TEST_CHARAC_3;
    friend2.friend_no = TEST_CHARAC_2;
    CharacFriends_Add(manager, &friend2);

    bool is_mutual = CharacFriends_IsMutual(manager, TEST_CHARAC_2, TEST_CHARAC_3);
    TEST_ASSERT(is_mutual == true, "Check Mutual Friends");
}

// 测试10: 获取粉丝列表
void test_get_fans(DBConnectionManager* manager) {
    // TEST_CHARAC_1 有 TEST_FRIEND_2, TEST_FRIEND_3 为好友
    // TEST_CHARAC_2, TEST_CHARAC_3 互为好友
    // 所以 TEST_CHARAC_2 的粉丝应该有 TEST_CHARAC_3

    CharacFriend fans[10];
    int count = CharacFriends_GetFans(manager, TEST_CHARAC_2, fans, 10);

    TEST_ASSERT(count >= 1, "Get Fans List");
}

// 测试11: 批量添加好友
void test_add_batch(DBConnectionManager* manager) {
    CharacFriend batch[3];
    for (int i = 0; i < 3; i++) {
        CharacFriend_Init(&batch[i]);
        batch[i].charac_no = TEST_CHARAC_3;
        batch[i].friend_no = TEST_FRIEND_1 + i;
    }

    int success_count = CharacFriends_AddBatch(manager, batch, 3);
    int total_count = CharacFriends_GetCount(manager, TEST_CHARAC_3);

    // TEST_CHARAC_3 已经有 TEST_CHARAC_2 为好友，再加3个新好友
    TEST_ASSERT(success_count == 3 && total_count >= 3, "Add Batch Friends");
}

// 测试12: 删除所有好友
void test_remove_all_friends(DBConnectionManager* manager) {
    int ret = CharacFriends_RemoveAll(manager, TEST_CHARAC_1);
    int count = CharacFriends_GetCount(manager, TEST_CHARAC_1);

    TEST_ASSERT(ret == 0 && count == 0, "Remove All Friends");
}

// 测试13: 查询空好友列表
void test_get_empty_list(DBConnectionManager* manager) {
    CharacFriend friends[10];
    int count = CharacFriends_GetList(manager, TEST_CHARAC_1, friends, 10);

    TEST_ASSERT(count == 0, "Get Empty Friend List");
}

// 测试14: 检查不存在的好友关系
void test_check_nonexistent_friend(DBConnectionManager* manager) {
    bool is_friend = CharacFriends_IsFriend(manager, TEST_CHARAC_1, 999999);
    TEST_ASSERT(is_friend == false, "Check Non-existent Friend");
}

// 测试15: 单向好友关系（非互为好友）
void test_non_mutual_friends(DBConnectionManager* manager) {
    CharacFriend friend;
    CharacFriend_Init(&friend);
    friend.charac_no = TEST_CHARAC_1;
    friend.friend_no = TEST_CHARAC_2;
    CharacFriends_Add(manager, &friend);

    // TEST_CHARAC_1 加了 TEST_CHARAC_2 为好友
    // 但 TEST_CHARAC_2 没有加 TEST_CHARAC_1
    bool is_mutual = CharacFriends_IsMutual(manager, TEST_CHARAC_1, TEST_CHARAC_2);

    TEST_ASSERT(is_mutual == false, "Check Non-mutual Friends");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("================================================================================\n");
    printf("CharacFriends ORM 测试程序\n");
    printf("================================================================================\n\n");

    // 初始化数据库连接
    DBConnectionManager manager;
    if (DBConnectionManager_Init(&manager, argv[1]) != 0) {
        printf("❌ Failed to initialize database connection\n");
        return 1;
    }

    // 连接到CAIN数据库
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        printf("❌ Failed to connect to CAIN database\n");
        DBConnectionManager_Destroy(&manager);
        return 1;
    }

    printf("✓ Database connection established\n\n");

    // 清理之前的测试数据
    printf("Cleaning up old test data...\n");
    cleanup_test_data(&manager);

    // 运行测试
    printf("\nRunning tests...\n");
    printf("================================================================================\n\n");

    test_add_friend(&manager);                  // 测试1
    test_is_friend(&manager);                   // 测试2
    test_get_count(&manager);                   // 测试3
    test_add_multiple_friends(&manager);        // 测试4
    test_get_friend_list(&manager);             // 测试5
    test_add_duplicate_friend(&manager);        // 测试6
    test_remove_friend(&manager);               // 测试7
    test_remove_nonexistent_friend(&manager);   // 测试8
    test_mutual_friends(&manager);              // 测试9
    test_get_fans(&manager);                    // 测试10
    test_add_batch(&manager);                   // 测试11
    test_remove_all_friends(&manager);          // 测试12
    test_get_empty_list(&manager);              // 测试13
    test_check_nonexistent_friend(&manager);    // 测试14
    test_non_mutual_friends(&manager);          // 测试15

    // 最终清理
    printf("\n================================================================================\n");
    printf("Cleaning up test data...\n");
    cleanup_test_data(&manager);

    // 输出测试结果
    printf("\n================================================================================\n");
    printf("测试结果:\n");
    printf("================================================================================\n");
    printf("✓ 通过: %d\n", tests_passed);
    printf("✗ 失败: %d\n", tests_failed);
    printf("总计:   %d\n", tests_passed + tests_failed);
    printf("成功率: %.1f%%\n", tests_passed * 100.0 / (tests_passed + tests_failed));
    printf("================================================================================\n");

    // 清理数据库连接
    DBConnectionManager_Destroy(&manager);

    return (tests_failed == 0) ? 0 : 1;
}
