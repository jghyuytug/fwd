#include "charac_npc_orm.h"
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

#define ASSERT_TRUE(condition, message) \
    do { \
        assertion_count++; \
        if (condition) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED)\n", message); \
        } \
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

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: values are equal)\n", message); \
        } \
    } while(0)

#define ASSERT_MEM_EQ(a, b, len, message) \
    do { \
        assertion_count++; \
        if (memcmp(a, b, len) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: memory differs)\n", message); \
        } \
    } while(0)

/* 测试用的角色编号 */
#define TEST_CHARAC_NO 999999

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    CharacNpc record, retrieved;
    int ret;
    unsigned char test_npc_data[100];
    unsigned char updated_npc_data[100];
    int i;

    printf("========================================\n");
    printf("CharacNpc ORM 测试套件\n");
    printf("========================================\n");

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化测试数据 */
    for (i = 0; i < 100; i++) {
        test_npc_data[i] = i;
        updated_npc_data[i] = i + 100;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    ASSERT_EQ(ret, 0, "数据库连接管理器初始化成功");

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    ASSERT_EQ(ret, 0, "数据库连接成功");

    /* 清理测试数据 */
    CharacNpc_Delete(&manager, TEST_CHARAC_NO);

    /*==========================================================================
     * 测试 1: 添加NPC记录
     *=========================================================================*/
    TEST_START("添加NPC记录");

    memset(&record, 0, sizeof(CharacNpc));
    record.charac_no = TEST_CHARAC_NO;
    record.npc_cnt = 5;
    memcpy(record.npc_data, test_npc_data, 100);
    record.npc_data_len = 100;

    ret = CharacNpc_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacNpc_Add 返回成功");

    ret = CharacNpc_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 1, "记录存在");

    /*==========================================================================
     * 测试 2: 获取NPC记录
     *=========================================================================*/
    TEST_START("获取NPC记录");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "CharacNpc_Get 返回成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO, "角色编号匹配");
    ASSERT_EQ(retrieved.npc_cnt, 5, "NPC数量匹配");
    ASSERT_EQ(retrieved.npc_data_len, 100, "NPC数据长度匹配");
    ASSERT_MEM_EQ(retrieved.npc_data, test_npc_data, 100, "NPC数据内容匹配");

    /*==========================================================================
     * 测试 3: 更新完整记录
     *=========================================================================*/
    TEST_START("更新完整记录");

    record.npc_cnt = 10;
    memcpy(record.npc_data, updated_npc_data, 100);
    record.npc_data_len = 100;

    ret = CharacNpc_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacNpc_Update 返回成功");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.npc_cnt, 10, "NPC数量已更新");
    ASSERT_MEM_EQ(retrieved.npc_data, updated_npc_data, 100, "NPC数据已更新");

    /*==========================================================================
     * 测试 4: 更新NPC数据
     *=========================================================================*/
    TEST_START("更新NPC数据");

    ret = CharacNpc_UpdateNpcData(&manager, TEST_CHARAC_NO, test_npc_data, 100);
    ASSERT_EQ(ret, 0, "CharacNpc_UpdateNpcData 返回成功");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_MEM_EQ(retrieved.npc_data, test_npc_data, 100, "NPC数据已更新");
    ASSERT_EQ(retrieved.npc_cnt, 10, "NPC数量未变");

    /*==========================================================================
     * 测试 5: 更新NPC数量
     *=========================================================================*/
    TEST_START("更新NPC数量");

    ret = CharacNpc_UpdateNpcCnt(&manager, TEST_CHARAC_NO, 20);
    ASSERT_EQ(ret, 0, "CharacNpc_UpdateNpcCnt 返回成功");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.npc_cnt, 20, "NPC数量已更新为20");

    /*==========================================================================
     * 测试 6: 递增NPC数量
     *=========================================================================*/
    TEST_START("递增NPC数量");

    ret = CharacNpc_IncrementNpcCnt(&manager, TEST_CHARAC_NO, 5);
    ASSERT_EQ(ret, 0, "CharacNpc_IncrementNpcCnt 返回成功");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.npc_cnt, 25, "NPC数量递增至25");

    /*==========================================================================
     * 测试 7: 打印NPC信息
     *=========================================================================*/
    TEST_START("打印NPC信息");

    printf("  调用 CharacNpc_PrintInfo:\n");
    CharacNpc_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "PrintInfo 执行成功");

    /*==========================================================================
     * 测试 8: REPLACE INTO 幂等性
     *=========================================================================*/
    TEST_START("REPLACE INTO 幂等性");

    memset(&record, 0, sizeof(CharacNpc));
    record.charac_no = TEST_CHARAC_NO;
    record.npc_cnt = 30;
    memcpy(record.npc_data, updated_npc_data, 100);
    record.npc_data_len = 100;

    ret = CharacNpc_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 Add 成功");

    ret = CharacNpc_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 Add 成功（幂等）");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.npc_cnt, 30, "NPC数量为30（最后一次Add的值）");

    /*==========================================================================
     * 测试 9: 边界测试 - 空数据
     *=========================================================================*/
    TEST_START("边界测试 - 空NPC数据");

    memset(&record, 0, sizeof(CharacNpc));
    record.charac_no = TEST_CHARAC_NO;
    record.npc_cnt = 0;
    record.npc_data_len = 0;

    ret = CharacNpc_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加空数据成功");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.npc_cnt, 0, "NPC数量为0");
    ASSERT_EQ(retrieved.npc_data_len, 0, "NPC数据长度为0");

    /*==========================================================================
     * 测试 10: 删除记录
     *=========================================================================*/
    TEST_START("删除记录");

    ret = CharacNpc_Delete(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "CharacNpc_Delete 返回成功");

    ret = CharacNpc_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "记录已删除");

    memset(&retrieved, 0, sizeof(CharacNpc));
    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_NEQ(ret, 0, "获取已删除记录失败（预期）");

    /*==========================================================================
     * 测试 11: 错误处理 - NULL参数
     *=========================================================================*/
    TEST_START("错误处理 - NULL参数");

    ret = CharacNpc_Add(NULL, &record);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacNpc_Add(&manager, NULL);
    ASSERT_NEQ(ret, 0, "NULL record 返回错误");

    ret = CharacNpc_Get(NULL, TEST_CHARAC_NO, &retrieved);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacNpc_Get(&manager, TEST_CHARAC_NO, NULL);
    ASSERT_NEQ(ret, 0, "NULL record 返回错误");

    /* 更新最后一个测试的状态 */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试完成\n");
    printf("========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("断言: %d/%d 通过\n", assertion_passed, assertion_count);
    printf("========================================\n");

    return (test_passed == test_count && assertion_passed == assertion_count) ? 0 : 1;
}
