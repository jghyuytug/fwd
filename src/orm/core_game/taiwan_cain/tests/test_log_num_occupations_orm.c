#include "log_num_occupations_orm.h"
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
    LogNumOccupations record, retrieved;
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
    printf("  服务器占用数统计 ORM 测试套件\n");
    printf("========================================\n");

    /* ==================== CRUD 操作测试 ==================== */

    TEST_START("添加服务器占用数记录（Add）");
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 10:00:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1000;
    record.num_occupations_seriaroom = 800;
    record.num_login_per_min = 50;
    record.num_logout_per_min = 30;

    result = LogNumOccupations_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第一条记录成功");

    /* 添加第二条记录 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 10:01:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1100;
    record.num_occupations_seriaroom = 850;
    record.num_login_per_min = 55;
    record.num_logout_per_min = 32;

    result = LogNumOccupations_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第二条记录成功");

    /* 添加第三条记录 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 10:02:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1200;
    record.num_occupations_seriaroom = 900;
    record.num_login_per_min = 60;
    record.num_logout_per_min = 35;

    result = LogNumOccupations_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第三条记录成功");

    TEST_START("获取服务器占用数记录（Get）");
    memset(&retrieved, 0, sizeof(LogNumOccupations));
    result = LogNumOccupations_Get(&manager, "2025-11-16 10:00:00", &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_STR_EQ(retrieved.occ_time, "2025-11-16 10:00:00", "记录时间匹配");
    ASSERT_EQ(retrieved.num_occupations_charscreen, 1000, "角色选择界面占用数匹配");
    ASSERT_EQ(retrieved.num_occupations_seriaroom, 800, "Seria房间占用数匹配");
    ASSERT_EQ(retrieved.num_login_per_min, 50, "每分钟登录数匹配");
    ASSERT_EQ(retrieved.num_logout_per_min, 30, "每分钟登出数匹配");

    TEST_START("检查记录存在（Exists）");
    result = LogNumOccupations_Exists(&manager, "2025-11-16 10:00:00");
    ASSERT_EQ(result, 1, "记录存在");

    result = LogNumOccupations_Exists(&manager, "2025-11-16 09:00:00");
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    TEST_START("更新服务器占用数记录（Update）");
    memset(&retrieved, 0, sizeof(LogNumOccupations));
    LogNumOccupations_Get(&manager, "2025-11-16 10:00:00", &retrieved);
    retrieved.num_occupations_charscreen = 1300;
    retrieved.num_occupations_seriaroom = 950;
    retrieved.num_login_per_min = 65;
    retrieved.num_logout_per_min = 40;

    result = LogNumOccupations_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(LogNumOccupations));
    LogNumOccupations_Get(&manager, "2025-11-16 10:00:00", &retrieved);
    ASSERT_EQ(retrieved.num_occupations_charscreen, 1300, "角色选择界面占用数已更新");
    ASSERT_EQ(retrieved.num_occupations_seriaroom, 950, "Seria房间占用数已更新");
    ASSERT_EQ(retrieved.num_login_per_min, 65, "每分钟登录数已更新");
    ASSERT_EQ(retrieved.num_logout_per_min, 40, "每分钟登出数已更新");

    TEST_START("REPLACE INTO 幂等性测试");
    /* 再次添加相同时间的记录，应该替换而不是报错 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 10:01:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 9999;
    record.num_occupations_seriaroom = 8888;
    record.num_login_per_min = 100;
    record.num_logout_per_min = 80;

    result = LogNumOccupations_Add(&manager, &record);
    ASSERT_EQ(result, 0, "REPLACE INTO执行成功");

    memset(&retrieved, 0, sizeof(LogNumOccupations));
    LogNumOccupations_Get(&manager, "2025-11-16 10:01:00", &retrieved);
    ASSERT_EQ(retrieved.num_occupations_charscreen, 9999, "记录已替换（角色选择界面）");
    ASSERT_EQ(retrieved.num_occupations_seriaroom, 8888, "记录已替换（Seria房间）");

    /* ==================== 专项操作测试 ==================== */

    TEST_START("获取最新记录（GetLatest）");
    /* 添加一条更晚的记录 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 10:05:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1500;
    record.num_occupations_seriaroom = 1000;
    record.num_login_per_min = 70;
    record.num_logout_per_min = 45;
    LogNumOccupations_Add(&manager, &record);

    memset(&retrieved, 0, sizeof(LogNumOccupations));
    result = LogNumOccupations_GetLatest(&manager, &retrieved);
    ASSERT_EQ(result, 0, "获取最新记录成功");
    ASSERT_STR_EQ(retrieved.occ_time, "2025-11-16 10:05:00", "最新记录时间是10:05:00");

    TEST_START("按时间范围删除记录（DeleteByTimeRange）");
    /* 添加测试数据 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 11:00:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 500;
    record.num_occupations_seriaroom = 400;
    record.num_login_per_min = 20;
    record.num_logout_per_min = 15;
    LogNumOccupations_Add(&manager, &record);

    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 11:01:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 550;
    record.num_occupations_seriaroom = 450;
    record.num_login_per_min = 25;
    record.num_logout_per_min = 18;
    LogNumOccupations_Add(&manager, &record);

    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 11:02:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 600;
    record.num_occupations_seriaroom = 500;
    record.num_login_per_min = 30;
    record.num_logout_per_min = 20;
    LogNumOccupations_Add(&manager, &record);

    result = LogNumOccupations_DeleteByTimeRange(&manager, "2025-11-16 11:00:00", "2025-11-16 11:01:00");
    ASSERT_GE(result, 0, "按时间范围删除成功");

    result = LogNumOccupations_Exists(&manager, "2025-11-16 11:00:00");
    ASSERT_EQ(result, 0, "11:00:00记录已删除");

    result = LogNumOccupations_Exists(&manager, "2025-11-16 11:01:00");
    ASSERT_EQ(result, 0, "11:01:00记录已删除");

    result = LogNumOccupations_Exists(&manager, "2025-11-16 11:02:00");
    ASSERT_EQ(result, 1, "11:02:00记录仍存在");

    TEST_START("删除过期记录（DeleteOlderThan）");
    /* 添加一条旧记录 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2020-01-01 10:00:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 100;
    record.num_occupations_seriaroom = 50;
    record.num_login_per_min = 5;
    record.num_logout_per_min = 3;
    LogNumOccupations_Add(&manager, &record);

    result = LogNumOccupations_DeleteOlderThan(&manager, 365);
    ASSERT_GE(result, 0, "删除过期记录成功");

    result = LogNumOccupations_Exists(&manager, "2020-01-01 10:00:00");
    ASSERT_EQ(result, 0, "旧记录已删除");

    /* ==================== 查询操作测试 ==================== */

    TEST_START("统计总记录数（CountAll）");
    result = LogNumOccupations_CountAll(&manager);
    ASSERT_GE(result, 0, "统计总记录数成功");
    printf("  当前记录总数: %d\n", result);

    TEST_START("查询平均角色选择界面占用数（GetAvgCharscreen）");
    /* 添加测试数据 */
    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 12:00:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1000;
    record.num_occupations_seriaroom = 800;
    record.num_login_per_min = 40;
    record.num_logout_per_min = 25;
    LogNumOccupations_Add(&manager, &record);

    memset(&record, 0, sizeof(LogNumOccupations));
    strncpy(record.occ_time, "2025-11-16 12:01:00", sizeof(record.occ_time) - 1);
    record.num_occupations_charscreen = 1200;
    record.num_occupations_seriaroom = 900;
    record.num_login_per_min = 45;
    record.num_logout_per_min = 28;
    LogNumOccupations_Add(&manager, &record);

    result = LogNumOccupations_GetAvgCharscreen(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取平均值成功");
    ASSERT_GE(result, 1000, "平均值 >= 1000");
    printf("  平均角色选择界面占用数: %d\n", result);

    TEST_START("查询平均Seria房间占用数（GetAvgSeriaroom）");
    result = LogNumOccupations_GetAvgSeriaroom(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取平均值成功");
    ASSERT_GE(result, 800, "平均值 >= 800");
    printf("  平均Seria房间占用数: %d\n", result);

    TEST_START("查询最高角色选择界面占用数（GetMaxCharscreen）");
    result = LogNumOccupations_GetMaxCharscreen(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取最大值成功");
    ASSERT_GE(result, 1200, "最大值应该是1200");
    printf("  最高角色选择界面占用数: %d\n", result);

    TEST_START("查询最高Seria房间占用数（GetMaxSeriaroom）");
    result = LogNumOccupations_GetMaxSeriaroom(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取最大值成功");
    ASSERT_GE(result, 900, "最大值应该是900");
    printf("  最高Seria房间占用数: %d\n", result);

    TEST_START("查询登录登出比率（GetLoginLogoutRatio）");
    result = LogNumOccupations_GetLoginLogoutRatio(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取登录登出比率成功");
    printf("  登录登出比率: %d%%\n", result);

    TEST_START("查询总登录数（GetTotalLogins）");
    result = LogNumOccupations_GetTotalLogins(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取总登录数成功");
    ASSERT_GE(result, 80, "总登录数 >= 80");
    printf("  总登录数: %d\n", result);

    TEST_START("查询总登出数（GetTotalLogouts）");
    result = LogNumOccupations_GetTotalLogouts(&manager, "2025-11-16 12:00:00", "2025-11-16 12:01:00");
    ASSERT_GE(result, 0, "获取总登出数成功");
    ASSERT_GE(result, 50, "总登出数 >= 50");
    printf("  总登出数: %d\n", result);

    /* ==================== 工具函数测试 ==================== */

    TEST_START("打印服务器占用数信息（PrintInfo）");
    LogNumOccupations_Get(&manager, "2025-11-16 12:00:00", &retrieved);
    printf("  调用 PrintInfo:\n");
    LogNumOccupations_PrintInfo(&retrieved);
    ASSERT_EQ(1, 1, "PrintInfo执行成功（目测输出）");

    /* ==================== 清理测试数据 ==================== */

    TEST_START("删除所有测试记录（Delete）");
    LogNumOccupations_Delete(&manager, "2025-11-16 10:00:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 10:01:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 10:02:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 10:05:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 11:02:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 12:00:00");
    LogNumOccupations_Delete(&manager, "2025-11-16 12:01:00");

    result = LogNumOccupations_Exists(&manager, "2025-11-16 10:00:00");
    ASSERT_EQ(result, 0, "测试记录已全部删除");

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
