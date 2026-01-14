#include "game_channel_orm.h"
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
            printf("  ✗ %s (FAILED: %lld != %lld)\n", message, (long long)(a), (long long)(b)); \
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
            printf("  ✗ %s (FAILED: %lld <= %lld)\n", message, (long long)(a), (long long)(b)); \
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

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    GameChannel channel, retrieved;
    int result, total;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize database manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        fprintf(stderr, "Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* ========== 测试用例 ========== */

    TEST_START("Add - 添加游戏频道");
    memset(&channel, 0, sizeof(GameChannel));
    channel.gc_now = 100;
    strncpy(channel.gc_ip, "192.168.1.100", sizeof(channel.gc_ip) - 1);
    channel.gc_port = 7600;
    channel.gc_max = 500;
    channel.gc_game = 1;
    strncpy(channel.gc_channel, "CH01", sizeof(channel.gc_channel) - 1);
    channel.gc_ch_group = 1;
    strncpy(channel.gc_channeltype, "Normal", sizeof(channel.gc_channeltype) - 1);
    strncpy(channel.gc_up_time, "2025-11-16 19:00:00", sizeof(channel.gc_up_time) - 1);
    channel.gc_swordman_cnt = 20;
    channel.gc_fighter_cnt = 15;
    channel.gc_gunner_cnt = 10;
    channel.gc_mage_cnt = 25;
    channel.gc_priest_cnt = 18;
    channel.gc_at_gunner_cnt = 8;
    channel.gc_thief_cnt = 4;
    channel.gc_hangame = 50;
    channel.gc_nexon = 50;
    channel.gc_type = 0;

    result = GameChannel_Add(&manager, &channel);
    ASSERT_GT(result, 0, "添加成功并返回自增ID");
    unsigned int gc_no1 = (unsigned int)result;

    TEST_START("Exists - 检查频道是否存在");
    result = GameChannel_Exists(&manager, gc_no1);
    ASSERT_EQ(result, 1, "频道存在");

    TEST_START("Get - 获取游戏频道");
    memset(&retrieved, 0, sizeof(GameChannel));
    result = GameChannel_Get(&manager, gc_no1, &retrieved);
    ASSERT_EQ(result, 0, "获取成功");
    ASSERT_EQ(retrieved.gc_no, gc_no1, "频道编号正确");
    ASSERT_EQ(retrieved.gc_now, 100, "当前在线人数正确");
    ASSERT_STR_EQ(retrieved.gc_ip, "192.168.1.100", "IP地址正确");
    ASSERT_EQ(retrieved.gc_port, 7600, "端口正确");
    ASSERT_EQ(retrieved.gc_swordman_cnt, 20, "鬼剑士数量正确");
    ASSERT_EQ(retrieved.gc_mage_cnt, 25, "魔法师数量正确");

    TEST_START("Update - 更新游戏频道");
    retrieved.gc_now = 200;
    retrieved.gc_swordman_cnt = 30;
    result = GameChannel_Update(&manager, &retrieved);
    ASSERT_EQ(result, 0, "更新成功");
    result = GameChannel_Get(&manager, gc_no1, &retrieved);
    ASSERT_EQ(retrieved.gc_now, 200, "更新后在线人数正确");
    ASSERT_EQ(retrieved.gc_swordman_cnt, 30, "更新后鬼剑士数量正确");

    TEST_START("UpdateOnlineCount - 更新在线人数");
    result = GameChannel_UpdateOnlineCount(&manager, gc_no1, 150);
    ASSERT_EQ(result, 0, "更新在线人数成功");
    result = GameChannel_Get(&manager, gc_no1, &retrieved);
    ASSERT_EQ(retrieved.gc_now, 150, "在线人数更新正确");

    TEST_START("UpdateJobCounts - 更新职业人数统计");
    result = GameChannel_UpdateJobCounts(&manager, gc_no1, 25, 20, 15, 30, 22, 10, 8);
    ASSERT_EQ(result, 0, "更新职业人数成功");
    result = GameChannel_Get(&manager, gc_no1, &retrieved);
    ASSERT_EQ(retrieved.gc_swordman_cnt, 25, "鬼剑士数量更新正确");
    ASSERT_EQ(retrieved.gc_mage_cnt, 30, "魔法师数量更新正确");
    ASSERT_EQ(retrieved.gc_thief_cnt, 8, "盗贼数量更新正确");

    TEST_START("UpdateTimestamp - 更新时间戳");
    result = GameChannel_UpdateTimestamp(&manager, gc_no1);
    ASSERT_EQ(result, 0, "更新时间戳成功");

    /* 添加更多测试数据 */
    memset(&channel, 0, sizeof(GameChannel));
    channel.gc_now = 80;
    strncpy(channel.gc_ip, "192.168.1.101", sizeof(channel.gc_ip) - 1);
    channel.gc_port = 7601;
    channel.gc_max = 500;
    channel.gc_game = 1;
    strncpy(channel.gc_channel, "CH02", sizeof(channel.gc_channel) - 1);
    channel.gc_ch_group = 1;
    strncpy(channel.gc_channeltype, "Normal", sizeof(channel.gc_channeltype) - 1);
    strncpy(channel.gc_up_time, "2025-11-16 19:00:00", sizeof(channel.gc_up_time) - 1);
    channel.gc_swordman_cnt = 10;
    channel.gc_fighter_cnt = 10;
    channel.gc_gunner_cnt = 10;
    channel.gc_mage_cnt = 20;
    channel.gc_priest_cnt = 10;
    channel.gc_at_gunner_cnt = 10;
    channel.gc_thief_cnt = 10;
    result = GameChannel_Add(&manager, &channel);
    unsigned int gc_no2 = (unsigned int)result;

    channel.gc_now = 120;
    strncpy(channel.gc_ip, "192.168.1.102", sizeof(channel.gc_ip) - 1);
    channel.gc_port = 7602;
    channel.gc_ch_group = 2;
    strncpy(channel.gc_channel, "CH03", sizeof(channel.gc_channel) - 1);
    result = GameChannel_Add(&manager, &channel);

    TEST_START("Count - 获取所有频道总数");
    result = GameChannel_Count(&manager);
    ASSERT_EQ(result, 3, "总频道数为3");

    TEST_START("CountByGroup - 按频道组统计");
    result = GameChannel_CountByGroup(&manager, 1);
    ASSERT_EQ(result, 2, "频道组1有2个频道");
    result = GameChannel_CountByGroup(&manager, 2);
    ASSERT_EQ(result, 1, "频道组2有1个频道");

    TEST_START("GetTotalOnline - 获取总在线人数");
    total = GameChannel_GetTotalOnline(&manager);
    ASSERT_EQ(total, 350, "总在线人数为350（150+80+120）");

    TEST_START("GetTotalJobCount - 获取职业总人数");
    total = GameChannel_GetTotalJobCount(&manager, 0);  /* 鬼剑士 */
    ASSERT_EQ(total, 45, "鬼剑士总人数为45（25+10+10）");
    total = GameChannel_GetTotalJobCount(&manager, 3);  /* 魔法师 */
    ASSERT_EQ(total, 70, "魔法师总人数为70（30+20+20）");

    TEST_START("DeleteByGroup - 删除指定频道组");
    result = GameChannel_DeleteByGroup(&manager, 2);
    ASSERT_EQ(result, 0, "删除频道组2成功");
    result = GameChannel_Count(&manager);
    ASSERT_EQ(result, 2, "删除后总频道数为2");

    TEST_START("Delete - 删除单个频道");
    result = GameChannel_Delete(&manager, gc_no2);
    ASSERT_EQ(result, 0, "删除成功");
    result = GameChannel_Exists(&manager, gc_no2);
    ASSERT_EQ(result, 0, "删除后频道不存在");

    TEST_START("Delete - 删除不存在的频道");
    result = GameChannel_Delete(&manager, 999999);
    ASSERT_EQ(result, 0, "删除不存在频道不报错");

    TEST_START("Get - 获取不存在的频道");
    result = GameChannel_Get(&manager, 999999, &retrieved);
    ASSERT_EQ(result, -1, "获取不存在频道返回-1");

    TEST_START("Exists - 检查不存在的频道");
    result = GameChannel_Exists(&manager, 999999);
    ASSERT_EQ(result, 0, "不存在的频道返回0");

    /* 最后一个测试计入 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
