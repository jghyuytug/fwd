#include "charac_option_orm.h"
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
    TEST_START("添加和获取角色选项配置");

    CharacOption record, retrieved;
    memset(&record, 0, sizeof(CharacOption));

    record.charac_no = 700001;
    record.blue_marble_enter_count = 5;
    strcpy(record.charac_inform_notice, "Test notice message");

    /* 添加选项数据 */
    unsigned char options_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    record.options = options_data;
    record.options_len = sizeof(options_data);

    /* 添加最佳通关时间数据 */
    unsigned char time_data[] = {0x10, 0x20, 0x30, 0x40};
    record.best_clear_time = time_data;
    record.best_clear_time_len = sizeof(time_data);

    ASSERT_EQ(CharacOption_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacOption_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.options_len, record.options_len, "选项数据长度匹配");
    ASSERT_EQ(retrieved.best_clear_time_len, record.best_clear_time_len, "最佳通关时间长度匹配");
    ASSERT_EQ(retrieved.blue_marble_enter_count, record.blue_marble_enter_count, "蓝色弹珠进入次数匹配");
    ASSERT_STR_EQ(retrieved.charac_inform_notice, record.charac_inform_notice, "通知信息匹配");

    CharacOption_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色选项配置");

    CharacOption record, retrieved;
    ASSERT_EQ(CharacOption_Get(manager, 700001, &record), 0, "获取已存在记录");

    record.blue_marble_enter_count = 10;
    strcpy(record.charac_inform_notice, "Updated notice");

    /* 更新选项数据 */
    unsigned char new_options[] = {0xAA, 0xBB, 0xCC};
    free(record.options);
    record.options = new_options;
    record.options_len = sizeof(new_options);

    /* 更新最佳通关时间 */
    unsigned char new_time[] = {0x11, 0x22, 0x33, 0x44, 0x55};
    free(record.best_clear_time);
    record.best_clear_time = new_time;
    record.best_clear_time_len = sizeof(new_time);

    ASSERT_EQ(CharacOption_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.blue_marble_enter_count, 10, "蓝色弹珠进入次数已更新");
    ASSERT_STR_EQ(retrieved.charac_inform_notice, "Updated notice", "通知信息已更新");
    ASSERT_EQ(retrieved.options_len, 3, "选项数据长度已更新");
    ASSERT_EQ(retrieved.best_clear_time_len, 5, "最佳通关时间长度已更新");

    CharacOption_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查角色选项配置是否存在");

    ASSERT_EQ(CharacOption_Exists(manager, 700001), 1, "记录存在");
    ASSERT_EQ(CharacOption_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_options(DBConnectionManager* manager) {
    TEST_START("更新选项数据");

    CharacOption retrieved;
    unsigned char new_options[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    ASSERT_EQ(CharacOption_UpdateOptions(manager, 700001, new_options, sizeof(new_options)), 0,
              "更新选项数据成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.options_len, 8, "选项数据长度已更新为8");

    CharacOption_Free(&retrieved);
}

void test_update_best_clear_time(DBConnectionManager* manager) {
    TEST_START("更新最佳通关时间");

    CharacOption retrieved;
    unsigned char new_time[] = {0xFF, 0xEE, 0xDD};

    ASSERT_EQ(CharacOption_UpdateBestClearTime(manager, 700001, new_time, sizeof(new_time)), 0,
              "更新最佳通关时间成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.best_clear_time_len, 3, "最佳通关时间长度已更新为3");

    CharacOption_Free(&retrieved);
}

void test_update_blue_marble_enter_count(DBConnectionManager* manager) {
    TEST_START("更新蓝色弹珠进入次数");

    CharacOption retrieved;

    ASSERT_EQ(CharacOption_UpdateBlueMarbleEnterCount(manager, 700001, 20), 0,
              "更新蓝色弹珠进入次数成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.blue_marble_enter_count, 20, "蓝色弹珠进入次数已更新为20");

    CharacOption_Free(&retrieved);
}

void test_increment_blue_marble_enter_count(DBConnectionManager* manager) {
    TEST_START("增加蓝色弹珠进入次数");

    CharacOption retrieved;
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取当前记录");
    int old_count = retrieved.blue_marble_enter_count;
    CharacOption_Free(&retrieved);

    ASSERT_EQ(CharacOption_IncrementBlueMarbleEnterCount(manager, 700001, 3), 0,
              "增加蓝色弹珠进入次数成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.blue_marble_enter_count, old_count + 3, "蓝色弹珠进入次数增加了3");

    CharacOption_Free(&retrieved);
}

void test_update_inform_notice(DBConnectionManager* manager) {
    TEST_START("更新角色通知信息");

    CharacOption retrieved;

    ASSERT_EQ(CharacOption_UpdateInformNotice(manager, 700001, "New inform notice"), 0,
              "更新通知信息成功");
    ASSERT_EQ(CharacOption_Get(manager, 700001, &retrieved), 0, "获取记录");
    ASSERT_STR_EQ(retrieved.charac_inform_notice, "New inform notice", "通知信息已更新");

    CharacOption_Free(&retrieved);
}

void test_get_total_count(DBConnectionManager* manager) {
    TEST_START("获取总配置记录数");

    /* 添加更多记录 */
    CharacOption record2, record3;
    memset(&record2, 0, sizeof(CharacOption));
    memset(&record3, 0, sizeof(CharacOption));

    record2.charac_no = 700002;
    record2.blue_marble_enter_count = 15;
    strcpy(record2.charac_inform_notice, "Notice 2");
    unsigned char data2[] = {0xAA, 0xBB};
    record2.options = data2;
    record2.options_len = sizeof(data2);
    record2.best_clear_time = data2;
    record2.best_clear_time_len = sizeof(data2);

    record3.charac_no = 700003;
    record3.blue_marble_enter_count = 25;
    strcpy(record3.charac_inform_notice, "Notice 3");
    unsigned char data3[] = {0xCC, 0xDD};
    record3.options = data3;
    record3.options_len = sizeof(data3);
    record3.best_clear_time = data3;
    record3.best_clear_time_len = sizeof(data3);

    CharacOption_Add(manager, &record2);
    CharacOption_Add(manager, &record3);

    int count = CharacOption_GetTotalCount(manager);
    ASSERT_TRUE(count >= 3, "至少有3条记录");
}

void test_get_by_blue_marble_count_range(DBConnectionManager* manager) {
    TEST_START("获取指定蓝色弹珠进入次数范围的角色列表");

    int charac_nos[10];
    int count = CharacOption_GetByBlueMarbleCountRange(manager, 15, 25, charac_nos, 10);

    ASSERT_TRUE(count >= 2, "至少找到2个角色（700002=15, 700003=25）");
}

void test_get_total_blue_marble_enter_count(DBConnectionManager* manager) {
    TEST_START("获取蓝色弹珠总进入次数");

    long long total = CharacOption_GetTotalBlueMarbleEnterCount(manager);
    ASSERT_TRUE(total >= 63, "总进入次数至少为63（23+15+25）");
}

void test_empty_blob(DBConnectionManager* manager) {
    TEST_START("处理最小BLOB数据");

    CharacOption record, retrieved;
    memset(&record, 0, sizeof(CharacOption));

    record.charac_no = 700004;
    record.blue_marble_enter_count = 1;
    strcpy(record.charac_inform_notice, "Minimal blob test");

    /* BLOB字段NOT NULL，使用最小数据（1字节） */
    unsigned char minimal_data[] = {0x00};
    record.options = minimal_data;
    record.options_len = 1;
    record.best_clear_time = minimal_data;
    record.best_clear_time_len = 1;

    ASSERT_EQ(CharacOption_Add(manager, &record), 0, "添加最小BLOB记录成功");
    ASSERT_EQ(CharacOption_Get(manager, record.charac_no, &retrieved), 0, "获取最小BLOB记录成功");
    ASSERT_EQ(retrieved.options_len, 1, "选项数据长度为1");
    ASSERT_EQ(retrieved.best_clear_time_len, 1, "最佳通关时间长度为1");

    CharacOption_Free(&retrieved);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除角色选项配置");

    ASSERT_EQ(CharacOption_Delete(manager, 700001), 0, "删除记录成功");
    ASSERT_EQ(CharacOption_Exists(manager, 700001), 0, "记录已删除");

    ASSERT_EQ(CharacOption_Delete(manager, 700002), 0, "删除记录成功");
    ASSERT_EQ(CharacOption_Delete(manager, 700003), 0, "删除记录成功");
    ASSERT_EQ(CharacOption_Delete(manager, 700004), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色选项配置信息");

    CharacOption record;
    memset(&record, 0, sizeof(CharacOption));
    record.charac_no = 700005;
    record.blue_marble_enter_count = 100;
    strcpy(record.charac_inform_notice, "Print test");

    unsigned char test_data[] = {0x12, 0x34, 0x56};
    record.options = test_data;
    record.options_len = sizeof(test_data);

    CharacOption_Add(manager, &record);

    CharacOption retrieved;
    CharacOption_Get(manager, 700005, &retrieved);

    printf("\n");
    CharacOption_PrintInfo(&retrieved);

    CharacOption_Delete(manager, 700005);
    CharacOption_Free(&retrieved);

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
    printf("角色选项配置 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_options(&manager);
    test_update_best_clear_time(&manager);
    test_update_blue_marble_enter_count(&manager);
    test_increment_blue_marble_enter_count(&manager);
    test_update_inform_notice(&manager);
    test_get_total_count(&manager);
    test_get_by_blue_marble_count_range(&manager);
    test_get_total_blue_marble_enter_count(&manager);
    test_empty_blob(&manager);
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
