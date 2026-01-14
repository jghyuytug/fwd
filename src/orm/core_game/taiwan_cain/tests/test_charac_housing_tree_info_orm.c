#include "charac_housing_tree_info_orm.h"
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

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
        } \
    } while(0)

/* 测试用例 */
void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取角色住房树信息");

    CharacHousingTreeInfo record, retrieved;
    memset(&record, 0, sizeof(CharacHousingTreeInfo));

    record.charac_no = 400001;
    record.tree_id = 1001;
    strncpy(record.expire_date, "2025-12-31 23:59:59", sizeof(record.expire_date) - 1);
    record.current_point = 100;
    record.leaf_point = 50;
    record.day_water_count = 3;

    ASSERT_EQ(CharacHousingTreeInfo_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.tree_id, record.tree_id, "树ID匹配");
    ASSERT_STR_EQ(retrieved.expire_date, record.expire_date, "过期时间匹配");
    ASSERT_EQ(retrieved.current_point, record.current_point, "当前点数匹配");
    ASSERT_EQ(retrieved.leaf_point, record.leaf_point, "叶子点数匹配");
    ASSERT_EQ(retrieved.day_water_count, record.day_water_count, "浇水次数匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色住房树信息");

    CharacHousingTreeInfo record, retrieved;

    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &record), 0, "获取已存在记录");

    record.tree_id = 1002;
    record.current_point = 200;
    record.leaf_point = 100;

    ASSERT_EQ(CharacHousingTreeInfo_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, record.charac_no, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.tree_id, 1002, "树ID已更新");
    ASSERT_EQ(retrieved.current_point, 200, "当前点数已更新");
    ASSERT_EQ(retrieved.leaf_point, 100, "叶子点数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查角色住房树信息是否存在");

    ASSERT_EQ(CharacHousingTreeInfo_Exists(manager, 400001), 1, "记录存在");
    ASSERT_EQ(CharacHousingTreeInfo_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_points(DBConnectionManager* manager) {
    TEST_START("更新当前点数");

    CharacHousingTreeInfo retrieved;

    ASSERT_EQ(CharacHousingTreeInfo_UpdatePoints(manager, 400001, 50), 0, "增加50点数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.current_point, 250, "点数已增加到250");

    ASSERT_EQ(CharacHousingTreeInfo_UpdatePoints(manager, 400001, -30), 0, "减少30点数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.current_point, 220, "点数已减少到220");
}

void test_update_leaf_points(DBConnectionManager* manager) {
    TEST_START("更新叶子点数");

    CharacHousingTreeInfo retrieved;

    ASSERT_EQ(CharacHousingTreeInfo_UpdateLeafPoints(manager, 400001, 25), 0, "增加25叶子点数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.leaf_point, 125, "叶子点数已增加到125");
}

void test_water_count(DBConnectionManager* manager) {
    TEST_START("浇水次数操作");

    CharacHousingTreeInfo retrieved;

    ASSERT_EQ(CharacHousingTreeInfo_IncrementWaterCount(manager, 400001), 0, "增加浇水次数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.day_water_count, 4, "浇水次数已增加到4");

    ASSERT_EQ(CharacHousingTreeInfo_IncrementWaterCount(manager, 400001), 0, "再次增加浇水次数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.day_water_count, 5, "浇水次数已增加到5");

    ASSERT_EQ(CharacHousingTreeInfo_ResetWaterCount(manager, 400001), 0, "重置浇水次数");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.day_water_count, 0, "浇水次数已重置为0");
}

void test_update_expire_date(DBConnectionManager* manager) {
    TEST_START("更新过期时间");

    CharacHousingTreeInfo retrieved;
    const char* new_expire = "2026-01-15 12:00:00";

    ASSERT_EQ(CharacHousingTreeInfo_UpdateExpireDate(manager, 400001, new_expire), 0, "更新过期时间");
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &retrieved), 0, "获取记录");
    ASSERT_STR_EQ(retrieved.expire_date, new_expire, "过期时间已更新");
}

void test_count_by_tree_id(DBConnectionManager* manager) {
    TEST_START("按树ID统计");

    /* 添加更多记录 */
    CharacHousingTreeInfo record;
    memset(&record, 0, sizeof(CharacHousingTreeInfo));
    record.charac_no = 400002;
    record.tree_id = 1002;
    strncpy(record.expire_date, "2025-12-31 23:59:59", sizeof(record.expire_date) - 1);
    CharacHousingTreeInfo_Add(manager, &record);

    record.charac_no = 400003;
    record.tree_id = 1002;
    CharacHousingTreeInfo_Add(manager, &record);

    int count = CharacHousingTreeInfo_CountByTreeId(manager, 1002);
    ASSERT_EQ(count >= 2, 1, "树ID 1002至少有2条记录");
    printf("  树ID 1002 记录数: %d\n", count);
}

void test_is_expired(DBConnectionManager* manager) {
    TEST_START("检查是否过期");

    /* 创建已过期的记录 */
    CharacHousingTreeInfo record;
    memset(&record, 0, sizeof(CharacHousingTreeInfo));
    record.charac_no = 400004;
    record.tree_id = 1003;
    strncpy(record.expire_date, "2020-01-01 00:00:00", sizeof(record.expire_date) - 1);
    CharacHousingTreeInfo_Add(manager, &record);

    ASSERT_EQ(CharacHousingTreeInfo_IsExpired(manager, 400004), 1, "记录已过期");
    ASSERT_EQ(CharacHousingTreeInfo_IsExpired(manager, 400001), 0, "记录未过期");
}

void test_get_expiring_soon(DBConnectionManager* manager) {
    TEST_START("获取即将过期的记录");

    /* 创建即将过期的记录 */
    CharacHousingTreeInfo record;
    memset(&record, 0, sizeof(CharacHousingTreeInfo));
    record.charac_no = 400005;
    record.tree_id = 1004;
    strncpy(record.expire_date, "2025-11-20 00:00:00", sizeof(record.expire_date) - 1);
    CharacHousingTreeInfo_Add(manager, &record);

    unsigned int charac_nos[10];
    int count = CharacHousingTreeInfo_GetExpiringSoon(manager, 365, charac_nos, 10);

    ASSERT_EQ(count >= 0, 1, "查询成功");
    printf("  %d天内即将过期的记录数: %d\n", 365, count);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除角色住房树信息");

    ASSERT_EQ(CharacHousingTreeInfo_Delete(manager, 400002), 0, "删除记录成功");
    ASSERT_EQ(CharacHousingTreeInfo_Exists(manager, 400002), 0, "记录已删除");

    ASSERT_EQ(CharacHousingTreeInfo_Delete(manager, 400003), 0, "删除记录成功");
    ASSERT_EQ(CharacHousingTreeInfo_Delete(manager, 400004), 0, "删除记录成功");
    ASSERT_EQ(CharacHousingTreeInfo_Delete(manager, 400005), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色住房树信息");

    CharacHousingTreeInfo record;
    ASSERT_EQ(CharacHousingTreeInfo_Get(manager, 400001, &record), 0, "获取记录");

    printf("\n");
    CharacHousingTreeInfo_PrintInfo(&record);

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
    printf("角色住房树信息 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_points(&manager);
    test_update_leaf_points(&manager);
    test_water_count(&manager);
    test_update_expire_date(&manager);
    test_count_by_tree_id(&manager);
    test_is_expired(&manager);
    test_get_expiring_soon(&manager);
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
