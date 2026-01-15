#include "charac_expert_job_orm.h"
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

    snprintf(query, sizeof(query), "DELETE FROM charac_expert_job WHERE charac_no >= 600000");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/* 测试1: Add操作 */
static void test_add(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_Add");
    cleanup_test_data(manager);

    CharacExpertJob record;
    record.charac_no = 600001;
    record.expert_job_giveup_cnt = 0;
    record.expert_job_info = 1001;
    record.expert_job_info_ex = 2001;
    memcpy(record.recipe, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    record.recipe_len = 8;

    int ret = CharacExpertJob_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully add a record");

    /* 验证添加成功 */
    int exists = CharacExpertJob_Exists(manager, 600001);
    ASSERT_EQUAL(exists, 1, "Record should exist after add");

    /* 测试幂等性 - 重复添加应该成功 */
    ret = CharacExpertJob_Add(manager, &record);
    ASSERT_EQUAL(ret, 0, "Should successfully update existing record (REPLACE)");

    cleanup_test_data(manager);
}

/* 测试2: Get操作 */
static void test_get(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_Get");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacExpertJob record, retrieved;
    record.charac_no = 600002;
    record.expert_job_giveup_cnt = 3;
    record.expert_job_info = 1002;
    record.expert_job_info_ex = 2002;
    memcpy(record.recipe, "\xAA\xBB\xCC\xDD", 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    int ret = CharacExpertJob_Get(manager, 600002, &retrieved);
    ASSERT_EQUAL(ret, 0, "Should retrieve existing record");
    ASSERT_EQUAL(retrieved.expert_job_giveup_cnt, 3, "Should have correct giveup_cnt");
    ASSERT_EQUAL(retrieved.expert_job_info, 1002, "Should have correct expert_job_info");
    ASSERT_EQUAL(retrieved.expert_job_info_ex, 2002, "Should have correct expert_job_info_ex");
    ASSERT_EQUAL(retrieved.recipe_len, 4, "Should have correct recipe length");

    /* 测试不存在的记录 */
    ret = CharacExpertJob_Get(manager, 699999, &retrieved);
    ASSERT_EQUAL(ret, -1, "Should fail for non-existent record");

    cleanup_test_data(manager);
}

/* 测试3: Update操作 */
static void test_update(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_Update");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacExpertJob record;
    record.charac_no = 600003;
    record.expert_job_giveup_cnt = 1;
    record.expert_job_info = 1000;
    record.expert_job_info_ex = 2000;
    memset(record.recipe, 0, 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    /* 更新职业信息 */
    int ret = CharacExpertJob_Update(manager, 600003, 1111, 2222);
    ASSERT_EQUAL(ret, 0, "Should successfully update expert job info");

    /* 验证更新成功 */
    CharacExpertJob retrieved;
    CharacExpertJob_Get(manager, 600003, &retrieved);
    ASSERT_EQUAL(retrieved.expert_job_info, 1111, "Should have updated expert_job_info");
    ASSERT_EQUAL(retrieved.expert_job_info_ex, 2222, "Should have updated expert_job_info_ex");

    cleanup_test_data(manager);
}

/* 测试4: Delete操作 */
static void test_delete(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_Delete");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacExpertJob record;
    record.charac_no = 600004;
    record.expert_job_giveup_cnt = 0;
    record.expert_job_info = 1000;
    record.expert_job_info_ex = 2000;
    memset(record.recipe, 0, 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    /* 删除记录 */
    int ret = CharacExpertJob_Delete(manager, 600004);
    ASSERT_EQUAL(ret, 0, "Should successfully delete record");

    /* 验证删除成功 */
    int exists = CharacExpertJob_Exists(manager, 600004);
    ASSERT_EQUAL(exists, 0, "Record should not exist after deletion");

    /* 删除不存在的记录应该成功（不报错） */
    ret = CharacExpertJob_Delete(manager, 600004);
    ASSERT_EQUAL(ret, 0, "Should return 0 even for non-existent record");

    cleanup_test_data(manager);
}

/* 测试5: Exists操作 */
static void test_exists(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_Exists");
    cleanup_test_data(manager);

    CharacExpertJob record;
    record.charac_no = 600005;
    record.expert_job_giveup_cnt = 0;
    record.expert_job_info = 1000;
    record.expert_job_info_ex = 2000;
    memset(record.recipe, 0, 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    int exists = CharacExpertJob_Exists(manager, 600005);
    ASSERT_EQUAL(exists, 1, "Should return 1 for existing record");

    exists = CharacExpertJob_Exists(manager, 699999);
    ASSERT_EQUAL(exists, 0, "Should return 0 for non-existent record");

    cleanup_test_data(manager);
}

/* 测试6: UpdateRecipe操作 */
static void test_update_recipe(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_UpdateRecipe");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacExpertJob record;
    record.charac_no = 600006;
    record.expert_job_giveup_cnt = 0;
    record.expert_job_info = 1000;
    record.expert_job_info_ex = 2000;
    memcpy(record.recipe, "\x00\x00\x00\x00", 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    /* 更新配方 */
    unsigned char new_recipe[] = "\xFF\xEE\xDD\xCC\xBB\xAA";
    int ret = CharacExpertJob_UpdateRecipe(manager, 600006, new_recipe, 6);
    ASSERT_EQUAL(ret, 0, "Should successfully update recipe");

    /* 验证更新成功 */
    CharacExpertJob retrieved;
    CharacExpertJob_Get(manager, 600006, &retrieved);
    ASSERT_EQUAL(retrieved.recipe_len, 6, "Should have updated recipe length");
    ASSERT_EQUAL(retrieved.recipe[0], 0xFF, "Should have correct first byte");
    ASSERT_EQUAL(retrieved.recipe[5], 0xAA, "Should have correct last byte");

    cleanup_test_data(manager);
}

/* 测试7: IncrementGiveupCnt操作 */
static void test_increment_giveup_cnt(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_IncrementGiveupCnt");
    cleanup_test_data(manager);

    /* 先添加一条记录 */
    CharacExpertJob record;
    record.charac_no = 600007;
    record.expert_job_giveup_cnt = 5;
    record.expert_job_info = 1000;
    record.expert_job_info_ex = 2000;
    memset(record.recipe, 0, 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    /* 递增放弃次数 */
    int ret = CharacExpertJob_IncrementGiveupCnt(manager, 600007, 3);
    ASSERT_EQUAL(ret, 0, "Should successfully increment giveup count");

    /* 验证递增成功 */
    CharacExpertJob retrieved;
    CharacExpertJob_Get(manager, 600007, &retrieved);
    ASSERT_EQUAL(retrieved.expert_job_giveup_cnt, 8, "Giveup count should be 5+3=8");

    cleanup_test_data(manager);
}

/* 测试8: PrintInfo工具函数 */
static void test_print_info(DBConnectionManager* manager) {
    TEST_START("CharacExpertJob_PrintInfo");

    CharacExpertJob record;
    record.charac_no = 600008;
    record.expert_job_giveup_cnt = 2;
    record.expert_job_info = 1234;
    record.expert_job_info_ex = 5678;
    memcpy(record.recipe, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    record.recipe_len = 8;

    printf("  Testing PrintInfo output:\n");
    CharacExpertJob_PrintInfo(&record);
    ASSERT_TRUE(1, "PrintInfo should execute without errors");

    printf("  Testing PrintInfo with NULL:\n");
    CharacExpertJob_PrintInfo(NULL);
    ASSERT_TRUE(1, "PrintInfo should handle NULL gracefully");
}

/* 测试9: 集成测试 - 完整流程 */
static void test_integration(DBConnectionManager* manager) {
    TEST_START("Integration Test - Complete Workflow");
    cleanup_test_data(manager);

    /* 1. 添加记录 */
    CharacExpertJob record;
    record.charac_no = 600100;
    record.expert_job_giveup_cnt = 0;
    record.expert_job_info = 100;
    record.expert_job_info_ex = 200;
    memcpy(record.recipe, "\xAA\xBB\xCC\xDD", 4);
    record.recipe_len = 4;
    CharacExpertJob_Add(manager, &record);

    /* 2. 验证存在 */
    int exists = CharacExpertJob_Exists(manager, 600100);
    ASSERT_EQUAL(exists, 1, "Integration: Record should exist after add");

    /* 3. 递增放弃次数 */
    CharacExpertJob_IncrementGiveupCnt(manager, 600100, 2);

    /* 4. 更新职业信息 */
    CharacExpertJob_Update(manager, 600100, 999, 888);

    /* 5. 验证更新成功 */
    CharacExpertJob retrieved;
    CharacExpertJob_Get(manager, 600100, &retrieved);
    ASSERT_EQUAL(retrieved.expert_job_giveup_cnt, 2, "Integration: Giveup count should be 2");
    ASSERT_EQUAL(retrieved.expert_job_info, 999, "Integration: expert_job_info should be 999");
    ASSERT_EQUAL(retrieved.expert_job_info_ex, 888, "Integration: expert_job_info_ex should be 888");

    /* 6. 删除记录 */
    CharacExpertJob_Delete(manager, 600100);
    exists = CharacExpertJob_Exists(manager, 600100);
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
    printf("  CharacExpertJob ORM 测试套件\n");
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
    test_update_recipe(&manager);
    test_increment_giveup_cnt(&manager);

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
