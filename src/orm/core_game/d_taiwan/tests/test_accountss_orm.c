#include "accountss_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

/* 测试宏 */
#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d == %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GT(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) > (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d <= %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GE(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) >= (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d < %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_STREQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
        } \
    } while(0)

/* 辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    /* 删除测试数据 */
    snprintf(query, sizeof(query),
        "DELETE FROM accountss WHERE accountname LIKE 'test_%%'");

    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

/* ==================== 测试用例 ==================== */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("测试添加和查询账户");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_001");
    snprintf(account.password, sizeof(account.password), "password123");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功，返回自增UID");

    Accountss retrieved;
    memset(&retrieved, 0, sizeof(Accountss));
    int ret = Accountss_Get(manager, new_uid, &retrieved);
    ASSERT_EQ(ret, 0, "查询账户成功");
    ASSERT_EQ(retrieved.uid, new_uid, "UID匹配");
    ASSERT_STREQ(retrieved.accountname, "test_user_001", "账户名匹配");
    ASSERT_STREQ(retrieved.password, "password123", "密码匹配");

    Accountss_PrintInfo(&retrieved);
}

void test_get_by_accountname(DBConnectionManager* manager) {
    TEST_START("测试按账户名查询");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_002");
    snprintf(account.password, sizeof(account.password), "pass456");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功");

    Accountss retrieved;
    memset(&retrieved, 0, sizeof(Accountss));
    int ret = Accountss_GetByAccountname(manager, "test_user_002", &retrieved);
    ASSERT_EQ(ret, 0, "按账户名查询成功");
    ASSERT_EQ(retrieved.uid, new_uid, "UID匹配");
    ASSERT_STREQ(retrieved.accountname, "test_user_002", "账户名匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("测试更新账户");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_003");
    snprintf(account.password, sizeof(account.password), "oldpass");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功");

    /* 查询并修改 */
    Accountss retrieved;
    memset(&retrieved, 0, sizeof(Accountss));
    Accountss_Get(manager, new_uid, &retrieved);

    snprintf(retrieved.accountname, sizeof(retrieved.accountname), "test_user_003_updated");
    snprintf(retrieved.password, sizeof(retrieved.password), "newpass");

    int ret = Accountss_Update(manager, &retrieved);
    ASSERT_EQ(ret, 0, "更新账户成功");

    /* 验证更新 */
    Accountss updated;
    memset(&updated, 0, sizeof(Accountss));
    Accountss_Get(manager, new_uid, &updated);
    ASSERT_STREQ(updated.accountname, "test_user_003_updated", "账户名已更新");
    ASSERT_STREQ(updated.password, "newpass", "密码已更新");
}

void test_update_password(DBConnectionManager* manager) {
    TEST_START("测试更新密码");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_004");
    snprintf(account.password, sizeof(account.password), "password1");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功");

    int ret = Accountss_UpdatePassword(manager, new_uid, "newpassword2");
    ASSERT_EQ(ret, 0, "更新密码成功");

    Accountss retrieved;
    memset(&retrieved, 0, sizeof(Accountss));
    Accountss_Get(manager, new_uid, &retrieved);
    ASSERT_STREQ(retrieved.password, "newpassword2", "密码已更新");
}

void test_verify_password(DBConnectionManager* manager) {
    TEST_START("测试验证密码");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_005");
    snprintf(account.password, sizeof(account.password), "secretpass");

    Accountss_Add(manager, &account);

    int verified = Accountss_VerifyPassword(manager, "test_user_005", "secretpass");
    ASSERT_EQ(verified, 1, "密码验证成功");

    verified = Accountss_VerifyPassword(manager, "test_user_005", "wrongpass");
    ASSERT_EQ(verified, 0, "错误密码验证失败");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("测试账户存在性检查");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_006");
    snprintf(account.password, sizeof(account.password), "pass");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功");

    int exists = Accountss_Exists(manager, new_uid);
    ASSERT_EQ(exists, 1, "已存在的UID返回1");

    exists = Accountss_Exists(manager, 999999);
    ASSERT_EQ(exists, 0, "不存在的UID返回0");
}

void test_exists_by_accountname(DBConnectionManager* manager) {
    TEST_START("测试按账户名检查存在性");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_007");
    snprintf(account.password, sizeof(account.password), "pass");

    Accountss_Add(manager, &account);

    int exists = Accountss_ExistsByAccountname(manager, "test_user_007");
    ASSERT_EQ(exists, 1, "已存在的账户名返回1");

    exists = Accountss_ExistsByAccountname(manager, "nonexistent_user");
    ASSERT_EQ(exists, 0, "不存在的账户名返回0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("测试删除账户");

    Accountss account;
    memset(&account, 0, sizeof(Accountss));
    snprintf(account.accountname, sizeof(account.accountname), "test_user_008");
    snprintf(account.password, sizeof(account.password), "pass");

    int new_uid = Accountss_Add(manager, &account);
    ASSERT_GT(new_uid, 0, "添加账户成功");

    int ret = Accountss_Delete(manager, new_uid);
    ASSERT_EQ(ret, 0, "删除账户成功");

    int exists = Accountss_Exists(manager, new_uid);
    ASSERT_EQ(exists, 0, "账户已不存在");
}

void test_count_total(DBConnectionManager* manager) {
    TEST_START("测试统计总账户数");

    int count = Accountss_CountTotal(manager);
    ASSERT_GE(count, 0, "总账户数>=0");

    printf("  当前总账户数: %d\n", count);
}

void test_count_by_pattern(DBConnectionManager* manager) {
    TEST_START("测试按模式统计");

    /* 添加几个测试账户 */
    Accountss account;
    for (int i = 0; i < 3; i++) {
        memset(&account, 0, sizeof(Accountss));
        snprintf(account.accountname, sizeof(account.accountname), "test_pattern_%d", i);
        snprintf(account.password, sizeof(account.password), "pass");
        Accountss_Add(manager, &account);
    }

    int count = Accountss_CountByPattern(manager, "test_pattern_%");
    ASSERT_GE(count, 3, "匹配模式的账户数>=3");

    printf("  匹配'test_pattern_%%'的账户数: %d\n", count);
}

void test_batch_delete(DBConnectionManager* manager) {
    TEST_START("测试批量删除");

    /* 添加测试账户 */
    int uids[3];
    Accountss account;
    for (int i = 0; i < 3; i++) {
        memset(&account, 0, sizeof(Accountss));
        snprintf(account.accountname, sizeof(account.accountname), "test_batch_%d", i);
        snprintf(account.password, sizeof(account.password), "pass");
        uids[i] = Accountss_Add(manager, &account);
        ASSERT_GT(uids[i], 0, "添加测试账户成功");
    }

    int ret = Accountss_BatchDelete(manager, uids, 3);
    ASSERT_EQ(ret, 0, "批量删除成功");

    /* 验证已删除 */
    for (int i = 0; i < 3; i++) {
        int exists = Accountss_Exists(manager, uids[i]);
        ASSERT_EQ(exists, 0, "批量删除的账户已不存在");
    }
}

void test_null_pointer_checks(DBConnectionManager* manager) {
    TEST_START("测试NULL指针检查");

    int ret = Accountss_Add(NULL, NULL);
    ASSERT_EQ(ret, -1, "manager=NULL时返回-1");

    Accountss account;
    ret = Accountss_Get(manager, 1, NULL);
    ASSERT_EQ(ret, -1, "account=NULL时返回-1");

    ret = Accountss_GetByAccountname(manager, NULL, &account);
    ASSERT_EQ(ret, -1, "accountname=NULL时返回-1");

    ret = Accountss_UpdatePassword(manager, 1, NULL);
    ASSERT_EQ(ret, -1, "password=NULL时返回-1");

    ret = Accountss_VerifyPassword(manager, NULL, "pass");
    ASSERT_EQ(ret, -1, "accountname=NULL时返回-1");
}

/* ==================== 主函数 ==================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        printf("用法: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("accountss_orm 测试套件\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("连接数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 运行测试 */
    test_add_and_get(&manager);
    test_get_by_accountname(&manager);
    test_update(&manager);
    test_update_password(&manager);
    test_verify_password(&manager);
    test_exists(&manager);
    test_exists_by_accountname(&manager);
    test_delete(&manager);
    test_count_total(&manager);
    test_count_by_pattern(&manager);
    test_batch_delete(&manager);
    test_null_pointer_checks(&manager);

    /* 最后一个测试完成时增加test_passed */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✅ 所有测试通过！\n");
        return 0;
    } else {
        printf("❌ 部分测试失败！\n");
        return 1;
    }
}
