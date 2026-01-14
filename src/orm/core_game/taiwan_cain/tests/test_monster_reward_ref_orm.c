#include "monster_reward_ref_orm.h"
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
    TEST_START("添加并获取怪物奖励配置");

    MonsterRewardRef record, retrieved;
    memset(&record, 0, sizeof(MonsterRewardRef));
    memset(&retrieved, 0, sizeof(MonsterRewardRef));

    record.level = 10;
    record.exp = 1000;

    int ret = MonsterRewardRef_Add(manager, &record);
    ASSERT(ret == 0, "添加怪物奖励配置成功");

    ret = MonsterRewardRef_Get(manager, 10, &retrieved);
    ASSERT(ret == 0, "获取怪物奖励配置成功");
    ASSERT_EQ(retrieved.level, 10, "等级正确");
    ASSERT_EQ(retrieved.exp, 1000, "经验值正确");

    MonsterRewardRef_Delete(manager, 10);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新怪物奖励配置");

    MonsterRewardRef record, retrieved;
    memset(&record, 0, sizeof(MonsterRewardRef));

    record.level = 20;
    record.exp = 2000;
    MonsterRewardRef_Add(manager, &record);

    record.exp = 2500;
    int ret = MonsterRewardRef_Update(manager, &record);
    ASSERT(ret == 0, "更新怪物奖励配置成功");

    MonsterRewardRef_Get(manager, 20, &retrieved);
    ASSERT_EQ(retrieved.exp, 2500, "更新后经验值正确");

    MonsterRewardRef_Delete(manager, 20);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除怪物奖励配置");

    MonsterRewardRef record;
    memset(&record, 0, sizeof(MonsterRewardRef));

    record.level = 30;
    record.exp = 3000;
    MonsterRewardRef_Add(manager, &record);

    int ret = MonsterRewardRef_Delete(manager, 30);
    ASSERT(ret == 0, "删除怪物奖励配置成功");

    ret = MonsterRewardRef_Exists(manager, 30);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查怪物奖励配置是否存在");

    MonsterRewardRef record;
    memset(&record, 0, sizeof(MonsterRewardRef));

    int ret = MonsterRewardRef_Exists(manager, 40);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.level = 40;
    record.exp = 4000;
    MonsterRewardRef_Add(manager, &record);

    ret = MonsterRewardRef_Exists(manager, 40);
    ASSERT(ret == 1, "存在的记录返回1");

    MonsterRewardRef_Delete(manager, 40);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    MonsterRewardRef record;
    short levels[] = {51, 52, 53};
    int i;

    int initial_count = MonsterRewardRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(MonsterRewardRef));
    for (i = 0; i < 3; i++) {
        record.level = levels[i];
        record.exp = levels[i] * 100;
        MonsterRewardRef_Add(manager, &record);
    }

    int new_count = MonsterRewardRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        MonsterRewardRef_Delete(manager, levels[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    MonsterRewardRef record;
    MonsterRewardRef records[10];
    short levels[] = {61, 62, 63};
    int i;

    memset(&record, 0, sizeof(MonsterRewardRef));
    for (i = 0; i < 3; i++) {
        record.level = levels[i];
        record.exp = levels[i] * 100;
        MonsterRewardRef_Add(manager, &record);
    }

    int count = MonsterRewardRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        MonsterRewardRef_Delete(manager, levels[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    MonsterRewardRef record;
    short levels[] = {71, 72, 73, 74, 75};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(MonsterRewardRef));
    for (i = 0; i < count; i++) {
        record.level = levels[i];
        record.exp = levels[i] * 100;
        MonsterRewardRef_Add(manager, &record);
    }

    int ret = MonsterRewardRef_BatchDelete(manager, levels, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = MonsterRewardRef_Exists(manager, levels[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    MonsterRewardRef record, retrieved;
    memset(&record, 0, sizeof(MonsterRewardRef));

    record.level = 80;
    record.exp = 8000;
    int ret = MonsterRewardRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.exp = 8500;
    ret = MonsterRewardRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    MonsterRewardRef_Get(manager, 80, &retrieved);
    ASSERT_EQ(retrieved.exp, 8500, "数据为最新数据");

    int count_before = MonsterRewardRef_Count(manager);
    record.exp = 9000;
    MonsterRewardRef_Add(manager, &record);
    int count_after = MonsterRewardRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    MonsterRewardRef_Delete(manager, 80);
    TEST_PASS();
}

void test_level_range(DBConnectionManager* manager)
{
    TEST_START("等级范围测试");

    MonsterRewardRef record, retrieved;
    memset(&record, 0, sizeof(MonsterRewardRef));

    record.level = 1;
    record.exp = 10;
    int ret = MonsterRewardRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最低等级成功");

    record.level = 999;
    record.exp = 999999;
    ret = MonsterRewardRef_Add(manager, &record);
    ASSERT(ret == 0, "添加最高等级成功");

    MonsterRewardRef_Get(manager, 1, &retrieved);
    ASSERT_EQ(retrieved.exp, 10, "最低等级经验值正确");

    MonsterRewardRef_Get(manager, 999, &retrieved);
    ASSERT_EQ(retrieved.exp, 999999, "最高等级经验值正确");

    MonsterRewardRef_Delete(manager, 1);
    MonsterRewardRef_Delete(manager, 999);
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
    printf("monster_reward_ref ORM 测试\n");
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
