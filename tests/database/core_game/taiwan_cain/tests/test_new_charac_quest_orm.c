#include "new_charac_quest_orm.h"
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

#define ASSERT_TRUE(condition, message) \
    do { \
        assertion_count++; \
        if (condition) { \
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
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
        } \
    } while(0)

/* 测试数据 */
#define TEST_CHARAC_NO 999001
#define TEST_CHARAC_NO_2 999002

/* 辅助函数：创建测试记录 */
void create_test_record(NewCharacQuest* record, unsigned int charac_no) {
    memset(record, 0, sizeof(NewCharacQuest));

    record->charac_no = charac_no;

    /* 设置 blob 数据 */
    const char* clear_data = "Test Clear Quest Data";
    const char* notify_data = "Test Notify Data";
    record->clear_quest_len = strlen(clear_data);
    memcpy(record->clear_quest, clear_data, record->clear_quest_len);
    record->quest_notify_len = strlen(notify_data);
    memcpy(record->quest_notify, notify_data, record->quest_notify_len);

    /* 设置游戏模式进度 */
    record->play_1 = 100;
    record->play_1_trigger = 1000;
    record->play_2 = 200;
    record->play_2_trigger = 2000;
    record->play_5 = 500;
    record->play_5_trigger = 5000;
    record->play_10 = 1000;
    record->play_10_trigger = 10000;

    /* 设置控制字段 */
    record->auto_clear = 1;
    record->urgent_quest_index = 5;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    NewCharacQuest record, retrieved;
    PlayProgress progress;
    int result;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize database manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("new_charac_quest ORM 测试开始\n");
    printf("========================================\n");

    /* ========== 测试 1: 添加任务记录 ========== */
    TEST_START("添加任务记录");
    create_test_record(&record, TEST_CHARAC_NO);
    result = NewCharacQuest_Add(&manager, &record);
    ASSERT_EQ(result, 0, "添加成功");

    /* ========== 测试 2: 记录存在性检查 ========== */
    TEST_START("记录存在性检查");
    result = NewCharacQuest_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(result, 1, "记录存在");

    /* ========== 测试 3: 获取任务记录 ========== */
    TEST_START("获取任务记录");
    memset(&retrieved, 0, sizeof(NewCharacQuest));
    result = NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO, "角色编号正确");
    ASSERT_EQ(retrieved.auto_clear, 1, "自动完成标志正确");
    ASSERT_EQ(retrieved.urgent_quest_index, 5, "紧急任务索引正确");
    ASSERT_EQ(retrieved.play_1, 100, "Play 1 ID 正确");
    ASSERT_EQ(retrieved.play_1_trigger, 1000, "Play 1 Trigger 正确");
    ASSERT_EQ(retrieved.play_10, 1000, "Play 10 ID 正确");
    ASSERT_EQ(retrieved.play_10_trigger, 10000, "Play 10 Trigger 正确");

    /* ========== 测试 4: Blob 字段验证 ========== */
    TEST_START("Blob 字段验证");
    ASSERT_TRUE(retrieved.clear_quest_len > 0, "clear_quest 有数据");
    ASSERT_TRUE(retrieved.quest_notify_len > 0, "quest_notify 有数据");
    ASSERT_EQ(retrieved.clear_quest_len, 21, "clear_quest 长度正确");
    ASSERT_EQ(retrieved.quest_notify_len, 16, "quest_notify 长度正确");

    /* ========== 测试 5: 更新任务记录 ========== */
    TEST_START("更新任务记录");
    /* 重新设置 blob 数据（因为 Get 不读取 blob 内容） */
    const char* clear_data2 = "Updated Clear Quest";
    const char* notify_data2 = "Updated Notify";
    retrieved.clear_quest_len = strlen(clear_data2);
    memcpy(retrieved.clear_quest, clear_data2, retrieved.clear_quest_len);
    retrieved.quest_notify_len = strlen(notify_data2);
    memcpy(retrieved.quest_notify, notify_data2, retrieved.quest_notify_len);

    retrieved.auto_clear = 0;
    retrieved.urgent_quest_index = 10;
    retrieved.play_1 = 150;
    result = NewCharacQuest_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");

    memset(&retrieved, 0, sizeof(NewCharacQuest));
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(retrieved.auto_clear, 0, "自动完成已更新");
    ASSERT_EQ(retrieved.urgent_quest_index, 10, "紧急任务索引已更新");
    ASSERT_EQ(retrieved.play_1, 150, "Play 1 已更新");

    /* ========== 测试 6: 获取游戏模式进度 ========== */
    TEST_START("获取游戏模式进度");
    memset(&progress, 0, sizeof(PlayProgress));
    result = NewCharacQuest_GetPlayProgress(&manager, TEST_CHARAC_NO, 1, &progress);
    ASSERT_EQ(result, 0, "获取 Play 1 进度成功");
    ASSERT_EQ(progress.play_id, 150, "Play 1 ID 正确");
    ASSERT_EQ(progress.trigger, 1000, "Play 1 Trigger 正确");

    result = NewCharacQuest_GetPlayProgress(&manager, TEST_CHARAC_NO, 10, &progress);
    ASSERT_EQ(result, 0, "获取 Play 10 进度成功");
    ASSERT_EQ(progress.play_id, 1000, "Play 10 ID 正确");
    ASSERT_EQ(progress.trigger, 10000, "Play 10 Trigger 正确");

    /* ========== 测试 7: 更新游戏模式进度 ========== */
    TEST_START("更新游戏模式进度");
    progress.play_id = 777;
    progress.trigger = 7777;
    result = NewCharacQuest_UpdatePlayProgress(&manager, TEST_CHARAC_NO, 15, &progress);
    ASSERT_EQ(result, 0, "更新 Play 15 进度成功");

    memset(&progress, 0, sizeof(PlayProgress));
    NewCharacQuest_GetPlayProgress(&manager, TEST_CHARAC_NO, 15, &progress);
    ASSERT_EQ(progress.play_id, 777, "Play 15 ID 已更新");
    ASSERT_EQ(progress.trigger, 7777, "Play 15 Trigger 已更新");

    /* ========== 测试 8: 检查游戏模式进度存在性 ========== */
    TEST_START("检查游戏模式进度存在性");
    result = NewCharacQuest_HasPlayProgress(&manager, TEST_CHARAC_NO, 1);
    ASSERT_EQ(result, 1, "Play 1 有进度");

    result = NewCharacQuest_HasPlayProgress(&manager, TEST_CHARAC_NO, 3);
    ASSERT_EQ(result, 0, "Play 3 无进度");

    /* ========== 测试 9: 更新紧急任务索引 ========== */
    TEST_START("更新紧急任务索引");
    result = NewCharacQuest_UpdateUrgentQuest(&manager, TEST_CHARAC_NO, 99);
    ASSERT_EQ(result, 0, "更新紧急任务索引成功");

    memset(&retrieved, 0, sizeof(NewCharacQuest));
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(retrieved.urgent_quest_index, 99, "紧急任务索引已更新");

    /* ========== 测试 10: 设置自动完成标志 ========== */
    TEST_START("设置自动完成标志");
    result = NewCharacQuest_SetAutoClear(&manager, TEST_CHARAC_NO, 1);
    ASSERT_EQ(result, 0, "设置自动完成成功");

    memset(&retrieved, 0, sizeof(NewCharacQuest));
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(retrieved.auto_clear, 1, "自动完成标志已更新");

    /* ========== 测试 11: 统计总记录数 ========== */
    TEST_START("统计总记录数");
    result = NewCharacQuest_Count(&manager);
    ASSERT_TRUE(result >= 1, "总记录数至少为1");
    printf("  当前总记录数: %d\n", result);

    /* ========== 测试 12: 按紧急任务索引统计 ========== */
    TEST_START("按紧急任务索引统计");
    result = NewCharacQuest_CountByUrgentQuest(&manager, 99);
    ASSERT_TRUE(result >= 1, "紧急任务索引=99 的记录至少1条");
    printf("  紧急任务索引=99 的记录数: %d\n", result);

    /* ========== 测试 13: 按自动完成统计 ========== */
    TEST_START("按自动完成统计");
    result = NewCharacQuest_CountByAutoClear(&manager);
    ASSERT_TRUE(result >= 1, "启用自动完成的记录至少1条");
    printf("  启用自动完成的记录数: %d\n", result);

    /* ========== 测试 14: 添加第二条记录并清除所有游戏模式 ========== */
    TEST_START("添加第二条记录并清除所有游戏模式");
    create_test_record(&record, TEST_CHARAC_NO_2);
    NewCharacQuest_Add(&manager, &record);

    result = NewCharacQuest_ClearAllPlay(&manager, TEST_CHARAC_NO_2);
    ASSERT_EQ(result, 0, "清除所有游戏模式成功");

    memset(&retrieved, 0, sizeof(NewCharacQuest));
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO_2, &retrieved);
    ASSERT_EQ(retrieved.play_1, 0, "Play 1 已清除");
    ASSERT_EQ(retrieved.play_1_trigger, 0, "Play 1 Trigger 已清除");
    ASSERT_EQ(retrieved.play_2, 0, "Play 2 已清除");
    ASSERT_EQ(retrieved.play_5, 0, "Play 5 已清除");
    ASSERT_EQ(retrieved.play_10, 0, "Play 10 已清除");

    /* ========== 测试 15: 边界测试 - 无效游戏模式索引 ========== */
    TEST_START("边界测试 - 无效游戏模式索引");
    result = NewCharacQuest_GetPlayProgress(&manager, TEST_CHARAC_NO, 0, &progress);
    ASSERT_EQ(result, -1, "索引0返回错误");

    result = NewCharacQuest_GetPlayProgress(&manager, TEST_CHARAC_NO, 21, &progress);
    ASSERT_EQ(result, -1, "索引21返回错误");

    /* ========== 测试 16: 打印信息 ========== */
    TEST_START("打印信息测试");
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    printf("  打印角色 %u 的任务信息:\n", TEST_CHARAC_NO);
    NewCharacQuest_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "打印信息执行完成");

    /* ========== 测试 17: 删除记录 ========== */
    TEST_START("删除记录");
    result = NewCharacQuest_Delete(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(result, 0, "删除第一条记录成功");

    result = NewCharacQuest_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(result, 0, "第一条记录已不存在");

    result = NewCharacQuest_Delete(&manager, TEST_CHARAC_NO_2);
    ASSERT_EQ(result, 0, "删除第二条记录成功");

    result = NewCharacQuest_Exists(&manager, TEST_CHARAC_NO_2);
    ASSERT_EQ(result, 0, "第二条记录已不存在");

    /* ========== 测试 18: REPLACE INTO 幂等性测试 ========== */
    TEST_START("REPLACE INTO 幂等性测试");
    create_test_record(&record, TEST_CHARAC_NO);
    NewCharacQuest_Add(&manager, &record);

    record.urgent_quest_index = 88;
    result = NewCharacQuest_Add(&manager, &record);
    ASSERT_EQ(result, 0, "重复添加成功（REPLACE）");

    memset(&retrieved, 0, sizeof(NewCharacQuest));
    NewCharacQuest_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(retrieved.urgent_quest_index, 88, "数据已更新");

    result = NewCharacQuest_Count(&manager);
    printf("  总记录数: %d（未重复）\n", result);

    /* 清理最后的测试数据 */
    NewCharacQuest_Delete(&manager, TEST_CHARAC_NO);

    /* 统计最后一个测试 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试完成\n");
    printf("========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
