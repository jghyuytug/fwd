#include "cs_table2_orm.h"
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

#define ASSERT_TRUE(expr, message) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
    } while(0)

#define ASSERT_GTE(a, b, message) \
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

/* 测试用例 */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取账户-角色关联记录");

    CsTable2 record;
    memset(&record, 0, sizeof(CsTable2));
    strcpy(record.account_id, "test_account_001");
    strcpy(record.charac_id, "test_charac_001");

    ASSERT_EQ(CsTable2_Add(manager, &record), 0, "添加记录成功");

    CsTable2 retrieved;
    memset(&retrieved, 0, sizeof(CsTable2));
    ASSERT_EQ(CsTable2_Get(manager, "test_account_001", "test_charac_001", &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.account_id, "test_account_001", "账户ID匹配");
    ASSERT_STR_EQ(retrieved.charac_id, "test_charac_001", "角色ID匹配");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CsTable2_Exists(manager, "test_account_001", "test_charac_001"), "记录存在");
    ASSERT_TRUE(!CsTable2_Exists(manager, "nonexistent_account", "nonexistent_charac"), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条账户-角色关联记录");

    CsTable2 record2;
    memset(&record2, 0, sizeof(CsTable2));
    strcpy(record2.account_id, "test_account_001");
    strcpy(record2.charac_id, "test_charac_002");
    ASSERT_EQ(CsTable2_Add(manager, &record2), 0, "添加记录2成功(同账户不同角色)");

    CsTable2 record3;
    memset(&record3, 0, sizeof(CsTable2));
    strcpy(record3.account_id, "test_account_001");
    strcpy(record3.charac_id, "test_charac_003");
    ASSERT_EQ(CsTable2_Add(manager, &record3), 0, "添加记录3成功");

    CsTable2 record4;
    memset(&record4, 0, sizeof(CsTable2));
    strcpy(record4.account_id, "test_account_002");
    strcpy(record4.charac_id, "test_charac_001");
    ASSERT_EQ(CsTable2_Add(manager, &record4), 0, "添加记录4成功(不同账户同角色)");

    CsTable2 record5;
    memset(&record5, 0, sizeof(CsTable2));
    strcpy(record5.account_id, "test_account_002");
    strcpy(record5.charac_id, "test_charac_004");
    ASSERT_EQ(CsTable2_Add(manager, &record5), 0, "添加记录5成功");
}

void test_get_by_account_id(DBConnectionManager* manager) {
    TEST_START("查询账户下的所有角色");

    CsTable2 records[10];
    memset(records, 0, sizeof(records));
    int count = CsTable2_GetByAccountId(manager, "test_account_001", records, 10);

    ASSERT_GTE(count, 3, "test_account_001至少有3个角色");
    if (count >= 3) {
        ASSERT_STR_EQ(records[0].account_id, "test_account_001", "第1条记录账户ID正确");
        ASSERT_STR_EQ(records[1].account_id, "test_account_001", "第2条记录账户ID正确");
        ASSERT_STR_EQ(records[2].account_id, "test_account_001", "第3条记录账户ID正确");
    }
}

void test_get_by_charac_id(DBConnectionManager* manager) {
    TEST_START("查询角色关联的所有账户");

    CsTable2 records[10];
    memset(records, 0, sizeof(records));
    int count = CsTable2_GetByCharacId(manager, "test_charac_001", records, 10);

    ASSERT_GTE(count, 2, "test_charac_001至少关联2个账户");
    if (count >= 2) {
        ASSERT_STR_EQ(records[0].charac_id, "test_charac_001", "第1条记录角色ID正确");
        ASSERT_STR_EQ(records[1].charac_id, "test_charac_001", "第2条记录角色ID正确");
    }
}

void test_count_all(DBConnectionManager* manager) {
    TEST_START("统计总记录数");

    int count = CsTable2_CountAll(manager);
    ASSERT_GTE(count, 5, "至少有5条记录");
}

void test_count_by_account_id(DBConnectionManager* manager) {
    TEST_START("统计账户关联的角色数");

    int count = CsTable2_CountByAccountId(manager, "test_account_001");
    ASSERT_GTE(count, 3, "test_account_001至少有3个角色");

    int count2 = CsTable2_CountByAccountId(manager, "test_account_002");
    ASSERT_GTE(count2, 2, "test_account_002至少有2个角色");
}

void test_count_by_charac_id(DBConnectionManager* manager) {
    TEST_START("统计角色关联的账户数");

    int count = CsTable2_CountByCharacId(manager, "test_charac_001");
    ASSERT_GTE(count, 2, "test_charac_001至少关联2个账户");

    int count2 = CsTable2_CountByCharacId(manager, "test_charac_002");
    ASSERT_GTE(count2, 1, "test_charac_002至少关联1个账户");
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有记录(分页)");

    CsTable2 records[10];
    memset(records, 0, sizeof(records));
    int count = CsTable2_GetAll(manager, 0, 10, records);

    ASSERT_GTE(count, 1, "至少返回1条记录");
    if (count > 0) {
        ASSERT_TRUE(strlen(records[0].account_id) > 0, "第1条记录账户ID不为空");
        ASSERT_TRUE(strlen(records[0].charac_id) > 0, "第1条记录角色ID不为空");
    }
}

void test_delete_by_charac_id(DBConnectionManager* manager) {
    TEST_START("删除角色的所有账户关联");

    int deleted = CsTable2_DeleteByCharacId(manager, "test_charac_004");
    ASSERT_GTE(deleted, 0, "删除操作执行成功");

    int count = CsTable2_CountByCharacId(manager, "test_charac_004");
    ASSERT_EQ(count, 0, "test_charac_004的关联已全部删除");
}

void test_delete_by_account_id(DBConnectionManager* manager) {
    TEST_START("删除账户的所有角色关联");

    int deleted = CsTable2_DeleteByAccountId(manager, "test_account_002");
    ASSERT_GTE(deleted, 0, "删除操作执行成功");

    int count = CsTable2_CountByAccountId(manager, "test_account_002");
    ASSERT_EQ(count, 0, "test_account_002的关联已全部删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除单条记录");

    ASSERT_EQ(CsTable2_Delete(manager, "test_account_001", "test_charac_001"), 0, "删除记录1成功");
    ASSERT_TRUE(!CsTable2_Exists(manager, "test_account_001", "test_charac_001"), "记录1已删除");

    ASSERT_EQ(CsTable2_Delete(manager, "test_account_001", "test_charac_002"), 0, "删除记录2成功");
    ASSERT_EQ(CsTable2_Delete(manager, "test_account_001", "test_charac_003"), 0, "删除记录3成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印账户-角色关联记录信息");

    CsTable2 record;
    memset(&record, 0, sizeof(CsTable2));
    strcpy(record.account_id, "print_test_account");
    strcpy(record.charac_id, "print_test_charac");

    CsTable2_Add(manager, &record);

    CsTable2 retrieved;
    memset(&retrieved, 0, sizeof(CsTable2));
    if (CsTable2_Get(manager, "print_test_account", "print_test_charac", &retrieved) == 0) {
        CsTable2_PrintInfo(&retrieved);
        ASSERT_TRUE(1, "打印信息成功");
        CsTable2_Delete(manager, "print_test_account", "print_test_charac");
    } else {
        printf("  ⚠ 无记录可打印\n");
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
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("客服表2 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_get_by_account_id(&manager);
    test_get_by_charac_id(&manager);
    test_count_all(&manager);
    test_count_by_account_id(&manager);
    test_count_by_charac_id(&manager);
    test_get_all(&manager);
    test_delete_by_charac_id(&manager);
    test_delete_by_account_id(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要算进去 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
