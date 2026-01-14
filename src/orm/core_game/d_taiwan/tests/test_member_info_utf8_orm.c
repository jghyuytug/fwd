#include "member_info_utf8_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("[PASS] %s\n", message); \
        test_passed++; \
    } else { \
        printf("[FAIL] %s\n", message); \
        test_failed++; \
    } \
} while(0)

void test_basic_crud(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to TAIWAN database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Info UTF8 ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);

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
        "DELETE FROM member_info_utf8 WHERE m_id IN (999001, 999002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberInfoUtf8 member;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add member */
    memset(&member, 0, sizeof(MemberInfoUtf8));
    member.m_id = 999001;
    strcpy(member.user_id, "testuser_utf8");
    strcpy(member.user_name, "TestUTF8");
    strcpy(member.first_ssn, "900101");
    strcpy(member.second_ssn, "1234567");
    strcpy(member.passwd, "md5hashpassword");
    strcpy(member.mobile_no, "010-1234-5678");
    member.reg_date = (unsigned int)time(NULL);
    strcpy(member.email, "testutf8@example.com");
    member.q_no = 1;
    strcpy(member.q_answer, "answer1");
    member.state = 1;
    strcpy(member.nickname, "NickUTF8");
    member.email_yn = 'y';
    member.ssn_check = 1;
    member.slot = 8;
    strcpy(member.last_play_time, "2025-11-17 10:00:00");
    member.hangame_flag = 0;
    member.hanmon_flag = 0;

    ret = MemberInfoUtf8_Add(manager, &member);
    TEST_ASSERT(ret == 0, "Add member (m_id=999001)");

    /* Test 2: Check existence */
    ret = MemberInfoUtf8_Exists(manager, 999001);
    TEST_ASSERT(ret == 1, "Member exists (m_id=999001)");

    /* Test 3: Get member by m_id */
    memset(&member, 0, sizeof(MemberInfoUtf8));
    ret = MemberInfoUtf8_Get(manager, 999001, &member);
    TEST_ASSERT(ret == 0, "Get member by m_id");
    TEST_ASSERT(member.m_id == 999001, "Verify m_id = 999001");
    TEST_ASSERT(strcmp(member.user_id, "testuser_utf8") == 0, "Verify user_id");
    TEST_ASSERT(strcmp(member.nickname, "NickUTF8") == 0, "Verify nickname");
    TEST_ASSERT(member.slot == 8, "Verify slot = 8");

    /* Test 4: Get by user_id */
    memset(&member, 0, sizeof(MemberInfoUtf8));
    ret = MemberInfoUtf8_GetByUserId(manager, "testuser_utf8", &member);
    TEST_ASSERT(ret == 0, "Get member by user_id");
    TEST_ASSERT(member.m_id == 999001, "Verify m_id via user_id");

    /* Test 5: Get by nickname */
    memset(&member, 0, sizeof(MemberInfoUtf8));
    ret = MemberInfoUtf8_GetByNickname(manager, "NickUTF8", &member);
    TEST_ASSERT(ret == 0, "Get member by nickname");
    TEST_ASSERT(member.m_id == 999001, "Verify m_id via nickname");

    /* Test 6: Update password */
    ret = MemberInfoUtf8_UpdatePassword(manager, 999001, "newmd5hash");
    TEST_ASSERT(ret == 0, "Update password");

    memset(&member, 0, sizeof(MemberInfoUtf8));
    ret = MemberInfoUtf8_Get(manager, 999001, &member);
    TEST_ASSERT(strcmp(member.passwd, "newmd5hash") == 0, "Verify updated password");

    /* Test 7: Update slot */
    ret = MemberInfoUtf8_UpdateSlot(manager, 999001, 12);
    TEST_ASSERT(ret == 0, "Update slot");

    memset(&member, 0, sizeof(MemberInfoUtf8));
    ret = MemberInfoUtf8_Get(manager, 999001, &member);
    TEST_ASSERT(member.slot == 12, "Verify updated slot = 12");

    /* Test 8: PrintInfo */
    printf("\n[INFO] Testing MemberInfoUtf8_PrintInfo:\n");
    MemberInfoUtf8_PrintInfo(&member);

    /* Test 9: Delete member */
    ret = MemberInfoUtf8_Delete(manager, 999001);
    TEST_ASSERT(ret == 0, "Delete member (m_id=999001)");

    ret = MemberInfoUtf8_Exists(manager, 999001);
    TEST_ASSERT(ret == 0, "Verify member deleted");

    printf("\n");
}
