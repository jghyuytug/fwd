#include "charac_members_orm.h"
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

#define ASSERT_TRUE(cond, message) \
    do { \
        assertion_count++; \
        if (cond) { \
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
        if (strcmp(a, b) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, a, b); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取角色成员");

    CharacMembers record, retrieved;
    memset(&record, 0, sizeof(CharacMembers));

    record.charac_no = 900001;
    record.master_no = 800001;
    record.exp = 1000;
    strcpy(record.create_time, "2025-11-16 10:00:00");
    strcpy(record.delete_time, "0000-00-00 00:00:00");

    ASSERT_EQ(CharacMembers_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacMembers_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.master_no, record.master_no, "主人编号匹配");
    ASSERT_EQ(retrieved.exp, record.exp, "经验值匹配");
    ASSERT_STR_EQ(retrieved.create_time, record.create_time, "创建时间匹配");
    ASSERT_STR_EQ(retrieved.delete_time, record.delete_time, "删除时间匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色成员信息");

    CharacMembers record, retrieved;
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &record), 0, "获取已存在记录");

    record.master_no = 800002;
    record.exp = 2000;
    strcpy(record.delete_time, "2025-12-31 23:59:59");

    ASSERT_EQ(CharacMembers_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.master_no, 800002, "主人编号已更新");
    ASSERT_EQ(retrieved.exp, 2000, "经验值已更新");
    ASSERT_STR_EQ(retrieved.delete_time, "2025-12-31 23:59:59", "删除时间已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查角色成员是否存在");

    ASSERT_EQ(CharacMembers_Exists(manager, 900001), 1, "记录存在");
    ASSERT_EQ(CharacMembers_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_master_no(DBConnectionManager* manager) {
    TEST_START("更新主人编号");

    CharacMembers retrieved;

    ASSERT_EQ(CharacMembers_UpdateMasterNo(manager, 900001, 800003), 0, "更新主人编号成功");
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.master_no, 800003, "主人编号已更新为800003");
}

void test_add_exp(DBConnectionManager* manager) {
    TEST_START("增加经验值");

    CharacMembers retrieved;
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取当前记录");
    int old_exp = retrieved.exp;

    ASSERT_EQ(CharacMembers_AddExp(manager, 900001, 500), 0, "增加经验值成功");
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.exp, old_exp + 500, "经验值增加了500");
}

void test_update_exp(DBConnectionManager* manager) {
    TEST_START("更新经验值");

    CharacMembers retrieved;

    ASSERT_EQ(CharacMembers_UpdateExp(manager, 900001, 5000), 0, "更新经验值成功");
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.exp, 5000, "经验值已更新为5000");
}

void test_update_delete_time(DBConnectionManager* manager) {
    TEST_START("更新删除时间");

    CharacMembers retrieved;

    ASSERT_EQ(CharacMembers_UpdateDeleteTime(manager, 900001, "2026-01-01 00:00:00"), 0, "更新删除时间成功");
    ASSERT_EQ(CharacMembers_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_STR_EQ(retrieved.delete_time, "2026-01-01 00:00:00", "删除时间已更新");
}

void test_get_by_master_no(DBConnectionManager* manager) {
    TEST_START("获取指定主人的所有成员");

    /* 添加多个成员给同一主人 */
    CharacMembers record2, record3;
    memset(&record2, 0, sizeof(CharacMembers));
    memset(&record3, 0, sizeof(CharacMembers));

    record2.charac_no = 900002;
    record2.master_no = 800003;
    record2.exp = 100;
    strcpy(record2.create_time, "2025-11-16 11:00:00");
    strcpy(record2.delete_time, "0000-00-00 00:00:00");

    record3.charac_no = 900003;
    record3.master_no = 800003;
    record3.exp = 200;
    strcpy(record3.create_time, "2025-11-16 12:00:00");
    strcpy(record3.delete_time, "0000-00-00 00:00:00");

    CharacMembers_Add(manager, &record2);
    CharacMembers_Add(manager, &record3);

    int charac_nos[10];
    int count = CharacMembers_GetByMasterNo(manager, 800003, charac_nos, 10);

    ASSERT_TRUE(count >= 3, "至少找到3个成员（900001, 900002, 900003）");
}

void test_get_count_by_master_no(DBConnectionManager* manager) {
    TEST_START("获取主人的成员数量");

    int count = CharacMembers_GetCountByMasterNo(manager, 800003);
    ASSERT_TRUE(count >= 3, "主人800003至少有3个成员");
}

void test_get_total_exp(DBConnectionManager* manager) {
    TEST_START("获取总经验值");

    long long total = CharacMembers_GetTotalExp(manager);
    ASSERT_TRUE(total >= 5300, "总经验值至少为5300（5000+100+200）");
}

void test_get_active_members_count(DBConnectionManager* manager) {
    TEST_START("获取活跃成员数量");

    /* 重置900002和900003的删除时间为未删除 */
    CharacMembers_UpdateDeleteTime(manager, 900002, "0000-00-00 00:00:00");
    CharacMembers_UpdateDeleteTime(manager, 900003, "0000-00-00 00:00:00");

    int count = CharacMembers_GetActiveMembersCount(manager);
    ASSERT_TRUE(count >= 2, "至少有2个活跃成员（900002, 900003）");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除角色成员");

    ASSERT_EQ(CharacMembers_Delete(manager, 900001), 0, "删除记录成功");
    ASSERT_EQ(CharacMembers_Exists(manager, 900001), 0, "记录已删除");

    ASSERT_EQ(CharacMembers_Delete(manager, 900002), 0, "删除记录成功");
    ASSERT_EQ(CharacMembers_Delete(manager, 900003), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色成员信息");

    CharacMembers record;
    memset(&record, 0, sizeof(CharacMembers));
    record.charac_no = 900004;
    record.master_no = 800004;
    record.exp = 3000;
    strcpy(record.create_time, "2025-11-16 13:00:00");
    strcpy(record.delete_time, "0000-00-00 00:00:00");

    CharacMembers_Add(manager, &record);

    printf("\n");
    CharacMembers_PrintInfo(&record);

    CharacMembers_Delete(manager, 900004);

    assertion_count++;
    assertion_passed++;
    printf("  ✓ 打印信息成功\n");
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "连接到 taiwan_cain 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("角色成员 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_master_no(&manager);
    test_add_exp(&manager);
    test_update_exp(&manager);
    test_update_delete_time(&manager);
    test_get_by_master_no(&manager);
    test_get_count_by_master_no(&manager);
    test_get_total_exp(&manager);
    test_get_active_members_count(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
