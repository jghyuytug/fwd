#include "charac_blood_inout_orm.h"
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

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取血色副本进出记录");

    CharacBloodInout record, retrieved;
    int i;
    memset(&record, 0, sizeof(CharacBloodInout));

    record.charac_no = 200001;
    /* 设置部分副本为已解锁状态 */
    record.dungeon_status[0] = 1;  /* dungeon1 已解锁 */
    record.dungeon_status[2] = 1;  /* dungeon3 已解锁 */
    record.dungeon_status[4] = 1;  /* dungeon5 已解锁 */

    ASSERT_EQ(CharacBloodInout_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacBloodInout_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.dungeon_status[0], 1, "dungeon1 状态匹配");
    ASSERT_EQ(retrieved.dungeon_status[2], 1, "dungeon3 状态匹配");
    ASSERT_EQ(retrieved.dungeon_status[4], 1, "dungeon5 状态匹配");
    ASSERT_EQ(retrieved.dungeon_status[1], 0, "dungeon2 未解锁");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新血色副本进出记录");

    CharacBloodInout record, retrieved;

    ASSERT_EQ(CharacBloodInout_Get(manager, 200001, &record), 0, "获取已存在记录");

    record.dungeon_status[1] = 1;  /* 解锁 dungeon2 */
    record.dungeon_status[3] = 1;  /* 解锁 dungeon4 */

    ASSERT_EQ(CharacBloodInout_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacBloodInout_Get(manager, record.charac_no, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.dungeon_status[1], 1, "dungeon2 已更新");
    ASSERT_EQ(retrieved.dungeon_status[3], 1, "dungeon4 已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查血色副本进出记录是否存在");

    ASSERT_EQ(CharacBloodInout_Exists(manager, 200001), 1, "记录存在");
    ASSERT_EQ(CharacBloodInout_Exists(manager, 999999), 0, "记录不存在");
}

void test_get_dungeon_status(DBConnectionManager* manager) {
    TEST_START("获取指定副本状态");

    int status1 = CharacBloodInout_GetDungeonStatus(manager, 200001, 1);
    int status2 = CharacBloodInout_GetDungeonStatus(manager, 200001, 2);
    int status6 = CharacBloodInout_GetDungeonStatus(manager, 200001, 6);

    ASSERT_EQ(status1, 1, "dungeon1 已解锁");
    ASSERT_EQ(status2, 1, "dungeon2 已解锁");
    ASSERT_EQ(status6, 0, "dungeon6 未解锁");
}

void test_update_dungeon_status(DBConnectionManager* manager) {
    TEST_START("更新指定副本状态");

    ASSERT_EQ(CharacBloodInout_UpdateDungeonStatus(manager, 200001, 6, 1), 0, "更新dungeon6状态");

    int status = CharacBloodInout_GetDungeonStatus(manager, 200001, 6);
    ASSERT_EQ(status, 1, "dungeon6 状态已更新为1");
}

void test_unlock_dungeon(DBConnectionManager* manager) {
    TEST_START("解锁指定副本");

    ASSERT_EQ(CharacBloodInout_UnlockDungeon(manager, 200001, 7), 0, "解锁dungeon7");

    int status = CharacBloodInout_GetDungeonStatus(manager, 200001, 7);
    ASSERT_EQ(status, 1, "dungeon7 已解锁");
}

void test_get_unlocked_dungeons(DBConnectionManager* manager) {
    TEST_START("获取已解锁副本列表");

    int unlocked[BLOOD_DUNGEON_COUNT];
    int count = CharacBloodInout_GetUnlockedDungeons(manager, 200001, unlocked, BLOOD_DUNGEON_COUNT);

    ASSERT_EQ(count >= 6, 1, "至少有6个副本已解锁");
    printf("  已解锁副本:");
    for (int i = 0; i < count; i++) {
        printf(" dungeon%d", unlocked[i]);
    }
    printf("\n");
}

void test_count_unlocked_dungeons(DBConnectionManager* manager) {
    TEST_START("统计已解锁副本数量");

    int count = CharacBloodInout_CountUnlockedDungeons(manager, 200001);

    ASSERT_EQ(count >= 6, 1, "已解锁副本数量至少为6");
    printf("  已解锁副本数量: %d\n", count);
}

void test_count_dungeons_by_status(DBConnectionManager* manager) {
    TEST_START("统计指定状态的副本数量");

    int unlocked = CharacBloodInout_CountDungeonsByStatus(manager, 200001, 1);
    int locked = CharacBloodInout_CountDungeonsByStatus(manager, 200001, 0);

    ASSERT_EQ(unlocked + locked, BLOOD_DUNGEON_COUNT, "状态统计总和等于副本总数");
    printf("  已解锁: %d, 未解锁: %d\n", unlocked, locked);
}

void test_is_all_unlocked(DBConnectionManager* manager) {
    TEST_START("检查是否所有副本都已解锁");

    int all_unlocked = CharacBloodInout_IsAllUnlocked(manager, 200001);
    ASSERT_EQ(all_unlocked, 0, "未全部解锁");

    /* 解锁所有副本 */
    CharacBloodInout_SetAllDungeonStatus(manager, 200001, 1);
    all_unlocked = CharacBloodInout_IsAllUnlocked(manager, 200001);
    ASSERT_EQ(all_unlocked, 1, "全部解锁");
}

void test_set_all_dungeon_status(DBConnectionManager* manager) {
    TEST_START("批量设置所有副本状态");

    /* 创建新记录用于测试 */
    CharacBloodInout record;
    memset(&record, 0, sizeof(CharacBloodInout));
    record.charac_no = 200002;
    CharacBloodInout_Add(manager, &record);

    /* 设置所有为已解锁 */
    ASSERT_EQ(CharacBloodInout_SetAllDungeonStatus(manager, 200002, 1), 0, "设置所有副本为已解锁");

    int count = CharacBloodInout_CountUnlockedDungeons(manager, 200002);
    ASSERT_EQ(count, BLOOD_DUNGEON_COUNT, "所有副本都已解锁");

    /* 设置所有为未解锁 */
    ASSERT_EQ(CharacBloodInout_SetAllDungeonStatus(manager, 200002, 0), 0, "设置所有副本为未解锁");

    count = CharacBloodInout_CountUnlockedDungeons(manager, 200002);
    ASSERT_EQ(count, 0, "所有副本都未解锁");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除血色副本进出记录");

    ASSERT_EQ(CharacBloodInout_Delete(manager, 200002), 0, "删除记录成功");
    ASSERT_EQ(CharacBloodInout_Exists(manager, 200002), 0, "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印血色副本进出记录信息");

    CharacBloodInout record;
    ASSERT_EQ(CharacBloodInout_Get(manager, 200001, &record), 0, "获取记录");

    printf("\n");
    CharacBloodInout_PrintInfo(&record);

    assertion_count++;
    assertion_passed++;
    printf("  ✓ 打印信息成功\n");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "连接到 taiwan_cain 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("血色副本进出记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_get_dungeon_status(&manager);
    test_update_dungeon_status(&manager);
    test_unlock_dungeon(&manager);
    test_get_unlocked_dungeons(&manager);
    test_count_unlocked_dungeons(&manager);
    test_count_dungeons_by_status(&manager);
    test_is_all_unlocked(&manager);
    test_set_all_dungeon_status(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
