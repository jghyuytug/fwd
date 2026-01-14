#include "charac_cerashop_restrict_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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
    TEST_START("添加并获取Cera商店限购记录");

    CharacCerashopRestrict record, retrieved;
    memset(&record, 0, sizeof(CharacCerashopRestrict));
    memset(&retrieved, 0, sizeof(CharacCerashopRestrict));

    unsigned int now = (unsigned int)time(NULL);
    record.charac_no = 1001;
    record.ipg_no = 2001;
    record.count = 5;
    record.next_date = now + 86400;
    record.end_date = now + 604800;
    record.last_access_date = now;

    int ret = CharacCerashopRestrict_Add(manager, &record);
    ASSERT(ret == 0, "添加限购记录成功");

    ret = CharacCerashopRestrict_Get(manager, 1001, 2001, &retrieved);
    ASSERT(ret == 0, "获取限购记录成功");
    ASSERT_EQ(retrieved.charac_no, 1001, "角色编号正确");
    ASSERT_EQ(retrieved.ipg_no, 2001, "商品编号正确");
    ASSERT_EQ(retrieved.count, 5, "购买数量正确");

    CharacCerashopRestrict_Delete(manager, 1001, 2001);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新限购记录");

    CharacCerashopRestrict record, retrieved;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    unsigned int now = (unsigned int)time(NULL);
    record.charac_no = 1002;
    record.ipg_no = 2002;
    record.count = 3;
    record.next_date = now;
    record.end_date = now;
    record.last_access_date = now;
    CharacCerashopRestrict_Add(manager, &record);

    record.count = 8;
    int ret = CharacCerashopRestrict_Update(manager, &record);
    ASSERT(ret == 0, "更新限购记录成功");

    CharacCerashopRestrict_Get(manager, 1002, 2002, &retrieved);
    ASSERT_EQ(retrieved.count, 8, "更新后数量正确");

    CharacCerashopRestrict_Delete(manager, 1002, 2002);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除限购记录");

    CharacCerashopRestrict record;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    record.charac_no = 1003;
    record.ipg_no = 2003;
    record.count = 2;
    CharacCerashopRestrict_Add(manager, &record);

    int ret = CharacCerashopRestrict_Delete(manager, 1003, 2003);
    ASSERT(ret == 0, "删除限购记录成功");

    ret = CharacCerashopRestrict_Exists(manager, 1003, 2003);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查限购记录是否存在");

    CharacCerashopRestrict record;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    int ret = CharacCerashopRestrict_Exists(manager, 1004, 2004);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.charac_no = 1004;
    record.ipg_no = 2004;
    record.count = 1;
    CharacCerashopRestrict_Add(manager, &record);

    ret = CharacCerashopRestrict_Exists(manager, 1004, 2004);
    ASSERT(ret == 1, "存在的记录返回1");

    CharacCerashopRestrict_Delete(manager, 1004, 2004);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    CharacCerashopRestrict record;
    int i;

    int initial_count = CharacCerashopRestrict_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(CharacCerashopRestrict));
    for (i = 0; i < 3; i++) {
        record.charac_no = 2001;
        record.ipg_no = 3001 + i;
        record.count = i + 1;
        CharacCerashopRestrict_Add(manager, &record);
    }

    int new_count = CharacCerashopRestrict_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        CharacCerashopRestrict_Delete(manager, 2001, 3001 + i);
    }

    TEST_PASS();
}

void test_get_by_character(DBConnectionManager* manager)
{
    TEST_START("获取指定角色的所有限购记录");

    CharacCerashopRestrict record;
    CharacCerashopRestrict records[10];
    int i;

    memset(&record, 0, sizeof(CharacCerashopRestrict));
    for (i = 0; i < 5; i++) {
        record.charac_no = 4001;
        record.ipg_no = 5001 + i;
        record.count = i + 1;
        CharacCerashopRestrict_Add(manager, &record);
    }

    int count = CharacCerashopRestrict_GetByCharacter(manager, 4001, records, 10);
    ASSERT_EQ(count, 5, "获取到5条角色限购记录");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].charac_no, 4001, "角色编号匹配");
    }

    for (i = 0; i < 5; i++) {
        CharacCerashopRestrict_Delete(manager, 4001, 5001 + i);
    }

    TEST_PASS();
}

void test_delete_by_character(DBConnectionManager* manager)
{
    TEST_START("删除指定角色的所有限购记录");

    CharacCerashopRestrict record;
    int i;

    memset(&record, 0, sizeof(CharacCerashopRestrict));
    for (i = 0; i < 4; i++) {
        record.charac_no = 5001;
        record.ipg_no = 6001 + i;
        record.count = i + 1;
        CharacCerashopRestrict_Add(manager, &record);
    }

    int deleted = CharacCerashopRestrict_DeleteByCharacter(manager, 5001);
    ASSERT_EQ(deleted, 4, "删除了4条记录");

    CharacCerashopRestrict records[10];
    int count = CharacCerashopRestrict_GetByCharacter(manager, 5001, records, 10);
    ASSERT_EQ(count, 0, "角色限购记录已全部删除");

    TEST_PASS();
}

void test_increase_count(DBConnectionManager* manager)
{
    TEST_START("增加购买数量");

    CharacCerashopRestrict record, retrieved;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    record.charac_no = 6001;
    record.ipg_no = 7001;
    record.count = 10;
    CharacCerashopRestrict_Add(manager, &record);

    int ret = CharacCerashopRestrict_IncreaseCount(manager, 6001, 7001, 5);
    ASSERT(ret == 0, "增加购买数量成功");

    CharacCerashopRestrict_Get(manager, 6001, 7001, &retrieved);
    ASSERT_EQ(retrieved.count, 15, "购买数量增加正确");

    CharacCerashopRestrict_Delete(manager, 6001, 7001);
    TEST_PASS();
}

void test_composite_key(DBConnectionManager* manager)
{
    TEST_START("复合主键测试");

    CharacCerashopRestrict record;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    // 同一角色的不同商品
    record.charac_no = 7001;
    record.ipg_no = 8001;
    record.count = 1;
    CharacCerashopRestrict_Add(manager, &record);

    record.ipg_no = 8002;
    record.count = 2;
    CharacCerashopRestrict_Add(manager, &record);

    CharacCerashopRestrict records[10];
    int count = CharacCerashopRestrict_GetByCharacter(manager, 7001, records, 10);
    ASSERT_EQ(count, 2, "同一角色2个不同商品");

    // 不同角色的相同商品
    record.charac_no = 7002;
    record.ipg_no = 8001;
    record.count = 3;
    CharacCerashopRestrict_Add(manager, &record);

    int ret = CharacCerashopRestrict_Exists(manager, 7001, 8001);
    ASSERT(ret == 1, "角色7001的商品8001存在");

    ret = CharacCerashopRestrict_Exists(manager, 7002, 8001);
    ASSERT(ret == 1, "角色7002的商品8001也存在");

    CharacCerashopRestrict_DeleteByCharacter(manager, 7001);
    CharacCerashopRestrict_DeleteByCharacter(manager, 7002);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印限购记录信息");

    CharacCerashopRestrict record;
    memset(&record, 0, sizeof(CharacCerashopRestrict));

    unsigned int now = (unsigned int)time(NULL);
    record.charac_no = 8001;
    record.ipg_no = 9001;
    record.count = 15;
    record.next_date = now + 86400;
    record.end_date = now + 604800;
    record.last_access_date = now;
    CharacCerashopRestrict_Add(manager, &record);

    CharacCerashopRestrict_Get(manager, 8001, 9001, &record);
    CharacCerashopRestrict_PrintInfo(&record);
    ASSERT(1, "打印限购记录信息成功");

    CharacCerashopRestrict_Delete(manager, 8001, 9001);
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
    printf("charac_cerashop_restrict ORM 测试\n");
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
    test_get_by_character(&manager);
    test_delete_by_character(&manager);
    test_increase_count(&manager);
    test_composite_key(&manager);
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
