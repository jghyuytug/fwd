#include "dnf_pcroom_orm.h"
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

#define ASSERT_TRUE(condition, message) \
    do { \
        assertion_count++; \
        if (condition) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
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

/* 测试数据 */
#define TEST_DISTRICT "测试区"
#define TEST_FIRM_NAME "测试网吧"
#define TEST_TELEPHONE "1234567890"
#define TEST_ADDRESS "测试地址123号"
#define TEST_LEADER "测试负责人"
#define TEST_IP "192.168.1"
#define TEST_START_IP 10
#define TEST_END_IP 50

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfPcroom record, retrieved;
    DnfPcroom records[100];
    int ret, new_id, count;

    printf("========================================\n");
    printf("DnfPcroom ORM 测试套件\n");
    printf("========================================\n");

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    ASSERT_EQ(ret, 0, "数据库连接管理器初始化成功");

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    ASSERT_EQ(ret, 0, "数据库连接成功");

    /*==========================================================================
     * 测试 1: 添加网吧信息
     *=========================================================================*/
    TEST_START("添加网吧信息");

    memset(&record, 0, sizeof(DnfPcroom));
    strncpy(record.district, TEST_DISTRICT, sizeof(record.district) - 1);
    strncpy(record.firm_name, TEST_FIRM_NAME, sizeof(record.firm_name) - 1);
    strncpy(record.telephone, TEST_TELEPHONE, sizeof(record.telephone) - 1);
    strncpy(record.address, TEST_ADDRESS, sizeof(record.address) - 1);
    strncpy(record.leader, TEST_LEADER, sizeof(record.leader) - 1);
    strncpy(record.ip, TEST_IP, sizeof(record.ip) - 1);
    record.start_ip = TEST_START_IP;
    record.end_ip = TEST_END_IP;

    new_id = DnfPcroom_Add(&manager, &record);
    ASSERT_GT(new_id, 0, "DnfPcroom_Add 返回有效ID");

    ret = DnfPcroom_Exists(&manager, new_id);
    ASSERT_EQ(ret, 1, "记录存在");

    /*==========================================================================
     * 测试 2: 获取网吧信息
     *=========================================================================*/
    TEST_START("获取网吧信息");

    memset(&retrieved, 0, sizeof(DnfPcroom));
    ret = DnfPcroom_Get(&manager, new_id, &retrieved);
    ASSERT_EQ(ret, 0, "DnfPcroom_Get 返回成功");
    ASSERT_EQ(retrieved.ip_no, new_id, "IP编号匹配");
    ASSERT_STREQ(retrieved.district, TEST_DISTRICT, "地区匹配");
    ASSERT_STREQ(retrieved.firm_name, TEST_FIRM_NAME, "网吧名称匹配");
    ASSERT_STREQ(retrieved.telephone, TEST_TELEPHONE, "电话匹配");
    ASSERT_STREQ(retrieved.address, TEST_ADDRESS, "地址匹配");
    ASSERT_STREQ(retrieved.leader, TEST_LEADER, "负责人匹配");
    ASSERT_STREQ(retrieved.ip, TEST_IP, "IP前缀匹配");
    ASSERT_EQ(retrieved.start_ip, TEST_START_IP, "起始IP匹配");
    ASSERT_EQ(retrieved.end_ip, TEST_END_IP, "结束IP匹配");

    /*==========================================================================
     * 测试 3: 更新网吧信息
     *=========================================================================*/
    TEST_START("更新网吧信息");

    strncpy(record.firm_name, "更新后的网吧", sizeof(record.firm_name) - 1);
    strncpy(record.telephone, "9876543210", sizeof(record.telephone) - 1);
    record.ip_no = new_id;
    record.start_ip = 20;
    record.end_ip = 60;

    ret = DnfPcroom_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "DnfPcroom_Update 返回成功");

    memset(&retrieved, 0, sizeof(DnfPcroom));
    ret = DnfPcroom_Get(&manager, new_id, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_STREQ(retrieved.firm_name, "更新后的网吧", "网吧名称已更新");
    ASSERT_STREQ(retrieved.telephone, "9876543210", "电话已更新");
    ASSERT_EQ(retrieved.start_ip, 20, "起始IP已更新");
    ASSERT_EQ(retrieved.end_ip, 60, "结束IP已更新");

    /*==========================================================================
     * 测试 4: 按地区查询
     *=========================================================================*/
    TEST_START("按地区查询网吧");

    memset(records, 0, sizeof(records));
    count = DnfPcroom_GetByDistrict(&manager, TEST_DISTRICT, records, 100);
    ASSERT_GT(count, 0, "找到至少一个网吧");
    ASSERT_STREQ(records[0].district, TEST_DISTRICT, "第一条记录的地区正确");

    /*==========================================================================
     * 测试 5: 按网吧名称模糊搜索
     *=========================================================================*/
    TEST_START("按网吧名称模糊搜索");

    memset(records, 0, sizeof(records));
    count = DnfPcroom_SearchByFirmName(&manager, "更新", records, 100);
    ASSERT_GT(count, 0, "找到至少一个匹配的网吧");
    ASSERT_TRUE(strstr(records[0].firm_name, "更新") != NULL, "网吧名称包含搜索关键词");

    /*==========================================================================
     * 测试 6: 检查IP是否在网吧范围内
     *=========================================================================*/
    TEST_START("检查IP是否在网吧范围内");

    memset(&retrieved, 0, sizeof(DnfPcroom));
    ret = DnfPcroom_CheckIpInRange(&manager, "192.168.1.30", &retrieved);
    ASSERT_EQ(ret, 1, "找到匹配的网吧");
    ASSERT_EQ(retrieved.ip_no, new_id, "找到的网吧ID正确");

    /* 测试超出范围的IP */
    memset(&retrieved, 0, sizeof(DnfPcroom));
    ret = DnfPcroom_CheckIpInRange(&manager, "192.168.1.100", &retrieved);
    ASSERT_EQ(ret, 0, "超出范围的IP没有找到匹配");

    /*==========================================================================
     * 测试 7: 按负责人查询
     *=========================================================================*/
    TEST_START("按负责人查询网吧");

    memset(records, 0, sizeof(records));
    count = DnfPcroom_GetByLeader(&manager, TEST_LEADER, records, 100);
    ASSERT_GT(count, 0, "找到至少一个网吧");
    ASSERT_STREQ(records[0].leader, TEST_LEADER, "负责人匹配");

    /*==========================================================================
     * 测试 8: 统计网吧总数
     *=========================================================================*/
    TEST_START("统计网吧总数");

    count = DnfPcroom_Count(&manager);
    ASSERT_GT(count, 0, "网吧总数大于0");
    printf("  当前网吧总数: %d\n", count);

    /*==========================================================================
     * 测试 9: 统计指定地区的网吧数量
     *=========================================================================*/
    TEST_START("统计指定地区的网吧数量");

    count = DnfPcroom_CountByDistrict(&manager, TEST_DISTRICT);
    ASSERT_GT(count, 0, "指定地区的网吧数量大于0");
    printf("  %s 地区网吧数量: %d\n", TEST_DISTRICT, count);

    /*==========================================================================
     * 测试 10: IP范围验证
     *=========================================================================*/
    TEST_START("IP范围验证");

    ret = DnfPcroom_ValidateIpRange(10, 50);
    ASSERT_EQ(ret, 1, "有效的IP范围 (10-50)");

    ret = DnfPcroom_ValidateIpRange(50, 10);
    ASSERT_EQ(ret, 0, "无效的IP范围 (50-10)");

    ret = DnfPcroom_ValidateIpRange(100, 100);
    ASSERT_EQ(ret, 1, "单个IP范围 (100-100)");

    /*==========================================================================
     * 测试 11: 添加无效IP范围（应该失败）
     *=========================================================================*/
    TEST_START("添加无效IP范围（应该失败）");

    memset(&record, 0, sizeof(DnfPcroom));
    strncpy(record.district, "测试区2", sizeof(record.district) - 1);
    strncpy(record.firm_name, "测试网吧2", sizeof(record.firm_name) - 1);
    strncpy(record.ip, "10.0.0", sizeof(record.ip) - 1);
    record.start_ip = 200;  /* 无效: start > end */
    record.end_ip = 100;

    ret = DnfPcroom_Add(&manager, &record);
    ASSERT_EQ(ret, -1, "添加无效IP范围失败（符合预期）");

    /*==========================================================================
     * 测试 12: 打印网吧信息
     *=========================================================================*/
    TEST_START("打印网吧信息");

    memset(&retrieved, 0, sizeof(DnfPcroom));
    ret = DnfPcroom_Get(&manager, new_id, &retrieved);
    ASSERT_EQ(ret, 0, "获取成功");

    printf("  打印测试数据:\n");
    DnfPcroom_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "打印函数执行成功");

    /*==========================================================================
     * 测试 13: 删除网吧信息
     *=========================================================================*/
    TEST_START("删除网吧信息");

    ret = DnfPcroom_Delete(&manager, new_id);
    ASSERT_EQ(ret, 0, "DnfPcroom_Delete 返回成功");

    ret = DnfPcroom_Exists(&manager, new_id);
    ASSERT_EQ(ret, 0, "记录已被删除");

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 最后一个测试的计数 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
