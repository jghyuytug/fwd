#include "pvp_grade_expand_orm.h"
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
    PvpGradeExpand record, retrieved;
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
    printf("  PVP等级扩展系统 ORM 测试套件\n");
    printf("========================================\n");

    /* ==================== CRUD 操作测试 ==================== */

    TEST_START("添加PVP等级扩展记录（Add）");
    memset(&record, 0, sizeof(PvpGradeExpand));
    record.charac_no = 100001;
    record.pvp_grade = 5;
    record.pvp_point = 1500;
    strncpy(record.last_play_time, "2025-11-16 10:00:00", sizeof(record.last_play_time) - 1);

    result = PvpGradeExpand_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第一条记录成功");

    /* 添加第二条记录 */
    memset(&record, 0, sizeof(PvpGradeExpand));
    record.charac_no = 100002;
    record.pvp_grade = 8;
    record.pvp_point = 3200;
    strncpy(record.last_play_time, "2025-11-16 11:00:00", sizeof(record.last_play_time) - 1);

    result = PvpGradeExpand_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第二条记录成功");

    /* 添加第三条记录 */
    memset(&record, 0, sizeof(PvpGradeExpand));
    record.charac_no = 100003;
    record.pvp_grade = 3;
    record.pvp_point = 800;
    strncpy(record.last_play_time, "2025-11-16 12:00:00", sizeof(record.last_play_time) - 1);

    result = PvpGradeExpand_Add(&manager, &record);
    ASSERT_EQ(result, 0, "插入第三条记录成功");

    TEST_START("获取PVP等级扩展记录（Get）");
    memset(&retrieved, 0, sizeof(PvpGradeExpand));
    result = PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_EQ(retrieved.charac_no, 100001, "角色编号匹配");
    ASSERT_EQ(retrieved.pvp_grade, 5, "PVP等级匹配");
    ASSERT_EQ(retrieved.pvp_point, 1500, "PVP积分匹配");
    ASSERT_STR_EQ(retrieved.last_play_time, "2025-11-16 10:00:00", "最后游戏时间匹配");

    TEST_START("检查记录存在（Exists）");
    result = PvpGradeExpand_Exists(&manager, 100001);
    ASSERT_EQ(result, 1, "记录存在");

    result = PvpGradeExpand_Exists(&manager, 999999);
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    TEST_START("更新PVP等级扩展记录（Update）");
    memset(&retrieved, 0, sizeof(PvpGradeExpand));
    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    retrieved.pvp_grade = 6;
    retrieved.pvp_point = 1800;
    strncpy(retrieved.last_play_time, "2025-11-16 13:00:00", sizeof(retrieved.last_play_time) - 1);

    result = PvpGradeExpand_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(PvpGradeExpand));
    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(retrieved.pvp_grade, 6, "PVP等级已更新");
    ASSERT_EQ(retrieved.pvp_point, 1800, "PVP积分已更新");
    ASSERT_STR_EQ(retrieved.last_play_time, "2025-11-16 13:00:00", "最后游戏时间已更新");

    /* ==================== 专项操作测试 ==================== */

    TEST_START("更新PVP积分（UpdatePoints）");
    result = PvpGradeExpand_UpdatePoints(&manager, 100001, 200); /* 增加200积分 */
    ASSERT_EQ(result, 0, "增加积分成功");

    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(retrieved.pvp_point, 2000, "积分已增加（1800+200）");

    result = PvpGradeExpand_UpdatePoints(&manager, 100001, -500); /* 减少500积分 */
    ASSERT_EQ(result, 0, "减少积分成功");

    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(retrieved.pvp_point, 1500, "积分已减少（2000-500）");

    TEST_START("更新PVP等级（UpdateGrade）");
    result = PvpGradeExpand_UpdateGrade(&manager, 100001, 7);
    ASSERT_EQ(result, 0, "更新等级成功");

    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(retrieved.pvp_grade, 7, "等级已更新为7");

    TEST_START("更新最后游戏时间（UpdateLastPlayTime）");
    result = PvpGradeExpand_UpdateLastPlayTime(&manager, 100001);
    ASSERT_EQ(result, 0, "更新最后游戏时间成功");

    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_NEQ(strcmp(retrieved.last_play_time, "2025-11-16 13:00:00"), 0, "最后游戏时间已更新（与旧时间不同）");

    TEST_START("重置PVP数据（Reset）");
    result = PvpGradeExpand_Reset(&manager, 100001);
    ASSERT_EQ(result, 0, "重置成功");

    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    ASSERT_EQ(retrieved.pvp_grade, 0, "等级已重置为0");
    ASSERT_EQ(retrieved.pvp_point, 0, "积分已重置为0");

    /* 恢复数据用于后续测试 */
    retrieved.pvp_grade = 7;
    retrieved.pvp_point = 2500;
    PvpGradeExpand_Update(&manager, &retrieved);

    /* ==================== 查询操作测试 ==================== */

    TEST_START("按积分排名（GetTopRanking）");
    PvpGradeExpand rankings[10];
    memset(rankings, 0, sizeof(rankings));

    result = PvpGradeExpand_GetTopRanking(&manager, 10, rankings);
    ASSERT_GE(result, 3, "返回至少3条记录");
    ASSERT_EQ(rankings[0].charac_no, 100002, "第1名是100002（积分3200）");
    ASSERT_EQ(rankings[0].pvp_point, 3200, "第1名积分3200");
    ASSERT_EQ(rankings[1].charac_no, 100001, "第2名是100001（积分2500）");
    ASSERT_EQ(rankings[2].charac_no, 100003, "第3名是100003（积分800）");

    TEST_START("统计指定等级玩家数（CountByGrade）");
    result = PvpGradeExpand_CountByGrade(&manager, 7);
    ASSERT_EQ(result, 1, "等级7有1个玩家");

    result = PvpGradeExpand_CountByGrade(&manager, 8);
    ASSERT_EQ(result, 1, "等级8有1个玩家");

    result = PvpGradeExpand_CountByGrade(&manager, 99);
    ASSERT_EQ(result, 0, "等级99没有玩家");

    TEST_START("查询不活跃玩家数（CountInactivePlayers）");
    /* 添加一个很久未登录的玩家 */
    memset(&record, 0, sizeof(PvpGradeExpand));
    record.charac_no = 100004;
    record.pvp_grade = 1;
    record.pvp_point = 100;
    strncpy(record.last_play_time, "2020-01-01 10:00:00", sizeof(record.last_play_time) - 1);
    PvpGradeExpand_Add(&manager, &record);

    result = PvpGradeExpand_CountInactivePlayers(&manager, 30); /* 30天未登录 */
    ASSERT_GE(result, 1, "至少有1个不活跃玩家");

    TEST_START("按等级范围查询（CountByGradeRange）");
    result = PvpGradeExpand_CountByGradeRange(&manager, 3, 7);
    ASSERT_EQ(result, 2, "等级3-7有2个玩家（等级3和等级7）");

    result = PvpGradeExpand_CountByGradeRange(&manager, 8, 10);
    ASSERT_EQ(result, 1, "等级8-10有1个玩家（等级8）");

    TEST_START("按积分范围查询（CountByPointRange）");
    result = PvpGradeExpand_CountByPointRange(&manager, 1000, 3000);
    ASSERT_EQ(result, 1, "积分1000-3000有1个玩家（2500积分）");

    result = PvpGradeExpand_CountByPointRange(&manager, 3000, 5000);
    ASSERT_EQ(result, 1, "积分3000-5000有1个玩家（3200积分）");

    TEST_START("查询总玩家数（CountAll）");
    result = PvpGradeExpand_CountAll(&manager);
    ASSERT_EQ(result, 4, "总共有4个玩家");

    /* ==================== 工具函数测试 ==================== */

    TEST_START("打印PVP等级扩展信息（PrintInfo）");
    PvpGradeExpand_Get(&manager, 100001, &retrieved);
    printf("  调用 PrintInfo:\n");
    PvpGradeExpand_PrintInfo(&retrieved);
    ASSERT_EQ(1, 1, "PrintInfo执行成功（目测输出）");

    /* ==================== 清理测试数据 ==================== */

    TEST_START("删除测试记录（Delete）");
    result = PvpGradeExpand_Delete(&manager, 100001);
    ASSERT_EQ(result, 0, "删除第一条记录成功");

    result = PvpGradeExpand_Delete(&manager, 100002);
    ASSERT_EQ(result, 0, "删除第二条记录成功");

    result = PvpGradeExpand_Delete(&manager, 100003);
    ASSERT_EQ(result, 0, "删除第三条记录成功");

    result = PvpGradeExpand_Delete(&manager, 100004);
    ASSERT_EQ(result, 0, "删除第四条记录成功");

    result = PvpGradeExpand_Exists(&manager, 100001);
    ASSERT_EQ(result, 0, "记录已被删除");

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
