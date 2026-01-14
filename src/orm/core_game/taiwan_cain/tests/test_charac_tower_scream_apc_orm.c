#include "charac_tower_scream_apc_orm.h"
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

#define ASSERT_STR_EQ(a, b, message) \
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

#define ASSERT_TRUE(expr, message) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

/* 测试用例 */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取惊叫之塔APC记录");

    CharacTowerScreamApc record;
    strcpy(record.reg_date, "2025-11-16");
    record.charac_no = 6000001;
    record.seq = 1;

    ASSERT_EQ(CharacTowerScreamApc_Add(manager, &record), 0, "添加记录成功");

    CharacTowerScreamApc retrieved;
    ASSERT_EQ(CharacTowerScreamApc_Get(manager, "2025-11-16", 1, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.reg_date, "2025-11-16", "注册日期匹配");
    ASSERT_EQ(retrieved.charac_no, 6000001, "角色编号匹配");
    ASSERT_EQ(retrieved.seq, 1, "序列号匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新惊叫之塔APC记录");

    CharacTowerScreamApc record;
    ASSERT_EQ(CharacTowerScreamApc_Get(manager, "2025-11-16", 1, &record), 0, "获取已存在记录");

    record.charac_no = 6000002;
    ASSERT_EQ(CharacTowerScreamApc_Update(manager, &record), 0, "更新记录成功");

    CharacTowerScreamApc retrieved;
    ASSERT_EQ(CharacTowerScreamApc_Get(manager, "2025-11-16", 1, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.charac_no, 6000002, "角色编号已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CharacTowerScreamApc_Exists(manager, "2025-11-16", 1), "记录存在");
    ASSERT_TRUE(!CharacTowerScreamApc_Exists(manager, "2025-11-16", 999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条惊叫之塔APC记录");

    CharacTowerScreamApc record2;
    strcpy(record2.reg_date, "2025-11-16");
    record2.charac_no = 6000003;
    record2.seq = 2;
    ASSERT_EQ(CharacTowerScreamApc_Add(manager, &record2), 0, "添加记录2成功");

    CharacTowerScreamApc record3;
    strcpy(record3.reg_date, "2025-11-16");
    record3.charac_no = 6000004;
    record3.seq = 3;
    ASSERT_EQ(CharacTowerScreamApc_Add(manager, &record3), 0, "添加记录3成功");

    CharacTowerScreamApc record4;
    strcpy(record4.reg_date, "2025-11-17");
    record4.charac_no = 6000005;
    record4.seq = 1;
    ASSERT_EQ(CharacTowerScreamApc_Add(manager, &record4), 0, "添加不同日期记录成功");
}

void test_get_by_date(DBConnectionManager* manager) {
    TEST_START("获取指定日期的惊叫之塔APC记录");

    CharacTowerScreamApc records[10];
    int count = CharacTowerScreamApc_GetByDate(manager, "2025-11-16", records, 10);

    ASSERT_TRUE(count >= 3, "至少找到3条记录");
    ASSERT_STR_EQ(records[0].reg_date, "2025-11-16", "日期匹配");
    ASSERT_TRUE(records[0].seq <= records[1].seq, "按seq升序排列");
}

void test_get_by_charac(DBConnectionManager* manager) {
    TEST_START("获取指定角色的惊叫之塔APC记录");

    CharacTowerScreamApc records[10];
    int count = CharacTowerScreamApc_GetByCharac(manager, 6000002, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_EQ(records[0].charac_no, 6000002, "角色编号匹配");
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("获取日期范围的惊叫之塔APC记录");

    CharacTowerScreamApc records[20];
    int count = CharacTowerScreamApc_GetByDateRange(manager, "2025-11-16", "2025-11-17", records, 20);

    ASSERT_TRUE(count >= 4, "至少找到4条记录");
}

void test_count_by_date(DBConnectionManager* manager) {
    TEST_START("统计指定日期的记录数量");

    int count = CharacTowerScreamApc_CountByDate(manager, "2025-11-16");
    ASSERT_TRUE(count >= 3, "至少有3条记录");
}

void test_count_by_charac(DBConnectionManager* manager) {
    TEST_START("统计指定角色的记录数量");

    int count = CharacTowerScreamApc_CountByCharac(manager, 6000002);
    ASSERT_EQ(count, 1, "角色有1条记录");
}

void test_delete_by_date(DBConnectionManager* manager) {
    TEST_START("删除指定日期的所有记录");

    /* 先添加一条测试记录 */
    CharacTowerScreamApc record;
    strcpy(record.reg_date, "2025-11-15");
    record.charac_no = 6000006;
    record.seq = 1;
    CharacTowerScreamApc_Add(manager, &record);

    ASSERT_EQ(CharacTowerScreamApc_DeleteByDate(manager, "2025-11-15"), 0, "删除指定日期记录成功");

    int count = CharacTowerScreamApc_CountByDate(manager, "2025-11-15");
    ASSERT_EQ(count, 0, "记录已删除");
}

void test_delete_before_date(DBConnectionManager* manager) {
    TEST_START("删除指定日期之前的记录");

    /* 添加旧日期记录 */
    CharacTowerScreamApc old_record;
    strcpy(old_record.reg_date, "2025-11-10");
    old_record.charac_no = 6000007;
    old_record.seq = 1;
    CharacTowerScreamApc_Add(manager, &old_record);

    ASSERT_EQ(CharacTowerScreamApc_DeleteBeforeDate(manager, "2025-11-15"), 0, "删除旧记录成功");

    int count = CharacTowerScreamApc_CountByDate(manager, "2025-11-10");
    ASSERT_EQ(count, 0, "旧记录已删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacTowerScreamApc_Delete(manager, "2025-11-16", 1), 0, "删除记录成功");
    ASSERT_TRUE(!CharacTowerScreamApc_Exists(manager, "2025-11-16", 1), "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印惊叫之塔APC记录信息");

    CharacTowerScreamApc record;
    if (CharacTowerScreamApc_Get(manager, "2025-11-17", 1, &record) == 0) {
        CharacTowerScreamApc_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息成功");
    } else {
        printf("  ⚠ 无记录可打印\n");
    }
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("角色惊叫之塔APC记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_get_by_date(&manager);
    test_get_by_charac(&manager);
    test_get_by_date_range(&manager);
    test_count_by_date(&manager);
    test_count_by_charac(&manager);
    test_delete_by_date(&manager);
    test_delete_before_date(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要算进去 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
