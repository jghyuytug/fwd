/**
 * 升级支援活动信息 ORM 单元测试
 *
 * 编译:
 * gcc -m32 -std=c99 -I../include/database/headers \
 *     -I/usr/include/mysql \
 *     -L/usr/lib/mysql \
 *     ../src/database/db_connection_manager.c \
 *     ../src/database/event_levelup_support_orm.c \
 *     test_event_levelup_support_orm.c \
 *     -o test_event_levelup_support_orm \
 *     -lmysqlclient -lpthread
 *
 * 运行:
 * ./test_event_levelup_support_orm ../config/database.cfg
 */

#include "../include/database/headers/event_levelup_support_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 测试统计 */
static int tests_passed = 0;
static int tests_failed = 0;
static int assertions_passed = 0;
static int assertions_failed = 0;

#define ASSERT(cond) do { \
    if (cond) { \
        assertions_passed++; \
        printf("  ✓ %s\n", #cond); \
    } else { \
        assertions_failed++; \
        printf("  ✗ %s\n", #cond); \
        return 0; \
    } \
} while (0)

#define RUN_TEST(test_name, test_func) do { \
    printf("\n[TEST %d] %s\n", tests_passed + tests_failed + 1, test_name); \
    if (test_func(&manager)) { \
        tests_passed++; \
    } else { \
        tests_failed++; \
    } \
} while (0)

/* 测试用例 */

int test_add_and_get(DBConnectionManager* manager) {
    EventLevelupSupport record = {0};
    record.charac_no = 1001;
    record.level = 50;
    record.state = 1;

    ASSERT(EventLevelupSupport_Add(manager, &record) == 0);

    EventLevelupSupport retrieved = {0};
    ASSERT(EventLevelupSupport_Get(manager, 1001, 50, &retrieved) == 0);
    ASSERT(retrieved.charac_no == 1001);
    ASSERT(retrieved.level == 50);
    ASSERT(retrieved.state == 1);

    return 1;
}

int test_update(DBConnectionManager* manager) {
    EventLevelupSupport record = {0};
    ASSERT(EventLevelupSupport_Get(manager, 1001, 50, &record) == 0);

    record.state = 2;
    ASSERT(EventLevelupSupport_Update(manager, &record) == 0);

    EventLevelupSupport updated = {0};
    ASSERT(EventLevelupSupport_Get(manager, 1001, 50, &updated) == 0);
    ASSERT(updated.state == 2);

    return 1;
}

int test_exists(DBConnectionManager* manager) {
    ASSERT(EventLevelupSupport_Exists(manager, 1001, 50) == 1);
    ASSERT(EventLevelupSupport_Exists(manager, 9999, 999) == 0);

    return 1;
}

int test_add_multiple(DBConnectionManager* manager) {
    EventLevelupSupport records[] = {
        {1001, 55, 0},
        {1001, 60, 1},
        {1002, 50, 0},
        {1002, 55, 1}
    };

    for (int i = 0; i < 4; i++) {
        ASSERT(EventLevelupSupport_Add(manager, &records[i]) == 0);
    }

    return 1;
}

int test_update_state(DBConnectionManager* manager) {
    ASSERT(EventLevelupSupport_UpdateState(manager, 1001, 55, 2) == 0);

    EventLevelupSupport record = {0};
    ASSERT(EventLevelupSupport_Get(manager, 1001, 55, &record) == 0);
    ASSERT(record.state == 2);

    return 1;
}

int test_get_by_charac(DBConnectionManager* manager) {
    EventLevelupSupport records[10] = {0};
    int count = EventLevelupSupport_GetByCharac(manager, 1001, records, 10);
    ASSERT(count >= 3);  // 至少有 50, 55, 60 三个等级
    ASSERT(records[0].charac_no == 1001);
    // 验证按 level 排序
    if (count > 1) {
        ASSERT(records[0].level <= records[1].level);
    }

    return 1;
}

int test_get_by_level(DBConnectionManager* manager) {
    EventLevelupSupport records[10] = {0};
    int count = EventLevelupSupport_GetByLevel(manager, 50, records, 10);
    ASSERT(count >= 2);  // 至少 1001 和 1002
    ASSERT(records[0].level == 50);
    // 验证按 charac_no 排序
    if (count > 1) {
        ASSERT(records[0].charac_no <= records[1].charac_no);
    }

    return 1;
}

int test_get_by_state(DBConnectionManager* manager) {
    EventLevelupSupport records[10] = {0};
    int count = EventLevelupSupport_GetByState(manager, 1, records, 10);
    ASSERT(count >= 1);
    ASSERT(records[0].state == 1);

    return 1;
}

int test_count_by_charac(DBConnectionManager* manager) {
    int count = EventLevelupSupport_CountByCharac(manager, 1001);
    ASSERT(count >= 3);  // 50, 55, 60

    count = EventLevelupSupport_CountByCharac(manager, 1002);
    ASSERT(count >= 2);  // 50, 55

    return 1;
}

int test_delete_by_charac(DBConnectionManager* manager) {
    ASSERT(EventLevelupSupport_DeleteByCharac(manager, 1002) == 0);

    int count = EventLevelupSupport_CountByCharac(manager, 1002);
    ASSERT(count == 0);

    return 1;
}

int test_delete(DBConnectionManager* manager) {
    ASSERT(EventLevelupSupport_Delete(manager, 1001, 50) == 0);
    ASSERT(EventLevelupSupport_Exists(manager, 1001, 50) == 0);

    ASSERT(EventLevelupSupport_Delete(manager, 1001, 55) == 0);
    ASSERT(EventLevelupSupport_Delete(manager, 1001, 60) == 0);

    return 1;
}

int test_replace_into_idempotent(DBConnectionManager* manager) {
    EventLevelupSupport record = {2001, 50, 1};
    ASSERT(EventLevelupSupport_Add(manager, &record) == 0);

    record.state = 2;
    ASSERT(EventLevelupSupport_Add(manager, &record) == 0);

    EventLevelupSupport retrieved = {0};
    ASSERT(EventLevelupSupport_Get(manager, 2001, 50, &retrieved) == 0);
    ASSERT(retrieved.state == 2);

    int count = EventLevelupSupport_CountByCharac(manager, 2001);
    ASSERT(count == 1);  // 应该只有1条记录，不是2条

    ASSERT(EventLevelupSupport_Delete(manager, 2001, 50) == 0);

    return 1;
}

int test_print_info(DBConnectionManager* manager) {
    EventLevelupSupport record = {99999, 99, 99};
    EventLevelupSupport_Add(manager, &record);

    EventLevelupSupport retrieved = {0};
    ASSERT(EventLevelupSupport_Get(manager, 99999, 99, &retrieved) == 0);

    EventLevelupSupport_PrintInfo(&retrieved);
    EventLevelupSupport_Delete(manager, 99999, 99);

    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        fprintf(stderr, "示例: %s ../config/database.cfg\n", argv[0]);
        return 1;
    }

    DBConnectionManager manager;
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
    printf("升级支援活动信息 ORM 测试\n");
    printf("========================================\n");

    RUN_TEST("添加和获取升级支援活动信息", test_add_and_get);
    RUN_TEST("更新升级支援活动信息", test_update);
    RUN_TEST("检查记录是否存在", test_exists);
    RUN_TEST("添加多条升级支援活动信息", test_add_multiple);
    RUN_TEST("更新状态", test_update_state);
    RUN_TEST("按角色查询", test_get_by_charac);
    RUN_TEST("按等级查询", test_get_by_level);
    RUN_TEST("按状态查询", test_get_by_state);
    RUN_TEST("统计角色记录数", test_count_by_charac);
    RUN_TEST("批量删除角色记录", test_delete_by_charac);
    RUN_TEST("删除记录", test_delete);
    RUN_TEST("REPLACE INTO 幂等性测试", test_replace_into_idempotent);
    RUN_TEST("打印升级支援活动信息", test_print_info);

    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", tests_passed, tests_passed + tests_failed);
    printf("断言: %d/%d 通过\n", assertions_passed, assertions_passed + assertions_failed);
    printf("========================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
