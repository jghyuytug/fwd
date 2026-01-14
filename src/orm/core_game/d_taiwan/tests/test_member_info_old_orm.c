#include "member_info_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("[PASS] %s\n", message); \
        test_passed++; \
    } else { \
        printf("[FAIL] %s\n", message); \
        test_failed++; \
    } \
} while(0)

/* Function prototypes */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to TAIWAN database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Info Old ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_old WHERE user_id IN ('testuser01', 'testuser02', 'testuser03')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberInfoOld member;
    int ret;
    unsigned int test_m_id = 0;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add member */
    memset(&member, 0, sizeof(MemberInfoOld));
    strcpy(member.user_id, "testuser01");
    strcpy(member.user_name, "Test1");
    strcpy(member.first_ssn, "800101");
    strcpy(member.second_ssn, "1234567");
    strcpy(member.passwd, "pass123");
    strcpy(member.mobile_no, "12345678");
    member.reg_date = (unsigned int)time(NULL);
    strcpy(member.email, "test1@example.com");
    member.q_no = 1;
    strcpy(member.q_answer, "answer1");
    member.state = 1;
    strcpy(member.nickname, "Nick1");
    member.email_yn = 'y';
    member.ssn_check = 1;
    strcpy(member.last_play_time, "2025-11-17 10:00:00");

    ret = MemberInfoOld_Add(manager, &member);
    TEST_ASSERT(ret == 0, "Add member (testuser01)");

    /* Get the inserted m_id for later tests */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_GetByUserId(manager, "testuser01", &member);
    if (ret == 0) {
        test_m_id = member.m_id;
        TEST_ASSERT(test_m_id > 0, "Retrieved auto-generated m_id");
    }

    /* Test 2: Check existence */
    ret = MemberInfoOld_Exists(manager, test_m_id);
    TEST_ASSERT(ret == 1, "Member exists (by m_id)");

    ret = MemberInfoOld_Exists(manager, 999999999);
    TEST_ASSERT(ret == 0, "Member not exists (m_id=999999999)");

    /* Test 3: Get member by m_id */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_Get(manager, test_m_id, &member);
    TEST_ASSERT(ret == 0, "Get member by m_id");
    TEST_ASSERT(strcmp(member.user_id, "testuser01") == 0, "Verify user_id");
    TEST_ASSERT(strcmp(member.user_name, "Test1") == 0, "Verify user_name");
    TEST_ASSERT(strcmp(member.nickname, "Nick1") == 0, "Verify nickname");
    TEST_ASSERT(strcmp(member.email, "test1@example.com") == 0, "Verify email");
    TEST_ASSERT(member.state == 1, "Verify state = 1");
    TEST_ASSERT(member.email_yn == 'y', "Verify email_yn = y");

    /* Test 4: Update member */
    strcpy(member.nickname, "NewNick1");
    strcpy(member.email, "newemail@example.com");
    member.email_yn = 'n';

    ret = MemberInfoOld_Update(manager, &member);
    TEST_ASSERT(ret == 0, "Update member");

    /* Verify update */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_Get(manager, test_m_id, &member);
    TEST_ASSERT(strcmp(member.nickname, "NewNick1") == 0, "Verify updated nickname");
    TEST_ASSERT(strcmp(member.email, "newemail@example.com") == 0, "Verify updated email");
    TEST_ASSERT(member.email_yn == 'n', "Verify updated email_yn = n");

    /* Test 5: Update password */
    ret = MemberInfoOld_UpdatePassword(manager, test_m_id, "newpass");
    TEST_ASSERT(ret == 0, "Update password");

    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_Get(manager, test_m_id, &member);
    TEST_ASSERT(strcmp(member.passwd, "newpass") == 0, "Verify updated password");

    /* Test 6: Update last play time */
    ret = MemberInfoOld_UpdateLastPlayTime(manager, test_m_id, "2025-11-17 15:30:00");
    TEST_ASSERT(ret == 0, "Update last play time");

    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_Get(manager, test_m_id, &member);
    TEST_ASSERT(strcmp(member.last_play_time, "2025-11-17 15:30:00") == 0,
                "Verify updated last_play_time");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MemberInfoOld members[10];
    MemberInfoOld member;
    int count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Setup test data */
    memset(&member, 0, sizeof(MemberInfoOld));
    strcpy(member.user_id, "testuser02");
    strcpy(member.user_name, "Test2");
    strcpy(member.first_ssn, "900101");
    strcpy(member.second_ssn, "2234567");
    strcpy(member.passwd, "pass456");
    strcpy(member.mobile_no, "87654321");
    member.reg_date = (unsigned int)time(NULL);
    strcpy(member.email, "test2@example.com");
    member.q_no = 2;
    strcpy(member.q_answer, "answer2");
    member.state = 1;
    strcpy(member.nickname, "Nick2");
    member.email_yn = 'y';
    member.ssn_check = 1;
    strcpy(member.last_play_time, "2025-11-17 11:00:00");
    MemberInfoOld_Add(manager, &member);

    strcpy(member.user_id, "testuser03");
    strcpy(member.user_name, "Test3");
    strcpy(member.first_ssn, "950101");
    strcpy(member.second_ssn, "3234567");
    strcpy(member.nickname, "Nick3");
    strcpy(member.email, "test3@example.com");
    member.state = 2;
    MemberInfoOld_Add(manager, &member);

    /* Test 1: Get by user_id */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_GetByUserId(manager, "testuser02", &member);
    TEST_ASSERT(ret == 0, "Get member by user_id");
    TEST_ASSERT(strcmp(member.user_id, "testuser02") == 0, "Verify user_id = testuser02");
    TEST_ASSERT(strcmp(member.nickname, "Nick2") == 0, "Verify nickname = Nick2");

    /* Test 2: Get by nickname */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_GetByNickname(manager, "Nick3", &member);
    TEST_ASSERT(ret == 0, "Get member by nickname");
    TEST_ASSERT(strcmp(member.user_id, "testuser03") == 0, "Verify user_id = testuser03");
    TEST_ASSERT(member.state == 2, "Verify state = 2");

    /* Test 3: Get by SSN */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_GetBySSN(manager, "900101", "2234567", &member);
    TEST_ASSERT(ret == 0, "Get member by SSN");
    TEST_ASSERT(strcmp(member.user_id, "testuser02") == 0, "Verify user_id via SSN");

    /* Test 4: Get by state */
    ret = MemberInfoOld_GetByState(manager, 1, members, 10, &count);
    TEST_ASSERT(ret == 0, "Get members by state (state=1)");
    TEST_ASSERT(count >= 2, "At least 2 members with state=1");

    ret = MemberInfoOld_GetByState(manager, 2, members, 10, &count);
    TEST_ASSERT(ret == 0, "Get members by state (state=2)");
    TEST_ASSERT(count >= 1, "At least 1 member with state=2");

    /* Test 5: Delete member */
    memset(&member, 0, sizeof(MemberInfoOld));
    ret = MemberInfoOld_GetByUserId(manager, "testuser03", &member);
    if (ret == 0) {
        ret = MemberInfoOld_Delete(manager, member.m_id);
        TEST_ASSERT(ret == 0, "Delete member (testuser03)");

        ret = MemberInfoOld_Exists(manager, member.m_id);
        TEST_ASSERT(ret == 0, "Verify member deleted");
    }

    /* Test 6: PrintInfo utility */
    printf("\n[INFO] Testing MemberInfoOld_PrintInfo:\n");
    memset(&member, 0, sizeof(MemberInfoOld));
    if (MemberInfoOld_GetByUserId(manager, "testuser02", &member) == 0) {
        MemberInfoOld_PrintInfo(&member);
    }

    printf("\n");
}
