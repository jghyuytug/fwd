#include "charac_dungeon_test_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

/* 测试宏 */
#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d == %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_TRUE(expr, message) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (a && b && strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a ? a : "NULL", b ? b : "NULL"); \
        } \
    } while(0)

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    CharacDungeonTest record, retrieved;
    int ret, count;
    CharacDungeonTest* all_records;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    CharacDungeonTest_Clear(&manager);

    /* 测试 1: 添加测试数据 */
    TEST_START("Add - 添加角色副本测试数据");
    memset(&record, 0, sizeof(CharacDungeonTest));
    record.charac_no = 12345678;

    /* 创建测试副本数据 */
    unsigned char test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    record.dungeon = test_data;
    record.dungeon_len = sizeof(test_data);

    ret = CharacDungeonTest_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加测试数据成功");

    /* 测试 2: 查询记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = CharacDungeonTest_Exists(&manager, 12345678);
    ASSERT_EQ(ret, 1, "记录存在");

    ret = CharacDungeonTest_Exists(&manager, 99999999);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 3: 获取测试数据 */
    TEST_START("Get - 获取角色副本测试数据");
    memset(&retrieved, 0, sizeof(CharacDungeonTest));
    ret = CharacDungeonTest_Get(&manager, 12345678, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.charac_no, 12345678, "角色编号正确");
    ASSERT_EQ(retrieved.dungeon_len, sizeof(test_data), "副本数据长度正确");

    if (retrieved.dungeon && retrieved.dungeon_len == sizeof(test_data)) {
        int data_match = (memcmp(retrieved.dungeon, test_data, sizeof(test_data)) == 0);
        ASSERT_TRUE(data_match, "副本数据内容正确");
    }

    CharacDungeonTest_PrintInfo(&retrieved);
    CharacDungeonTest_Free(&retrieved);

    /* 测试 4: 更新副本数据 */
    TEST_START("Update - 更新副本数据");
    unsigned char updated_data[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    record.dungeon = updated_data;
    record.dungeon_len = sizeof(updated_data);

    ret = CharacDungeonTest_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(CharacDungeonTest));
    ret = CharacDungeonTest_Get(&manager, 12345678, &retrieved);
    ASSERT_EQ(retrieved.dungeon_len, sizeof(updated_data), "更新后数据长度正确");

    if (retrieved.dungeon && retrieved.dungeon_len == sizeof(updated_data)) {
        int data_match = (memcmp(retrieved.dungeon, updated_data, sizeof(updated_data)) == 0);
        ASSERT_TRUE(data_match, "更新后数据内容正确");
    }

    CharacDungeonTest_Free(&retrieved);

    /* 测试 5: REPLACE INTO 幂等性测试 */
    TEST_START("Add (REPLACE) - 测试 REPLACE INTO 幂等性");
    unsigned char replace_data[] = {0x11, 0x22, 0x33, 0x44};
    record.dungeon = replace_data;
    record.dungeon_len = sizeof(replace_data);

    ret = CharacDungeonTest_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 REPLACE 成功");

    ret = CharacDungeonTest_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 REPLACE 成功（幂等性）");

    count = CharacDungeonTest_Count(&manager);
    ASSERT_EQ(count, 1, "记录数量仍为1（未重复插入）");

    /* 测试 6: 批量添加 */
    TEST_START("Batch Add - 批量添加多个角色测试数据");
    CharacDungeonTest records[3];
    unsigned char data1[] = {0x01, 0x02};
    unsigned char data2[] = {0x03, 0x04, 0x05};
    unsigned char data3[] = {0x06};

    records[0].charac_no = 10001;
    records[0].dungeon = data1;
    records[0].dungeon_len = sizeof(data1);

    records[1].charac_no = 10002;
    records[1].dungeon = data2;
    records[1].dungeon_len = sizeof(data2);

    records[2].charac_no = 10003;
    records[2].dungeon = data3;
    records[2].dungeon_len = sizeof(data3);

    for (int i = 0; i < 3; i++) {
        ret = CharacDungeonTest_Add(&manager, &records[i]);
        ASSERT_EQ(ret, 0, "批量添加记录成功");
    }

    count = CharacDungeonTest_Count(&manager);
    ASSERT_EQ(count, 4, "总记录数为4");

    /* 测试 7: 获取所有记录 */
    TEST_START("GetAll - 获取所有测试数据");
    ret = CharacDungeonTest_GetAll(&manager, &all_records, &count);
    ASSERT_EQ(ret, 0, "获取所有记录成功");
    ASSERT_EQ(count, 4, "记录数量正确");

    if (all_records) {
        for (int i = 0; i < count; i++) {
            CharacDungeonTest_Free(&all_records[i]);
        }
        free(all_records);
    }

    /* 测试 8: 批量删除 */
    TEST_START("BatchDelete - 批量删除测试数据");
    unsigned int delete_ids[] = {10001, 10003};
    ret = CharacDungeonTest_BatchDelete(&manager, delete_ids, 2);
    ASSERT_EQ(ret, 0, "批量删除成功");

    count = CharacDungeonTest_Count(&manager);
    ASSERT_EQ(count, 2, "删除后记录数为2");

    ret = CharacDungeonTest_Exists(&manager, 10002);
    ASSERT_EQ(ret, 1, "10002 仍然存在");

    ret = CharacDungeonTest_Exists(&manager, 10001);
    ASSERT_EQ(ret, 0, "10001 已删除");

    /* 测试 9: 空数据处理 */
    TEST_START("Empty Data - 处理空副本数据");
    memset(&record, 0, sizeof(CharacDungeonTest));
    record.charac_no = 20001;
    record.dungeon = NULL;
    record.dungeon_len = 0;

    ret = CharacDungeonTest_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加空数据记录成功");

    memset(&retrieved, 0, sizeof(CharacDungeonTest));
    ret = CharacDungeonTest_Get(&manager, 20001, &retrieved);
    ASSERT_EQ(ret, 0, "获取空数据记录成功");
    ASSERT_EQ(retrieved.dungeon_len, 0, "副本数据长度为0");

    CharacDungeonTest_Free(&retrieved);

    /* 测试 10: 删除单个记录 */
    TEST_START("Delete - 删除单个测试记录");
    ret = CharacDungeonTest_Delete(&manager, 12345678);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = CharacDungeonTest_Exists(&manager, 12345678);
    ASSERT_EQ(ret, 0, "记录已删除");

    /* 测试 11: 清空所有数据 */
    TEST_START("Clear - 清空所有测试数据");
    ret = CharacDungeonTest_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = CharacDungeonTest_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 12: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(CharacDungeonTest));
    ret = CharacDungeonTest_Get(&manager, 99999999, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    CharacDungeonTest_Clear(&manager);
    DBConnectionManager_Cleanup(&manager);

    /* 计算最后一个测试是否通过 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}