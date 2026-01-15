#include "dnf_charac_mov_orm.h"
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

#define ASSERT_GT(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) > (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d <= %d)\n", message, (int)(a), (int)(b)); \
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

/* 辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    /* 删除测试数据 - 使用测试用的m_id范围 */
    snprintf(query, sizeof(query),
        "DELETE FROM dnf_charac_mov WHERE m_id >= 900000 AND m_id < 999999");

    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

/* ==================== 测试用例 ==================== */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("测试添加和查询转服记录");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900001;
    record.server_id = 1;
    record.charac_no = 12345;
    record.move_server_id = 2;
    record.move_charac_no = 67890;
    record.move_check = 0;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功，返回自增ID");

    DnfCharacMov retrieved;
    memset(&retrieved, 0, sizeof(DnfCharacMov));
    int ret = DnfCharacMov_Get(manager, new_id, &retrieved);
    ASSERT_EQ(ret, 0, "查询转服记录成功");
    ASSERT_EQ(retrieved.id, new_id, "ID匹配");
    ASSERT_EQ(retrieved.m_id, 900001, "会员ID匹配");
    ASSERT_EQ(retrieved.server_id, 1, "原服务器ID匹配");
    ASSERT_EQ(retrieved.charac_no, 12345, "原角色编号匹配");
    ASSERT_EQ(retrieved.move_server_id, 2, "目标服务器ID匹配");
    ASSERT_EQ(retrieved.move_charac_no, 67890, "目标角色编号匹配");
    ASSERT_EQ(retrieved.move_check, 0, "移动检查状态匹配");

    DnfCharacMov_PrintInfo(&retrieved);
}

void test_get_by_character(DBConnectionManager* manager) {
    TEST_START("测试按角色信息查询（唯一索引）");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900002;
    record.server_id = 3;
    record.charac_no = 11111;
    record.move_server_id = 4;
    record.move_charac_no = 22222;
    record.move_check = 1;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功");

    DnfCharacMov retrieved;
    memset(&retrieved, 0, sizeof(DnfCharacMov));
    int ret = DnfCharacMov_GetByCharacter(manager, 900002, 3, 11111, &retrieved);
    ASSERT_EQ(ret, 0, "按角色信息查询成功");
    ASSERT_EQ(retrieved.id, new_id, "ID匹配");
    ASSERT_EQ(retrieved.m_id, 900002, "会员ID匹配");
    ASSERT_EQ(retrieved.server_id, 3, "原服务器ID匹配");
    ASSERT_EQ(retrieved.charac_no, 11111, "原角色编号匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("测试更新转服记录");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900003;
    record.server_id = 5;
    record.charac_no = 33333;
    record.move_server_id = 6;
    record.move_charac_no = 44444;
    record.move_check = 0;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功");

    /* 查询并修改 */
    DnfCharacMov retrieved;
    memset(&retrieved, 0, sizeof(DnfCharacMov));
    DnfCharacMov_Get(manager, new_id, &retrieved);

    retrieved.move_server_id = 7;
    retrieved.move_charac_no = 55555;
    retrieved.move_check = 1;

    int ret = DnfCharacMov_Update(manager, &retrieved);
    ASSERT_EQ(ret, 0, "更新转服记录成功");

    /* 验证更新 */
    DnfCharacMov updated;
    memset(&updated, 0, sizeof(DnfCharacMov));
    DnfCharacMov_Get(manager, new_id, &updated);
    ASSERT_EQ(updated.move_server_id, 7, "目标服务器ID已更新");
    ASSERT_EQ(updated.move_charac_no, 55555, "目标角色编号已更新");
    ASSERT_EQ(updated.move_check, 1, "移动检查状态已更新");
}

void test_update_move_check(DBConnectionManager* manager) {
    TEST_START("测试更新移动检查状态");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900004;
    record.server_id = 8;
    record.charac_no = 66666;
    record.move_server_id = 9;
    record.move_charac_no = 77777;
    record.move_check = 0;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功");

    int ret = DnfCharacMov_UpdateMoveCheck(manager, new_id, 2);
    ASSERT_EQ(ret, 0, "更新移动检查状态成功");

    DnfCharacMov retrieved;
    memset(&retrieved, 0, sizeof(DnfCharacMov));
    DnfCharacMov_Get(manager, new_id, &retrieved);
    ASSERT_EQ(retrieved.move_check, 2, "移动检查状态已更新");
}

void test_get_by_mid(DBConnectionManager* manager) {
    TEST_START("测试查询会员的所有转服记录");

    /* 添加同一会员的多条转服记录 */
    DnfCharacMov record;
    int test_m_id = 900005;

    for (int i = 0; i < 3; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = test_m_id;
        record.server_id = 10 + i;
        record.charac_no = 80000 + i;
        record.move_server_id = 20 + i;
        record.move_charac_no = 90000 + i;
        record.move_check = i;

        int new_id = DnfCharacMov_Add(manager, &record);
        ASSERT_GT(new_id, 0, "添加转服记录成功");
    }

    DnfCharacMov records[10];
    memset(records, 0, sizeof(records));
    int count = DnfCharacMov_GetByMid(manager, test_m_id, records, 10);
    ASSERT_GE(count, 3, "查询到至少3条记录");

    printf("  查询到 %d 条会员 %d 的转服记录\n", count, test_m_id);
}

void test_get_by_source_server(DBConnectionManager* manager) {
    TEST_START("测试查询从某服务器转出的记录");

    DnfCharacMov record;
    int test_server_id = 15;

    for (int i = 0; i < 2; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = 900006 + i;
        record.server_id = test_server_id;
        record.charac_no = 85000 + i;
        record.move_server_id = 25 + i;
        record.move_charac_no = 95000 + i;
        record.move_check = 0;

        int new_id = DnfCharacMov_Add(manager, &record);
        ASSERT_GT(new_id, 0, "添加转服记录成功");
    }

    DnfCharacMov records[10];
    memset(records, 0, sizeof(records));
    int count = DnfCharacMov_GetBySourceServer(manager, test_server_id, records, 10);
    ASSERT_GE(count, 2, "查询到至少2条记录");

    printf("  查询到 %d 条从服务器 %d 转出的记录\n", count, test_server_id);
}

void test_get_by_target_server(DBConnectionManager* manager) {
    TEST_START("测试查询转入某服务器的记录");

    DnfCharacMov record;
    int test_move_server_id = 30;

    for (int i = 0; i < 2; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = 900008 + i;
        record.server_id = 16 + i;
        record.charac_no = 86000 + i;
        record.move_server_id = test_move_server_id;
        record.move_charac_no = 96000 + i;
        record.move_check = 0;

        int new_id = DnfCharacMov_Add(manager, &record);
        ASSERT_GT(new_id, 0, "添加转服记录成功");
    }

    DnfCharacMov records[10];
    memset(records, 0, sizeof(records));
    int count = DnfCharacMov_GetByTargetServer(manager, test_move_server_id, records, 10);
    ASSERT_GE(count, 2, "查询到至少2条记录");

    printf("  查询到 %d 条转入服务器 %d 的记录\n", count, test_move_server_id);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("测试记录存在性检查");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900010;
    record.server_id = 18;
    record.charac_no = 87000;
    record.move_server_id = 32;
    record.move_charac_no = 97000;
    record.move_check = 0;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功");

    int exists = DnfCharacMov_Exists(manager, new_id);
    ASSERT_EQ(exists, 1, "已存在的ID返回1");

    exists = DnfCharacMov_Exists(manager, 999999);
    ASSERT_EQ(exists, 0, "不存在的ID返回0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("测试删除转服记录");

    DnfCharacMov record;
    memset(&record, 0, sizeof(DnfCharacMov));
    record.m_id = 900011;
    record.server_id = 19;
    record.charac_no = 88000;
    record.move_server_id = 33;
    record.move_charac_no = 98000;
    record.move_check = 0;

    int new_id = DnfCharacMov_Add(manager, &record);
    ASSERT_GT(new_id, 0, "添加转服记录成功");

    int ret = DnfCharacMov_Delete(manager, new_id);
    ASSERT_EQ(ret, 0, "删除转服记录成功");

    int exists = DnfCharacMov_Exists(manager, new_id);
    ASSERT_EQ(exists, 0, "记录已不存在");
}

void test_count_by_mid(DBConnectionManager* manager) {
    TEST_START("测试统计会员的转服次数");

    int test_m_id = 900012;
    DnfCharacMov record;

    for (int i = 0; i < 3; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = test_m_id;
        record.server_id = 20 + i;
        record.charac_no = 89000 + i;
        record.move_server_id = 35 + i;
        record.move_charac_no = 99000 + i;
        record.move_check = 0;

        DnfCharacMov_Add(manager, &record);
    }

    int count = DnfCharacMov_CountByMid(manager, test_m_id);
    ASSERT_GE(count, 3, "统计到至少3条记录");

    printf("  会员 %d 的转服次数: %d\n", test_m_id, count);
}

void test_count_by_move_check(DBConnectionManager* manager) {
    TEST_START("测试按检查状态统计记录数");

    DnfCharacMov record;
    int test_move_check = 5;

    for (int i = 0; i < 2; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = 900013 + i;
        record.server_id = 23 + i;
        record.charac_no = 89500 + i;
        record.move_server_id = 38 + i;
        record.move_charac_no = 99500 + i;
        record.move_check = test_move_check;

        DnfCharacMov_Add(manager, &record);
    }

    int count = DnfCharacMov_CountByMoveCheck(manager, test_move_check);
    ASSERT_GE(count, 2, "统计到至少2条记录");

    printf("  move_check=%d 的记录数: %d\n", test_move_check, count);
}

void test_count_total(DBConnectionManager* manager) {
    TEST_START("测试统计总转服记录数");

    int count = DnfCharacMov_CountTotal(manager);
    ASSERT_GE(count, 0, "总记录数>=0");

    printf("  当前总转服记录数: %d\n", count);
}

void test_batch_delete(DBConnectionManager* manager) {
    TEST_START("测试批量删除");

    /* 添加测试记录 */
    int ids[3];
    DnfCharacMov record;
    for (int i = 0; i < 3; i++) {
        memset(&record, 0, sizeof(DnfCharacMov));
        record.m_id = 900015 + i;
        record.server_id = 25 + i;
        record.charac_no = 89700 + i;
        record.move_server_id = 40 + i;
        record.move_charac_no = 99700 + i;
        record.move_check = 0;

        ids[i] = DnfCharacMov_Add(manager, &record);
        ASSERT_GT(ids[i], 0, "添加测试记录成功");
    }

    int ret = DnfCharacMov_BatchDelete(manager, ids, 3);
    ASSERT_EQ(ret, 0, "批量删除成功");

    /* 验证已删除 */
    for (int i = 0; i < 3; i++) {
        int exists = DnfCharacMov_Exists(manager, ids[i]);
        ASSERT_EQ(exists, 0, "批量删除的记录已不存在");
    }
}

void test_null_pointer_checks(DBConnectionManager* manager) {
    TEST_START("测试NULL指针检查");

    int ret = DnfCharacMov_Add(NULL, NULL);
    ASSERT_EQ(ret, -1, "manager=NULL时返回-1");

    DnfCharacMov record;
    ret = DnfCharacMov_Get(manager, 1, NULL);
    ASSERT_EQ(ret, -1, "record=NULL时返回-1");

    ret = DnfCharacMov_GetByCharacter(manager, 1, 1, 1, NULL);
    ASSERT_EQ(ret, -1, "record=NULL时返回-1");

    ret = DnfCharacMov_GetByMid(manager, 1, NULL, 10);
    ASSERT_EQ(ret, -1, "records=NULL时返回-1");

    ret = DnfCharacMov_GetByMid(manager, 1, &record, 0);
    ASSERT_EQ(ret, -1, "max_count<=0时返回-1");
}

/* ==================== 主函数 ==================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        printf("用法: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_charac_mov_orm 测试套件\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("连接数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 运行测试 */
    test_add_and_get(&manager);
    test_get_by_character(&manager);
    test_update(&manager);
    test_update_move_check(&manager);
    test_get_by_mid(&manager);
    test_get_by_source_server(&manager);
    test_get_by_target_server(&manager);
    test_exists(&manager);
    test_delete(&manager);
    test_count_by_mid(&manager);
    test_count_by_move_check(&manager);
    test_count_total(&manager);
    test_batch_delete(&manager);
    test_null_pointer_checks(&manager);

    /* 最后一个测试完成时增加test_passed */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✅ 所有测试通过！\n");
        return 0;
    } else {
        printf("❌ 部分测试失败！\n");
        return 1;
    }
}
