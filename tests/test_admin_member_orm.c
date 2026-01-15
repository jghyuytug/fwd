/**
 * DNF游戏服务器 - AdminMember表ORM测试程序
 *
 * 功能:
 * 1. 测试管理员CRUD操作
 * 2. 测试管理员权限检查
 * 3. 测试管理员等级管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "admin_member_orm.h"

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

// 测试创建管理员
int test_create_admin(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建管理员记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AdminMember admin;

    // 创建版主
    print_info("创建版主 (账号ID=10, Level=1)");
    AdminMember_Init(&admin);
    admin.m_id = 10;
    admin.admin_level = ADMIN_LEVEL_MODERATOR;

    if (AdminMember_Create(manager, &admin) == 0) {
        print_success("版主创建成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
        printf("  创建时间: %s\n", admin.create_time);
    } else {
        print_error("版主创建失败");
        return -1;
    }

    // 创建GM
    print_info("创建GM (账号ID=11, Level=2)");
    AdminMember_Init(&admin);
    admin.m_id = 11;
    admin.admin_level = ADMIN_LEVEL_GM;

    if (AdminMember_Create(manager, &admin) == 0) {
        print_success("GM创建成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
    } else {
        print_error("GM创建失败");
        return -1;
    }

    // 创建高级GM
    print_info("创建高级GM (账号ID=12, Level=3)");
    AdminMember_Init(&admin);
    admin.m_id = 12;
    admin.admin_level = ADMIN_LEVEL_SENIOR_GM;

    if (AdminMember_Create(manager, &admin) == 0) {
        print_success("高级GM创建成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
    } else {
        print_error("高级GM创建失败");
        return -1;
    }

    // 创建管理员
    print_info("创建管理员 (账号ID=13, Level=4)");
    AdminMember_Init(&admin);
    admin.m_id = 13;
    admin.admin_level = ADMIN_LEVEL_ADMIN;

    if (AdminMember_Create(manager, &admin) == 0) {
        print_success("管理员创建成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
    } else {
        print_error("管理员创建失败");
        return -1;
    }

    // 创建超级管理员
    print_info("创建超级管理员 (账号ID=14, Level=5)");
    AdminMember_Init(&admin);
    admin.m_id = 14;
    admin.admin_level = ADMIN_LEVEL_SUPER_ADMIN;

    if (AdminMember_Create(manager, &admin) == 0) {
        print_success("超级管理员创建成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
    } else {
        print_error("超级管理员创建失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试查询管理员
int test_query_admin(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询管理员记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AdminMember admin;

    // 查询版主
    print_info("查询版主 (账号ID=10)");
    if (AdminMember_GetByID(manager, 10, &admin) == 0) {
        print_success("查询成功");
        AdminMember_Print(&admin);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 查询GM
    print_info("查询GM (账号ID=11)");
    if (AdminMember_GetByID(manager, 11, &admin) == 0) {
        print_success("查询成功");
        printf("  账号ID: %d\n", admin.m_id);
        printf("  等级: %d (%s)\n", admin.admin_level,
               AdminMember_GetLevelName(admin.admin_level));
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试更新管理员
int test_update_admin(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新管理员记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AdminMember admin;

    // 获取原管理员信息
    print_info("获取版主原信息 (账号ID=10)");
    if (AdminMember_GetByID(manager, 10, &admin) != 0) {
        print_error("获取原信息失败");
        return -1;
    }
    printf("  原等级: %d (%s)\n", admin.admin_level,
           AdminMember_GetLevelName(admin.admin_level));

    // 升级到GM
    print_info("升级版主到GM (Level 1 -> 2)");
    if (AdminMember_UpdateLevel(manager, 10, ADMIN_LEVEL_GM) == 0) {
        print_success("升级成功");

        // 验证更新
        if (AdminMember_GetByID(manager, 10, &admin) == 0) {
            printf("  新等级: %d (%s)\n", admin.admin_level,
                   AdminMember_GetLevelName(admin.admin_level));

            if (admin.admin_level == ADMIN_LEVEL_GM) {
                print_success("等级更新验证通过");
            } else {
                print_error("等级更新验证失败");
                return -1;
            }
        } else {
            print_error("验证查询失败");
            return -1;
        }
    } else {
        print_error("升级失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试权限检查
int test_permission_check(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 管理员权限检查%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 检查存在的管理员
    print_info("检查账号10是否为管理员");
    if (AdminMember_Exists(manager, 10)) {
        print_success("正确识别出管理员");
    } else {
        print_error("错误地判断不是管理员");
        return -1;
    }

    // 检查不存在的管理员
    print_info("检查账号999是否为管理员");
    if (!AdminMember_Exists(manager, 999)) {
        print_success("正确识别出非管理员");
    } else {
        print_error("错误地判断是管理员");
        return -1;
    }

    // 检查GM权限 (账号11是GM, Level=2)
    print_info("检查账号11是否具有GM权限 (Level >= 2)");
    if (AdminMember_HasLevel(manager, 11, ADMIN_LEVEL_GM)) {
        print_success("权限检查通过 - 具有GM权限");
    } else {
        print_error("权限检查失败");
        return -1;
    }

    // 检查高级GM权限 (账号11是GM, Level=2, 不足Level 3)
    print_info("检查账号11是否具有高级GM权限 (Level >= 3)");
    if (!AdminMember_HasLevel(manager, 11, ADMIN_LEVEL_SENIOR_GM)) {
        print_success("权限检查通过 - 权限不足");
    } else {
        print_error("权限检查失败 - 错误地给予权限");
        return -1;
    }

    // 检查超级管理员的全部权限
    print_info("检查账号14(超级管理员)是否具有管理员权限 (Level >= 4)");
    if (AdminMember_HasLevel(manager, 14, ADMIN_LEVEL_ADMIN)) {
        print_success("权限检查通过 - 超级管理员具有管理员权限");
    } else {
        print_error("权限检查失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试按等级查询
int test_query_by_level(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 按等级查询管理员%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AdminMember admins[100];

    // 查询GM等级的管理员
    print_info("查询所有GM等级的管理员 (Level=2)");
    int count = AdminMember_GetByLevel(manager, ADMIN_LEVEL_GM, admins, 100);
    if (count >= 0) {
        print_success("查询成功");
        printf("  GM数量: %d\n", count);
        for (int i = 0; i < count; i++) {
            printf("    %d. 账号ID=%d, 等级=%d (%s)\n",
                   i + 1, admins[i].m_id, admins[i].admin_level,
                   AdminMember_GetLevelName(admins[i].admin_level));
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    // 查询高级GM
    print_info("查询所有高级GM (Level=3)");
    count = AdminMember_GetByLevel(manager, ADMIN_LEVEL_SENIOR_GM, admins, 100);
    if (count >= 0) {
        print_success("查询成功");
        printf("  高级GM数量: %d\n", count);
        for (int i = 0; i < count; i++) {
            printf("    %d. 账号ID=%d, 等级=%d (%s)\n",
                   i + 1, admins[i].m_id, admins[i].admin_level,
                   AdminMember_GetLevelName(admins[i].admin_level));
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试获取所有管理员
int test_get_all_admins(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 获取所有管理员%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AdminMember admins[100];

    print_info("获取所有管理员列表");
    int count = AdminMember_GetAll(manager, admins, 100);
    if (count >= 0) {
        print_success("查询成功");
        printf("  管理员总数: %d\n", count);
        printf("  管理员列表:\n");
        for (int i = 0; i < count; i++) {
            printf("    %d. 账号ID=%d, 等级=%d (%s), 创建时间=%s\n",
                   i + 1, admins[i].m_id, admins[i].admin_level,
                   AdminMember_GetLevelName(admins[i].admin_level),
                   admins[i].create_time);
        }
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试统计功能
int test_statistics(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 统计功能%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 获取管理员总数
    print_info("获取管理员总数");
    int total = AdminMember_GetCount(manager);
    if (total >= 0) {
        print_success("查询成功");
        printf("  管理员总数: %d\n", total);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 获取各等级管理员数量
    print_info("获取各等级管理员数量");
    for (int level = ADMIN_LEVEL_MODERATOR; level <= ADMIN_LEVEL_SUPER_ADMIN; level++) {
        int count = AdminMember_GetCountByLevel(manager, level);
        if (count >= 0) {
            printf("  %s (Level %d): %d 人\n",
                   AdminMember_GetLevelName(level), level, count);
        } else {
            print_error("查询失败");
            return -1;
        }
    }
    print_success("统计查询全部成功");

    printf("\n");
    return 0;
}

// 测试删除管理员
int test_delete_admin(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 删除管理员记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 删除账号12的管理员
    print_info("删除账号12的管理员记录 (高级GM)");
    if (AdminMember_Delete(manager, 12) == 0) {
        print_success("删除成功");

        // 验证删除
        if (!AdminMember_Exists(manager, 12)) {
            print_success("确认管理员已被删除");
        } else {
            print_error("管理员仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    // 再次查询确认
    print_info("再次查询确认删除");
    AdminMember admin;
    if (AdminMember_GetByID(manager, 12, &admin) != 0) {
        print_success("确认查询不到已删除的管理员");
    } else {
        print_error("仍然可以查询到已删除的管理员");
        return -1;
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - AdminMember表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_admin_member_orm <配置文件路径>");
        printf("  示例: ./test_admin_member_orm ../config/database.cfg\n");
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

    // 连接到账号数据库 (admin_member表在test_d_taiwan)
    print_info("连接到账号数据库 (test_d_taiwan)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 8;

    if (test_create_admin(&manager) == 0) success_count++;
    if (test_query_admin(&manager) == 0) success_count++;
    if (test_update_admin(&manager) == 0) success_count++;
    if (test_permission_check(&manager) == 0) success_count++;
    if (test_query_by_level(&manager) == 0) success_count++;
    if (test_get_all_admins(&manager) == 0) success_count++;
    if (test_statistics(&manager) == 0) success_count++;
    if (test_delete_admin(&manager) == 0) success_count++;

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
