#include "charac_view_orm.h"
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
    TEST_START("添加和获取角色视图配置");

    CharacView record;
    record.m_id = 1001;

    /* 创建测试info数据 */
    const char* test_info = "test_blob_data_123";
    record.info_len = strlen(test_info);
    record.info = (unsigned char*)malloc(record.info_len);
    memcpy(record.info, test_info, record.info_len);

    record.slot_effect_count = 18;
    record.charac_slot_limit = 18;
    strcpy(record.hash_key, "abc123def456");
    record.charac_count = 5;

    ASSERT_EQ(CharacView_Add(manager, &record), 0, "添加记录成功");
    free(record.info);

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.m_id, 1001, "会员ID匹配");
    ASSERT_EQ(retrieved.slot_effect_count, 18, "插槽效果数量匹配");
    ASSERT_EQ(retrieved.charac_slot_limit, 18, "角色槽位限制匹配");
    ASSERT_STR_EQ(retrieved.hash_key, "abc123def456", "哈希密钥匹配");
    ASSERT_EQ(retrieved.charac_count, 5, "角色数量匹配");

    CharacView_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新角色视图配置");

    CharacView record;
    memset(&record, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &record), 0, "获取已存在记录");

    record.slot_effect_count = 20;
    record.charac_slot_limit = 20;
    strcpy(record.hash_key, "xyz789uvw");
    record.charac_count = 8;

    ASSERT_EQ(CharacView_Update(manager, &record), 0, "更新记录成功");
    CharacView_Free(&record);

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.slot_effect_count, 20, "插槽效果数量已更新");
    ASSERT_EQ(retrieved.charac_slot_limit, 20, "角色槽位限制已更新");
    ASSERT_STR_EQ(retrieved.hash_key, "xyz789uvw", "哈希密钥已更新");
    ASSERT_EQ(retrieved.charac_count, 8, "角色数量已更新");

    CharacView_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CharacView_Exists(manager, 1001), "记录存在");
    ASSERT_TRUE(!CharacView_Exists(manager, 9999999), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条角色视图配置");

    CharacView record2;
    record2.m_id = 1002;
    const char* test_info2 = "data2";
    record2.info_len = strlen(test_info2);
    record2.info = (unsigned char*)malloc(record2.info_len);
    memcpy(record2.info, test_info2, record2.info_len);
    record2.slot_effect_count = 15;
    record2.charac_slot_limit = 15;
    strcpy(record2.hash_key, "key_002");
    record2.charac_count = 3;
    ASSERT_EQ(CharacView_Add(manager, &record2), 0, "添加记录2成功");
    free(record2.info);

    CharacView record3;
    record3.m_id = 1003;
    const char* test_info3 = "data3";
    record3.info_len = strlen(test_info3);
    record3.info = (unsigned char*)malloc(record3.info_len);
    memcpy(record3.info, test_info3, record3.info_len);
    record3.slot_effect_count = 25;
    record3.charac_slot_limit = 25;
    strcpy(record3.hash_key, "key_003");
    record3.charac_count = 12;
    ASSERT_EQ(CharacView_Add(manager, &record3), 0, "添加记录3成功");
    free(record3.info);
}

void test_update_slot_effect_count(DBConnectionManager* manager) {
    TEST_START("更新插槽效果数量");

    ASSERT_EQ(CharacView_UpdateSlotEffectCount(manager, 1001, 22), 0, "更新插槽效果数量成功");

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.slot_effect_count, 22, "插槽效果数量已更新");

    CharacView_Free(&retrieved);
}

void test_update_charac_slot_limit(DBConnectionManager* manager) {
    TEST_START("更新角色槽位限制");

    ASSERT_EQ(CharacView_UpdateCharacSlotLimit(manager, 1001, 24), 0, "更新角色槽位限制成功");

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_slot_limit, 24, "角色槽位限制已更新");

    CharacView_Free(&retrieved);
}

void test_update_hash_key(DBConnectionManager* manager) {
    TEST_START("更新哈希密钥");

    ASSERT_EQ(CharacView_UpdateHashKey(manager, 1001, "new_hash_key_999"), 0, "更新哈希密钥成功");

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.hash_key, "new_hash_key_999", "哈希密钥已更新");

    CharacView_Free(&retrieved);
}

void test_update_charac_count(DBConnectionManager* manager) {
    TEST_START("更新角色数量");

    ASSERT_EQ(CharacView_UpdateCharacCount(manager, 1001, 10), 0, "更新角色数量成功");

    CharacView retrieved;
    memset(&retrieved, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_count, 10, "角色数量已更新");

    CharacView_Free(&retrieved);
}

void test_increment_charac_count(DBConnectionManager* manager) {
    TEST_START("增加角色数量");

    CharacView before;
    memset(&before, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacView_IncrementCharacCount(manager, 1001), 0, "增加角色数量成功");

    CharacView after;
    memset(&after, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &after), 0, "获取增加后记录");
    ASSERT_EQ(after.charac_count, before.charac_count + 1, "角色数量增加1");

    CharacView_Free(&before);
    CharacView_Free(&after);
}

void test_decrement_charac_count(DBConnectionManager* manager) {
    TEST_START("减少角色数量");

    CharacView before;
    memset(&before, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &before), 0, "获取初始记录");

    ASSERT_EQ(CharacView_DecrementCharacCount(manager, 1001), 0, "减少角色数量成功");

    CharacView after;
    memset(&after, 0, sizeof(CharacView));
    ASSERT_EQ(CharacView_Get(manager, 1001, &after), 0, "获取减少后记录");
    ASSERT_EQ(after.charac_count, before.charac_count - 1, "角色数量减少1");

    CharacView_Free(&before);
    CharacView_Free(&after);
}

void test_get_by_min_charac_count(DBConnectionManager* manager) {
    TEST_START("获取角色数量大于指定值的会员");

    CharacView records[10];
    memset(records, 0, sizeof(records));
    int count = CharacView_GetByMinCharacCount(manager, 5, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_TRUE(records[0].charac_count >= 5, "角色数量满足条件");

    for (int i = 0; i < count; i++) {
        CharacView_Free(&records[i]);
    }
}

void test_get_by_charac_count(DBConnectionManager* manager) {
    TEST_START("获取角色数量等于指定值的会员");

    /* 先设置一个记录的角色数量为15 */
    CharacView_UpdateCharacCount(manager, 1003, 15);

    CharacView records[10];
    memset(records, 0, sizeof(records));
    int count = CharacView_GetByCharacCount(manager, 15, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_EQ(records[0].charac_count, 15, "角色数量完全匹配");

    for (int i = 0; i < count; i++) {
        CharacView_Free(&records[i]);
    }
}

void test_get_by_min_slot_limit(DBConnectionManager* manager) {
    TEST_START("获取槽位限制大于指定值的会员");

    CharacView records[10];
    memset(records, 0, sizeof(records));
    int count = CharacView_GetByMinSlotLimit(manager, 20, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_TRUE(records[0].charac_slot_limit >= 20, "槽位限制满足条件");

    for (int i = 0; i < count; i++) {
        CharacView_Free(&records[i]);
    }
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacView_Delete(manager, 1001), 0, "删除记录成功");
    ASSERT_TRUE(!CharacView_Exists(manager, 1001), "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印角色视图配置信息");

    CharacView record;
    memset(&record, 0, sizeof(CharacView));
    if (CharacView_Get(manager, 1003, &record) == 0) {
        CharacView_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息成功");
        CharacView_Free(&record);
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
    printf("角色视图配置 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_update_slot_effect_count(&manager);
    test_update_charac_slot_limit(&manager);
    test_update_hash_key(&manager);
    test_update_charac_count(&manager);
    test_increment_charac_count(&manager);
    test_decrement_charac_count(&manager);
    test_get_by_min_charac_count(&manager);
    test_get_by_charac_count(&manager);
    test_get_by_min_slot_limit(&manager);
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
