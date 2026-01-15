#include "charac_quest_shop_orm.h"
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

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取任务商店数据");

    CharacQuestShop record, retrieved;
    memset(&record, 0, sizeof(CharacQuestShop));

    record.charac_no = 800001;
    record.qp = 1000;
    record.max_hp = 500;
    record.max_mp = 300;
    record.psy_attack = 100;
    record.psy_defense = 80;
    record.mag_attack = 120;
    record.mag_defence = 90;
    record.move_speed = 50;
    record.attack_speed = 60;
    record.hp_regen = 10;
    record.mp_regen = 8;
    record.all_element_resist = 20;
    record.fire_element_resist = 15;
    record.water_element_resist = 15;
    record.light_element_resist = 15;
    record.dark_element_resist = 15;

    ASSERT_EQ(CharacQuestShop_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.qp, record.qp, "任务点数匹配");
    ASSERT_EQ(retrieved.max_hp, record.max_hp, "最大HP匹配");
    ASSERT_EQ(retrieved.psy_attack, record.psy_attack, "物理攻击匹配");
    ASSERT_EQ(retrieved.mag_attack, record.mag_attack, "魔法攻击匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新任务商店数据");

    CharacQuestShop record, retrieved;
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &record), 0, "获取已存在记录");

    record.qp = 2000;
    record.max_hp = 1000;
    record.psy_attack = 200;

    ASSERT_EQ(CharacQuestShop_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.qp, 2000, "QP已更新");
    ASSERT_EQ(retrieved.max_hp, 1000, "HP已更新");
    ASSERT_EQ(retrieved.psy_attack, 200, "物理攻击已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_EQ(CharacQuestShop_Exists(manager, 800001), 1, "记录存在");
    ASSERT_EQ(CharacQuestShop_Exists(manager, 999999), 0, "记录不存在");
}

void test_add_quest_points(DBConnectionManager* manager) {
    TEST_START("增加任务点数");

    CharacQuestShop retrieved;
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取当前记录");
    unsigned int old_qp = retrieved.qp;

    ASSERT_EQ(CharacQuestShop_AddQuestPoints(manager, 800001, 500), 0, "增加QP成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.qp, old_qp + 500, "QP增加了500");
}

void test_spend_quest_points(DBConnectionManager* manager) {
    TEST_START("消费任务点数");

    CharacQuestShop retrieved;
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取当前记录");
    unsigned int old_qp = retrieved.qp;

    ASSERT_EQ(CharacQuestShop_SpendQuestPoints(manager, 800001, 300), 0, "消费QP成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.qp, old_qp - 300, "QP减少了300");

    /* 测试QP不足 */
    ASSERT_EQ(CharacQuestShop_SpendQuestPoints(manager, 800001, 999999), -1, "QP不足时消费失败");
}

void test_update_stat(DBConnectionManager* manager) {
    TEST_START("更新单个属性");

    CharacQuestShop retrieved;

    ASSERT_EQ(CharacQuestShop_UpdateStat(manager, 800001, "max_hp", 1500), 0, "更新max_hp成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.max_hp, 1500, "max_hp已更新为1500");

    ASSERT_EQ(CharacQuestShop_UpdateStat(manager, 800001, "psy_attack", 250), 0, "更新psy_attack成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.psy_attack, 250, "psy_attack已更新为250");
}

void test_increment_stat(DBConnectionManager* manager) {
    TEST_START("增加单个属性值");

    CharacQuestShop retrieved;
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取当前记录");
    unsigned short old_hp = retrieved.max_hp;

    ASSERT_EQ(CharacQuestShop_IncrementStat(manager, 800001, "max_hp", 200), 0, "增加max_hp成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.max_hp, old_hp + 200, "max_hp增加了200");
}

void test_update_element_resists(DBConnectionManager* manager) {
    TEST_START("批量更新元素抗性");

    CharacQuestShop retrieved;

    ASSERT_EQ(CharacQuestShop_UpdateElementResists(manager, 800001, 50, 40, 30, 20, 10), 0,
              "更新元素抗性成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.all_element_resist, 50, "全元素抗性已更新");
    ASSERT_EQ(retrieved.fire_element_resist, 40, "火抗性已更新");
    ASSERT_EQ(retrieved.water_element_resist, 30, "水抗性已更新");
    ASSERT_EQ(retrieved.light_element_resist, 20, "光抗性已更新");
    ASSERT_EQ(retrieved.dark_element_resist, 10, "暗抗性已更新");
}

void test_update_element_attacks(DBConnectionManager* manager) {
    TEST_START("批量更新元素攻击");

    CharacQuestShop retrieved;

    ASSERT_EQ(CharacQuestShop_UpdateElementAttacks(manager, 800001, 60, 50, 40, 30, 20), 0,
              "更新元素攻击成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.all_element_attack, 60, "全元素攻击已更新");
    ASSERT_EQ(retrieved.fire_element_attack, 50, "火攻击已更新");
    ASSERT_EQ(retrieved.water_element_attack, 40, "水攻击已更新");
    ASSERT_EQ(retrieved.light_element_attack, 30, "光攻击已更新");
    ASSERT_EQ(retrieved.dark_element_attack, 20, "暗攻击已更新");
}

void test_get_total_count(DBConnectionManager* manager) {
    TEST_START("获取总记录数");

    /* 添加更多记录 */
    CharacQuestShop record2, record3;
    memset(&record2, 0, sizeof(CharacQuestShop));
    memset(&record3, 0, sizeof(CharacQuestShop));

    record2.charac_no = 800002;
    record2.qp = 1500;
    record2.max_hp = 600;

    record3.charac_no = 800003;
    record3.qp = 3000;
    record3.max_hp = 800;

    CharacQuestShop_Add(manager, &record2);
    CharacQuestShop_Add(manager, &record3);

    int count = CharacQuestShop_GetTotalCount(manager);
    ASSERT_TRUE(count >= 3, "至少有3条记录");
}

void test_get_by_qp_range(DBConnectionManager* manager) {
    TEST_START("获取指定QP范围的角色列表");

    unsigned int charac_nos[10];
    int count = CharacQuestShop_GetByQPRange(manager, 1000, 2000, charac_nos, 10);

    ASSERT_TRUE(count >= 1, "至少找到1个角色在QP范围1000-2000");
}

void test_get_top_by_qp(DBConnectionManager* manager) {
    TEST_START("获取QP排名前N的角色");

    unsigned int charac_nos[5];
    int count = CharacQuestShop_GetTopByQP(manager, 5, charac_nos);

    ASSERT_TRUE(count >= 3, "找到至少3个角色");
    ASSERT_TRUE(count <= 5, "不超过限制数量5");

    /* 验证第一名QP最高 */
    CharacQuestShop first, second;
    if (count >= 2) {
        CharacQuestShop_Get(manager, charac_nos[0], &first);
        CharacQuestShop_Get(manager, charac_nos[1], &second);
        ASSERT_TRUE(first.qp >= second.qp, "第一名QP >= 第二名QP");
    }
}

void test_get_total_qp(DBConnectionManager* manager) {
    TEST_START("获取所有角色的总QP");

    long long total_qp = CharacQuestShop_GetTotalQP(manager);
    ASSERT_TRUE(total_qp > 0, "总QP大于0");
}

void test_reset_all_stats(DBConnectionManager* manager) {
    TEST_START("重置所有属性（保留QP）");

    CharacQuestShop retrieved;
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取记录");
    unsigned int old_qp = retrieved.qp;

    ASSERT_EQ(CharacQuestShop_ResetAllStats(manager, 800001), 0, "重置所有属性成功");
    ASSERT_EQ(CharacQuestShop_Get(manager, 800001, &retrieved), 0, "获取重置后记录");

    ASSERT_EQ(retrieved.qp, old_qp, "QP保留未变");
    ASSERT_EQ(retrieved.max_hp, 0, "max_hp已重置为0");
    ASSERT_EQ(retrieved.psy_attack, 0, "psy_attack已重置为0");
    ASSERT_EQ(retrieved.all_element_resist, 0, "all_element_resist已重置为0");
    ASSERT_EQ(retrieved.all_element_attack, 0, "all_element_attack已重置为0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacQuestShop_Delete(manager, 800001), 0, "删除记录成功");
    ASSERT_EQ(CharacQuestShop_Exists(manager, 800001), 0, "记录已删除");

    ASSERT_EQ(CharacQuestShop_Delete(manager, 800002), 0, "删除记录成功");
    ASSERT_EQ(CharacQuestShop_Delete(manager, 800003), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印任务商店信息");

    CharacQuestShop record;
    memset(&record, 0, sizeof(CharacQuestShop));

    record.charac_no = 800004;
    record.qp = 5000;
    record.max_hp = 2000;
    record.max_mp = 1500;
    record.psy_attack = 300;
    record.mag_attack = 350;
    record.all_element_resist = 100;
    record.all_element_attack = 120;

    CharacQuestShop_Add(manager, &record);

    CharacQuestShop retrieved;
    CharacQuestShop_Get(manager, 800004, &retrieved);

    printf("\n");
    CharacQuestShop_PrintInfo(&retrieved);

    CharacQuestShop_Delete(manager, 800004);

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
    printf("角色任务商店 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_quest_points(&manager);
    test_spend_quest_points(&manager);
    test_update_stat(&manager);
    test_increment_stat(&manager);
    test_update_element_resists(&manager);
    test_update_element_attacks(&manager);
    test_get_total_count(&manager);
    test_get_by_qp_range(&manager);
    test_get_top_by_qp(&manager);
    test_get_total_qp(&manager);
    test_reset_all_stats(&manager);
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
