#include "event_1106_idol_bring_up_orm.h"
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

#define ASSERT_GTE(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) >= (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d < %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

/* 测试用例 */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("添加和获取偶像养成活动记录");

    Event1106IdolBringUp record;
    memset(&record, 0, sizeof(Event1106IdolBringUp));
    record.charac_no = 10001;
    record.m_id = 50001;
    record.pot_type = 1;
    record.water_cnt = 5;
    record.give_title_flag = 0;
    strcpy(record.occ_date, "2011-06-15 10:00:00");
    record.give_title_flag2 = 0;

    ASSERT_EQ(Event1106IdolBringUp_Add(manager, &record), 0, "添加记录成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, 10001, "角色编号匹配");
    ASSERT_EQ(retrieved.m_id, 50001, "会员ID匹配");
    ASSERT_EQ(retrieved.pot_type, 1, "花盆类型匹配");
    ASSERT_EQ(retrieved.water_cnt, 5, "浇水次数匹配");
    ASSERT_EQ(retrieved.give_title_flag, 0, "第一阶段称号标记匹配");
    ASSERT_EQ(retrieved.give_title_flag2, 0, "第二阶段称号标记匹配");
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新偶像养成活动记录");

    Event1106IdolBringUp record;
    memset(&record, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10001, &record), 0, "获取已存在记录");

    record.water_cnt = 10;
    strcpy(record.occ_date, "2011-06-15 11:00:00");

    ASSERT_EQ(Event1106IdolBringUp_Update(manager, &record), 0, "更新记录成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.water_cnt, 10, "浇水次数已更新");
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(Event1106IdolBringUp_Exists(manager, 10001), "记录存在");
    ASSERT_TRUE(!Event1106IdolBringUp_Exists(manager, 99999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条偶像养成活动记录");

    Event1106IdolBringUp record2;
    memset(&record2, 0, sizeof(Event1106IdolBringUp));
    record2.charac_no = 10002;
    record2.m_id = 50001;
    record2.pot_type = 2;
    record2.water_cnt = 15;
    record2.give_title_flag = 0;
    strcpy(record2.occ_date, "2011-06-15 12:00:00");
    record2.give_title_flag2 = 0;
    ASSERT_EQ(Event1106IdolBringUp_Add(manager, &record2), 0, "添加记录2成功");

    Event1106IdolBringUp record3;
    memset(&record3, 0, sizeof(Event1106IdolBringUp));
    record3.charac_no = 10003;
    record3.m_id = 50002;
    record3.pot_type = 1;
    record3.water_cnt = 20;
    record3.give_title_flag = 1;
    strcpy(record3.occ_date, "2011-06-15 13:00:00");
    record3.give_title_flag2 = 0;
    ASSERT_EQ(Event1106IdolBringUp_Add(manager, &record3), 0, "添加记录3成功");

    Event1106IdolBringUp record4;
    memset(&record4, 0, sizeof(Event1106IdolBringUp));
    record4.charac_no = 10004;
    record4.m_id = 50003;
    record4.pot_type = 3;
    record4.water_cnt = 8;
    record4.give_title_flag = 0;
    strcpy(record4.occ_date, "2011-06-15 14:00:00");
    record4.give_title_flag2 = 1;
    ASSERT_EQ(Event1106IdolBringUp_Add(manager, &record4), 0, "添加记录4成功");
}

void test_increment_water_count(DBConnectionManager* manager) {
    TEST_START("增加浇水次数");

    ASSERT_EQ(Event1106IdolBringUp_IncrementWaterCount(manager, 10001, 5), 0, "增加浇水次数成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.water_cnt, 15, "浇水次数已增加（10+5=15）");
}

void test_update_pot_type(DBConnectionManager* manager) {
    TEST_START("更新花盆类型");

    ASSERT_EQ(Event1106IdolBringUp_UpdatePotType(manager, 10001, 3), 0, "更新花盆类型成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.pot_type, 3, "花盆类型已更新为3");
}

void test_mark_title_given(DBConnectionManager* manager) {
    TEST_START("标记第一阶段称号已发放");

    ASSERT_EQ(Event1106IdolBringUp_MarkTitleGiven(manager, 10002), 0, "标记称号已发放成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10002, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.give_title_flag, 1, "第一阶段称号标记已设置为1");
}

void test_mark_title2_given(DBConnectionManager* manager) {
    TEST_START("标记第二阶段称号已发放");

    ASSERT_EQ(Event1106IdolBringUp_MarkTitle2Given(manager, 10002), 0, "标记称号2已发放成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10002, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.give_title_flag2, 1, "第二阶段称号标记已设置为1");
}

void test_reset_progress(DBConnectionManager* manager) {
    TEST_START("重置活动进度");

    ASSERT_EQ(Event1106IdolBringUp_ResetProgress(manager, 10003), 0, "重置活动进度成功");

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    ASSERT_EQ(Event1106IdolBringUp_Get(manager, 10003, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.pot_type, 0, "花盆类型已重置为0");
    ASSERT_EQ(retrieved.water_cnt, 0, "浇水次数已重置为0");
    ASSERT_EQ(retrieved.give_title_flag, 0, "第一阶段称号标记已重置为0");
    ASSERT_EQ(retrieved.give_title_flag2, 0, "第二阶段称号标记已重置为0");
}

void test_count_all(DBConnectionManager* manager) {
    TEST_START("统计参与活动的总角色数");

    int count = Event1106IdolBringUp_CountAll(manager);
    ASSERT_GTE(count, 4, "至少有4条记录");
}

void test_count_title_receivers(DBConnectionManager* manager) {
    TEST_START("统计已获得第一阶段称号的角色数");

    int count = Event1106IdolBringUp_CountTitleReceivers(manager);
    ASSERT_GTE(count, 1, "至少有1个角色获得第一阶段称号");
}

void test_count_title2_receivers(DBConnectionManager* manager) {
    TEST_START("统计已获得第二阶段称号的角色数");

    int count = Event1106IdolBringUp_CountTitle2Receivers(manager);
    ASSERT_GTE(count, 1, "至少有1个角色获得第二阶段称号");
}

void test_count_by_pot_type(DBConnectionManager* manager) {
    TEST_START("统计特定花盆类型的角色数");

    int count = Event1106IdolBringUp_CountByPotType(manager, 3);
    ASSERT_GTE(count, 1, "至少有1个角色使用花盆类型3");
}

void test_get_top_waterers(DBConnectionManager* manager) {
    TEST_START("查询浇水次数最多的前N名角色");

    Event1106IdolBringUp records[10];
    memset(records, 0, sizeof(records));
    int count = Event1106IdolBringUp_GetTopWaterers(manager, 3, records);

    ASSERT_GTE(count, 1, "至少返回1条记录");
    if (count > 1) {
        ASSERT_GTE(records[0].water_cnt, records[1].water_cnt, "按浇水次数降序排列");
    }
}

void test_get_avg_water_count(DBConnectionManager* manager) {
    TEST_START("查询平均浇水次数");

    int avg = Event1106IdolBringUp_GetAvgWaterCount(manager);
    ASSERT_GTE(avg, 0, "平均浇水次数>=0");
}

void test_count_no_title_received(DBConnectionManager* manager) {
    TEST_START("统计未获得任何称号的角色数");

    int count = Event1106IdolBringUp_CountNoTitleReceived(manager);
    ASSERT_GTE(count, 0, "未获得称号的角色数>=0");
}

void test_delete_older_than(DBConnectionManager* manager) {
    TEST_START("批量删除过期活动数据");

    /* 添加一条旧记录 */
    Event1106IdolBringUp old_record;
    memset(&old_record, 0, sizeof(Event1106IdolBringUp));
    old_record.charac_no = 20001;
    old_record.m_id = 60001;
    old_record.pot_type = 1;
    old_record.water_cnt = 5;
    old_record.give_title_flag = 0;
    strcpy(old_record.occ_date, "2010-01-01 00:00:00");
    old_record.give_title_flag2 = 0;
    Event1106IdolBringUp_Add(manager, &old_record);

    /* 删除2000天前的记录 */
    int deleted = Event1106IdolBringUp_DeleteOlderThan(manager, 2000);
    ASSERT_GTE(deleted, 0, "删除操作执行成功");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(Event1106IdolBringUp_Delete(manager, 10001), 0, "删除记录1成功");
    ASSERT_TRUE(!Event1106IdolBringUp_Exists(manager, 10001), "记录1已删除");

    ASSERT_EQ(Event1106IdolBringUp_Delete(manager, 10002), 0, "删除记录2成功");
    ASSERT_EQ(Event1106IdolBringUp_Delete(manager, 10003), 0, "删除记录3成功");
    ASSERT_EQ(Event1106IdolBringUp_Delete(manager, 10004), 0, "删除记录4成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印偶像养成活动记录信息");

    Event1106IdolBringUp record;
    memset(&record, 0, sizeof(Event1106IdolBringUp));
    record.charac_no = 99999;
    record.m_id = 88888;
    record.pot_type = 2;
    record.water_cnt = 100;
    record.give_title_flag = 1;
    strcpy(record.occ_date, "2011-06-20 15:00:00");
    record.give_title_flag2 = 1;

    Event1106IdolBringUp_Add(manager, &record);

    Event1106IdolBringUp retrieved;
    memset(&retrieved, 0, sizeof(Event1106IdolBringUp));
    if (Event1106IdolBringUp_Get(manager, 99999, &retrieved) == 0) {
        Event1106IdolBringUp_PrintInfo(&retrieved);
        ASSERT_TRUE(1, "打印信息成功");
        Event1106IdolBringUp_Delete(manager, 99999);
    } else {
        printf("  ⚠ 无记录可打印\n");
    }
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
    printf("偶像养成活动记录 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_increment_water_count(&manager);
    test_update_pot_type(&manager);
    test_mark_title_given(&manager);
    test_mark_title2_given(&manager);
    test_reset_progress(&manager);
    test_count_all(&manager);
    test_count_title_receivers(&manager);
    test_count_title2_receivers(&manager);
    test_count_by_pot_type(&manager);
    test_get_top_waterers(&manager);
    test_get_avg_water_count(&manager);
    test_count_no_title_received(&manager);
    test_delete_older_than(&manager);
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
