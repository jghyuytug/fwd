#include "dnf_master_charac_orm.h"
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a, b); \
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    DnfMasterCharac record, retrieved;
    int ret, count;

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
    DnfMasterCharac_Clear(&manager);

    /* 测试 1: 添加DNF主角色记录 */
    TEST_START("Add - 添加DNF主角色记录");
    memset(&record, 0, sizeof(DnfMasterCharac));
    record.m_id = 1001;
    record.global_type = 1;
    record.server_id = 11;
    record.charac_no = 5001;
    strcpy(record.charac_name, "MasterChar01");
    record.job = 0;
    record.lev = 60;

    ret = DnfMasterCharac_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    /* 测试 2: 检查记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = DnfMasterCharac_Exists(&manager, 1001, 1, 11);
    ASSERT_EQ(ret, 1, "记录存在");

    ret = DnfMasterCharac_Exists(&manager, 9999, 9, 99);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 3: 获取DNF主角色记录 */
    TEST_START("Get - 获取DNF主角色记录");
    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 1001, 1, 11, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.m_id, 1001, "会员ID正确");
    ASSERT_EQ(retrieved.global_type, 1, "全局类型正确");
    ASSERT_EQ(retrieved.server_id, 11, "服务器ID正确");
    ASSERT_EQ(retrieved.charac_no, 5001, "角色编号正确");
    ASSERT_STR_EQ(retrieved.charac_name, "MasterChar01", "角色名称正确");
    ASSERT_EQ(retrieved.job, 0, "职业正确");
    ASSERT_EQ(retrieved.lev, 60, "等级正确");

    DnfMasterCharac_PrintInfo(&retrieved);

    /* 测试 4: 更新记录 */
    TEST_START("Update - 更新DNF主角色记录");
    record.lev = 70;
    strcpy(record.charac_name, "MasterChar01_v2");
    ret = DnfMasterCharac_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 1001, 1, 11, &retrieved);
    ASSERT_EQ(retrieved.lev, 70, "更新后等级正确");
    ASSERT_STR_EQ(retrieved.charac_name, "MasterChar01_v2", "更新后名称正确");

    /* 测试 5: UpdateLevel - 直接更新等级 */
    TEST_START("UpdateLevel - 直接更新等级");
    ret = DnfMasterCharac_UpdateLevel(&manager, 1001, 1, 11, 80);
    ASSERT_EQ(ret, 0, "更新等级成功");

    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 1001, 1, 11, &retrieved);
    ASSERT_EQ(retrieved.lev, 80, "等级已更新为80");

    /* 测试 6: UpdateCharacNo - 更新角色编号 */
    TEST_START("UpdateCharacNo - 更新角色编号");
    ret = DnfMasterCharac_UpdateCharacNo(&manager, 1001, 1, 11, 5002);
    ASSERT_EQ(ret, 0, "更新角色编号成功");

    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 1001, 1, 11, &retrieved);
    ASSERT_EQ(retrieved.charac_no, 5002, "角色编号已更新为5002");

    /* 测试 7: SetMasterCharac - 设置主角色（幂等操作） */
    TEST_START("SetMasterCharac - 设置主角色（幂等操作）");
    ret = DnfMasterCharac_SetMasterCharac(&manager, 1001, 1, 12, 6001, "SecondMaster", 1, 50);
    ASSERT_EQ(ret, 0, "设置第二个主角色成功");

    ret = DnfMasterCharac_SetMasterCharac(&manager, 1001, 1, 12, 6002, "SecondMaster_v2", 1, 55);
    ASSERT_EQ(ret, 0, "重复设置主角色成功（REPLACE）");

    count = DnfMasterCharac_Count(&manager);
    ASSERT_EQ(count, 2, "总记录数为2");

    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 1001, 1, 12, &retrieved);
    ASSERT_EQ(retrieved.charac_no, 6002, "主角色编号已更新");
    ASSERT_EQ(retrieved.lev, 55, "主角色等级已更新");

    /* 测试 8: 批量添加不同服务器的主角色 */
    TEST_START("Batch Add - 批量添加不同服务器主角色");
    DnfMasterCharac chars[3];

    chars[0].m_id = 1001;
    chars[0].global_type = 1;
    chars[0].server_id = 13;
    chars[0].charac_no = 7001;
    strcpy(chars[0].charac_name, "Server13Master");
    chars[0].job = 2;
    chars[0].lev = 45;

    chars[1].m_id = 1001;
    chars[1].global_type = 2;
    chars[1].server_id = 11;
    chars[1].charac_no = 8001;
    strcpy(chars[1].charac_name, "Type2Master");
    chars[1].job = 3;
    chars[1].lev = 55;

    chars[2].m_id = 2001;
    chars[2].global_type = 1;
    chars[2].server_id = 11;
    chars[2].charac_no = 9001;
    strcpy(chars[2].charac_name, "User2Master");
    chars[2].job = 4;
    chars[2].lev = 65;

    for (int i = 0; i < 3; i++) {
        ret = DnfMasterCharac_Add(&manager, &chars[i]);
        ASSERT_EQ(ret, 0, "批量添加成功");
    }

    count = DnfMasterCharac_Count(&manager);
    ASSERT_EQ(count, 5, "总记录数为5");

    /* 测试 9: Delete - 删除单条记录 */
    TEST_START("Delete - 删除单条记录");
    ret = DnfMasterCharac_Delete(&manager, 1001, 1, 13);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = DnfMasterCharac_Exists(&manager, 1001, 1, 13);
    ASSERT_EQ(ret, 0, "记录已删除");

    count = DnfMasterCharac_Count(&manager);
    ASSERT_EQ(count, 4, "删除后剩余4条记录");

    /* 测试 10: DeleteByMid - 按会员ID删除 */
    TEST_START("DeleteByMid - 按会员ID删除所有主角色");
    ret = DnfMasterCharac_DeleteByMid(&manager, 1001);
    ASSERT_EQ(ret, 0, "按会员ID删除成功");

    count = DnfMasterCharac_Count(&manager);
    ASSERT_EQ(count, 1, "删除后剩余1条记录（用户2001）");

    /* 测试 11: DeleteByServer - 按服务器ID删除 */
    TEST_START("DeleteByServer - 按服务器ID删除");

    /* 先添加测试数据 */
    memset(&record, 0, sizeof(DnfMasterCharac));
    record.m_id = 3001;
    record.global_type = 1;
    record.server_id = 15;
    record.charac_no = 10001;
    strcpy(record.charac_name, "Server15Test");
    record.job = 5;
    record.lev = 70;
    DnfMasterCharac_Add(&manager, &record);

    record.m_id = 3002;
    record.server_id = 15;
    record.charac_no = 10002;
    strcpy(record.charac_name, "Server15Test2");
    DnfMasterCharac_Add(&manager, &record);

    ret = DnfMasterCharac_DeleteByServer(&manager, 15);
    ASSERT_EQ(ret, 0, "按服务器ID删除成功");

    ret = DnfMasterCharac_Exists(&manager, 3001, 1, 15);
    ASSERT_EQ(ret, 0, "服务器15的记录已删除");

    /* 测试 12: Clear - 清空所有数据 */
    TEST_START("Clear - 清空所有数据");
    ret = DnfMasterCharac_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = DnfMasterCharac_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 13: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(DnfMasterCharac));
    ret = DnfMasterCharac_Get(&manager, 9999, 9, 99, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    DnfMasterCharac_Clear(&manager);
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
