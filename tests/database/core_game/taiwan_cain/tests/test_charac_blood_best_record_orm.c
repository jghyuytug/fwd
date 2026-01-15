#include "charac_blood_best_record_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) { \
            test_passed++; \
        } \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT(condition, message) \
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

#define ASSERT_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: expected %d, got %d)\n", message, (int)(b), (int)(a)); \
        } \
    } while(0)

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取最佳记录");

    CharacBloodBestRecord record;
    memset(&record, 0, sizeof(record));
    record.charac_no = 100001;
    record.dungeon_index = 1;
    record.best_round = 50;
    record.best_time = 180000;  /* 3分钟 */

    int ret = CharacBloodBestRecord_Add(manager, &record);
    ASSERT(ret == 0, "添加最佳记录成功");

    CharacBloodBestRecord retrieved;
    memset(&retrieved, 0, sizeof(retrieved));
    ret = CharacBloodBestRecord_Get(manager, 100001, 1, &retrieved);
    ASSERT(ret == 0, "获取最佳记录成功");
    ASSERT_EQ(retrieved.charac_no, 100001, "角色编号正确");
    ASSERT_EQ(retrieved.dungeon_index, 1, "副本索引正确");
    ASSERT_EQ(retrieved.best_round, 50, "最佳回合数正确");
    ASSERT_EQ(retrieved.best_time, 180000, "最佳时间正确");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新最佳记录");

    CharacBloodBestRecord record;
    CharacBloodBestRecord_Get(manager, 100001, 1, &record);
    record.best_round = 60;
    record.best_time = 150000;  /* 2.5分钟 */

    int ret = CharacBloodBestRecord_Update(manager, &record);
    ASSERT(ret == 0, "更新成功");

    CharacBloodBestRecord retrieved;
    CharacBloodBestRecord_Get(manager, 100001, 1, &retrieved);
    ASSERT_EQ(retrieved.best_round, 60, "更新后回合数正确");
    ASSERT_EQ(retrieved.best_time, 150000, "更新后时间正确");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录存在性");

    int exists = CharacBloodBestRecord_Exists(manager, 100001, 1);
    ASSERT(exists == 1, "记录存在");

    exists = CharacBloodBestRecord_Exists(manager, 999999, 999);
    ASSERT(exists == 0, "不存在的记录返回0");
}

void test_get_by_charac(DBConnectionManager* manager) {
    TEST_START("按角色查询所有记录");

    /* 为同一角色添加多个副本记录 */
    CharacBloodBestRecord records[5];
    for (int i = 0; i < 5; i++) {
        memset(&records[i], 0, sizeof(records[i]));
        records[i].charac_no = 100001;
        records[i].dungeon_index = i + 1;
        records[i].best_round = 40 + i * 5;
        records[i].best_time = 200000 - i * 10000;
        CharacBloodBestRecord_Add(manager, &records[i]);
    }

    CharacBloodBestRecord retrieved[10];
    int count = CharacBloodBestRecord_GetByCharac(manager, 100001, retrieved, 10);
    ASSERT(count >= 5, "查询到角色的所有副本记录");
}

void test_get_by_dungeon(DBConnectionManager* manager) {
    TEST_START("按副本查询所有记录");

    /* 为同一副本添加多个角色记录 */
    CharacBloodBestRecord records[5];
    for (int i = 0; i < 5; i++) {
        memset(&records[i], 0, sizeof(records[i]));
        records[i].charac_no = 100001 + i;
        records[i].dungeon_index = 1;
        records[i].best_round = 45 + i * 3;
        records[i].best_time = 180000 + i * 5000;
        CharacBloodBestRecord_Add(manager, &records[i]);
    }

    CharacBloodBestRecord retrieved[10];
    int count = CharacBloodBestRecord_GetByDungeon(manager, 1, retrieved, 10);
    ASSERT(count >= 5, "查询到副本的所有角色记录");
}

void test_get_top_by_time(DBConnectionManager* manager) {
    TEST_START("按时间排行查询");

    CharacBloodBestRecord top_records[10];
    int count = CharacBloodBestRecord_GetTopByTime(manager, 1, 3, top_records, 10);
    ASSERT(count >= 3, "查询到前3名时间排行");

    /* 验证排序：时间递增 */
    if (count >= 2) {
        ASSERT(top_records[0].best_time <= top_records[1].best_time, "时间排序正确（升序）");
    }
}

void test_get_top_by_round(DBConnectionManager* manager) {
    TEST_START("按回合数排行查询");

    CharacBloodBestRecord top_records[10];
    int count = CharacBloodBestRecord_GetTopByRound(manager, 1, 3, top_records, 10);
    ASSERT(count >= 3, "查询到前3名回合数排行");

    /* 验证排序：回合数递减 */
    if (count >= 2) {
        ASSERT(top_records[0].best_round >= top_records[1].best_round, "回合数排序正确（降序）");
    }
}

void test_update_if_better(DBConnectionManager* manager) {
    TEST_START("智能更新最佳记录");

    CharacBloodBestRecord better_record;
    memset(&better_record, 0, sizeof(better_record));
    better_record.charac_no = 100002;
    better_record.dungeon_index = 2;
    better_record.best_round = 70;
    better_record.best_time = 120000;  /* 2分钟 */

    int ret = CharacBloodBestRecord_UpdateIfBetter(manager, &better_record);
    ASSERT(ret == 0, "首次记录插入成功");

    /* 尝试更新为更好的时间 */
    better_record.best_time = 100000;  /* 1分40秒，更快 */
    ret = CharacBloodBestRecord_UpdateIfBetter(manager, &better_record);
    ASSERT(ret == 0, "更好的记录更新成功");

    CharacBloodBestRecord retrieved;
    CharacBloodBestRecord_Get(manager, 100002, 2, &retrieved);
    ASSERT_EQ(retrieved.best_time, 100000, "更新为更好的时间");

    /* 尝试更新为更差的时间（不应更新） */
    better_record.best_time = 150000;
    better_record.best_round = 60;
    ret = CharacBloodBestRecord_UpdateIfBetter(manager, &better_record);
    ASSERT(ret == 0, "更差的记录被拒绝更新");

    CharacBloodBestRecord_Get(manager, 100002, 2, &retrieved);
    ASSERT_EQ(retrieved.best_time, 100000, "保持原有更好的时间");
}

void test_count_by_charac(DBConnectionManager* manager) {
    TEST_START("统计角色副本记录数");

    int count = CharacBloodBestRecord_CountByCharac(manager, 100001);
    ASSERT(count >= 5, "角色至少有5个副本记录");
    printf("  [INFO] 角色100001副本记录数: %d\n", count);
}

void test_count_by_dungeon(DBConnectionManager* manager) {
    TEST_START("统计副本挑战人数");

    int count = CharacBloodBestRecord_CountByDungeon(manager, 1);
    ASSERT(count >= 5, "副本至少有5个角色记录");
    printf("  [INFO] 副本1挑战人数: %d\n", count);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除最佳记录");

    int ret = CharacBloodBestRecord_Delete(manager, 100002, 2);
    ASSERT(ret == 0, "删除成功");

    int exists = CharacBloodBestRecord_Exists(manager, 100002, 2);
    ASSERT(exists == 0, "记录已被删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印记录信息");

    CharacBloodBestRecord record;
    if (CharacBloodBestRecord_Get(manager, 100001, 1, &record) == 0) {
        CharacBloodBestRecord_PrintInfo(&record);
        ASSERT(1, "打印信息成功");
    }
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n========================================\n");
    printf("角色血色副本最佳记录 ORM 测试\n");
    printf("========================================\n");

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

    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_get_by_charac(&manager);
    test_get_by_dungeon(&manager);
    test_get_top_by_time(&manager);
    test_get_top_by_round(&manager);
    test_update_if_better(&manager);
    test_count_by_charac(&manager);
    test_count_by_dungeon(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    if (!current_test_failed) {
        test_passed++;
    }

    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count,
           test_count > 0 ? (test_passed * 100.0 / test_count) : 0);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count,
           assertion_count > 0 ? (assertion_passed * 100.0 / assertion_count) : 0);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
