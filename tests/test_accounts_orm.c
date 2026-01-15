/**
 * DNF游戏服务器 - Accounts表ORM测试程序
 *
 * 功能:
 * 1. 测试账号CRUD操作
 * 2. 测试账号验证
 * 3. 测试查询功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "accounts_orm.h"

// ANSI颜色代码
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_separator() {
    printf("%s================================================================================\n%s",
           COLOR_CYAN, COLOR_RESET);
}

void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

void print_warning(const char* msg) {
    printf("%s⚠ %s%s\n", COLOR_YELLOW, msg, COLOR_RESET);
}

// 测试创建账号
int test_create_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;
    Account_Init(&account);

    // 创建测试账号1
    strcpy(account.accountname, "testuser1");
    strcpy(account.password, "5f4dcc3b5aa765d61d8327deb882cf99");  // MD5 of "password"
    strcpy(account.qq, "123456789");
    account.dzuid = 0;
    account.billing = 0;
    strcpy(account.VIP, "");

    print_info("创建账号: testuser1");
    int uid = Account_Create(manager, &account);
    if (uid > 0) {
        print_success("账号创建成功");
        printf("  分配的UID: %d\n", uid);
        Account_Print(&account);
    } else {
        print_error("账号创建失败");
        return -1;
    }

    // 创建测试账号2
    Account_Init(&account);
    strcpy(account.accountname, "testuser2");
    strcpy(account.password, "e10adc3949ba59abbe56e057f20f883e");  // MD5 of "123456"
    strcpy(account.qq, "987654321");
    account.billing = 1;  // 付费用户
    strcpy(account.VIP, "VIP1");

    print_info("创建账号: testuser2");
    uid = Account_Create(manager, &account);
    if (uid > 0) {
        print_success("账号创建成功");
        printf("  分配的UID: %d\n", uid);
    } else {
        print_error("账号创建失败");
        return -1;
    }

    // 创建测试账号3 (无QQ)
    Account_Init(&account);
    strcpy(account.accountname, "testuser3");
    strcpy(account.password, "202cb962ac59075b964b07152d234b70");  // MD5 of "123"
    account.qq[0] = '\0';  // 无QQ
    account.billing = 0;

    print_info("创建账号: testuser3 (无QQ绑定)");
    uid = Account_Create(manager, &account);
    if (uid > 0) {
        print_success("账号创建成功");
        printf("  分配的UID: %d\n", uid);
    } else {
        print_error("账号创建失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试查询账号
int test_query_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    // 按账号名查询
    print_info("按账号名查询: testuser1");
    if (Account_GetByName(manager, "testuser1", &account) == 0) {
        print_success("查询成功");
        Account_Print(&account);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 按UID查询
    int uid = account.UID;
    print_info("按UID查询");
    printf("  UID: %d\n", uid);

    if (Account_GetByUID(manager, uid, &account) == 0) {
        print_success("查询成功");
        printf("  账号名: %s\n", account.accountname);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 按QQ查询
    print_info("按QQ查询: 123456789");
    if (Account_GetByQQ(manager, "123456789", &account) == 0) {
        print_success("查询成功");
        printf("  账号名: %s\n", account.accountname);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试验证账号
int test_verify_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 验证账号密码%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    // 正确的账号密码
    print_info("验证正确密码: testuser1");
    if (Account_Verify(manager, "testuser1", "5f4dcc3b5aa765d61d8327deb882cf99", &account)) {
        print_success("验证成功");
        printf("  UID: %d\n", account.UID);
    } else {
        print_error("验证失败");
        return -1;
    }

    // 错误的密码
    print_info("验证错误密码: testuser1");
    if (!Account_Verify(manager, "testuser1", "wrongpassword", NULL)) {
        print_success("正确拒绝了错误密码");
    } else {
        print_error("错误地接受了错误密码");
        return -1;
    }

    // 不存在的账号
    print_info("验证不存在的账号");
    if (!Account_Verify(manager, "nonexistent", "anypassword", NULL)) {
        print_success("正确拒绝了不存在的账号");
    } else {
        print_error("错误地接受了不存在的账号");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试更新账号
int test_update_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 更新账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    // 获取账号
    if (Account_GetByName(manager, "testuser1", &account) != 0) {
        print_error("获取账号失败");
        return -1;
    }

    print_info("更新testuser1的VIP状态");
    printf("  原VIP: '%s'\n", account.VIP);

    strcpy(account.VIP, "VIP5");
    account.billing = 1;  // 改为付费用户

    if (Account_Update(manager, &account) == 0) {
        print_success("更新成功");

        // 重新查询验证
        Account updated;
        if (Account_GetByUID(manager, account.UID, &updated) == 0) {
            printf("  新VIP: '%s'\n", updated.VIP);
            printf("  新计费状态: %d\n", updated.billing);
        }
    } else {
        print_error("更新失败");
        return -1;
    }

    // 测试更新密码
    print_info("更新密码");
    if (Account_UpdatePassword(manager, account.UID, "newpasswordhash") == 0) {
        print_success("密码更新成功");
    } else {
        print_error("密码更新失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试存在性检查
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 存在性检查%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 检查存在的账号名
    print_info("检查已存在的账号名: testuser1");
    if (Account_ExistsByName(manager, "testuser1")) {
        print_success("正确识别出账号存在");
    } else {
        print_error("错误地判断账号不存在");
        return -1;
    }

    // 检查不存在的账号名
    print_info("检查不存在的账号名: nonexistent");
    if (!Account_ExistsByName(manager, "nonexistent")) {
        print_success("正确识别出账号不存在");
    } else {
        print_error("错误地判断账号存在");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试账号列表和计数
int test_list_and_count(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 账号列表和计数%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 获取账号总数
    print_info("获取账号总数");
    int count = Account_GetCount(manager);
    if (count >= 0) {
        print_success("获取成功");
        printf("  总账号数: %d\n", count);
    } else {
        print_error("获取失败");
        return -1;
    }

    // 获取账号列表
    print_info("获取账号列表 (前10个)");
    Account accounts[10];
    int fetched = Account_GetList(manager, 0, 10, accounts, 10);

    if (fetched >= 0) {
        print_success("获取成功");
        printf("  获取到 %d 个账号:\n", fetched);

        for (int i = 0; i < fetched; i++) {
            printf("    %d. UID=%d, Name=%s, VIP=%s, Billing=%d\n",
                   i + 1, accounts[i].UID, accounts[i].accountname,
                   accounts[i].VIP[0] ? accounts[i].VIP : "(none)",
                   accounts[i].billing);
        }
    } else {
        print_error("获取失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试删除账号
int test_delete_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 删除账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    // 查找要删除的账号
    if (Account_GetByName(manager, "testuser3", &account) != 0) {
        print_error("找不到要删除的账号");
        return -1;
    }

    print_info("删除账号: testuser3");
    printf("  UID: %d\n", account.UID);

    if (Account_Delete(manager, account.UID) == 0) {
        print_success("删除成功");

        // 验证已删除
        if (!Account_ExistsByUID(manager, account.UID)) {
            print_success("确认账号已删除");
        } else {
            print_error("账号仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - Accounts表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_accounts_orm <配置文件路径>");
        printf("  示例: ./test_accounts_orm ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("配置文件: %s\n\n", config_file);

    // 初始化数据库连接
    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("初始化数据库连接...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");

    // 连接到账号数据库
    print_info("连接到账号数据库...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 7;

    if (test_create_account(&manager) == 0) success_count++;
    if (test_query_account(&manager) == 0) success_count++;
    if (test_verify_account(&manager) == 0) success_count++;
    if (test_update_account(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_list_and_count(&manager) == 0) success_count++;
    if (test_delete_account(&manager) == 0) success_count++;

    // 清理
    print_info("清理数据库连接...");
    DBConnectionManager_Cleanup(&manager);
    print_success("清理完成");

    printf("\n");
    print_separator();
    printf("%s测试结果: %d/%d 通过%s\n",
           success_count == total_tests ? COLOR_GREEN : COLOR_YELLOW,
           success_count, total_tests, COLOR_RESET);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
