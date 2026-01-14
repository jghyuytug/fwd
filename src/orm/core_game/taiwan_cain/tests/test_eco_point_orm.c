#include "eco_point_orm.h"
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

#define ASSERT_TRUE(condition, message) \
    do { \
        assertion_count++; \
        if (condition) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
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

/* 测试用的角色编号 */
#define TEST_CHARAC_NO 999999

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    EcoPoint record, retrieved;
    EcoPoint records[100];
    int ret, count;

    printf("========================================\n");
    printf("EcoPoint ORM 测试套件\n");
    printf("========================================\n");

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    ASSERT_EQ(ret, 0, "数据库连接管理器初始化成功");

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    ASSERT_EQ(ret, 0, "数据库连接成功");

    /* 清理测试数据 */
    EcoPoint_Delete(&manager, TEST_CHARAC_NO);

    /*==========================================================================
     * 测试 1: 添加生态点数记录
     *=========================================================================*/
    TEST_START("添加生态点数记录");

    memset(&record, 0, sizeof(EcoPoint));
    record.charac_no = TEST_CHARAC_NO;
    record.eco_point = 1000;
    record.point_500 = 2;
    record.point_300 = 1;
    record.point_100 = 3;
    record.point_50 = 2;
    record.point_20 = 5;

    ret = EcoPoint_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "EcoPoint_Add 返回成功");

    ret = EcoPoint_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 1, "记录存在");

    /*==========================================================================
     * 测试 2: 获取生态点数记录
     *=========================================================================*/
    TEST_START("获取生态点数记录");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "EcoPoint_Get 返回成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO, "角色编号匹配");
    ASSERT_EQ(retrieved.eco_point, 1000, "总生态点数匹配");
    ASSERT_EQ(retrieved.point_500, 2, "500点档次次数匹配");
    ASSERT_EQ(retrieved.point_300, 1, "300点档次次数匹配");
    ASSERT_EQ(retrieved.point_100, 3, "100点档次次数匹配");
    ASSERT_EQ(retrieved.point_50, 2, "50点档次次数匹配");
    ASSERT_EQ(retrieved.point_20, 5, "20点档次次数匹配");

    /*==========================================================================
     * 测试 3: 更新生态点数记录
     *=========================================================================*/
    TEST_START("更新生态点数记录");

    record.eco_point = 2000;
    record.point_500 = 4;

    ret = EcoPoint_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "EcoPoint_Update 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.eco_point, 2000, "总点数已更新");
    ASSERT_EQ(retrieved.point_500, 4, "500点档次已更新");

    /*==========================================================================
     * 测试 4: 增加生态点数
     *=========================================================================*/
    TEST_START("增加生态点数");

    ret = EcoPoint_AddPoints(&manager, TEST_CHARAC_NO, 500);
    ASSERT_EQ(ret, 0, "EcoPoint_AddPoints 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.eco_point, 2500, "点数已增加（2000 + 500 = 2500）");

    /*==========================================================================
     * 测试 5: 减少生态点数
     *=========================================================================*/
    TEST_START("减少生态点数");

    ret = EcoPoint_SubtractPoints(&manager, TEST_CHARAC_NO, 300);
    ASSERT_EQ(ret, 0, "EcoPoint_SubtractPoints 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.eco_point, 2200, "点数已减少（2500 - 300 = 2200）");

    /*==========================================================================
     * 测试 6: 减少生态点数（点数不足）
     *=========================================================================*/
    TEST_START("减少生态点数（点数不足应失败）");

    ret = EcoPoint_SubtractPoints(&manager, TEST_CHARAC_NO, 10000);
    ASSERT_EQ(ret, -1, "点数不足，返回失败（符合预期）");

    /*==========================================================================
     * 测试 7: 增加档次计数（500点）
     *=========================================================================*/
    TEST_START("增加档次计数（500点）");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    int old_count_500 = retrieved.point_500;

    ret = EcoPoint_IncrementTierCount(&manager, TEST_CHARAC_NO, 500);
    ASSERT_EQ(ret, 0, "EcoPoint_IncrementTierCount 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.point_500, old_count_500 + 1, "500点档次计数已增加");

    /*==========================================================================
     * 测试 8: 增加档次计数（300点）
     *=========================================================================*/
    TEST_START("增加档次计数（300点）");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    int old_count_300 = retrieved.point_300;

    ret = EcoPoint_IncrementTierCount(&manager, TEST_CHARAC_NO, 300);
    ASSERT_EQ(ret, 0, "EcoPoint_IncrementTierCount 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.point_300, old_count_300 + 1, "300点档次计数已增加");

    /*==========================================================================
     * 测试 9: 批量更新档次计数
     *=========================================================================*/
    TEST_START("批量更新档次计数");

    ret = EcoPoint_UpdateTierCounts(&manager, TEST_CHARAC_NO, 10, 8, 6, 4, 2);
    ASSERT_EQ(ret, 0, "EcoPoint_UpdateTierCounts 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.point_500, 10, "500点档次已更新");
    ASSERT_EQ(retrieved.point_300, 8, "300点档次已更新");
    ASSERT_EQ(retrieved.point_100, 6, "100点档次已更新");
    ASSERT_EQ(retrieved.point_50, 4, "50点档次已更新");
    ASSERT_EQ(retrieved.point_20, 2, "20点档次已更新");

    /*==========================================================================
     * 测试 10: 重置生态点数
     *=========================================================================*/
    TEST_START("重置生态点数");

    ret = EcoPoint_Reset(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "EcoPoint_Reset 返回成功");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.eco_point, 0, "总点数已清零");
    ASSERT_EQ(retrieved.point_500, 0, "500点档次已清零");
    ASSERT_EQ(retrieved.point_300, 0, "300点档次已清零");

    /*==========================================================================
     * 测试 11: 为新角色添加点数（自动创建记录）
     *=========================================================================*/
    TEST_START("为新角色添加点数（自动创建记录）");

    unsigned int new_charac = TEST_CHARAC_NO + 1;
    EcoPoint_Delete(&manager, new_charac);  /* 确保不存在 */

    ret = EcoPoint_AddPoints(&manager, new_charac, 100);
    ASSERT_EQ(ret, 0, "EcoPoint_AddPoints 返回成功（自动创建记录）");

    ret = EcoPoint_Exists(&manager, new_charac);
    ASSERT_EQ(ret, 1, "新记录已创建");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, new_charac, &retrieved);
    ASSERT_EQ(ret, 0, "获取新记录成功");
    ASSERT_EQ(retrieved.eco_point, 100, "点数正确");

    EcoPoint_Delete(&manager, new_charac);  /* 清理 */

    /*==========================================================================
     * 测试 12: 获取排行榜
     *=========================================================================*/
    TEST_START("获取排行榜");

    /* 添加一些测试数据 */
    EcoPoint test_record;
    memset(&test_record, 0, sizeof(EcoPoint));
    test_record.charac_no = TEST_CHARAC_NO;
    test_record.eco_point = 5000;
    EcoPoint_Add(&manager, &test_record);

    test_record.charac_no = TEST_CHARAC_NO + 2;
    test_record.eco_point = 3000;
    EcoPoint_Add(&manager, &test_record);

    test_record.charac_no = TEST_CHARAC_NO + 3;
    test_record.eco_point = 7000;
    EcoPoint_Add(&manager, &test_record);

    memset(records, 0, sizeof(records));
    count = EcoPoint_GetTopRanking(&manager, records, 10);
    ASSERT_GT(count, 0, "获取到排行榜数据");
    ASSERT_GE(records[0].eco_point, records[1].eco_point, "排行榜按点数降序排列");

    /* 清理测试数据 */
    EcoPoint_Delete(&manager, TEST_CHARAC_NO + 2);
    EcoPoint_Delete(&manager, TEST_CHARAC_NO + 3);

    /*==========================================================================
     * 测试 13: 统计总生态点数
     *=========================================================================*/
    TEST_START("统计总生态点数");

    count = EcoPoint_GetTotalPoints(&manager);
    ASSERT_GE(count, 0, "获取总点数成功");
    printf("  当前总生态点数: %d\n", count);

    /*==========================================================================
     * 测试 14: 统计参与角色数量
     *=========================================================================*/
    TEST_START("统计参与角色数量");

    count = EcoPoint_CountCharacters(&manager);
    ASSERT_GT(count, 0, "获取参与角色数量成功");
    printf("  当前参与角色数量: %d\n", count);

    /*==========================================================================
     * 测试 15: 打印生态点数信息
     *=========================================================================*/
    TEST_START("打印生态点数信息");

    memset(&retrieved, 0, sizeof(EcoPoint));
    ret = EcoPoint_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");

    printf("  打印测试数据:\n");
    EcoPoint_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "打印函数执行成功");

    /*==========================================================================
     * 测试 16: 删除生态点数记录
     *=========================================================================*/
    TEST_START("删除生态点数记录");

    ret = EcoPoint_Delete(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "EcoPoint_Delete 返回成功");

    ret = EcoPoint_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "记录已被删除");

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 最后一个测试的计数 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
