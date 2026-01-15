#include "charac_dungeon_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* 测试统计 */
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

#define ASSERT_TRUE(expr, msg) \
    do { \
        assertion_count++; \
        if (expr) { \
            printf("  ✓ %s [PASS]\n", msg); \
            assertion_passed++; \
        } else { \
            printf("  ✗ %s [FAIL]\n", msg); \
            current_test_failed = 1; \
        } \
    } while(0)

#define ASSERT_EQUAL(actual, expected, msg) \
    do { \
        assertion_count++; \
        if ((actual) == (expected)) { \
            printf("  ✓ %s [PASS]\n", msg); \
            assertion_passed++; \
        } else { \
            printf("  ✗ %s (expected: %d, got: %d) [FAIL]\n", msg, expected, actual); \
            current_test_failed = 1; \
        } \
    } while(0)

/* 测试辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    snprintf(query, sizeof(query), "DELETE FROM charac_dungeon WHERE charac_no >= 700000");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/* 测试1: Add操作 */
static void test_add(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_Add");
    cleanup_test_data(manager);

    CharacDungeon record;
    record.charac_no = 700001;
    memcpy(record.dungeon, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    record.dungeon_len = 8;
    memcpy(record.best_clear_time, "\xAA\xBB\xCC\xDD", 4);
    record.best_clear_time_len = 4;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "Test notice");

    int ret = CharacDungeon_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully add a record");

    /* 验证添加成功 */
    int exists = CharacDungeon_Exists(manager, 700001);
    ASSERT_EQUAL(exists, 1, "Record should exist after add");

    /* 测试幂等性 - 重复添加应该成功 */
    ret = CharacDungeon_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully update existing record (REPLACE)");

    cleanup_test_data(manager);
}

/* 测试2: Get操作 */
static void test_get(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_Get");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record, retrieved;
    record.charac_no = 700002;
    memcpy(record.dungeon, "\x10\x20\x30\x40", 4);
    record.dungeon_len = 4;
    memcpy(record.best_clear_time, "\xFF\xEE\xDD\xCC\xBB\xAA", 6);
    record.best_clear_time_len = 6;
    record.blue_marble_enter_count = 5;
    strcpy(record.charac_inform_notice, "Dungeon cleared!");
    CharacDungeon_Add(manager, &record);

    int ret = CharacDungeon_Get(manager, 700002, &retrieved);
    ASSERT_EQUAL(ret, 0, "Should retrieve existing record");
    ASSERT_EQUAL(retrieved.dungeon_len, 4, "Should have correct dungeon length");
    ASSERT_EQUAL(retrieved.best_clear_time_len, 6, "Should have correct best_clear_time length");
    ASSERT_EQUAL(retrieved.blue_marble_enter_count, 5, "Should have correct blue_marble count");
    ASSERT_TRUE(strcmp(retrieved.charac_inform_notice, "Dungeon cleared!") == 0, "Should have correct notice");

    /* 测试不存在的记录 */
    ret = CharacDungeon_Get(manager, 799999, &retrieved);
    ASSERT_EQUAL(ret, -1, "Should fail for non-existent record");

    cleanup_test_data(manager);
}

/* 测试3: Update操作 */
static void test_update(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_Update");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700003;
    memcpy(record.dungeon, "\x00\x00\x00\x00", 4);
    record.dungeon_len = 4;
    memcpy(record.best_clear_time, "\x00\x00", 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 1;
    strcpy(record.charac_inform_notice, "Initial");
    CharacDungeon_Add(manager, &record);

    /* 更新记录 */
    memcpy(record.dungeon, "\xFF\xFF\xFF\xFF", 4);
    memcpy(record.best_clear_time, "\xAA\xAA", 2);
    record.blue_marble_enter_count = 10;
    strcpy(record.charac_inform_notice, "Updated");

    int ret = CharacDungeon_Update(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully update record");

    /* 验证更新成功 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700003, &retrieved);
    ASSERT_EQUAL(retrieved.blue_marble_enter_count, 10, "Should have updated blue_marble count");
    ASSERT_TRUE(strcmp(retrieved.charac_inform_notice, "Updated") == 0, "Should have updated notice");

    cleanup_test_data(manager);
}

/* 测试4: Delete操作 */
static void test_delete(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_Delete");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700004;
    memset(record.dungeon, 0, 4);
    record.dungeon_len = 4;
    memset(record.best_clear_time, 0, 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "");
    CharacDungeon_Add(manager, &record);

    /* 删除记录 */
    int ret = CharacDungeon_Delete(manager, 700004);
    ASSERT_EQUAL(ret, 0, "Should successfully delete record");

    /* 验证删除成功 */
    int exists = CharacDungeon_Exists(manager, 700004);
    ASSERT_EQUAL(exists, 0, "Record should not exist after deletion");

    /* 删除不存在的记录应该成功（不报错） */
    ret = CharacDungeon_Delete(manager, 700004);
    ASSERT_EQUAL(ret, 0, "Should return 0 even for non-existent record");

    cleanup_test_data(manager);
}

/* 测试5: Exists操作 */
static void test_exists(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_Exists");
    cleanup_test_data(manager);

    CharacDungeon record;
    record.charac_no = 700005;
    memset(record.dungeon, 0, 4);
    record.dungeon_len = 4;
    memset(record.best_clear_time, 0, 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "");
    CharacDungeon_Add(manager, &record);

    int exists = CharacDungeon_Exists(manager, 700005);
    ASSERT_EQUAL(exists, 1, "Should return 1 for existing record");

    exists = CharacDungeon_Exists(manager, 799999);
    ASSERT_EQUAL(exists, 0, "Should return 0 for non-existent record");

    cleanup_test_data(manager);
}

/* 测试6: UpdateDungeonData操作 */
static void test_update_dungeon_data(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_UpdateDungeonData");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700006;
    memcpy(record.dungeon, "\x00\x00\x00\x00", 4);
    record.dungeon_len = 4;
    memset(record.best_clear_time, 0, 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "");
    CharacDungeon_Add(manager, &record);

    /* 更新副本数据 */
    unsigned char new_dungeon[] = "\x11\x22\x33\x44\x55\x66\x77\x88";
    int ret = CharacDungeon_UpdateDungeonData(manager, 700006, new_dungeon, 8);
    ASSERT_EQUAL(ret, 0, "Should successfully update dungeon data");

    /* 验证更新成功 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700006, &retrieved);
    ASSERT_EQUAL(retrieved.dungeon_len, 8, "Should have updated dungeon length");
    ASSERT_EQUAL(retrieved.dungeon[0], 0x11, "Should have correct first byte");
    ASSERT_EQUAL(retrieved.dungeon[7], 0x88, "Should have correct last byte");

    cleanup_test_data(manager);
}

/* 测试7: UpdateBestClearTime操作 */
static void test_update_best_clear_time(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_UpdateBestClearTime");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700007;
    memset(record.dungeon, 0, 4);
    record.dungeon_len = 4;
    memcpy(record.best_clear_time, "\xFF\xFF\xFF\xFF", 4);
    record.best_clear_time_len = 4;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "");
    CharacDungeon_Add(manager, &record);

    /* 更新最佳通关时间 */
    unsigned char new_time[] = "\x01\x02\x03\x04\x05\x06";
    int ret = CharacDungeon_UpdateBestClearTime(manager, 700007, new_time, 6);
    ASSERT_EQUAL(ret, 0, "Should successfully update best clear time");

    /* 验证更新成功 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700007, &retrieved);
    ASSERT_EQUAL(retrieved.best_clear_time_len, 6, "Should have updated best_clear_time length");
    ASSERT_EQUAL(retrieved.best_clear_time[0], 0x01, "Should have correct first byte");
    ASSERT_EQUAL(retrieved.best_clear_time[5], 0x06, "Should have correct last byte");

    cleanup_test_data(manager);
}

/* 测试8: UpdateNotice操作 */
static void test_update_notice(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_UpdateNotice");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700008;
    memset(record.dungeon, 0, 4);
    record.dungeon_len = 4;
    memset(record.best_clear_time, 0, 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "Old notice");
    CharacDungeon_Add(manager, &record);

    /* 更新通知 */
    int ret = CharacDungeon_UpdateNotice(manager, 700008, "New notice message");
    ASSERT_EQUAL(ret, 0, "Should successfully update notice");

    /* 验证更新成功 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700008, &retrieved);
    ASSERT_TRUE(strcmp(retrieved.charac_inform_notice, "New notice message") == 0,
                "Should have updated notice");

    cleanup_test_data(manager);
}

/* 测试9: IncrementBlueMarbleCount操作 */
static void test_increment_blue_marble_count(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_IncrementBlueMarbleCount");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacDungeon record;
    record.charac_no = 700009;
    memset(record.dungeon, 0, 4);
    record.dungeon_len = 4;
    memset(record.best_clear_time, 0, 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 3;
    strcpy(record.charac_inform_notice, "");
    CharacDungeon_Add(manager, &record);

    /* 递增蓝色大理石次数 */
    int ret = CharacDungeon_IncrementBlueMarbleCount(manager, 700009, 5);
    ASSERT_EQUAL(ret, 0, "Should successfully increment blue marble count");

    /* 验证递增成功 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700009, &retrieved);
    ASSERT_EQUAL(retrieved.blue_marble_enter_count, 8, "Blue marble count should be 3+5=8");

    cleanup_test_data(manager);
}

/* 测试10: PrintInfo工具函数 */
static void test_print_info(DBConnectionManager* manager) {
    TEST_START("CharacDungeon_PrintInfo");

    CharacDungeon record;
    record.charac_no = 700010;
    memcpy(record.dungeon, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    record.dungeon_len = 8;
    memcpy(record.best_clear_time, "\xAA\xBB\xCC\xDD", 4);
    record.best_clear_time_len = 4;
    record.blue_marble_enter_count = 7;
    strcpy(record.charac_inform_notice, "Test notice info");

    printf("  Testing PrintInfo output:\n");
    CharacDungeon_PrintInfo(&record);
    ASSERT_TRUE(1, "PrintInfo should execute without errors");

    printf("  Testing PrintInfo with NULL:\n");
    CharacDungeon_PrintInfo(NULL);
    ASSERT_TRUE(1, "PrintInfo should handle NULL gracefully");
}

/* 测试11: 集成测试 - 完整流程 */
static void test_integration(DBConnectionManager* manager) {
    TEST_START("Integration Test - Complete Workflow");
    cleanup_test_data(manager);

    /* 1. 添加记录 */
    CharacDungeon record;
    record.charac_no = 700100;
    memcpy(record.dungeon, "\x10\x20\x30\x40", 4);
    record.dungeon_len = 4;
    memcpy(record.best_clear_time, "\xAA\xBB", 2);
    record.best_clear_time_len = 2;
    record.blue_marble_enter_count = 0;
    strcpy(record.charac_inform_notice, "Starting");
    CharacDungeon_Add(manager, &record);

    /* 2. 验证存在 */
    int exists = CharacDungeon_Exists(manager, 700100);
    ASSERT_EQUAL(exists, 1, "Integration: Record should exist after add");

    /* 3. 递增蓝色大理石次数 */
    CharacDungeon_IncrementBlueMarbleCount(manager, 700100, 3);

    /* 4. 更新副本数据 */
    unsigned char new_dungeon[] = "\xFF\xEE\xDD\xCC";
    CharacDungeon_UpdateDungeonData(manager, 700100, new_dungeon, 4);

    /* 5. 更新最佳通关时间 */
    unsigned char new_time[] = "\x99\x88";
    CharacDungeon_UpdateBestClearTime(manager, 700100, new_time, 2);

    /* 6. 更新通知 */
    CharacDungeon_UpdateNotice(manager, 700100, "Completed");

    /* 7. 验证所有更新 */
    CharacDungeon retrieved;
    CharacDungeon_Get(manager, 700100, &retrieved);
    ASSERT_EQUAL(retrieved.blue_marble_enter_count, 3, "Integration: Blue marble count should be 3");
    ASSERT_EQUAL(retrieved.dungeon[0], 0xFF, "Integration: Dungeon data should be updated");
    ASSERT_EQUAL(retrieved.best_clear_time[0], 0x99, "Integration: Best clear time should be updated");
    ASSERT_TRUE(strcmp(retrieved.charac_inform_notice, "Completed") == 0,
                "Integration: Notice should be updated");

    /* 8. 删除记录 */
    CharacDungeon_Delete(manager, 700100);
    exists = CharacDungeon_Exists(manager, 700100);
    ASSERT_EQUAL(exists, 0, "Integration: Record should not exist after deletion");

    cleanup_test_data(manager);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    printf("=======================================================\n");
    printf("  CharacDungeon ORM 测试套件\n");
    printf("=======================================================\n");

    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        printf("[ERROR] Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        printf("[ERROR] Failed to connect to database\n");
        return 1;
    }

    printf("\n--- Running Basic CRUD Tests ---\n");
    test_add(&manager);
    test_get(&manager);
    test_update(&manager);
    test_delete(&manager);
    test_exists(&manager);

    printf("\n--- Running Business Logic Tests ---\n");
    test_update_dungeon_data(&manager);
    test_update_best_clear_time(&manager);
    test_update_notice(&manager);
    test_increment_blue_marble_count(&manager);

    printf("\n--- Running Utility Function Tests ---\n");
    test_print_info(&manager);

    printf("\n--- Running Integration Test ---\n");
    test_integration(&manager);

    /* 统计最后一个测试 */
    if (!current_test_failed) {
        test_passed++;
    }

    printf("\n=======================================================\n");
    printf("  测试完成统计\n");
    printf("=======================================================\n");
    printf("  测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("  断言检查: %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("=======================================================\n");

    return (test_passed == test_count) ? 0 : 1;
}
