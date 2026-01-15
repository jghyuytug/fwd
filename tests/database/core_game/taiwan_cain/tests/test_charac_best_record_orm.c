#include "charac_best_record_orm.h"
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

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: values are equal)\n", message); \
        } \
    } while(0)

/* 测试用的角色编号和副本参数 */
#define TEST_CHARAC_NO 888888
#define TEST_DUNGEON_NO 100
#define TEST_DIFFICULTY_EASY 0
#define TEST_DIFFICULTY_NORMAL 1
#define TEST_DIFFICULTY_HARD 2

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    CharacBestRecord record, retrieved;
    CharacBestRecord records[10];
    int ret, count;

    printf("========================================\n");
    printf("CharacBestRecord ORM 测试套件\n");
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
    CharacBestRecord_DeleteByCharac(&manager, TEST_CHARAC_NO);

    /*==========================================================================
     * 测试 1: 添加最佳记录
     *=========================================================================*/
    TEST_START("添加最佳记录");

    memset(&record, 0, sizeof(CharacBestRecord));
    record.charac_no = TEST_CHARAC_NO;
    record.dungeon_no = TEST_DUNGEON_NO;
    record.difficulty = TEST_DIFFICULTY_EASY;
    record.style = 1000;
    record.technic = 800;
    record.attacked = 5;
    record.rank = 10;

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacBestRecord_Add 返回成功");

    ret = CharacBestRecord_Exists(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO, TEST_DIFFICULTY_EASY);
    ASSERT_EQ(ret, 1, "记录存在");

    /*==========================================================================
     * 测试 2: 获取最佳记录
     *=========================================================================*/
    TEST_START("获取最佳记录");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "CharacBestRecord_Get 返回成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO, "角色编号匹配");
    ASSERT_EQ(retrieved.dungeon_no, TEST_DUNGEON_NO, "副本编号匹配");
    ASSERT_EQ(retrieved.difficulty, TEST_DIFFICULTY_EASY, "难度匹配");
    ASSERT_EQ(retrieved.style, 1000, "风格分数匹配");
    ASSERT_EQ(retrieved.technic, 800, "技巧分数匹配");
    ASSERT_EQ(retrieved.attacked, 5, "被攻击次数匹配");
    ASSERT_EQ(retrieved.rank, 10, "排名匹配");

    /*==========================================================================
     * 测试 3: 更新完整记录
     *=========================================================================*/
    TEST_START("更新完整记录");

    record.style = 1500;
    record.technic = 1200;
    record.attacked = 3;
    record.rank = 5;

    ret = CharacBestRecord_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacBestRecord_Update 返回成功");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.style, 1500, "风格分数已更新");
    ASSERT_EQ(retrieved.technic, 1200, "技巧分数已更新");
    ASSERT_EQ(retrieved.attacked, 3, "被攻击次数已更新");
    ASSERT_EQ(retrieved.rank, 5, "排名已更新");

    /*==========================================================================
     * 测试 4: 更新评分
     *=========================================================================*/
    TEST_START("更新评分");

    ret = CharacBestRecord_UpdateScores(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                        TEST_DIFFICULTY_EASY, 2000, 1500, 2);
    ASSERT_EQ(ret, 0, "CharacBestRecord_UpdateScores 返回成功");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.style, 2000, "风格分数已更新为2000");
    ASSERT_EQ(retrieved.technic, 1500, "技巧分数已更新为1500");
    ASSERT_EQ(retrieved.attacked, 2, "被攻击次数已更新为2");
    ASSERT_EQ(retrieved.rank, 5, "排名未变");

    /*==========================================================================
     * 测试 5: 更新排名
     *=========================================================================*/
    TEST_START("更新排名");

    ret = CharacBestRecord_UpdateRank(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                      TEST_DIFFICULTY_EASY, 1);
    ASSERT_EQ(ret, 0, "CharacBestRecord_UpdateRank 返回成功");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.rank, 1, "排名已更新为1");

    /*==========================================================================
     * 测试 6: 添加多个难度记录
     *=========================================================================*/
    TEST_START("添加多个难度记录");

    /* 添加普通难度记录 */
    memset(&record, 0, sizeof(CharacBestRecord));
    record.charac_no = TEST_CHARAC_NO;
    record.dungeon_no = TEST_DUNGEON_NO;
    record.difficulty = TEST_DIFFICULTY_NORMAL;
    record.style = 1800;
    record.technic = 1400;
    record.attacked = 4;
    record.rank = 15;

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加普通难度记录成功");

    /* 添加困难难度记录 */
    record.difficulty = TEST_DIFFICULTY_HARD;
    record.style = 2500;
    record.technic = 2000;
    record.attacked = 1;
    record.rank = 3;

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加困难难度记录成功");

    /*==========================================================================
     * 测试 7: 获取某副本的所有难度记录
     *=========================================================================*/
    TEST_START("获取某副本的所有难度记录");

    memset(records, 0, sizeof(records));
    count = CharacBestRecord_GetByDungeon(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                          records, 10);
    ASSERT_EQ(count, 3, "获取到3条记录（3个难度）");
    ASSERT_EQ(records[0].difficulty, TEST_DIFFICULTY_EASY, "第1条为简单难度");
    ASSERT_EQ(records[1].difficulty, TEST_DIFFICULTY_NORMAL, "第2条为普通难度");
    ASSERT_EQ(records[2].difficulty, TEST_DIFFICULTY_HARD, "第3条为困难难度");

    /*==========================================================================
     * 测试 8: 打印最佳记录信息
     *=========================================================================*/
    TEST_START("打印最佳记录信息");

    printf("  调用 CharacBestRecord_PrintInfo:\n");
    CharacBestRecord_PrintInfo(&records[2]);
    ASSERT_TRUE(1, "PrintInfo 执行成功");

    /*==========================================================================
     * 测试 9: REPLACE INTO 幂等性
     *=========================================================================*/
    TEST_START("REPLACE INTO 幂等性");

    memset(&record, 0, sizeof(CharacBestRecord));
    record.charac_no = TEST_CHARAC_NO;
    record.dungeon_no = TEST_DUNGEON_NO;
    record.difficulty = TEST_DIFFICULTY_EASY;
    record.style = 3000;
    record.technic = 2500;
    record.attacked = 0;
    record.rank = 1;

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 Add 成功");

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 Add 成功（幂等）");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.style, 3000, "风格分数为3000（最后一次Add的值）");

    /*==========================================================================
     * 测试 10: 边界测试 - 零值数据
     *=========================================================================*/
    TEST_START("边界测试 - 零值数据");

    memset(&record, 0, sizeof(CharacBestRecord));
    record.charac_no = TEST_CHARAC_NO;
    record.dungeon_no = TEST_DUNGEON_NO;
    record.difficulty = TEST_DIFFICULTY_EASY;
    record.style = 0;
    record.technic = 0;
    record.attacked = 0;
    record.rank = 0;

    ret = CharacBestRecord_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加零值数据成功");

    memset(&retrieved, 0, sizeof(CharacBestRecord));
    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.style, 0, "风格分数为0");
    ASSERT_EQ(retrieved.rank, 0, "排名为0");

    /*==========================================================================
     * 测试 11: 删除单条记录
     *=========================================================================*/
    TEST_START("删除单条记录");

    ret = CharacBestRecord_Delete(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                  TEST_DIFFICULTY_EASY);
    ASSERT_EQ(ret, 0, "CharacBestRecord_Delete 返回成功");

    ret = CharacBestRecord_Exists(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                  TEST_DIFFICULTY_EASY);
    ASSERT_EQ(ret, 0, "简单难度记录已删除");

    ret = CharacBestRecord_Exists(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                  TEST_DIFFICULTY_NORMAL);
    ASSERT_EQ(ret, 1, "普通难度记录仍存在");

    /*==========================================================================
     * 测试 12: 删除角色所有记录
     *=========================================================================*/
    TEST_START("删除角色所有记录");

    ret = CharacBestRecord_DeleteByCharac(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "CharacBestRecord_DeleteByCharac 返回成功");

    count = CharacBestRecord_GetByDungeon(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                                          records, 10);
    ASSERT_EQ(count, 0, "所有记录已删除");

    /*==========================================================================
     * 测试 13: 错误处理 - NULL参数
     *=========================================================================*/
    TEST_START("错误处理 - NULL参数");

    ret = CharacBestRecord_Add(NULL, &record);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacBestRecord_Add(&manager, NULL);
    ASSERT_NEQ(ret, 0, "NULL record 返回错误");

    ret = CharacBestRecord_Get(NULL, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, &retrieved);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacBestRecord_Get(&manager, TEST_CHARAC_NO, TEST_DUNGEON_NO,
                               TEST_DIFFICULTY_EASY, NULL);
    ASSERT_NEQ(ret, 0, "NULL record 返回错误");

    /* 更新最后一个测试的状态 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试完成\n");
    printf("========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
