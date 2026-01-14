/**
 * DNF游戏服务器 - CharacItemStat ORM测试
 */

#include "charac_item_stat_orm.h"
#include "db_connection_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ANSI颜色代码 */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/* 测试结果统计 */
static int tests_passed = 0;
static int tests_failed = 0;

/* 辅助函数：打印测试标题 */
static void print_test_header(const char* title) {
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
    printf(COLOR_CYAN "%s\n" COLOR_RESET, title);
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
}

/* 辅助函数：打印测试步骤 */
static void print_step(const char* step) {
    printf(COLOR_BLUE "● %s\n" COLOR_RESET, step);
}

/* 辅助函数：打印成功信息 */
static void print_success(const char* msg) {
    printf(COLOR_GREEN "✓ %s\n" COLOR_RESET, msg);
    tests_passed++;
}

/* 辅助函数：打印失败信息 */
static void print_error(const char* msg) {
    printf(COLOR_RED "✗ %s\n" COLOR_RESET, msg);
    tests_failed++;
}

/* 辅助函数：生成测试BLOB数据 */
static void generate_test_blob(unsigned char* data, int size, int pattern) {
    int i;
    for (i = 0; i < size; i++) {
        data[i] = (unsigned char)((i + pattern) % 256);
    }
}

/* ========================================================================
 * 测试用例
 * ======================================================================== */

/* 测试1: 创建装备属性记录 */
static void test_create(DBConnectionManager* manager) {
    CharacItemStat item_stat1, item_stat2;
    int ret;

    print_test_header("测试1: 创建装备属性记录");

    /* 测试1.1: 创建角色1的装备属性（小数据量） */
    print_step("创建角色1的装备属性（小数据量: 100字节）");
    CharacItemStat_Init(&item_stat1);
    item_stat1.charac_no = 1;

    /* 分配并填充BLOB数据 */
    CharacItemStat_AllocateBlob(&item_stat1, 1, 100);  /* cooltime_item */
    CharacItemStat_AllocateBlob(&item_stat1, 2, 100);  /* effect_item */
    CharacItemStat_AllocateBlob(&item_stat1, 3, 100);  /* check_flag */

    generate_test_blob(item_stat1.cooltime_item, 100, 0);
    generate_test_blob(item_stat1.effect_item, 100, 1);
    generate_test_blob(item_stat1.check_flag, 100, 2);

    ret = CharacItemStat_Create(manager, &item_stat1);
    if (ret == 0) {
        print_success("装备属性记录创建成功");
        printf("  角色ID: %d\n", item_stat1.charac_no);
        printf("  冷却数据: %d bytes\n", item_stat1.cooltime_item_len);
        printf("  效果数据: %d bytes\n", item_stat1.effect_item_len);
        printf("  标记数据: %d bytes\n", item_stat1.check_flag_len);
    } else {
        print_error("装备属性记录创建失败");
    }

    CharacItemStat_Free(&item_stat1);

    /* 测试1.2: 创建角色2的装备属性（大数据量） */
    print_step("创建角色2的装备属性（大数据量: 1024字节）");
    CharacItemStat_Init(&item_stat2);
    item_stat2.charac_no = 2;

    CharacItemStat_AllocateBlob(&item_stat2, 1, 1024);
    CharacItemStat_AllocateBlob(&item_stat2, 2, 1024);
    CharacItemStat_AllocateBlob(&item_stat2, 3, 1024);

    generate_test_blob(item_stat2.cooltime_item, 1024, 10);
    generate_test_blob(item_stat2.effect_item, 1024, 11);
    generate_test_blob(item_stat2.check_flag, 1024, 12);

    ret = CharacItemStat_Create(manager, &item_stat2);
    if (ret == 0) {
        print_success("装备属性记录创建成功");
        printf("  角色ID: %d\n", item_stat2.charac_no);
        printf("  数据大小: %d bytes\n", item_stat2.cooltime_item_len);
    } else {
        print_error("装备属性记录创建失败");
    }

    CharacItemStat_Free(&item_stat2);
}

/* 测试2: 查询装备属性记录 */
static void test_query(DBConnectionManager* manager) {
    CharacItemStat item_stat;
    int ret;

    print_test_header("测试2: 查询装备属性记录");

    /* 测试2.1: 查询角色1的装备属性 */
    print_step("查询角色1的装备属性");
    CharacItemStat_Init(&item_stat);
    ret = CharacItemStat_GetByCharacNo(manager, 1, &item_stat);
    if (ret == 0) {
        print_success("查询成功");
        CharacItemStat_Print(&item_stat);

        /* 验证数据完整性 */
        if (item_stat.cooltime_item_len == 100 &&
            item_stat.effect_item_len == 100 &&
            item_stat.check_flag_len == 100) {
            print_success("数据完整性验证通过");
        } else {
            print_error("数据完整性验证失败");
        }
    } else {
        print_error("查询失败");
    }

    CharacItemStat_Free(&item_stat);

    /* 测试2.2: 查询角色2的装备属性 */
    print_step("查询角色2的装备属性");
    CharacItemStat_Init(&item_stat);
    ret = CharacItemStat_GetByCharacNo(manager, 2, &item_stat);
    if (ret == 0) {
        print_success("查询成功");
        printf("  角色ID: %d\n", item_stat.charac_no);
        printf("  数据大小: %d bytes\n", item_stat.cooltime_item_len);
    } else {
        print_error("查询失败");
    }

    CharacItemStat_Free(&item_stat);
}

/* 测试3: 更新装备属性数据 */
static void test_update(DBConnectionManager* manager) {
    CharacItemStat item_stat;
    int ret;

    print_test_header("测试3: 更新装备属性数据");

    /* 测试3.1: 获取原数据 */
    print_step("获取角色1的原装备属性数据");
    CharacItemStat_Init(&item_stat);
    ret = CharacItemStat_GetByCharacNo(manager, 1, &item_stat);
    if (ret == 0) {
        printf("  原数据大小: %d bytes\n", item_stat.cooltime_item_len);
    } else {
        print_error("获取原数据失败");
        return;
    }

    /* 测试3.2: 更新为更大的数据 */
    print_step("更新为更大的数据（500字节）");
    CharacItemStat_Free(&item_stat);
    CharacItemStat_AllocateBlob(&item_stat, 1, 500);
    CharacItemStat_AllocateBlob(&item_stat, 2, 500);
    CharacItemStat_AllocateBlob(&item_stat, 3, 500);

    generate_test_blob(item_stat.cooltime_item, 500, 20);
    generate_test_blob(item_stat.effect_item, 500, 21);
    generate_test_blob(item_stat.check_flag, 500, 22);

    ret = CharacItemStat_Update(manager, &item_stat);
    if (ret == 0) {
        print_success("更新成功");
        printf("  新数据大小: %d bytes\n", item_stat.cooltime_item_len);
    } else {
        print_error("更新失败");
    }

    /* 测试3.3: 验证更新结果 */
    print_step("验证更新结果");
    CharacItemStat_Free(&item_stat);
    CharacItemStat_Init(&item_stat);
    ret = CharacItemStat_GetByCharacNo(manager, 1, &item_stat);
    if (ret == 0 && item_stat.cooltime_item_len == 500) {
        print_success("更新验证通过");
        printf("  新数据大小: %d bytes\n", item_stat.cooltime_item_len);
    } else {
        print_error("更新验证失败");
    }

    CharacItemStat_Free(&item_stat);
}

/* 测试4: 存在性检查 */
static void test_exists(DBConnectionManager* manager) {
    bool exists;

    print_test_header("测试4: 存在性检查");

    /* 测试4.1: 检查已存在的记录 */
    print_step("检查已存在的装备属性（角色1）");
    exists = CharacItemStat_Exists(manager, 1);
    if (exists) {
        print_success("正确识别出记录存在");
    } else {
        print_error("未能识别出记录存在");
    }

    /* 测试4.2: 检查不存在的记录 */
    print_step("检查不存在的装备属性（角色999）");
    exists = CharacItemStat_Exists(manager, 999);
    if (!exists) {
        print_success("正确识别出记录不存在");
    } else {
        print_error("错误识别为记录存在");
    }
}

/* 测试5: BLOB大小查询 */
static void test_blob_size(DBConnectionManager* manager) {
    int size;

    print_test_header("测试5: BLOB大小查询");

    /* 测试5.1: 查询cooltime_item大小 */
    print_step("查询角色1的cooltime_item数据大小");
    size = CharacItemStat_GetBlobSize(manager, 1, "cooltime_item");
    if (size > 0) {
        print_success("查询成功");
        printf("  cooltime_item 数据大小: %d 字节\n", size);
    } else {
        print_error("查询失败");
    }

    /* 测试5.2: 查询effect_item大小 */
    print_step("查询角色1的effect_item数据大小");
    size = CharacItemStat_GetBlobSize(manager, 1, "effect_item");
    if (size > 0) {
        print_success("查询成功");
        printf("  effect_item 数据大小: %d 字节\n", size);
    } else {
        print_error("查询失败");
    }

    /* 测试5.3: 查询check_flag大小 */
    print_step("查询角色2的check_flag数据大小");
    size = CharacItemStat_GetBlobSize(manager, 2, "check_flag");
    if (size > 0) {
        print_success("查询成功");
        printf("  check_flag 数据大小: %d 字节\n", size);
    } else {
        print_error("查询失败");
    }
}

/* 测试6: 统计功能 */
static void test_statistics(DBConnectionManager* manager) {
    int count, charac_nos[10];
    int i;

    print_test_header("测试6: 统计功能");

    /* 测试6.1: 获取记录总数 */
    print_step("获取装备属性记录总数");
    count = CharacItemStat_GetCount(manager);
    if (count >= 0) {
        print_success("查询成功");
        printf("  记录总数: %d\n", count);
    } else {
        print_error("查询失败");
    }

    /* 测试6.2: 获取所有角色编号列表 */
    print_step("获取所有有装备属性的角色编号列表");
    count = CharacItemStat_GetAllCharacNos(manager, charac_nos, 10);
    if (count > 0) {
        print_success("查询成功");
        printf("  角色数量: %d\n", count);
        printf("  角色列表: ");
        for (i = 0; i < count; i++) {
            printf("%d ", charac_nos[i]);
        }
        printf("\n");
    } else {
        print_error("查询失败");
    }
}

/* 测试7: 装备属性数据复制 */
static void test_copy(DBConnectionManager* manager) {
    CharacItemStat src, dest;
    int ret;

    print_test_header("测试7: 装备属性数据复制");

    /* 测试7.1: 获取源数据 */
    print_step("获取源装备属性数据（角色1）");
    CharacItemStat_Init(&src);
    ret = CharacItemStat_GetByCharacNo(manager, 1, &src);
    if (ret == 0) {
        print_success("源数据获取成功");
        printf("  源数据大小: %d bytes\n", src.cooltime_item_len);
    } else {
        print_error("源数据获取失败");
        return;
    }

    /* 测试7.2: 复制数据 */
    print_step("复制装备属性数据");
    CharacItemStat_Init(&dest);
    ret = CharacItemStat_Copy(&dest, &src);
    if (ret == 0) {
        print_success("复制成功");
        printf("  目标数据大小: %d bytes\n", dest.cooltime_item_len);
    } else {
        print_error("复制失败");
        CharacItemStat_Free(&src);
        return;
    }

    /* 测试7.3: 验证数据一致性 */
    if (dest.charac_no == src.charac_no &&
        dest.cooltime_item_len == src.cooltime_item_len &&
        dest.effect_item_len == src.effect_item_len &&
        dest.check_flag_len == src.check_flag_len &&
        memcmp(dest.cooltime_item, src.cooltime_item, src.cooltime_item_len) == 0 &&
        memcmp(dest.effect_item, src.effect_item, src.effect_item_len) == 0 &&
        memcmp(dest.check_flag, src.check_flag, src.check_flag_len) == 0) {
        print_success("数据一致性验证通过");
    } else {
        print_error("数据一致性验证失败");
    }

    CharacItemStat_Free(&src);
    CharacItemStat_Free(&dest);
}

/* 测试8: 删除装备属性记录 */
static void test_delete(DBConnectionManager* manager) {
    int ret;
    bool exists;

    print_test_header("测试8: 删除装备属性记录");

    /* 测试8.1: 删除角色2的装备属性 */
    print_step("删除角色2的装备属性记录");
    ret = CharacItemStat_Delete(manager, 2);
    if (ret == 0) {
        print_success("删除成功");
    } else {
        print_error("删除失败");
    }

    /* 测试8.2: 确认已删除 */
    exists = CharacItemStat_Exists(manager, 2);
    if (!exists) {
        print_success("确认装备属性已被删除");
    } else {
        print_error("装备属性仍然存在");
    }
}

/* ========================================================================
 * 主函数
 * ======================================================================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    const char* config_file;

    /* 打印测试标题 */
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
    printf(COLOR_CYAN "  DNF游戏服务器 - CharacItemStat表ORM测试  \n" COLOR_RESET);
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
    printf("\n");

    /* 检查命令行参数 */
    if (argc < 2) {
        printf(COLOR_RED "用法: %s <配置文件路径>\n" COLOR_RESET, argv[0]);
        return 1;
    }
    config_file = argv[1];
    printf("配置文件: %s\n\n", config_file);

    /* 初始化数据库连接 */
    print_step("初始化数据库连接...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");

    /* 连接到角色数据库 */
    print_step("连接到角色数据库 (test_taiwan_cain)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        print_error("连接失败");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    /* 运行测试用例 */
    test_create(&manager);
    test_query(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_blob_size(&manager);
    test_statistics(&manager);
    test_copy(&manager);
    test_delete(&manager);

    /* 清理数据库连接 */
    print_step("清理数据库连接...");
    DBConnectionManager_Cleanup(&manager);
    print_success("清理完成");

    /* 打印测试结果 */
    printf("\n");
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
    if (tests_failed == 0) {
        printf(COLOR_GREEN "测试结果: %d/%d 通过\n" COLOR_RESET,
               tests_passed, tests_passed + tests_failed);
    } else {
        printf(COLOR_YELLOW "测试结果: %d/%d 通过\n" COLOR_RESET,
               tests_passed, tests_passed + tests_failed);
    }
    printf(COLOR_CYAN "================================================================================\n" COLOR_RESET);
    printf("\n");

    return (tests_failed == 0) ? 0 : 1;
}
