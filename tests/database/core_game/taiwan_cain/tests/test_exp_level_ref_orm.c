#include "exp_level_ref_orm.h"
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
            printf("  ✓ %s (%u == %u)\n", message, (unsigned int)(a), (unsigned int)(b)); \
        } else { \
            printf("  ✗ FAILED: %s (%u != %u)\n", message, (unsigned int)(a), (unsigned int)(b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取经验等级配置");

    ExpLevelRef record, retrieved;
    memset(&record, 0, sizeof(ExpLevelRef));
    memset(&retrieved, 0, sizeof(ExpLevelRef));

    record.lev = 1;
    record.exp = 100;

    int ret = ExpLevelRef_Add(manager, &record);
    ASSERT(ret == 0, "添加经验等级配置成功");

    ret = ExpLevelRef_Get(manager, 1, &retrieved);
    ASSERT(ret == 0, "获取经验等级配置成功");
    ASSERT_EQ(retrieved.lev, 1, "等级正确");
    ASSERT_EQ(retrieved.exp, 100, "经验值正确");

    ExpLevelRef_Delete(manager, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新经验等级配置");

    ExpLevelRef record, retrieved;
    memset(&record, 0, sizeof(ExpLevelRef));

    record.lev = 2;
    record.exp = 200;
    ExpLevelRef_Add(manager, &record);

    record.exp = 250;
    int ret = ExpLevelRef_Update(manager, &record);
    ASSERT(ret == 0, "更新经验等级配置成功");

    ExpLevelRef_Get(manager, 2, &retrieved);
    ASSERT_EQ(retrieved.exp, 250, "更新后经验值正确");

    ExpLevelRef_Delete(manager, 2);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除经验等级配置");

    ExpLevelRef record;
    memset(&record, 0, sizeof(ExpLevelRef));

    record.lev = 3;
    record.exp = 300;
    ExpLevelRef_Add(manager, &record);

    int ret = ExpLevelRef_Delete(manager, 3);
    ASSERT(ret == 0, "删除经验等级配置成功");

    ret = ExpLevelRef_Exists(manager, 3);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查经验等级配置是否存在");

    ExpLevelRef record;
    memset(&record, 0, sizeof(ExpLevelRef));

    int ret = ExpLevelRef_Exists(manager, 4);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.lev = 4;
    record.exp = 400;
    ExpLevelRef_Add(manager, &record);

    ret = ExpLevelRef_Exists(manager, 4);
    ASSERT(ret == 1, "存在的记录返回1");

    ExpLevelRef_Delete(manager, 4);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    ExpLevelRef record;
    unsigned int levs[] = {11, 12, 13};
    int i;

    int initial_count = ExpLevelRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(ExpLevelRef));
    for (i = 0; i < 3; i++) {
        record.lev = levs[i];
        record.exp = levs[i] * 100;
        ExpLevelRef_Add(manager, &record);
    }

    int new_count = ExpLevelRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        ExpLevelRef_Delete(manager, levs[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    ExpLevelRef record;
    ExpLevelRef records[10];
    unsigned int levs[] = {21, 22, 23};
    int i;

    memset(&record, 0, sizeof(ExpLevelRef));
    for (i = 0; i < 3; i++) {
        record.lev = levs[i];
        record.exp = levs[i] * 100;
        ExpLevelRef_Add(manager, &record);
    }

    int count = ExpLevelRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        ExpLevelRef_Delete(manager, levs[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    ExpLevelRef record;
    unsigned int levs[] = {31, 32, 33, 34, 35};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(ExpLevelRef));
    for (i = 0; i < count; i++) {
        record.lev = levs[i];
        record.exp = levs[i] * 100;
        ExpLevelRef_Add(manager, &record);
    }

    int ret = ExpLevelRef_BatchDelete(manager, levs, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = ExpLevelRef_Exists(manager, levs[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    ExpLevelRef record, retrieved;
    memset(&record, 0, sizeof(ExpLevelRef));

    record.lev = 50;
    record.exp = 5000;
    int ret = ExpLevelRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.exp = 5500;
    ret = ExpLevelRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    ExpLevelRef_Get(manager, 50, &retrieved);
    ASSERT_EQ(retrieved.exp, 5500, "数据为最新数据");

    int count_before = ExpLevelRef_Count(manager);
    record.exp = 6000;
    ExpLevelRef_Add(manager, &record);
    int count_after = ExpLevelRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    ExpLevelRef_Delete(manager, 50);
    TEST_PASS();
}

void test_level_range(DBConnectionManager* manager)
{
    TEST_START("等级范围测试");

    ExpLevelRef record, retrieved;
    memset(&record, 0, sizeof(ExpLevelRef));

    record.lev = 1;
    record.exp = 0;
    int ret = ExpLevelRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最低等级成功");

    record.lev = 999;
    record.exp = 999999999;
    ret = ExpLevelRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最高等级成功");

    ExpLevelRef_Get(manager, 1, &retrieved);
    ASSERT_EQ(retrieved.exp, 0, "最低等级经验值正确");

    ExpLevelRef_Get(manager, 999, &retrieved);
    ASSERT_EQ(retrieved.exp, 999999999, "最高等级经验值正确");

    ExpLevelRef_Delete(manager, 1);
    ExpLevelRef_Delete(manager, 999);
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
    printf("exp_level_ref ORM 测试\n");
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
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_level_range(&manager);

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
