#include "stat_game_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 测试统计变量 */
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: \"%s\" != \"%s\")\n", message, (a), (b)); \
        } \
    } while(0)

#define ASSERT_GE(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) >= (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d < %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    StatGameChannel record, retrieved;
    int result;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize DBConnectionManager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("  游戏频道统计 ORM 测试套件\n");
    printf("========================================\n");

    /* ==================== CRUD 操作测试 ==================== */

    TEST_START("添加游戏频道统计记录（Add）");
    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_01", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-16 10:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 1250;

    result = StatGameChannel_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第一条记录成功");

    /* 添加第二条记录（同频道，不同时间） */
    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_01", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-16 11:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 1580;

    result = StatGameChannel_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第二条记录成功");

    /* 添加第三条记录（同频道，不同时间） */
    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_01", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-16 12:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 2100;

    result = StatGameChannel_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第三条记录成功");

    TEST_START("获取游戏频道统计记录（Get）");
    memset(&retrieved, 0, sizeof(StatGameChannel));
    result = StatGameChannel_Get(&manager, "channel_01", "2025-11-16 10:00:00", &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_STR_EQ(retrieved.gc_channel, "channel_01", "频道名称匹配");
    ASSERT_STR_EQ(retrieved.gc_up_time, "2025-11-16 10:00:00", "更新时间匹配");
    ASSERT_EQ(retrieved.gc_now, 1250, "在线人数匹配");

    TEST_START("检查记录存在（Exists）");
    result = StatGameChannel_Exists(&manager, "channel_01", "2025-11-16 10:00:00");
    ASSERT_EQ(result, 1, "记录存在");

    result = StatGameChannel_Exists(&manager, "channel_01", "2025-11-16 09:00:00");
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    result = StatGameChannel_Exists(&manager, "channel_99", "2025-11-16 10:00:00");
    ASSERT_EQ(result, 0, "不存在的频道返回0");

    TEST_START("更新游戏频道统计记录（Update）");
    memset(&retrieved, 0, sizeof(StatGameChannel));
    StatGameChannel_Get(&manager, "channel_01", "2025-11-16 10:00:00", &retrieved);
    retrieved.gc_now = 1350; /* 更新在线人数 */

    result = StatGameChannel_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(StatGameChannel));
    StatGameChannel_Get(&manager, "channel_01", "2025-11-16 10:00:00", &retrieved);
    ASSERT_EQ(retrieved.gc_now, 1350, "在线人数已更新");

    /* ==================== 专项操作测试 ==================== */

    TEST_START("按频道查询最新记录（GetLatest）");
    memset(&retrieved, 0, sizeof(StatGameChannel));
    result = StatGameChannel_GetLatest(&manager, "channel_01", &retrieved);
    ASSERT_EQ(result, 0, "查询最新记录成功");
    ASSERT_STR_EQ(retrieved.gc_up_time, "2025-11-16 12:00:00", "返回最新时间的记录");
    ASSERT_EQ(retrieved.gc_now, 2100, "最新记录的在线人数正确");

    TEST_START("按频道删除时间范围记录（DeleteByTimeRange）");
    /* 添加更多测试记录 */
    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_02", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-15 10:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 800;
    StatGameChannel_Add(&manager, &record);

    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_02", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-15 11:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 950;
    StatGameChannel_Add(&manager, &record);

    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_02", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2025-11-15 12:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 1100;
    StatGameChannel_Add(&manager, &record);

    result = StatGameChannel_DeleteByTimeRange(&manager, "channel_02",
        "2025-11-15 10:30:00", "2025-11-15 11:30:00");
    ASSERT_EQ(result, 1, "删除了1条记录（11:00:00在范围内）");

    /* 验证删除 */
    result = StatGameChannel_Exists(&manager, "channel_02", "2025-11-15 10:00:00");
    ASSERT_EQ(result, 1, "10:00:00记录仍存在（不在删除范围）");

    result = StatGameChannel_Exists(&manager, "channel_02", "2025-11-15 11:00:00");
    ASSERT_EQ(result, 0, "11:00:00记录已删除");

    result = StatGameChannel_Exists(&manager, "channel_02", "2025-11-15 12:00:00");
    ASSERT_EQ(result, 1, "12:00:00记录仍存在（不在删除范围）");

    TEST_START("批量删除过期记录（DeleteOlderThan）");
    /* 添加旧记录 */
    memset(&record, 0, sizeof(StatGameChannel));
    strncpy(record.gc_channel, "channel_03", sizeof(record.gc_channel) - 1);
    strncpy(record.gc_up_time, "2020-01-01 10:00:00", sizeof(record.gc_up_time) - 1);
    record.gc_now = 500;
    StatGameChannel_Add(&manager, &record);

    result = StatGameChannel_DeleteOlderThan(&manager, 30); /* 删除30天前的记录 */
    ASSERT_GE(result, 1, "删除了至少1条过期记录");

    /* 验证旧记录已删除 */
    result = StatGameChannel_Exists(&manager, "channel_03", "2020-01-01 10:00:00");
    ASSERT_EQ(result, 0, "2020年的旧记录已删除");

    /* ==================== 查询操作测试 ==================== */

    TEST_START("按频道统计记录数（CountByChannel）");
    result = StatGameChannel_CountByChannel(&manager, "channel_01");
    ASSERT_EQ(result, 3, "channel_01有3条记录");

    result = StatGameChannel_CountByChannel(&manager, "channel_02");
    ASSERT_EQ(result, 2, "channel_02有2条记录（删除1条后剩余）");

    result = StatGameChannel_CountByChannel(&manager, "channel_99");
    ASSERT_EQ(result, 0, "不存在的频道返回0");

    TEST_START("统计总记录数（CountAll）");
    result = StatGameChannel_CountAll(&manager);
    ASSERT_GE(result, 5, "至少有5条记录");

    TEST_START("统计不同频道数量（CountDistinctChannels）");
    result = StatGameChannel_CountDistinctChannels(&manager);
    ASSERT_GE(result, 2, "至少有2个不同频道");

    TEST_START("查询时间范围平均在线人数（GetAverageOnline）");
    result = StatGameChannel_GetAverageOnline(&manager, "channel_01",
        "2025-11-16 10:00:00", "2025-11-16 12:00:00");
    /* 平均值 = (1350 + 1580 + 2100) / 3 = 1676.67 */
    ASSERT_GE(result, 1600, "平均在线人数>=1600");
    ASSERT_GE(1700, result, "平均在线人数<=1700");

    TEST_START("查询时间范围最高在线人数（GetMaxOnline）");
    result = StatGameChannel_GetMaxOnline(&manager, "channel_01",
        "2025-11-16 10:00:00", "2025-11-16 12:00:00");
    ASSERT_EQ(result, 2100, "最高在线人数为2100");

    /* ==================== 工具函数测试 ==================== */

    TEST_START("打印游戏频道统计信息（PrintInfo）");
    StatGameChannel_Get(&manager, "channel_01", "2025-11-16 12:00:00", &retrieved);
    printf("  调用 PrintInfo:\n");
    StatGameChannel_PrintInfo(&retrieved);
    ASSERT_EQ(1, 1, "PrintInfo执行成功（目测输出）");

    /* ==================== 清理测试数据 ==================== */

    TEST_START("删除测试记录（Delete）");
    result = StatGameChannel_Delete(&manager, "channel_01", "2025-11-16 10:00:00");
    ASSERT_EQ(result, 0, "删除第一条记录成功");

    result = StatGameChannel_Delete(&manager, "channel_01", "2025-11-16 11:00:00");
    ASSERT_EQ(result, 0, "删除第二条记录成功");

    result = StatGameChannel_Delete(&manager, "channel_01", "2025-11-16 12:00:00");
    ASSERT_EQ(result, 0, "删除第三条记录成功");

    result = StatGameChannel_Delete(&manager, "channel_02", "2025-11-15 10:00:00");
    ASSERT_EQ(result, 0, "删除channel_02记录成功");

    result = StatGameChannel_Delete(&manager, "channel_02", "2025-11-15 12:00:00");
    ASSERT_EQ(result, 0, "删除channel_02最后一条记录成功");

    result = StatGameChannel_Exists(&manager, "channel_01", "2025-11-16 10:00:00");
    ASSERT_EQ(result, 0, "channel_01记录已被删除");

    /* 统计最后一个测试用例 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理连接 */
    DBConnectionManager_Cleanup(&manager);

    /* ==================== 测试结果汇总 ==================== */

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count,
        (test_count > 0) ? (test_passed * 100.0 / test_count) : 0);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count,
        (assertion_count > 0) ? (assertion_passed * 100.0 / assertion_count) : 0);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        return 1;
    }
}
