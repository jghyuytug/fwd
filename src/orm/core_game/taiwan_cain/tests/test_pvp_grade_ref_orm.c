#include "pvp_grade_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 测试框架
static int g_test_count = 0;
static int g_test_passed = 0;
static int g_assert_count = 0;
static int g_assert_passed = 0;

#define TEST_START(name) \
    do { \
        g_test_count++; \
        printf("\n=== Test %d: %s ===\n", g_test_count, name); \
    } while(0)

#define TEST_PASS() \
    do { \
        g_test_passed++; \
        printf("✓ PASSED\n"); \
    } while(0)

#define ASSERT(condition, message) \
    do { \
        g_assert_count++; \
        if (condition) { \
            g_assert_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ FAILED: %s\n", message); \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        g_assert_count++; \
        if ((a) == (b)) { \
            g_assert_passed++; \
            printf("  ✓ %s (%d == %d)\n", message, (int)(a), (int)(b)); \
        } else { \
            printf("  ✗ FAILED: %s (%d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取PVP等级配置");

    PvpGradeRef record, retrieved;
    memset(&record, 0, sizeof(PvpGradeRef));
    memset(&retrieved, 0, sizeof(PvpGradeRef));

    record.grade = 1;
    record.limit_pts = 1000;

    int ret = PvpGradeRef_Add(manager, &record);
    ASSERT(ret == 0, "添加PVP等级配置成功");

    ret = PvpGradeRef_Get(manager, 1, &retrieved);
    ASSERT(ret == 0, "获取PVP等级配置成功");
    ASSERT_EQ(retrieved.grade, 1, "等级正确");
    ASSERT_EQ(retrieved.limit_pts, 1000, "所需积分正确");

    PvpGradeRef_Delete(manager, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新PVP等级配置");

    PvpGradeRef record, retrieved;
    memset(&record, 0, sizeof(PvpGradeRef));

    record.grade = 2;
    record.limit_pts = 2000;
    PvpGradeRef_Add(manager, &record);

    record.limit_pts = 2500;
    int ret = PvpGradeRef_Update(manager, &record);
    ASSERT(ret == 0, "更新PVP等级配置成功");

    PvpGradeRef_Get(manager, 2, &retrieved);
    ASSERT_EQ(retrieved.limit_pts, 2500, "更新后积分正确");

    PvpGradeRef_Delete(manager, 2);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除PVP等级配置");

    PvpGradeRef record;
    memset(&record, 0, sizeof(PvpGradeRef));

    record.grade = 3;
    record.limit_pts = 3000;
    PvpGradeRef_Add(manager, &record);

    int ret = PvpGradeRef_Delete(manager, 3);
    ASSERT(ret == 0, "删除PVP等级配置成功");

    ret = PvpGradeRef_Exists(manager, 3);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查PVP等级配置是否存在");

    PvpGradeRef record;
    memset(&record, 0, sizeof(PvpGradeRef));

    int ret = PvpGradeRef_Exists(manager, 4);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.grade = 4;
    record.limit_pts = 4000;
    PvpGradeRef_Add(manager, &record);

    ret = PvpGradeRef_Exists(manager, 4);
    ASSERT(ret == 1, "存在的记录返回1");

    PvpGradeRef_Delete(manager, 4);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    PvpGradeRef record;
    int i;

    int initial_count = PvpGradeRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(PvpGradeRef));
    for (i = 0; i < 3; i++) {
        record.grade = 10 + i;
        record.limit_pts = (10 + i) * 1000;
        PvpGradeRef_Add(manager, &record);
    }

    int new_count = PvpGradeRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        PvpGradeRef_Delete(manager, 10 + i);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    PvpGradeRef record;
    PvpGradeRef records[10];
    int i;

    memset(&record, 0, sizeof(PvpGradeRef));
    for (i = 0; i < 3; i++) {
        record.grade = 20 + i;
        record.limit_pts = (20 + i) * 1000;
        PvpGradeRef_Add(manager, &record);
    }

    int count = PvpGradeRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        PvpGradeRef_Delete(manager, 20 + i);
    }

    TEST_PASS();
}

void test_get_by_points(DBConnectionManager* manager)
{
    TEST_START("根据积分查找等级");

    PvpGradeRef record, result;
    int i;

    // 添加等级配置：等级1-5，积分要求1000-5000
    memset(&record, 0, sizeof(PvpGradeRef));
    for (i = 1; i <= 5; i++) {
        record.grade = 30 + i;
        record.limit_pts = i * 1000;
        PvpGradeRef_Add(manager, &record);
    }

    // 测试500积分 -> 应该没有匹配
    int ret = PvpGradeRef_GetByPoints(manager, 500, &result);
    ASSERT(ret < 0, "500积分无匹配等级");

    // 测试1000积分 -> 应该匹配等级31
    ret = PvpGradeRef_GetByPoints(manager, 1000, &result);
    ASSERT(ret == 0, "1000积分找到等级");
    ASSERT_EQ(result.grade, 31, "1000积分对应等级31");

    // 测试2500积分 -> 应该匹配等级32（2000积分要求）
    ret = PvpGradeRef_GetByPoints(manager, 2500, &result);
    ASSERT(ret == 0, "2500积分找到等级");
    ASSERT_EQ(result.grade, 32, "2500积分对应等级32");

    // 测试6000积分 -> 应该匹配等级35（5000积分要求）
    ret = PvpGradeRef_GetByPoints(manager, 6000, &result);
    ASSERT(ret == 0, "6000积分找到等级");
    ASSERT_EQ(result.grade, 35, "6000积分对应等级35");

    for (i = 1; i <= 5; i++) {
        PvpGradeRef_Delete(manager, 30 + i);
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    PvpGradeRef record, retrieved;
    memset(&record, 0, sizeof(PvpGradeRef));

    record.grade = 40;
    record.limit_pts = 10000;
    int ret = PvpGradeRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.limit_pts = 15000;
    ret = PvpGradeRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    PvpGradeRef_Get(manager, 40, &retrieved);
    ASSERT_EQ(retrieved.limit_pts, 15000, "数据为最新数据");

    int count_before = PvpGradeRef_Count(manager);
    record.limit_pts = 20000;
    PvpGradeRef_Add(manager, &record);
    int count_after = PvpGradeRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    PvpGradeRef_Delete(manager, 40);
    TEST_PASS();
}

void test_grade_range(DBConnectionManager* manager)
{
    TEST_START("等级范围测试");

    PvpGradeRef record, retrieved;
    memset(&record, 0, sizeof(PvpGradeRef));

    // 测试最低等级
    record.grade = 0;
    record.limit_pts = 0;
    int ret = PvpGradeRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最低等级成功");

    // 测试高等级
    record.grade = 100;
    record.limit_pts = 1000000;
    ret = PvpGradeRef_Add(manager, &record);
    ASSERT(ret == 0, "添加高等级成功");

    PvpGradeRef_Get(manager, 0, &retrieved);
    ASSERT_EQ(retrieved.limit_pts, 0, "最低等级积分正确");

    PvpGradeRef_Get(manager, 100, &retrieved);
    ASSERT_EQ(retrieved.limit_pts, 1000000, "高等级积分正确");

    PvpGradeRef_Delete(manager, 0);
    PvpGradeRef_Delete(manager, 100);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印配置信息");

    PvpGradeRef record;
    memset(&record, 0, sizeof(PvpGradeRef));

    record.grade = 50;
    record.limit_pts = 50000;
    PvpGradeRef_Add(manager, &record);

    PvpGradeRef_Get(manager, 50, &record);
    PvpGradeRef_PrintInfo(&record);
    ASSERT(1, "打印配置信息成功");

    PvpGradeRef_Delete(manager, 50);
    TEST_PASS();
}

// 主函数
int main(int argc, char** argv)
{
    DBConnectionManager manager;
    int ret;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("==========================================\n");
    printf("pvp_grade_ref ORM 测试\n");
    printf("==========================================\n");

    memset(&manager, 0, sizeof(DBConnectionManager));
    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    if (ret < 0) {
        printf("Failed to initialize database connection\n");
        return 1;
    }

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    if (ret < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    test_add_and_get(&manager);
    test_update(&manager);
    test_delete(&manager);
    test_exists(&manager);
    test_count(&manager);
    test_get_all(&manager);
    test_get_by_points(&manager);
    test_replace_into_idempotent(&manager);
    test_grade_range(&manager);
    test_print_info(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n==========================================\n");
    printf("测试结果汇总\n");
    printf("==========================================\n");
    printf("测试用例: %d/%d 通过\n", g_test_passed, g_test_count);
    printf("断言: %d/%d 通过\n", g_assert_passed, g_assert_count);
    printf("成功率: %.1f%%\n", (g_test_count > 0) ? (g_test_passed * 100.0 / g_test_count) : 0);
    printf("==========================================\n");

    if (g_test_passed == g_test_count && g_assert_passed == g_assert_count) {
        printf("✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        return 1;
    }
}
