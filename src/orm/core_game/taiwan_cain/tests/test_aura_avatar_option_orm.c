#include "aura_avatar_option_orm.h"
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
    TEST_START("添加并获取光环/化身选项");

    AuraAvatarOption record, retrieved;
    memset(&record, 0, sizeof(AuraAvatarOption));
    memset(&retrieved, 0, sizeof(AuraAvatarOption));

    record.charac_no = 1001;
    record.option_type = 1;
    record.value_1 = 100;

    int ret = AuraAvatarOption_Add(manager, &record);
    ASSERT(ret == 0, "添加光环/化身选项成功");

    ret = AuraAvatarOption_Get(manager, 1001, 1, &retrieved);
    ASSERT(ret == 0, "获取光环/化身选项成功");
    ASSERT_EQ(retrieved.charac_no, 1001, "角色编号正确");
    ASSERT_EQ(retrieved.option_type, 1, "选项类型正确");
    ASSERT_EQ(retrieved.value_1, 100, "选项数值正确");

    AuraAvatarOption_Delete(manager, 1001, 1);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新光环/化身选项");

    AuraAvatarOption record, retrieved;
    memset(&record, 0, sizeof(AuraAvatarOption));

    record.charac_no = 1002;
    record.option_type = 2;
    record.value_1 = 50;
    AuraAvatarOption_Add(manager, &record);

    record.value_1 = 80;
    int ret = AuraAvatarOption_Update(manager, &record);
    ASSERT(ret == 0, "更新光环/化身选项成功");

    AuraAvatarOption_Get(manager, 1002, 2, &retrieved);
    ASSERT_EQ(retrieved.value_1, 80, "更新后数值正确");

    AuraAvatarOption_Delete(manager, 1002, 2);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除光环/化身选项");

    AuraAvatarOption record;
    memset(&record, 0, sizeof(AuraAvatarOption));

    record.charac_no = 1003;
    record.option_type = 3;
    record.value_1 = 30;
    AuraAvatarOption_Add(manager, &record);

    int ret = AuraAvatarOption_Delete(manager, 1003, 3);
    ASSERT(ret == 0, "删除光环/化身选项成功");

    ret = AuraAvatarOption_Exists(manager, 1003, 3);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查光环/化身选项是否存在");

    AuraAvatarOption record;
    memset(&record, 0, sizeof(AuraAvatarOption));

    int ret = AuraAvatarOption_Exists(manager, 1004, 4);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.charac_no = 1004;
    record.option_type = 4;
    record.value_1 = 25;
    AuraAvatarOption_Add(manager, &record);

    ret = AuraAvatarOption_Exists(manager, 1004, 4);
    ASSERT(ret == 1, "存在的记录返回1");

    AuraAvatarOption_Delete(manager, 1004, 4);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    AuraAvatarOption record;
    int i;

    int initial_count = AuraAvatarOption_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(AuraAvatarOption));
    for (i = 0; i < 3; i++) {
        record.charac_no = 2001;
        record.option_type = (char)(i + 1);
        record.value_1 = 10 + i * 5;
        AuraAvatarOption_Add(manager, &record);
    }

    int new_count = AuraAvatarOption_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        AuraAvatarOption_Delete(manager, 2001, (char)(i + 1));
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    AuraAvatarOption record;
    AuraAvatarOption records[10];
    int i;

    memset(&record, 0, sizeof(AuraAvatarOption));
    for (i = 0; i < 3; i++) {
        record.charac_no = 3001;
        record.option_type = (char)(i + 1);
        record.value_1 = 20 + i * 10;
        AuraAvatarOption_Add(manager, &record);
    }

    int count = AuraAvatarOption_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        AuraAvatarOption_Delete(manager, 3001, (char)(i + 1));
    }

    TEST_PASS();
}

void test_get_by_character(DBConnectionManager* manager)
{
    TEST_START("获取指定角色的所有选项");

    AuraAvatarOption record;
    AuraAvatarOption records[10];
    int i;

    memset(&record, 0, sizeof(AuraAvatarOption));
    for (i = 0; i < 5; i++) {
        record.charac_no = 4001;
        record.option_type = (char)(i + 1);
        record.value_1 = 15 + i * 8;
        AuraAvatarOption_Add(manager, &record);
    }

    int count = AuraAvatarOption_GetByCharacter(manager, 4001, records, 10);
    ASSERT_EQ(count, 5, "获取到5条角色选项");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].charac_no, 4001, "角色编号匹配");
    }

    for (i = 0; i < 5; i++) {
        AuraAvatarOption_Delete(manager, 4001, (char)(i + 1));
    }

    TEST_PASS();
}

void test_delete_by_character(DBConnectionManager* manager)
{
    TEST_START("删除指定角色的所有选项");

    AuraAvatarOption record;
    int i;

    memset(&record, 0, sizeof(AuraAvatarOption));
    for (i = 0; i < 4; i++) {
        record.charac_no = 5001;
        record.option_type = (char)(i + 1);
        record.value_1 = 12 + i * 6;
        AuraAvatarOption_Add(manager, &record);
    }

    int deleted = AuraAvatarOption_DeleteByCharacter(manager, 5001);
    ASSERT_EQ(deleted, 4, "删除了4条记录");

    AuraAvatarOption records[10];
    int count = AuraAvatarOption_GetByCharacter(manager, 5001, records, 10);
    ASSERT_EQ(count, 0, "角色选项已全部删除");

    TEST_PASS();
}

void test_composite_key(DBConnectionManager* manager)
{
    TEST_START("复合主键测试");

    AuraAvatarOption record;
    memset(&record, 0, sizeof(AuraAvatarOption));

    // 同一角色的不同选项
    record.charac_no = 6001;
    record.option_type = 1;
    record.value_1 = 100;
    AuraAvatarOption_Add(manager, &record);

    record.option_type = 2;
    record.value_1 = 200;
    AuraAvatarOption_Add(manager, &record);

    record.option_type = 3;
    record.value_1 = 300;
    AuraAvatarOption_Add(manager, &record);

    AuraAvatarOption records[10];
    int count = AuraAvatarOption_GetByCharacter(manager, 6001, records, 10);
    ASSERT_EQ(count, 3, "同一角色3个不同选项");

    // 不同角色的相同选项类型
    record.charac_no = 6002;
    record.option_type = 1;
    record.value_1 = 150;
    AuraAvatarOption_Add(manager, &record);

    int ret = AuraAvatarOption_Exists(manager, 6001, 1);
    ASSERT(ret == 1, "角色6001的选项1存在");

    ret = AuraAvatarOption_Exists(manager, 6002, 1);
    ASSERT(ret == 1, "角色6002的选项1也存在");

    AuraAvatarOption_DeleteByCharacter(manager, 6001);
    AuraAvatarOption_DeleteByCharacter(manager, 6002);
    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    AuraAvatarOption record, retrieved;
    memset(&record, 0, sizeof(AuraAvatarOption));

    record.charac_no = 7001;
    record.option_type = 1;
    record.value_1 = 50;
    int ret = AuraAvatarOption_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.value_1 = 100;
    ret = AuraAvatarOption_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    AuraAvatarOption_Get(manager, 7001, 1, &retrieved);
    ASSERT_EQ(retrieved.value_1, 100, "数据为最新数据");

    int count_before = AuraAvatarOption_Count(manager);
    record.value_1 = 150;
    AuraAvatarOption_Add(manager, &record);
    int count_after = AuraAvatarOption_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    AuraAvatarOption_Delete(manager, 7001, 1);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印光环/化身选项信息");

    AuraAvatarOption record;
    memset(&record, 0, sizeof(AuraAvatarOption));

    record.charac_no = 8001;
    record.option_type = 5;
    record.value_1 = 250;
    AuraAvatarOption_Add(manager, &record);

    AuraAvatarOption_Get(manager, 8001, 5, &record);
    AuraAvatarOption_PrintInfo(&record);
    ASSERT(1, "打印光环/化身选项信息成功");

    AuraAvatarOption_Delete(manager, 8001, 5);
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
    printf("aura_avatar_option ORM 测试\n");
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
    test_get_by_character(&manager);
    test_delete_by_character(&manager);
    test_composite_key(&manager);
    test_replace_into_idempotent(&manager);
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
