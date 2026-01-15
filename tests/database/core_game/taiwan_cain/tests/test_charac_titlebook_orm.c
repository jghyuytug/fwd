#include "charac_titlebook_orm.h"
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
    TEST_START("添加和获取称号簿");

    CharacTitlebook record, retrieved;
    memset(&record, 0, sizeof(CharacTitlebook));

    record.charac_no = 900001;

    /* 创建测试数据 */
    unsigned char specific_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    unsigned char general_data[] = {0x10, 0x20, 0x30};
    unsigned char despair_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    unsigned char event_data[] = {0xFF, 0xEE};

    record.specific_section = specific_data;
    record.specific_section_len = sizeof(specific_data);
    record.general_section = general_data;
    record.general_section_len = sizeof(general_data);
    record.despair = despair_data;
    record.despair_len = sizeof(despair_data);
    record.event = event_data;
    record.event_len = sizeof(event_data);

    ASSERT_EQ(CharacTitlebook_Add(manager, &record), 0, "添加记录成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, record.charac_no, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_no, record.charac_no, "角色编号匹配");
    ASSERT_EQ(retrieved.specific_section_len, record.specific_section_len, "特定称号区长度匹配");
    ASSERT_EQ(retrieved.general_section_len, record.general_section_len, "通用称号区长度匹配");
    ASSERT_EQ(retrieved.despair_len, record.despair_len, "绝望之塔称号区长度匹配");
    ASSERT_EQ(retrieved.event_len, record.event_len, "活动称号区长度匹配");

    CharacTitlebook_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新称号簿");

    CharacTitlebook record, retrieved;
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &record), 0, "获取已存在记录");

    /* 更新数据 */
    unsigned char new_specific[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    unsigned char new_general[] = {0xAA, 0xBB};
    unsigned char new_despair[] = {0x99, 0x88, 0x77};
    unsigned char new_event[] = {0x01, 0x02, 0x03, 0x04};

    free(record.specific_section);
    free(record.general_section);
    free(record.despair);
    free(record.event);

    record.specific_section = new_specific;
    record.specific_section_len = sizeof(new_specific);
    record.general_section = new_general;
    record.general_section_len = sizeof(new_general);
    record.despair = new_despair;
    record.despair_len = sizeof(new_despair);
    record.event = new_event;
    record.event_len = sizeof(new_event);

    ASSERT_EQ(CharacTitlebook_Update(manager, &record), 0, "更新记录成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.specific_section_len, 6, "特定称号区长度已更新为6");
    ASSERT_EQ(retrieved.general_section_len, 2, "通用称号区长度已更新为2");
    ASSERT_EQ(retrieved.despair_len, 3, "绝望之塔称号区长度已更新为3");
    ASSERT_EQ(retrieved.event_len, 4, "活动称号区长度已更新为4");

    CharacTitlebook_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查称号簿是否存在");

    ASSERT_EQ(CharacTitlebook_Exists(manager, 900001), 1, "记录存在");
    ASSERT_EQ(CharacTitlebook_Exists(manager, 999999), 0, "记录不存在");
}

void test_update_specific_section(DBConnectionManager* manager) {
    TEST_START("更新特定称号区");

    CharacTitlebook retrieved;
    unsigned char new_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22};

    ASSERT_EQ(CharacTitlebook_UpdateSpecificSection(manager, 900001, new_data, sizeof(new_data)), 0,
              "更新特定称号区成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.specific_section_len, 8, "特定称号区长度已更新为8");

    CharacTitlebook_Free(&retrieved);
}

void test_update_general_section(DBConnectionManager* manager) {
    TEST_START("更新通用称号区");

    CharacTitlebook retrieved;
    unsigned char new_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};

    ASSERT_EQ(CharacTitlebook_UpdateGeneralSection(manager, 900001, new_data, sizeof(new_data)), 0,
              "更新通用称号区成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.general_section_len, 5, "通用称号区长度已更新为5");

    CharacTitlebook_Free(&retrieved);
}

void test_update_despair(DBConnectionManager* manager) {
    TEST_START("更新绝望之塔称号区");

    CharacTitlebook retrieved;
    unsigned char new_data[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

    ASSERT_EQ(CharacTitlebook_UpdateDespair(manager, 900001, new_data, sizeof(new_data)), 0,
              "更新绝望之塔称号区成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.despair_len, 6, "绝望之塔称号区长度已更新为6");

    CharacTitlebook_Free(&retrieved);
}

void test_update_event(DBConnectionManager* manager) {
    TEST_START("更新活动称号区");

    CharacTitlebook retrieved;
    unsigned char new_data[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};

    ASSERT_EQ(CharacTitlebook_UpdateEvent(manager, 900001, new_data, sizeof(new_data)), 0,
              "更新活动称号区成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, 900001, &retrieved), 0, "获取记录");
    ASSERT_EQ(retrieved.event_len, 7, "活动称号区长度已更新为7");

    CharacTitlebook_Free(&retrieved);
}

void test_get_total_count(DBConnectionManager* manager) {
    TEST_START("获取总称号簿记录数");

    /* 添加更多记录 */
    CharacTitlebook record2, record3;
    memset(&record2, 0, sizeof(CharacTitlebook));
    memset(&record3, 0, sizeof(CharacTitlebook));

    record2.charac_no = 900002;
    unsigned char data2[] = {0xAA, 0xBB};
    record2.specific_section = data2;
    record2.specific_section_len = sizeof(data2);
    record2.general_section = data2;
    record2.general_section_len = sizeof(data2);
    record2.despair = data2;
    record2.despair_len = sizeof(data2);
    record2.event = data2;
    record2.event_len = sizeof(data2);

    record3.charac_no = 900003;
    unsigned char data3[] = {0xCC, 0xDD};
    record3.specific_section = data3;
    record3.specific_section_len = sizeof(data3);
    record3.general_section = data3;
    record3.general_section_len = sizeof(data3);
    record3.despair = data3;
    record3.despair_len = sizeof(data3);
    record3.event = data3;
    record3.event_len = sizeof(data3);

    CharacTitlebook_Add(manager, &record2);
    CharacTitlebook_Add(manager, &record3);

    int count = CharacTitlebook_GetTotalCount(manager);
    ASSERT_TRUE(count >= 3, "至少有3条记录");
}

void test_minimal_blob(DBConnectionManager* manager) {
    TEST_START("处理最小BLOB数据");

    CharacTitlebook record, retrieved;
    memset(&record, 0, sizeof(CharacTitlebook));

    record.charac_no = 900004;

    /* BLOB字段NOT NULL，使用最小数据（1字节） */
    unsigned char minimal_data[] = {0x00};
    record.specific_section = minimal_data;
    record.specific_section_len = 1;
    record.general_section = minimal_data;
    record.general_section_len = 1;
    record.despair = minimal_data;
    record.despair_len = 1;
    record.event = minimal_data;
    record.event_len = 1;

    ASSERT_EQ(CharacTitlebook_Add(manager, &record), 0, "添加最小BLOB记录成功");
    ASSERT_EQ(CharacTitlebook_Get(manager, record.charac_no, &retrieved), 0, "获取最小BLOB记录成功");
    ASSERT_EQ(retrieved.specific_section_len, 1, "特定称号区长度为1");
    ASSERT_EQ(retrieved.general_section_len, 1, "通用称号区长度为1");
    ASSERT_EQ(retrieved.despair_len, 1, "绝望之塔称号区长度为1");
    ASSERT_EQ(retrieved.event_len, 1, "活动称号区长度为1");

    CharacTitlebook_Free(&retrieved);
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除称号簿");

    ASSERT_EQ(CharacTitlebook_Delete(manager, 900001), 0, "删除记录成功");
    ASSERT_EQ(CharacTitlebook_Exists(manager, 900001), 0, "记录已删除");

    ASSERT_EQ(CharacTitlebook_Delete(manager, 900002), 0, "删除记录成功");
    ASSERT_EQ(CharacTitlebook_Delete(manager, 900003), 0, "删除记录成功");
    ASSERT_EQ(CharacTitlebook_Delete(manager, 900004), 0, "删除记录成功");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印称号簿信息");

    CharacTitlebook record;
    memset(&record, 0, sizeof(CharacTitlebook));

    record.charac_no = 900005;
    unsigned char test_data[] = {0x12, 0x34, 0x56, 0x78};
    record.specific_section = test_data;
    record.specific_section_len = sizeof(test_data);
    record.general_section = test_data;
    record.general_section_len = sizeof(test_data);
    record.despair = test_data;
    record.despair_len = sizeof(test_data);
    record.event = test_data;
    record.event_len = sizeof(test_data);

    CharacTitlebook_Add(manager, &record);

    CharacTitlebook retrieved;
    CharacTitlebook_Get(manager, 900005, &retrieved);

    printf("\n");
    CharacTitlebook_PrintInfo(&retrieved);

    CharacTitlebook_Delete(manager, 900005);
    CharacTitlebook_Free(&retrieved);

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
    printf("角色称号簿 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_update_specific_section(&manager);
    test_update_general_section(&manager);
    test_update_despair(&manager);
    test_update_event(&manager);
    test_get_total_count(&manager);
    test_minimal_blob(&manager);
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
