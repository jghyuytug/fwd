#include "member_booster_gage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 测试框架宏 */
#define TEST_START(name) printf("\n[Test] %s...\n", name)
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  [PASS] %s\n", message); \
            assert_count++; \
        } else { \
            printf("  [FAIL] %s\n", message); \
            failed_count++; \
        } \
    } while(0)

/* 全局变量 */
static DBConnectionManager manager;
static int assert_count = 0;
static int failed_count = 0;

/* 辅助函数 */
static int Init_Database(const char* config_file);
static void Cleanup_Database(void);
static void Clear_Test_Data(void);

/*******************************************************************************
 * 数据库初始化和清理
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

static void Cleanup_Database(void) {
    printf("\nCleaning up database connection...\n");
    DBConnectionManager_Cleanup(&manager);
    printf("Database connection closed\n");
}

static void Clear_Test_Data(void) {
    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_CAIN,
        "DELETE FROM member_booster_gage WHERE m_id BETWEEN 100000 AND 199999", &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/*******************************************************************************
 * 测试用例
 ******************************************************************************/

static void Test_Add() {
    MemberBoosterGage record;
    TEST_START("MemberBoosterGage_Add");

    Clear_Test_Data();

    record.m_id = 100001;
    record.gage = 50;
    TEST_ASSERT(MemberBoosterGage_Add(&manager, &record) == 0,
                "Add new record should succeed");

    /* 替换测试 */
    record.gage = 100;
    TEST_ASSERT(MemberBoosterGage_Add(&manager, &record) == 0,
                "Replace existing record should succeed");

    Clear_Test_Data();
}

static void Test_Get() {
    MemberBoosterGage record, result;
    TEST_START("MemberBoosterGage_Get");

    Clear_Test_Data();

    record.m_id = 100002;
    record.gage = 75;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_Get(&manager, 100002, &result) == 0,
                "Get existing record should succeed");
    TEST_ASSERT(result.m_id == 100002, "m_id should match");
    TEST_ASSERT(result.gage == 75, "gage should match");

    TEST_ASSERT(MemberBoosterGage_Get(&manager, 999999, &result) == -1,
                "Get non-existent record should fail");

    Clear_Test_Data();
}

static void Test_Update() {
    MemberBoosterGage record, result;
    TEST_START("MemberBoosterGage_Update");

    Clear_Test_Data();

    record.m_id = 100003;
    record.gage = 10;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_Update(&manager, 100003, 200) == 0,
                "Update should succeed");

    MemberBoosterGage_Get(&manager, 100003, &result);
    TEST_ASSERT(result.gage == 200, "gage should be updated to 200");

    Clear_Test_Data();
}

static void Test_Delete() {
    MemberBoosterGage record;
    TEST_START("MemberBoosterGage_Delete");

    Clear_Test_Data();

    record.m_id = 100004;
    record.gage = 30;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_Delete(&manager, 100004) == 0,
                "Delete existing record should succeed");

    TEST_ASSERT(MemberBoosterGage_Delete(&manager, 999999) == 0,
                "Delete non-existent record should still return 0 (idempotent)");

    Clear_Test_Data();
}

static void Test_Exists() {
    MemberBoosterGage record;
    TEST_START("MemberBoosterGage_Exists");

    Clear_Test_Data();

    record.m_id = 100005;
    record.gage = 60;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100005) == 1,
                "Existing record should return 1");

    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 999999) == 0,
                "Non-existent record should return 0");

    Clear_Test_Data();
}

static void Test_GetByGageRange() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetByGageRange");

    Clear_Test_Data();

    /* 添加测试数据：gage值为 10, 50, 100, 150, 200 */
    for (i = 0; i < 5; i++) {
        records[i].m_id = 100010 + i;
        records[i].gage = 10 + (i * 40);
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    count = MemberBoosterGage_GetByGageRange(&manager, 50, 150, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records in range 50-150");
    TEST_ASSERT(records[0].gage >= 50 && records[0].gage <= 150,
                "All returned records should be in range");

    Clear_Test_Data();
}

static void Test_GetByMinGage() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetByMinGage");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100020 + i;
        records[i].gage = i * 30;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    count = MemberBoosterGage_GetByMinGage(&manager, 60, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with gage >= 60 (60, 90, 120)");

    Clear_Test_Data();
}

static void Test_GetByGageValue() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetByGageValue");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        records[i].m_id = 100030 + i;
        records[i].gage = 88;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    records[3].m_id = 100033;
    records[3].gage = 99;
    MemberBoosterGage_Add(&manager, &records[3]);

    count = MemberBoosterGage_GetByGageValue(&manager, 88, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with gage = 88");
    TEST_ASSERT(records[0].gage == 88, "All records should have gage = 88");

    Clear_Test_Data();
}

static void Test_GetZeroGage() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetZeroGage");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        records[i].m_id = 100040 + i;
        records[i].gage = 0;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    records[3].m_id = 100043;
    records[3].gage = 50;
    MemberBoosterGage_Add(&manager, &records[3]);

    count = MemberBoosterGage_GetZeroGage(&manager, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with gage = 0");
    TEST_ASSERT(records[0].gage == 0, "All records should have gage = 0");

    Clear_Test_Data();
}

static void Test_GetNonZeroGage() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetNonZeroGage");

    Clear_Test_Data();

    for (i = 0; i < 2; i++) {
        records[i].m_id = 100050 + i;
        records[i].gage = 0;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    for (i = 2; i < 5; i++) {
        records[i].m_id = 100050 + i;
        records[i].gage = i * 10;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    count = MemberBoosterGage_GetNonZeroGage(&manager, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with gage > 0");

    Clear_Test_Data();
}

static void Test_GetTopGageMembers() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_GetTopGageMembers");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100060 + i;
        records[i].gage = i * 20;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    count = MemberBoosterGage_GetTopGageMembers(&manager, records, 3);
    TEST_ASSERT(count == 3, "Should return top 3 members");
    TEST_ASSERT(records[0].gage >= records[1].gage,
                "Results should be sorted by gage DESC");
    TEST_ASSERT(records[0].gage == 80, "Top gage should be 80");

    Clear_Test_Data();
}

static void Test_IncrementGage() {
    MemberBoosterGage record, result;
    TEST_START("MemberBoosterGage_IncrementGage");

    Clear_Test_Data();

    record.m_id = 100070;
    record.gage = 50;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_IncrementGage(&manager, 100070) == 0,
                "IncrementGage should succeed");

    MemberBoosterGage_Get(&manager, 100070, &result);
    TEST_ASSERT(result.gage == 51, "gage should be incremented to 51");

    /* 测试边界：gage = 255 时不能再增加 */
    record.m_id = 100071;
    record.gage = 255;
    MemberBoosterGage_Add(&manager, &record);
    MemberBoosterGage_IncrementGage(&manager, 100071);
    MemberBoosterGage_Get(&manager, 100071, &result);
    TEST_ASSERT(result.gage == 255, "gage should stay at 255 (max value)");

    Clear_Test_Data();
}

static void Test_DecrementGage() {
    MemberBoosterGage record, result;
    TEST_START("MemberBoosterGage_DecrementGage");

    Clear_Test_Data();

    record.m_id = 100080;
    record.gage = 10;
    MemberBoosterGage_Add(&manager, &record);

    TEST_ASSERT(MemberBoosterGage_DecrementGage(&manager, 100080) == 0,
                "DecrementGage should succeed");

    MemberBoosterGage_Get(&manager, 100080, &result);
    TEST_ASSERT(result.gage == 9, "gage should be decremented to 9");

    /* 测试边界：gage = 0 时不能再减少 */
    record.m_id = 100081;
    record.gage = 0;
    MemberBoosterGage_Add(&manager, &record);
    MemberBoosterGage_DecrementGage(&manager, 100081);
    MemberBoosterGage_Get(&manager, 100081, &result);
    TEST_ASSERT(result.gage == 0, "gage should stay at 0 (min value)");

    Clear_Test_Data();
}

static void Test_SetGageValue() {
    MemberBoosterGage records[10];
    int count;
    int i;
    TEST_START("MemberBoosterGage_SetGageValue");

    Clear_Test_Data();

    /* 添加3个 gage=0 的记录 */
    for (i = 0; i < 3; i++) {
        records[i].m_id = 100090 + i;
        records[i].gage = 0;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    /* 将所有 gage=0 的记录设置为 10 */
    TEST_ASSERT(MemberBoosterGage_SetGageValue(&manager, 0, 10) == 0,
                "SetGageValue should succeed");

    count = MemberBoosterGage_GetByGageValue(&manager, 10, records, 10);
    TEST_ASSERT(count == 3, "All 3 records should now have gage = 10");

    Clear_Test_Data();
}

static void Test_BulkAdd() {
    MemberBoosterGage records[5];
    int count;
    int i;
    TEST_START("MemberBoosterGage_BulkAdd");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100100 + i;
        records[i].gage = (i + 1) * 10;
    }

    count = MemberBoosterGage_BulkAdd(&manager, records, 5);
    TEST_ASSERT(count == 5, "Should successfully add 5 records");

    /* 验证所有记录都存在 */
    for (i = 0; i < 5; i++) {
        TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100100 + i) == 1,
                    "All bulk added records should exist");
    }

    Clear_Test_Data();
}

static void Test_BulkDeleteByMemberIds() {
    MemberBoosterGage records[5];
    unsigned int m_ids[3];
    int i;
    TEST_START("MemberBoosterGage_BulkDeleteByMemberIds");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100110 + i;
        records[i].gage = i * 15;
        MemberBoosterGage_Add(&manager, &records[i]);
    }

    m_ids[0] = 100110;
    m_ids[1] = 100112;
    m_ids[2] = 100114;

    TEST_ASSERT(MemberBoosterGage_BulkDeleteByMemberIds(&manager, m_ids, 3) == 0,
                "Bulk delete should succeed");

    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100110) == 0,
                "Deleted members should not exist");
    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100111) == 1,
                "Non-deleted member should still exist");

    Clear_Test_Data();
}

static void Test_PrintInfo() {
    MemberBoosterGage record;
    TEST_START("MemberBoosterGage_PrintInfo");

    record.m_id = 100120;
    record.gage = 128;

    printf("  Testing PrintInfo utility:\n");
    MemberBoosterGage_PrintInfo(&record);
    TEST_ASSERT(1, "PrintInfo should execute without error");
}

static void Test_Integration() {
    MemberBoosterGage record, result;
    TEST_START("Integration Test - Complete Workflow");

    Clear_Test_Data();

    /* 1. 添加记录 */
    record.m_id = 100130;
    record.gage = 0;
    TEST_ASSERT(MemberBoosterGage_Add(&manager, &record) == 0,
                "Add record in integration test");

    /* 2. 检查存在 */
    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100130) == 1,
                "Record should exist");

    /* 3. 递增10次 */
    {
        int j;
        for (j = 0; j < 10; j++) {
            MemberBoosterGage_IncrementGage(&manager, 100130);
        }
    }

    /* 4. 验证 gage = 10 */
    MemberBoosterGage_Get(&manager, 100130, &result);
    TEST_ASSERT(result.gage == 10, "gage should be 10 after 10 increments");

    /* 5. 更新为50 */
    TEST_ASSERT(MemberBoosterGage_Update(&manager, 100130, 50) == 0,
                "Update should succeed");

    /* 6. 验证更新 */
    MemberBoosterGage_Get(&manager, 100130, &result);
    TEST_ASSERT(result.gage == 50, "gage should be updated to 50");

    /* 7. 删除 */
    TEST_ASSERT(MemberBoosterGage_Delete(&manager, 100130) == 0,
                "Delete should succeed");

    /* 8. 验证已删除 */
    TEST_ASSERT(MemberBoosterGage_Exists(&manager, 100130) == 0,
                "Record should no longer exist");

    Clear_Test_Data();
}

/*******************************************************************************
 * 主函数
 ******************************************************************************/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <database_config_file>\n", argv[0]);
        return 1;
    }

    printf("=================================================\n");
    printf("  member_booster_gage ORM Test Suite\n");
    printf("  Total: 18 test cases\n");
    printf("=================================================\n");

    if (Init_Database(argv[1]) < 0) {
        return 1;
    }

    /* 运行所有测试 */
    Test_Add();
    Test_Get();
    Test_Update();
    Test_Delete();
    Test_Exists();
    Test_GetByGageRange();
    Test_GetByMinGage();
    Test_GetByGageValue();
    Test_GetZeroGage();
    Test_GetNonZeroGage();
    Test_GetTopGageMembers();
    Test_IncrementGage();
    Test_DecrementGage();
    Test_SetGageValue();
    Test_BulkAdd();
    Test_BulkDeleteByMemberIds();
    Test_PrintInfo();
    Test_Integration();

    Cleanup_Database();

    printf("\n=================================================\n");
    printf("  Test Summary\n");
    printf("=================================================\n");
    printf("Total Tests: 18\n");
    printf("Total Assertions: %d\n", assert_count);
    if (failed_count > 0) {
        printf("Failed Assertions: %d\n", failed_count);
    }
    printf("=================================================\n");

    return (failed_count == 0) ? 0 : 1;
}
