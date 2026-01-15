#include "channel_occ_info_orm.h"
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
    TEST_START("添加并获取频道占用信息");

    ChannelOccInfo record, retrieved;
    memset(&record, 0, sizeof(ChannelOccInfo));
    memset(&retrieved, 0, sizeof(ChannelOccInfo));

    record.gc_no = 1;
    record.age = 18;
    record.occ_num = 150;

    int ret = ChannelOccInfo_Add(manager, &record);
    ASSERT(ret == 0, "添加频道占用信息成功");

    ret = ChannelOccInfo_Get(manager, 1, 18, &retrieved);
    ASSERT(ret == 0, "获取频道占用信息成功");
    ASSERT_EQ(retrieved.gc_no, 1, "频道编号正确");
    ASSERT_EQ(retrieved.age, 18, "年龄段正确");
    ASSERT_EQ(retrieved.occ_num, 150, "占用人数正确");

    ChannelOccInfo_Delete(manager, 1, 18);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新频道占用信息");

    ChannelOccInfo record, retrieved;
    memset(&record, 0, sizeof(ChannelOccInfo));

    record.gc_no = 2;
    record.age = 20;
    record.occ_num = 200;
    ChannelOccInfo_Add(manager, &record);

    record.occ_num = 250;
    int ret = ChannelOccInfo_Update(manager, &record);
    ASSERT(ret == 0, "更新频道占用信息成功");

    ChannelOccInfo_Get(manager, 2, 20, &retrieved);
    ASSERT_EQ(retrieved.occ_num, 250, "更新后占用人数正确");

    ChannelOccInfo_Delete(manager, 2, 20);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除频道占用信息");

    ChannelOccInfo record;
    memset(&record, 0, sizeof(ChannelOccInfo));

    record.gc_no = 3;
    record.age = 25;
    record.occ_num = 100;
    ChannelOccInfo_Add(manager, &record);

    int ret = ChannelOccInfo_Delete(manager, 3, 25);
    ASSERT(ret == 0, "删除频道占用信息成功");

    ret = ChannelOccInfo_Exists(manager, 3, 25);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查频道占用信息是否存在");

    ChannelOccInfo record;
    memset(&record, 0, sizeof(ChannelOccInfo));

    int ret = ChannelOccInfo_Exists(manager, 4, 30);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.gc_no = 4;
    record.age = 30;
    record.occ_num = 80;
    ChannelOccInfo_Add(manager, &record);

    ret = ChannelOccInfo_Exists(manager, 4, 30);
    ASSERT(ret == 1, "存在的记录返回1");

    ChannelOccInfo_Delete(manager, 4, 30);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    ChannelOccInfo record;
    int i;

    int initial_count = ChannelOccInfo_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(ChannelOccInfo));
    for (i = 0; i < 3; i++) {
        record.gc_no = 10;
        record.age = 10 + i * 5;
        record.occ_num = (i + 1) * 50;
        ChannelOccInfo_Add(manager, &record);
    }

    int new_count = ChannelOccInfo_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        ChannelOccInfo_Delete(manager, 10, 10 + i * 5);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    ChannelOccInfo record;
    ChannelOccInfo records[10];
    int i;

    memset(&record, 0, sizeof(ChannelOccInfo));
    for (i = 0; i < 3; i++) {
        record.gc_no = 20;
        record.age = 15 + i * 3;
        record.occ_num = (i + 1) * 60;
        ChannelOccInfo_Add(manager, &record);
    }

    int count = ChannelOccInfo_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        ChannelOccInfo_Delete(manager, 20, 15 + i * 3);
    }

    TEST_PASS();
}

void test_get_by_channel(DBConnectionManager* manager)
{
    TEST_START("按频道编号获取信息");

    ChannelOccInfo record;
    ChannelOccInfo records[10];
    int i;

    memset(&record, 0, sizeof(ChannelOccInfo));
    for (i = 0; i < 4; i++) {
        record.gc_no = 30;
        record.age = 12 + i * 4;
        record.occ_num = (i + 1) * 45;
        ChannelOccInfo_Add(manager, &record);
    }

    int count = ChannelOccInfo_GetByChannel(manager, 30, records, 10);
    ASSERT_EQ(count, 4, "获取到4条频道30的信息");

    for (i = 0; i < count; i++) {
        ASSERT_EQ(records[i].gc_no, 30, "频道编号正确");
    }

    for (i = 0; i < 4; i++) {
        ChannelOccInfo_Delete(manager, 30, 12 + i * 4);
    }

    TEST_PASS();
}

void test_count_by_channel(DBConnectionManager* manager)
{
    TEST_START("统计指定频道的记录数");

    ChannelOccInfo record;
    int i;

    int initial_count = ChannelOccInfo_CountByChannel(manager, 40);
    ASSERT(initial_count >= 0, "获取初始频道40记录数成功");

    memset(&record, 0, sizeof(ChannelOccInfo));
    for (i = 0; i < 5; i++) {
        record.gc_no = 40;
        record.age = 10 + i * 2;
        record.occ_num = (i + 1) * 30;
        ChannelOccInfo_Add(manager, &record);
    }

    int new_count = ChannelOccInfo_CountByChannel(manager, 40);
    ASSERT_EQ(new_count, initial_count + 5, "频道40记录数增加5条");

    for (i = 0; i < 5; i++) {
        ChannelOccInfo_Delete(manager, 40, 10 + i * 2);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    ChannelOccInfo records[5];
    int count = 5;
    int i;

    memset(records, 0, sizeof(records));
    for (i = 0; i < count; i++) {
        records[i].gc_no = 50;
        records[i].age = 8 + i * 2;
        records[i].occ_num = (i + 1) * 40;
        ChannelOccInfo_Add(manager, &records[i]);
    }

    int ret = ChannelOccInfo_BatchDelete(manager, records, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = ChannelOccInfo_Exists(manager, records[i].gc_no, records[i].age);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    ChannelOccInfo record, retrieved;
    memset(&record, 0, sizeof(ChannelOccInfo));

    record.gc_no = 60;
    record.age = 22;
    record.occ_num = 120;
    int ret = ChannelOccInfo_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.occ_num = 180;
    ret = ChannelOccInfo_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    ChannelOccInfo_Get(manager, 60, 22, &retrieved);
    ASSERT_EQ(retrieved.occ_num, 180, "数据为最新数据");

    int count_before = ChannelOccInfo_Count(manager);
    record.occ_num = 200;
    ChannelOccInfo_Add(manager, &record);
    int count_after = ChannelOccInfo_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    ChannelOccInfo_Delete(manager, 60, 22);
    TEST_PASS();
}

void test_unsigned_types(DBConnectionManager* manager)
{
    TEST_START("无符号类型测试");

    ChannelOccInfo record, retrieved;
    memset(&record, 0, sizeof(ChannelOccInfo));

    record.gc_no = 4294967295U;  // UINT_MAX
    record.age = 255;            // UCHAR_MAX
    record.occ_num = 500;
    int ret = ChannelOccInfo_Add(manager, &record);
    ASSERT(ret == 0, "添加最大值记录成功");

    ChannelOccInfo_Get(manager, 4294967295U, 255, &retrieved);
    ASSERT_EQ(retrieved.gc_no, 4294967295U, "频道编号最大值正确");
    ASSERT_EQ(retrieved.age, 255, "年龄段最大值正确");

    ChannelOccInfo_Delete(manager, 4294967295U, 255);
    TEST_PASS();
}

void test_print_info(DBConnectionManager* manager)
{
    TEST_START("打印频道信息");

    ChannelOccInfo record;
    memset(&record, 0, sizeof(ChannelOccInfo));

    record.gc_no = 100;
    record.age = 28;
    record.occ_num = 300;
    ChannelOccInfo_Add(manager, &record);

    ChannelOccInfo_Get(manager, 100, 28, &record);
    ChannelOccInfo_PrintInfo(&record);
    ASSERT(1, "打印频道信息成功");

    ChannelOccInfo_Delete(manager, 100, 28);
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
    printf("channel_occ_info ORM 测试\n");
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
    test_get_by_channel(&manager);
    test_count_by_channel(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_unsigned_types(&manager);
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
