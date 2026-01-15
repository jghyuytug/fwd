#include "dnf_story_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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

#define ASSERT_NEQ(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) != (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d == %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GT(a, b, message) \
    do { \
        assertion_count++; \
        if ((a) > (b)) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: %d <= %d)\n", message, (int)(a), (int)(b)); \
        } \
    } while(0)

#define ASSERT_GE(a, b, message) \
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

#define ASSERT_STREQ(a, b, message) \
    do { \
        assertion_count++; \
        if (strcmp((a), (b)) == 0) { \
            assertion_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            current_test_failed = 1; \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
        } \
    } while(0)

/* 辅助函数 */
static void cleanup_test_data(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult result;

    /* 删除测试数据 - 使用测试用的m_id范围 */
    snprintf(query, sizeof(query),
        "DELETE FROM dnf_story WHERE m_id >= 900000 AND m_id < 999999");

    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

/* ==================== 测试用例 ==================== */

void test_add_and_get(DBConnectionManager* manager) {
    TEST_START("测试添加和查询故事记录");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 1;
    record.notice_flag = 0;
    record.m_id = 900001;
    snprintf(record.reg_id, sizeof(record.reg_id), "test001");
    snprintf(record.title, sizeof(record.title), "测试故事标题");
    snprintf(record.url, sizeof(record.url), "http://test.com/story1");
    snprintf(record.img_name, sizeof(record.img_name), "story1.jpg");
    record.opt = 1;
    record.open_flag = 'y';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 10:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = "这是测试故事的内容。";

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功，返回自增no");

    DnfStory retrieved;
    memset(&retrieved, 0, sizeof(DnfStory));
    retrieved.content = NULL;
    int ret = DnfStory_Get(manager, new_no, &retrieved);
    ASSERT_EQ(ret, 0, "查询故事记录成功");
    ASSERT_EQ(retrieved.no, new_no, "no匹配");
    ASSERT_EQ(retrieved.story_type, 1, "story_type匹配");
    ASSERT_EQ(retrieved.m_id, 900001, "m_id匹配");
    ASSERT_STREQ(retrieved.title, "测试故事标题", "title匹配");
    ASSERT_EQ(retrieved.open_flag, 'y', "open_flag匹配");

    DnfStory_PrintInfo(&retrieved);
    DnfStory_FreeContent(&retrieved);
}

void test_add_with_null_content(DBConnectionManager* manager) {
    TEST_START("测试添加无内容的故事记录");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 2;
    record.notice_flag = 1;
    record.m_id = 900002;
    snprintf(record.reg_id, sizeof(record.reg_id), "test002");
    snprintf(record.title, sizeof(record.title), "无内容公告");
    snprintf(record.url, sizeof(record.url), "http://test.com/notice");
    snprintf(record.img_name, sizeof(record.img_name), "notice.jpg");
    record.opt = 0;
    record.open_flag = 'n';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 11:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = NULL;  /* NULL内容 */

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加无内容故事成功");

    DnfStory retrieved;
    memset(&retrieved, 0, sizeof(DnfStory));
    retrieved.content = NULL;
    int ret = DnfStory_Get(manager, new_no, &retrieved);
    ASSERT_EQ(ret, 0, "查询成功");
    ASSERT_STREQ(retrieved.title, "无内容公告", "title匹配");

    DnfStory_FreeContent(&retrieved);
}

void test_update(DBConnectionManager* manager) {
    TEST_START("测试更新故事记录");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 3;
    record.notice_flag = 0;
    record.m_id = 900003;
    snprintf(record.reg_id, sizeof(record.reg_id), "test003");
    snprintf(record.title, sizeof(record.title), "原标题");
    snprintf(record.url, sizeof(record.url), "http://test.com/story3");
    snprintf(record.img_name, sizeof(record.img_name), "story3.jpg");
    record.opt = 0;
    record.open_flag = 'n';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 12:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = "原内容";

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功");

    /* 查询并修改 */
    DnfStory retrieved;
    memset(&retrieved, 0, sizeof(DnfStory));
    retrieved.content = NULL;
    DnfStory_Get(manager, new_no, &retrieved);

    snprintf(retrieved.title, sizeof(retrieved.title), "新标题");
    retrieved.hits = 100;
    retrieved.open_flag = 'y';
    DnfStory_FreeContent(&retrieved);
    retrieved.content = "新内容";

    int ret = DnfStory_Update(manager, &retrieved);
    ASSERT_EQ(ret, 0, "更新故事记录成功");

    /* 验证更新 */
    DnfStory updated;
    memset(&updated, 0, sizeof(DnfStory));
    updated.content = NULL;
    DnfStory_Get(manager, new_no, &updated);
    ASSERT_STREQ(updated.title, "新标题", "标题已更新");
    ASSERT_EQ(updated.hits, 100, "点击数已更新");
    ASSERT_EQ(updated.open_flag, 'y', "开放标志已更新");

    DnfStory_FreeContent(&updated);
}

void test_update_hits(DBConnectionManager* manager) {
    TEST_START("测试更新点击数");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 1;
    record.notice_flag = 0;
    record.m_id = 900004;
    snprintf(record.reg_id, sizeof(record.reg_id), "test004");
    snprintf(record.title, sizeof(record.title), "点击数测试");
    snprintf(record.url, sizeof(record.url), "http://test.com/hits");
    snprintf(record.img_name, sizeof(record.img_name), "hits.jpg");
    record.opt = 0;
    record.open_flag = 'y';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 13:00:00");
    record.hits = 10;
    record.reserve_time = 0;
    record.content = NULL;

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功");

    /* 更新点击数 */
    int ret = DnfStory_UpdateHits(manager, new_no, 50);
    ASSERT_EQ(ret, 0, "更新点击数成功");

    DnfStory retrieved;
    memset(&retrieved, 0, sizeof(DnfStory));
    retrieved.content = NULL;
    DnfStory_Get(manager, new_no, &retrieved);
    ASSERT_EQ(retrieved.hits, 50, "点击数已更新为50");

    /* 增加点击数 */
    ret = DnfStory_IncrementHits(manager, new_no);
    ASSERT_EQ(ret, 0, "增加点击数成功");

    DnfStory retrieved2;
    memset(&retrieved2, 0, sizeof(DnfStory));
    retrieved2.content = NULL;
    DnfStory_Get(manager, new_no, &retrieved2);
    ASSERT_EQ(retrieved2.hits, 51, "点击数已增加到51");

    DnfStory_FreeContent(&retrieved);
    DnfStory_FreeContent(&retrieved2);
}

void test_update_open_flag(DBConnectionManager* manager) {
    TEST_START("测试更新开放标志");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 1;
    record.notice_flag = 0;
    record.m_id = 900005;
    snprintf(record.reg_id, sizeof(record.reg_id), "test005");
    snprintf(record.title, sizeof(record.title), "开放标志测试");
    snprintf(record.url, sizeof(record.url), "http://test.com/flag");
    snprintf(record.img_name, sizeof(record.img_name), "flag.jpg");
    record.opt = 0;
    record.open_flag = 'n';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 14:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = NULL;

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功");

    int ret = DnfStory_UpdateOpenFlag(manager, new_no, 'y');
    ASSERT_EQ(ret, 0, "更新开放标志成功");

    DnfStory retrieved;
    memset(&retrieved, 0, sizeof(DnfStory));
    retrieved.content = NULL;
    DnfStory_Get(manager, new_no, &retrieved);
    ASSERT_EQ(retrieved.open_flag, 'y', "开放标志已更新为y");

    DnfStory_FreeContent(&retrieved);
}

void test_get_by_mid(DBConnectionManager* manager) {
    TEST_START("测试查询会员的所有故事");

    int test_m_id = 900006;

    /* 添加多条记录 */
    for (int i = 0; i < 3; i++) {
        DnfStory record;
        memset(&record, 0, sizeof(DnfStory));
        record.story_type = i;
        record.notice_flag = 0;
        record.m_id = test_m_id;
        snprintf(record.reg_id, sizeof(record.reg_id), "test%03d", 6 + i);
        snprintf(record.title, sizeof(record.title), "会员故事%d", i);
        snprintf(record.url, sizeof(record.url), "http://test.com/mid%d", i);
        snprintf(record.img_name, sizeof(record.img_name), "mid%d.jpg", i);
        record.opt = 0;
        record.open_flag = 'y';
        snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 15:00:00");
        record.hits = i * 10;
        record.reserve_time = 0;
        record.content = NULL;

        int new_no = DnfStory_Add(manager, &record);
        ASSERT_GT(new_no, 0, "添加记录成功");
    }

    DnfStory records[10];
    memset(records, 0, sizeof(records));
    for (int i = 0; i < 10; i++) {
        records[i].content = NULL;
    }

    int count = DnfStory_GetByMid(manager, test_m_id, records, 10);
    ASSERT_GE(count, 3, "查询到至少3条记录");

    printf("  查询到 %d 条会员 %d 的故事\n", count, test_m_id);

    /* 释放内存 */
    for (int i = 0; i < count; i++) {
        DnfStory_FreeContent(&records[i]);
    }
}

void test_get_by_story_type(DBConnectionManager* manager) {
    TEST_START("测试按故事类型查询");

    int test_story_type = 5;

    /* 添加多条记录 */
    for (int i = 0; i < 2; i++) {
        DnfStory record;
        memset(&record, 0, sizeof(DnfStory));
        record.story_type = test_story_type;
        record.notice_flag = 0;
        record.m_id = 900007 + i;
        snprintf(record.reg_id, sizeof(record.reg_id), "test%03d", 10 + i);
        snprintf(record.title, sizeof(record.title), "类型故事%d", i);
        snprintf(record.url, sizeof(record.url), "http://test.com/type%d", i);
        snprintf(record.img_name, sizeof(record.img_name), "type%d.jpg", i);
        record.opt = 0;
        record.open_flag = 'y';
        snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 16:00:00");
        record.hits = 0;
        record.reserve_time = 0;
        record.content = NULL;

        int new_no = DnfStory_Add(manager, &record);
        ASSERT_GT(new_no, 0, "添加记录成功");
    }

    DnfStory records[10];
    memset(records, 0, sizeof(records));
    for (int i = 0; i < 10; i++) {
        records[i].content = NULL;
    }

    int count = DnfStory_GetByStoryType(manager, test_story_type, records, 10);
    ASSERT_GE(count, 2, "查询到至少2条记录");

    printf("  查询到 %d 条类型为 %d 的故事\n", count, test_story_type);

    /* 释放内存 */
    for (int i = 0; i < count; i++) {
        DnfStory_FreeContent(&records[i]);
    }
}

void test_exists(DBConnectionManager* manager) {
    TEST_START("测试记录存在性检查");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 1;
    record.notice_flag = 0;
    record.m_id = 900010;
    snprintf(record.reg_id, sizeof(record.reg_id), "test010");
    snprintf(record.title, sizeof(record.title), "存在性测试");
    snprintf(record.url, sizeof(record.url), "http://test.com/exists");
    snprintf(record.img_name, sizeof(record.img_name), "exists.jpg");
    record.opt = 0;
    record.open_flag = 'y';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 17:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = NULL;

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功");

    int exists = DnfStory_Exists(manager, new_no);
    ASSERT_EQ(exists, 1, "已存在的no返回1");

    exists = DnfStory_Exists(manager, 999999);
    ASSERT_EQ(exists, 0, "不存在的no返回0");
}

void test_delete(DBConnectionManager* manager) {
    TEST_START("测试删除故事记录");

    DnfStory record;
    memset(&record, 0, sizeof(DnfStory));
    record.story_type = 1;
    record.notice_flag = 0;
    record.m_id = 900011;
    snprintf(record.reg_id, sizeof(record.reg_id), "test011");
    snprintf(record.title, sizeof(record.title), "删除测试");
    snprintf(record.url, sizeof(record.url), "http://test.com/delete");
    snprintf(record.img_name, sizeof(record.img_name), "delete.jpg");
    record.opt = 0;
    record.open_flag = 'y';
    snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 18:00:00");
    record.hits = 0;
    record.reserve_time = 0;
    record.content = NULL;

    int new_no = DnfStory_Add(manager, &record);
    ASSERT_GT(new_no, 0, "添加故事记录成功");

    int ret = DnfStory_Delete(manager, new_no);
    ASSERT_EQ(ret, 0, "删除故事记录成功");

    int exists = DnfStory_Exists(manager, new_no);
    ASSERT_EQ(exists, 0, "记录已不存在");
}

void test_count_by_mid(DBConnectionManager* manager) {
    TEST_START("测试统计会员的故事数量");

    int test_m_id = 900012;

    /* 添加多条记录 */
    for (int i = 0; i < 3; i++) {
        DnfStory record;
        memset(&record, 0, sizeof(DnfStory));
        record.story_type = 1;
        record.notice_flag = 0;
        record.m_id = test_m_id;
        snprintf(record.reg_id, sizeof(record.reg_id), "test%03d", 12 + i);
        snprintf(record.title, sizeof(record.title), "统计测试%d", i);
        snprintf(record.url, sizeof(record.url), "http://test.com/count%d", i);
        snprintf(record.img_name, sizeof(record.img_name), "count%d.jpg", i);
        record.opt = 0;
        record.open_flag = 'y';
        snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 19:00:00");
        record.hits = 0;
        record.reserve_time = 0;
        record.content = NULL;

        DnfStory_Add(manager, &record);
    }

    int count = DnfStory_CountByMid(manager, test_m_id);
    ASSERT_GE(count, 3, "统计到至少3条记录");

    printf("  会员 %d 的故事数量: %d\n", test_m_id, count);
}

void test_count_by_story_type(DBConnectionManager* manager) {
    TEST_START("测试按故事类型统计");

    int test_story_type = 10;

    /* 添加多条记录 */
    for (int i = 0; i < 2; i++) {
        DnfStory record;
        memset(&record, 0, sizeof(DnfStory));
        record.story_type = test_story_type;
        record.notice_flag = 0;
        record.m_id = 900013 + i;
        snprintf(record.reg_id, sizeof(record.reg_id), "test%03d", 15 + i);
        snprintf(record.title, sizeof(record.title), "类型统计%d", i);
        snprintf(record.url, sizeof(record.url), "http://test.com/typecount%d", i);
        snprintf(record.img_name, sizeof(record.img_name), "typecount%d.jpg", i);
        record.opt = 0;
        record.open_flag = 'y';
        snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 20:00:00");
        record.hits = 0;
        record.reserve_time = 0;
        record.content = NULL;

        DnfStory_Add(manager, &record);
    }

    int count = DnfStory_CountByStoryType(manager, test_story_type);
    ASSERT_GE(count, 2, "统计到至少2条记录");

    printf("  story_type=%d 的故事数量: %d\n", test_story_type, count);
}

void test_count_open(DBConnectionManager* manager) {
    TEST_START("测试统计开放的故事数量");

    int count = DnfStory_CountOpen(manager);
    ASSERT_GE(count, 0, "开放故事数>=0");

    printf("  当前开放的故事数量: %d\n", count);
}

void test_count_total(DBConnectionManager* manager) {
    TEST_START("测试统计总故事数");

    int count = DnfStory_CountTotal(manager);
    ASSERT_GE(count, 0, "总故事数>=0");

    printf("  当前总故事数: %d\n", count);
}

void test_batch_delete(DBConnectionManager* manager) {
    TEST_START("测试批量删除");

    /* 添加测试记录 */
    int nos[3];
    for (int i = 0; i < 3; i++) {
        DnfStory record;
        memset(&record, 0, sizeof(DnfStory));
        record.story_type = 1;
        record.notice_flag = 0;
        record.m_id = 900015 + i;
        snprintf(record.reg_id, sizeof(record.reg_id), "test%03d", 20 + i);
        snprintf(record.title, sizeof(record.title), "批量删除%d", i);
        snprintf(record.url, sizeof(record.url), "http://test.com/batch%d", i);
        snprintf(record.img_name, sizeof(record.img_name), "batch%d.jpg", i);
        record.opt = 0;
        record.open_flag = 'y';
        snprintf(record.reg_date, sizeof(record.reg_date), "2025-11-16 21:00:00");
        record.hits = 0;
        record.reserve_time = 0;
        record.content = NULL;

        nos[i] = DnfStory_Add(manager, &record);
        ASSERT_GT(nos[i], 0, "添加测试记录成功");
    }

    int ret = DnfStory_BatchDelete(manager, nos, 3);
    ASSERT_EQ(ret, 0, "批量删除成功");

    /* 验证已删除 */
    for (int i = 0; i < 3; i++) {
        int exists = DnfStory_Exists(manager, nos[i]);
        ASSERT_EQ(exists, 0, "批量删除的记录已不存在");
    }
}

void test_null_pointer_checks(DBConnectionManager* manager) {
    TEST_START("测试NULL指针检查");

    int ret = DnfStory_Add(NULL, NULL);
    ASSERT_EQ(ret, -1, "manager=NULL时返回-1");

    DnfStory record;
    ret = DnfStory_Get(manager, 1, NULL);
    ASSERT_EQ(ret, -1, "record=NULL时返回-1");

    ret = DnfStory_GetByMid(manager, 1, NULL, 10);
    ASSERT_EQ(ret, -1, "records=NULL时返回-1");

    ret = DnfStory_GetByMid(manager, 1, &record, 0);
    ASSERT_EQ(ret, -1, "max_count<=0时返回-1");

    ret = DnfStory_UpdateOpenFlag(manager, 1, 'x');
    ASSERT_EQ(ret, -1, "无效的open_flag返回-1");
}

/* ==================== 主函数 ==================== */

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        printf("用法: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("dnf_story_orm 测试套件\n");
    printf("========================================\n");

    /* 初始化数据库连接 */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("连接数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 运行测试 */
    test_add_and_get(&manager);
    test_add_with_null_content(&manager);
    test_update(&manager);
    test_update_hits(&manager);
    test_update_open_flag(&manager);
    test_get_by_mid(&manager);
    test_get_by_story_type(&manager);
    test_exists(&manager);
    test_delete(&manager);
    test_count_by_mid(&manager);
    test_count_by_story_type(&manager);
    test_count_open(&manager);
    test_count_total(&manager);
    test_batch_delete(&manager);
    test_null_pointer_checks(&manager);

    /* 最后一个测试完成时增加test_passed */
    if (!current_test_failed) {
        test_passed++;
    }

    /* 清理测试数据 */
    cleanup_test_data(&manager);

    /* 清理数据库连接 */
    DBConnectionManager_Cleanup(&manager);

    /* 打印测试结果 */
    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过 (%.1f%%)\n",
        test_passed, test_count, (test_passed * 100.0) / test_count);
    printf("断言: %d/%d 通过 (%.1f%%)\n",
        assertion_passed, assertion_count, (assertion_passed * 100.0) / assertion_count);
    printf("========================================\n");

    if (test_passed == test_count && assertion_passed == assertion_count) {
        printf("✅ 所有测试通过！\n");
        return 0;
    } else {
        printf("❌ 部分测试失败！\n");
        return 1;
    }
}
