#include "charac_view_act8_orm.h"
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

#define ASSERT_EQ_ULL(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) == (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %llu != %llu)\n", message, (unsigned long long)(a), (unsigned long long)(b)); \
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
    TEST_START("添加和获取Act8角色视图配置");

    CharacViewAct8 record;
    record.m_id = 1000001ULL;

    /* 创建测试info数据 */
    const char* test_info = "act8_test_data_123";
    record.info_len = strlen(test_info);
    record.info = (unsigned char*)malloc(record.info_len);
    memcpy(record.info, test_info, record.info_len);

    record.slot_effect_count = 18;
    record.charac_slot_limit = 18;
    strcpy(record.hash_key, "act8_hash_abc123");

    ASSERT_EQ(CharacViewAct8_Add(manager, &record), 0, "添加记录成功");
    free(record.info);

    CharacViewAct8 retrieved;
    memset(&retrieved, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &retrieved), 0, "获取记录成功");
    ASSERT_EQ_ULL(retrieved.m_id, 1000001ULL, "会员ID匹配");
    ASSERT_EQ(retrieved.slot_effect_count, 18, "插槽效果数量匹配");
    ASSERT_EQ(retrieved.charac_slot_limit, 18, "角色槽位限制匹配");
    ASSERT_STR_EQ(retrieved.hash_key, "act8_hash_abc123", "哈希密钥匹配");

    CharacViewAct8_Free(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("更新Act8角色视图配置");

    CharacViewAct8 record;
    memset(&record, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &record), 0, "获取已存在记录");

    record.slot_effect_count = 20;
    record.charac_slot_limit = 20;
    strcpy(record.hash_key, "act8_new_hash");

    ASSERT_EQ(CharacViewAct8_Update(manager, &record), 0, "更新记录成功");
    CharacViewAct8_Free(&record);

    CharacViewAct8 retrieved;
    memset(&retrieved, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &retrieved), 0, "获取更新后记录");
    ASSERT_EQ(retrieved.slot_effect_count, 20, "插槽效果数量已更新");
    ASSERT_EQ(retrieved.charac_slot_limit, 20, "角色槽位限制已更新");
    ASSERT_STR_EQ(retrieved.hash_key, "act8_new_hash", "哈希密钥已更新");

    CharacViewAct8_Free(&retrieved);
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("检查记录是否存在");

    ASSERT_TRUE(CharacViewAct8_Exists(manager, 1000001ULL), "记录存在");
    ASSERT_TRUE(!CharacViewAct8_Exists(manager, 9999999999ULL), "记录不存在");
}

void test_add_multiple_records(DBConnectionManager* manager) {
    TEST_START("添加多条Act8角色视图配置");

    CharacViewAct8 record2;
    record2.m_id = 1000002ULL;
    const char* test_info2 = "data2_act8";
    record2.info_len = strlen(test_info2);
    record2.info = (unsigned char*)malloc(record2.info_len);
    memcpy(record2.info, test_info2, record2.info_len);
    record2.slot_effect_count = 16;
    record2.charac_slot_limit = 16;
    strcpy(record2.hash_key, "key_002_act8");
    ASSERT_EQ(CharacViewAct8_Add(manager, &record2), 0, "添加记录2成功");
    free(record2.info);

    CharacViewAct8 record3;
    record3.m_id = 1000003ULL;
    const char* test_info3 = "data3_act8";
    record3.info_len = strlen(test_info3);
    record3.info = (unsigned char*)malloc(record3.info_len);
    memcpy(record3.info, test_info3, record3.info_len);
    record3.slot_effect_count = 25;
    record3.charac_slot_limit = 25;
    strcpy(record3.hash_key, "key_003_act8");
    ASSERT_EQ(CharacViewAct8_Add(manager, &record3), 0, "添加记录3成功");
    free(record3.info);
}

void test_update_slot_effect_count(DBConnectionManager* manager) {
    TEST_START("更新插槽效果数量");

    ASSERT_EQ(CharacViewAct8_UpdateSlotEffectCount(manager, 1000001ULL, 22), 0, "更新插槽效果数量成功");

    CharacViewAct8 retrieved;
    memset(&retrieved, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.slot_effect_count, 22, "插槽效果数量已更新");

    CharacViewAct8_Free(&retrieved);
}

void test_update_charac_slot_limit(DBConnectionManager* manager) {
    TEST_START("更新角色槽位限制");

    ASSERT_EQ(CharacViewAct8_UpdateCharacSlotLimit(manager, 1000001ULL, 24), 0, "更新角色槽位限制成功");

    CharacViewAct8 retrieved;
    memset(&retrieved, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &retrieved), 0, "获取记录成功");
    ASSERT_EQ(retrieved.charac_slot_limit, 24, "角色槽位限制已更新");

    CharacViewAct8_Free(&retrieved);
}

void test_update_hash_key(DBConnectionManager* manager) {
    TEST_START("更新哈希密钥");

    ASSERT_EQ(CharacViewAct8_UpdateHashKey(manager, 1000001ULL, "final_hash_act8"), 0, "更新哈希密钥成功");

    CharacViewAct8 retrieved;
    memset(&retrieved, 0, sizeof(CharacViewAct8));
    ASSERT_EQ(CharacViewAct8_Get(manager, 1000001ULL, &retrieved), 0, "获取记录成功");
    ASSERT_STR_EQ(retrieved.hash_key, "final_hash_act8", "哈希密钥已更新");

    CharacViewAct8_Free(&retrieved);
}

void test_get_by_min_slot_limit(DBConnectionManager* manager) {
    TEST_START("获取槽位限制大于指定值的会员");

    CharacViewAct8 records[10];
    memset(records, 0, sizeof(records));
    int count = CharacViewAct8_GetByMinSlotLimit(manager, 20, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_TRUE(records[0].charac_slot_limit >= 20, "槽位限制满足条件");

    for (int i = 0; i < count; i++) {
        CharacViewAct8_Free(&records[i]);
    }
}

void test_get_by_slot_effect_count(DBConnectionManager* manager) {
    TEST_START("获取指定插槽效果数量的会员");

    CharacViewAct8 records[10];
    memset(records, 0, sizeof(records));
    int count = CharacViewAct8_GetBySlotEffectCount(manager, 22, records, 10);

    ASSERT_TRUE(count >= 1, "至少找到1条记录");
    ASSERT_EQ(records[0].slot_effect_count, 22, "插槽效果数量完全匹配");

    for (int i = 0; i < count; i++) {
        CharacViewAct8_Free(&records[i]);
    }
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("删除记录");

    ASSERT_EQ(CharacViewAct8_Delete(manager, 1000001ULL), 0, "删除记录成功");
    ASSERT_TRUE(!CharacViewAct8_Exists(manager, 1000001ULL), "记录已删除");
}

void test_print_info(DBConnectionManager* manager) {
    TEST_START("打印Act8角色视图配置信息");

    CharacViewAct8 record;
    memset(&record, 0, sizeof(CharacViewAct8));
    if (CharacViewAct8_Get(manager, 1000003ULL, &record) == 0) {
        CharacViewAct8_PrintInfo(&record);
        ASSERT_TRUE(1, "打印信息成功");
        CharacViewAct8_Free(&record);
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
    printf("Act8角色视图配置 ORM 测试\n");
    printf("========================================\n");

    /* 运行测试用例 */
    test_add_and_get(&manager);
    test_update(&manager);
    test_exists(&manager);
    test_add_multiple_records(&manager);
    test_update_slot_effect_count(&manager);
    test_update_charac_slot_limit(&manager);
    test_update_hash_key(&manager);
    test_get_by_min_slot_limit(&manager);
    test_get_by_slot_effect_count(&manager);
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
