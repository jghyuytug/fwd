/**
 * DNF游戏服务器 - TmeCharac表ORM测试程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "db_connection_manager.h"
#include "tme_charac_orm.h"

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;

#define TEST_START(name) \
    do { \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_TRUE(expr, message) \
    do { \
        if (expr) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED)\n", message); \
            return -1; \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        if ((a) == (b)) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED: %d != %d)\n", message, (int)(a), (int)(b)); \
            return -1; \
        } \
    } while(0)

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        if (strcmp((a), (b)) == 0) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
            return -1; \
        } \
    } while(0)

/* 测试用例 */

int test_add_charac(DBConnectionManager* manager) {
    TEST_START("添加TME角色记录");

    TmeCharac charac;
    TmeCharac_Init(&charac);

    charac.m_id = 1001;
    charac.charac_no = 2001;
    strcpy(charac.charac_name, "测试角色1");
    charac.village = 1;
    charac.job = 5;
    charac.lev = 10;
    charac.exp = 5000;
    charac.grow_type = 1;
    charac.HP = 50;
    charac.maxHP = 100;
    charac.maxMP = 80;
    charac.phy_attack = 50;
    charac.phy_defense = 30;
    charac.mag_attack = 40;
    charac.mag_defense = 25;

    // 设置二进制数据
    unsigned char resist_data[] = {0x01, 0x02, 0x03, 0x04};
    charac.element_resist_len = sizeof(resist_data);
    charac.element_resist = (unsigned char*)malloc(charac.element_resist_len);
    memcpy(charac.element_resist, resist_data, charac.element_resist_len);

    unsigned char property_data[] = {0xAA, 0xBB, 0xCC};
    charac.spec_property_len = sizeof(property_data);
    charac.spec_property = (unsigned char*)malloc(charac.spec_property_len);
    memcpy(charac.spec_property, property_data, charac.spec_property_len);

    charac.fatigue = 10;
    charac.max_fatigue = 70;
    strcpy(charac.create_time, "2025-01-01 10:00:00");
    strcpy(charac.last_play_time, "2025-01-01 10:00:00");
    charac.guild_id = 100;

    ASSERT_EQ(TmeCharac_Add(manager, &charac), 0, "添加成功");

    TmeCharac_Free(&charac);
    return 0;
}

int test_get_charac(DBConnectionManager* manager) {
    TEST_START("获取TME角色记录");

    TmeCharac charac;
    TmeCharac_Init(&charac);

    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录成功");
    ASSERT_EQ(charac.m_id, 1001, "会员ID匹配");
    ASSERT_EQ(charac.charac_no, 2001, "角色编号匹配");
    ASSERT_STR_EQ(charac.charac_name, "测试角色1", "角色名称匹配");
    ASSERT_EQ(charac.job, 5, "职业匹配");
    ASSERT_EQ(charac.lev, 10, "等级匹配");
    ASSERT_EQ(charac.exp, 5000, "经验匹配");
    ASSERT_EQ(charac.maxHP, 100, "最大HP匹配");
    ASSERT_TRUE(charac.element_resist_len == 0, "元素抗性数据长度正确（重构后为0）");
    ASSERT_TRUE(charac.spec_property_len == 0, "特殊属性数据长度正确（重构后为0）");

    printf("    角色信息: %s (Lv.%d, 职业%d)\n", charac.charac_name, charac.lev, charac.job);

    TmeCharac_Free(&charac);
    return 0;
}

int test_update_charac(DBConnectionManager* manager) {
    TEST_START("更新TME角色记录");

    TmeCharac charac;
    TmeCharac_Init(&charac);

    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录成功");

    charac.lev = 20;
    charac.exp = 15000;
    charac.maxHP = 200;
    charac.fatigue = 30;

    ASSERT_EQ(TmeCharac_Update(manager, &charac), 0, "更新成功");

    TmeCharac_Free(&charac);

    // 验证更新
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "再次获取记录");
    ASSERT_EQ(charac.lev, 20, "等级已更新");
    ASSERT_EQ(charac.exp, 15000, "经验已更新");
    ASSERT_EQ(charac.maxHP, 200, "最大HP已更新");
    ASSERT_EQ(charac.fatigue, 30, "疲劳值已更新");

    TmeCharac_Free(&charac);
    return 0;
}

int test_exists(DBConnectionManager* manager) {
    TEST_START("检查角色是否存在");

    ASSERT_TRUE(TmeCharac_Exists(manager, 1001, 2001), "角色存在");
    ASSERT_TRUE(!TmeCharac_Exists(manager, 9999, 9999), "不存在的角色");

    return 0;
}

int test_update_level(DBConnectionManager* manager) {
    TEST_START("更新角色等级");

    ASSERT_EQ(TmeCharac_UpdateLevel(manager, 1001, 2001, 25, 20000), 0, "更新等级成功");

    TmeCharac charac;
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录");
    ASSERT_EQ(charac.lev, 25, "等级已更新为25");
    ASSERT_EQ(charac.exp, 20000, "经验已更新为20000");

    TmeCharac_Free(&charac);
    return 0;
}

int test_update_hp(DBConnectionManager* manager) {
    TEST_START("更新角色HP/MP");

    ASSERT_EQ(TmeCharac_UpdateHP(manager, 1001, 2001, 80, 250, 150), 0, "更新HP/MP成功");

    TmeCharac charac;
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录");
    ASSERT_EQ(charac.HP, 80, "当前HP已更新");
    ASSERT_EQ(charac.maxHP, 250, "最大HP已更新");
    ASSERT_EQ(charac.maxMP, 150, "最大MP已更新");

    TmeCharac_Free(&charac);
    return 0;
}

int test_update_fatigue(DBConnectionManager* manager) {
    TEST_START("更新疲劳值");

    ASSERT_EQ(TmeCharac_UpdateFatigue(manager, 1001, 2001, 50), 0, "更新疲劳值成功");

    TmeCharac charac;
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录");
    ASSERT_EQ(charac.fatigue, 50, "疲劳值已更新");

    TmeCharac_Free(&charac);
    return 0;
}

int test_update_guild(DBConnectionManager* manager) {
    TEST_START("更新公会信息");

    ASSERT_EQ(TmeCharac_UpdateGuild(manager, 1001, 2001, 200, 2), 0, "更新公会信息成功");

    TmeCharac charac;
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2001, &charac), 0, "获取记录");
    ASSERT_EQ(charac.guild_id, 200, "公会ID已更新");
    ASSERT_EQ(charac.guild_right, 2, "公会权限已更新");

    TmeCharac_Free(&charac);
    return 0;
}

int test_add_more_characs(DBConnectionManager* manager) {
    TEST_START("添加更多角色");

    for (int i = 2; i <= 5; i++) {
        TmeCharac charac;
        TmeCharac_Init(&charac);

        charac.m_id = 1001;
        charac.charac_no = 2000 + i;
        sprintf(charac.charac_name, "角色%d", i);
        charac.job = i;
        charac.lev = i * 5;
        charac.exp = i * 1000;
        charac.maxHP = 100 + i * 10;
        charac.maxMP = 80 + i * 10;
        strcpy(charac.create_time, "2025-01-01 10:00:00");
        strcpy(charac.last_play_time, "2025-01-01 10:00:00");

        ASSERT_EQ(TmeCharac_Add(manager, &charac), 0, "添加角色成功");
        TmeCharac_Free(&charac);
    }

    // 添加不同会员的角色
    TmeCharac charac;
    TmeCharac_Init(&charac);
    charac.m_id = 1002;
    charac.charac_no = 3001;
    strcpy(charac.charac_name, "其他会员角色");
    charac.lev = 15;
    charac.guild_id = 200;
    strcpy(charac.create_time, "2025-01-01 10:00:00");
    strcpy(charac.last_play_time, "2025-01-01 10:00:00");

    ASSERT_EQ(TmeCharac_Add(manager, &charac), 0, "添加其他会员角色成功");
    TmeCharac_Free(&charac);

    return 0;
}

int test_get_by_member_id(DBConnectionManager* manager) {
    TEST_START("查询会员的所有角色");

    TmeCharac characs[10];
    for (int i = 0; i < 10; i++) {
        TmeCharac_Init(&characs[i]);
    }

    int count = TmeCharac_GetByMemberId(manager, 1001, characs, 10);
    ASSERT_TRUE(count >= 5, "查询到至少5个角色");
    printf("    会员1001共有 %d 个角色\n", count);

    for (int i = 0; i < count; i++) {
        TmeCharac_Free(&characs[i]);
    }

    return 0;
}

int test_get_by_name(DBConnectionManager* manager) {
    TEST_START("根据角色名查询");

    TmeCharac charac;
    TmeCharac_Init(&charac);

    ASSERT_EQ(TmeCharac_GetByName(manager, "测试角色1", &charac), 0, "查询成功");
    ASSERT_EQ(charac.m_id, 1001, "会员ID正确");
    ASSERT_EQ(charac.charac_no, 2001, "角色编号正确");

    TmeCharac_Free(&charac);
    return 0;
}

int test_name_exists(DBConnectionManager* manager) {
    TEST_START("检查角色名是否存在");

    ASSERT_TRUE(TmeCharac_NameExists(manager, "测试角色1"), "角色名存在");
    ASSERT_TRUE(!TmeCharac_NameExists(manager, "不存在的角色"), "角色名不存在");

    return 0;
}

int test_count_by_member_id(DBConnectionManager* manager) {
    TEST_START("统计会员角色数量");

    int count = TmeCharac_CountByMemberId(manager, 1001);
    ASSERT_TRUE(count >= 5, "会员1001至少有5个角色");
    printf("    会员1001角色数: %d\n", count);

    int count2 = TmeCharac_CountByMemberId(manager, 1002);
    ASSERT_TRUE(count2 >= 1, "会员1002至少有1个角色");

    return 0;
}

int test_get_by_guild_id(DBConnectionManager* manager) {
    TEST_START("查询公会成员角色");

    TmeCharac characs[10];
    for (int i = 0; i < 10; i++) {
        TmeCharac_Init(&characs[i]);
    }

    int count = TmeCharac_GetByGuildId(manager, 200, characs, 10);
    ASSERT_TRUE(count >= 2, "公会200至少有2个成员");
    printf("    公会200共有 %d 个成员\n", count);

    for (int i = 0; i < count; i++) {
        TmeCharac_Free(&characs[i]);
    }

    return 0;
}

int test_mark_deleted(DBConnectionManager* manager) {
    TEST_START("标记角色为删除");

    ASSERT_EQ(TmeCharac_MarkDeleted(manager, 1001, 2005), 0, "标记删除成功");

    TmeCharac charac;
    TmeCharac_Init(&charac);
    ASSERT_EQ(TmeCharac_Get(manager, 1001, 2005, &charac), 0, "获取记录");
    ASSERT_EQ(charac.delete_flag, 1, "删除标记已设置");
    ASSERT_TRUE(strcmp(charac.delete_time, "0000-00-00 00:00:00") != 0, "删除时间已设置");

    TmeCharac_Free(&charac);
    return 0;
}

int test_delete(DBConnectionManager* manager) {
    TEST_START("删除TME角色记录");

    ASSERT_EQ(TmeCharac_Delete(manager, 1001, 2004), 0, "删除成功");
    ASSERT_TRUE(!TmeCharac_Exists(manager, 1001, 2004), "记录已删除");

    return 0;
}

int test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色信息");

    TmeCharac charac;
    TmeCharac_Init(&charac);

    if (TmeCharac_Get(manager, 1001, 2001, &charac) == 0) {
        TmeCharac_PrintInfo(&charac);
        ASSERT_TRUE(1, "打印信息成功");
        TmeCharac_Free(&charac);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("TME角色表 ORM 测试\n");
    printf("========================================\n");

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 - 避免重复键冲突 */
    char cleanup_query[256];
    DBQueryResult cleanup_result;
    
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    snprintf(cleanup_query, sizeof(cleanup_query),
             "DELETE FROM tme_charac WHERE m_id >= 1001 AND m_id <= 1010");
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_TAIWAN, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);

    /* 运行测试用例 */
    if (test_add_charac(&manager) == 0) test_passed++;
    if (test_get_charac(&manager) == 0) test_passed++;
    if (test_update_charac(&manager) == 0) test_passed++;
    if (test_exists(&manager) == 0) test_passed++;
    if (test_update_level(&manager) == 0) test_passed++;
    if (test_update_hp(&manager) == 0) test_passed++;
    if (test_update_fatigue(&manager) == 0) test_passed++;
    if (test_update_guild(&manager) == 0) test_passed++;
    if (test_add_more_characs(&manager) == 0) test_passed++;
    if (test_get_by_member_id(&manager) == 0) test_passed++;
    if (test_get_by_name(&manager) == 0) test_passed++;
    if (test_name_exists(&manager) == 0) test_passed++;
    if (test_count_by_member_id(&manager) == 0) test_passed++;
    if (test_get_by_guild_id(&manager) == 0) test_passed++;
    if (test_mark_deleted(&manager) == 0) test_passed++;
    if (test_delete(&manager) == 0) test_passed++;
    if (test_print_info(&manager) == 0) test_passed++;

    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("========================================\n");

    return (test_passed == test_count) ? 0 : 1;
}
