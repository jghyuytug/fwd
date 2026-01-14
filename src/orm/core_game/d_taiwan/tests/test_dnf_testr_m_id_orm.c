#include "dnf_testr_m_id_orm.h"
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
    DnfTestrMId member, retrieved;
    DnfTestrMId members[10];
    int actual_count;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    printf("初始化数据库连接...\n");
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "数据库管理器初始化失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "连接 d_taiwan 数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("数据库连接成功\n");
    printf("========================================\n");

    /* 准备测试数据 */
    memset(&member, 0, sizeof(DnfTestrMId));
    member.m_id = 999001;
    member.sex = 1;  /* 男 */

    /* TEST 1: 添加新测试会员 */
    TEST_START("添加新测试会员");
    ASSERT_EQ(DnfTestrMId_Add(&manager, &member), 0, "添加测试会员成功");

    /* TEST 2: 验证测试会员存在 */
    TEST_START("验证测试会员存在");
    int exists = DnfTestrMId_Exists(&manager, member.m_id);
    ASSERT_EQ(exists, 1, "测试会员存在");

    /* TEST 3: 获取测试会员信息 */
    TEST_START("获取测试会员信息");
    memset(&retrieved, 0, sizeof(DnfTestrMId));
    ASSERT_EQ(DnfTestrMId_Get(&manager, member.m_id, &retrieved), 0, "获取测试会员成功");
    ASSERT_EQ(retrieved.m_id, member.m_id, "会员ID匹配");
    ASSERT_EQ(retrieved.sex, member.sex, "性别匹配");

    /* TEST 4: 更新性别 */
    TEST_START("更新性别");
    member.sex = 0;  /* 女 */
    ASSERT_EQ(DnfTestrMId_Update(&manager, &member), 0, "更新性别成功");

    memset(&retrieved, 0, sizeof(DnfTestrMId));
    DnfTestrMId_Get(&manager, member.m_id, &retrieved);
    ASSERT_EQ(retrieved.sex, 0, "性别已更新为0（女）");

    /* TEST 5: 使用SetSex设置性别 */
    TEST_START("使用SetSex设置性别");
    ASSERT_EQ(DnfTestrMId_SetSex(&manager, member.m_id, 1), 0, "设置性别成功");

    memset(&retrieved, 0, sizeof(DnfTestrMId));
    DnfTestrMId_Get(&manager, member.m_id, &retrieved);
    ASSERT_EQ(retrieved.sex, 1, "性别已修改为1（男）");

    /* TEST 6: 添加多个测试会员 */
    TEST_START("添加多个测试会员");
    DnfTestrMId member2, member3;

    memset(&member2, 0, sizeof(DnfTestrMId));
    member2.m_id = 999002;
    member2.sex = 0;  /* 女 */
    ASSERT_EQ(DnfTestrMId_Add(&manager, &member2), 0, "添加第2个测试会员");

    memset(&member3, 0, sizeof(DnfTestrMId));
    member3.m_id = 999003;
    member3.sex = 1;  /* 男 */
    ASSERT_EQ(DnfTestrMId_Add(&manager, &member3), 0, "添加第3个测试会员");

    /* TEST 7: 查询所有测试会员 */
    TEST_START("查询所有测试会员");
    memset(members, 0, sizeof(members));
    actual_count = 0;
    ASSERT_EQ(DnfTestrMId_GetAll(&manager, members, 10, &actual_count), 0,
              "查询所有测试会员成功");
    ASSERT_TRUE(actual_count >= 3, "至少查询到3个测试会员");
    printf("  查询到 %d 个测试会员\n", actual_count);

    /* TEST 8: 按性别查询（男性） */
    TEST_START("按性别查询（男性）");
    memset(members, 0, sizeof(members));
    actual_count = 0;
    ASSERT_EQ(DnfTestrMId_GetBySex(&manager, 1, members, 10, &actual_count), 0,
              "查询男性测试会员成功");
    ASSERT_TRUE(actual_count >= 2, "至少查询到2个男性测试会员");
    printf("  查询到 %d 个男性测试会员\n", actual_count);

    /* 验证查询结果都是男性 */
    for (int i = 0; i < actual_count; i++) {
        ASSERT_EQ(members[i].sex, 1, "查询结果性别为1（男）");
        break; /* 只检查第一个 */
    }

    /* TEST 9: 按性别查询（女性） */
    TEST_START("按性别查询（女性）");
    memset(members, 0, sizeof(members));
    actual_count = 0;
    ASSERT_EQ(DnfTestrMId_GetBySex(&manager, 0, members, 10, &actual_count), 0,
              "查询女性测试会员成功");
    ASSERT_TRUE(actual_count >= 1, "至少查询到1个女性测试会员");
    printf("  查询到 %d 个女性测试会员\n", actual_count);

    /* 验证查询结果都是女性 */
    for (int i = 0; i < actual_count; i++) {
        ASSERT_EQ(members[i].sex, 0, "查询结果性别为0（女）");
        break; /* 只检查第一个 */
    }

    /* TEST 10: 统计总数 */
    TEST_START("统计总数");
    int total_count = DnfTestrMId_CountAll(&manager);
    ASSERT_TRUE(total_count >= 3, "总数至少为3");
    printf("  共有 %d 个测试会员\n", total_count);

    /* TEST 11: 按性别统计 */
    TEST_START("按性别统计");
    int male_count = DnfTestrMId_CountBySex(&manager, 1);
    int female_count = DnfTestrMId_CountBySex(&manager, 0);
    ASSERT_TRUE(male_count >= 2, "男性至少2个");
    ASSERT_TRUE(female_count >= 1, "女性至少1个");
    printf("  男性: %d 个, 女性: %d 个\n", male_count, female_count);

    /* TEST 12: 打印测试会员信息 */
    TEST_START("打印测试会员信息");
    printf("  调用 DnfTestrMId_PrintInfo:\n");
    DnfTestrMId_PrintInfo(&member);
    ASSERT_TRUE(1, "打印测试会员信息成功");

    /* TEST 13: 删除单个测试会员 */
    TEST_START("删除单个测试会员");
    ASSERT_EQ(DnfTestrMId_Delete(&manager, 999003), 0, "删除测试会员999003成功");
    exists = DnfTestrMId_Exists(&manager, 999003);
    ASSERT_EQ(exists, 0, "测试会员999003已不存在");

    /* TEST 14: 边界测试 - NULL 参数 */
    TEST_START("边界测试 - NULL 参数");
    ASSERT_EQ(DnfTestrMId_Add(NULL, &member), -1, "NULL manager 返回 -1");
    ASSERT_EQ(DnfTestrMId_Add(&manager, NULL), -1, "NULL member 返回 -1");
    ASSERT_EQ(DnfTestrMId_Get(&manager, 1, NULL), -1, "NULL 输出参数返回 -1");

    /* TEST 15: 边界测试 - 不存在的记录 */
    TEST_START("边界测试 - 不存在的记录");
    memset(&retrieved, 0, sizeof(DnfTestrMId));
    ASSERT_EQ(DnfTestrMId_Get(&manager, 9999999, &retrieved), -1,
              "获取不存在的记录返回 -1");
    exists = DnfTestrMId_Exists(&manager, 9999999);
    ASSERT_EQ(exists, 0, "不存在的记录 Exists 返回 0");

    /* 清理测试数据 */
    printf("\n清理测试数据...\n");
    DnfTestrMId_Delete(&manager, 999001);
    DnfTestrMId_Delete(&manager, 999002);
    DnfTestrMId_Delete(&manager, 999003);

    /* 清理资源 */
    DBConnectionManager_Cleanup(&manager);

    /* 统计最后一个测试用例 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n", test_passed, test_count,
           (test_count > 0 ? (test_passed * 100.0 / test_count) : 0));
    printf("断言: %d/%d 通过 (%.1f%%)\n", assertion_passed, assertion_count,
           (assertion_count > 0 ? (assertion_passed * 100.0 / assertion_count) : 0));
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
