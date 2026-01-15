/**
 * DNF游戏服务器 - MemberPunishHack表ORM测试程序
 * 测试黑客处罚记录系统的20个功能点
 */

#include "member_punish_hack_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_MEMBER_1 200001
#define TEST_MEMBER_2 200002
#define TEST_IP_1 "192.168.100.10"
#define TEST_IP_2 "192.168.100.20"

int g_test_passed = 0;
int g_test_failed = 0;

#define TEST_ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("[测试通过] %s\n", test_name); \
            g_test_passed++; \
        } else { \
            printf("[测试失败] %s\n", test_name); \
            g_test_failed++; \
        } \
    } while(0)

/* ========================================================================
 * 测试用例
 * ======================================================================== */

void test_add_hack(DBConnectionManager* manager) {
    MemberPunishHack hack;
    MemberPunishHack_Init(&hack);

    hack.m_id = TEST_MEMBER_1;
    hack.occ_time = (unsigned int)time(NULL);
    hack.period = 86400; /* 1天 */
    hack.now_flag = HACK_FLAG_ACTIVE;
    hack.auto_flag = HACK_AUTO_SYSTEM;
    strcpy(hack.reason, "检测到外挂行为");
    strcpy(hack.hack_ip, TEST_IP_1);

    int ret = MemberPunishHack_Add(manager, &hack);
    TEST_ASSERT(ret == 0, "测试1: 添加黑客处罚记录");
}

void test_get_hack(DBConnectionManager* manager) {
    MemberPunishHack hack;
    int ret = MemberPunishHack_Get(manager, TEST_MEMBER_1, &hack);
    TEST_ASSERT(ret == 0 && hack.m_id == TEST_MEMBER_1, "测试2: 获取黑客处罚记录");
}

void test_exists(DBConnectionManager* manager) {
    bool exists = MemberPunishHack_Exists(manager, TEST_MEMBER_1);
    TEST_ASSERT(exists == true, "测试3: 检查记录是否存在");
}

void test_has_active_punish(DBConnectionManager* manager) {
    bool has_active = MemberPunishHack_HasActiveHackPunish(manager, TEST_MEMBER_1);
    TEST_ASSERT(has_active == true, "测试4: 检查是否有生效的黑客处罚");
}

void test_add_multiple(DBConnectionManager* manager) {
    MemberPunishHack hack2;
    MemberPunishHack_Init(&hack2);

    hack2.m_id = TEST_MEMBER_2;
    hack2.occ_time = (unsigned int)time(NULL);
    hack2.period = 172800; /* 2天 */
    hack2.now_flag = HACK_FLAG_ACTIVE;
    hack2.auto_flag = HACK_AUTO_MANUAL;
    strcpy(hack2.reason, "手动封禁-多次违规");
    strcpy(hack2.hack_ip, TEST_IP_2);

    int ret = MemberPunishHack_Add(manager, &hack2);
    TEST_ASSERT(ret == 0, "测试5: 添加第二个黑客处罚记录");
}

void test_get_by_ip(DBConnectionManager* manager) {
    MemberPunishHack hacks[10];
    int count = MemberPunishHack_GetByIP(manager, TEST_IP_1, hacks, 10);
    TEST_ASSERT(count >= 1, "测试6: 按IP地址查询处罚记录");
}

void test_add_duplicate(DBConnectionManager* manager) {
    MemberPunishHack hack;
    MemberPunishHack_Init(&hack);

    hack.m_id = TEST_MEMBER_1;
    hack.occ_time = (unsigned int)time(NULL);
    hack.period = 259200; /* 3天 */
    hack.now_flag = HACK_FLAG_ACTIVE;
    hack.auto_flag = HACK_AUTO_SYSTEM;
    strcpy(hack.reason, "重复添加测试-应该覆盖原记录");
    strcpy(hack.hack_ip, TEST_IP_1);

    int ret = MemberPunishHack_Add(manager, &hack);
    TEST_ASSERT(ret == 0, "测试7: 添加重复记录（幂等性-REPLACE INTO）");
}

void test_update_now_flag(DBConnectionManager* manager) {
    int ret = MemberPunishHack_UpdateNowFlag(manager, TEST_MEMBER_2, HACK_FLAG_INACTIVE);
    TEST_ASSERT(ret == 0, "测试8: 更新当前状态标志");
}

void test_get_all(DBConnectionManager* manager) {
    MemberPunishHack hacks[10];
    int count = MemberPunishHack_GetAll(manager, hacks, 0, 10);
    TEST_ASSERT(count >= 2, "测试9: 获取所有记录（分页）");
}

void test_get_auto_flag(DBConnectionManager* manager) {
    MemberPunishHack hacks[10];
    int count = MemberPunishHack_GetAutoFlag(manager, hacks, 0, 10);
    TEST_ASSERT(count >= 1, "测试10: 获取系统自动处罚记录");
}

void test_get_manual_flag(DBConnectionManager* manager) {
    MemberPunishHack hacks[10];
    int count = MemberPunishHack_GetManualFlag(manager, hacks, 0, 10);
    TEST_ASSERT(count >= 1, "测试11: 获取手动处罚记录");
}

void test_update_hack(DBConnectionManager* manager) {
    MemberPunishHack hack;
    int ret = MemberPunishHack_Get(manager, TEST_MEMBER_1, &hack);
    if (ret == 0) {
        strcpy(hack.reason, "更新后的处罚原因");
        ret = MemberPunishHack_Update(manager, &hack);
        TEST_ASSERT(ret == 0, "测试12: 更新黑客处罚记录");
    } else {
        TEST_ASSERT(0, "测试12: 更新黑客处罚记录（获取失败）");
    }
}

void test_is_expired(DBConnectionManager* manager) {
    /* 添加一个已过期的记录 */
    MemberPunishHack hack_expired;
    MemberPunishHack_Init(&hack_expired);

    hack_expired.m_id = 200003;
    hack_expired.occ_time = (unsigned int)time(NULL) - 172800; /* 2天前 */
    hack_expired.period = 86400; /* 只封1天，所以已过期 */
    hack_expired.now_flag = HACK_FLAG_ACTIVE;
    hack_expired.auto_flag = HACK_AUTO_SYSTEM;
    strcpy(hack_expired.reason, "过期测试");
    strcpy(hack_expired.hack_ip, "192.168.100.99");

    MemberPunishHack_Add(manager, &hack_expired);

    bool expired = MemberPunishHack_IsExpired(manager, 200003);
    TEST_ASSERT(expired == true, "测试13: 检查处罚是否已过期");
}

void test_get_expired_records(DBConnectionManager* manager) {
    MemberPunishHack hacks[10];
    int count = MemberPunishHack_GetExpiredRecords(manager, hacks, 10);
    TEST_ASSERT(count >= 1, "测试14: 获取已过期的记录列表");
}

void test_update_expired_flags(DBConnectionManager* manager) {
    int updated = MemberPunishHack_UpdateExpiredFlags(manager);
    TEST_ASSERT(updated >= 0, "测试15: 批量更新过期记录的now_flag");
}

void test_delete_hack(DBConnectionManager* manager) {
    int ret = MemberPunishHack_Delete(manager, TEST_MEMBER_2);
    TEST_ASSERT(ret == 0, "测试16: 删除黑客处罚记录");
}

void test_delete_nonexistent(DBConnectionManager* manager) {
    int ret = MemberPunishHack_Delete(manager, TEST_MEMBER_2);
    TEST_ASSERT(ret == 0, "测试17: 删除不存在的记录（幂等性）");
}

void test_add_batch(DBConnectionManager* manager) {
    MemberPunishHack hacks[3];
    int i;

    for (i = 0; i < 3; i++) {
        MemberPunishHack_Init(&hacks[i]);
        hacks[i].m_id = 200010 + i;
        hacks[i].occ_time = (unsigned int)time(NULL);
        hacks[i].period = 86400 * (i + 1);
        hacks[i].now_flag = HACK_FLAG_ACTIVE;
        hacks[i].auto_flag = HACK_AUTO_SYSTEM;
        sprintf(hacks[i].reason, "批量添加测试 %d", i + 1);
        sprintf(hacks[i].hack_ip, "192.168.100.%d", 30 + i);
    }

    int success = MemberPunishHack_AddBatch(manager, hacks, 3);
    TEST_ASSERT(success == 3, "测试18: 批量添加黑客处罚记录");
}

void test_get_count(DBConnectionManager* manager) {
    int count = MemberPunishHack_GetCount(manager);
    TEST_ASSERT(count >= 3, "测试19: 获取处罚记录总数");
}

void test_utility_functions(DBConnectionManager* manager) {
    MemberPunishHack hack;
    MemberPunishHack_Init(&hack);

    hack.m_id = 200099;
    hack.occ_time = (unsigned int)time(NULL);
    hack.period = 3600;
    hack.now_flag = HACK_FLAG_ACTIVE;
    hack.auto_flag = HACK_AUTO_MANUAL;
    strcpy(hack.reason, "工具函数测试");
    strcpy(hack.hack_ip, "192.168.100.99");

    const char* now_flag_name = MemberPunishHack_GetNowFlagName(HACK_FLAG_ACTIVE);
    const char* auto_flag_name = MemberPunishHack_GetAutoFlagName(HACK_AUTO_MANUAL);
    unsigned int expire_time = MemberPunishHack_CalculateExpireTime(3600);

    printf("\n[工具函数测试结果]\n");
    printf("  当前状态标志名称: %s\n", now_flag_name);
    printf("  自动标志名称: %s\n", auto_flag_name);
    printf("  计算的过期时间: %u\n", expire_time);

    MemberPunishHack_Print(&hack);

    TEST_ASSERT(strlen(now_flag_name) > 0 && strlen(auto_flag_name) > 0, "测试20: 工具函数");
}

/* ========================================================================
 * 测试清理函数
 * ======================================================================== */

void cleanup_test_data(DBConnectionManager* manager) {
    printf("\n[开始清理测试数据]\n");
    MemberPunishHack_Delete(manager, TEST_MEMBER_1);
    MemberPunishHack_Delete(manager, TEST_MEMBER_2);
    MemberPunishHack_Delete(manager, 200003);
    MemberPunishHack_Delete(manager, 200010);
    MemberPunishHack_Delete(manager, 200011);
    MemberPunishHack_Delete(manager, 200012);
    MemberPunishHack_Delete(manager, 200099);
    printf("[测试数据清理完成]\n\n");
}

/* ========================================================================
 * 主测试函数
 * ======================================================================== */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("MemberPunishHack ORM 测试程序\n");
    printf("========================================\n\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        printf("[错误] 无法初始化数据库连接管理器\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        printf("[错误] 无法连接到数据库\n");
        return 1;
    }

    printf("[信息] 数据库连接成功\n\n");

    cleanup_test_data(&manager);

    printf("========================================\n");
    printf("开始执行测试用例\n");
    printf("========================================\n\n");

    test_add_hack(&manager);
    test_get_hack(&manager);
    test_exists(&manager);
    test_has_active_punish(&manager);
    test_add_multiple(&manager);
    test_get_by_ip(&manager);
    test_add_duplicate(&manager);
    test_update_now_flag(&manager);
    test_get_all(&manager);
    test_get_auto_flag(&manager);
    test_get_manual_flag(&manager);
    test_update_hack(&manager);
    test_is_expired(&manager);
    test_get_expired_records(&manager);
    test_update_expired_flags(&manager);
    test_delete_hack(&manager);
    test_delete_nonexistent(&manager);
    test_add_batch(&manager);
    test_get_count(&manager);
    test_utility_functions(&manager);

    cleanup_test_data(&manager);

    printf("\n========================================\n");
    printf("测试结果汇总\n");
    printf("========================================\n");
    printf("通过: %d\n", g_test_passed);
    printf("失败: %d\n", g_test_failed);
    printf("总计: %d\n", g_test_passed + g_test_failed);
    printf("成功率: %.1f%%\n", (float)g_test_passed / (g_test_passed + g_test_failed) * 100);
    printf("========================================\n");

    return (g_test_failed == 0) ? 0 : 1;
}
