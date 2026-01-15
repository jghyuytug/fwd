#include "dnf_restrict_state_orm.h"
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfRestrictState state, retrieved;
    DnfRestrictState states[10];
    int actual_count;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    printf("初始化数据库连接...\n");
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "数据库管理器初始化失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "连接 d_taiwan 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("数据库连接成功\n");
    printf("========================================\n");

    /* 准备测试数据 */
    memset(&state, 0, sizeof(DnfRestrictState));
    state.server_group = 999;
    state.category = 1;
    state.restrict_code = 100;
    state.restrict_value = 'Y';
    strcpy(state.mod_date, "2025-11-17 10:00:00");
    strcpy(state.reg_date, "2025-11-17 10:00:00");

    /* TEST 1: 添加新限制状态 */
    TEST_START("添加新限制状态");
    ASSERT_EQ(DnfRestrictState_Add(&manager, &state), 0, "添加限制状态成功");

    /* TEST 2: 验证限制状态存在 */
    TEST_START("验证限制状态存在");
    int exists = DnfRestrictState_Exists(&manager, state.server_group, state.category, state.restrict_code);
    ASSERT_EQ(exists, 1, "限制状态存在");

    /* TEST 3: 获取限制状态信息 */
    TEST_START("获取限制状态信息");
    memset(&retrieved, 0, sizeof(DnfRestrictState));
    ASSERT_EQ(DnfRestrictState_Get(&manager, state.server_group, state.category,
                                     state.restrict_code, &retrieved), 0, "获取限制状态成功");
    ASSERT_EQ(retrieved.server_group, state.server_group, "服务器组匹配");
    ASSERT_EQ(retrieved.category, state.category, "分类匹配");
    ASSERT_EQ(retrieved.restrict_code, state.restrict_code, "限制代码匹配");
    ASSERT_EQ(retrieved.restrict_value, state.restrict_value, "限制值匹配");
    ASSERT_STR_EQ(retrieved.mod_date, state.mod_date, "修改日期匹配");
    ASSERT_STR_EQ(retrieved.reg_date, state.reg_date, "注册日期匹配");

    /* TEST 4: 更新限制状态 */
    TEST_START("更新限制状态");
    state.restrict_value = 'N';
    strcpy(state.mod_date, "2025-11-17 11:00:00");
    ASSERT_EQ(DnfRestrictState_Update(&manager, &state), 0, "更新限制状态成功");

    memset(&retrieved, 0, sizeof(DnfRestrictState));
    DnfRestrictState_Get(&manager, state.server_group, state.category, state.restrict_code, &retrieved);
    ASSERT_EQ(retrieved.restrict_value, 'N', "限制值已更新为 'N'");
    ASSERT_STR_EQ(retrieved.mod_date, "2025-11-17 11:00:00", "修改日期已更新");

    /* TEST 5: 设置限制值 */
    TEST_START("设置限制值");
    ASSERT_EQ(DnfRestrictState_SetRestrictValue(&manager, state.server_group, state.category,
                                                  state.restrict_code, 'Y'), 0, "设置限制值成功");

    memset(&retrieved, 0, sizeof(DnfRestrictState));
    DnfRestrictState_Get(&manager, state.server_group, state.category, state.restrict_code, &retrieved);
    ASSERT_EQ(retrieved.restrict_value, 'Y', "限制值已修改为 'Y'");

    /* TEST 6: 添加多个限制状态（测试批量查询） */
    TEST_START("添加多个限制状态");
    DnfRestrictState state2, state3, state4;

    memset(&state2, 0, sizeof(DnfRestrictState));
    state2.server_group = 999;
    state2.category = 1;
    state2.restrict_code = 101;
    state2.restrict_value = 'Y';
    strcpy(state2.mod_date, "2025-11-17 11:00:00");
    strcpy(state2.reg_date, "2025-11-17 11:00:00");
    ASSERT_EQ(DnfRestrictState_Add(&manager, &state2), 0, "添加第2个限制状态");

    memset(&state3, 0, sizeof(DnfRestrictState));
    state3.server_group = 999;
    state3.category = 2;
    state3.restrict_code = 100;
    state3.restrict_value = 'N';
    strcpy(state3.mod_date, "2025-11-17 12:00:00");
    strcpy(state3.reg_date, "2025-11-17 12:00:00");
    ASSERT_EQ(DnfRestrictState_Add(&manager, &state3), 0, "添加第3个限制状态");

    memset(&state4, 0, sizeof(DnfRestrictState));
    state4.server_group = 998;
    state4.category = 1;
    state4.restrict_code = 100;
    state4.restrict_value = 'Y';
    strcpy(state4.mod_date, "2025-11-17 13:00:00");
    strcpy(state4.reg_date, "2025-11-17 13:00:00");
    ASSERT_EQ(DnfRestrictState_Add(&manager, &state4), 0, "添加第4个限制状态");

    /* TEST 7: 按服务器组查询 */
    TEST_START("按服务器组查询");
    memset(states, 0, sizeof(states));
    actual_count = 0;
    ASSERT_EQ(DnfRestrictState_GetByServerGroup(&manager, 999, states, 10, &actual_count), 0,
              "查询服务器组999成功");
    ASSERT_EQ(actual_count, 3, "查询到3个限制状态");
    printf("  查询到 %d 个限制状态\n", actual_count);

    /* TEST 8: 按分类查询 */
    TEST_START("按分类查询");
    memset(states, 0, sizeof(states));
    actual_count = 0;
    ASSERT_EQ(DnfRestrictState_GetByCategory(&manager, 1, states, 10, &actual_count), 0,
              "查询分类1成功");
    ASSERT_TRUE(actual_count >= 3, "至少查询到3个限制状态");
    printf("  查询到 %d 个限制状态\n", actual_count);

    /* TEST 9: 按服务器组和分类查询 */
    TEST_START("按服务器组和分类查询");
    memset(states, 0, sizeof(states));
    actual_count = 0;
    ASSERT_EQ(DnfRestrictState_GetByServerGroupAndCategory(&manager, 999, 1, states, 10, &actual_count), 0,
              "查询服务器组999分类1成功");
    ASSERT_EQ(actual_count, 2, "查询到2个限制状态");
    printf("  查询到 %d 个限制状态\n", actual_count);

    /* TEST 10: 统计服务器组限制状态数量 */
    TEST_START("统计服务器组限制状态数量");
    int count = DnfRestrictState_CountByServerGroup(&manager, 999);
    ASSERT_EQ(count, 3, "服务器组999有3个限制状态");
    printf("  服务器组999共有 %d 个限制状态\n", count);

    /* TEST 11: 打印限制状态信息 */
    TEST_START("打印限制状态信息");
    printf("  调用 DnfRestrictState_PrintInfo:\n");
    DnfRestrictState_PrintInfo(&state);
    ASSERT_TRUE(1, "打印限制状态信息成功");

    /* TEST 12: 删除单个限制状态 */
    TEST_START("删除单个限制状态");
    ASSERT_EQ(DnfRestrictState_Delete(&manager, 999, 2, 100), 0, "删除限制状态成功");
    exists = DnfRestrictState_Exists(&manager, 999, 2, 100);
    ASSERT_EQ(exists, 0, "限制状态已不存在");

    /* TEST 13: 按服务器组批量删除 */
    TEST_START("按服务器组批量删除");
    ASSERT_EQ(DnfRestrictState_DeleteByServerGroup(&manager, 998), 0, "删除服务器组998的所有限制状态成功");
    count = DnfRestrictState_CountByServerGroup(&manager, 998);
    ASSERT_EQ(count, 0, "服务器组998的限制状态数量为0");

    /* TEST 14: 边界测试 - NULL 参数 */
    TEST_START("边界测试 - NULL 参数");
    ASSERT_EQ(DnfRestrictState_Add(NULL, &state), -1, "NULL manager 返回 -1");
    ASSERT_EQ(DnfRestrictState_Add(&manager, NULL), -1, "NULL state 返回 -1");
    ASSERT_EQ(DnfRestrictState_Get(&manager, 1, 1, 1, NULL), -1, "NULL 输出参数返回 -1");

    /* TEST 15: 边界测试 - 不存在的记录 */
    TEST_START("边界测试 - 不存在的记录");
    memset(&retrieved, 0, sizeof(DnfRestrictState));
    ASSERT_EQ(DnfRestrictState_Get(&manager, 9999, 9999, 9999, &retrieved), -1,
              "获取不存在的记录返回 -1");
    exists = DnfRestrictState_Exists(&manager, 9999, 9999, 9999);
    ASSERT_EQ(exists, 0, "不存在的记录 Exists 返回 0");

    /* 清理测试数据 */
    printf("\n清理测试数据...\n");
    DnfRestrictState_Delete(&manager, 999, 1, 100);
    DnfRestrictState_Delete(&manager, 999, 1, 101);
    DnfRestrictState_Delete(&manager, 999, 2, 100);
    DnfRestrictState_Delete(&manager, 998, 1, 100);

    /* 清理资源 */
    DBConnectionManager_Cleanup(&manager);

    /* 统计最后一个测试用例 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n", test_passed, test_count,
           (test_count > 0 ? (test_passed * 100.0 / test_count) : 0));
    printf("断言: %d/%d 通过 (%.1f%%)\n", assertion_passed, assertion_count,
           (assertion_count > 0 ? (assertion_passed * 100.0 / assertion_count) : 0));
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
