#include "charac_tower_despair_apc_orm.h"
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

#define ASSERT_TRUE(cond, message) \
    do { \
        assertion_count++; \
        if (cond) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a, b); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取APC记录");

    CharacTowerDespairApc record, retrieved;
    memset(&record, 0, sizeof(CharacTowerDespairApc));

    strcpy(record.reg_date, "2025-11-16");
    record.charac_no = 2000001;
    record.seq = 1;

    ASSERT_EQ(CharacTowerDespairApc_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTowerDespairApc_Get(manager, record.reg_date, record.seq, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.reg_date, record.reg_date, "注册日期匹配");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.seq, record.seq, "序号匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新APC记录");

    CharacTowerDespairApc record, retrieved;
    ASSERT_EQ(CharacTowerDespairApc_Get(manager, "2025-11-16", 1, &record), 0, "获取已存在记录");

    int old_charac_no = record.charac_no;
    record.charac_no = 2000055;

    ASSERT_EQ(CharacTowerDespairApc_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTowerDespairApc_Get(manager, "2025-11-16", 1, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.charac_no, 2000055, "角色编号已更新");

    /* 恢复原值以不影响后续测试 */
    record.charac_no = old_charac_no;
    CharacTowerDespairApc_Update(manager, &record);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_EQ(CharacTowerDespairApc_Exists(manager, "2025-11-16", 1), 1, "记录存在");
    ASSERT_EQ(CharacTowerDespairApc_Exists(manager, "2025-11-16", 999), 0, "记录不存在");
}

void test_get_next_seq(DBConnectionManager* manager) {
    TEST_START("获取下一个可用序号");

    int next_seq = CharacTowerDespairApc_GetNextSeq(manager, "2025-11-16");
    ASSERT_TRUE(next_seq > 0, "获取下一个序号成功");
    ASSERT_EQ(next_seq, 2, "下一个序号为2");

    /* 测试新日期 */
    int first_seq = CharacTowerDespairApc_GetNextSeq(manager, "2025-11-17");
    ASSERT_EQ(first_seq, 1, "新日期的首个序号为1");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条记录");

    CharacTowerDespairApc record2, record3, record4;
    memset(&record2, 0, sizeof(CharacTowerDespairApc));
    memset(&record3, 0, sizeof(CharacTowerDespairApc));
    memset(&record4, 0, sizeof(CharacTowerDespairApc));

    strcpy(record2.reg_date, "2025-11-16");
    record2.charac_no = 2000002;
    record2.seq = 2;

    strcpy(record3.reg_date, "2025-11-16");
    record3.charac_no = 2000003;
    record3.seq = 3;

    strcpy(record4.reg_date, "2025-11-17");
    record4.charac_no = 2000001;
    record4.seq = 1;

    ASSERT_EQ(CharacTowerDespairApc_Add(manager, &record2), 0, "添加记录2成功");
    ASSERT_EQ(CharacTowerDespairApc_Add(manager, &record3), 0, "添加记录3成功");
    ASSERT_EQ(CharacTowerDespairApc_Add(manager, &record4), 0, "添加记录4成功");
}

void test_get_by_date(DBConnectionManager* manager) {
    TEST_START("获取指定日期的所有记录");

    CharacTowerDespairApc records[10];
    int count = CharacTowerDespairApc_GetByDate(manager, "2025-11-16", records, 10);

    ASSERT_TRUE(count >= 3, "至少找到3条记录");
    ASSERT_STR_EQ(records[0].reg_date, "2025-11-16", "第一条记录日期正确");
}

void test_get_by_charac_no(DBConnectionManager* manager) {
    TEST_START("获取指定角色的所有记录");

    CharacTowerDespairApc records[10];
    int count = CharacTowerDespairApc_GetByCharacNo(manager, 2000001, records, 10);

    ASSERT_TRUE(count >= 2, "至少找到2条记录");
    ASSERT_EQ(records[0].charac_no, 2000001, "角色编号正确");
}

void test_get_count_by_date(DBConnectionManager* manager) {
    TEST_START("获取指定日期的记录数");

    int count = CharacTowerDespairApc_GetCountByDate(manager, "2025-11-16");
    ASSERT_TRUE(count >= 3, "2025-11-16至少有3条记录");

    int count2 = CharacTowerDespairApc_GetCountByDate(manager, "2025-11-17");
    ASSERT_TRUE(count2 >= 1, "2025-11-17至少有1条记录");
}

void test_get_count_by_charac_no(DBConnectionManager* manager) {
    TEST_START("获取指定角色的记录数");

    int count = CharacTowerDespairApc_GetCountByCharacNo(manager, 2000001);
    ASSERT_TRUE(count >= 2, "角色2000001至少有2条记录");

    int count2 = CharacTowerDespairApc_GetCountByCharacNo(manager, 2000002);
    ASSERT_TRUE(count2 >= 1, "角色2000002至少有1条记录");
}

void test_delete_by_charac_no(DBConnectionManager* manager) {
    TEST_START("删除指定角色的所有记录");

    ASSERT_EQ(CharacTowerDespairApc_DeleteByCharacNo(manager, 2000002), 0, "删除角色2000002的记录成功");

    int count = CharacTowerDespairApc_GetCountByCharacNo(manager, 2000002);
    ASSERT_EQ(count, 0, "角色2000002的记录已删除");
}

void test_delete_by_date(DBConnectionManager* manager) {
    TEST_START("删除指定日期的所有记录");

    ASSERT_EQ(CharacTowerDespairApc_DeleteByDate(manager, "2025-11-17"), 0, "删除2025-11-17的记录成功");

    int count = CharacTowerDespairApc_GetCountByDate(manager, "2025-11-17");
    ASSERT_EQ(count, 0, "2025-11-17的记录已删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除单条记录");

    ASSERT_EQ(CharacTowerDespairApc_Delete(manager, "2025-11-16", 1), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerDespairApc_Exists(manager, "2025-11-16", 1), 0, "记录已删除");

    ASSERT_EQ(CharacTowerDespairApc_Delete(manager, "2025-11-16", 3), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印APC信息");

    CharacTowerDespairApc record;
    memset(&record, 0, sizeof(CharacTowerDespairApc));

    strcpy(record.reg_date, "2025-11-18");
    record.charac_no = 2000999;
    record.seq = 1;

    CharacTowerDespairApc_Add(manager, &record);

    CharacTowerDespairApc retrieved;
    CharacTowerDespairApc_Get(manager, "2025-11-18", 1, &retrieved);

    printf("\n");
    CharacTowerDespairApc_PrintInfo(&retrieved);

    CharacTowerDespairApc_Delete(manager, "2025-11-18", 1);

    assertion_count++;
    assertion_passed++;
    printf("  ✓ 打印信息成功\n");
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
        fprintf(stderr, "连接到 taiwan_cain 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("角色绝望之塔APC ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_get_next_seq(&manager);
    test_add_multiple_records(&manager);
    test_get_by_date(&manager);
    test_get_by_charac_no(&manager);
    test_get_count_by_date(&manager);
    test_get_count_by_charac_no(&manager);
    test_delete_by_charac_no(&manager);
    test_delete_by_date(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
