#include "quest_category_orm.h"
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        g_assert_count++; \
        if (strcmp(a, b) == 0) { \
            g_assert_passed++; \
            printf("  ✓ %s ('%s' == '%s')\n", message, a, b); \
        } else { \
            printf("  ✗ FAILED: %s ('%s' != '%s')\n", message, a, b); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取任务分类");

    QuestCategory record, retrieved;
    memset(&record, 0, sizeof(QuestCategory));
    memset(&retrieved, 0, sizeof(QuestCategory));

    record.quest_idx = 99991;
    strcpy(record.quest_name, "测试任务分类1");

    int ret = QuestCategory_Add(manager, &record);
    ASSERT(ret == 0, "添加任务分类成功");

    ret = QuestCategory_Get(manager, 99991, &retrieved);
    ASSERT(ret == 0, "获取任务分类成功");
    ASSERT_EQ(retrieved.quest_idx, 99991, "任务索引正确");
    ASSERT_STR_EQ(retrieved.quest_name, "测试任务分类1", "任务名称正确");

    QuestCategory_Delete(manager, 99991);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新任务分类");

    QuestCategory record, retrieved;
    memset(&record, 0, sizeof(QuestCategory));

    record.quest_idx = 99992;
    strcpy(record.quest_name, "原始名称");
    QuestCategory_Add(manager, &record);

    strcpy(record.quest_name, "更新后名称");
    int ret = QuestCategory_Update(manager, &record);
    ASSERT(ret == 0, "更新任务分类成功");

    QuestCategory_Get(manager, 99992, &retrieved);
    ASSERT_STR_EQ(retrieved.quest_name, "更新后名称", "更新后名称正确");

    QuestCategory_Delete(manager, 99992);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除任务分类");

    QuestCategory record;
    memset(&record, 0, sizeof(QuestCategory));

    record.quest_idx = 99993;
    strcpy(record.quest_name, "待删除分类");
    QuestCategory_Add(manager, &record);

    int ret = QuestCategory_Delete(manager, 99993);
    ASSERT(ret == 0, "删除任务分类成功");

    ret = QuestCategory_Exists(manager, 99993);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查任务分类是否存在");

    QuestCategory record;
    memset(&record, 0, sizeof(QuestCategory));

    int ret = QuestCategory_Exists(manager, 99994);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.quest_idx = 99994;
    strcpy(record.quest_name, "存在性测试");
    QuestCategory_Add(manager, &record);

    ret = QuestCategory_Exists(manager, 99994);
    ASSERT(ret == 1, "存在的记录返回1");

    QuestCategory_Delete(manager, 99994);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    QuestCategory record;
    int quest_idxs[] = {99995, 99996, 99997};
    int i;

    int initial_count = QuestCategory_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(QuestCategory));
    for (i = 0; i < 3; i++) {
        record.quest_idx = quest_idxs[i];
        snprintf(record.quest_name, sizeof(record.quest_name), "测试分类%d", i + 1);
        QuestCategory_Add(manager, &record);
    }

    int new_count = QuestCategory_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        QuestCategory_Delete(manager, quest_idxs[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    QuestCategory record;
    QuestCategory records[10];
    int quest_idxs[] = {99981, 99982, 99983};
    int i;

    memset(&record, 0, sizeof(QuestCategory));
    for (i = 0; i < 3; i++) {
        record.quest_idx = quest_idxs[i];
        snprintf(record.quest_name, sizeof(record.quest_name), "批量测试%d", i + 1);
        QuestCategory_Add(manager, &record);
    }

    int count = QuestCategory_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        QuestCategory_Delete(manager, quest_idxs[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    QuestCategory record;
    int quest_idxs[] = {99971, 99972, 99973, 99974, 99975};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(QuestCategory));
    for (i = 0; i < count; i++) {
        record.quest_idx = quest_idxs[i];
        snprintf(record.quest_name, sizeof(record.quest_name), "批量删除%d", i + 1);
        QuestCategory_Add(manager, &record);
    }

    int ret = QuestCategory_BatchDelete(manager, quest_idxs, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = QuestCategory_Exists(manager, quest_idxs[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    QuestCategory record, retrieved;
    memset(&record, 0, sizeof(QuestCategory));

    record.quest_idx = 99990;
    strcpy(record.quest_name, "第一次添加");
    int ret = QuestCategory_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    strcpy(record.quest_name, "第二次添加");
    ret = QuestCategory_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    QuestCategory_Get(manager, 99990, &retrieved);
    ASSERT_STR_EQ(retrieved.quest_name, "第二次添加", "数据为最新数据");

    int count_before = QuestCategory_Count(manager);
    strcpy(record.quest_name, "第三次添加");
    QuestCategory_Add(manager, &record);
    int count_after = QuestCategory_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    QuestCategory_Delete(manager, 99990);
    TEST_PASS();
}

void test_utf8_support(DBConnectionManager* manager)
{
    TEST_START("UTF8中文支持测试");

    QuestCategory record, retrieved;
    memset(&record, 0, sizeof(QuestCategory));
    memset(&retrieved, 0, sizeof(QuestCategory));

    record.quest_idx = 99980;
    strcpy(record.quest_name, "神聖的守護者");
    int ret = QuestCategory_Add(manager, &record);
    ASSERT(ret == 0, "添加中文任务名成功");

    ret = QuestCategory_Get(manager, 99980, &retrieved);
    ASSERT(ret == 0, "获取中文任务名成功");
    ASSERT_STR_EQ(retrieved.quest_name, "神聖的守護者", "中文内容正确");

    QuestCategory_Delete(manager, 99980);
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
    printf("quest_category ORM 测试\n");
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
    test_utf8_support(&manager);

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
