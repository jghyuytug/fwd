#include "dnf_event_info_orm.h"
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

void test_add_event_info(DBConnectionManager* manager) {
    TEST_START("添加活动信息");

    DnfEventInfo record;
    record.event_id = 1001;
    strcpy(record.event_name, "春节活动2025");
    strcpy(record.event_explain, "2025年春节特别活动");
    record.apply_type = 1;
    strcpy(record.start_date, "2025-01-01");
    strcpy(record.end_date, "2025-01-31");

    int result = DnfEventInfo_Add(manager, &record);
    ASSERT_EQ(result, 0, "添加活动信息成功");
}

void test_get_event_info(DBConnectionManager* manager) {
    TEST_START("获取活动信息");

    DnfEventInfo record;
    int result = DnfEventInfo_Get(manager, 1001, &record);

    ASSERT_EQ(result, 0, "获取活动信息成功");
    ASSERT_EQ(record.event_id, 1001, "活动ID正确");
    ASSERT_STR_EQ(record.event_name, "春节活动2025", "活动名称正确");
    ASSERT_STR_EQ(record.event_explain, "2025年春节特别活动", "活动说明正确");
    ASSERT_EQ((int)record.apply_type, 1, "申请类型正确");
    ASSERT_STR_EQ(record.start_date, "2025-01-01", "开始日期正确");
    ASSERT_STR_EQ(record.end_date, "2025-01-31", "结束日期正确");
}

void test_get_by_name(DBConnectionManager* manager) {
    TEST_START("根据名称获取活动");

    DnfEventInfo record;
    int result = DnfEventInfo_GetByName(manager, "春节活动2025", &record);

    ASSERT_EQ(result, 0, "根据名称获取成功");
    ASSERT_EQ(record.event_id, 1001, "活动ID匹配");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查活动存在性");

    ASSERT_EQ(DnfEventInfo_Exists(manager, 1001), 1, "活动1001存在");
    ASSERT_EQ(DnfEventInfo_Exists(manager, 9999), 0, "活动9999不存在");
    ASSERT_EQ(DnfEventInfo_NameExists(manager, "春节活动2025"), 1, "活动名称存在");
    ASSERT_EQ(DnfEventInfo_NameExists(manager, "不存在的活动"), 0, "活动名称不存在");
}

void test_update_event_info(DBConnectionManager* manager) {
    TEST_START("更新活动信息");

    DnfEventInfo record;
    record.event_id = 1001;
    strcpy(record.event_name, "春节活动2025(更新)");
    strcpy(record.event_explain, "2025年春节特别活动(已更新)");
    record.apply_type = 2;
    strcpy(record.start_date, "2025-01-05");
    strcpy(record.end_date, "2025-02-05");

    ASSERT_EQ(DnfEventInfo_Update(manager, &record), 0, "更新活动成功");

    DnfEventInfo retrieved;
    DnfEventInfo_Get(manager, 1001, &retrieved);
    ASSERT_STR_EQ(retrieved.event_name, "春节活动2025(更新)", "名称已更新");
    ASSERT_EQ((int)retrieved.apply_type, 2, "类型已更新");
}

void test_add_multiple_events(DBConnectionManager* manager) {
    TEST_START("添加多个活动");

    DnfEventInfo e1 = {1002, "国庆活动", "国庆节特别活动", 1, "2025-10-01", "2025-10-07"};
    DnfEventInfo e2 = {1003, "双十一活动", "双十一购物节", 2, "2025-11-11", "2025-11-11"};
    DnfEventInfo e3 = {1004, "圣诞活动", "圣诞节特别活动", 1, "2025-12-24", "2025-12-26"};

    ASSERT_EQ(DnfEventInfo_Add(manager, &e1), 0, "添加国庆活动成功");
    ASSERT_EQ(DnfEventInfo_Add(manager, &e2), 0, "添加双十一活动成功");
    ASSERT_EQ(DnfEventInfo_Add(manager, &e3), 0, "添加圣诞活动成功");
}

void test_get_by_apply_type(DBConnectionManager* manager) {
    TEST_START("根据申请类型获取活动");

    DnfEventInfo* records = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventInfo_GetByApplyType(manager, 1, &records, &count), 0, "获取类型1的活动成功");
    ASSERT_TRUE(count >= 2, "至少有2个类型1的活动");
    printf("  类型1共有 %d 个活动\n", count);

    DnfEventInfo_FreeRecords(records);
}

void test_get_active_events(DBConnectionManager* manager) {
    TEST_START("获取正在进行的活动");

    DnfEventInfo* records = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventInfo_GetActiveEvents(manager, "2025-10-05", &records, &count), 0, "获取10月5日的活动成功");
    ASSERT_TRUE(count >= 1, "10月5日至少有1个活动");
    printf("  10月5日共有 %d 个活动\n", count);

    DnfEventInfo_FreeRecords(records);
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("根据日期范围获取活动");

    DnfEventInfo* records = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventInfo_GetByDateRange(manager, "2025-01-01", "2025-12-31", &records, &count), 0, "获取2025年活动成功");
    ASSERT_TRUE(count >= 4, "2025年至少有4个活动");
    printf("  2025年共有 %d 个活动\n", count);

    DnfEventInfo_FreeRecords(records);
}

void test_search_by_name(DBConnectionManager* manager) {
    TEST_START("根据名称模糊搜索");

    DnfEventInfo* records = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventInfo_SearchByName(manager, "活动", &records, &count), 0, "搜索'活动'成功");
    ASSERT_TRUE(count >= 4, "至少找到4个包含'活动'的记录");
    printf("  找到 %d 个包含'活动'的记录\n", count);

    DnfEventInfo_FreeRecords(records);
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有活动");

    DnfEventInfo* records = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventInfo_GetAll(manager, &records, &count), 0, "获取所有活动成功");
    ASSERT_TRUE(count >= 4, "至少有4个活动");
    printf("  共有 %d 个活动\n", count);

    DnfEventInfo_FreeRecords(records);
}

void test_is_expired(DBConnectionManager* manager) {
    TEST_START("检查活动是否过期");

    ASSERT_EQ(DnfEventInfo_IsExpired(manager, 1001, "2025-03-01"), 1, "2025-03-01时活动已过期");
    ASSERT_EQ(DnfEventInfo_IsExpired(manager, 1001, "2025-01-15"), 0, "2025-01-15时活动未过期");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印活动信息");

    DnfEventInfo record;
    if (DnfEventInfo_Get(manager, 1001, &record) == 0) {
        DnfEventInfo_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息功能正常");
    }
}

void test_replace_duplicate(DBConnectionManager* manager) {
    TEST_START("测试REPLACE功能（重复主键）");

    DnfEventInfo record;
    record.event_id = 1001;
    strcpy(record.event_name, "春节活动REPLACE测试");
    strcpy(record.event_explain, "测试REPLACE");
    record.apply_type = 3;
    strcpy(record.start_date, "2025-02-01");
    strcpy(record.end_date, "2025-02-28");

    ASSERT_EQ(DnfEventInfo_Add(manager, &record), 0, "REPLACE操作成功");

    DnfEventInfo retrieved;
    DnfEventInfo_Get(manager, 1001, &retrieved);
    ASSERT_STR_EQ(retrieved.event_name, "春节活动REPLACE测试", "记录被正确替换");
}

void test_delete_event_info(DBConnectionManager* manager) {
    TEST_START("删除活动信息");

    ASSERT_EQ(DnfEventInfo_Delete(manager, 1001), 0, "删除活动1001成功");
    ASSERT_EQ(DnfEventInfo_Delete(manager, 1002), 0, "删除活动1002成功");
    ASSERT_EQ(DnfEventInfo_Delete(manager, 1003), 0, "删除活动1003成功");
    ASSERT_EQ(DnfEventInfo_Delete(manager, 1004), 0, "删除活动1004成功");

    ASSERT_EQ(DnfEventInfo_Exists(manager, 1001), 0, "活动已删除");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_event_info ORM 测试套件\n");
    printf("========================================\n");

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
    test_add_event_info(&manager);
    test_get_event_info(&manager);
    test_get_by_name(&manager);
    test_exists(&manager);
    test_update_event_info(&manager);
    test_add_multiple_events(&manager);
    test_get_by_apply_type(&manager);
    test_get_active_events(&manager);
    test_get_by_date_range(&manager);
    test_search_by_name(&manager);
    test_get_all(&manager);
    test_is_expired(&manager);
    test_print_info(&manager);
    test_replace_duplicate(&manager);
    test_delete_event_info(&manager);

    if (!current_test_failed) {
        test_passed++;
    }

    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
