/**
 * DNF游戏服务器 - Accounts ORM 测试程序
 *
 * 功能:
 * 1. 测试账号CRUD操作
 * 2. 测试账号查询功能
 * 3. 测试密码验证
 * 4. 测试VIP和QQ更新
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "accounts_orm.h"

/* ANSI颜色代码 */
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

/* 测试创建账号 */
int test_create_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;
    int i;

    /* 创建账号1 */
    print_info("创建测试账号1 - testuser001");
    Account_Init(&account);
    strcpy(account.accountname, "testuser001");
    strcpy(account.password, "password123");
    strcpy(account.qq, "123456789");
    account.dzuid = 1001;
    account.billing = 1;
    strcpy(account.VIP, "0");

    if (Account_Create(manager, &account) == 0) {
        print_success("账号创建成功");
        printf("  账号名: %s, UID: %d\n", account.accountname, account.UID);
    } else {
        print_error("账号创建失败");
        return -1;
    }

    /* 创建账号2 */
    print_info("创建测试账号2 - testuser002");
    Account_Init(&account);
    strcpy(account.accountname, "testuser002");
    strcpy(account.password, "pass456");
    strcpy(account.qq, "987654321");
    account.dzuid = 1002;
    account.billing = 0;
    strcpy(account.VIP, "1");

    if (Account_Create(manager, &account) == 0) {
        print_success("账号创建成功");
    } else {
        print_error("账号创建失败");
        return -1;
    }

    /* 批量创建账号 */
    print_info("批量创建测试账号 (testuser003-testuser005)");
    for (i = 3; i <= 5; i++) {
        Account_Init(&account);
        snprintf(account.accountname, sizeof(account.accountname), "testuser%03d", i);
        snprintf(account.password, sizeof(account.password), "pass%d", i);
        snprintf(account.qq, sizeof(account.qq), "10000%d", i);
        account.dzuid = 1000 + i;
        account.billing = i % 2;
        strcpy(account.VIP, (i % 2 == 0) ? "1" : "0");
        Account_Create(manager, &account);
    }
    print_success("批量创建完成 (3个账号)");

    printf("\n");
    return 0;
}

/* 测试查询账号 */
int test_query_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;
    int count;

    /* 按账号名查询 */
    print_info("查询账号 'testuser001'");
    if (Account_GetByName(manager, "testuser001", &account) == 0) {
        print_success("查询成功");
        printf("  UID: %d, QQ: %s, VIP: %s\n",
               account.UID, account.qq, account.VIP);
    } else {
        print_error("查询失败");
        return -1;
    }

    /* 按UID查询 */
    int test_uid = account.UID;
    printf("  按UID查询 (UID=%d)\n", test_uid);
    print_info("按UID查询");
    if (Account_GetByUID(manager, test_uid, &account) == 0) {
        print_success("查询成功");
        printf("  账号名: %s\n", account.accountname);
    } else {
        print_error("查询失败");
        return -1;
    }

    /* 查询不存在的账号 */
    print_info("查询不存在的账号 'nonexistent'");
    if (Account_GetByName(manager, "nonexistent", &account) == 0) {
        print_warning("查到了不存在的账号");
    } else {
        print_success("正确返回查询失败");
    }

    /* 获取账号总数 */
    print_info("获取账号总数");
    count = Account_GetCount(manager);
    if (count >= 0) {
        print_success("获取成功");
        printf("  账号总数: %d\n", count);
    } else {
        print_error("获取失败");
        return -1;
    }

    /* 检查账号是否存在 */
    print_info("检查账号是否存在 (testuser001)");
    if (Account_NameExists(manager, "testuser001")) {
        print_success("账号存在");
    } else {
        print_error("检测失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试更新账号 */
int test_update_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    /* 获取测试账号 */
    if (Account_GetByName(manager, "testuser001", &account) != 0) {
        print_error("无法找到测试账号");
        return -1;
    }

    printf("  原账号信息: QQ=%s, VIP=%s\n", account.qq, account.VIP);

    /* 更新账号信息 */
    print_info("更新账号完整信息");
    strcpy(account.qq, "999888777");
    strcpy(account.VIP, "1");
    account.billing = 1;

    if (Account_Update(manager, &account) == 0) {
        print_success("更新成功");

        /* 验证更新 */
        Account verify_account;
        if (Account_GetByUID(manager, account.UID, &verify_account) == 0) {
            if (strcmp(verify_account.qq, "999888777") == 0 &&
                strcmp(verify_account.VIP, "1") == 0) {
                print_success("验证: 账号信息更新正确");
                printf("  新QQ: %s, 新VIP: %s\n",
                       verify_account.qq, verify_account.VIP);
            } else {
                print_error("验证失败: 信息不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试密码验证 */
int test_verify_password(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 密码验证%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    int uid;

    /* 正确密码验证 */
    print_info("验证正确密码 (testuser001/password123)");
    uid = Account_Verify(manager, "testuser001", "password123");
    if (uid > 0) {
        print_success("验证成功");
        printf("  返回UID: %d\n", uid);
    } else {
        print_error("验证失败");
        return -1;
    }

    /* 错误密码验证 */
    print_info("验证错误密码 (testuser001/wrongpass)");
    uid = Account_Verify(manager, "testuser001", "wrongpass");
    if (uid == -1) {
        print_success("正确返回验证失败");
    } else {
        print_error("应该返回失败但返回了成功");
        return -1;
    }

    /* 不存在的账号 */
    print_info("验证不存在的账号 (nouser/nopass)");
    uid = Account_Verify(manager, "nouser", "nopass");
    if (uid == -1) {
        print_success("正确返回验证失败");
    } else {
        print_error("应该返回失败但返回了成功");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试专用更新函数 */
int test_specialized_updates(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 专用更新函数%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    /* 获取测试账号 */
    if (Account_GetByName(manager, "testuser002", &account) != 0) {
        print_error("无法找到测试账号");
        return -1;
    }

    int test_uid = account.UID;

    /* 更新密码 */
    print_info("更新账号密码");
    if (Account_UpdatePassword(manager, test_uid, "newpass789") == 0) {
        print_success("密码更新成功");

        /* 验证新密码 */
        int verify_uid = Account_Verify(manager, "testuser002", "newpass789");
        if (verify_uid == test_uid) {
            print_success("验证: 新密码生效");
        } else {
            print_error("验证失败: 新密码无效");
            return -1;
        }
    } else {
        print_error("密码更新失败");
        return -1;
    }

    /* 更新VIP状态 */
    print_info("更新VIP状态");
    if (Account_UpdateVIP(manager, test_uid, "2") == 0) {
        print_success("VIP更新成功");

        /* 验证VIP */
        if (Account_GetByUID(manager, test_uid, &account) == 0) {
            if (strcmp(account.VIP, "2") == 0) {
                print_success("验证: VIP状态更新正确");
            } else {
                print_error("验证失败: VIP状态不匹配");
                return -1;
            }
        }
    } else {
        print_error("VIP更新失败");
        return -1;
    }

    /* 更新QQ号 */
    print_info("更新QQ号");
    if (Account_UpdateQQ(manager, test_uid, "555666777") == 0) {
        print_success("QQ更新成功");

        /* 验证QQ */
        if (Account_GetByUID(manager, test_uid, &account) == 0) {
            if (strcmp(account.qq, "555666777") == 0) {
                print_success("验证: QQ号更新正确");
            } else {
                print_error("验证失败: QQ号不匹配");
                return -1;
            }
        }
    } else {
        print_error("QQ更新失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试按QQ查询 */
int test_query_by_qq(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 按QQ查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account accounts[10];
    int count;

    /* 按QQ查询 */
    print_info("查询QQ '555666777' 的账号");
    count = Account_GetByQQ(manager, "555666777", accounts, 10);
    if (count > 0) {
        print_success("查询成功");
        printf("  找到 %d 个账号\n", count);
        printf("  账号名: %s, UID: %d\n",
               accounts[0].accountname, accounts[0].UID);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试按VIP查询 */
int test_query_by_vip(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 按VIP查询%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account accounts[10];
    int count;
    int i;

    /* 按VIP查询 */
    print_info("查询VIP='1'的账号");
    count = Account_GetByVIP(manager, "1", accounts, 10);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 个VIP账号\n", count);

        int display = (count > 3) ? 3 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] %s (UID=%d)\n",
                   i + 1, accounts[i].accountname, accounts[i].UID);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试搜索功能 */
int test_search_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 搜索账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account accounts[10];
    int count;
    int i;

    /* 按关键词搜索 */
    print_info("搜索关键词 'testuser'");
    count = Account_Search(manager, "testuser", accounts, 10);
    if (count >= 0) {
        print_success("搜索成功");
        printf("  找到 %d 个匹配账号\n", count);

        int display = (count > 5) ? 5 : count;
        for (i = 0; i < display; i++) {
            printf("  [%d] %s (QQ=%s)\n",
                   i + 1, accounts[i].accountname, accounts[i].qq);
        }
    } else {
        print_error("搜索失败");
        return -1;
    }

    printf("\n");
    return 0;
}

/* 测试删除账号 */
int test_delete_account(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试9: 删除账号%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    Account account;

    /* 获取要删除的账号 */
    if (Account_GetByName(manager, "testuser005", &account) != 0) {
        print_error("无法找到测试账号");
        return -1;
    }

    int test_uid = account.UID;
    printf("  准备删除UID: %d (testuser005)\n", test_uid);

    /* 删除账号 */
    print_info("删除账号");
    if (Account_Delete(manager, test_uid) == 0) {
        print_success("删除成功");

        /* 验证删除 */
        if (!Account_Exists(manager, test_uid)) {
            print_success("验证: 账号已被删除");
        } else {
            print_error("验证失败: 账号仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    /* 尝试删除不存在的账号 */
    print_info("删除不存在的账号 (UID=999999)");
    if (Account_Delete(manager, 999999) == 0) {
        print_success("返回成功(没有行被删除)");
    } else {
        print_warning("返回失败");
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

    /* 初始化数据库连接 */
    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("初始化数据库连接...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");

    /* 连接到台湾数据库 */
    print_info("连接到台湾数据库 (test_d_taiwan)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    /* 清理旧测试数据 */
    print_info("清理旧测试数据...");
    char cleanup_query[256];
    snprintf(cleanup_query, sizeof(cleanup_query),
             "DELETE FROM accounts WHERE accountname LIKE 'testuser%%'");
    DBQueryResult cleanup_result;
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    /* 运行测试 */
    int success_count = 0;
    int total_tests = 9;

    if (test_create_account(&manager) == 0) success_count++;
    if (test_query_account(&manager) == 0) success_count++;
    if (test_update_account(&manager) == 0) success_count++;
    if (test_verify_password(&manager) == 0) success_count++;
    if (test_specialized_updates(&manager) == 0) success_count++;
    if (test_query_by_qq(&manager) == 0) success_count++;
    if (test_query_by_vip(&manager) == 0) success_count++;
    if (test_search_account(&manager) == 0) success_count++;
    if (test_delete_account(&manager) == 0) success_count++;

    /* 清理测试数据 */
    print_separator();
    print_info("清理测试数据...");
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    /* 清理连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 输出测试结果 */
    print_separator();
    printf("\n");
    printf("%s测试结果汇总:%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("  通过: %s%d%s\n", COLOR_GREEN, success_count, COLOR_RESET);
    printf("  失败: %s%d%s\n", COLOR_RED, total_tests - success_count, COLOR_RESET);
    printf("  总计: %d\n", total_tests);
    printf("  成功率: %.1f%%\n", (success_count * 100.0) / total_tests);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
