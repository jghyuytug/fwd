#include "charac_housing_info_orm.h"
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
#define TEST_CHARAC_NO 888888

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    CharacHousingInfo record, retrieved;
    int ret;
    unsigned char test_decoration[144];
    unsigned char updated_decoration[144];
    int i;

    printf("========================================\n");
    printf("CharacHousingInfo ORM 测试套件\n");
    printf("========================================\n");

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* 初始化测试数据 */
    for (i = 0; i < 144; i++) {
        test_decoration[i] = i % 256;
        updated_decoration[i] = (i + 50) % 256;
    }

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));

    ret = DBConnectionManager_Initialize(&manager, argv[1]);
    ASSERT_EQ(ret, 0, "数据库连接管理器初始化成功");

    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    ASSERT_EQ(ret, 0, "数据库连接成功");

    /* 清理测试数据 */
    CharacHousingInfo_Delete(&manager, TEST_CHARAC_NO);

    /*==========================================================================
     * 测试 1: 添加住房信息
     *=========================================================================*/
    TEST_START("添加住房信息");

    memset(&record, 0, sizeof(CharacHousingInfo));
    record.charac_no = TEST_CHARAC_NO;
    record.installed = 5;
    memcpy(record.decoration_inven, test_decoration, 144);
    record.decoration_inven_len = 144;
    record.version = 1;

    ret = CharacHousingInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_Add 返回成功");

    ret = CharacHousingInfo_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 1, "记录存在");

    /*==========================================================================
     * 测试 2: 获取住房信息
     *=========================================================================*/
    TEST_START("获取住房信息");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_Get 返回成功");
    ASSERT_EQ(retrieved.charac_no, TEST_CHARAC_NO, "角色编号匹配");
    ASSERT_EQ(retrieved.installed, 5, "已安装数量匹配");
    ASSERT_EQ(retrieved.decoration_inven_len, 144, "装饰物品栏长度匹配");
    ASSERT_MEM_EQ(retrieved.decoration_inven, test_decoration, 144, "装饰物品栏内容匹配");
    ASSERT_EQ(retrieved.version, 1, "版本号匹配");

    /*==========================================================================
     * 测试 3: 更新完整记录
     *=========================================================================*/
    TEST_START("更新完整记录");

    record.installed = 10;
    memcpy(record.decoration_inven, updated_decoration, 144);
    record.decoration_inven_len = 144;
    record.version = 2;

    ret = CharacHousingInfo_Update(&manager, &record);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_Update 返回成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.installed, 10, "已安装数量已更新");
    ASSERT_MEM_EQ(retrieved.decoration_inven, updated_decoration, 144, "装饰物品栏已更新");
    ASSERT_EQ(retrieved.version, 2, "版本号已更新");

    /*==========================================================================
     * 测试 4: 更新装饰物品栏
     *=========================================================================*/
    TEST_START("更新装饰物品栏");

    ret = CharacHousingInfo_UpdateDecorationInven(&manager, TEST_CHARAC_NO,
                                                  test_decoration, 144);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_UpdateDecorationInven 返回成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_MEM_EQ(retrieved.decoration_inven, test_decoration, 144, "装饰物品栏已更新");
    ASSERT_EQ(retrieved.installed, 10, "已安装数量未变");

    /*==========================================================================
     * 测试 5: 更新已安装数量
     *=========================================================================*/
    TEST_START("更新已安装数量");

    ret = CharacHousingInfo_UpdateInstalled(&manager, TEST_CHARAC_NO, 15);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_UpdateInstalled 返回成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.installed, 15, "已安装数量已更新为15");

    /*==========================================================================
     * 测试 6: 更新版本号
     *=========================================================================*/
    TEST_START("更新版本号");

    ret = CharacHousingInfo_UpdateVersion(&manager, TEST_CHARAC_NO, 3);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_UpdateVersion 返回成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.version, 3, "版本号已更新为3");

    /*==========================================================================
     * 测试 7: 递增已安装数量
     *=========================================================================*/
    TEST_START("递增已安装数量");

    ret = CharacHousingInfo_IncrementInstalled(&manager, TEST_CHARAC_NO, 5);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_IncrementInstalled 返回成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.installed, 20, "已安装数量递增至20");

    /*==========================================================================
     * 测试 8: 打印住房信息
     *=========================================================================*/
    TEST_START("打印住房信息");

    printf("  调用 CharacHousingInfo_PrintInfo:\n");
    CharacHousingInfo_PrintInfo(&retrieved);
    ASSERT_TRUE(1, "PrintInfo 执行成功");

    /*==========================================================================
     * 测试 9: REPLACE INTO 幂等性
     *=========================================================================*/
    TEST_START("REPLACE INTO 幂等性");

    memset(&record, 0, sizeof(CharacHousingInfo));
    record.charac_no = TEST_CHARAC_NO;
    record.installed = 25;
    memcpy(record.decoration_inven, updated_decoration, 144);
    record.decoration_inven_len = 144;
    record.version = 4;

    ret = CharacHousingInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第一次 Add 成功");

    ret = CharacHousingInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "第二次 Add 成功（幂等）");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.installed, 25, "已安装数量为25（最后一次Add的值）");

    /*==========================================================================
     * 测试 10: 边界测试 - 部分数据
     *=========================================================================*/
    TEST_START("边界测试 - 部分装饰数据");

    memset(&record, 0, sizeof(CharacHousingInfo));
    record.charac_no = TEST_CHARAC_NO;
    record.installed = 0;
    memcpy(record.decoration_inven, test_decoration, 72);
    record.decoration_inven_len = 72;
    record.version = 0;

    ret = CharacHousingInfo_Add(&manager, &record);
    ASSERT_EQ(ret, 0, "添加部分数据成功");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_EQ(ret, 0, "重新获取成功");
    ASSERT_EQ(retrieved.installed, 0, "已安装数量为0");
    ASSERT_EQ(retrieved.decoration_inven_len, 144, "装饰物品栏长度为144（binary类型自动填充）");

    /*==========================================================================
     * 测试 11: 删除记录
     *=========================================================================*/
    TEST_START("删除记录");

    ret = CharacHousingInfo_Delete(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "CharacHousingInfo_Delete 返回成功");

    ret = CharacHousingInfo_Exists(&manager, TEST_CHARAC_NO);
    ASSERT_EQ(ret, 0, "记录已删除");

    memset(&retrieved, 0, sizeof(CharacHousingInfo));
    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, &retrieved);
    ASSERT_NEQ(ret, 0, "获取已删除记录失败（预期）");

    /*==========================================================================
     * 测试 12: 错误处理 - NULL参数
     *=========================================================================*/
    TEST_START("错误处理 - NULL参数");

    ret = CharacHousingInfo_Add(NULL, &record);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacHousingInfo_Add(&manager, NULL);
    ASSERT_NEQ(ret, 0, "NULL record 返回错误");

    ret = CharacHousingInfo_Get(NULL, TEST_CHARAC_NO, &retrieved);
    ASSERT_NEQ(ret, 0, "NULL manager 返回错误");

    ret = CharacHousingInfo_Get(&manager, TEST_CHARAC_NO, NULL);
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
