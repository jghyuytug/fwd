#include "charac_dimension_inout_20130613_orm.h"
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

/* 测试数据常量 */
#define TEST_CHARAC_NO_1 100001
#define TEST_CHARAC_NO_2 100002
#define TEST_CHARAC_NO_3 100003
#define TEST_CHARAC_NO_NONEXIST 999999

/* 辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    CharacDimensionInout20130613_Delete(manager, TEST_CHARAC_NO_1);
    CharacDimensionInout20130613_Delete(manager, TEST_CHARAC_NO_2);
    CharacDimensionInout20130613_Delete(manager, TEST_CHARAC_NO_3);
}

/* ==================== 测试用例 ==================== */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("测试添加和查询记录");

    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    record.charac_no = TEST_CHARAC_NO_1;
    record.dungeon1 = 1;
    record.dungeon2 = 0;
    record.dungeon3 = 1;
    record.dungeon4 = 0;
    record.dungeon5 = 1;
    record.dungeon6 = 0;
    record.dungeon7 = 1;
    record.dungeon8 = 0;
    record.dungeon9 = 1;
    record.dungeon10 = 0;

    int ret = CharacDimensionInout20130613_Add(manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    CharacDimensionInout20130613 retrieved;
    memset(&retrieved, 0, sizeof(CharacDimensionInout20130613));
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_1, &retrieved);
    ASSERT_EQ(ret, 0, "查询记录成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO_1, "角色编号匹配");
    ASSERT_EQ(retrieved.dungeon1, 1, "dungeon1状态匹配");
    ASSERT_EQ(retrieved.dungeon2, 0, "dungeon2状态匹配");
    ASSERT_EQ(retrieved.dungeon5, 1, "dungeon5状态匹配");
    ASSERT_EQ(retrieved.dungeon10, 0, "dungeon10状态匹配");

    CharacDimensionInout20130613_PrintInfo(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("测试更新记录");

    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    record.charac_no = TEST_CHARAC_NO_1;

    /* 查询现有记录 */
    int ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_1, &record);
    ASSERT_EQ(ret, 0, "查询记录成功");

    /* 修改字段 */
    record.dungeon2 = 1;  /* 改为已进入 */
    record.dungeon4 = 1;  /* 改为已进入 */

    ret = CharacDimensionInout20130613_Update(manager, &record);
    ASSERT_EQ(ret, 0, "更新记录成功");

    /* 验证更新 */
    CharacDimensionInout20130613 updated;
    memset(&updated, 0, sizeof(CharacDimensionInout20130613));
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_1, &updated);
    ASSERT_EQ(ret, 0, "查询更新后记录成功");
    ASSERT_EQ(updated.dungeon2, 1, "dungeon2已更新为1");
    ASSERT_EQ(updated.dungeon4, 1, "dungeon4已更新为1");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("测试记录存在性检查");

    int ret = CharacDimensionInout20130613_Exists(manager, TEST_CHARAC_NO_1);
    ASSERT_EQ(ret, 1, "已存在的记录返回1");

    ret = CharacDimensionInout20130613_Exists(manager, TEST_CHARAC_NO_NONEXIST);
    ASSERT_EQ(ret, 0, "不存在的记录返回0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("测试删除记录");

    int ret = CharacDimensionInout20130613_Delete(manager, TEST_CHARAC_NO_1);
    ASSERT_EQ(ret, 0, "删除记录成功");

    ret = CharacDimensionInout20130613_Exists(manager, TEST_CHARAC_NO_1);
    ASSERT_EQ(ret, 0, "记录已不存在");
}

void test_set_dungeon_status(DBConnectionManager* manager) {
    TEST_START("测试设置单个副本状态");

    /* 创建测试记录 */
    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    record.charac_no = TEST_CHARAC_NO_2;
    CharacDimensionInout20130613_Add(manager, &record);

    /* 设置副本5为已进入 */
    int ret = CharacDimensionInout20130613_SetDungeonStatus(manager, TEST_CHARAC_NO_2, 5, DUNGEON_STATUS_ENTERED);
    ASSERT_EQ(ret, 0, "设置副本5状态成功");

    /* 验证状态 */
    char status = -1;
    ret = CharacDimensionInout20130613_GetDungeonStatus(manager, TEST_CHARAC_NO_2, 5, &status);
    ASSERT_EQ(ret, 0, "查询副本5状态成功");
    ASSERT_EQ(status, DUNGEON_STATUS_ENTERED, "副本5状态为已进入");

    /* 设置副本3为已进入 */
    ret = CharacDimensionInout20130613_SetDungeonStatus(manager, TEST_CHARAC_NO_2, 3, DUNGEON_STATUS_ENTERED);
    ASSERT_EQ(ret, 0, "设置副本3状态成功");

    ret = CharacDimensionInout20130613_GetDungeonStatus(manager, TEST_CHARAC_NO_2, 3, &status);
    ASSERT_EQ(ret, 0, "查询副本3状态成功");
    ASSERT_EQ(status, DUNGEON_STATUS_ENTERED, "副本3状态为已进入");
}

void test_set_all_dungeons(DBConnectionManager* manager) {
    TEST_START("测试设置所有副本状态");

    /* 设置所有副本为已进入 */
    int ret = CharacDimensionInout20130613_SetAllDungeons(manager, TEST_CHARAC_NO_2, DUNGEON_STATUS_ENTERED);
    ASSERT_EQ(ret, 0, "设置所有副本为已进入成功");

    /* 验证 */
    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_2, &record);
    ASSERT_EQ(ret, 0, "查询记录成功");
    ASSERT_EQ(record.dungeon1, 1, "dungeon1=1");
    ASSERT_EQ(record.dungeon5, 1, "dungeon5=1");
    ASSERT_EQ(record.dungeon10, 1, "dungeon10=1");

    CharacDimensionInout20130613_PrintInfo(&record);
}

void test_clear_all_dungeons(DBConnectionManager* manager) {
    TEST_START("测试清空所有副本状态");

    /* 清空所有副本 */
    int ret = CharacDimensionInout20130613_ClearAllDungeons(manager, TEST_CHARAC_NO_2);
    ASSERT_EQ(ret, 0, "清空所有副本成功");

    /* 验证 */
    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_2, &record);
    ASSERT_EQ(ret, 0, "查询记录成功");
    ASSERT_EQ(record.dungeon1, 0, "dungeon1=0");
    ASSERT_EQ(record.dungeon5, 0, "dungeon5=0");
    ASSERT_EQ(record.dungeon10, 0, "dungeon10=0");
}

void test_get_completed_count(DBConnectionManager* manager) {
    TEST_START("测试统计已完成副本数量");

    /* 设置部分副本为已进入 */
    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    record.charac_no = TEST_CHARAC_NO_3;
    record.dungeon1 = 1;
    record.dungeon3 = 1;
    record.dungeon5 = 1;
    record.dungeon7 = 1;
    record.dungeon9 = 1;
    CharacDimensionInout20130613_Add(manager, &record);

    /* 统计已完成数量 */
    int count = 0;
    int ret = CharacDimensionInout20130613_GetCompletedCount(manager, TEST_CHARAC_NO_3, &count);
    ASSERT_EQ(ret, 0, "统计成功");
    ASSERT_EQ(count, 5, "已完成5个副本");
}

void test_count_by_completed_dungeons(DBConnectionManager* manager) {
    TEST_START("测试按已完成副本数量统计角色数");

    /* 创建多个测试角色 */
    CharacDimensionInout20130613 r1, r2, r3;
    memset(&r1, 0, sizeof(CharacDimensionInout20130613));
    memset(&r2, 0, sizeof(CharacDimensionInout20130613));
    memset(&r3, 0, sizeof(CharacDimensionInout20130613));

    /* 角色1: 完成5个副本（重复使用TEST_CHARAC_NO_3，已有5个副本） */
    /* 角色2: 完成10个副本 */
    r2.charac_no = TEST_CHARAC_NO_2;
    CharacDimensionInout20130613_SetAllDungeons(manager, TEST_CHARAC_NO_2, DUNGEON_STATUS_ENTERED);

    /* 角色3: 完成0个副本 */
    r3.charac_no = TEST_CHARAC_NO_1;
    r3.dungeon1 = 0;
    r3.dungeon2 = 0;
    r3.dungeon3 = 0;
    r3.dungeon4 = 0;
    r3.dungeon5 = 0;
    r3.dungeon6 = 0;
    r3.dungeon7 = 0;
    r3.dungeon8 = 0;
    r3.dungeon9 = 0;
    r3.dungeon10 = 0;
    CharacDimensionInout20130613_Add(manager, &r3);

    /* 统计 */
    int count = CharacDimensionInout20130613_CountByCompletedDungeons(manager, 5);
    ASSERT_EQ(count, 1, "完成5个副本的角色数为1");

    count = CharacDimensionInout20130613_CountByCompletedDungeons(manager, 10);
    ASSERT_EQ(count, 1, "完成10个副本的角色数为1");

    count = CharacDimensionInout20130613_CountByCompletedDungeons(manager, 0);
    ASSERT_EQ(count, 1, "完成0个副本的角色数为1");
}

void test_count_by_dungeon_status(DBConnectionManager* manager) {
    TEST_START("测试按指定副本状态统计角色数");

    /* 统计dungeon1为已进入的角色数 */
    int count = CharacDimensionInout20130613_CountByDungeonStatus(manager, 1, DUNGEON_STATUS_ENTERED);
    ASSERT_GE(count, 2, "dungeon1已进入的角色数>=2（TEST_CHARAC_NO_2和3）");

    /* 统计dungeon2为已进入的角色数 */
    count = CharacDimensionInout20130613_CountByDungeonStatus(manager, 2, DUNGEON_STATUS_ENTERED);
    ASSERT_GE(count, 1, "dungeon2已进入的角色数>=1（TEST_CHARAC_NO_2）");
}

void test_count_total(DBConnectionManager* manager) {
    TEST_START("测试统计总记录数");

    int count = CharacDimensionInout20130613_CountTotal(manager);
    ASSERT_GE(count, 3, "总记录数>=3");
    printf("  当前总记录数: %d\n", count);
}

void test_count_fully_completed(DBConnectionManager* manager) {
    TEST_START("测试统计完成所有副本的角色数");

    int count = CharacDimensionInout20130613_CountFullyCompleted(manager);
    ASSERT_EQ(count, 1, "完成所有副本的角色数为1（TEST_CHARAC_NO_2）");
}

void test_replace_into_behavior(DBConnectionManager* manager) {
    TEST_START("测试REPLACE INTO行为（幂等性）");

    CharacDimensionInout20130613 record;
    memset(&record, 0, sizeof(CharacDimensionInout20130613));
    record.charac_no = TEST_CHARAC_NO_1;
    record.dungeon1 = 1;
    record.dungeon2 = 1;

    /* 第一次添加 */
    int ret = CharacDimensionInout20130613_Add(manager, &record);
    ASSERT_EQ(ret, 0, "第一次添加成功");

    /* 修改数据并再次添加 */
    record.dungeon3 = 1;
    record.dungeon4 = 1;
    ret = CharacDimensionInout20130613_Add(manager, &record);
    ASSERT_EQ(ret, 0, "第二次添加成功（REPLACE）");

    /* 验证数据被更新 */
    CharacDimensionInout20130613 retrieved;
    memset(&retrieved, 0, sizeof(CharacDimensionInout20130613));
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_1, &retrieved);
    ASSERT_EQ(ret, 0, "查询记录成功");
    ASSERT_EQ(retrieved.dungeon3, 1, "dungeon3已更新为1");
    ASSERT_EQ(retrieved.dungeon4, 1, "dungeon4已更新为1");
}

void test_boundary_values(DBConnectionManager* manager) {
    TEST_START("测试边界值");

    /* 测试无效副本索引 */
    char status = 0;
    int ret = CharacDimensionInout20130613_GetDungeonStatus(manager, TEST_CHARAC_NO_1, 0, &status);
    ASSERT_EQ(ret, -1, "副本索引0无效，返回-1");

    ret = CharacDimensionInout20130613_GetDungeonStatus(manager, TEST_CHARAC_NO_1, 11, &status);
    ASSERT_EQ(ret, -1, "副本索引11无效，返回-1");

    ret = CharacDimensionInout20130613_SetDungeonStatus(manager, TEST_CHARAC_NO_1, -1, 1);
    ASSERT_EQ(ret, -1, "副本索引-1无效，返回-1");

    /* 测试无效completed_count */
    ret = CharacDimensionInout20130613_CountByCompletedDungeons(manager, -1);
    ASSERT_EQ(ret, -1, "completed_count=-1无效，返回-1");

    ret = CharacDimensionInout20130613_CountByCompletedDungeons(manager, 11);
    ASSERT_EQ(ret, -1, "completed_count=11无效，返回-1");
}

void test_null_pointer_checks(DBConnectionManager* manager) {
    TEST_START("测试NULL指针检查");

    int ret = CharacDimensionInout20130613_Add(NULL, NULL);
    ASSERT_EQ(ret, -1, "manager=NULL时返回-1");

    CharacDimensionInout20130613 record;
    ret = CharacDimensionInout20130613_Get(manager, TEST_CHARAC_NO_1, NULL);
    ASSERT_EQ(ret, -1, "record=NULL时返回-1");

    ret = CharacDimensionInout20130613_GetDungeonStatus(manager, TEST_CHARAC_NO_1, 1, NULL);
    ASSERT_EQ(ret, -1, "status=NULL时返回-1");

    ret = CharacDimensionInout20130613_GetCompletedCount(manager, TEST_CHARAC_NO_1, NULL);
    ASSERT_EQ(ret, -1, "count=NULL时返回-1");
}

/* ==================== 主函数 ==================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        printf("用法: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("charac_dimension_inout_20130613_orm 测试套件\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("连接数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_delete(&manager);
    test_set_dungeon_status(&manager);
    test_set_all_dungeons(&manager);
    test_clear_all_dungeons(&manager);
    test_get_completed_count(&manager);
    test_count_by_completed_dungeons(&manager);
    test_count_by_dungeon_status(&manager);
    test_count_total(&manager);
    test_count_fully_completed(&manager);
    test_replace_into_behavior(&manager);
    test_boundary_values(&manager);
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
