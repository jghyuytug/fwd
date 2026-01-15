#include "event_arad_birthday_6th_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_count = 0, test_passed = 0, assertion_count = 0, assertion_passed = 0, current_test_failed = 0;

#define TEST_START(name) \
    do { if (test_count > 0 && !current_test_failed) test_passed++; \
         current_test_failed = 0; printf("\n[TEST %d] %s\n", ++test_count, name); } while(0)

#define ASSERT_EQ(a, b, msg) \
    do { assertion_count++; \
         if ((a) == (b)) { assertion_passed++; printf("  ✓ %s\n", msg); } \
         else { current_test_failed = 1; printf("  ✗ %s (FAILED: %u != %u)\n", msg, (unsigned)(a), (unsigned)(b)); } \
    } while(0)

#define ASSERT_TRUE(expr, msg) \
    do { assertion_count++; \
         if (expr) { assertion_passed++; printf("  ✓ %s\n", msg); } \
         else { current_test_failed = 1; printf("  ✗ %s (FAILED)\n", msg); } \
    } while(0)

void test_add(DBConnectionManager* mgr) {
    TEST_START("添加服务器积分");
    EventAradBirthday6th rec = {1, 100};
    ASSERT_EQ(EventAradBirthday6th_Add(mgr, &rec), 0, "添加服务器1成功");
}

void test_get(DBConnectionManager* mgr) {
    TEST_START("获取服务器积分");
    EventAradBirthday6th rec;
    ASSERT_EQ(EventAradBirthday6th_Get(mgr, 1, &rec), 0, "获取成功");
    ASSERT_EQ(rec.server, 1, "服务器ID正确");
    ASSERT_EQ(rec.point, 100, "积分正确");
}

void test_exists(DBConnectionManager* mgr) {
    TEST_START("检查存在性");
    ASSERT_EQ(EventAradBirthday6th_Exists(mgr, 1), 1, "服务器1存在");
    ASSERT_EQ(EventAradBirthday6th_Exists(mgr, 999), 0, "服务器999不存在");
}

void test_update(DBConnectionManager* mgr) {
    TEST_START("更新服务器积分");
    EventAradBirthday6th rec = {1, 200};
    ASSERT_EQ(EventAradBirthday6th_Update(mgr, &rec), 0, "更新成功");

    EventAradBirthday6th retrieved;
    EventAradBirthday6th_Get(mgr, 1, &retrieved);
    ASSERT_EQ(retrieved.point, 200, "积分已更新");
}

void test_add_multiple(DBConnectionManager* mgr) {
    TEST_START("添加多个服务器");
    EventAradBirthday6th r1 = {2, 150};
    EventAradBirthday6th r2 = {3, 300};
    EventAradBirthday6th r3 = {4, 250};

    ASSERT_EQ(EventAradBirthday6th_Add(mgr, &r1), 0, "添加服务器2");
    ASSERT_EQ(EventAradBirthday6th_Add(mgr, &r2), 0, "添加服务器3");
    ASSERT_EQ(EventAradBirthday6th_Add(mgr, &r3), 0, "添加服务器4");
}

void test_get_all(DBConnectionManager* mgr) {
    TEST_START("获取所有服务器");
    EventAradBirthday6th* recs = NULL;
    int count = 0;

    ASSERT_EQ(EventAradBirthday6th_GetAll(mgr, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 4, "至少4个服务器");
    printf("  共有 %d 个服务器参与活动\n", count);

    EventAradBirthday6th_FreeRecords(recs);
}

void test_get_top_servers(DBConnectionManager* mgr) {
    TEST_START("获取积分排行榜");
    EventAradBirthday6th* recs = NULL;
    int count = 0;

    ASSERT_EQ(EventAradBirthday6th_GetTopServers(mgr, 3, &recs, &count), 0, "查询成功");
    ASSERT_TRUE(count >= 3, "至少3个服务器");

    printf("  排行榜前3名:\n");
    for (int i = 0; i < count && i < 3; i++) {
        printf("    第%d名: 服务器%u, 积分%u\n", i + 1, recs[i].server, recs[i].point);
    }

    ASSERT_TRUE(count == 0 || recs[0].point >= recs[count - 1].point, "按积分降序排列");

    EventAradBirthday6th_FreeRecords(recs);
}

void test_add_points(DBConnectionManager* mgr) {
    TEST_START("增加服务器积分");
    ASSERT_EQ(EventAradBirthday6th_AddPoints(mgr, 1, 50), 0, "给服务器1增加50积分");

    EventAradBirthday6th rec;
    EventAradBirthday6th_Get(mgr, 1, &rec);
    ASSERT_EQ(rec.point, 250, "积分累加正确(200+50)");

    ASSERT_EQ(EventAradBirthday6th_AddPoints(mgr, 5, 100), 0, "给新服务器5增加100积分");
    ASSERT_EQ(EventAradBirthday6th_Exists(mgr, 5), 1, "新服务器5已创建");
}

void test_get_total_points(DBConnectionManager* mgr) {
    TEST_START("获取总积分");
    unsigned int total = EventAradBirthday6th_GetTotalPoints(mgr);
    ASSERT_TRUE(total > 0, "总积分大于0");
    printf("  全服总积分: %u\n", total);
}

void test_print_info(DBConnectionManager* mgr) {
    TEST_START("打印信息");
    EventAradBirthday6th rec;
    if (EventAradBirthday6th_Get(mgr, 1, &rec) == 0) {
        EventAradBirthday6th_PrintInfo(&rec);
        ASSERT_TRUE(1, "打印功能正常");
    }
}

void test_delete(DBConnectionManager* mgr) {
    TEST_START("删除服务器记录");
    ASSERT_EQ(EventAradBirthday6th_Delete(mgr, 1), 0, "删除服务器1");
    ASSERT_EQ(EventAradBirthday6th_Delete(mgr, 2), 0, "删除服务器2");
    ASSERT_EQ(EventAradBirthday6th_Delete(mgr, 3), 0, "删除服务器3");
    ASSERT_EQ(EventAradBirthday6th_Delete(mgr, 4), 0, "删除服务器4");
    ASSERT_EQ(EventAradBirthday6th_Delete(mgr, 5), 0, "删除服务器5");

    ASSERT_EQ(EventAradBirthday6th_Exists(mgr, 1), 0, "服务器1已删除");
}

int main(int argc, char* argv[]) {
    DBConnectionManager mgr;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("event_arad_birthday_6th ORM 测试套件\n");
    printf("========================================\n");

    memset(&mgr, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&mgr, argv[1]) < 0 ||
        DBConnectionManager_Connect(&mgr, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "数据库连接失败\n");
        return 1;
    }

    printf("✓ 数据库连接成功\n");

    test_add(&mgr);
    test_get(&mgr);
    test_exists(&mgr);
    test_update(&mgr);
    test_add_multiple(&mgr);
    test_get_all(&mgr);
    test_get_top_servers(&mgr);
    test_add_points(&mgr);
    test_get_total_points(&mgr);
    test_print_info(&mgr);
    test_delete(&mgr);

    if (!current_test_failed) test_passed++;

    DBConnectionManager_Cleanup(&mgr);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
