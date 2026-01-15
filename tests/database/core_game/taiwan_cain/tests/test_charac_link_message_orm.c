#include "charac_link_message_orm.h"
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
    TEST_START("添加并获取链接消息");

    CharacLinkMessage record, retrieved;
    memset(&record, 0, sizeof(CharacLinkMessage));
    memset(&retrieved, 0, sizeof(CharacLinkMessage));

    record.m_id = 100;
    record.message_flag = 1;

    int ret = CharacLinkMessage_Add(manager, &record);
    ASSERT(ret == 0, "添加链接消息成功");

    ret = CharacLinkMessage_Get(manager, 100, &retrieved);
    ASSERT(ret == 0, "获取链接消息成功");
    ASSERT_EQ(retrieved.m_id, 100, "消息ID正确");
    ASSERT_EQ(retrieved.message_flag, 1, "消息标志正确");

    CharacLinkMessage_Delete(manager, 100);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新链接消息");

    CharacLinkMessage record, retrieved;
    memset(&record, 0, sizeof(CharacLinkMessage));

    record.m_id = 200;
    record.message_flag = 2;
    CharacLinkMessage_Add(manager, &record);

    record.message_flag = 5;
    int ret = CharacLinkMessage_Update(manager, &record);
    ASSERT(ret == 0, "更新链接消息成功");

    CharacLinkMessage_Get(manager, 200, &retrieved);
    ASSERT_EQ(retrieved.message_flag, 5, "更新后消息标志正确");

    CharacLinkMessage_Delete(manager, 200);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除链接消息");

    CharacLinkMessage record;
    memset(&record, 0, sizeof(CharacLinkMessage));

    record.m_id = 300;
    record.message_flag = 3;
    CharacLinkMessage_Add(manager, &record);

    int ret = CharacLinkMessage_Delete(manager, 300);
    ASSERT(ret == 0, "删除链接消息成功");

    ret = CharacLinkMessage_Exists(manager, 300);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查链接消息是否存在");

    CharacLinkMessage record;
    memset(&record, 0, sizeof(CharacLinkMessage));

    int ret = CharacLinkMessage_Exists(manager, 400);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.m_id = 400;
    record.message_flag = 4;
    CharacLinkMessage_Add(manager, &record);

    ret = CharacLinkMessage_Exists(manager, 400);
    ASSERT(ret == 1, "存在的记录返回1");

    CharacLinkMessage_Delete(manager, 400);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    CharacLinkMessage record;
    unsigned int m_ids[] = {501, 502, 503};
    int i;

    int initial_count = CharacLinkMessage_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(CharacLinkMessage));
    for (i = 0; i < 3; i++) {
        record.m_id = m_ids[i];
        record.message_flag = (char)(i + 1);
        CharacLinkMessage_Add(manager, &record);
    }

    int new_count = CharacLinkMessage_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        CharacLinkMessage_Delete(manager, m_ids[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    CharacLinkMessage record;
    CharacLinkMessage records[10];
    unsigned int m_ids[] = {601, 602, 603};
    int i;

    memset(&record, 0, sizeof(CharacLinkMessage));
    for (i = 0; i < 3; i++) {
        record.m_id = m_ids[i];
        record.message_flag = (char)(i + 1);
        CharacLinkMessage_Add(manager, &record);
    }

    int count = CharacLinkMessage_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        CharacLinkMessage_Delete(manager, m_ids[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    CharacLinkMessage record;
    unsigned int m_ids[] = {701, 702, 703, 704, 705};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(CharacLinkMessage));
    for (i = 0; i < count; i++) {
        record.m_id = m_ids[i];
        record.message_flag = (char)(i + 1);
        CharacLinkMessage_Add(manager, &record);
    }

    int ret = CharacLinkMessage_BatchDelete(manager, m_ids, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = CharacLinkMessage_Exists(manager, m_ids[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    CharacLinkMessage record, retrieved;
    memset(&record, 0, sizeof(CharacLinkMessage));

    record.m_id = 800;
    record.message_flag = 8;
    int ret = CharacLinkMessage_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.message_flag = 9;
    ret = CharacLinkMessage_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    CharacLinkMessage_Get(manager, 800, &retrieved);
    ASSERT_EQ(retrieved.message_flag, 9, "数据为最新数据");

    int count_before = CharacLinkMessage_Count(manager);
    record.message_flag = 10;
    CharacLinkMessage_Add(manager, &record);
    int count_after = CharacLinkMessage_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    CharacLinkMessage_Delete(manager, 800);
    TEST_PASS();
}

void test_message_flag_range(DBConnectionManager* manager)
{
    TEST_START("消息标志范围测试");

    CharacLinkMessage record, retrieved;
    memset(&record, 0, sizeof(CharacLinkMessage));

    record.m_id = 900;
    record.message_flag = -128;
    int ret = CharacLinkMessage_Add(manager, &record);
    ASSERT(ret == 0, "添加最小标志成功");

    record.m_id = 901;
    record.message_flag = 127;
    ret = CharacLinkMessage_Add(manager, &record);
    ASSERT(ret == 0, "添加最大标志成功");

    CharacLinkMessage_Get(manager, 900, &retrieved);
    ASSERT_EQ(retrieved.message_flag, -128, "最小标志正确");

    CharacLinkMessage_Get(manager, 901, &retrieved);
    ASSERT_EQ(retrieved.message_flag, 127, "最大标志正确");

    CharacLinkMessage_Delete(manager, 900);
    CharacLinkMessage_Delete(manager, 901);
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
    printf("charac_link_message ORM 测试\n");
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
    test_message_flag_range(&manager);

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
