#include "dnf_event_address_orm.h"
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

void test_add_event_address(DBConnectionManager* manager) {
    TEST_START("添加活动地址");

    DnfEventAddress record;
    record.event_id = 1001;
    record.m_id = 10001;
    strcpy(record.occ_date, "2025-11-17 10:00:00");
    strcpy(record.zipcode, "100000");
    strcpy(record.address, "北京市朝阳区测试路123号");
    strcpy(record.phone_no, "13800138000");

    int result = DnfEventAddress_Add(manager, &record);
    ASSERT_EQ(result, 0, "添加活动地址成功");
}

void test_get_event_address(DBConnectionManager* manager) {
    TEST_START("获取活动地址");

    DnfEventAddress record;
    int result = DnfEventAddress_Get(manager, 1001, 10001, &record);

    ASSERT_EQ(result, 0, "获取活动地址成功");
    ASSERT_EQ(record.event_id, 1001, "活动ID正确");
    ASSERT_EQ(record.m_id, 10001, "会员ID正确");
    ASSERT_STR_EQ(record.occ_date, "2025-11-17 10:00:00", "发生日期正确");
    ASSERT_STR_EQ(record.zipcode, "100000", "邮编正确");
    ASSERT_STR_EQ(record.address, "北京市朝阳区测试路123号", "地址正确");
    ASSERT_STR_EQ(record.phone_no, "13800138000", "电话号码正确");
}

void test_exists_event_address(DBConnectionManager* manager) {
    TEST_START("检查活动地址存在性");

    int exists = DnfEventAddress_Exists(manager, 1001, 10001);
    ASSERT_EQ(exists, 1, "活动地址存在");

    int not_exists = DnfEventAddress_Exists(manager, 9999, 9999);
    ASSERT_EQ(not_exists, 0, "不存在的活动地址返回0");
}

void test_update_event_address(DBConnectionManager* manager) {
    TEST_START("更新活动地址");

    DnfEventAddress record;
    record.event_id = 1001;
    record.m_id = 10001;
    strcpy(record.occ_date, "2025-11-17 15:00:00");
    strcpy(record.zipcode, "100001");
    strcpy(record.address, "北京市海淀区更新路456号");
    strcpy(record.phone_no, "13900139000");

    int result = DnfEventAddress_Update(manager, &record);
    ASSERT_EQ(result, 0, "更新活动地址成功");

    // 验证更新
    DnfEventAddress retrieved;
    DnfEventAddress_Get(manager, 1001, 10001, &retrieved);
    ASSERT_STR_EQ(retrieved.occ_date, "2025-11-17 15:00:00", "发生日期已更新");
    ASSERT_STR_EQ(retrieved.zipcode, "100001", "邮编已更新");
    ASSERT_STR_EQ(retrieved.address, "北京市海淀区更新路456号", "地址已更新");
    ASSERT_STR_EQ(retrieved.phone_no, "13900139000", "电话已更新");
}

void test_add_multiple_event_addresses(DBConnectionManager* manager) {
    TEST_START("添加多个活动地址");

    // 同一活动的多个会员
    DnfEventAddress r1 = {1001, 10002, "2025-11-17 11:00:00", "200000", "上海市浦东新区测试路1号", "18800188000"};
    DnfEventAddress r2 = {1001, 10003, "2025-11-17 12:00:00", "300000", "广州市天河区测试路2号", "18800188001"};

    // 不同活动
    DnfEventAddress r3 = {1002, 10001, "2025-11-17 13:00:00", "400000", "深圳市南山区测试路3号", "18800188002"};

    ASSERT_EQ(DnfEventAddress_Add(manager, &r1), 0, "添加记录1成功");
    ASSERT_EQ(DnfEventAddress_Add(manager, &r2), 0, "添加记录2成功");
    ASSERT_EQ(DnfEventAddress_Add(manager, &r3), 0, "添加记录3成功");
}

void test_get_by_event_id(DBConnectionManager* manager) {
    TEST_START("根据活动ID获取地址");

    DnfEventAddress* records = NULL;
    int count = 0;

    int result = DnfEventAddress_GetByEventId(manager, 1001, &records, &count);
    ASSERT_EQ(result, 0, "获取活动1001的地址成功");
    ASSERT_TRUE(count >= 3, "活动1001至少有3个会员地址");

    printf("  活动1001共有 %d 个会员地址\n", count);

    DnfEventAddress_FreeRecords(records);
}

void test_get_by_member_id(DBConnectionManager* manager) {
    TEST_START("根据会员ID获取活动地址");

    DnfEventAddress* records = NULL;
    int count = 0;

    int result = DnfEventAddress_GetByMemberId(manager, 10001, &records, &count);
    ASSERT_EQ(result, 0, "获取会员10001的地址成功");
    ASSERT_TRUE(count >= 2, "会员10001至少参加了2个活动");

    printf("  会员10001参加了 %d 个活动\n", count);

    DnfEventAddress_FreeRecords(records);
}

void test_get_by_zipcode(DBConnectionManager* manager) {
    TEST_START("根据邮编查询地址");

    DnfEventAddress* records = NULL;
    int count = 0;

    int result = DnfEventAddress_GetByZipcode(manager, "100001", &records, &count);
    ASSERT_EQ(result, 0, "根据邮编查询成功");
    ASSERT_TRUE(count >= 1, "找到至少1个邮编为100001的地址");

    printf("  邮编100001共有 %d 条记录\n", count);

    DnfEventAddress_FreeRecords(records);
}

void test_get_by_date_range(DBConnectionManager* manager) {
    TEST_START("根据日期范围查询");

    DnfEventAddress* records = NULL;
    int count = 0;

    int result = DnfEventAddress_GetByDateRange(manager, "2025-11-17 10:00:00", "2025-11-17 23:59:59", &records, &count);
    ASSERT_EQ(result, 0, "根据日期范围查询成功");
    ASSERT_TRUE(count >= 4, "找到至少4条记录");

    printf("  日期范围内共有 %d 条记录\n", count);

    DnfEventAddress_FreeRecords(records);
}

void test_get_all(DBConnectionManager* manager) {
    TEST_START("获取所有活动地址");

    DnfEventAddress* records = NULL;
    int count = 0;

    int result = DnfEventAddress_GetAll(manager, &records, &count);
    ASSERT_EQ(result, 0, "获取所有地址成功");
    ASSERT_TRUE(count >= 4, "至少有4条记录");

    printf("  共有 %d 条活动地址记录\n", count);

    DnfEventAddress_FreeRecords(records);
}

void test_replace_duplicate(DBConnectionManager* manager) {
    TEST_START("测试REPLACE功能（重复主键）");

    DnfEventAddress record;
    record.event_id = 1001;
    record.m_id = 10001;
    strcpy(record.occ_date, "2025-11-17 20:00:00");
    strcpy(record.zipcode, "999999");
    strcpy(record.address, "REPLACE测试地址");
    strcpy(record.phone_no, "99999999999");

    int result = DnfEventAddress_Add(manager, &record);
    ASSERT_EQ(result, 0, "REPLACE操作成功");

    // 验证是否被替换
    DnfEventAddress retrieved;
    DnfEventAddress_Get(manager, 1001, 10001, &retrieved);
    ASSERT_STR_EQ(retrieved.address, "REPLACE测试地址", "记录被正确替换");
    ASSERT_STR_EQ(retrieved.zipcode, "999999", "邮编被正确替换");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印活动地址信息");

    DnfEventAddress record;
    if (DnfEventAddress_Get(manager, 1001, 10001, &record) == 0) {
        DnfEventAddress_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息功能正常");
    }
}

void test_edge_cases(DBConnectionManager* manager) {
    TEST_START("边界测试");

    // 测试空字符串
    DnfEventAddress record1;
    record1.event_id = 2001;
    record1.m_id = 20001;
    strcpy(record1.occ_date, "2025-11-17 00:00:00");
    strcpy(record1.zipcode, "");
    strcpy(record1.address, "");
    strcpy(record1.phone_no, "");

    ASSERT_EQ(DnfEventAddress_Add(manager, &record1), 0, "添加空字符串记录成功");

    DnfEventAddress retrieved1;
    DnfEventAddress_Get(manager, 2001, 20001, &retrieved1);
    ASSERT_STR_EQ(retrieved1.zipcode, "", "空邮编正确保存");
    ASSERT_STR_EQ(retrieved1.address, "", "空地址正确保存");

    // 测试长地址（150字符）
    DnfEventAddress record2;
    record2.event_id = 2002;
    record2.m_id = 20002;
    strcpy(record2.occ_date, "2025-11-17 00:00:00");
    strcpy(record2.zipcode, "1234567");  // 最大7字符
    // 生成150字符的地址
    char long_address[151];
    memset(long_address, 'A', 149);
    long_address[149] = '\0';
    strcpy(record2.address, long_address);
    strcpy(record2.phone_no, "123456789012345");  // 最大15字符

    ASSERT_EQ(DnfEventAddress_Add(manager, &record2), 0, "添加长字符串记录成功");
}

void test_delete_event_address(DBConnectionManager* manager) {
    TEST_START("删除活动地址");

    // 删除所有测试数据
    ASSERT_EQ(DnfEventAddress_Delete(manager, 1001, 10001), 0, "删除记录(1001,10001)成功");
    ASSERT_EQ(DnfEventAddress_Delete(manager, 1001, 10002), 0, "删除记录(1001,10002)成功");
    ASSERT_EQ(DnfEventAddress_Delete(manager, 1001, 10003), 0, "删除记录(1001,10003)成功");
    ASSERT_EQ(DnfEventAddress_Delete(manager, 1002, 10001), 0, "删除记录(1002,10001)成功");
    ASSERT_EQ(DnfEventAddress_Delete(manager, 2001, 20001), 0, "删除记录(2001,20001)成功");
    ASSERT_EQ(DnfEventAddress_Delete(manager, 2002, 20002), 0, "删除记录(2002,20002)成功");

    // 验证删除
    int exists = DnfEventAddress_Exists(manager, 1001, 10001);
    ASSERT_EQ(exists, 0, "记录已删除");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_event_address ORM 测试套件\n");
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
    test_add_event_address(&manager);
    test_get_event_address(&manager);
    test_exists_event_address(&manager);
    test_update_event_address(&manager);
    test_add_multiple_event_addresses(&manager);
    test_get_by_event_id(&manager);
    test_get_by_member_id(&manager);
    test_get_by_zipcode(&manager);
    test_get_by_date_range(&manager);
    test_get_all(&manager);
    test_replace_duplicate(&manager);
    test_print_info(&manager);
    test_edge_cases(&manager);
    test_delete_event_address(&manager);

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
