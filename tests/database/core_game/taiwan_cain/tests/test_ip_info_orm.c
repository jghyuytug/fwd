#include "ip_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 测试统计变量 */
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: \"%s\" != \"%s\")\n", message, (a), (b)); \
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    IpInfo record, retrieved;
    int result;
    int insert_id;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize DBConnectionManager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("  IP信息管理系统 ORM 测试套件\n");
    printf("========================================\n");

    /* ==================== CRUD 操作测试 ==================== */

    TEST_START("添加IP信息记录（Add）");
    memset(&record, 0, sizeof(IpInfo));
    record.m_id = 100001;
    strncpy(record.ip, "192.168.1.", sizeof(record.ip) - 1);
    record.start_ip = 10;
    record.end_ip = 20;
    strncpy(record.occ_time, "2025-11-16 10:00:00", sizeof(record.occ_time) - 1);
    record.ip_check = 0; /* 未检查 */
    record.vendor_no = 5001;
    record.speed_no = 2; /* 中速 */
    strncpy(record.start_time, "2025-11-16 10:00:00", sizeof(record.start_time) - 1);
    strncpy(record.end_time, "2025-11-16 18:00:00", sizeof(record.end_time) - 1);
    record.charge_flag = 1; /* 计费 */
    record.settle_no = 0;

    insert_id = IpInfo_Add(&manager, &record);
    ASSERT_GE(insert_id, 1, "插入成功并返回自增ID");

    TEST_START("获取IP信息记录（Get）");
    memset(&retrieved, 0, sizeof(IpInfo));
    result = IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_EQ(retrieved.m_id, 100001, "m_id匹配");
    ASSERT_STR_EQ(retrieved.ip, "192.168.1.", "IP前缀匹配");
    ASSERT_EQ(retrieved.start_ip, 10, "start_ip匹配");
    ASSERT_EQ(retrieved.end_ip, 20, "end_ip匹配");
    ASSERT_EQ(retrieved.ip_check, 0, "ip_check匹配（未检查）");
    ASSERT_EQ(retrieved.vendor_no, 5001, "vendor_no匹配");
    ASSERT_EQ(retrieved.speed_no, 2, "speed_no匹配（中速）");
    ASSERT_EQ(retrieved.charge_flag, 1, "charge_flag匹配（计费）");

    TEST_START("检查记录存在（Exists）");
    result = IpInfo_Exists(&manager, (unsigned int)insert_id);
    ASSERT_EQ(result, 1, "记录存在");

    result = IpInfo_Exists(&manager, 999999);
    ASSERT_EQ(result, 0, "不存在的记录返回0");

    TEST_START("更新IP信息记录（Update）");
    retrieved.ip_check = 1; /* 已检查 */
    retrieved.speed_no = 3; /* 高速 */
    retrieved.charge_flag = 0; /* 改为免费 */
    retrieved.settle_no = 8001;

    result = IpInfo_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");

    /* 验证更新 */
    memset(&retrieved, 0, sizeof(IpInfo));
    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    ASSERT_EQ(retrieved.ip_check, 1, "ip_check已更新（已检查）");
    ASSERT_EQ(retrieved.speed_no, 3, "speed_no已更新（高速）");
    ASSERT_EQ(retrieved.charge_flag, 0, "charge_flag已更新（免费）");
    ASSERT_EQ(retrieved.settle_no, 8001, "settle_no已更新");

    /* ==================== 专项操作测试 ==================== */

    TEST_START("IP范围检查（CheckIpInRange）");
    memset(&retrieved, 0, sizeof(IpInfo));
    result = IpInfo_CheckIpInRange(&manager, "192.168.1.", 15, &retrieved);
    ASSERT_EQ(result, 0, "IP在范围内（15在10-20之间）");
    ASSERT_EQ(retrieved.m_id, 100001, "匹配的记录m_id正确");

    result = IpInfo_CheckIpInRange(&manager, "192.168.1.", 5, &retrieved);
    ASSERT_EQ(result, -1, "IP不在范围内（5 < 10）");

    result = IpInfo_CheckIpInRange(&manager, "192.168.1.", 25, &retrieved);
    ASSERT_EQ(result, -1, "IP不在范围内（25 > 20）");

    result = IpInfo_CheckIpInRange(&manager, "192.168.2.", 15, &retrieved);
    ASSERT_EQ(result, -1, "IP前缀不匹配");

    TEST_START("更新IP检查状态（UpdateCheckStatus）");
    result = IpInfo_UpdateCheckStatus(&manager, (unsigned int)insert_id, 2); /* 异常 */
    ASSERT_EQ(result, 0, "更新检查状态成功");

    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    ASSERT_EQ(retrieved.ip_check, 2, "ip_check已更新为异常状态");

    TEST_START("更新计费状态（UpdateChargeStatus）");
    result = IpInfo_UpdateChargeStatus(&manager, (unsigned int)insert_id, 1, 9001);
    ASSERT_EQ(result, 0, "更新计费状态成功");

    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    ASSERT_EQ(retrieved.charge_flag, 1, "charge_flag已更新为计费");
    ASSERT_EQ(retrieved.settle_no, 9001, "settle_no已更新");

    TEST_START("延长结束时间（ExtendEndTime）");
    char original_end_time[20];
    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    strncpy(original_end_time, retrieved.end_time, sizeof(original_end_time) - 1);

    result = IpInfo_ExtendEndTime(&manager, (unsigned int)insert_id, 3); /* 延长3小时 */
    ASSERT_EQ(result, 0, "延长结束时间成功");

    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    ASSERT_NEQ(strcmp(retrieved.end_time, original_end_time), 0, "end_time已变化");

    /* ==================== 查询操作测试 ==================== */

    TEST_START("按会员ID统计（CountByMemberId）");
    /* 添加第二条记录 */
    memset(&record, 0, sizeof(IpInfo));
    record.m_id = 100001;
    strncpy(record.ip, "192.168.2.", sizeof(record.ip) - 1);
    record.start_ip = 1;
    record.end_ip = 50;
    strncpy(record.occ_time, "2025-11-16 11:00:00", sizeof(record.occ_time) - 1);
    record.ip_check = 0;
    record.vendor_no = 5002;
    record.speed_no = 1;
    strncpy(record.start_time, "2025-11-16 11:00:00", sizeof(record.start_time) - 1);
    strncpy(record.end_time, "2025-11-16 19:00:00", sizeof(record.end_time) - 1);
    record.charge_flag = 0;
    record.settle_no = 0;

    int insert_id2 = IpInfo_Add(&manager, &record);
    ASSERT_GE(insert_id2, 1, "第二条记录插入成功");

    result = IpInfo_CountByMemberId(&manager, 100001);
    ASSERT_EQ(result, 2, "会员100001有2条IP记录");

    result = IpInfo_CountByMemberId(&manager, 999999);
    ASSERT_EQ(result, 0, "不存在的会员返回0");

    TEST_START("按供应商统计（CountByVendor）");
    result = IpInfo_CountByVendor(&manager, 5001);
    ASSERT_EQ(result, 1, "供应商5001有1条记录");

    result = IpInfo_CountByVendor(&manager, 5002);
    ASSERT_EQ(result, 1, "供应商5002有1条记录");

    TEST_START("按IP前缀统计（CountByIpPrefix）");
    result = IpInfo_CountByIpPrefix(&manager, "192.168.1.");
    ASSERT_EQ(result, 1, "IP前缀192.168.1.有1条记录");

    result = IpInfo_CountByIpPrefix(&manager, "192.168.2.");
    ASSERT_EQ(result, 1, "IP前缀192.168.2.有1条记录");

    result = IpInfo_CountByIpPrefix(&manager, "10.0.0.");
    ASSERT_EQ(result, 0, "不存在的IP前缀返回0");

    TEST_START("统计有效记录（CountActive）");
    /* 添加一条过期记录 */
    memset(&record, 0, sizeof(IpInfo));
    record.m_id = 100002;
    strncpy(record.ip, "10.0.0.", sizeof(record.ip) - 1);
    record.start_ip = 100;
    record.end_ip = 200;
    strncpy(record.occ_time, "2020-01-01 10:00:00", sizeof(record.occ_time) - 1);
    record.ip_check = 0;
    record.vendor_no = 5003;
    record.speed_no = 1;
    strncpy(record.start_time, "2020-01-01 10:00:00", sizeof(record.start_time) - 1);
    strncpy(record.end_time, "2020-01-01 18:00:00", sizeof(record.end_time) - 1); /* 过期 */
    record.charge_flag = 0;
    record.settle_no = 0;

    int insert_id3 = IpInfo_Add(&manager, &record);

    result = IpInfo_CountActive(&manager);
    ASSERT_EQ(result, 2, "有效记录数为2（排除过期记录）");

    TEST_START("按检查状态统计（CountByCheckStatus）");
    result = IpInfo_CountByCheckStatus(&manager, 0); /* 未检查 */
    ASSERT_EQ(result, 2, "未检查状态有2条记录");

    result = IpInfo_CountByCheckStatus(&manager, 2); /* 异常 */
    ASSERT_EQ(result, 1, "异常状态有1条记录");

    result = IpInfo_CountByCheckStatus(&manager, 1); /* 已检查 */
    ASSERT_EQ(result, 0, "已检查状态有0条记录");

    TEST_START("批量删除过期记录（DeleteExpired）");
    result = IpInfo_DeleteExpired(&manager);
    ASSERT_GE(result, 1, "删除了至少1条过期记录");

    /* 验证过期记录已删除 */
    result = IpInfo_Exists(&manager, (unsigned int)insert_id3);
    ASSERT_EQ(result, 0, "过期记录已被删除");

    /* ==================== 工具函数测试 ==================== */

    TEST_START("打印IP信息（PrintInfo）");
    IpInfo_Get(&manager, (unsigned int)insert_id, &retrieved);
    printf("  调用 PrintInfo:\n");
    IpInfo_PrintInfo(&retrieved);
    ASSERT_EQ(1, 1, "PrintInfo执行成功（目测输出）");

    /* ==================== 清理测试数据 ==================== */

    TEST_START("删除测试记录（Delete）");
    result = IpInfo_Delete(&manager, (unsigned int)insert_id);
    ASSERT_EQ(result, 0, "删除第一条记录成功");

    result = IpInfo_Delete(&manager, (unsigned int)insert_id2);
    ASSERT_EQ(result, 0, "删除第二条记录成功");

    result = IpInfo_Exists(&manager, (unsigned int)insert_id);
    ASSERT_EQ(result, 0, "记录已被删除");

    /* 统计最后一个测试用例 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理连接 */
    DBConnectionManager_Cleanup(&manager);

    /* ==================== 测试结果汇总 ==================== */

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count,
        (test_count > 0) ? (test_passed * 100.0 / test_count) : 0);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count,
        (assertion_count > 0) ? (assertion_passed * 100.0 / assertion_count) : 0);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        return 1;
    }
}
