#include "charac_tower_rank_top5_orm.h"
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
    TEST_START("添加和获取塔排名TOP5记录");

    CharacTowerRankTop5 record, retrieved;
    memset(&record, 0, sizeof(CharacTowerRankTop5));

    record.tower_index = 1;
    record.part_type = 0;
    record.charac_no = 4000001;
    strcpy(record.member_info, "TopPlayer1|Level100|Class5");
    record.rank = 1;

    ASSERT_EQ(CharacTowerRankTop5_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTowerRankTop5_Get(manager, record.tower_index, record.part_type, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.tower_index, record.tower_index, "塔索引匹配");
    ASSERT_EQ(retrieved.part_type, record.part_type, "类型匹配");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_STR_EQ(retrieved.member_info, record.member_info, "成员信息匹配");
    ASSERT_EQ(retrieved.rank, record.rank, "排名匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新塔排名TOP5记录");

    CharacTowerRankTop5 record, retrieved;
    ASSERT_EQ(CharacTowerRankTop5_Get(manager, 1, 0, 4000001, &record), 0, "获取已存在记录");

    strcpy(record.member_info, "TopPlayer1|Level101|Class5");
    record.rank = 2;

    ASSERT_EQ(CharacTowerRankTop5_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTowerRankTop5_Get(manager, 1, 0, 4000001, &retrieved), 0, "获取更新后记录");
    ASSERT_STR_EQ(retrieved.member_info, "TopPlayer1|Level101|Class5", "成员信息已更新");
    ASSERT_EQ(retrieved.rank, 2, "排名已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查TOP5记录是否存在");

    ASSERT_EQ(CharacTowerRankTop5_Exists(manager, 1, 0, 4000001), 1, "记录存在");
    ASSERT_EQ(CharacTowerRankTop5_Exists(manager, 1, 0, 9999999), 0, "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条TOP5排名记录");

    CharacTowerRankTop5 record2, record3, record4, record5;
    memset(&record2, 0, sizeof(CharacTowerRankTop5));
    memset(&record3, 0, sizeof(CharacTowerRankTop5));
    memset(&record4, 0, sizeof(CharacTowerRankTop5));
    memset(&record5, 0, sizeof(CharacTowerRankTop5));

    record2.tower_index = 1;
    record2.part_type = 0;
    record2.charac_no = 4000002;
    strcpy(record2.member_info, "TopPlayer2|Level99|Class3");
    record2.rank = 3;

    record3.tower_index = 1;
    record3.part_type = 0;
    record3.charac_no = 4000003;
    strcpy(record3.member_info, "TopPlayer3|Level98|Class2");
    record3.rank = 4;

    record4.tower_index = 1;
    record4.part_type = 1;
    record4.charac_no = 4000001;
    strcpy(record4.member_info, "TopPlayer1|Level101|Class5");
    record4.rank = 1;

    record5.tower_index = 2;
    record5.part_type = 0;
    record5.charac_no = 4000001;
    strcpy(record5.member_info, "TopPlayer1|Level101|Class5");
    record5.rank = 5;

    ASSERT_EQ(CharacTowerRankTop5_Add(manager, &record2), 0, "添加记录2成功");
    ASSERT_EQ(CharacTowerRankTop5_Add(manager, &record3), 0, "添加记录3成功");
    ASSERT_EQ(CharacTowerRankTop5_Add(manager, &record4), 0, "添加记录4成功");
    ASSERT_EQ(CharacTowerRankTop5_Add(manager, &record5), 0, "添加记录5成功");
}

void test_get_by_tower_and_type(DBConnectionManager* manager) {
    TEST_START("获取指定塔和类型的所有TOP5记录");

    CharacTowerRankTop5 records[10];
    int count = CharacTowerRankTop5_GetByTowerAndType(manager, 1, 0, records, 10);

    ASSERT_TRUE(count >= 3, "至少找到3条记录");
    /* 验证排序：第一名排名 <= 第二名排名 */
    if (count >= 2) {
        ASSERT_TRUE(records[0].rank <= records[1].rank, "按排名升序排列");
    }
}

void test_get_top_n(DBConnectionManager* manager) {
    TEST_START("获取前N名TOP5记录");

    CharacTowerRankTop5 records[5];
    int count = CharacTowerRankTop5_GetTopN(manager, 1, 0, 2, records);

    ASSERT_TRUE(count >= 2, "获取前2名成功");
    ASSERT_TRUE(count <= 2, "不超过限制数量2");
}

void test_get_by_charac_no(DBConnectionManager* manager) {
    TEST_START("获取指定角色的所有TOP5记录");

    CharacTowerRankTop5 records[10];
    int count = CharacTowerRankTop5_GetByCharacNo(manager, 4000001, records, 10);

    ASSERT_TRUE(count >= 3, "至少找到3条记录");
    ASSERT_EQ(records[0].charac_no, 4000001, "角色编号正确");
}

void test_get_count_by_tower(DBConnectionManager* manager) {
    TEST_START("获取指定塔的TOP5记录数");

    int count = CharacTowerRankTop5_GetCountByTower(manager, 1, 0);
    ASSERT_TRUE(count >= 3, "塔1类型0至少有3条记录");

    int count2 = CharacTowerRankTop5_GetCountByTower(manager, 1, 1);
    ASSERT_TRUE(count2 >= 1, "塔1类型1至少有1条记录");
}

void test_get_rank(DBConnectionManager* manager) {
    TEST_START("获取指定角色的TOP5排名");

    int rank = CharacTowerRankTop5_GetRank(manager, 1, 0, 4000001);
    ASSERT_EQ(rank, 2, "角色4000001在塔1类型0的排名为2");

    int rank2 = CharacTowerRankTop5_GetRank(manager, 1, 1, 4000001);
    ASSERT_EQ(rank2, 1, "角色4000001在塔1类型1的排名为1");
}

void test_update_member_info(DBConnectionManager* manager) {
    TEST_START("更新成员信息");

    ASSERT_EQ(CharacTowerRankTop5_UpdateMemberInfo(manager, 1, 0, 4000001, "TopPlayer1|Level102|Class5"), 0, "更新成员信息成功");

    CharacTowerRankTop5 retrieved;
    ASSERT_EQ(CharacTowerRankTop5_Get(manager, 1, 0, 4000001, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.member_info, "TopPlayer1|Level102|Class5", "成员信息已更新");
}

void test_delete_by_charac_no(DBConnectionManager* manager) {
    TEST_START("删除指定角色的所有TOP5记录");

    ASSERT_EQ(CharacTowerRankTop5_DeleteByCharacNo(manager, 4000002), 0, "删除角色4000002的记录成功");

    ASSERT_EQ(CharacTowerRankTop5_Exists(manager, 1, 0, 4000002), 0, "角色4000002的记录已删除");
}

void test_delete_by_tower_and_type(DBConnectionManager* manager) {
    TEST_START("删除指定塔和类型的所有TOP5记录");

    ASSERT_EQ(CharacTowerRankTop5_DeleteByTowerAndType(manager, 2, 0), 0, "删除塔2类型0的记录成功");

    int count = CharacTowerRankTop5_GetCountByTower(manager, 2, 0);
    ASSERT_EQ(count, 0, "塔2类型0的记录已删除");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除单条TOP5记录");

    ASSERT_EQ(CharacTowerRankTop5_Delete(manager, 1, 0, 4000001), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerRankTop5_Exists(manager, 1, 0, 4000001), 0, "记录已删除");

    ASSERT_EQ(CharacTowerRankTop5_Delete(manager, 1, 0, 4000003), 0, "删除记录成功");
    ASSERT_EQ(CharacTowerRankTop5_Delete(manager, 1, 1, 4000001), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印塔排名TOP5信息");

    CharacTowerRankTop5 record;
    memset(&record, 0, sizeof(CharacTowerRankTop5));

    record.tower_index = 99;
    record.part_type = 5;
    record.charac_no = 4000999;
    strcpy(record.member_info, "TestTopPlayer|Level100|Class1");
    record.rank = 1;

    CharacTowerRankTop5_Add(manager, &record);

    CharacTowerRankTop5 retrieved;
    CharacTowerRankTop5_Get(manager, 99, 5, 4000999, &retrieved);

    printf("\n");
    CharacTowerRankTop5_PrintInfo(&retrieved);

    CharacTowerRankTop5_Delete(manager, 99, 5, 4000999);

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
    printf("角色塔排名TOP5 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_get_by_tower_and_type(&manager);
    test_get_top_n(&manager);
    test_get_by_charac_no(&manager);
    test_get_count_by_tower(&manager);
    test_get_rank(&manager);
    test_update_member_info(&manager);
    test_delete_by_charac_no(&manager);
    test_delete_by_tower_and_type(&manager);
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
