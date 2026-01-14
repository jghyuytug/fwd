#include "../include/database/headers/member_dungeon_orm.h"
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
        "DELETE FROM member_dungeon WHERE m_id BETWEEN 100000 AND 199999", &result);
    DBQueryResult_Free(&result);
    printf("Test data cleared\n");
}

/*******************************************************************************
 * CRUD 操作测试
 ******************************************************************************/

static void Test_Add() {
    MemberDungeon record;
    TEST_START("MemberDungeon_Add");

    Clear_Test_Data();

    record.m_id = 100001;
    strcpy(record.dungeon, "{\"dungeons\":[1,2,3],\"progress\":50}");

    TEST_ASSERT(MemberDungeon_Add(&manager, &record) == 0,
                "Should successfully add a record");

    /* 测试幂等性：重复添加相同 m_id */
    strcpy(record.dungeon, "{\"dungeons\":[4,5,6],\"progress\":75}");
    TEST_ASSERT(MemberDungeon_Add(&manager, &record) == 0,
                "Should successfully update existing record (REPLACE)");

    Clear_Test_Data();
}

static void Test_Get() {
    MemberDungeon record, retrieved;
    TEST_START("MemberDungeon_Get");

    Clear_Test_Data();

    record.m_id = 100002;
    strcpy(record.dungeon, "{\"level\":10,\"cleared\":[1,2,3,4,5]}");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_Get(&manager, 100002, &retrieved) == 0,
                "Should retrieve existing record");
    TEST_ASSERT(retrieved.m_id == 100002,
                "Should have correct m_id");
    TEST_ASSERT(strcmp(retrieved.dungeon, record.dungeon) == 0,
                "Should have matching dungeon data");

    TEST_ASSERT(MemberDungeon_Get(&manager, 999999, &retrieved) < 0,
                "Should fail for non-existent record");

    Clear_Test_Data();
}

static void Test_Update() {
    MemberDungeon record;
    TEST_START("MemberDungeon_Update");

    Clear_Test_Data();

    record.m_id = 100003;
    strcpy(record.dungeon, "old_data");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_Update(&manager, 100003, "new_dungeon_data") == 0,
                "Should successfully update dungeon data");

    MemberDungeon_Get(&manager, 100003, &record);
    TEST_ASSERT(strcmp(record.dungeon, "new_dungeon_data") == 0,
                "Should have updated dungeon data");

    Clear_Test_Data();
}

static void Test_Delete() {
    MemberDungeon record;
    TEST_START("MemberDungeon_Delete");

    Clear_Test_Data();

    record.m_id = 100004;
    strcpy(record.dungeon, "test_data");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_Delete(&manager, 100004) == 0,
                "Should successfully delete record");

    TEST_ASSERT(MemberDungeon_Get(&manager, 100004, &record) < 0,
                "Record should not exist after deletion");

    /* 测试删除不存在的记录（应该成功，返回0） */
    TEST_ASSERT(MemberDungeon_Delete(&manager, 999999) == 0,
                "Should return 0 even for non-existent record");

    Clear_Test_Data();
}

static void Test_Exists() {
    MemberDungeon record;
    TEST_START("MemberDungeon_Exists");

    Clear_Test_Data();

    record.m_id = 100005;
    strcpy(record.dungeon, "exists_test");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_Exists(&manager, 100005) == 1,
                "Should return 1 for existing record");

    TEST_ASSERT(MemberDungeon_Exists(&manager, 999999) == 0,
                "Should return 0 for non-existent record");

    Clear_Test_Data();
}

/*******************************************************************************
 * 业务查询测试
 ******************************************************************************/

static void Test_GetEmptyDungeonMembers() {
    MemberDungeon records[10];
    int count;
    int i;
    TEST_START("MemberDungeon_GetEmptyDungeonMembers");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100010 + i;
        if (i < 2) {
            strcpy(records[i].dungeon, "");  /* 空数据 */
        } else {
            strcpy(records[i].dungeon, "has_data");
        }
        MemberDungeon_Add(&manager, &records[i]);
    }

    count = MemberDungeon_GetEmptyDungeonMembers(&manager, records, 10);
    TEST_ASSERT(count == 2, "Should find 2 members with empty dungeon data");

    Clear_Test_Data();
}

static void Test_GetNonEmptyDungeonMembers() {
    MemberDungeon records[10];
    int count;
    int i;
    TEST_START("MemberDungeon_GetNonEmptyDungeonMembers");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100020 + i;
        if (i < 3) {
            strcpy(records[i].dungeon, "dungeon_data");
        } else {
            strcpy(records[i].dungeon, "");
        }
        MemberDungeon_Add(&manager, &records[i]);
    }

    count = MemberDungeon_GetNonEmptyDungeonMembers(&manager, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 members with non-empty dungeon data");

    Clear_Test_Data();
}

static void Test_GetDungeonLength() {
    MemberDungeon record;
    int length;
    TEST_START("MemberDungeon_GetDungeonLength");

    Clear_Test_Data();

    record.m_id = 100030;
    strcpy(record.dungeon, "12345678901234567890");  /* 20 characters */
    MemberDungeon_Add(&manager, &record);

    length = MemberDungeon_GetDungeonLength(&manager, 100030);
    TEST_ASSERT(length == 20, "Should return correct dungeon data length");

    Clear_Test_Data();
}

static void Test_SearchDungeonByKeyword() {
    MemberDungeon records[10];
    int count;
    int i;
    TEST_START("MemberDungeon_SearchDungeonByKeyword");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100040 + i;
        if (i < 2) {
            strcpy(records[i].dungeon, "keyword_found_here");
        } else {
            strcpy(records[i].dungeon, "other_data");
        }
        MemberDungeon_Add(&manager, &records[i]);
    }

    count = MemberDungeon_SearchDungeonByKeyword(&manager, "keyword", records, 10);
    TEST_ASSERT(count == 2, "Should find 2 records containing keyword");

    Clear_Test_Data();
}

static void Test_ClearDungeonData() {
    MemberDungeon record;
    TEST_START("MemberDungeon_ClearDungeonData");

    Clear_Test_Data();

    record.m_id = 100050;
    strcpy(record.dungeon, "some_dungeon_data");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_ClearDungeonData(&manager, 100050) == 0,
                "Should successfully clear dungeon data");

    MemberDungeon_Get(&manager, 100050, &record);
    TEST_ASSERT(strlen(record.dungeon) == 0,
                "Dungeon data should be empty after clearing");

    Clear_Test_Data();
}

static void Test_AppendDungeonData() {
    MemberDungeon record;
    TEST_START("MemberDungeon_AppendDungeonData");

    Clear_Test_Data();

    record.m_id = 100060;
    strcpy(record.dungeon, "initial_");
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_AppendDungeonData(&manager, 100060, "appended") == 0,
                "Should successfully append data");

    MemberDungeon_Get(&manager, 100060, &record);
    TEST_ASSERT(strcmp(record.dungeon, "initial_appended") == 0,
                "Should have appended data correctly");

    Clear_Test_Data();
}

static void Test_GetMembersByDungeonSize() {
    MemberDungeon records[10];
    int count;
    int i;
    TEST_START("MemberDungeon_GetMembersByDungeonSize");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100070 + i;
        /* 创建不同长度的数据: 5, 10, 15, 20, 25 字符 */
        memset(records[i].dungeon, 'x', (i + 1) * 5);
        records[i].dungeon[(i + 1) * 5] = '\0';
        MemberDungeon_Add(&manager, &records[i]);
    }

    count = MemberDungeon_GetMembersByDungeonSize(&manager, 10, 20, records, 10);
    TEST_ASSERT(count == 3, "Should find 3 records with dungeon size between 10 and 20");

    Clear_Test_Data();
}

static void Test_CountNonEmptyDungeons() {
    MemberDungeon records[10];
    int count;
    int i;
    TEST_START("MemberDungeon_CountNonEmptyDungeons");

    Clear_Test_Data();

    for (i = 0; i < 5; i++) {
        records[i].m_id = 100080 + i;
        if (i < 3) {
            strcpy(records[i].dungeon, "has_data");
        } else {
            strcpy(records[i].dungeon, "");
        }
        MemberDungeon_Add(&manager, &records[i]);
    }

    count = MemberDungeon_CountNonEmptyDungeons(&manager);
    TEST_ASSERT(count == 3, "Should count 3 members with non-empty dungeon data");

    Clear_Test_Data();
}

static void Test_TruncateDungeonData() {
    MemberDungeon record;
    TEST_START("MemberDungeon_TruncateDungeonData");

    Clear_Test_Data();

    record.m_id = 100090;
    strcpy(record.dungeon, "1234567890abcdefghij");  /* 20 characters */
    MemberDungeon_Add(&manager, &record);

    TEST_ASSERT(MemberDungeon_TruncateDungeonData(&manager, 100090, 10) == 0,
                "Should successfully truncate dungeon data");

    MemberDungeon_Get(&manager, 100090, &record);
    TEST_ASSERT(strlen(record.dungeon) == 10,
                "Dungeon data should be truncated to 10 characters");
    TEST_ASSERT(strcmp(record.dungeon, "1234567890") == 0,
                "Should have correct truncated data");

    Clear_Test_Data();
}

/*******************************************************************************
 * 批量操作测试
 ******************************************************************************/

static void Test_BulkAdd() {
    MemberDungeon records[3];
    int count;
    int i;
    TEST_START("MemberDungeon_BulkAdd");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        records[i].m_id = 100100 + i;
        snprintf(records[i].dungeon, sizeof(records[i].dungeon), "bulk_data_%d", i);
    }

    count = MemberDungeon_BulkAdd(&manager, records, 3);
    TEST_ASSERT(count == 3, "Should successfully add 3 records in bulk");

    Clear_Test_Data();
}

static void Test_BulkDeleteByMemberIds() {
    MemberDungeon record;
    unsigned int ids[3] = {100110, 100111, 100112};
    int i;
    TEST_START("MemberDungeon_BulkDeleteByMemberIds");

    Clear_Test_Data();

    for (i = 0; i < 3; i++) {
        record.m_id = ids[i];
        strcpy(record.dungeon, "bulk_delete_test");
        MemberDungeon_Add(&manager, &record);
    }

    TEST_ASSERT(MemberDungeon_BulkDeleteByMemberIds(&manager, ids, 3) == 0,
                "Should successfully delete 3 records in bulk");

    TEST_ASSERT(MemberDungeon_Exists(&manager, 100110) == 0,
                "First record should be deleted");
    TEST_ASSERT(MemberDungeon_Exists(&manager, 100111) == 0,
                "Second record should be deleted");
    TEST_ASSERT(MemberDungeon_Exists(&manager, 100112) == 0,
                "Third record should be deleted");

    Clear_Test_Data();
}

/*******************************************************************************
 * 工具函数测试
 ******************************************************************************/

static void Test_PrintInfo() {
    MemberDungeon record;
    TEST_START("MemberDungeon_PrintInfo");

    record.m_id = 100200;
    strcpy(record.dungeon, "{\"test\":\"data\"}");

    printf("  Testing PrintInfo output:\n");
    MemberDungeon_PrintInfo(&record);

    TEST_ASSERT(1, "PrintInfo should execute without errors");

    /* 测试 NULL 指针 */
    printf("  Testing PrintInfo with NULL:\n");
    MemberDungeon_PrintInfo(NULL);
    TEST_ASSERT(1, "PrintInfo should handle NULL gracefully");
}

/*******************************************************************************
 * 集成测试
 ******************************************************************************/

static void Test_Integration() {
    MemberDungeon record, retrieved;
    int count;
    TEST_START("Integration Test - Complete Workflow");

    Clear_Test_Data();

    /* 1. 添加记录 */
    record.m_id = 100300;
    strcpy(record.dungeon, "{\"dungeons\":[1,2,3,4,5],\"level\":20}");
    MemberDungeon_Add(&manager, &record);

    /* 2. 验证存在 */
    TEST_ASSERT(MemberDungeon_Exists(&manager, 100300) == 1,
                "Integration: Record should exist after add");

    /* 3. 获取并验证数据 */
    MemberDungeon_Get(&manager, 100300, &retrieved);
    TEST_ASSERT(strcmp(retrieved.dungeon, record.dungeon) == 0,
                "Integration: Retrieved data should match original");

    /* 4. 追加数据 */
    MemberDungeon_AppendDungeonData(&manager, 100300, ",extra_data");
    MemberDungeon_Get(&manager, 100300, &retrieved);
    TEST_ASSERT(strstr(retrieved.dungeon, "extra_data") != NULL,
                "Integration: Appended data should be present");

    /* 5. 获取长度 */
    count = MemberDungeon_GetDungeonLength(&manager, 100300);
    TEST_ASSERT(count > 0, "Integration: Should get valid data length");

    /* 6. 更新数据 */
    MemberDungeon_Update(&manager, 100300, "updated_data");
    MemberDungeon_Get(&manager, 100300, &retrieved);
    TEST_ASSERT(strcmp(retrieved.dungeon, "updated_data") == 0,
                "Integration: Data should be updated");

    /* 7. 删除记录 */
    MemberDungeon_Delete(&manager, 100300);
    TEST_ASSERT(MemberDungeon_Exists(&manager, 100300) == 0,
                "Integration: Record should not exist after deletion");

    Clear_Test_Data();
}

/*******************************************************************************
 * 主函数
 ******************************************************************************/

int main(int argc, char* argv[]) {
    const char* config_file = "../config/database.cfg";

    printf("=======================================================\n");
    printf("  MemberDungeon ORM 模块测试套件\n");
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
    Test_GetEmptyDungeonMembers();
    Test_GetNonEmptyDungeonMembers();
    Test_GetDungeonLength();
    Test_SearchDungeonByKeyword();
    Test_ClearDungeonData();
    Test_AppendDungeonData();
    Test_GetMembersByDungeonSize();
    Test_CountNonEmptyDungeons();
    Test_TruncateDungeonData();

    printf("\n--- Running Bulk Operation Tests ---\n");
    Test_BulkAdd();
    Test_BulkDeleteByMemberIds();

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
