#include "single_rank_avg_orm.h"
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
            printf("  ✗ %s (FAILED: %lld != %lld)\n", message, (long long)(a), (long long)(b)); \
        } \
    } while(0)

#define ASSERT_NE(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %lld == %lld)\n", message, (long long)(a), (long long)(b)); \
        } \
    } while(0)

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    SingleRankAvg record, retrieved;
    int result;
    long long total;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* ========== 测试用例 ========== */

    TEST_START("Add - 添加单人排名统计");
    memset(&record, 0, sizeof(SingleRankAvg));
    record.dungeon_index = 1;
    record.level = 50;
    record.job = 0;  /* 鬼剑士 */
    record.clear_count = 100;
    record.average = 12345;
    result = SingleRankAvg_Add(&manager, &record);
    ASSERT_EQ(result, 0, "添加成功");

    TEST_START("Exists - 检查记录是否存在");
    result = SingleRankAvg_Exists(&manager, 1, 50, 0);
    ASSERT_EQ(result, 1, "记录存在");

    TEST_START("Get - 获取单人排名统计");
    memset(&retrieved, 0, sizeof(SingleRankAvg));
    result = SingleRankAvg_Get(&manager, 1, 50, 0, &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_EQ(retrieved.dungeon_index, 1, "副本索引正确");
    ASSERT_EQ(retrieved.level, 50, "等级正确");
    ASSERT_EQ(retrieved.job, 0, "职业正确");
    ASSERT_EQ(retrieved.clear_count, 100, "通关次数正确");
    ASSERT_EQ(retrieved.average, 12345, "平均值正确");

    TEST_START("Update - 更新单人排名统计");
    record.clear_count = 200;
    record.average = 23456;
    result = SingleRankAvg_Update(&manager, &record);
    ASSERT_EQ(result, 0, "更新成功");
    result = SingleRankAvg_Get(&manager, 1, 50, 0, &retrieved);
    ASSERT_EQ(retrieved.clear_count, 200, "更新后通关次数正确");
    ASSERT_EQ(retrieved.average, 23456, "更新后平均值正确");

    TEST_START("Add - REPLACE INTO幂等性测试");
    record.clear_count = 300;
    record.average = 34567;
    result = SingleRankAvg_Add(&manager, &record);
    ASSERT_EQ(result, 0, "重复添加成功（REPLACE INTO）");

    TEST_START("IncrementClearCount - 增加通关次数");
    result = SingleRankAvg_IncrementClearCount(&manager, 1, 50, 0, 50);
    ASSERT_EQ(result, 0, "增加通关次数成功");
    result = SingleRankAvg_Get(&manager, 1, 50, 0, &retrieved);
    ASSERT_EQ(retrieved.clear_count, 350, "通关次数增加正确（300+50=350）");

    TEST_START("UpdateAverage - 更新平均值");
    result = SingleRankAvg_UpdateAverage(&manager, 1, 50, 0, 45678);
    ASSERT_EQ(result, 0, "更新平均值成功");
    result = SingleRankAvg_Get(&manager, 1, 50, 0, &retrieved);
    ASSERT_EQ(retrieved.average, 45678, "平均值更新正确");

    /* 添加更多测试数据 */
    memset(&record, 0, sizeof(SingleRankAvg));
    record.dungeon_index = 1;
    record.level = 60;
    record.job = 0;
    record.clear_count = 150;
    record.average = 11111;
    SingleRankAvg_Add(&manager, &record);

    record.dungeon_index = 1;
    record.level = 50;
    record.job = 1;  /* 格斗家 */
    record.clear_count = 120;
    record.average = 22222;
    SingleRankAvg_Add(&manager, &record);

    record.dungeon_index = 2;
    record.level = 50;
    record.job = 0;
    record.clear_count = 80;
    record.average = 33333;
    SingleRankAvg_Add(&manager, &record);

    TEST_START("Count - 获取所有记录总数");
    result = SingleRankAvg_Count(&manager);
    ASSERT_EQ(result, 4, "总记录数为4");

    TEST_START("CountByDungeon - 按副本统计");
    result = SingleRankAvg_CountByDungeon(&manager, 1);
    ASSERT_EQ(result, 3, "副本1有3条记录");
    result = SingleRankAvg_CountByDungeon(&manager, 2);
    ASSERT_EQ(result, 1, "副本2有1条记录");

    TEST_START("CountByDungeonAndLevel - 按副本和等级统计");
    result = SingleRankAvg_CountByDungeonAndLevel(&manager, 1, 50);
    ASSERT_EQ(result, 2, "副本1等级50有2条记录");
    result = SingleRankAvg_CountByDungeonAndLevel(&manager, 1, 60);
    ASSERT_EQ(result, 1, "副本1等级60有1条记录");

    TEST_START("GetTotalClearCountByJob - 获取指定职业总通关次数");
    total = SingleRankAvg_GetTotalClearCountByJob(&manager, 0);
    ASSERT_EQ(total, 580, "职业0总通关次数为580（350+150+80）");
    total = SingleRankAvg_GetTotalClearCountByJob(&manager, 1);
    ASSERT_EQ(total, 120, "职业1总通关次数为120");

    TEST_START("DeleteByDungeon - 删除指定副本的所有统计");
    result = SingleRankAvg_DeleteByDungeon(&manager, 2);
    ASSERT_EQ(result, 0, "删除副本2的记录成功");
    result = SingleRankAvg_Count(&manager);
    ASSERT_EQ(result, 3, "删除后总记录数为3");

    TEST_START("Delete - 删除单条记录");
    result = SingleRankAvg_Delete(&manager, 1, 60, 0);
    ASSERT_EQ(result, 0, "删除成功");
    result = SingleRankAvg_Exists(&manager, 1, 60, 0);
    ASSERT_EQ(result, 0, "删除后记录不存在");

    TEST_START("Delete - 删除不存在的记录");
    result = SingleRankAvg_Delete(&manager, 999, 999, 999);
    ASSERT_EQ(result, 0, "删除不存在记录不报错");

    TEST_START("Get - 获取不存在的记录");
    result = SingleRankAvg_Get(&manager, 999, 999, 999, &retrieved);
    ASSERT_EQ(result, -1, "获取不存在记录返回-1");

    TEST_START("Exists - 检查不存在的记录");
    result = SingleRankAvg_Exists(&manager, 999, 999, 999);
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    TEST_START("复合主键 - 同副本不同等级或职业");
    /* 添加测试数据 */
    memset(&record, 0, sizeof(SingleRankAvg));
    record.dungeon_index = 10;
    record.level = 50;
    record.job = 0;
    record.clear_count = 100;
    record.average = 10000;
    SingleRankAvg_Add(&manager, &record);

    record.dungeon_index = 10;
    record.level = 60;
    record.job = 0;
    record.clear_count = 200;
    record.average = 20000;
    SingleRankAvg_Add(&manager, &record);

    record.dungeon_index = 10;
    record.level = 50;
    record.job = 1;
    record.clear_count = 150;
    record.average = 15000;
    SingleRankAvg_Add(&manager, &record);

    result = SingleRankAvg_CountByDungeon(&manager, 10);
    ASSERT_EQ(result, 3, "同副本不同等级或职业可以共存");

    /* 最后一个测试计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
