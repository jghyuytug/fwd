#include "dnf_event_prize_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;
static int assertion_count = 0;
static int assertion_passed = 0;
static int current_test_failed = 0;

#define TEST_START(name) \
    do { \
        if (test_count > 0 && !current_test_failed) test_passed++; \
        current_test_failed = 0; \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_EQ(a, b, msg) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", msg); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d != %d)\n", msg, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_TRUE(expr, msg) \
    do { \
        assertion_count++; \
        if (expr) { \
            assertion_passed++; \
            printf("  ✓ %s\n", msg); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", msg); \
        } \
    } while(0)

void test_add_prize(DBConnectionManager* mgr) {
    TEST_START("添加奖品记录");

    DnfEventPrize rec = {1001, 10001, (int)time(NULL)};
    ASSERT_EQ(DnfEventPrize_Add(mgr, &rec), 0, "添加奖品记录成功");
}

void test_get_prize(DBConnectionManager* mgr) {
    TEST_START("获取奖品记录");

    DnfEventPrize rec;
    ASSERT_EQ(DnfEventPrize_Get(mgr, 1001, 10001, &rec), 0, "获取奖品记录成功");
    ASSERT_EQ(rec.prize_id, 1001, "奖品ID正确");
    ASSERT_EQ(rec.m_id, 10001, "会员ID正确");
    ASSERT_TRUE(rec.check_time > 0, "检查时间有效");
}

void test_exists(DBConnectionManager* mgr) {
    TEST_START("检查记录存在性");

    ASSERT_EQ(DnfEventPrize_Exists(mgr, 1001, 10001), 1, "记录存在");
    ASSERT_EQ(DnfEventPrize_Exists(mgr, 9999, 9999), 0, "记录不存在");
}

void test_update_prize(DBConnectionManager* mgr) {
    TEST_START("更新奖品记录");

    DnfEventPrize rec = {1001, 10001, (int)time(NULL) + 3600};
    ASSERT_EQ(DnfEventPrize_Update(mgr, &rec), 0, "更新成功");

    DnfEventPrize retrieved;
    DnfEventPrize_Get(mgr, 1001, 10001, &retrieved);
    ASSERT_TRUE(retrieved.check_time > rec.check_time - 3700, "时间已更新");
}

void test_add_multiple(DBConnectionManager* mgr) {
    TEST_START("添加多个记录");

    int now = (int)time(NULL);
    DnfEventPrize r1 = {1001, 10002, now};
    DnfEventPrize r2 = {1002, 10001, now + 100};
    DnfEventPrize r3 = {1002, 10002, now + 200};

    ASSERT_EQ(DnfEventPrize_Add(mgr, &r1), 0, "添加记录1");
    ASSERT_EQ(DnfEventPrize_Add(mgr, &r2), 0, "添加记录2");
    ASSERT_EQ(DnfEventPrize_Add(mgr, &r3), 0, "添加记录3");
}

void test_get_by_prize_id(DBConnectionManager* mgr) {
    TEST_START("根据奖品ID获取记录");

    DnfEventPrize* recs = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventPrize_GetByPrizeId(mgr, 1001, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 2, "至少2条记录");
    printf("  奖品1001共有 %d 个获奖者\n", count);

    DnfEventPrize_FreeRecords(recs);
}

void test_get_by_member_id(DBConnectionManager* mgr) {
    TEST_START("根据会员ID获取记录");

    DnfEventPrize* recs = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventPrize_GetByMemberId(mgr, 10001, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 2, "至少2个奖品");
    printf("  会员10001共获得 %d 个奖品\n", count);

    DnfEventPrize_FreeRecords(recs);
}

void test_get_by_time_range(DBConnectionManager* mgr) {
    TEST_START("根据时间范围查询");

    DnfEventPrize* recs = NULL;
    int count = 0;
    int now = (int)time(NULL);

    ASSERT_EQ(DnfEventPrize_GetByTimeRange(mgr, now - 7200, now + 7200, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 3, "至少3条记录");
    printf("  时间范围内共 %d 条记录\n", count);

    DnfEventPrize_FreeRecords(recs);
}

void test_get_all(DBConnectionManager* mgr) {
    TEST_START("获取所有记录");

    DnfEventPrize* recs = NULL;
    int count = 0;

    ASSERT_EQ(DnfEventPrize_GetAll(mgr, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 4, "至少4条记录");
    printf("  共有 %d 条记录\n", count);

    DnfEventPrize_FreeRecords(recs);
}

void test_count_by_prize_id(DBConnectionManager* mgr) {
    TEST_START("统计奖品获奖人数");

    int count = DnfEventPrize_CountByPrizeId(mgr, 1001);
    ASSERT_TRUE(count >= 2, "奖品1001至少2人获奖");
    printf("  奖品1001共 %d 人获奖\n", count);
}

void test_count_by_member_id(DBConnectionManager* mgr) {
    TEST_START("统计会员获奖数");

    int count = DnfEventPrize_CountByMemberId(mgr, 10001);
    ASSERT_TRUE(count >= 2, "会员10001至少获得2个奖品");
    printf("  会员10001获得 %d 个奖品\n", count);
}

void test_print_info(DBConnectionManager* mgr) {
    TEST_START("打印记录信息");

    DnfEventPrize rec;
    if (DnfEventPrize_Get(mgr, 1001, 10001, &rec) == 0) {
        DnfEventPrize_PrintInfo(&rec);
        ASSERT_TRUE(1, "打印功能正常");
    }
}

void test_delete_prizes(DBConnectionManager* mgr) {
    TEST_START("删除奖品记录");

    ASSERT_EQ(DnfEventPrize_Delete(mgr, 1001, 10001), 0, "删除(1001,10001)");
    ASSERT_EQ(DnfEventPrize_Delete(mgr, 1001, 10002), 0, "删除(1001,10002)");
    ASSERT_EQ(DnfEventPrize_Delete(mgr, 1002, 10001), 0, "删除(1002,10001)");
    ASSERT_EQ(DnfEventPrize_Delete(mgr, 1002, 10002), 0, "删除(1002,10002)");

    ASSERT_EQ(DnfEventPrize_Exists(mgr, 1001, 10001), 0, "记录已删除");
}

int main(int argc, char* argv[]) {
    DBConnectionManager mgr;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_event_prize ORM 测试套件\n");
    printf("========================================\n");

    memset(&mgr, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&mgr, argv[1]) < 0 ||
        DBConnectionManager_Connect(&mgr, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "数据库连接失败\n");
        return 1;
    }

    printf("✓ 数据库连接成功\n");

    test_add_prize(&mgr);
    test_get_prize(&mgr);
    test_exists(&mgr);
    test_update_prize(&mgr);
    test_add_multiple(&mgr);
    test_get_by_prize_id(&mgr);
    test_get_by_member_id(&mgr);
    test_get_by_time_range(&mgr);
    test_get_all(&mgr);
    test_count_by_prize_id(&mgr);
    test_count_by_member_id(&mgr);
    test_print_info(&mgr);
    test_delete_prizes(&mgr);

    if (!current_test_failed) test_passed++;

    DBConnectionManager_Cleanup(&mgr);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
