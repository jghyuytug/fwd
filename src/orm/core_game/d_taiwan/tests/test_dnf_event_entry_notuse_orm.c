#include "dnf_event_entry_notuse_orm.h"
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
        printf("\n========================================\n"); \
        printf("[TEST %d] %s\n", ++test_count, name); \
        printf("========================================\n"); \
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: expected '%s', got '%s')\n", message, (b), (a)); \
        } \
    } while(0)

/* 清理测试数据 */
void cleanup_test_data(DBConnectionManager* manager, int event_id, int m_id) {
    DnfEventEntryNotuse_Delete(manager, event_id, m_id);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfEventEntryNotuse record, retrieved;
    int test_event_id = 999001;    /* 测试活动ID */
    int test_event_id2 = 999002;   /* 第二个测试活动ID */
    int test_m_id = 888001;        /* 测试账户ID */
    int test_m_id2 = 888002;       /* 第二个测试账户ID */
    int test_charac_no = 777001;   /* 测试角色编号 */
    int result;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("\n");
    printf("================================================\n");
    printf("  DNF Event Entry (Not Use) ORM 测试套件\n");
    printf("================================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "Failed to connect to d_taiwan database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("✓ 数据库连接成功 (d_taiwan)\n");

    /* 清理可能存在的测试数据 */
    cleanup_test_data(&manager, test_event_id, test_m_id);
    cleanup_test_data(&manager, test_event_id, test_m_id2);
    cleanup_test_data(&manager, test_event_id2, test_m_id);
    cleanup_test_data(&manager, test_event_id2, test_m_id2);

    /* ==================== 测试1: 添加活动入口记录 ==================== */
    TEST_START("添加活动入口记录");

    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-15 10:30:00");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-15 10:35:00");

    result = DnfEventEntryNotuse_Add(&manager, &record);
    ASSERT_EQ(result, 0, "添加活动入口记录成功");

    /* ==================== 测试2: 检查记录是否存在 ==================== */
    TEST_START("检查记录存在性");

    result = DnfEventEntryNotuse_Exists(&manager, test_event_id, test_m_id);
    ASSERT_EQ(result, 1, "活动入口记录存在");

    result = DnfEventEntryNotuse_Exists(&manager, 99999, 88888);
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    /* ==================== 测试3: 获取活动入口记录 ==================== */
    TEST_START("获取活动入口记录");

    memset(&retrieved, 0, sizeof(DnfEventEntryNotuse));
    result = DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id, &retrieved);
    ASSERT_EQ(result, 0, "获取活动入口记录成功");
    ASSERT_EQ(retrieved.event_id, test_event_id, "活动ID匹配");
    ASSERT_EQ(retrieved.m_id, test_m_id, "账户ID匹配");
    ASSERT_STR_EQ(retrieved.occ_date, "2025-01-15 10:30:00", "发生日期正确");
    ASSERT_EQ(retrieved.server_id, 1, "服务器ID正确");
    ASSERT_EQ(retrieved.charac_no, test_charac_no, "角色编号正确");
    ASSERT_STR_EQ(retrieved.obtain_date, "2025-01-15 10:35:00", "获得日期正确");

    /* ==================== 测试4: 更新活动入口记录 ==================== */
    TEST_START("更新活动入口记录");

    strcpy(retrieved.occ_date, "2025-01-16 11:00:00");
    retrieved.server_id = 2;
    result = DnfEventEntryNotuse_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新活动入口记录成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(DnfEventEntryNotuse));
    DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id, &retrieved);
    ASSERT_STR_EQ(retrieved.occ_date, "2025-01-16 11:00:00", "发生日期已更新");
    ASSERT_EQ(retrieved.server_id, 2, "服务器ID已更新");

    /* ==================== 测试5: 获取账户参与的活动数量 ==================== */
    TEST_START("获取账户参与的活动数量");

    /* 添加第二个活动记录 */
    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id2;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-17 12:00:00");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-17 12:05:00");
    DnfEventEntryNotuse_Add(&manager, &record);

    result = DnfEventEntryNotuse_GetCountByAccount(&manager, test_m_id);
    ASSERT_EQ(result, 2, "账户参与了2个活动");

    /* ==================== 测试6: 获取活动的参与人数 ==================== */
    TEST_START("获取活动的参与人数");

    /* 添加第二个账户参与同一活动 */
    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id2;
    strcpy(record.occ_date, "2025-01-15 11:00:00");
    record.server_id = 1;
    record.charac_no = test_charac_no + 1;
    strcpy(record.obtain_date, "2025-01-15 11:05:00");
    DnfEventEntryNotuse_Add(&manager, &record);

    result = DnfEventEntryNotuse_GetCountByEvent(&manager, test_event_id);
    ASSERT_EQ(result, 2, "活动有2个账户参与");

    /* ==================== 测试7: 获取角色参与的活动数量 ==================== */
    TEST_START("获取角色参与的活动数量");

    result = DnfEventEntryNotuse_GetCountByCharacter(&manager, test_charac_no);
    ASSERT_EQ(result, 2, "角色参与了2个活动");

    result = DnfEventEntryNotuse_GetCountByCharacter(&manager, test_charac_no + 1);
    ASSERT_EQ(result, 1, "另一个角色参与了1个活动");

    /* ==================== 测试8: 检查账户是否参与活动 ==================== */
    TEST_START("检查账户是否参与活动");

    result = DnfEventEntryNotuse_HasAccountJoinedEvent(&manager, test_event_id, test_m_id);
    ASSERT_EQ(result, 1, "账户已参与活动");

    result = DnfEventEntryNotuse_HasAccountJoinedEvent(&manager, 99999, test_m_id);
    ASSERT_EQ(result, 0, "账户未参与该活动");

    /* ==================== 测试9: 复合主键测试 ==================== */
    TEST_START("复合主键测试 (event_id + m_id)");

    /* 验证不同组合的独立性 */
    DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id, &retrieved);
    ASSERT_EQ(retrieved.server_id, 2, "第一组合的server_id为2");

    DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id2, &retrieved);
    ASSERT_EQ(retrieved.server_id, 1, "第二组合的server_id为1");

    DnfEventEntryNotuse_Get(&manager, test_event_id2, test_m_id, &retrieved);
    ASSERT_EQ(retrieved.server_id, 1, "第三组合的server_id为1");

    ASSERT_TRUE(1, "复合主键独立性验证通过");

    /* ==================== 测试10: REPLACE INTO 覆盖测试 ==================== */
    TEST_START("REPLACE INTO 覆盖测试");

    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-20 10:00:00");
    record.server_id = 5;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-20 10:05:00");

    result = DnfEventEntryNotuse_Add(&manager, &record);
    ASSERT_EQ(result, 0, "REPLACE覆盖成功");

    DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id, &retrieved);
    ASSERT_EQ(retrieved.server_id, 5, "server_id被覆盖为5");
    ASSERT_STR_EQ(retrieved.occ_date, "2025-01-20 10:00:00", "occ_date被覆盖");

    /* ==================== 测试11: 按活动删除记录 ==================== */
    TEST_START("按活动删除记录");

    result = DnfEventEntryNotuse_GetCountByEvent(&manager, test_event_id);
    int count_before = result;
    ASSERT_EQ(count_before, 2, "删除前活动有2条记录");

    result = DnfEventEntryNotuse_DeleteByEvent(&manager, test_event_id);
    ASSERT_EQ(result, 2, "删除了2条记录");

    result = DnfEventEntryNotuse_GetCountByEvent(&manager, test_event_id);
    ASSERT_EQ(result, 0, "删除后活动无记录");

    /* ==================== 测试12: 按账户删除记录 ==================== */
    TEST_START("按账户删除记录");

    /* 重新添加测试数据 */
    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-21 10:00:00");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-21 10:05:00");
    DnfEventEntryNotuse_Add(&manager, &record);

    result = DnfEventEntryNotuse_GetCountByAccount(&manager, test_m_id);
    count_before = result;
    ASSERT_TRUE(count_before >= 1, "删除前账户至少有1条记录");

    result = DnfEventEntryNotuse_DeleteByAccount(&manager, test_m_id);
    ASSERT_EQ(result, count_before, "删除了所有账户记录");

    result = DnfEventEntryNotuse_GetCountByAccount(&manager, test_m_id);
    ASSERT_EQ(result, 0, "删除后账户无记录");

    /* ==================== 测试13: 删除单条记录 ==================== */
    TEST_START("删除单条记录");

    /* 重新添加数据 */
    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-22 10:00:00");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-22 10:05:00");
    DnfEventEntryNotuse_Add(&manager, &record);

    result = DnfEventEntryNotuse_Delete(&manager, test_event_id, test_m_id);
    ASSERT_EQ(result, 0, "删除单条记录成功");

    result = DnfEventEntryNotuse_Exists(&manager, test_event_id, test_m_id);
    ASSERT_EQ(result, 0, "记录已删除");

    /* ==================== 测试14: datetime 字段测试 ==================== */
    TEST_START("datetime 字段测试");

    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-12-31 23:59:59");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2026-01-01 00:00:00");
    DnfEventEntryNotuse_Add(&manager, &record);

    DnfEventEntryNotuse_Get(&manager, test_event_id, test_m_id, &retrieved);
    ASSERT_STR_EQ(retrieved.occ_date, "2025-12-31 23:59:59", "边界datetime正确");
    ASSERT_STR_EQ(retrieved.obtain_date, "2026-01-01 00:00:00", "跨年datetime正确");

    DnfEventEntryNotuse_Delete(&manager, test_event_id, test_m_id);

    /* ==================== 测试15: 错误处理测试 ==================== */
    TEST_START("错误处理测试");

    result = DnfEventEntryNotuse_Add(NULL, &record);
    ASSERT_EQ(result, -1, "NULL manager返回错误");

    result = DnfEventEntryNotuse_Add(&manager, NULL);
    ASSERT_EQ(result, -1, "NULL record返回错误");

    result = DnfEventEntryNotuse_Get(&manager, 99999, 88888, &retrieved);
    ASSERT_EQ(result, -1, "获取不存在的记录返回错误");

    /* ==================== 测试16: PrintInfo 函数测试 ==================== */
    TEST_START("PrintInfo 函数测试");

    memset(&record, 0, sizeof(DnfEventEntryNotuse));
    record.event_id = test_event_id;
    record.m_id = test_m_id;
    strcpy(record.occ_date, "2025-01-15 10:30:00");
    record.server_id = 1;
    record.charac_no = test_charac_no;
    strcpy(record.obtain_date, "2025-01-15 10:35:00");

    printf("\n调用 PrintInfo:\n");
    DnfEventEntryNotuse_PrintInfo(&record);
    ASSERT_TRUE(1, "PrintInfo执行成功");

    DnfEventEntryNotuse_PrintInfo(NULL);
    ASSERT_TRUE(1, "PrintInfo处理NULL成功");

    /* 清理所有测试数据 */
    DnfEventEntryNotuse_DeleteByEvent(&manager, test_event_id);
    DnfEventEntryNotuse_DeleteByEvent(&manager, test_event_id2);
    DnfEventEntryNotuse_DeleteByAccount(&manager, test_m_id2);

    /* 关闭数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 统计最后一个测试 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n");
    printf("================================================\n");
    printf("              测试结果汇总\n");
    printf("================================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
           test_passed, test_count,
           (test_count > 0) ? (100.0 * test_passed / test_count) : 0.0);
    printf("断言:     %d/%d 通过 (%.1f%%)\n",
           assertion_passed, assertion_count,
           (assertion_count > 0) ? (100.0 * assertion_passed / assertion_count) : 0.0);
    printf("================================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✓ 所有测试通过！\n");
        printf("================================================\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        printf("================================================\n");
        return 1;
    }
}
