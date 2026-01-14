#include "charac_action_point_orm.h"
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
    TEST_START("添加和获取AP记录");

    CharacActionPoint record;
    memset(&record, 0, sizeof(CharacActionPoint));
    strcpy(record.occ_date, "2025-11-16");
    record.charac_no = 9001;
    record.ap_sum = 150;
    record.is_reward_medal = 0;
    record.is_reward_item_1 = 0;
    record.is_reward_item_2 = 0;
    record.is_reward_item_3 = 0;
    record.is_reward_item_4 = 0;

    /* 创建测试BLOB数据 */
    const unsigned char test_state[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    record.ap_clear_state_len = 5;
    record.ap_clear_state = (unsigned char*)malloc(record.ap_clear_state_len);
    memcpy(record.ap_clear_state, test_state, record.ap_clear_state_len);

    ASSERT_EQ(CharacActionPoint_Add(manager, &record), 0, "添加记录成功");
    free(record.ap_clear_state);

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9001, "2025-11-16", &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, 9001, "角色编号匹配");
    ASSERT_STR_EQ(retrieved.occ_date, "2025-11-16", "发生日期匹配");
    ASSERT_EQ(retrieved.ap_sum, 150, "AP总和匹配");
    ASSERT_EQ(retrieved.ap_clear_state_len, 5, "AP清除状态长度匹配");

    CharacActionPoint_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新AP记录");

    CharacActionPoint record;
    memset(&record, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9001, "2025-11-16", &record), 0, "获取已存在记录");

    record.ap_sum = 300;
    record.is_reward_medal = 1;
    record.is_reward_item_1 = 1;

    ASSERT_EQ(CharacActionPoint_Update(manager, &record), 0, "更新记录成功");
    CharacActionPoint_Free(&record);

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9001, "2025-11-16", &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.ap_sum, 300, "AP总和已更新");
    ASSERT_EQ(retrieved.is_reward_medal, 1, "勋章奖励标识已更新");
    ASSERT_EQ(retrieved.is_reward_item_1, 1, "物品1奖励标识已更新");

    CharacActionPoint_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CharacActionPoint_Exists(manager, 9001, "2025-11-16"), "记录存在");
    ASSERT_TRUE(!CharacActionPoint_Exists(manager, 9999, "2025-11-16"), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条AP记录");

    CharacActionPoint record2;
    memset(&record2, 0, sizeof(CharacActionPoint));
    strcpy(record2.occ_date, "2025-11-16");
    record2.charac_no = 9002;
    record2.ap_sum = 200;
    const unsigned char state2[] = {0xAA, 0xBB};
    record2.ap_clear_state = (unsigned char*)malloc(2);
    memcpy(record2.ap_clear_state, state2, 2);
    record2.ap_clear_state_len = 2;
    ASSERT_EQ(CharacActionPoint_Add(manager, &record2), 0, "添加记录2成功");
    free(record2.ap_clear_state);

    CharacActionPoint record3;
    memset(&record3, 0, sizeof(CharacActionPoint));
    strcpy(record3.occ_date, "2025-11-15");
    record3.charac_no = 9001;
    record3.ap_sum = 100;
    const unsigned char state3[] = {0xFF};
    record3.ap_clear_state = (unsigned char*)malloc(1);
    memcpy(record3.ap_clear_state, state3, 1);
    record3.ap_clear_state_len = 1;
    ASSERT_EQ(CharacActionPoint_Add(manager, &record3), 0, "添加记录3成功");
    free(record3.ap_clear_state);
}

void test_increment_ap_sum(DBConnectionManager* manager) {
    TEST_START("增加AP值");

    ASSERT_EQ(CharacActionPoint_IncrementAPSum(manager, 9001, "2025-11-16", 50), 0, "增加AP值成功");

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9001, "2025-11-16", &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.ap_sum, 350, "AP值已增加50 (300+50=350)");

    CharacActionPoint_Free(&retrieved);
}

void test_claim_reward_medal(DBConnectionManager* manager) {
    TEST_START("领取勋章奖励");

    /* 创建新记录用于测试 */
    CharacActionPoint record;
    memset(&record, 0, sizeof(CharacActionPoint));
    strcpy(record.occ_date, "2025-11-17");
    record.charac_no = 9003;
    record.ap_sum = 500;
    const unsigned char state[] = {0x00};
    record.ap_clear_state = (unsigned char*)malloc(1);
    memcpy(record.ap_clear_state, state, 1);
    record.ap_clear_state_len = 1;
    CharacActionPoint_Add(manager, &record);
    free(record.ap_clear_state);

    ASSERT_EQ(CharacActionPoint_ClaimRewardMedal(manager, 9003, "2025-11-17"), 0, "领取勋章奖励成功");

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9003, "2025-11-17", &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.is_reward_medal, 1, "勋章奖励已领取");

    CharacActionPoint_Free(&retrieved);
}

void test_claim_reward_item(DBConnectionManager* manager) {
    TEST_START("领取物品奖励");

    ASSERT_EQ(CharacActionPoint_ClaimRewardItem(manager, 9003, "2025-11-17", 1), 0, "领取物品1奖励成功");
    ASSERT_EQ(CharacActionPoint_ClaimRewardItem(manager, 9003, "2025-11-17", 3), 0, "领取物品3奖励成功");

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9003, "2025-11-17", &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.is_reward_item_1, 1, "物品1奖励已领取");
    ASSERT_EQ(retrieved.is_reward_item_3, 1, "物品3奖励已领取");

    CharacActionPoint_Free(&retrieved);
}

void test_update_ap_clear_state(DBConnectionManager* manager) {
    TEST_START("更新AP清除状态");

    const unsigned char new_state[] = {0xDE, 0xAD, 0xBE, 0xEF};
    ASSERT_EQ(CharacActionPoint_UpdateAPClearState(manager, 9003, "2025-11-17", new_state, 4), 0,
              "更新AP清除状态成功");

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    ASSERT_EQ(CharacActionPoint_Get(manager, 9003, "2025-11-17", &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.ap_clear_state_len, 4, "AP清除状态长度已更新");

    CharacActionPoint_Free(&retrieved);
}

void test_get_by_date(DBConnectionManager* manager) {
    TEST_START("按日期查询所有角色的AP记录");

    CharacActionPoint records[10];
    memset(records, 0, sizeof(records));
    int count = CharacActionPoint_GetByDate(manager, "2025-11-16", records, 10);

    ASSERT_TRUE(count >= 2, "至少找到2条记录");
    ASSERT_TRUE(records[0].ap_sum >= records[1].ap_sum, "按AP总和降序排列");

    int i;
    for (i = 0; i < count; i++) {
        CharacActionPoint_Free(&records[i]);
    }
}

void test_get_by_charac_no(DBConnectionManager* manager) {
    TEST_START("按角色查询所有日期的AP记录");

    CharacActionPoint records[10];
    memset(records, 0, sizeof(records));
    int count = CharacActionPoint_GetByCharacNo(manager, 9001, records, 10);

    ASSERT_TRUE(count >= 2, "至少找到2条记录（9001角色）");
    ASSERT_STR_EQ(records[0].occ_date, "2025-11-16", "最新日期在前");

    int i;
    for (i = 0; i < count; i++) {
        CharacActionPoint_Free(&records[i]);
    }
}

void test_check_reward_status(DBConnectionManager* manager) {
    TEST_START("检查奖励领取状态");

    int medal_claimed = 0;
    int items_claimed = 0;

    ASSERT_EQ(CharacActionPoint_CheckRewardStatus(manager, 9003, "2025-11-17",
                                                   &medal_claimed, &items_claimed), 0,
              "查询奖励状态成功");
    ASSERT_EQ(medal_claimed, 1, "勋章已领取");
    ASSERT_EQ(items_claimed, 2, "已领取2个物品奖励（item_1和item_3）");
}

void test_get_by_min_ap_sum(DBConnectionManager* manager) {
    TEST_START("获取指定日期AP总和大于指定值的角色");

    CharacActionPoint records[10];
    memset(records, 0, sizeof(records));
    int count = CharacActionPoint_GetByMinAPSum(manager, "2025-11-16", 250, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录（AP>=250）");
    ASSERT_TRUE(records[0].ap_sum >= 250, "AP总和满足条件");

    int i;
    for (i = 0; i < count; i++) {
        CharacActionPoint_Free(&records[i]);
    }
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacActionPoint_Delete(manager, 9001, "2025-11-16"), 0, "删除记录成功");
    ASSERT_TRUE(!CharacActionPoint_Exists(manager, 9001, "2025-11-16"), "记录已删除");

    /* 清理其他测试数据 */
    CharacActionPoint_Delete(manager, 9001, "2025-11-15");
    CharacActionPoint_Delete(manager, 9002, "2025-11-16");
    CharacActionPoint_Delete(manager, 9003, "2025-11-17");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印AP记录信息");

    /* 创建一条记录用于打印 */
    CharacActionPoint record;
    memset(&record, 0, sizeof(CharacActionPoint));
    strcpy(record.occ_date, "2025-11-18");
    record.charac_no = 9999;
    record.ap_sum = 999;
    record.is_reward_medal = 1;
    record.is_reward_item_1 = 1;
    record.is_reward_item_2 = 1;
    record.is_reward_item_3 = 1;
    record.is_reward_item_4 = 1;
    const unsigned char state[] = {0x12, 0x34, 0x56, 0x78};
    record.ap_clear_state = (unsigned char*)malloc(4);
    memcpy(record.ap_clear_state, state, 4);
    record.ap_clear_state_len = 4;
    CharacActionPoint_Add(manager, &record);
    free(record.ap_clear_state);

    CharacActionPoint retrieved;
    memset(&retrieved, 0, sizeof(CharacActionPoint));
    if (CharacActionPoint_Get(manager, 9999, "2025-11-18", &retrieved) == 0) {
        CharacActionPoint_PrintInfo(&retrieved);
        ASSERT_TRUE(1, "打印信息成功");
        CharacActionPoint_Free(&retrieved);
    }

    /* 清理 */
    CharacActionPoint_Delete(manager, 9999, "2025-11-18");
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
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("========================================\n");
    printf("角色行动点数 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_increment_ap_sum(&manager);
    test_claim_reward_medal(&manager);
    test_claim_reward_item(&manager);
    test_update_ap_clear_state(&manager);
    test_get_by_date(&manager);
    test_get_by_charac_no(&manager);
    test_check_reward_status(&manager);
    test_get_by_min_ap_sum(&manager);
    test_delete(&manager);
    test_print_info(&manager);

    /* 最后一个测试也要算进去 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
