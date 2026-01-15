#include "ch_status_orm.h"
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    ChStatus record, retrieved;
    int ret, count;

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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    ChStatus_Clear(&manager);

    /* 测试 1: 添加记录 */
    TEST_START("Add - 添加角色状态记录");
    memset(&record, 0, sizeof(ChStatus));
    record.gc_group = 1;
    record.gc_status = 10;

    ret = ChStatus_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    /* 测试 2: 检查记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = ChStatus_Exists(&manager, 1);
    ASSERT_EQ(ret, 1, "记录存在");

    ret = ChStatus_Exists(&manager, 99);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 3: 获取记录 */
    TEST_START("Get - 获取角色状态记录");
    memset(&retrieved, 0, sizeof(ChStatus));
    ret = ChStatus_Get(&manager, 1, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.gc_group, 1, "分组ID正确");
    ASSERT_EQ(retrieved.gc_status, 10, "状态值正确");

    ChStatus_PrintInfo(&retrieved);

    /* 测试 4: 更新记录 */
    TEST_START("Update - 更新记录");
    record.gc_status = 20;
    ret = ChStatus_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(ChStatus));
    ret = ChStatus_Get(&manager, 1, &retrieved);
    ASSERT_EQ(retrieved.gc_status, 20, "更新后状态值正确");

    /* 测试 5: UpdateStatus - 直接更新状态 */
    TEST_START("UpdateStatus - 直接更新状态值");
    ret = ChStatus_UpdateStatus(&manager, 1, 30);
    ASSERT_EQ(ret, 0, "更新状态成功");

    memset(&retrieved, 0, sizeof(ChStatus));
    ret = ChStatus_Get(&manager, 1, &retrieved);
    ASSERT_EQ(retrieved.gc_status, 30, "状态已更新为30");

    /* 测试 6: SetStatus - 设置状态（幂等操作） */
    TEST_START("SetStatus - 设置状态（幂等操作）");
    ret = ChStatus_SetStatus(&manager, 2, 40);
    ASSERT_EQ(ret, 0, "设置状态成功");

    ret = ChStatus_SetStatus(&manager, 2, 50);
    ASSERT_EQ(ret, 0, "重复设置状态成功");

    count = ChStatus_Count(&manager);
    ASSERT_EQ(count, 2, "总记录数为2");

    /* 测试 7: 批量添加 */
    TEST_START("Batch Add - 批量添加记录");
    ChStatus records[3];
    records[0].gc_group = 3;
    records[0].gc_status = 11;

    records[1].gc_group = 4;
    records[1].gc_status = 12;

    records[2].gc_group = 5;
    records[2].gc_status = 13;

    for (int i = 0; i < 3; i++) {
        ret = ChStatus_Add(&manager, &records[i]);
        ASSERT_EQ(ret, 0, "批量添加记录成功");
    }

    count = ChStatus_Count(&manager);
    ASSERT_EQ(count, 5, "总记录数为5");

    /* 测试 8: BatchSetStatus - 批量设置状态 */
    TEST_START("BatchSetStatus - 批量设置状态");
    unsigned char groups[] = {3, 4, 5};
    ret = ChStatus_BatchSetStatus(&manager, groups, 3, 99);
    ASSERT_EQ(ret, 0, "批量设置状态成功");

    memset(&retrieved, 0, sizeof(ChStatus));
    ret = ChStatus_Get(&manager, 3, &retrieved);
    ASSERT_EQ(retrieved.gc_status, 99, "分组3状态已更新为99");

    /* 测试 9: BatchDelete - 批量删除 */
    TEST_START("BatchDelete - 批量删除记录");
    unsigned char delete_groups[] = {3, 5};
    ret = ChStatus_BatchDelete(&manager, delete_groups, 2);
    ASSERT_EQ(ret, 0, "批量删除成功");

    count = ChStatus_Count(&manager);
    ASSERT_EQ(count, 3, "删除后剩余3条记录");

    /* 测试 10: Delete - 删除单条记录 */
    TEST_START("Delete - 删除单条记录");
    ret = ChStatus_Delete(&manager, 1);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = ChStatus_Exists(&manager, 1);
    ASSERT_EQ(ret, 0, "记录已删除");

    /* 测试 11: Clear - 清空所有数据 */
    TEST_START("Clear - 清空所有数据");
    ret = ChStatus_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = ChStatus_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 12: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(ChStatus));
    ret = ChStatus_Get(&manager, 99, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    ChStatus_Clear(&manager);
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
