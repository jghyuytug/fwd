/**
 * DNF游戏服务器 - MemberPunishInfo表ORM测试程序
 * 测试处罚信息系统的20个功能点
 */

#include "member_punish_info_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_MEMBER_1 100001
#define TEST_MEMBER_2 100002
#define TEST_ADMIN_1 "admin001"
#define TEST_ADMIN_2 "admin002"

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

void test_add_punish(DBConnectionManager* manager) {
    MemberPunishInfo punish;
    MemberPunishInfo_Init(&punish);

    punish.m_id = TEST_MEMBER_1;
    punish.punish_type = PUNISH_TYPE_CHAT_BAN;
    MemberPunishInfo_GetTimeString(punish.occ_time, sizeof(punish.occ_time));
    punish.punish_value = 0;
    punish.apply_flag = PUNISH_STATUS_ACTIVE;
    MemberPunishInfo_GetTimeString(punish.start_time, sizeof(punish.start_time));
    MemberPunishInfo_GetExpireTime(punish.end_time, sizeof(punish.end_time), 86400); /* 1天 */
    strcpy(punish.admin_id, TEST_ADMIN_1);
    strcpy(punish.reason, "违规聊天内容");
    strcpy(punish.ip, "192.168.1.100");
    strcpy(punish.mac, "00:11:22:33:44:55");

    int ret = MemberPunishInfo_Add(manager, &punish);
    TEST_ASSERT(ret == 0, "测试1: 添加处罚记录");
}

void test_get_punish(DBConnectionManager* manager) {
    MemberPunishInfo punish;
    int ret = MemberPunishInfo_Get(manager, TEST_MEMBER_1, PUNISH_TYPE_CHAT_BAN, &punish);
    TEST_ASSERT(ret == 0 && punish.m_id == TEST_MEMBER_1, "测试2: 获取处罚记录");
}

void test_get_count(DBConnectionManager* manager) {
    int count = MemberPunishInfo_GetCount(manager, TEST_MEMBER_1);
    TEST_ASSERT(count >= 1, "测试3: 获取处罚记录总数");
}

void test_has_active_punish(DBConnectionManager* manager) {
    bool has_active = MemberPunishInfo_HasActivePunish(manager, TEST_MEMBER_1, PUNISH_TYPE_CHAT_BAN);
    TEST_ASSERT(has_active == true, "测试4: 检查是否有指定类型的生效处罚");
}

void test_add_multiple_punishes(DBConnectionManager* manager) {
    MemberPunishInfo punish1, punish2;

    MemberPunishInfo_Init(&punish1);
    punish1.m_id = TEST_MEMBER_1;
    punish1.punish_type = PUNISH_TYPE_TRADE_BAN;
    MemberPunishInfo_GetTimeString(punish1.occ_time, sizeof(punish1.occ_time));
    punish1.apply_flag = PUNISH_STATUS_ACTIVE;
    MemberPunishInfo_GetTimeString(punish1.start_time, sizeof(punish1.start_time));
    MemberPunishInfo_GetExpireTime(punish1.end_time, sizeof(punish1.end_time), 172800); /* 2天 */
    strcpy(punish1.admin_id, TEST_ADMIN_1);
    strcpy(punish1.reason, "违规交易");

    MemberPunishInfo_Init(&punish2);
    punish2.m_id = TEST_MEMBER_2;
    punish2.punish_type = PUNISH_TYPE_WARNING;
    MemberPunishInfo_GetTimeString(punish2.occ_time, sizeof(punish2.occ_time));
    punish2.apply_flag = PUNISH_STATUS_ACTIVE;
    MemberPunishInfo_GetTimeString(punish2.start_time, sizeof(punish2.start_time));
    MemberPunishInfo_GetExpireTime(punish2.end_time, sizeof(punish2.end_time), 86400);
    strcpy(punish2.admin_id, TEST_ADMIN_2);
    strcpy(punish2.reason, "首次警告");

    int ret1 = MemberPunishInfo_Add(manager, &punish1);
    int ret2 = MemberPunishInfo_Add(manager, &punish2);
    TEST_ASSERT(ret1 == 0 && ret2 == 0, "测试5: 添加多个处罚记录");
}

void test_get_by_member(DBConnectionManager* manager) {
    MemberPunishInfo punishes[10];
    int count = MemberPunishInfo_GetByMember(manager, TEST_MEMBER_1, punishes, 10);
    TEST_ASSERT(count >= 2, "测试6: 获取指定会员的所有处罚");
}

void test_add_duplicate(DBConnectionManager* manager) {
    MemberPunishInfo punish;
    MemberPunishInfo_Init(&punish);

    punish.m_id = TEST_MEMBER_1;
    punish.punish_type = PUNISH_TYPE_CHAT_BAN;
    MemberPunishInfo_GetTimeString(punish.occ_time, sizeof(punish.occ_time));
    punish.apply_flag = PUNISH_STATUS_ACTIVE;
    MemberPunishInfo_GetTimeString(punish.start_time, sizeof(punish.start_time));
    MemberPunishInfo_GetExpireTime(punish.end_time, sizeof(punish.end_time), 86400);
    strcpy(punish.admin_id, TEST_ADMIN_1);
    strcpy(punish.reason, "重复添加测试");

    int ret = MemberPunishInfo_Add(manager, &punish);
    TEST_ASSERT(ret == 0, "测试7: 添加重复记录（幂等性）");
}

void test_update_status(DBConnectionManager* manager) {
    int ret = MemberPunishInfo_UpdateStatus(manager, TEST_MEMBER_2, PUNISH_TYPE_WARNING, PUNISH_STATUS_PENDING);
    TEST_ASSERT(ret == 0, "测试8: 更新处罚状态");
}

void test_lift_punish(DBConnectionManager* manager) {
    int ret = MemberPunishInfo_Lift(manager, TEST_MEMBER_1, PUNISH_TYPE_TRADE_BAN);
    TEST_ASSERT(ret == 0, "测试9: 解除处罚");
}

void test_has_any_active_punish(DBConnectionManager* manager) {
    bool has_any = MemberPunishInfo_HasAnyActivePunish(manager, TEST_MEMBER_1);
    TEST_ASSERT(has_any == true, "测试10: 检查是否有任何生效处罚");
}

void test_get_active_list(DBConnectionManager* manager) {
    MemberPunishInfo punishes[10];
    int count = MemberPunishInfo_GetActive(manager, TEST_MEMBER_1, punishes, 10);
    TEST_ASSERT(count >= 1, "测试11: 获取会员的生效处罚列表");
}

void test_update_punish(DBConnectionManager* manager) {
    MemberPunishInfo punish;
    int ret = MemberPunishInfo_Get(manager, TEST_MEMBER_1, PUNISH_TYPE_CHAT_BAN, &punish);
    if (ret == 0) {
        strcpy(punish.reason, "更新后的原因");
        ret = MemberPunishInfo_Update(manager, &punish);
        TEST_ASSERT(ret == 0, "测试12: 更新处罚记录");
    } else {
        TEST_ASSERT(0, "测试12: 更新处罚记录（获取失败）");
    }
}

void test_get_by_admin(DBConnectionManager* manager) {
    MemberPunishInfo punishes[20];
    int count = MemberPunishInfo_GetByAdmin(manager, TEST_ADMIN_1, punishes, 20);
    TEST_ASSERT(count >= 2, "测试13: 按管理员ID查询处罚记录");
}

void test_delete_punish(DBConnectionManager* manager) {
    int ret = MemberPunishInfo_Delete(manager, TEST_MEMBER_2, PUNISH_TYPE_WARNING);
    TEST_ASSERT(ret == 0, "测试14: 删除处罚记录");
}

void test_delete_nonexistent(DBConnectionManager* manager) {
    int ret = MemberPunishInfo_Delete(manager, TEST_MEMBER_2, PUNISH_TYPE_WARNING);
    TEST_ASSERT(ret == 0, "测试15: 删除不存在的记录（幂等性）");
}

void test_add_batch(DBConnectionManager* manager) {
    MemberPunishInfo punishes[3];
    int i;

    for (i = 0; i < 3; i++) {
        MemberPunishInfo_Init(&punishes[i]);
        punishes[i].m_id = TEST_MEMBER_2;
        punishes[i].punish_type = PUNISH_TYPE_CHAT_BAN + i;
        MemberPunishInfo_GetTimeString(punishes[i].occ_time, sizeof(punishes[i].occ_time));
        punishes[i].apply_flag = PUNISH_STATUS_ACTIVE;
        MemberPunishInfo_GetTimeString(punishes[i].start_time, sizeof(punishes[i].start_time));
        MemberPunishInfo_GetExpireTime(punishes[i].end_time, sizeof(punishes[i].end_time), 86400);
        sprintf(punishes[i].admin_id, "%s", TEST_ADMIN_2);
        sprintf(punishes[i].reason, "批量添加测试 %d", i + 1);
    }

    int success = MemberPunishInfo_AddBatch(manager, punishes, 3);
    TEST_ASSERT(success >= 3, "测试16: 批量添加处罚记录");
}

void test_delete_all(DBConnectionManager* manager) {
    int ret = MemberPunishInfo_DeleteAll(manager, TEST_MEMBER_1);
    TEST_ASSERT(ret == 0, "测试17: 删除指定会员的所有处罚");
}

void test_get_empty_list(DBConnectionManager* manager) {
    MemberPunishInfo punishes[10];
    int count = MemberPunishInfo_GetByMember(manager, TEST_MEMBER_1, punishes, 10);
    TEST_ASSERT(count == 0, "测试18: 获取空列表");
}

void test_check_nonexistent(DBConnectionManager* manager) {
    bool has_active = MemberPunishInfo_HasActivePunish(manager, TEST_MEMBER_1, PUNISH_TYPE_CHAT_BAN);
    TEST_ASSERT(has_active == false, "测试19: 检查不存在的处罚");
}

void test_utility_functions(DBConnectionManager* manager) {
    MemberPunishInfo punish;
    MemberPunishInfo_Init(&punish);

    char time_str[20];
    MemberPunishInfo_GetTimeString(time_str, sizeof(time_str));

    char expire_str[20];
    MemberPunishInfo_GetExpireTime(expire_str, sizeof(expire_str), 3600);

    punish.m_id = TEST_MEMBER_2;
    punish.punish_type = PUNISH_TYPE_WARNING;
    strcpy(punish.occ_time, time_str);
    punish.apply_flag = PUNISH_STATUS_ACTIVE;
    strcpy(punish.start_time, time_str);
    strcpy(punish.end_time, expire_str);
    strcpy(punish.admin_id, TEST_ADMIN_2);
    strcpy(punish.reason, "工具函数测试");

    const char* type_name = MemberPunishInfo_GetTypeName(PUNISH_TYPE_WARNING);
    const char* status_name = MemberPunishInfo_GetStatusName(PUNISH_STATUS_ACTIVE);

    printf("\n[工具函数测试结果]\n");
    printf("  当前时间: %s\n", time_str);
    printf("  过期时间: %s\n", expire_str);
    printf("  处罚类型名称: %s\n", type_name);
    printf("  处罚状态名称: %s\n", status_name);

    MemberPunishInfo_Print(&punish);

    TEST_ASSERT(strlen(time_str) > 0 && strlen(expire_str) > 0, "测试20: 工具函数");
}

/* ========================================================================
 * 测试清理函数
 * ======================================================================== */

void cleanup_test_data(DBConnectionManager* manager) {
    printf("\n[开始清理测试数据]\n");
    MemberPunishInfo_DeleteAll(manager, TEST_MEMBER_1);
    MemberPunishInfo_DeleteAll(manager, TEST_MEMBER_2);
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
    printf("MemberPunishInfo ORM 测试程序\n");
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

    test_add_punish(&manager);
    test_get_punish(&manager);
    test_get_count(&manager);
    test_has_active_punish(&manager);
    test_add_multiple_punishes(&manager);
    test_get_by_member(&manager);
    test_add_duplicate(&manager);
    test_update_status(&manager);
    test_lift_punish(&manager);
    test_has_any_active_punish(&manager);
    test_get_active_list(&manager);
    test_update_punish(&manager);
    test_get_by_admin(&manager);
    test_delete_punish(&manager);
    test_delete_nonexistent(&manager);
    test_add_batch(&manager);
    test_delete_all(&manager);
    test_get_empty_list(&manager);
    test_check_nonexistent(&manager);
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
