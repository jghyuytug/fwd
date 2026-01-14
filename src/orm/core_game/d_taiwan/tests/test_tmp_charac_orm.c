#include "tmp_charac_orm.h"
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
    TmpCharac record, retrieved;
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
    TmpCharac_Clear(&manager);

    /* 测试 1: 添加临时角色 */
    TEST_START("Add - 添加临时角色记录");
    memset(&record, 0, sizeof(TmpCharac));
    record.m_id = 1001;
    record.charac_no = 5001;
    strcpy(record.charac_name, "TestChar");
    record.village = 1;
    record.job = 0;
    record.lev = 10;
    record.exp = 5000;
    record.grow_type = 0;
    record.HP = 50;
    record.maxHP = 500;
    record.maxMP = 300;
    record.phy_attack = 100;
    record.phy_defense = 80;
    record.mag_attack = 90;
    record.mag_defense = 75;
    record.inven_weight = 1000;
    record.hp_regen = 10;
    record.mp_regen = 15;
    record.move_speed = 200;
    record.attack_speed = 150;
    record.cast_speed = 140;
    record.hit_recovery = 100;
    record.jump = 180;
    record.charac_weight = 5000;
    record.fatigue = 50;
    record.max_fatigue = 156;
    record.premium_fatigue = 0;
    record.max_premium_fatigue = 0;
    strcpy(record.create_time, "2025-11-16 10:00:00");
    strcpy(record.last_play_time, "2025-11-16 12:00:00");
    record.dungeon_clear_point = 100;
    strcpy(record.delete_time, "0000-00-00 00:00:00");
    record.delete_flag = 0;
    record.guild_id = 0;
    record.guild_right = 0;
    record.member_flag = 0;

    ret = TmpCharac_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加记录成功");

    /* 测试 2: 检查记录是否存在 */
    TEST_START("Exists - 检查记录是否存在");
    ret = TmpCharac_Exists(&manager, 1001, 5001);
    ASSERT_EQ(ret, 1, "记录存在");

    ret = TmpCharac_Exists(&manager, 9999, 9999);
    ASSERT_EQ(ret, 0, "不存在的记录");

    /* 测试 3: 获取临时角色 */
    TEST_START("Get - 获取临时角色记录");
    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5001, &retrieved);
    ASSERT_EQ(ret, 0, "获取数据成功");
    ASSERT_EQ(retrieved.m_id, 1001, "会员ID正确");
    ASSERT_EQ(retrieved.charac_no, 5001, "角色编号正确");
    ASSERT_STR_EQ(retrieved.charac_name, "TestChar", "角色名称正确");
    ASSERT_EQ(retrieved.lev, 10, "等级正确");
    ASSERT_EQ(retrieved.exp, 5000, "经验值正确");
    ASSERT_EQ(retrieved.job, 0, "职业正确");
    ASSERT_EQ(retrieved.maxHP, 500, "最大HP正确");

    TmpCharac_PrintInfo(&retrieved);

    /* 测试 4: 更新记录 */
    TEST_START("Update - 更新临时角色记录");
    record.lev = 20;
    record.exp = 15000;
    record.maxHP = 800;
    ret = TmpCharac_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "更新成功");

    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5001, &retrieved);
    ASSERT_EQ(retrieved.lev, 20, "更新后等级正确");
    ASSERT_EQ(retrieved.exp, 15000, "更新后经验值正确");
    ASSERT_EQ(retrieved.maxHP, 800, "更新后最大HP正确");

    /* 测试 5: UpdateLevel - 直接更新等级和经验 */
    TEST_START("UpdateLevel - 直接更新等级和经验");
    ret = TmpCharac_UpdateLevel(&manager, 1001, 5001, 30, 30000);
    ASSERT_EQ(ret, 0, "更新等级成功");

    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5001, &retrieved);
    ASSERT_EQ(retrieved.lev, 30, "等级已更新为30");
    ASSERT_EQ(retrieved.exp, 30000, "经验已更新为30000");

    /* 测试 6: UpdateFatigue - 更新疲劳度 */
    TEST_START("UpdateFatigue - 更新疲劳度");
    ret = TmpCharac_UpdateFatigue(&manager, 1001, 5001, 100, 50);
    ASSERT_EQ(ret, 0, "更新疲劳度成功");

    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5001, &retrieved);
    ASSERT_EQ(retrieved.fatigue, 100, "疲劳度已更新");
    ASSERT_EQ(retrieved.premium_fatigue, 50, "高级疲劳度已更新");

    /* 测试 7: UpdateGuild - 更新公会信息 */
    TEST_START("UpdateGuild - 更新公会信息");
    ret = TmpCharac_UpdateGuild(&manager, 1001, 5001, 888, 2);
    ASSERT_EQ(ret, 0, "更新公会信息成功");

    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5001, &retrieved);
    ASSERT_EQ(retrieved.guild_id, 888, "公会ID已更新");
    ASSERT_EQ(retrieved.guild_right, 2, "公会权限已更新");

    /* 测试 8: 批量添加测试数据 */
    TEST_START("Batch Add - 批量添加角色");
    TmpCharac chars[3];
    for (int i = 0; i < 3; i++) {
        memset(&chars[i], 0, sizeof(TmpCharac));
        chars[i].m_id = 1001;
        chars[i].charac_no = 5002 + i;
        sprintf(chars[i].charac_name, "Char%d", i + 2);
        chars[i].village = 1;
        chars[i].job = i;
        chars[i].lev = 5 + i;
        chars[i].exp = 1000 * (i + 1);
        chars[i].maxHP = 300 + i * 50;
        chars[i].maxMP = 200 + i * 30;
        chars[i].max_fatigue = 156;
        strcpy(chars[i].create_time, "2025-11-16 10:00:00");
        strcpy(chars[i].last_play_time, "2025-11-16 12:00:00");
        strcpy(chars[i].delete_time, "0000-00-00 00:00:00");

        ret = TmpCharac_Add(&manager, &chars[i]);
        ASSERT_EQ(ret, 0, "批量添加角色成功");
    }

    count = TmpCharac_Count(&manager);
    ASSERT_EQ(count, 4, "总记录数为4");

    /* 测试 9: MarkDeleted - 标记删除 */
    TEST_START("MarkDeleted - 标记角色为已删除");
    ret = TmpCharac_MarkDeleted(&manager, 1001, 5002);
    ASSERT_EQ(ret, 0, "标记删除成功");

    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 1001, 5002, &retrieved);
    ASSERT_EQ(retrieved.delete_flag, 1, "删除标记已设置");

    /* 测试 10: Delete - 删除单条记录 */
    TEST_START("Delete - 删除单条记录");
    ret = TmpCharac_Delete(&manager, 1001, 5003);
    ASSERT_EQ(ret, 0, "删除成功");

    ret = TmpCharac_Exists(&manager, 1001, 5003);
    ASSERT_EQ(ret, 0, "记录已删除");

    /* 测试 11: DeleteByMid - 按会员ID删除所有角色 */
    TEST_START("DeleteByMid - 按会员ID删除");
    ret = TmpCharac_DeleteByMid(&manager, 1001);
    ASSERT_EQ(ret, 0, "删除成功");

    count = TmpCharac_Count(&manager);
    ASSERT_EQ(count, 0, "所有角色已删除");

    /* 测试 12: Clear - 清空所有数据 */
    TEST_START("Clear - 清空所有数据");

    /* 先添加一条测试数据 */
    memset(&record, 0, sizeof(TmpCharac));
    record.m_id = 2001;
    record.charac_no = 6001;
    strcpy(record.charac_name, "ClearTest");
    record.lev = 1;
    strcpy(record.create_time, "2025-11-16 10:00:00");
    strcpy(record.last_play_time, "2025-11-16 12:00:00");
    strcpy(record.delete_time, "0000-00-00 00:00:00");
    TmpCharac_Add(&manager, &record);

    ret = TmpCharac_Clear(&manager);
    ASSERT_EQ(ret, 0, "清空成功");

    count = TmpCharac_Count(&manager);
    ASSERT_EQ(count, 0, "所有记录已清空");

    /* 测试 13: 获取不存在的记录 */
    TEST_START("Get Non-Existent - 获取不存在的记录");
    memset(&retrieved, 0, sizeof(TmpCharac));
    ret = TmpCharac_Get(&manager, 9999, 9999, &retrieved);
    ASSERT_NEQ(ret, 0, "获取不存在的记录应失败");

    /* 清理 */
    TmpCharac_Clear(&manager);
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
