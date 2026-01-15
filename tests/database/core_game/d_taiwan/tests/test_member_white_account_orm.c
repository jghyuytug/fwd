/**
 * DNF游戏服务器 - MemberWhiteAccount表ORM测试程序
 * 测试白名单账号管理系统的15个功能点
 */

#include "member_white_account_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define TEST_MEMBER_1 300001
#define TEST_MEMBER_2 300002
#define TEST_MEMBER_3 300003

int g_test_passed = 0;
int g_test_failed = 0;

#define TEST_ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("[测试通过] %s\n", test_name); \
            g_test_passed++; \
        } else { \
            printf("[测试失败] %s\n", test_name); \
            g_test_failed++; \
        } \
    } while(0)

/* ========================================================================
 * 辅助函数
 * ======================================================================== */

void get_current_datetime(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/* ========================================================================
 * 测试用例
 * ======================================================================== */

void test_add_whitelist(DBConnectionManager* manager) {
    MemberWhiteAccount account;
    MemberWhiteAccount_Init(&account);

    account.m_id = TEST_MEMBER_1;
    get_current_datetime(account.reg_date, sizeof(account.reg_date));

    int ret = MemberWhiteAccount_Add(manager, &account);
    TEST_ASSERT(ret == 0, "测试1: 添加白名单账号");
}

void test_get_whitelist(DBConnectionManager* manager) {
    MemberWhiteAccount account;
    int ret = MemberWhiteAccount_Get(manager, TEST_MEMBER_1, &account);
    TEST_ASSERT(ret == 0 && account.m_id == TEST_MEMBER_1, "测试2: 获取白名单记录");
}

void test_exists(DBConnectionManager* manager) {
    bool exists = MemberWhiteAccount_Exists(manager, TEST_MEMBER_1);
    TEST_ASSERT(exists == true, "测试3: 检查账号是否在白名单中");
}

void test_is_whitelisted(DBConnectionManager* manager) {
    bool whitelisted = MemberWhiteAccount_IsWhitelisted(manager, TEST_MEMBER_1);
    TEST_ASSERT(whitelisted == true, "测试4: 快速检查白名单状态（业务函数）");
}

void test_add_multiple(DBConnectionManager* manager) {
    MemberWhiteAccount account2;
    MemberWhiteAccount_Init(&account2);

    account2.m_id = TEST_MEMBER_2;
    get_current_datetime(account2.reg_date, sizeof(account2.reg_date));

    int ret = MemberWhiteAccount_Add(manager, &account2);
    TEST_ASSERT(ret == 0, "测试5: 添加第二个白名单账号");
}

void test_add_duplicate(DBConnectionManager* manager) {
    MemberWhiteAccount account;
    MemberWhiteAccount_Init(&account);

    account.m_id = TEST_MEMBER_1;
    get_current_datetime(account.reg_date, sizeof(account.reg_date));

    int ret = MemberWhiteAccount_Add(manager, &account);
    TEST_ASSERT(ret == 0, "测试6: 添加重复记录（幂等性-REPLACE INTO）");
}

void test_get_all(DBConnectionManager* manager) {
    MemberWhiteAccount accounts[10];
    int count = MemberWhiteAccount_GetAll(manager, accounts, 0, 10);
    TEST_ASSERT(count >= 2, "测试7: 获取所有白名单账号（分页）");
}

void test_get_count(DBConnectionManager* manager) {
    int count = MemberWhiteAccount_GetCount(manager);
    TEST_ASSERT(count >= 2, "测试8: 获取白名单总数");
}

void test_get_recent_added(DBConnectionManager* manager) {
    MemberWhiteAccount accounts[5];
    int count = MemberWhiteAccount_GetRecentAdded(manager, accounts, 5);
    TEST_ASSERT(count >= 2, "测试9: 获取最近添加的白名单账号");
}

void test_add_batch(DBConnectionManager* manager) {
    MemberWhiteAccount accounts[3];
    int i;

    for (i = 0; i < 3; i++) {
        MemberWhiteAccount_Init(&accounts[i]);
        accounts[i].m_id = 300010 + i;
        get_current_datetime(accounts[i].reg_date, sizeof(accounts[i].reg_date));
    }

    int success = MemberWhiteAccount_AddBatch(manager, accounts, 3);
    TEST_ASSERT(success == 3, "测试10: 批量添加白名单账号");
}

void test_get_by_date_range(DBConnectionManager* manager) {
    MemberWhiteAccount accounts[10];
    char start_date[20] = "2020-01-01 00:00:00";
    char end_date[20] = "2030-12-31 23:59:59";

    int count = MemberWhiteAccount_GetByDateRange(manager, start_date, end_date, accounts, 10);
    TEST_ASSERT(count >= 3, "测试11: 按日期范围查询白名单");
}

void test_get_old_records(DBConnectionManager* manager) {
    /* 添加一个旧记录用于测试 */
    MemberWhiteAccount old_account;
    MemberWhiteAccount_Init(&old_account);
    old_account.m_id = 300099;
    strcpy(old_account.reg_date, "2020-01-01 00:00:00");
    MemberWhiteAccount_Add(manager, &old_account);

    MemberWhiteAccount accounts[10];
    int count = MemberWhiteAccount_GetOldRecords(manager, 365, accounts, 10);
    TEST_ASSERT(count >= 1, "测试12: 获取过期/长期未活动的白名单");
}

void test_delete_whitelist(DBConnectionManager* manager) {
    int ret = MemberWhiteAccount_Delete(manager, TEST_MEMBER_2);
    TEST_ASSERT(ret == 0, "测试13: 删除白名单账号");
}

void test_delete_nonexistent(DBConnectionManager* manager) {
    int ret = MemberWhiteAccount_Delete(manager, TEST_MEMBER_2);
    TEST_ASSERT(ret == 0, "测试14: 删除不存在的记录（幂等性）");
}

void test_utility_functions(DBConnectionManager* manager) {
    MemberWhiteAccount account;
    MemberWhiteAccount_Init(&account);

    account.m_id = 300100;
    get_current_datetime(account.reg_date, sizeof(account.reg_date));

    printf("\n[工具函数测试]:\n");
    MemberWhiteAccount_Print(&account);

    TEST_ASSERT(account.m_id == 300100, "测试15: 工具函数");
}

/* ========================================================================
 * 测试清理函数
 * ======================================================================== */

void cleanup_test_data(DBConnectionManager* manager) {
    printf("\n[开始清理测试数据]\n");
    MemberWhiteAccount_Delete(manager, TEST_MEMBER_1);
    MemberWhiteAccount_Delete(manager, TEST_MEMBER_2);
    MemberWhiteAccount_Delete(manager, TEST_MEMBER_3);
    MemberWhiteAccount_Delete(manager, 300010);
    MemberWhiteAccount_Delete(manager, 300011);
    MemberWhiteAccount_Delete(manager, 300012);
    MemberWhiteAccount_Delete(manager, 300099);
    MemberWhiteAccount_Delete(manager, 300100);
    printf("[测试数据清理完成]\n\n");
}

/* ========================================================================
 * 主测试函数
 * ======================================================================== */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("MemberWhiteAccount ORM 测试程序\n");
    printf("========================================\n\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        printf("[错误] 无法初始化数据库连接管理器\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        printf("[错误] 无法连接到数据库\n");
        return 1;
    }

    printf("[信息] 数据库连接成功\n\n");

    cleanup_test_data(&manager);

    printf("========================================\n");
    printf("开始执行测试用例\n");
    printf("========================================\n\n");

    test_add_whitelist(&manager);
    test_get_whitelist(&manager);
    test_exists(&manager);
    test_is_whitelisted(&manager);
    test_add_multiple(&manager);
    test_add_duplicate(&manager);
    test_get_all(&manager);
    test_get_count(&manager);
    test_get_recent_added(&manager);
    test_add_batch(&manager);
    test_get_by_date_range(&manager);
    test_get_old_records(&manager);
    test_delete_whitelist(&manager);
    test_delete_nonexistent(&manager);
    test_utility_functions(&manager);

    cleanup_test_data(&manager);

    printf("\n========================================\n");
    printf("测试结果汇总\n");
    printf("========================================\n");
    printf("通过: %d\n", g_test_passed);
    printf("失败: %d\n", g_test_failed);
    printf("总计: %d\n", g_test_passed + g_test_failed);
    printf("成功率: %.1f%%\n", (float)g_test_passed / (g_test_passed + g_test_failed) * 100);
    printf("========================================\n");

    return (g_test_failed == 0) ? 0 : 1;
}
