#include "item_making_skill_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 测试框架
static int g_test_count = 0;
static int g_test_passed = 0;
static int g_assert_count = 0;
static int g_assert_passed = 0;

#define TEST_START(name) \
    do { \
        g_test_count++; \
        printf("\n=== Test %d: %s ===\n", g_test_count, name); \
    } while(0)

#define TEST_PASS() \
    do { \
        g_test_passed++; \
        printf("✓ PASSED\n"); \
    } while(0)

#define ASSERT(condition, message) \
    do { \
        g_assert_count++; \
        if (condition) { \
            g_assert_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ FAILED: %s\n", message); \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        g_assert_count++; \
        if ((a) == (b)) { \
            g_assert_passed++; \
            printf("  ✓ %s (%d == %d)\n", message, (int)(a), (int)(b)); \
        } else { \
            printf("  ✗ FAILED: %s (%d != %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

// 测试用例
void test_add_and_get(DBConnectionManager* manager)
{
    TEST_START("添加并获取制作技能信息");

    ItemMakingSkillInfo record, retrieved;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));
    memset(&retrieved, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10001;
    record.weapon = 10;
    record.cloth = 5;
    record.leather = 8;
    record.light_armor = 3;
    record.heavy_armor = 7;
    record.plate = 6;
    record.amulet = 4;
    record.wrist = 9;
    record.ring = 2;
    record.support = 1;
    record.magic_stone = 11;

    int ret = ItemMakingSkillInfo_Add(manager, &record);
    ASSERT(ret == 0, "添加制作技能信息成功");

    ret = ItemMakingSkillInfo_Get(manager, 10001, &retrieved);
    ASSERT(ret == 0, "获取制作技能信息成功");
    ASSERT_EQ(retrieved.charac_no, 10001, "角色编号正确");
    ASSERT_EQ(retrieved.weapon, 10, "武器制作等级正确");
    ASSERT_EQ(retrieved.cloth, 5, "布甲制作等级正确");
    ASSERT_EQ(retrieved.magic_stone, 11, "魔法石制作等级正确");

    ItemMakingSkillInfo_Delete(manager, 10001);
    TEST_PASS();
}

void test_update(DBConnectionManager* manager)
{
    TEST_START("更新制作技能信息");

    ItemMakingSkillInfo record, retrieved;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10002;
    record.weapon = 5;
    record.cloth = 3;
    ItemMakingSkillInfo_Add(manager, &record);

    record.weapon = 15;
    record.cloth = 8;
    record.leather = 10;
    int ret = ItemMakingSkillInfo_Update(manager, &record);
    ASSERT(ret == 0, "更新制作技能信息成功");

    ItemMakingSkillInfo_Get(manager, 10002, &retrieved);
    ASSERT_EQ(retrieved.weapon, 15, "更新后武器等级正确");
    ASSERT_EQ(retrieved.cloth, 8, "更新后布甲等级正确");
    ASSERT_EQ(retrieved.leather, 10, "更新后皮甲等级正确");

    ItemMakingSkillInfo_Delete(manager, 10002);
    TEST_PASS();
}

void test_delete(DBConnectionManager* manager)
{
    TEST_START("删除制作技能信息");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10003;
    record.weapon = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    int ret = ItemMakingSkillInfo_Delete(manager, 10003);
    ASSERT(ret == 0, "删除制作技能信息成功");

    ret = ItemMakingSkillInfo_Exists(manager, 10003);
    ASSERT(ret == 0, "删除后记录不存在");

    TEST_PASS();
}

void test_exists(DBConnectionManager* manager)
{
    TEST_START("检查制作技能信息是否存在");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    int ret = ItemMakingSkillInfo_Exists(manager, 10004);
    ASSERT(ret == 0, "不存在的记录返回0");

    record.charac_no = 10004;
    record.weapon = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    ret = ItemMakingSkillInfo_Exists(manager, 10004);
    ASSERT(ret == 1, "存在的记录返回1");

    ItemMakingSkillInfo_Delete(manager, 10004);
    TEST_PASS();
}

void test_upgrade_skill(DBConnectionManager* manager)
{
    TEST_START("升级指定类型制作技能");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10005;
    record.weapon = 5;
    record.cloth = 3;
    ItemMakingSkillInfo_Add(manager, &record);

    int ret = ItemMakingSkillInfo_UpgradeSkill(manager, 10005, MAKING_SKILL_WEAPON, 3);
    ASSERT(ret == 0, "升级武器技能成功");

    int level = ItemMakingSkillInfo_GetSkillLevel(manager, 10005, MAKING_SKILL_WEAPON);
    ASSERT_EQ(level, 8, "武器技能升级后等级正确");

    ret = ItemMakingSkillInfo_UpgradeSkill(manager, 10005, MAKING_SKILL_CLOTH, 5);
    ASSERT(ret == 0, "升级布甲技能成功");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10005, MAKING_SKILL_CLOTH);
    ASSERT_EQ(level, 8, "布甲技能升级后等级正确");

    ItemMakingSkillInfo_Delete(manager, 10005);
    TEST_PASS();
}

void test_set_skill_level(DBConnectionManager* manager)
{
    TEST_START("设置指定类型制作技能等级");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10006;
    record.weapon = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    int ret = ItemMakingSkillInfo_SetSkillLevel(manager, 10006, MAKING_SKILL_WEAPON, 20);
    ASSERT(ret == 0, "设置武器技能等级成功");

    int level = ItemMakingSkillInfo_GetSkillLevel(manager, 10006, MAKING_SKILL_WEAPON);
    ASSERT_EQ(level, 20, "武器技能等级设置正确");

    ret = ItemMakingSkillInfo_SetSkillLevel(manager, 10006, MAKING_SKILL_RING, 15);
    ASSERT(ret == 0, "设置戒指技能等级成功");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10006, MAKING_SKILL_RING);
    ASSERT_EQ(level, 15, "戒指技能等级设置正确");

    ItemMakingSkillInfo_Delete(manager, 10006);
    TEST_PASS();
}

void test_get_skill_level(DBConnectionManager* manager)
{
    TEST_START("获取指定类型制作技能等级");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10007;
    record.weapon = 12;
    record.cloth = 8;
    record.plate = 15;
    ItemMakingSkillInfo_Add(manager, &record);

    int level = ItemMakingSkillInfo_GetSkillLevel(manager, 10007, MAKING_SKILL_WEAPON);
    ASSERT_EQ(level, 12, "获取武器技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10007, MAKING_SKILL_CLOTH);
    ASSERT_EQ(level, 8, "获取布甲技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10007, MAKING_SKILL_PLATE);
    ASSERT_EQ(level, 15, "获取板甲技能等级正确");

    ItemMakingSkillInfo_Delete(manager, 10007);
    TEST_PASS();
}

void test_reset_all_skills(DBConnectionManager* manager)
{
    TEST_START("重置所有制作技能");

    ItemMakingSkillInfo record, retrieved;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10008;
    record.weapon = 10;
    record.cloth = 8;
    record.leather = 12;
    record.amulet = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    int ret = ItemMakingSkillInfo_ResetAllSkills(manager, 10008);
    ASSERT(ret == 0, "重置所有技能成功");

    ItemMakingSkillInfo_Get(manager, 10008, &retrieved);
    ASSERT_EQ(retrieved.weapon, 0, "武器技能已重置");
    ASSERT_EQ(retrieved.cloth, 0, "布甲技能已重置");
    ASSERT_EQ(retrieved.leather, 0, "皮甲技能已重置");
    ASSERT_EQ(retrieved.amulet, 0, "项链技能已重置");

    ItemMakingSkillInfo_Delete(manager, 10008);
    TEST_PASS();
}

void test_get_max_skill_level(DBConnectionManager* manager)
{
    TEST_START("获取最高制作技能等级");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10009;
    record.weapon = 8;
    record.cloth = 12;
    record.leather = 5;
    record.plate = 20;
    record.ring = 3;
    ItemMakingSkillInfo_Add(manager, &record);

    int max_level = ItemMakingSkillInfo_GetMaxSkillLevel(manager, 10009);
    ASSERT_EQ(max_level, 20, "最高技能等级正确（plate=20）");

    ItemMakingSkillInfo_Delete(manager, 10009);
    TEST_PASS();
}

void test_count(DBConnectionManager* manager)
{
    TEST_START("统计记录数");

    ItemMakingSkillInfo record;
    int charac_nos[] = {10011, 10012, 10013};
    int i;

    int initial_count = ItemMakingSkillInfo_Count(manager);
    ASSERT(initial_count >= 0, "获取初始记录数成功");

    memset(&record, 0, sizeof(ItemMakingSkillInfo));
    for (i = 0; i < 3; i++) {
        record.charac_no = charac_nos[i];
        record.weapon = (i + 1) * 5;
        ItemMakingSkillInfo_Add(manager, &record);
    }

    int new_count = ItemMakingSkillInfo_Count(manager);
    ASSERT_EQ(new_count, initial_count + 3, "记录数增加3条");

    for (i = 0; i < 3; i++) {
        ItemMakingSkillInfo_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_get_all(DBConnectionManager* manager)
{
    TEST_START("获取所有记录");

    ItemMakingSkillInfo record;
    ItemMakingSkillInfo records[10];
    int charac_nos[] = {10021, 10022, 10023};
    int i;

    memset(&record, 0, sizeof(ItemMakingSkillInfo));
    for (i = 0; i < 3; i++) {
        record.charac_no = charac_nos[i];
        record.weapon = (i + 1) * 3;
        ItemMakingSkillInfo_Add(manager, &record);
    }

    int count = ItemMakingSkillInfo_GetAll(manager, records, 10);
    ASSERT(count >= 3, "至少获取到3条记录");

    for (i = 0; i < 3; i++) {
        ItemMakingSkillInfo_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_get_by_skill_level_range(DBConnectionManager* manager)
{
    TEST_START("按技能等级范围查询");

    ItemMakingSkillInfo record;
    ItemMakingSkillInfo records[10];
    int charac_nos[] = {10031, 10032, 10033, 10034};
    int i;

    memset(&record, 0, sizeof(ItemMakingSkillInfo));
    record.charac_no = 10031;
    record.weapon = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    record.charac_no = 10032;
    record.weapon = 10;
    ItemMakingSkillInfo_Add(manager, &record);

    record.charac_no = 10033;
    record.weapon = 15;
    ItemMakingSkillInfo_Add(manager, &record);

    record.charac_no = 10034;
    record.weapon = 20;
    ItemMakingSkillInfo_Add(manager, &record);

    int count = ItemMakingSkillInfo_GetBySkillLevelRange(manager, MAKING_SKILL_WEAPON, 8, 16, records, 10);
    ASSERT(count >= 2, "查询到等级8-16的记录（至少2条）");

    for (i = 0; i < 4; i++) {
        ItemMakingSkillInfo_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_count_active_users(DBConnectionManager* manager)
{
    TEST_START("统计活跃用户数");

    ItemMakingSkillInfo record;
    int charac_nos[] = {10041, 10042, 10043};
    int i;

    int initial_active = ItemMakingSkillInfo_CountActiveUsers(manager);
    ASSERT(initial_active >= 0, "获取初始活跃用户数成功");

    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10041;
    record.weapon = 5;
    ItemMakingSkillInfo_Add(manager, &record);

    record.charac_no = 10042;
    record.cloth = 3;
    ItemMakingSkillInfo_Add(manager, &record);

    record.charac_no = 10043;
    // 所有技能都是0
    ItemMakingSkillInfo_Add(manager, &record);

    int new_active = ItemMakingSkillInfo_CountActiveUsers(manager);
    ASSERT(new_active >= initial_active + 2, "活跃用户数增加至少2个（有技能的角色）");

    for (i = 0; i < 3; i++) {
        ItemMakingSkillInfo_Delete(manager, charac_nos[i]);
    }

    TEST_PASS();
}

void test_batch_delete(DBConnectionManager* manager)
{
    TEST_START("批量删除");

    ItemMakingSkillInfo record;
    int charac_nos[] = {10051, 10052, 10053, 10054, 10055};
    int count = 5;
    int i;

    memset(&record, 0, sizeof(ItemMakingSkillInfo));
    for (i = 0; i < count; i++) {
        record.charac_no = charac_nos[i];
        record.weapon = (i + 1) * 2;
        ItemMakingSkillInfo_Add(manager, &record);
    }

    int ret = ItemMakingSkillInfo_BatchDelete(manager, charac_nos, count);
    ASSERT(ret == 0, "批量删除成功");

    for (i = 0; i < count; i++) {
        int exists = ItemMakingSkillInfo_Exists(manager, charac_nos[i]);
        ASSERT(exists == 0, "记录已删除");
    }

    TEST_PASS();
}

void test_replace_into_idempotent(DBConnectionManager* manager)
{
    TEST_START("REPLACE INTO 幂等性测试");

    ItemMakingSkillInfo record, retrieved;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10060;
    record.weapon = 10;
    record.cloth = 5;
    int ret = ItemMakingSkillInfo_Add(manager, &record);
    ASSERT(ret == 0, "第一次添加成功");

    record.weapon = 15;
    record.cloth = 8;
    record.leather = 12;
    ret = ItemMakingSkillInfo_Add(manager, &record);
    ASSERT(ret == 0, "第二次添加成功（幂等操作）");

    ItemMakingSkillInfo_Get(manager, 10060, &retrieved);
    ASSERT_EQ(retrieved.weapon, 15, "数据为最新数据（weapon=15）");
    ASSERT_EQ(retrieved.cloth, 8, "数据为最新数据（cloth=8）");
    ASSERT_EQ(retrieved.leather, 12, "数据为最新数据（leather=12）");

    int count_before = ItemMakingSkillInfo_Count(manager);
    record.weapon = 20;
    ItemMakingSkillInfo_Add(manager, &record);
    int count_after = ItemMakingSkillInfo_Count(manager);
    ASSERT_EQ(count_before, count_after, "记录数不变（未重复插入）");

    ItemMakingSkillInfo_Delete(manager, 10060);
    TEST_PASS();
}

void test_all_skill_types(DBConnectionManager* manager)
{
    TEST_START("测试所有11种技能类型");

    ItemMakingSkillInfo record;
    memset(&record, 0, sizeof(ItemMakingSkillInfo));

    record.charac_no = 10070;
    record.weapon = 11;
    record.cloth = 10;
    record.leather = 9;
    record.light_armor = 8;
    record.heavy_armor = 7;
    record.plate = 6;
    record.amulet = 5;
    record.wrist = 4;
    record.ring = 3;
    record.support = 2;
    record.magic_stone = 1;
    ItemMakingSkillInfo_Add(manager, &record);

    int level;
    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_WEAPON);
    ASSERT_EQ(level, 11, "WEAPON技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_CLOTH);
    ASSERT_EQ(level, 10, "CLOTH技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_LEATHER);
    ASSERT_EQ(level, 9, "LEATHER技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_LIGHT_ARMOR);
    ASSERT_EQ(level, 8, "LIGHT_ARMOR技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_HEAVY_ARMOR);
    ASSERT_EQ(level, 7, "HEAVY_ARMOR技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_PLATE);
    ASSERT_EQ(level, 6, "PLATE技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_AMULET);
    ASSERT_EQ(level, 5, "AMULET技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_WRIST);
    ASSERT_EQ(level, 4, "WRIST技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_RING);
    ASSERT_EQ(level, 3, "RING技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_SUPPORT);
    ASSERT_EQ(level, 2, "SUPPORT技能等级正确");

    level = ItemMakingSkillInfo_GetSkillLevel(manager, 10070, MAKING_SKILL_MAGIC_STONE);
    ASSERT_EQ(level, 1, "MAGIC_STONE技能等级正确");

    ItemMakingSkillInfo_Delete(manager, 10070);
    TEST_PASS();
}

// 主函数
int main(int argc, char** argv)
{
    DBConnectionManager manager;
    int ret;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("==========================================\n");
    printf("item_making_skill_info ORM 测试\n");
    printf("==========================================\n");

    memset(&manager, 0, sizeof(DBConnectionManager));
    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    if (ret < 0) {
        printf("Failed to initialize database connection\n");
        return 1;
    }

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    if (ret < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    test_add_and_get(&manager);
    test_update(&manager);
    test_delete(&manager);
    test_exists(&manager);
    test_upgrade_skill(&manager);
    test_set_skill_level(&manager);
    test_get_skill_level(&manager);
    test_reset_all_skills(&manager);
    test_get_max_skill_level(&manager);
    test_count(&manager);
    test_get_all(&manager);
    test_get_by_skill_level_range(&manager);
    test_count_active_users(&manager);
    test_batch_delete(&manager);
    test_replace_into_idempotent(&manager);
    test_all_skill_types(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n==========================================\n");
    printf("测试结果汇总\n");
    printf("==========================================\n");
    printf("测试用例: %d/%d 通过\n", g_test_passed, g_test_count);
    printf("断言: %d/%d 通过\n", g_assert_passed, g_assert_count);
    printf("成功率: %.1f%%\n", (g_test_count > 0) ? (g_test_passed * 100.0 / g_test_count) : 0);
    printf("==========================================\n");

    if (g_test_passed == g_test_count && g_assert_passed == g_assert_count) {
        printf("✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("✗ 部分测试失败\n");
        return 1;
    }
}
