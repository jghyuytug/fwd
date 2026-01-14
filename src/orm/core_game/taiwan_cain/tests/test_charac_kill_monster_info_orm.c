#include "charac_kill_monster_info_orm.h"
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
    TEST_START("添加和获取击杀怪物信息");

    CharacKillMonsterInfo record, retrieved;
    unsigned char boss_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    unsigned char named_data[] = {0x0A, 0x0B, 0x0C};
    unsigned char apc_data[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

    memset(&record, 0, sizeof(CharacKillMonsterInfo));
    record.charac_no = 600001;
    record.boss_info = boss_data;
    record.boss_info_len = sizeof(boss_data);
    record.named_info = named_data;
    record.named_info_len = sizeof(named_data);
    record.apc_boss_info = apc_data;
    record.apc_boss_info_len = sizeof(apc_data);

    ASSERT_EQ(CharacKillMonsterInfo_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.boss_info_len, record.boss_info_len, "Boss信息长度匹配");
    ASSERT_EQ(retrieved.named_info_len, record.named_info_len, "Named信息长度匹配");
    ASSERT_EQ(retrieved.apc_boss_info_len, record.apc_boss_info_len, "APC Boss信息长度匹配");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新击杀怪物信息");

    CharacKillMonsterInfo record, retrieved;
    unsigned char new_boss_data[] = {0x10, 0x20, 0x30, 0x40};
    unsigned char new_named_data[] = {0xAA, 0xBB};
    unsigned char new_apc_data[] = {0x99, 0x88, 0x77};

    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600001, &record), 0, "获取已存在记录");
    CharacKillMonsterInfo_Free(&record);

    memset(&record, 0, sizeof(CharacKillMonsterInfo));
    record.charac_no = 600001;
    record.boss_info = new_boss_data;
    record.boss_info_len = sizeof(new_boss_data);
    record.named_info = new_named_data;
    record.named_info_len = sizeof(new_named_data);
    record.apc_boss_info = new_apc_data;
    record.apc_boss_info_len = sizeof(new_apc_data);

    ASSERT_EQ(CharacKillMonsterInfo_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, record.charac_no, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.boss_info_len, 4, "Boss信息长度已更新");
    ASSERT_EQ(retrieved.named_info_len, 2, "Named信息长度已更新");
    ASSERT_EQ(retrieved.apc_boss_info_len, 3, "APC Boss信息长度已更新");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查击杀怪物信息是否存在");

    ASSERT_EQ(CharacKillMonsterInfo_Exists(manager, 600001), 1, "记录存在");
    ASSERT_EQ(CharacKillMonsterInfo_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_boss_info(DBConnectionManager* manager) {
    TEST_START("单独更新Boss信息");

    CharacKillMonsterInfo retrieved;
    unsigned char boss_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    ASSERT_EQ(CharacKillMonsterInfo_UpdateBossInfo(manager, 600001, boss_data, sizeof(boss_data)), 0,
              "更新Boss信息成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.boss_info_len, 6, "Boss信息长度已更新为6");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_update_named_info(DBConnectionManager* manager) {
    TEST_START("单独更新Named怪物信息");

    CharacKillMonsterInfo retrieved;
    unsigned char named_data[] = {0x11, 0x22, 0x33, 0x44};

    ASSERT_EQ(CharacKillMonsterInfo_UpdateNamedInfo(manager, 600001, named_data, sizeof(named_data)), 0,
              "更新Named信息成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.named_info_len, 4, "Named信息长度已更新为4");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_update_apc_boss_info(DBConnectionManager* manager) {
    TEST_START("单独更新APC Boss信息");

    CharacKillMonsterInfo retrieved;
    unsigned char apc_data[] = {0x55, 0x66, 0x77, 0x88, 0x99};

    ASSERT_EQ(CharacKillMonsterInfo_UpdateApcBossInfo(manager, 600001, apc_data, sizeof(apc_data)), 0,
              "更新APC Boss信息成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.apc_boss_info_len, 5, "APC Boss信息长度已更新为5");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_empty_blob(DBConnectionManager* manager) {
    TEST_START("处理空blob数据");

    CharacKillMonsterInfo record, retrieved;
    unsigned char empty_data[] = {};

    memset(&record, 0, sizeof(CharacKillMonsterInfo));
    record.charac_no = 600002;
    record.boss_info = empty_data;
    record.boss_info_len = 0;
    record.named_info = empty_data;
    record.named_info_len = 0;
    record.apc_boss_info = empty_data;
    record.apc_boss_info_len = 0;

    ASSERT_EQ(CharacKillMonsterInfo_Add(manager, &record), 0, "添加空数据记录成功");
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600002, &retrieved), 0, "获取空数据记录");
    ASSERT_EQ(retrieved.boss_info_len, 0, "Boss信息长度为0");
    ASSERT_EQ(retrieved.named_info_len, 0, "Named信息长度为0");
    ASSERT_EQ(retrieved.apc_boss_info_len, 0, "APC Boss信息长度为0");

    CharacKillMonsterInfo_Free(&retrieved);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除击杀怪物信息");

    ASSERT_EQ(CharacKillMonsterInfo_Delete(manager, 600001), 0, "删除记录成功");
    ASSERT_EQ(CharacKillMonsterInfo_Exists(manager, 600001), 0, "记录已删除");

    ASSERT_EQ(CharacKillMonsterInfo_Delete(manager, 600002), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印击杀怪物信息");

    CharacKillMonsterInfo record, retrieved;
    unsigned char test_data[] = {0x12, 0x34, 0x56, 0x78};

    memset(&record, 0, sizeof(CharacKillMonsterInfo));
    record.charac_no = 600003;
    record.boss_info = test_data;
    record.boss_info_len = sizeof(test_data);
    record.named_info = test_data;
    record.named_info_len = sizeof(test_data);
    record.apc_boss_info = test_data;
    record.apc_boss_info_len = sizeof(test_data);

    CharacKillMonsterInfo_Add(manager, &record);
    ASSERT_EQ(CharacKillMonsterInfo_Get(manager, 600003, &retrieved), 0, "获取记录");

    printf("\n");
    CharacKillMonsterInfo_PrintInfo(&retrieved);

    CharacKillMonsterInfo_Free(&retrieved);
    CharacKillMonsterInfo_Delete(manager, 600003);

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
    printf("角色击杀怪物信息 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_boss_info(&manager);
    test_update_named_info(&manager);
    test_update_apc_boss_info(&manager);
    test_empty_blob(&manager);
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
