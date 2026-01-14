#include "limit_create_character_ip_orm.h"
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
        if (a && b && strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a ? a : "NULL", b ? b : "NULL"); \
        } \
    } while(0)

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    LimitCreateCharacterIP record, retrieved;
    int ret, count;
    LimitCreateCharacterIP* all_records;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize database connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    LimitCreateCharacterIP_Clear(&manager);

    /* 测试 1: IP转换函数 */
    TEST_START("IP Conversion - IP字符串和整数转换");
    unsigned int ip_int = LimitCreateCharacterIP_StrToInt("192.168.1.100");
    ASSERT_TRUE(ip_int > 0, "IP字符串转整数成功");

    char ip_str_buf[17];
    LimitCreateCharacterIP_IntToStr(ip_int, ip_str_buf, sizeof(ip_str_buf));
    ASSERT_STR_EQ(ip_str_buf, "192.168.1.100", "IP整数转字符串成功");

    /* 测试 2: 添加IP限制记录 */
    TEST_START("Add - 添加IP创建角色限制记录");
    memset(&record, 0, sizeof(LimitCreateCharacterIP));
    record.ip = ip_int;
    strcpy(record.ip_str, "192.168.1.100");
    record.count = 3;
    record.last_access_mid = 10001;
    strcpy(record.last_access_time, "2025-11-16 10:00:00");

    ret = LimitCreateCharacterIP_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    /* 测试 3: 查询记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = LimitCreateCharacterIP_Exists(&manager, ip_int);
    ASSERT_EQ(ret, 1, "记录存在");

    unsigned int fake_ip = LimitCreateCharacterIP_StrToInt("10.0.0.1");
    ret = LimitCreateCharacterIP_Exists(&manager, fake_ip);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 4: 获取IP限制记录（通过IP整数） */
    TEST_START("Get - 获取IP限制记录（通过IP整数）");
    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, ip_int, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.ip, ip_int, "IP整数正确");
    ASSERT_STR_EQ(retrieved.ip_str, "192.168.1.100", "IP字符串正确");
    ASSERT_EQ(retrieved.count, 3, "创建次数正确");
    ASSERT_EQ(retrieved.last_access_mid, 10001, "最后访问会员ID正确");

    LimitCreateCharacterIP_PrintInfo(&retrieved);

    /* 测试 5: 获取IP限制记录（通过IP字符串） */
    TEST_START("GetByIPStr - 获取IP限制记录（通过IP字符串）");
    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_GetByIPStr(&manager, "192.168.1.100", &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.ip, ip_int, "IP整数正确");
    ASSERT_EQ(retrieved.count, 3, "创建次数正确");

    /* 测试 6: 更新IP限制记录 */
    TEST_START("Update - 更新IP限制记录");
    record.count = 5;
    record.last_access_mid = 10002;
    strcpy(record.last_access_time, "2025-11-16 12:00:00");

    ret = LimitCreateCharacterIP_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, ip_int, &retrieved);
    ASSERT_EQ(retrieved.count, 5, "更新后次数正确");
    ASSERT_EQ(retrieved.last_access_mid, 10002, "更新后会员ID正确");

    /* 测试 7: REPLACE INTO 幂等性测试 */
    TEST_START("Add (REPLACE) - 测试 REPLACE INTO 幂等性");
    record.count = 7;
    strcpy(record.last_access_time, "2025-11-16 14:00:00");

    ret = LimitCreateCharacterIP_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 REPLACE 成功");

    ret = LimitCreateCharacterIP_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 REPLACE 成功（幂等性）");

    count = LimitCreateCharacterIP_Count(&manager);
    ASSERT_EQ(count, 1, "记录数量仍为1（未重复插入）");

    /* 测试 8: 增加创建次数 */
    TEST_START("IncrementCount - 增加创建次数");
    ret = LimitCreateCharacterIP_IncrementCount(&manager, ip_int, 10003);
    ASSERT_EQ(ret, 0, "增加次数成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, ip_int, &retrieved);
    ASSERT_EQ(retrieved.count, 8, "次数已增加");
    ASSERT_EQ(retrieved.last_access_mid, 10003, "会员ID已更新");

    /* 测试 9: 重置创建次数 */
    TEST_START("ResetCount - 重置创建次数");
    ret = LimitCreateCharacterIP_ResetCount(&manager, ip_int);
    ASSERT_EQ(ret, 0, "重置成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, ip_int, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "次数已重置为0");

    /* 测试 10: 更新访问会员ID */
    TEST_START("UpdateAccessMid - 更新最后访问会员ID");
    ret = LimitCreateCharacterIP_UpdateAccessMid(&manager, ip_int, 20001);
    ASSERT_EQ(ret, 0, "更新会员ID成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, ip_int, &retrieved);
    ASSERT_EQ(retrieved.last_access_mid, 20001, "会员ID已更新");

    /* 测试 11: 批量添加 */
    TEST_START("Batch Add - 批量添加IP限制记录");
    LimitCreateCharacterIP records[3];

    records[0].ip = LimitCreateCharacterIP_StrToInt("192.168.1.101");
    strcpy(records[0].ip_str, "192.168.1.101");
    records[0].count = 1;
    records[0].last_access_mid = 20001;
    strcpy(records[0].last_access_time, "2025-11-16 15:00:00");

    records[1].ip = LimitCreateCharacterIP_StrToInt("192.168.1.102");
    strcpy(records[1].ip_str, "192.168.1.102");
    records[1].count = 2;
    records[1].last_access_mid = 20002;
    strcpy(records[1].last_access_time, "2025-11-16 16:00:00");

    records[2].ip = LimitCreateCharacterIP_StrToInt("192.168.1.103");
    strcpy(records[2].ip_str, "192.168.1.103");
    records[2].count = 3;
    records[2].last_access_mid = 20003;
    strcpy(records[2].last_access_time, "2025-11-16 17:00:00");

    for (int i = 0; i < 3; i++) {
        ret = LimitCreateCharacterIP_Add(&manager, &records[i]);
        ASSERT_EQ(ret, 0, "批量添加记录成功");
    }

    count = LimitCreateCharacterIP_Count(&manager);
    ASSERT_EQ(count, 4, "总记录数为4");

    /* 测试 12: 获取所有记录 */
    TEST_START("GetAll - 获取所有IP限制记录");
    ret = LimitCreateCharacterIP_GetAll(&manager, &all_records, &count);
    ASSERT_EQ(ret, 0, "获取所有记录成功");
    ASSERT_EQ(count, 4, "记录数量正确");

    if (all_records) {
        free(all_records);
    }

    /* 测试 13: 按次数范围查询 */
    TEST_START("GetByCountRange - 按创建次数范围查询");
    ret = LimitCreateCharacterIP_GetByCountRange(&manager, 1, 2, &all_records, &count);
    ASSERT_EQ(ret, 0, "范围查询成功");
    ASSERT_EQ(count, 2, "查询到2条记录");

    if (all_records) {
        free(all_records);
    }

    /* 测试 14: 按会员ID查询 */
    TEST_START("GetByMid - 按会员ID查询IP记录");
    ret = LimitCreateCharacterIP_GetByMid(&manager, 20001, &all_records, &count);
    ASSERT_EQ(ret, 0, "按会员ID查询成功");
    ASSERT_TRUE(count > 0, "查询到记录");

    if (all_records) {
        free(all_records);
    }

    /* 测试 15: 批量重置次数 */
    TEST_START("BatchResetCount - 批量重置创建次数");
    unsigned int reset_ips[] = {records[0].ip, records[1].ip};
    ret = LimitCreateCharacterIP_BatchResetCount(&manager, reset_ips, 2);
    ASSERT_EQ(ret, 0, "批量重置成功");

    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, records[0].ip, &retrieved);
    ASSERT_EQ(retrieved.count, 0, "第一个IP次数已重置");

    /* 测试 16: 批量删除 */
    TEST_START("BatchDelete - 批量删除IP限制记录");
    unsigned int delete_ips[] = {records[0].ip, records[2].ip};
    ret = LimitCreateCharacterIP_BatchDelete(&manager, delete_ips, 2);
    ASSERT_EQ(ret, 0, "批量删除成功");

    count = LimitCreateCharacterIP_Count(&manager);
    ASSERT_EQ(count, 2, "删除后记录数为2");

    ret = LimitCreateCharacterIP_Exists(&manager, records[1].ip);
    ASSERT_EQ(ret, 1, "第二个IP仍然存在");

    ret = LimitCreateCharacterIP_Exists(&manager, records[0].ip);
    ASSERT_EQ(ret, 0, "第一个IP已删除");

    /* 测试 17: 删除单个记录 */
    TEST_START("Delete - 删除单个IP限制记录");
    ret = LimitCreateCharacterIP_Delete(&manager, ip_int);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = LimitCreateCharacterIP_Exists(&manager, ip_int);
    ASSERT_EQ(ret, 0, "记录已删除");

    /* 测试 18: 清空所有数据 */
    TEST_START("Clear - 清空所有IP限制数据");
    ret = LimitCreateCharacterIP_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = LimitCreateCharacterIP_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 19: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(LimitCreateCharacterIP));
    ret = LimitCreateCharacterIP_Get(&manager, fake_ip, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    LimitCreateCharacterIP_Clear(&manager);
    DBConnectionManager_Cleanup(&manager);

    /* 计算最后一个测试是否通过 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
