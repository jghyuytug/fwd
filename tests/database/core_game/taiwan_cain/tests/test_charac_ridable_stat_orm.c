#include "charac_ridable_stat_orm.h"
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
    TEST_START("添加并获取坐骑数据");

    CharacRidableStat record;
    CharacRidableStat retrieved;
    unsigned int test_charac_no = 99991;
    unsigned char test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};

    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);

    // 添加数据
    int ret = CharacRidableStat_Add(manager, &record);
    ASSERT(ret == 0, "添加坐骑数据成功");

    // 获取数据
    ret = CharacRidableStat_Get(manager, test_charac_no, &retrieved);
    ASSERT(ret == 0, "获取坐骑数据成功");
    ASSERT_EQ(retrieved.cooltime_len, sizeof(test_data), "数据长度正确");
    ASSERT(memcmp(retrieved.cooltime, test_data, sizeof(test_data)) == 0, "数据内容正确");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新坐骑数据");

    CharacRidableStat record;
    CharacRidableStat retrieved;
    unsigned int test_charac_no = 99992;
    unsigned char old_data[] = {0xAA, 0xBB};
    unsigned char new_data[] = {0xCC, 0xDD, 0xEE};

    // 先添加
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, old_data, sizeof(old_data));
    record.cooltime_len = sizeof(old_data);
    CharacRidableStat_Add(manager, &record);

    // 更新
    memcpy(record.cooltime, new_data, sizeof(new_data));
    record.cooltime_len = sizeof(new_data);
    int ret = CharacRidableStat_Update(manager, &record);
    ASSERT(ret == 0, "更新坐骑数据成功");

    // 验证
    CharacRidableStat_Get(manager, test_charac_no, &retrieved);
    ASSERT_EQ(retrieved.cooltime_len, sizeof(new_data), "更新后数据长度正确");
    ASSERT(memcmp(retrieved.cooltime, new_data, sizeof(new_data)) == 0, "更新后数据内容正确");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除坐骑数据");

    CharacRidableStat record;
    unsigned int test_charac_no = 99993;
    unsigned char test_data[] = {0x11, 0x22};

    // 先添加
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);
    CharacRidableStat_Add(manager, &record);

    // 删除
    int ret = CharacRidableStat_Delete(manager, test_charac_no);
    ASSERT(ret == 0, "删除坐骑数据成功");

    // 验证不存在
    ret = CharacRidableStat_Exists(manager, test_charac_no);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查数据是否存在");

    CharacRidableStat record;
    unsigned int test_charac_no = 99994;
    unsigned char test_data[] = {0x55};

    // 检查不存在
    int ret = CharacRidableStat_Exists(manager, test_charac_no);
    ASSERT(ret == 0, "检查不存在的记录返回0");

    // 添加后检查
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);
    CharacRidableStat_Add(manager, &record);

    ret = CharacRidableStat_Exists(manager, test_charac_no);
    ASSERT(ret == 1, "检查存在的记录返回1");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    CharacRidableStat record;
    unsigned int test_charac_nos[] = {99995, 99996, 99997};
    unsigned char test_data[] = {0x99};
    int i;

    // 获取初始数量
    int initial_count = CharacRidableStat_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    // 添加3条记录
    memset(&record, 0, sizeof(CharacRidableStat));
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);

    for (i = 0; i < 3; i++) {
        record.charac_no = test_charac_nos[i];
        CharacRidableStat_Add(manager, &record);
    }

    // 验证数量增加
    int new_count = CharacRidableStat_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    // 清理
    for (i = 0; i < 3; i++) {
        CharacRidableStat_Delete(manager, test_charac_nos[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    CharacRidableStat record;
    CharacRidableStat records[10];
    unsigned int test_charac_nos[] = {99981, 99982, 99983};
    unsigned char test_data[] = {0x77};
    int i;

    // 添加3条记录
    memset(&record, 0, sizeof(CharacRidableStat));
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);

    for (i = 0; i < 3; i++) {
        record.charac_no = test_charac_nos[i];
        CharacRidableStat_Add(manager, &record);
    }

    // 获取所有记录
    int count = CharacRidableStat_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    // 清理
    for (i = 0; i < 3; i++) {
        CharacRidableStat_Delete(manager, test_charac_nos[i]);
    }

    TEST_PASS();
}

void test_clear_cooltime(DBConnectionManager* manager)
{
    TEST_START("清空冷却时间");

    CharacRidableStat record;
    CharacRidableStat retrieved;
    unsigned int test_charac_no = 99998;
    unsigned char test_data[] = {0xFF, 0xEE, 0xDD};

    // 添加数据
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);
    CharacRidableStat_Add(manager, &record);

    // 清空冷却时间
    int ret = CharacRidableStat_ClearCooltime(manager, test_charac_no);
    ASSERT(ret == 0, "清空冷却时间成功");

    // 验证数据为空
    CharacRidableStat_Get(manager, test_charac_no, &retrieved);
    ASSERT_EQ(retrieved.cooltime_len, 0, "清空后数据长度为0");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    CharacRidableStat record;
    unsigned int test_charac_nos[] = {99971, 99972, 99973, 99974, 99975};
    unsigned char test_data[] = {0x88};
    int count = 5;
    int i;

    // 添加5条记录
    memset(&record, 0, sizeof(CharacRidableStat));
    memcpy(record.cooltime, test_data, sizeof(test_data));
    record.cooltime_len = sizeof(test_data);

    for (i = 0; i < count; i++) {
        record.charac_no = test_charac_nos[i];
        CharacRidableStat_Add(manager, &record);
    }

    // 批量删除
    int ret = CharacRidableStat_BatchDelete(manager, test_charac_nos, count);
    ASSERT(ret == 0, "批量删除成功");

    // 验证都已删除
    for (i = 0; i < count; i++) {
        int exists = CharacRidableStat_Exists(manager, test_charac_nos[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_ensure_exists(DBConnectionManager* manager)
{
    TEST_START("确保记录存在");

    unsigned int test_charac_no = 99999;

    // 确保不存在的记录
    CharacRidableStat_Delete(manager, test_charac_no);

    int ret = CharacRidableStat_EnsureExists(manager, test_charac_no);
    ASSERT(ret == 0, "确保记录存在成功（新创建）");

    // 验证已创建
    int exists = CharacRidableStat_Exists(manager, test_charac_no);
    ASSERT(exists == 1, "记录已创建");

    // 再次调用（已存在）
    ret = CharacRidableStat_EnsureExists(manager, test_charac_no);
    ASSERT(ret == 0, "确保记录存在成功（已存在）");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    CharacRidableStat record;
    CharacRidableStat retrieved;
    unsigned int test_charac_no = 99990;
    unsigned char data1[] = {0x10, 0x20};
    unsigned char data2[] = {0x30, 0x40, 0x50};

    // 第一次添加
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = test_charac_no;
    memcpy(record.cooltime, data1, sizeof(data1));
    record.cooltime_len = sizeof(data1);
    int ret = CharacRidableStat_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    // 第二次添加相同角色（应该替换）
    memcpy(record.cooltime, data2, sizeof(data2));
    record.cooltime_len = sizeof(data2);
    ret = CharacRidableStat_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    // 验证数据为最新的
    CharacRidableStat_Get(manager, test_charac_no, &retrieved);
    ASSERT_EQ(retrieved.cooltime_len, sizeof(data2), "数据为最新数据");
    ASSERT(memcmp(retrieved.cooltime, data2, sizeof(data2)) == 0, "数据内容为最新");

    // 验证只有一条记录
    int count_before = CharacRidableStat_Count(manager);
    memcpy(record.cooltime, data1, sizeof(data1));
    record.cooltime_len = sizeof(data1);
    CharacRidableStat_Add(manager, &record);
    int count_after = CharacRidableStat_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    // 清理
    CharacRidableStat_Delete(manager, test_charac_no);

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
    printf("charac_ridable_stat ORM 测试\n");
    printf("==========================================\n");

    // 初始化数据库连接
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

    // 运行测试
    test_add_and_get(&manager);
    test_update(&manager);
    test_delete(&manager);
    test_exists(&manager);
    test_count(&manager);
    test_get_all(&manager);
    test_clear_cooltime(&manager);
    test_batch_delete(&manager);
    test_ensure_exists(&manager);
    test_replace_into_idempotent(&manager);

    // 清理
    DBConnectionManager_Cleanup(&manager);

    // 输出结果
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
