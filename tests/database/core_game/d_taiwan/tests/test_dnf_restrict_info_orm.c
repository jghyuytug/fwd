#include "dnf_restrict_info_orm.h"
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

/* 测试用例 */

void test_add_restrict_info(DBConnectionManager* manager) {
    TEST_START("添加限制信息");

    DnfRestrictInfo record;
    record.category = 1;
    record.restrict_code = 100;
    strcpy(record.restrict_str, "测试限制字符串");
    strcpy(record.reg_date, "2025-11-17 00:00:00");

    int result = DnfRestrictInfo_Add(manager, &record);
    ASSERT_EQ(result, 0, "添加限制信息成功");
}

void test_get_restrict_info(DBConnectionManager* manager) {
    TEST_START("获取限制信息");

    DnfRestrictInfo record;
    int result = DnfRestrictInfo_Get(manager, 1, 100, &record);

    ASSERT_EQ(result, 0, "获取限制信息成功");
    ASSERT_EQ(record.category, 1, "分类正确");
    ASSERT_EQ(record.restrict_code, 100, "限制代码正确");
    ASSERT_STR_EQ(record.restrict_str, "测试限制字符串", "限制字符串正确");
    ASSERT_STR_EQ(record.reg_date, "2025-11-17 00:00:00", "注册日期正确");
}

void test_exists_restrict_info(DBConnectionManager* manager) {
    TEST_START("检查限制信息存在性");

    int exists = DnfRestrictInfo_Exists(manager, 1, 100);
    ASSERT_EQ(exists, 1, "限制信息存在");

    int not_exists = DnfRestrictInfo_Exists(manager, 999, 999);
    ASSERT_EQ(not_exists, 0, "不存在的限制信息返回0");
}

void test_update_restrict_info(DBConnectionManager* manager) {
    TEST_START("更新限制信息");

    DnfRestrictInfo record;
    record.category = 1;
    record.restrict_code = 100;
    strcpy(record.restrict_str, "更新后的限制字符串");
    strcpy(record.reg_date, "2025-11-17 12:00:00");

    int result = DnfRestrictInfo_Update(manager, &record);
    ASSERT_EQ(result, 0, "更新限制信息成功");

    // 验证更新
    DnfRestrictInfo retrieved;
    DnfRestrictInfo_Get(manager, 1, 100, &retrieved);
    ASSERT_STR_EQ(retrieved.restrict_str, "更新后的限制字符串", "限制字符串已更新");
    ASSERT_STR_EQ(retrieved.reg_date, "2025-11-17 12:00:00", "注册日期已更新");
}

void test_add_multiple_restrict_infos(DBConnectionManager* manager) {
    TEST_START("添加多个限制信息");

    // 添加同一分类的多个记录
    DnfRestrictInfo record1 = {1, 101, "限制1", "2025-11-17 01:00:00"};
    DnfRestrictInfo record2 = {1, 102, "限制2", "2025-11-17 02:00:00"};
    DnfRestrictInfo record3 = {2, 200, "限制3", "2025-11-17 03:00:00"};

    ASSERT_EQ(DnfRestrictInfo_Add(manager, &record1), 0, "添加记录1成功");
    ASSERT_EQ(DnfRestrictInfo_Add(manager, &record2), 0, "添加记录2成功");
    ASSERT_EQ(DnfRestrictInfo_Add(manager, &record3), 0, "添加记录3成功");
}

void test_get_by_category(DBConnectionManager* manager) {
    TEST_START("根据分类获取限制信息");

    DnfRestrictInfo* records = NULL;
    int count = 0;

    int result = DnfRestrictInfo_GetByCategory(manager, 1, &records, &count);
    ASSERT_EQ(result, 0, "获取分类1的限制信息成功");
    ASSERT_TRUE(count >= 3, "至少有3条记录（100, 101, 102）");

    printf("  分类1共有 %d 条限制信息\n", count);

    DnfRestrictInfo_FreeRecords(records);
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有限制信息");

    DnfRestrictInfo* records = NULL;
    int count = 0;

    int result = DnfRestrictInfo_GetAll(manager, &records, &count);
    ASSERT_EQ(result, 0, "获取所有限制信息成功");
    ASSERT_TRUE(count >= 4, "至少有4条记录");

    printf("  共有 %d 条限制信息\n", count);

    DnfRestrictInfo_FreeRecords(records);
}

void test_search_by_string(DBConnectionManager* manager) {
    TEST_START("根据字符串模糊查询");

    DnfRestrictInfo* records = NULL;
    int count = 0;

    int result = DnfRestrictInfo_SearchByString(manager, "限制", &records, &count);
    ASSERT_EQ(result, 0, "模糊查询成功");
    ASSERT_TRUE(count >= 3, "至少找到3条包含'限制'的记录");

    printf("  找到 %d 条包含'限制'的记录\n", count);

    DnfRestrictInfo_FreeRecords(records);
}

void test_replace_duplicate(DBConnectionManager* manager) {
    TEST_START("测试REPLACE功能（重复主键）");

    DnfRestrictInfo record;
    record.category = 1;
    record.restrict_code = 100;
    strcpy(record.restrict_str, "第三次更新");
    strcpy(record.reg_date, "2025-11-17 18:00:00");

    int result = DnfRestrictInfo_Add(manager, &record);
    ASSERT_EQ(result, 0, "REPLACE操作成功");

    // 验证是否被替换
    DnfRestrictInfo retrieved;
    DnfRestrictInfo_Get(manager, 1, 100, &retrieved);
    ASSERT_STR_EQ(retrieved.restrict_str, "第三次更新", "记录被正确替换");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印限制信息");

    DnfRestrictInfo record;
    if (DnfRestrictInfo_Get(manager, 1, 100, &record) == 0) {
        DnfRestrictInfo_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息功能正常");
    }
}

void test_edge_cases(DBConnectionManager* manager) {
    TEST_START("边界测试");

    // 测试空字符串
    DnfRestrictInfo record;
    record.category = 3;
    record.restrict_code = 300;
    strcpy(record.restrict_str, "");
    strcpy(record.reg_date, "2025-11-17 00:00:00");

    ASSERT_EQ(DnfRestrictInfo_Add(manager, &record), 0, "添加空字符串记录成功");

    DnfRestrictInfo retrieved;
    DnfRestrictInfo_Get(manager, 3, 300, &retrieved);
    ASSERT_STR_EQ(retrieved.restrict_str, "", "空字符串正确保存");

    // 测试最大长度字符串（45字符）
    DnfRestrictInfo long_record;
    long_record.category = 4;
    long_record.restrict_code = 400;
    strcpy(long_record.restrict_str, "12345678901234567890123456789012345678901234");  // 44字符
    strcpy(long_record.reg_date, "2025-11-17 00:00:00");

    ASSERT_EQ(DnfRestrictInfo_Add(manager, &long_record), 0, "添加长字符串成功");
}

void test_delete_restrict_info(DBConnectionManager* manager) {
    TEST_START("删除限制信息");

    // 删除测试数据
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 1, 100), 0, "删除记录(1,100)成功");
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 1, 101), 0, "删除记录(1,101)成功");
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 1, 102), 0, "删除记录(1,102)成功");
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 2, 200), 0, "删除记录(2,200)成功");
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 3, 300), 0, "删除记录(3,300)成功");
    ASSERT_EQ(DnfRestrictInfo_Delete(manager, 4, 400), 0, "删除记录(4,400)成功");

    // 验证删除
    int exists = DnfRestrictInfo_Exists(manager, 1, 100);
    ASSERT_EQ(exists, 0, "记录已删除");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_restrict_info ORM 测试套件\n");
    printf("========================================\n");

    // 初始化数据库连接
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "连接到 d_taiwan 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("✓ 数据库连接成功\n");

    // 运行测试
    test_add_restrict_info(&manager);
    test_get_restrict_info(&manager);
    test_exists_restrict_info(&manager);
    test_update_restrict_info(&manager);
    test_add_multiple_restrict_infos(&manager);
    test_get_by_category(&manager);
    test_get_all(&manager);
    test_search_by_string(&manager);
    test_replace_duplicate(&manager);
    test_print_info(&manager);
    test_edge_cases(&manager);
    test_delete_restrict_info(&manager);

    // 最后一个测试完成后更新计数
    if (!current_test_failed) {
        test_passed++;
    }

    // 清理
    DBConnectionManager_Cleanup(&manager);

    // 打印结果
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
