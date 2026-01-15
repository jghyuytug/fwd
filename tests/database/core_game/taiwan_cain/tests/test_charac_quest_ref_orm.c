#include "charac_quest_ref_orm.h"
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
    TEST_START("添加并获取任务映射");

    CharacQuestRef record, retrieved;
    memset(&record, 0, sizeof(CharacQuestRef));
    memset(&retrieved, 0, sizeof(CharacQuestRef));

    record.origin_idx = 1001;
    record.mapped_idx = 2001;

    int ret = CharacQuestRef_Add(manager, &record);
    ASSERT(ret == 0, "添加任务映射成功");

    ret = CharacQuestRef_Get(manager, 1001, &retrieved);
    ASSERT(ret == 0, "获取任务映射成功");
    ASSERT_EQ(retrieved.origin_idx, 1001, "原始任务ID正确");
    ASSERT_EQ(retrieved.mapped_idx, 2001, "映射任务ID正确");

    CharacQuestRef_Delete(manager, 1001);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新任务映射");

    CharacQuestRef record, retrieved;
    memset(&record, 0, sizeof(CharacQuestRef));

    record.origin_idx = 1002;
    record.mapped_idx = 2002;
    CharacQuestRef_Add(manager, &record);

    record.mapped_idx = 2099;
    int ret = CharacQuestRef_Update(manager, &record);
    ASSERT(ret == 0, "更新任务映射成功");

    CharacQuestRef_Get(manager, 1002, &retrieved);
    ASSERT_EQ(retrieved.mapped_idx, 2099, "更新后映射任务ID正确");

    CharacQuestRef_Delete(manager, 1002);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除任务映射");

    CharacQuestRef record;
    memset(&record, 0, sizeof(CharacQuestRef));

    record.origin_idx = 1003;
    record.mapped_idx = 2003;
    CharacQuestRef_Add(manager, &record);

    int ret = CharacQuestRef_Delete(manager, 1003);
    ASSERT(ret == 0, "删除任务映射成功");

    ret = CharacQuestRef_Exists(manager, 1003);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查任务映射是否存在");

    CharacQuestRef record;
    memset(&record, 0, sizeof(CharacQuestRef));

    int ret = CharacQuestRef_Exists(manager, 1004);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.origin_idx = 1004;
    record.mapped_idx = 2004;
    CharacQuestRef_Add(manager, &record);

    ret = CharacQuestRef_Exists(manager, 1004);
    ASSERT(ret == 1, "存在的记录返回1");

    CharacQuestRef_Delete(manager, 1004);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    CharacQuestRef record;
    int origin_idxs[] = {1011, 1012, 1013};
    int i;

    int initial_count = CharacQuestRef_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(CharacQuestRef));
    for (i = 0; i < 3; i++) {
        record.origin_idx = origin_idxs[i];
        record.mapped_idx = 2000 + origin_idxs[i];
        CharacQuestRef_Add(manager, &record);
    }

    int new_count = CharacQuestRef_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        CharacQuestRef_Delete(manager, origin_idxs[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    CharacQuestRef record;
    CharacQuestRef records[10];
    int origin_idxs[] = {1021, 1022, 1023};
    int i;

    memset(&record, 0, sizeof(CharacQuestRef));
    for (i = 0; i < 3; i++) {
        record.origin_idx = origin_idxs[i];
        record.mapped_idx = 2000 + origin_idxs[i];
        CharacQuestRef_Add(manager, &record);
    }

    int count = CharacQuestRef_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        CharacQuestRef_Delete(manager, origin_idxs[i]);
    }

    TEST_PASS();
}

void test_get_by_mapped_idx(DBConnectionManager* manager)
{
    TEST_START("根据映射ID反向查找");

    CharacQuestRef record, retrieved;
    memset(&record, 0, sizeof(CharacQuestRef));

    record.origin_idx = 1031;
    record.mapped_idx = 2031;
    CharacQuestRef_Add(manager, &record);

    int ret = CharacQuestRef_GetByMappedIdx(manager, 2031, &retrieved);
    ASSERT(ret == 0, "根据映射ID查找成功");
    ASSERT_EQ(retrieved.origin_idx, 1031, "查找到的原始ID正确");
    ASSERT_EQ(retrieved.mapped_idx, 2031, "查找到的映射ID正确");

    CharacQuestRef_Delete(manager, 1031);
    TEST_PASS();
}

void test_mapped_idx_exists(DBConnectionManager* manager)
{
    TEST_START("检查映射ID是否存在");

    CharacQuestRef record;
    memset(&record, 0, sizeof(CharacQuestRef));

    int ret = CharacQuestRef_MappedIdxExists(manager, 2041);
    ASSERT(ret == 0, "不存在的映射ID返回0");

    record.origin_idx = 1041;
    record.mapped_idx = 2041;
    CharacQuestRef_Add(manager, &record);

    ret = CharacQuestRef_MappedIdxExists(manager, 2041);
    ASSERT(ret == 1, "存在的映射ID返回1");

    CharacQuestRef_Delete(manager, 1041);
    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    CharacQuestRef record;
    int origin_idxs[] = {1051, 1052, 1053, 1054, 1055};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(CharacQuestRef));
    for (i = 0; i < count; i++) {
        record.origin_idx = origin_idxs[i];
        record.mapped_idx = 2000 + origin_idxs[i];
        CharacQuestRef_Add(manager, &record);
    }

    int ret = CharacQuestRef_BatchDelete(manager, origin_idxs, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = CharacQuestRef_Exists(manager, origin_idxs[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    CharacQuestRef record, retrieved;
    memset(&record, 0, sizeof(CharacQuestRef));

    record.origin_idx = 1061;
    record.mapped_idx = 2061;
    int ret = CharacQuestRef_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.mapped_idx = 2099;
    ret = CharacQuestRef_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    CharacQuestRef_Get(manager, 1061, &retrieved);
    ASSERT_EQ(retrieved.mapped_idx, 2099, "数据为最新数据");

    int count_before = CharacQuestRef_Count(manager);
    record.mapped_idx = 2100;
    CharacQuestRef_Add(manager, &record);
    int count_after = CharacQuestRef_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    CharacQuestRef_Delete(manager, 1061);
    TEST_PASS();
}

void test_unique_constraint(DBConnectionManager* manager)
{
    TEST_START("唯一索引约束测试");

    CharacQuestRef record1, record2;
    memset(&record1, 0, sizeof(CharacQuestRef));
    memset(&record2, 0, sizeof(CharacQuestRef));

    // 添加第一条记录
    record1.origin_idx = 1071;
    record1.mapped_idx = 2071;
    int ret = CharacQuestRef_Add(manager, &record1);
    ASSERT(ret == 0, "添加第一条记录成功");

    // 尝试添加相同映射ID的记录（应该失败或替换）
    record2.origin_idx = 1072;
    record2.mapped_idx = 2071;  // 相同的映射ID
    ret = CharacQuestRef_Add(manager, &record2);
    // 注意：由于mapped_idx有唯一索引，REPLACE INTO会删除旧记录并插入新记录

    // 验证只保留了最新的记录
    CharacQuestRef retrieved;
    ret = CharacQuestRef_Exists(manager, 1071);
    ASSERT(ret == 0, "旧记录已被替换");

    ret = CharacQuestRef_Get(manager, 1072, &retrieved);
    ASSERT(ret == 0, "新记录存在");
    ASSERT_EQ(retrieved.mapped_idx, 2071, "映射ID正确");

    CharacQuestRef_Delete(manager, 1072);
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
    printf("charac_quest_ref ORM 测试\n");
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
    test_get_by_mapped_idx(&manager);
    test_mapped_idx_exists(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_unique_constraint(&manager);

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
