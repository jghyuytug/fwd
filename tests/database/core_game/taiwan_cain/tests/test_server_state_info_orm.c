#include "server_state_info_orm.h"
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

#define ASSERT_STREQ(a, b, message) \
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

#define ASSERT_MEMEQ(a, b, len, message) \
    do { \
        assertion_count++; \
        if (memcmp((a), (b), (len)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: binary data mismatch)\n", message); \
        } \
    } while(0)

/* 辅助函数：创建测试表 */
void create_test_table(DBConnectionManager* manager) {
    char query[4096];
    DBQueryResult result;

    snprintf(query, sizeof(query),
        "CREATE TABLE IF NOT EXISTS server_state_info ("
        "  category INT(11) NOT NULL DEFAULT -1,"
        "  code INT(11) NOT NULL DEFAULT -1,"
        "  state BINARY(12) NOT NULL DEFAULT '\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0',"
        "  start_time DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "  end_time DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "  PRIMARY KEY (category, code)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}

/* 辅助函数：清空测试表 */
void cleanup_test_table(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    snprintf(query, sizeof(query), "TRUNCATE TABLE server_state_info");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 创建测试表 */
    create_test_table(&manager);
    cleanup_test_table(&manager);

    /* 测试数据 */
    ServerStateInfo record;
    memset(&record, 0, sizeof(ServerStateInfo));
    record.category = 1;
    record.code = 100;
    /* 设置状态数据（12字节）*/
    unsigned char test_state[SERVER_STATE_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                                     0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
    memcpy(record.state, test_state, SERVER_STATE_SIZE);
    snprintf(record.start_time, sizeof(record.start_time), "2025-01-01 10:00:00");
    snprintf(record.end_time, sizeof(record.end_time), "2025-12-31 23:59:59");

    /* ========== 测试 CRUD 操作 ========== */

    TEST_START("Add - 添加服务器状态信息");
    int ret = ServerStateInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加成功");

    TEST_START("Exists - 检查记录是否存在");
    ret = ServerStateInfo_Exists(&manager, 1, 100);
    ASSERT_EQ(ret, 1, "记录存在");

    TEST_START("Get - 获取服务器状态信息");
    ServerStateInfo retrieved;
    memset(&retrieved, 0, sizeof(ServerStateInfo));
    ret = ServerStateInfo_Get(&manager, 1, 100, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");
    ASSERT_EQ(retrieved.category, 1, "category字段正确");
    ASSERT_EQ(retrieved.code, 100, "code字段正确");
    ASSERT_MEMEQ(retrieved.state, test_state, SERVER_STATE_SIZE, "state字段正确");
    ASSERT_STREQ(retrieved.start_time, "2025-01-01 10:00:00", "start_time字段正确");
    ASSERT_STREQ(retrieved.end_time, "2025-12-31 23:59:59", "end_time字段正确");

    TEST_START("Update - 更新服务器状态信息");
    unsigned char new_state[SERVER_STATE_SIZE] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA,
                                                    0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4};
    memcpy(retrieved.state, new_state, SERVER_STATE_SIZE);
    snprintf(retrieved.start_time, sizeof(retrieved.start_time), "2025-06-01 00:00:00");
    snprintf(retrieved.end_time, sizeof(retrieved.end_time), "2025-12-31 00:00:00");
    ret = ServerStateInfo_Update(&manager, &retrieved);
    ASSERT_EQ(ret, 0, "更新成功");

    /* 验证更新 */
    ServerStateInfo updated;
    memset(&updated, 0, sizeof(ServerStateInfo));
    ServerStateInfo_Get(&manager, 1, 100, &updated);
    ASSERT_MEMEQ(updated.state, new_state, SERVER_STATE_SIZE, "更新后state字段正确");
    ASSERT_STREQ(updated.start_time, "2025-06-01 00:00:00", "更新后start_time正确");

    TEST_START("Add - REPLACE INTO幂等性测试");
    record.category = 1;
    record.code = 100;
    memcpy(record.state, test_state, SERVER_STATE_SIZE);
    ret = ServerStateInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "重复添加成功（REPLACE INTO）");

    /* ========== 测试专项操作 ========== */

    TEST_START("UpdateState - 更新状态数据");
    unsigned char state_update[SERVER_STATE_SIZE] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                                                       0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
    ret = ServerStateInfo_UpdateState(&manager, 1, 100, state_update);
    ASSERT_EQ(ret, 0, "更新状态数据成功");

    ServerStateInfo state_check;
    memset(&state_check, 0, sizeof(ServerStateInfo));
    ServerStateInfo_Get(&manager, 1, 100, &state_check);
    ASSERT_MEMEQ(state_check.state, state_update, SERVER_STATE_SIZE, "状态数据更新正确");

    TEST_START("UpdateTimeRange - 更新时间范围");
    ret = ServerStateInfo_UpdateTimeRange(&manager, 1, 100, "2025-07-01 00:00:00", "2025-07-31 23:59:59");
    ASSERT_EQ(ret, 0, "更新时间范围成功");

    ServerStateInfo time_check;
    memset(&time_check, 0, sizeof(ServerStateInfo));
    ServerStateInfo_Get(&manager, 1, 100, &time_check);
    ASSERT_STREQ(time_check.start_time, "2025-07-01 00:00:00", "开始时间更新正确");
    ASSERT_STREQ(time_check.end_time, "2025-07-31 23:59:59", "结束时间更新正确");

    TEST_START("ExtendEndTime - 延长结束时间");
    ret = ServerStateInfo_ExtendEndTime(&manager, 1, 100, 86400);  /* 延长1天 */
    ASSERT_EQ(ret, 0, "延长结束时间成功");

    ServerStateInfo extend_check;
    memset(&extend_check, 0, sizeof(ServerStateInfo));
    ServerStateInfo_Get(&manager, 1, 100, &extend_check);
    ASSERT_STREQ(extend_check.end_time, "2025-08-01 23:59:59", "结束时间延长正确");

    /* ========== 测试查询操作 ========== */

    /* 添加更多测试数据 */
    ServerStateInfo record2;
    memset(&record2, 0, sizeof(ServerStateInfo));
    record2.category = 1;
    record2.code = 101;
    memcpy(record2.state, test_state, SERVER_STATE_SIZE);
    snprintf(record2.start_time, sizeof(record2.start_time), "2020-01-01 00:00:00");
    snprintf(record2.end_time, sizeof(record2.end_time), "2020-12-31 23:59:59");  /* 过期 */
    ServerStateInfo_Add(&manager, &record2);

    ServerStateInfo record3;
    memset(&record3, 0, sizeof(ServerStateInfo));
    record3.category = 2;
    record3.code = 200;
    memcpy(record3.state, test_state, SERVER_STATE_SIZE);
    snprintf(record3.start_time, sizeof(record3.start_time), "2025-01-01 00:00:00");
    snprintf(record3.end_time, sizeof(record3.end_time), "2026-12-31 23:59:59");
    ServerStateInfo_Add(&manager, &record3);

    TEST_START("Count - 获取所有记录总数");
    int count = ServerStateInfo_Count(&manager);
    ASSERT_EQ(count, 3, "总记录数为3");

    TEST_START("CountByCategory - 按分类统计");
    count = ServerStateInfo_CountByCategory(&manager, 1);
    ASSERT_EQ(count, 2, "分类1有2条记录");

    count = ServerStateInfo_CountByCategory(&manager, 2);
    ASSERT_EQ(count, 1, "分类2有1条记录");

    TEST_START("CountActive - 统计当前有效记录");
    count = ServerStateInfo_CountActive(&manager);
    ASSERT_EQ(count, 1, "当前有效记录数为1（未过期）");

    TEST_START("DeleteExpired - 删除过期记录");
    ret = ServerStateInfo_DeleteExpired(&manager);
    ASSERT_EQ(ret, 0, "删除过期记录成功");

    count = ServerStateInfo_Count(&manager);
    ASSERT_EQ(count, 1, "删除后总记录数为1");

    TEST_START("DeleteByCategory - 删除指定分类的所有记录");
    ret = ServerStateInfo_DeleteByCategory(&manager, 1);
    ASSERT_EQ(ret, 0, "删除分类1的记录成功");

    count = ServerStateInfo_Count(&manager);
    ASSERT_EQ(count, 1, "删除后总记录数为1");

    /* ========== 测试边界条件 ========== */

    TEST_START("Delete - 删除不存在的记录");
    ret = ServerStateInfo_Delete(&manager, 999, 999);
    ASSERT_EQ(ret, 0, "删除不存在记录不报错");

    TEST_START("Get - 获取不存在的记录");
    ServerStateInfo notfound;
    memset(&notfound, 0, sizeof(ServerStateInfo));
    ret = ServerStateInfo_Get(&manager, 999, 999, &notfound);
    ASSERT_EQ(ret, -1, "获取不存在记录返回-1");

    TEST_START("Exists - 检查不存在的记录");
    ret = ServerStateInfo_Exists(&manager, 999, 999);
    ASSERT_EQ(ret, 0, "不存在的记录返回0");

    /* ========== 测试复合主键 ========== */

    TEST_START("复合主键 - 同category不同code");
    ServerStateInfo pk1;
    memset(&pk1, 0, sizeof(ServerStateInfo));
    pk1.category = 10;
    pk1.code = 1;
    memcpy(pk1.state, test_state, SERVER_STATE_SIZE);
    snprintf(pk1.start_time, sizeof(pk1.start_time), "2025-01-01 00:00:00");
    snprintf(pk1.end_time, sizeof(pk1.end_time), "2025-12-31 23:59:59");
    ServerStateInfo_Add(&manager, &pk1);

    ServerStateInfo pk2;
    memset(&pk2, 0, sizeof(ServerStateInfo));
    pk2.category = 10;
    pk2.code = 2;
    memcpy(pk2.state, test_state, SERVER_STATE_SIZE);
    snprintf(pk2.start_time, sizeof(pk2.start_time), "2025-01-01 00:00:00");
    snprintf(pk2.end_time, sizeof(pk2.end_time), "2025-12-31 23:59:59");
    ServerStateInfo_Add(&manager, &pk2);

    count = ServerStateInfo_CountByCategory(&manager, 10);
    ASSERT_EQ(count, 2, "同分类不同代码可以共存");

    /* ========== 清理 ========== */

    cleanup_test_table(&manager);
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    if (test_count > 0 && !current_test_failed) {
        test_passed++;
    }

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
