#include "../include/database/headers/charac_achievement_orm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* 全局测试统计 */
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

/* 全局数据库连接管理器 */
static DBConnectionManager manager;

/* 测试用例宏 */
#define TEST_START(name) \
    do { \
        total_tests++; \
        printf("\n[TEST %d] %s\n", total_tests, name); \
    } while(0)

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            passed_tests++; \
            printf("  ✓ %s [PASS]\n", message); \
        } else { \
            failed_tests++; \
            printf("  ✗ %s [FAIL]\n", message); \
        } \
    } while(0)

/*******************************************************************************
 * 辅助函数
 ******************************************************************************/

static int Init_Database(const char* config_file) {
    printf("Initializing database connection...\n");

    if (DBConnectionManager_Initialize(&manager, config_file) < 0) {
        printf("Failed to initialize database connection manager\n");
        return -1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to taiwan_cain database\n");
        DBConnectionManager_Cleanup(&manager);
        return -1;
    }

    printf("Database connection established\n");
    return 0;
}

static void Clear_Test_Data(void) {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_CAIN,
        "DELETE FROM charac_achievement WHERE charac_no BETWEEN 900000 AND 999999", &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/*******************************************************************************
 * CRUD 操作测试
 ******************************************************************************/

static void Test_Add() {
    CharacAchievement record;
    TEST_START("CharacAchievement_Add");

    Clear_Test_Data();

    record.charac_no = 900001;
    memcpy(record.achievement, "\x01\x02\x03\x04\x05", 5);
    record.achievement_len = 5;
    strcpy(record.last_update_time, "2025-11-14 10:00:00");

    TEST_ASSERT(CharacAchievement_Add(&manager, &record) == 0,
                "Should successfully add a record");

    /* 测试幂等性：重复添加相同 charac_no */
    memcpy(record.achievement, "\x0A\x0B\x0C", 3);
    record.achievement_len = 3;
    TEST_ASSERT(CharacAchievement_Add(&manager, &record) == 0,
                "Should successfully update existing record (REPLACE)");

    Clear_Test_Data();
}

static void Test_Get() {
    CharacAchievement record, retrieved;
    TEST_START("CharacAchievement_Get");

    Clear_Test_Data();

    record.charac_no = 900002;
    memcpy(record.achievement, "\x10\x20\x30\x40\x50\x60\x70\x80", 8);
    record.achievement_len = 8;
    strcpy(record.last_update_time, "2025-11-14 11:00:00");
    CharacAchievement_Add(&manager, &record);

    TEST_ASSERT(CharacAchievement_Get(&manager, 900002, &retrieved) == 0,
                "Should retrieve existing record");
    TEST_ASSERT(retrieved.charac_no == 900002,
                "Should have correct charac_no");
    TEST_ASSERT(retrieved.achievement_len == 8,
                "Should have correct achievement length");
    TEST_ASSERT(memcmp(retrieved.achievement, record.achievement, 8) == 0,
                "Should have matching achievement data");

    TEST_ASSERT(CharacAchievement_Get(&manager, 999999, &retrieved) < 0,
                "Should fail for non-existent record");

    Clear_Test_Data();
}

static void Test_Update() {
    CharacAchievement record;
    unsigned char new_data[] = "\xFF\xEE\xDD\xCC\xBB\xAA";
    TEST_START("CharacAchievement_Update");

    Clear_Test_Data();

    record.charac_no = 900003;
    memcpy(record.achievement, "\x01\x02\x03", 3);
    record.achievement_len = 3;
    strcpy(record.last_update_time, "2025-11-14 12:00:00");
    CharacAchievement_Add(&manager, &record);

    TEST_ASSERT(CharacAchievement_Update(&manager, 900003, new_data, 6) == 0,
                "Should successfully update achievement data");

    CharacAchievement_Get(&manager, 900003, &record);
    TEST_ASSERT(record.achievement_len == 6,
                "Should have updated achievement length");
    TEST_ASSERT(memcmp(record.achievement, new_data, 6) == 0,
                "Should have updated achievement data");

    Clear_Test_Data();
}

static void Test_Delete() {
    CharacAchievement record;
    TEST_START("CharacAchievement_Delete");

    Clear_Test_Data();

    record.charac_no = 900004;
    memcpy(record.achievement, "\xAA\xBB\xCC", 3);
    record.achievement_len = 3;
    strcpy(record.last_update_time, "2025-11-14 13:00:00");
    CharacAchievement_Add(&manager, &record);

    TEST_ASSERT(CharacAchievement_Delete(&manager, 900004) == 0,
                "Should successfully delete record");

    TEST_ASSERT(CharacAchievement_Get(&manager, 900004, &record) < 0,
                "Record should not exist after deletion");

    /* 测试删除不存在的记录 */
    TEST_ASSERT(CharacAchievement_Delete(&manager, 999999) == 0,
                "Should return 0 even for non-existent record");

    Clear_Test_Data();
}

static void Test_Exists() {
    CharacAchievement record;
    TEST_START("CharacAchievement_Exists");

    Clear_Test_Data();

    record.charac_no = 900005;
    memcpy(record.achievement, "\x11\x22\x33", 3);
    record.achievement_len = 3;
    strcpy(record.last_update_time, "2025-11-14 14:00:00");
    CharacAchievement_Add(&manager, &record);

    TEST_ASSERT(CharacAchievement_Exists(&manager, 900005) == 1,
                "Should return 1 for existing record");

    TEST_ASSERT(CharacAchievement_Exists(&manager, 999999) == 0,
                "Should return 0 for non-existent record");

    Clear_Test_Data();
}

/*******************************************************************************
 * 业务查询测试
 ******************************************************************************/

static void Test_GetByLastUpdateTimeRange() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_GetByLastUpdateTimeRange");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900010 + i;
        memcpy(records[i].achievement, "\x01\x02\x03", 3);
        records[i].achievement_len = 3;
        snprintf(records[i].last_update_time, sizeof(records[i].last_update_time),
                 "2025-11-14 10:%02d:00", i * 10);
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_GetByLastUpdateTimeRange(&manager,
        "2025-11-14 10:10:00", "2025-11-14 10:30:00", records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records in time range (10:10 to 10:30)");

    Clear_Test_Data();
}

static void Test_GetRecentUpdates() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_GetRecentUpdates");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900020 + i;
        memcpy(records[i].achievement, "\xAA\xBB", 2);
        records[i].achievement_len = 2;
        snprintf(records[i].last_update_time, sizeof(records[i].last_update_time),
                 "2025-11-14 15:%02d:00", i * 5);
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_GetRecentUpdates(&manager, records, 3);
    TEST_ASSERT(count == 3, "Should get top 3 recent updates");
    TEST_ASSERT(records[0].charac_no == 900024, "First should be most recent (900024)");

    Clear_Test_Data();
}

static void Test_GetEmptyAchievements() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_GetEmptyAchievements");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900030 + i;
        if (i < 2) {
            records[i].achievement_len = 0;  /* 空数据 */
        } else {
            memcpy(records[i].achievement, "\x01\x02", 2);
            records[i].achievement_len = 2;
        }
        strcpy(records[i].last_update_time, "2025-11-14 16:00:00");
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_GetEmptyAchievements(&manager, records, 10);
    TEST_ASSERT(count == 2, "Should find 2 characters with empty achievements");

    Clear_Test_Data();
}

static void Test_GetNonEmptyAchievements() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_GetNonEmptyAchievements");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900040 + i;
        if (i < 3) {
            memcpy(records[i].achievement, "\xFF\xEE", 2);
            records[i].achievement_len = 2;
        } else {
            records[i].achievement_len = 0;
        }
        strcpy(records[i].last_update_time, "2025-11-14 17:00:00");
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_GetNonEmptyAchievements(&manager, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 characters with non-empty achievements");

    Clear_Test_Data();
}

static void Test_GetAchievementSize() {
    CharacAchievement record;
    int size;
    TEST_START("CharacAchievement_GetAchievementSize");

    Clear_Test_Data();

    record.charac_no = 900050;
    memcpy(record.achievement, "12345678901234567890", 20);
    record.achievement_len = 20;
    strcpy(record.last_update_time, "2025-11-14 18:00:00");
    CharacAchievement_Add(&manager, &record);

    size = CharacAchievement_GetAchievementSize(&manager, 900050);
    TEST_ASSERT(size == 20, "Should return correct achievement data size");

    Clear_Test_Data();
}

static void Test_GetByAchievementSize() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_GetByAchievementSize");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900060 + i;
        /* 创建不同大小的数据: 5, 10, 15, 20, 25 字节 */
        memset(records[i].achievement, 0xAA, (i + 1) * 5);
        records[i].achievement_len = (i + 1) * 5;
        strcpy(records[i].last_update_time, "2025-11-14 19:00:00");
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_GetByAchievementSize(&manager, 10, 20, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with achievement size between 10 and 20");

    Clear_Test_Data();
}

static void Test_ClearAchievement() {
    CharacAchievement record;
    int size;
    TEST_START("CharacAchievement_ClearAchievement");

    Clear_Test_Data();

    record.charac_no = 900070;
    memcpy(record.achievement, "\x01\x02\x03\x04\x05", 5);
    record.achievement_len = 5;
    strcpy(record.last_update_time, "2025-11-14 20:00:00");
    CharacAchievement_Add(&manager, &record);

    TEST_ASSERT(CharacAchievement_ClearAchievement(&manager, 900070) == 0,
                "Should successfully clear achievement data");

    size = CharacAchievement_GetAchievementSize(&manager, 900070);
    TEST_ASSERT(size == 0, "Achievement data should be empty after clearing");

    Clear_Test_Data();
}

static void Test_CountNonEmptyAchievements() {
    CharacAchievement records[10];
    int count;
    int i;
    TEST_START("CharacAchievement_CountNonEmptyAchievements");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].charac_no = 900080 + i;
        if (i < 3) {
            memcpy(records[i].achievement, "\x11\x22", 2);
            records[i].achievement_len = 2;
        } else {
            records[i].achievement_len = 0;
        }
        strcpy(records[i].last_update_time, "2025-11-14 21:00:00");
        CharacAchievement_Add(&manager, &records[i]);
    }

    count = CharacAchievement_CountNonEmptyAchievements(&manager);
    TEST_ASSERT(count == 3, "Should count 3 characters with non-empty achievements");

    Clear_Test_Data();
}

static void Test_UpdateTimestamp() {
    CharacAchievement record;
    char old_time[32];
    TEST_START("CharacAchievement_UpdateTimestamp");

    Clear_Test_Data();

    record.charac_no = 900090;
    memcpy(record.achievement, "\x01\x02\x03", 3);
    record.achievement_len = 3;
    strcpy(record.last_update_time, "2020-01-01 00:00:00");
    CharacAchievement_Add(&manager, &record);

    CharacAchievement_Get(&manager, 900090, &record);
    strcpy(old_time, record.last_update_time);

    TEST_ASSERT(CharacAchievement_UpdateTimestamp(&manager, 900090) == 0,
                "Should successfully update timestamp");

    CharacAchievement_Get(&manager, 900090, &record);
    TEST_ASSERT(strcmp(record.last_update_time, old_time) != 0,
                "Timestamp should be updated to current time");

    Clear_Test_Data();
}

/*******************************************************************************
 * 批量操作测试
 ******************************************************************************/

static void Test_BulkAdd() {
    CharacAchievement records[3];
    int count;
    int i;
    TEST_START("CharacAchievement_BulkAdd");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        records[i].charac_no = 900100 + i;
        memcpy(records[i].achievement, "\x01\x02\x03", 3);
        records[i].achievement_len = 3;
        strcpy(records[i].last_update_time, "2025-11-14 22:00:00");
    }

    count = CharacAchievement_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "Should successfully add 3 records in bulk");

    Clear_Test_Data();
}

static void Test_BulkDeleteByCharacNos() {
    CharacAchievement record;
    unsigned int ids[3] = {900110, 900111, 900112};
    int i;
    TEST_START("CharacAchievement_BulkDeleteByCharacNos");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        record.charac_no = ids[i];
        memcpy(record.achievement, "\xAA\xBB", 2);
        record.achievement_len = 2;
        strcpy(record.last_update_time, "2025-11-14 23:00:00");
        CharacAchievement_Add(&manager, &record);
    }

    TEST_ASSERT(CharacAchievement_BulkDeleteByCharacNos(&manager, ids, 3) == 0,
                "Should successfully delete 3 records in bulk");

    TEST_ASSERT(CharacAchievement_Exists(&manager, 900110) == 0,
                "First record should be deleted");
    TEST_ASSERT(CharacAchievement_Exists(&manager, 900111) == 0,
                "Second record should be deleted");
    TEST_ASSERT(CharacAchievement_Exists(&manager, 900112) == 0,
                "Third record should be deleted");

    Clear_Test_Data();
}

/*******************************************************************************
 * 工具函数测试
 ******************************************************************************/

static void Test_PrintInfo() {
    CharacAchievement record;
    TEST_START("CharacAchievement_PrintInfo");

    record.charac_no = 900200;
    memcpy(record.achievement, "\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    record.achievement_len = 8;
    strcpy(record.last_update_time, "2025-11-14 23:59:59");

    printf("  Testing PrintInfo output:\n");
    CharacAchievement_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo should execute without errors");

    /* 测试 NULL 指针 */
    printf("  Testing PrintInfo with NULL:\n");
    CharacAchievement_PrintInfo(NULL);
    TEST_ASSERT(1, "PrintInfo should handle NULL gracefully");
}

/*******************************************************************************
 * 集成测试
 ******************************************************************************/

static void Test_Integration() {
    CharacAchievement record, retrieved;
    int count, size;
    TEST_START("Integration Test - Complete Workflow");

    Clear_Test_Data();

    /* 1. 添加记录 */
    record.charac_no = 900300;
    memcpy(record.achievement, "\x10\x20\x30\x40\x50\x60\x70\x80\x90\xA0", 10);
    record.achievement_len = 10;
    strcpy(record.last_update_time, "2025-11-14 10:30:00");
    CharacAchievement_Add(&manager, &record);

    /* 2. 验证存在 */
    TEST_ASSERT(CharacAchievement_Exists(&manager, 900300) == 1,
                "Integration: Record should exist after add");

    /* 3. 获取并验证数据 */
    CharacAchievement_Get(&manager, 900300, &retrieved);
    TEST_ASSERT(memcmp(retrieved.achievement, record.achievement, 10) == 0,
                "Integration: Retrieved achievement data should match original");

    /* 4. 获取数据大小 */
    size = CharacAchievement_GetAchievementSize(&manager, 900300);
    TEST_ASSERT(size == 10, "Integration: Should get correct achievement size");

    /* 5. 更新数据 */
    unsigned char new_data[] = "\xFF\xEE\xDD\xCC\xBB";
    CharacAchievement_Update(&manager, 900300, new_data, 5);
    CharacAchievement_Get(&manager, 900300, &retrieved);
    TEST_ASSERT(retrieved.achievement_len == 5, "Integration: Data should be updated");

    /* 6. 更新时间戳 */
    CharacAchievement_UpdateTimestamp(&manager, 900300);
    TEST_ASSERT(1, "Integration: Should update timestamp successfully");

    /* 7. 清空成就 */
    CharacAchievement_ClearAchievement(&manager, 900300);
    size = CharacAchievement_GetAchievementSize(&manager, 900300);
    TEST_ASSERT(size == 0, "Integration: Achievement should be cleared");

    /* 8. 删除记录 */
    CharacAchievement_Delete(&manager, 900300);
    TEST_ASSERT(CharacAchievement_Exists(&manager, 900300) == 0,
                "Integration: Record should not exist after deletion");

    Clear_Test_Data();
}

/*******************************************************************************
 * 主函数
 ******************************************************************************/

int main(int argc, char* argv[]) {
    const char* config_file = "../config/database.cfg";

    printf("=======================================================\n");
    printf("  CharacAchievement ORM 模块测试套件\n");
    printf("=======================================================\n\n");

    if (argc > 1) {
        config_file = argv[1];
    }

    if (Init_Database(config_file) < 0) {
        printf("Failed to initialize database connection\n");
        return 1;
    }

    printf("\n--- Running CRUD Tests ---\n");
    Test_Add();
    Test_Get();
    Test_Update();
    Test_Delete();
    Test_Exists();

    printf("\n--- Running Business Query Tests ---\n");
    Test_GetByLastUpdateTimeRange();
    Test_GetRecentUpdates();
    Test_GetEmptyAchievements();
    Test_GetNonEmptyAchievements();
    Test_GetAchievementSize();
    Test_GetByAchievementSize();
    Test_ClearAchievement();
    Test_CountNonEmptyAchievements();
    Test_UpdateTimestamp();

    printf("\n--- Running Bulk Operation Tests ---\n");
    Test_BulkAdd();
    Test_BulkDeleteByCharacNos();

    printf("\n--- Running Utility Function Tests ---\n");
    Test_PrintInfo();

    printf("\n--- Running Integration Test ---\n");
    Test_Integration();

    printf("\n=======================================================\n");
    printf("  测试完成统计\n");
    printf("=======================================================\n");
    printf("  总测试数: %d\n", total_tests);
    printf("  通过: %d\n", passed_tests);
    printf("  失败: %d\n", failed_tests);
    printf("  通过率: %.1f%%\n", (float)passed_tests / total_tests * 100);
    printf("=======================================================\n");

    DBConnectionManager_Cleanup(&manager);

    return (failed_tests > 0) ? 1 : 0;
}
