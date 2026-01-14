#include "charac_tower_record_orm.h"
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

#define ASSERT_TRUE(cond, message) \
    do { \
        assertion_count++; \
        if (cond) { \
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
        if (strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a, b); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取塔记录");

    CharacTowerRecord record, retrieved;
    memset(&record, 0, sizeof(CharacTowerRecord));

    record.charac_no = 5000001;
    record.tower_index = 1;
    strcpy(record.member_info_1, "Player1");
    record.stage_1 = 50;
    record.play_time_1 = 3600;
    strcpy(record.occ_time_1, "2025-11-16 10:00:00");

    strcpy(record.member_info_2, "Player1|Player2");
    record.stage_2 = 60;
    record.play_time_2 = 4500;
    strcpy(record.occ_time_2, "2025-11-16 11:00:00");

    strcpy(record.member_info_3, "Player1|Player2|Player3");
    record.stage_3 = 70;
    record.play_time_3 = 5400;
    strcpy(record.occ_time_3, "2025-11-16 12:00:00");

    strcpy(record.member_info_4, "Player1|Player2|Player3|Player4");
    record.stage_4 = 80;
    record.play_time_4 = 6300;
    strcpy(record.occ_time_4, "2025-11-16 13:00:00");

    ASSERT_EQ(CharacTowerRecord_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTowerRecord_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.tower_index, record.tower_index, "塔索引匹配");
    ASSERT_STR_EQ(retrieved.member_info_1, record.member_info_1, "1人成员信息匹配");
    ASSERT_EQ(retrieved.stage_1, record.stage_1, "1人阶段匹配");
    ASSERT_EQ(retrieved.play_time_1, record.play_time_1, "1人游戏时间匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新塔记录");

    CharacTowerRecord record, retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &record), 0, "获取已存在记录");

    record.tower_index = 2;
    strcpy(record.member_info_1, "UpdatedPlayer1");
    record.stage_1 = 55;
    record.play_time_1 = 3500;

    ASSERT_EQ(CharacTowerRecord_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.tower_index, 2, "塔索引已更新");
    ASSERT_STR_EQ(retrieved.member_info_1, "UpdatedPlayer1", "1人成员信息已更新");
    ASSERT_EQ(retrieved.stage_1, 55, "1人阶段已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_EQ(CharacTowerRecord_Exists(manager, 5000001), 1, "记录存在");
    ASSERT_EQ(CharacTowerRecord_Exists(manager, 9999999), 0, "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条塔记录");

    CharacTowerRecord record2, record3;
    memset(&record2, 0, sizeof(CharacTowerRecord));
    memset(&record3, 0, sizeof(CharacTowerRecord));

    record2.charac_no = 5000002;
    record2.tower_index = 1;
    strcpy(record2.member_info_1, "Player2");
    record2.stage_1 = 45;
    record2.play_time_1 = 3000;
    strcpy(record2.occ_time_1, "2025-11-16 10:30:00");

    record3.charac_no = 5000003;
    record3.tower_index = 2;
    strcpy(record3.member_info_2, "Player3A|Player3B");
    record3.stage_2 = 65;
    record3.play_time_2 = 4000;
    strcpy(record3.occ_time_2, "2025-11-16 11:30:00");

    ASSERT_EQ(CharacTowerRecord_Add(manager, &record2), 0, "添加记录2成功");
    ASSERT_EQ(CharacTowerRecord_Add(manager, &record3), 0, "添加记录3成功");
}

void test_update_member_record(DBConnectionManager* manager) {
    TEST_START("更新指定人数规模的记录");

    ASSERT_EQ(CharacTowerRecord_UpdateMemberRecord(manager, 5000001, 2,
                                                    "NewPlayer1|NewPlayer2",
                                                    65, 4200, "2025-11-16 14:00:00"), 0, "更新2人记录成功");

    CharacTowerRecord retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.member_info_2, "NewPlayer1|NewPlayer2", "2人成员信息已更新");
    ASSERT_EQ(retrieved.stage_2, 65, "2人阶段已更新");
    ASSERT_EQ(retrieved.play_time_2, 4200, "2人游戏时间已更新");
}

void test_update_member_info(DBConnectionManager* manager) {
    TEST_START("更新成员信息");

    ASSERT_EQ(CharacTowerRecord_UpdateMemberInfo(manager, 5000001, 3, "P1|P2|P3"), 0, "更新3人成员信息成功");

    CharacTowerRecord retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.member_info_3, "P1|P2|P3", "3人成员信息已更新");
}

void test_update_stage(DBConnectionManager* manager) {
    TEST_START("更新阶段");

    ASSERT_EQ(CharacTowerRecord_UpdateStage(manager, 5000001, 1, 60), 0, "更新1人阶段成功");

    CharacTowerRecord retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.stage_1, 60, "1人阶段已更新");
}

void test_update_play_time(DBConnectionManager* manager) {
    TEST_START("更新游戏时间");

    ASSERT_EQ(CharacTowerRecord_UpdatePlayTime(manager, 5000001, 1, 3200), 0, "更新1人游戏时间成功");

    CharacTowerRecord retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.play_time_1, 3200, "1人游戏时间已更新");
}

void test_update_tower_index(DBConnectionManager* manager) {
    TEST_START("更新塔索引");

    ASSERT_EQ(CharacTowerRecord_UpdateTowerIndex(manager, 5000001, 3), 0, "更新塔索引成功");

    CharacTowerRecord retrieved;
    ASSERT_EQ(CharacTowerRecord_Get(manager, 5000001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.tower_index, 3, "塔索引已更新");
}

void test_get_by_tower_index(DBConnectionManager* manager) {
    TEST_START("获取指定塔索引的所有记录");

    CharacTowerRecord records[10];
    int count = CharacTowerRecord_GetByTowerIndex(manager, 2, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_EQ(records[0].tower_index, 2, "塔索引正确");
}

void test_get_fastest_by_member_size(DBConnectionManager* manager) {
    TEST_START("获取指定人数规模最快通关记录");

    CharacTowerRecord records[5];
    int count = CharacTowerRecord_GetFastestByMemberSize(manager, 1, 5, records);

    ASSERT_TRUE(count >= 1, "至少找到1条1人记录");
    if (count >= 2) {
        ASSERT_TRUE(records[0].play_time_1 <= records[1].play_time_1, "按游戏时间升序排列");
    }
}

void test_get_count_by_tower_index(DBConnectionManager* manager) {
    TEST_START("获取指定塔的记录总数");

    int count = CharacTowerRecord_GetCountByTowerIndex(manager, 2);
    ASSERT_TRUE(count >= 1, "塔2至少有1条记录");
}

void test_delete_by_tower_index(DBConnectionManager* manager) {
    TEST_START("删除指定塔索引的所有记录");

    /* 先添加一条塔4的记录用于删除测试 */
    CharacTowerRecord temp;
    memset(&temp, 0, sizeof(CharacTowerRecord));
    temp.charac_no = 5000099;
    temp.tower_index = 4;
    CharacTowerRecord_Add(manager, &temp);

    ASSERT_EQ(CharacTowerRecord_DeleteByTowerIndex(manager, 4), 0, "删除塔4的记录成功");

    int count = CharacTowerRecord_GetCountByTowerIndex(manager, 4);
    ASSERT_EQ(count, 0, "塔4的记录已删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除单条记录");

    ASSERT_EQ(CharacTowerRecord_Delete(manager, 5000002), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerRecord_Exists(manager, 5000002), 0, "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印塔记录信息");

    CharacTowerRecord record;
    if (CharacTowerRecord_Get(manager, 5000001, &record) == 0) {
        printf("\n");
        CharacTowerRecord_PrintInfo(&record);
        assertion_count++;
        assertion_passed++;
        printf("  ✓ 打印信息成功\n");
    } else {
        assertion_count++;
        current_test_failed = 1;
        printf("  ✗ 获取记录失败\n");
    }
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
    printf("角色塔记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_update_member_record(&manager);
    test_update_member_info(&manager);
    test_update_stage(&manager);
    test_update_play_time(&manager);
    test_update_tower_index(&manager);
    test_get_by_tower_index(&manager);
    test_get_fastest_by_member_size(&manager);
    test_get_count_by_tower_index(&manager);
    test_delete_by_tower_index(&manager);
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
