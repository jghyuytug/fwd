#include "charac_friends_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_TRUE(expr, msg) \
    do { \
        assertion_count++; \
        if (expr) { \
            printf("  ✓ %s [PASS]\n", msg); \
            assertion_passed++; \
        } else { \
            printf("  ✗ %s [FAIL]\n", msg); \
            current_test_failed = 1; \
        } \
    } while(0)

#define ASSERT_EQUAL(actual, expected, msg) \
    do { \
        assertion_count++; \
        if ((actual) == (expected)) { \
            printf("  ✓ %s [PASS]\n", msg); \
            assertion_passed++; \
        } else { \
            printf("  ✗ %s (expected: %d, got: %d) [FAIL]\n", msg, expected, actual); \
            current_test_failed = 1; \
        } \
    } while(0)

/* 测试辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    snprintf(query, sizeof(query), "DELETE FROM charac_friends WHERE charac_no >= 700000");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/* 测试1: Add操作 */
static void test_add(DBConnectionManager* manager) {
    TEST_START("CharacFriends_Add");
    cleanup_test_data(manager);

    CharacFriends record;
    record.charac_no = 700001;
    record.friend_no = 700002;

    int ret = CharacFriends_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully add a friendship");

    /* 验证添加成功 */
    int exists = CharacFriends_Exists(manager, 700001, 700002);
    ASSERT_EQUAL(exists, 1, "Friendship should exist after add");

    /* 测试幂等性 - 重复添加应该成功 */
    ret = CharacFriends_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully handle duplicate add (REPLACE)");

    cleanup_test_data(manager);
}

/* 测试2: Delete操作 */
static void test_delete(DBConnectionManager* manager) {
    TEST_START("CharacFriends_Delete");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacFriends record;
    record.charac_no = 700001;
    record.friend_no = 700002;
    CharacFriends_Add(manager, &record);

    /* 删除记录 */
    int ret = CharacFriends_Delete(manager, 700001, 700002);
    ASSERT_EQUAL(ret, 0, "Should successfully delete friendship");

    /* 验证删除成功 */
    int exists = CharacFriends_Exists(manager, 700001, 700002);
    ASSERT_EQUAL(exists, 0, "Friendship should not exist after delete");

    /* 删除不存在的记录应该成功（不报错） */
    ret = CharacFriends_Delete(manager, 700001, 700002);
    ASSERT_EQUAL(ret, 0, "Should return 0 even for non-existent friendship");

    cleanup_test_data(manager);
}

/* 测试3: Exists操作 */
static void test_exists(DBConnectionManager* manager) {
    TEST_START("CharacFriends_Exists");
    cleanup_test_data(manager);

    CharacFriends record;
    record.charac_no = 700001;
    record.friend_no = 700002;
    CharacFriends_Add(manager, &record);

    int exists = CharacFriends_Exists(manager, 700001, 700002);
    ASSERT_EQUAL(exists, 1, "Should return 1 for existing friendship");

    exists = CharacFriends_Exists(manager, 700001, 700003);
    ASSERT_EQUAL(exists, 0, "Should return 0 for non-existent friendship");

    cleanup_test_data(manager);
}

/* 测试4: GetFriendsByCharacNo操作 */
static void test_get_friends_by_charac_no(DBConnectionManager* manager) {
    TEST_START("CharacFriends_GetFriendsByCharacNo");
    cleanup_test_data(manager);

    /* 添加多个好友 */
    CharacFriends records[5];
    int i;
    for (i = 0; i < 5; i++) {
        records[i].charac_no = 700010;
        records[i].friend_no = 700020 + i;
        CharacFriends_Add(manager, &records[i]);
    }

    /* 查询好友列表 */
    unsigned int friend_nos[10];
    int count = CharacFriends_GetFriendsByCharacNo(manager, 700010, friend_nos, 10);
    ASSERT_EQUAL(count, 5, "Should find 5 friends for charac_no 700010");

    /* 验证好友编号是按序排列的 */
    ASSERT_EQUAL(friend_nos[0], 700020, "First friend should be 700020");
    ASSERT_EQUAL(friend_nos[4], 700024, "Last friend should be 700024");

    cleanup_test_data(manager);
}

/* 测试5: GetFriendsCount操作 */
static void test_get_friends_count(DBConnectionManager* manager) {
    TEST_START("CharacFriends_GetFriendsCount");
    cleanup_test_data(manager);

    /* 添加3个好友 */
    CharacFriends records[3];
    int i;
    for (i = 0; i < 3; i++) {
        records[i].charac_no = 700030;
        records[i].friend_no = 700040 + i;
        CharacFriends_Add(manager, &records[i]);
    }

    int count = CharacFriends_GetFriendsCount(manager, 700030);
    ASSERT_EQUAL(count, 3, "Should have 3 friends");

    /* 未添加好友的角色 */
    count = CharacFriends_GetFriendsCount(manager, 700099);
    ASSERT_EQUAL(count, 0, "Should have 0 friends for charac_no 700099");

    cleanup_test_data(manager);
}

/* 测试6: IsMutualFriend操作 */
static void test_is_mutual_friend(DBConnectionManager* manager) {
    TEST_START("CharacFriends_IsMutualFriend");
    cleanup_test_data(manager);

    /* 添加双向好友关系 */
    CharacFriends record1, record2;
    record1.charac_no = 700050;
    record1.friend_no = 700051;
    record2.charac_no = 700051;
    record2.friend_no = 700050;

    CharacFriends_Add(manager, &record1);
    CharacFriends_Add(manager, &record2);

    int is_mutual = CharacFriends_IsMutualFriend(manager, 700050, 700051);
    ASSERT_EQUAL(is_mutual, 1, "Should be mutual friends");

    /* 单向好友关系 */
    CharacFriends record3;
    record3.charac_no = 700060;
    record3.friend_no = 700061;
    CharacFriends_Add(manager, &record3);

    is_mutual = CharacFriends_IsMutualFriend(manager, 700060, 700061);
    ASSERT_EQUAL(is_mutual, 0, "Should not be mutual friends (one-way only)");

    cleanup_test_data(manager);
}

/* 测试7: BulkAdd操作 */
static void test_bulk_add(DBConnectionManager* manager) {
    TEST_START("CharacFriends_BulkAdd");
    cleanup_test_data(manager);

    CharacFriends records[3];
    records[0].charac_no = 700070;
    records[0].friend_no = 700071;
    records[1].charac_no = 700070;
    records[1].friend_no = 700072;
    records[2].charac_no = 700070;
    records[2].friend_no = 700073;

    int ret = CharacFriends_BulkAdd(manager, records, 3);
    ASSERT_EQUAL(ret, 3, "Should successfully add 3 friendships in bulk");

    /* 验证批量添加成功 */
    int count = CharacFriends_GetFriendsCount(manager, 700070);
    ASSERT_EQUAL(count, 3, "Should have 3 friends after bulk add");

    cleanup_test_data(manager);
}

/* 测试8: DeleteAllByCharacNo操作 */
static void test_delete_all_by_charac_no(DBConnectionManager* manager) {
    TEST_START("CharacFriends_DeleteAllByCharacNo");
    cleanup_test_data(manager);

    /* 添加多个好友 */
    CharacFriends records[4];
    int i;
    for (i = 0; i < 4; i++) {
        records[i].charac_no = 700080;
        records[i].friend_no = 700081 + i;
        CharacFriends_Add(manager, &records[i]);
    }

    /* 验证好友数量 */
    int count = CharacFriends_GetFriendsCount(manager, 700080);
    ASSERT_EQUAL(count, 4, "Should have 4 friends before delete");

    /* 删除所有好友 */
    int ret = CharacFriends_DeleteAllByCharacNo(manager, 700080);
    ASSERT_EQUAL(ret, 0, "Should successfully delete all friendships");

    /* 验证删除成功 */
    count = CharacFriends_GetFriendsCount(manager, 700080);
    ASSERT_EQUAL(count, 0, "Should have 0 friends after delete all");

    cleanup_test_data(manager);
}

/* 测试9: PrintInfo工具函数 */
static void test_print_info(DBConnectionManager* manager) {
    TEST_START("CharacFriends_PrintInfo");

    CharacFriends record;
    record.charac_no = 700090;
    record.friend_no = 700091;

    printf("  Testing PrintInfo output:\n");
    CharacFriends_PrintInfo(&record);
    ASSERT_TRUE(1, "PrintInfo should execute without errors");

    printf("  Testing PrintInfo with NULL:\n");
    CharacFriends_PrintInfo(NULL);
    ASSERT_TRUE(1, "PrintInfo should handle NULL gracefully");
}

/* 测试10: 集成测试 - 完整流程 */
static void test_integration(DBConnectionManager* manager) {
    TEST_START("Integration Test - Complete Workflow");
    cleanup_test_data(manager);

    /* 1. 添加好友 */
    CharacFriends record;
    record.charac_no = 700100;
    record.friend_no = 700101;
    CharacFriends_Add(manager, &record);

    /* 2. 验证存在 */
    int exists = CharacFriends_Exists(manager, 700100, 700101);
    ASSERT_EQUAL(exists, 1, "Integration: Friendship should exist after add");

    /* 3. 添加更多好友 */
    CharacFriends more_friends[2];
    more_friends[0].charac_no = 700100;
    more_friends[0].friend_no = 700102;
    more_friends[1].charac_no = 700100;
    more_friends[1].friend_no = 700103;
    CharacFriends_BulkAdd(manager, more_friends, 2);

    /* 4. 验证好友数量 */
    int count = CharacFriends_GetFriendsCount(manager, 700100);
    ASSERT_EQUAL(count, 3, "Integration: Should have 3 friends total");

    /* 5. 删除所有好友 */
    CharacFriends_DeleteAllByCharacNo(manager, 700100);
    count = CharacFriends_GetFriendsCount(manager, 700100);
    ASSERT_EQUAL(count, 0, "Integration: Should have 0 friends after delete all");

    cleanup_test_data(manager);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    printf("=======================================================\n");
    printf("  CharacFriends ORM 测试套件\n");
    printf("=======================================================\n");

    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        printf("[ERROR] Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        printf("[ERROR] Failed to connect to database\n");
        return 1;
    }

    printf("\n--- Running Basic CRUD Tests ---\n");
    test_add(&manager);
    test_delete(&manager);
    test_exists(&manager);

    printf("\n--- Running Query Tests ---\n");
    test_get_friends_by_charac_no(&manager);
    test_get_friends_count(&manager);
    test_is_mutual_friend(&manager);

    printf("\n--- Running Bulk Operation Tests ---\n");
    test_bulk_add(&manager);
    test_delete_all_by_charac_no(&manager);

    printf("\n--- Running Utility Function Tests ---\n");
    test_print_info(&manager);

    printf("\n--- Running Integration Test ---\n");
    test_integration(&manager);

    /* 统计最后一个测试 */
    if (!current_test_failed) {
        test_passed++;
    }

    printf("\n=======================================================\n");
    printf("  测试完成统计\n");
    printf("=======================================================\n");
    printf("  测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("  断言检查: %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("=======================================================\n");

    return (test_passed == test_count) ? 0 : 1;
}
