#include "charac_action_point_desc_orm.h"
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
        if (strcmp((a), (b)) == 0) { \
            g_assert_passed++; \
            printf("  ✓ %s (\"%s\" == \"%s\")\n", message, (a), (b)); \
        } else { \
            printf("  ✗ FAILED: %s (\"%s\" != \"%s\")\n", message, (a), (b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取行动点描述");

    CharacActionPointDesc record, retrieved;
    memset(&record, 0, sizeof(CharacActionPointDesc));
    memset(&retrieved, 0, sizeof(CharacActionPointDesc));

    record.action_group_index = 1;
    record.action_index = 1;
    strcpy(record.action_group_name, "Test Action Group");

    int ret = CharacActionPointDesc_Add(manager, &record);
    ASSERT(ret == 0, "添加行动点描述成功");

    ret = CharacActionPointDesc_Get(manager, 1, 1, &retrieved);
    ASSERT(ret == 0, "获取行动点描述成功");
    ASSERT_EQ(retrieved.action_group_index, 1, "行动组索引正确");
    ASSERT_EQ(retrieved.action_index, 1, "行动索引正确");
    ASSERT_STR_EQ(retrieved.action_group_name, "Test Action Group", "行动组名称正确");

    CharacActionPointDesc_Delete(manager, 1, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新行动点描述");

    CharacActionPointDesc record, retrieved;
    memset(&record, 0, sizeof(CharacActionPointDesc));

    record.action_group_index = 2;
    record.action_index = 1;
    strcpy(record.action_group_name, "Original Name");
    CharacActionPointDesc_Add(manager, &record);

    strcpy(record.action_group_name, "Updated Name");
    int ret = CharacActionPointDesc_Update(manager, &record);
    ASSERT(ret == 0, "更新行动点描述成功");

    CharacActionPointDesc_Get(manager, 2, 1, &retrieved);
    ASSERT_STR_EQ(retrieved.action_group_name, "Updated Name", "更新后名称正确");

    CharacActionPointDesc_Delete(manager, 2, 1);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除行动点描述");

    CharacActionPointDesc record;
    memset(&record, 0, sizeof(CharacActionPointDesc));

    record.action_group_index = 3;
    record.action_index = 1;
    strcpy(record.action_group_name, "To Be Deleted");
    CharacActionPointDesc_Add(manager, &record);

    int ret = CharacActionPointDesc_Delete(manager, 3, 1);
    ASSERT(ret == 0, "删除行动点描述成功");

    ret = CharacActionPointDesc_Exists(manager, 3, 1);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查行动点描述是否存在");

    CharacActionPointDesc record;
    memset(&record, 0, sizeof(CharacActionPointDesc));

    int ret = CharacActionPointDesc_Exists(manager, 4, 1);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.action_group_index = 4;
    record.action_index = 1;
    strcpy(record.action_group_name, "Exists Test");
    CharacActionPointDesc_Add(manager, &record);

    ret = CharacActionPointDesc_Exists(manager, 4, 1);
    ASSERT(ret == 1, "存在的记录返回1");

    CharacActionPointDesc_Delete(manager, 4, 1);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    CharacActionPointDesc record;
    int i;

    int initial_count = CharacActionPointDesc_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(CharacActionPointDesc));
    for (i = 1; i <= 3; i++) {
        record.action_group_index = 10;
        record.action_index = i;
        sprintf(record.action_group_name, "Action %d", i);
        CharacActionPointDesc_Add(manager, &record);
    }

    int new_count = CharacActionPointDesc_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 1; i <= 3; i++) {
        CharacActionPointDesc_Delete(manager, 10, i);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    CharacActionPointDesc record;
    CharacActionPointDesc records[10];
    int i;

    memset(&record, 0, sizeof(CharacActionPointDesc));
    for (i = 1; i <= 3; i++) {
        record.action_group_index = 20;
        record.action_index = i;
        sprintf(record.action_group_name, "Action %d", i);
        CharacActionPointDesc_Add(manager, &record);
    }

    int count = CharacActionPointDesc_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 1; i <= 3; i++) {
        CharacActionPointDesc_Delete(manager, 20, i);
    }

    TEST_PASS();
}

void test_get_by_group_index(DBConnectionManager* manager)
{
    TEST_START("按行动组索引获取记录");

    CharacActionPointDesc record;
    CharacActionPointDesc records[10];
    int i;

    memset(&record, 0, sizeof(CharacActionPointDesc));
    for (i = 1; i <= 5; i++) {
        record.action_group_index = 30;
        record.action_index = i;
        sprintf(record.action_group_name, "Group 30 Action %d", i);
        CharacActionPointDesc_Add(manager, &record);
    }

    int count = CharacActionPointDesc_GetByGroupIndex(manager, 30, records, 10);
    ASSERT_EQ(count, 5, "获取到5条记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].action_group_index, 30, "记录的行动组索引正确");
    }

    for (i = 1; i <= 5; i++) {
        CharacActionPointDesc_Delete(manager, 30, i);
    }

    TEST_PASS();
}

void test_count_by_group(DBConnectionManager* manager)
{
    TEST_START("按行动组统计记录数");

    CharacActionPointDesc record;
    int i;

    memset(&record, 0, sizeof(CharacActionPointDesc));
    for (i = 1; i <= 4; i++) {
        record.action_group_index = 40;
        record.action_index = i;
        sprintf(record.action_group_name, "Group 40 Action %d", i);
        CharacActionPointDesc_Add(manager, &record);
    }

    int count = CharacActionPointDesc_CountByGroup(manager, 40);
    ASSERT_EQ(count, 4, "行动组40有4条记录");

    for (i = 1; i <= 4; i++) {
        CharacActionPointDesc_Delete(manager, 40, i);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    CharacActionPointDesc record;
    CharacActionPointDesc keys[5];
    int i;

    memset(&record, 0, sizeof(CharacActionPointDesc));
    for (i = 1; i <= 5; i++) {
        record.action_group_index = 50;
        record.action_index = i;
        sprintf(record.action_group_name, "Batch Action %d", i);
        CharacActionPointDesc_Add(manager, &record);

        keys[i-1].action_group_index = 50;
        keys[i-1].action_index = i;
    }

    int ret = CharacActionPointDesc_BatchDelete(manager, keys, 5);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 1; i <= 5; i++) {
        int exists = CharacActionPointDesc_Exists(manager, 50, i);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    CharacActionPointDesc record, retrieved;
    memset(&record, 0, sizeof(CharacActionPointDesc));

    record.action_group_index = 60;
    record.action_index = 1;
    strcpy(record.action_group_name, "Original Name");
    int ret = CharacActionPointDesc_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    strcpy(record.action_group_name, "Updated Name");
    ret = CharacActionPointDesc_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    CharacActionPointDesc_Get(manager, 60, 1, &retrieved);
    ASSERT_STR_EQ(retrieved.action_group_name, "Updated Name", "数据为最新数据");

    int count_before = CharacActionPointDesc_Count(manager);
    strcpy(record.action_group_name, "Final Name");
    CharacActionPointDesc_Add(manager, &record);
    int count_after = CharacActionPointDesc_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    CharacActionPointDesc_Delete(manager, 60, 1);
    TEST_PASS();
}

void test_composite_key(DBConnectionManager* manager)
{
    TEST_START("复合主键测试");

    CharacActionPointDesc record1, record2, retrieved;
    memset(&record1, 0, sizeof(CharacActionPointDesc));
    memset(&record2, 0, sizeof(CharacActionPointDesc));

    // 同一组不同索引
    record1.action_group_index = 70;
    record1.action_index = 1;
    strcpy(record1.action_group_name, "Group 70 Action 1");
    CharacActionPointDesc_Add(manager, &record1);

    record2.action_group_index = 70;
    record2.action_index = 2;
    strcpy(record2.action_group_name, "Group 70 Action 2");
    CharacActionPointDesc_Add(manager, &record2);

    int ret = CharacActionPointDesc_Get(manager, 70, 1, &retrieved);
    ASSERT(ret == 0, "获取第一条记录成功");
    ASSERT_STR_EQ(retrieved.action_group_name, "Group 70 Action 1", "第一条记录名称正确");

    ret = CharacActionPointDesc_Get(manager, 70, 2, &retrieved);
    ASSERT(ret == 0, "获取第二条记录成功");
    ASSERT_STR_EQ(retrieved.action_group_name, "Group 70 Action 2", "第二条记录名称正确");

    CharacActionPointDesc_Delete(manager, 70, 1);
    CharacActionPointDesc_Delete(manager, 70, 2);
    TEST_PASS();
}

void test_long_string(DBConnectionManager* manager)
{
    TEST_START("长字符串处理测试");

    CharacActionPointDesc record, retrieved;
    memset(&record, 0, sizeof(CharacActionPointDesc));

    record.action_group_index = 80;
    record.action_index = 1;
    strcpy(record.action_group_name, "This is a very long action group name for testing string handling capabilities");
    int ret = CharacActionPointDesc_Add(manager, &record);
    ASSERT(ret == 0, "添加长字符串成功");

    ret = CharacActionPointDesc_Get(manager, 80, 1, &retrieved);
    ASSERT(ret == 0, "获取长字符串成功");
    ASSERT(strlen(retrieved.action_group_name) > 50, "字符串长度正确");

    CharacActionPointDesc_Delete(manager, 80, 1);
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
    printf("charac_action_point_desc ORM 测试\n");
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
    test_get_by_group_index(&manager);
    test_count_by_group(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_composite_key(&manager);
    test_long_string(&manager);

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
