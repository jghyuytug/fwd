#include "limit_create_character_orm.h"
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
    LimitCreateCharacter record, retrieved;
    int ret, count;
    LimitCreateCharacter* all_records;

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
    LimitCreateCharacter_Clear(&manager);

    /* 测试 1: 添加限制记录 */
    TEST_START("Add - 添加创建角色限制记录");
    memset(&record, 0, sizeof(LimitCreateCharacter));
    record.m_id = 10001;
    record.count = 3;
    strcpy(record.last_access_time, "2025-11-16 10:00:00");

    ret = LimitCreateCharacter_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    /* 测试 2: 查询记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = LimitCreateCharacter_Exists(&manager, 10001);
    ASSERT_EQ(ret, 1, "记录存在");

    ret = LimitCreateCharacter_Exists(&manager, 99999);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 3: 获取限制记录 */
    TEST_START("Get - 获取创建角色限制记录");
    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 10001, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.m_id, 10001, "会员ID正确");
    ASSERT_EQ(retrieved.count, 3, "创建次数正确");
    ASSERT_STR_EQ(retrieved.last_access_time, "2025-11-16 10:00:00", "访问时间正确");

    LimitCreateCharacter_PrintInfo(&retrieved);

    /* 测试 4: 更新限制记录 */
    TEST_START("Update - 更新限制记录");
    record.count = 5;
    strcpy(record.last_access_time, "2025-11-16 12:00:00");

    ret = LimitCreateCharacter_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 10001, &retrieved);
    ASSERT_EQ(retrieved.count, 5, "更新后次数正确");
    ASSERT_STR_EQ(retrieved.last_access_time, "2025-11-16 12:00:00", "更新后时间正确");

    /* 测试 5: REPLACE INTO 幂等性测试 */
    TEST_START("Add (REPLACE) - 测试 REPLACE INTO 幂等性");
    record.count = 7;
    strcpy(record.last_access_time, "2025-11-16 14:00:00");

    ret = LimitCreateCharacter_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 REPLACE 成功");

    ret = LimitCreateCharacter_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 REPLACE 成功（幂等性）");

    count = LimitCreateCharacter_Count(&manager);
    ASSERT_EQ(count, 1, "记录数量仍为1（未重复插入）");

    /* 测试 6: 增加创建次数 */
    TEST_START("IncrementCount - 增加创建次数");
    ret = LimitCreateCharacter_IncrementCount(&manager, 10001);
    ASSERT_EQ(ret, 0, "增加次数成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 10001, &retrieved);
    ASSERT_EQ(retrieved.count, 8, "次数已增加");

    /* 测试 7: 重置创建次数 */
    TEST_START("ResetCount - 重置创建次数");
    ret = LimitCreateCharacter_ResetCount(&manager, 10001);
    ASSERT_EQ(ret, 0, "重置成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 10001, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "次数已重置为0");

    /* 测试 8: 批量添加 */
    TEST_START("Batch Add - 批量添加限制记录");
    LimitCreateCharacter records[3];

    records[0].m_id = 20001;
    records[0].count = 1;
    strcpy(records[0].last_access_time, "2025-11-16 15:00:00");

    records[1].m_id = 20002;
    records[1].count = 2;
    strcpy(records[1].last_access_time, "2025-11-16 16:00:00");

    records[2].m_id = 20003;
    records[2].count = 3;
    strcpy(records[2].last_access_time, "2025-11-16 17:00:00");

    for (int i = 0; i < 3; i++) {
        ret = LimitCreateCharacter_Add(&manager, &records[i]);
        ASSERT_EQ(ret, 0, "批量添加记录成功");
    }

    count = LimitCreateCharacter_Count(&manager);
    ASSERT_EQ(count, 4, "总记录数为4");

    /* 测试 9: 获取所有记录 */
    TEST_START("GetAll - 获取所有限制记录");
    ret = LimitCreateCharacter_GetAll(&manager, &all_records, &count);
    ASSERT_EQ(ret, 0, "获取所有记录成功");
    ASSERT_EQ(count, 4, "记录数量正确");

    if (all_records) {
        free(all_records);
    }

    /* 测试 10: 按次数范围查询 */
    TEST_START("GetByCountRange - 按创建次数范围查询");
    ret = LimitCreateCharacter_GetByCountRange(&manager, 1, 2, &all_records, &count);
    ASSERT_EQ(ret, 0, "范围查询成功");
    ASSERT_EQ(count, 2, "查询到2条记录");

    if (all_records) {
        free(all_records);
    }

    /* 测试 11: 批量重置次数 */
    TEST_START("BatchResetCount - 批量重置创建次数");
    unsigned int reset_ids[] = {20001, 20002};
    ret = LimitCreateCharacter_BatchResetCount(&manager, reset_ids, 2);
    ASSERT_EQ(ret, 0, "批量重置成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 20001, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "20001次数已重置");

    /* 测试 12: 批量删除 */
    TEST_START("BatchDelete - 批量删除限制记录");
    unsigned int delete_ids[] = {20001, 20003};
    ret = LimitCreateCharacter_BatchDelete(&manager, delete_ids, 2);
    ASSERT_EQ(ret, 0, "批量删除成功");

    count = LimitCreateCharacter_Count(&manager);
    ASSERT_EQ(count, 2, "删除后记录数为2");

    ret = LimitCreateCharacter_Exists(&manager, 20002);
    ASSERT_EQ(ret, 1, "20002 仍然存在");

    ret = LimitCreateCharacter_Exists(&manager, 20001);
    ASSERT_EQ(ret, 0, "20001 已删除");

    /* 测试 13: 删除单个记录 */
    TEST_START("Delete - 删除单个限制记录");
    ret = LimitCreateCharacter_Delete(&manager, 10001);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = LimitCreateCharacter_Exists(&manager, 10001);
    ASSERT_EQ(ret, 0, "记录已删除");

    /* 测试 14: 清空所有数据 */
    TEST_START("Clear - 清空所有限制数据");
    ret = LimitCreateCharacter_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = LimitCreateCharacter_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 15: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(LimitCreateCharacter));
    ret = LimitCreateCharacter_Get(&manager, 99999, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    LimitCreateCharacter_Clear(&manager);
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
