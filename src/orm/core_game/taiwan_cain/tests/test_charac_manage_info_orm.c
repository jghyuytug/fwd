#include "charac_manage_info_orm.h"
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
    TEST_START("添加和获取角色管理信息");

    CharacManageInfo record, retrieved;
    memset(&record, 0, sizeof(CharacManageInfo));

    record.charac_no = 800001;
    record.tag_charac_no = 900001;
    record.striker_skill_index = 5;
    record.max_equip_level = 85;

    ASSERT_EQ(CharacManageInfo_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacManageInfo_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.tag_charac_no, record.tag_charac_no, "标记角色编号匹配");
    ASSERT_EQ(retrieved.striker_skill_index, record.striker_skill_index, "打击技能索引匹配");
    ASSERT_EQ(retrieved.max_equip_level, record.max_equip_level, "最大装备等级匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色管理信息");

    CharacManageInfo record, retrieved;
    ASSERT_EQ(CharacManageInfo_Get(manager, 800001, &record), 0, "获取已存在记录");

    record.tag_charac_no = 900002;
    record.striker_skill_index = 10;
    record.max_equip_level = 90;

    ASSERT_EQ(CharacManageInfo_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacManageInfo_Get(manager, 800001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.tag_charac_no, 900002, "标记角色编号已更新");
    ASSERT_EQ(retrieved.striker_skill_index, 10, "打击技能索引已更新");
    ASSERT_EQ(retrieved.max_equip_level, 90, "最大装备等级已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查角色管理信息是否存在");

    ASSERT_EQ(CharacManageInfo_Exists(manager, 800001), 1, "记录存在");
    ASSERT_EQ(CharacManageInfo_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_tag_charac_no(DBConnectionManager* manager) {
    TEST_START("更新标记角色编号");

    CharacManageInfo retrieved;

    ASSERT_EQ(CharacManageInfo_UpdateTagCharacNo(manager, 800001, 900003), 0, "更新标记角色编号成功");
    ASSERT_EQ(CharacManageInfo_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.tag_charac_no, 900003, "标记角色编号已更新为900003");
}

void test_update_striker_skill_index(DBConnectionManager* manager) {
    TEST_START("更新打击技能索引");

    CharacManageInfo retrieved;

    ASSERT_EQ(CharacManageInfo_UpdateStrikerSkillIndex(manager, 800001, 15), 0, "更新打击技能索引成功");
    ASSERT_EQ(CharacManageInfo_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.striker_skill_index, 15, "打击技能索引已更新为15");
}

void test_update_max_equip_level(DBConnectionManager* manager) {
    TEST_START("更新最大装备等级");

    CharacManageInfo retrieved;

    ASSERT_EQ(CharacManageInfo_UpdateMaxEquipLevel(manager, 800001, 95), 0, "更新最大装备等级成功");
    ASSERT_EQ(CharacManageInfo_Get(manager, 800001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.max_equip_level, 95, "最大装备等级已更新为95");
}

void test_get_by_tag_charac_no(DBConnectionManager* manager) {
    TEST_START("获取指定标记角色的所有角色");

    /* 添加多个角色使用相同标记 */
    CharacManageInfo record2, record3;
    memset(&record2, 0, sizeof(CharacManageInfo));
    memset(&record3, 0, sizeof(CharacManageInfo));

    record2.charac_no = 800002;
    record2.tag_charac_no = 900003;
    record2.striker_skill_index = 1;
    record2.max_equip_level = 70;

    record3.charac_no = 800003;
    record3.tag_charac_no = 900003;
    record3.striker_skill_index = 2;
    record3.max_equip_level = 75;

    CharacManageInfo_Add(manager, &record2);
    CharacManageInfo_Add(manager, &record3);

    unsigned int charac_nos[10];
    int count = CharacManageInfo_GetByTagCharacNo(manager, 900003, charac_nos, 10);

    ASSERT_TRUE(count >= 3, "至少找到3个角色（800001, 800002, 800003）");
}

void test_get_count_by_equip_level_range(DBConnectionManager* manager) {
    TEST_START("获取指定装备等级范围的角色数量");

    int count = CharacManageInfo_GetCountByEquipLevelRange(manager, 70, 95);
    ASSERT_TRUE(count >= 3, "装备等级70-95范围内至少有3个角色");

    count = CharacManageInfo_GetCountByEquipLevelRange(manager, 100, 200);
    ASSERT_EQ(count, 0, "装备等级100-200范围内没有角色");
}

void test_get_max_equip_level(DBConnectionManager* manager) {
    TEST_START("获取最高装备等级");

    unsigned short max_level = CharacManageInfo_GetMaxEquipLevel(manager);
    ASSERT_TRUE(max_level >= 95, "最高装备等级至少为95");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除角色管理信息");

    ASSERT_EQ(CharacManageInfo_Delete(manager, 800001), 0, "删除记录成功");
    ASSERT_EQ(CharacManageInfo_Exists(manager, 800001), 0, "记录已删除");

    ASSERT_EQ(CharacManageInfo_Delete(manager, 800002), 0, "删除记录成功");
    ASSERT_EQ(CharacManageInfo_Delete(manager, 800003), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色管理信息");

    CharacManageInfo record;
    memset(&record, 0, sizeof(CharacManageInfo));
    record.charac_no = 800004;
    record.tag_charac_no = 900004;
    record.striker_skill_index = 20;
    record.max_equip_level = 100;

    CharacManageInfo_Add(manager, &record);

    printf("\n");
    CharacManageInfo_PrintInfo(&record);

    CharacManageInfo_Delete(manager, 800004);

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
    printf("角色管理信息 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_tag_charac_no(&manager);
    test_update_striker_skill_index(&manager);
    test_update_max_equip_level(&manager);
    test_get_by_tag_charac_no(&manager);
    test_get_count_by_equip_level_range(&manager);
    test_get_max_equip_level(&manager);
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
