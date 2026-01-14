#include "member_info_euckr_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test data cleanup */
void cleanup_test_data(DBConnectionManager* manager);

/* Test functions */
void test_add_member(DBConnectionManager* manager);
void test_get_member(DBConnectionManager* manager);
void test_update_member(DBConnectionManager* manager);
void test_delete_member(DBConnectionManager* manager);
void test_exists_member(DBConnectionManager* manager);
void test_get_by_userid(DBConnectionManager* manager);
void test_get_by_nickname(DBConnectionManager* manager);
void test_get_by_ssn(DBConnectionManager* manager);
void test_update_password(DBConnectionManager* manager);
void test_update_email(DBConnectionManager* manager);
void test_update_state(DBConnectionManager* manager);
void test_update_last_play_time(DBConnectionManager* manager);

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
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Info EUCKR ORM Test Suite ===\n\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_add_member(&manager);
    test_get_member(&manager);
    test_exists_member(&manager);
    test_get_by_userid(&manager);
    test_get_by_nickname(&manager);
    test_get_by_ssn(&manager);
    test_update_password(&manager);
    test_update_email(&manager);
    test_update_state(&manager);
    test_update_last_play_time(&manager);
    test_update_member(&manager);
    test_delete_member(&manager);

    /* Clean up after tests */
    cleanup_test_data(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_euckr WHERE user_id IN ('test_user_001', 'test_user_002')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_member(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Add member\n");

    memset(&member, 0, sizeof(MemberInfoEuckr));
    strncpy(member.user_id, "test_user_001", sizeof(member.user_id) - 1);
    strncpy(member.user_name, "TestUser", sizeof(member.user_name) - 1);
    strncpy(member.first_ssn, "900101", sizeof(member.first_ssn) - 1);
    strncpy(member.second_ssn, "1234567", sizeof(member.second_ssn) - 1);
    strncpy(member.passwd, "hashed_password_here", sizeof(member.passwd) - 1);
    strncpy(member.mobile_no, "010-1234-5678", sizeof(member.mobile_no) - 1);
    member.reg_date = (int)time(NULL);
    strncpy(member.email, "test@example.com", sizeof(member.email) - 1);
    member.q_no = 1;
    strncpy(member.q_answer, "My Answer", sizeof(member.q_answer) - 1);
    member.state = 1;
    strncpy(member.nickname, "TestNick", sizeof(member.nickname) - 1);
    member.email_yn = 'y';
    member.ssn_check = 1;
    member.slot = 8;
    strncpy(member.last_play_time, "2025-11-17 10:00:00", sizeof(member.last_play_time) - 1);
    member.hangame_flag = 0;

    if (MemberInfoEuckr_Add(manager, &member) == 0) {
        printf("  PASS: Member added successfully\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add member\n");
        test_failed++;
    }
}

void test_get_member(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Get member by m_id\n");

    /* First get member by user_id to obtain m_id */
    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        unsigned int m_id = member.m_id;

        /* Now test Get by m_id */
        if (MemberInfoEuckr_Get(manager, m_id, &member) == 0) {
            printf("  PASS: Retrieved member (m_id=%u, user_id=%s)\n", member.m_id, member.user_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to retrieve member by m_id\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for m_id test\n");
        test_failed++;
    }
}

void test_exists_member(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Check member exists\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_Exists(manager, member.m_id)) {
            printf("  PASS: Member exists (m_id=%u)\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Member should exist\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for exists test\n");
        test_failed++;
    }
}

void test_get_by_userid(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Get member by user_id\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        printf("  PASS: Retrieved member by user_id (m_id=%u, nickname=%s)\n",
               member.m_id, member.nickname);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get member by user_id\n");
        test_failed++;
    }
}

void test_get_by_nickname(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Get member by nickname\n");

    if (MemberInfoEuckr_GetByNickname(manager, "TestNick", &member) == 0) {
        printf("  PASS: Retrieved member by nickname (m_id=%u, user_id=%s)\n",
               member.m_id, member.user_id);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get member by nickname\n");
        test_failed++;
    }
}

void test_get_by_ssn(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Get member by SSN\n");

    if (MemberInfoEuckr_GetBySSN(manager, "900101", "1234567", &member) == 0) {
        printf("  PASS: Retrieved member by SSN (m_id=%u, user_id=%s)\n",
               member.m_id, member.user_id);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get member by SSN\n");
        test_failed++;
    }
}

void test_update_password(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Update password\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_UpdatePassword(manager, member.m_id, "new_hashed_password") == 0) {
            printf("  PASS: Password updated for m_id=%u\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to update password\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for password update test\n");
        test_failed++;
    }
}

void test_update_email(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Update email\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_UpdateEmail(manager, member.m_id, "newemail@example.com") == 0) {
            printf("  PASS: Email updated for m_id=%u\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to update email\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for email update test\n");
        test_failed++;
    }
}

void test_update_state(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Update state\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_UpdateState(manager, member.m_id, 2) == 0) {
            printf("  PASS: State updated for m_id=%u\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to update state\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for state update test\n");
        test_failed++;
    }
}

void test_update_last_play_time(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Update last play time\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_UpdateLastPlayTime(manager, member.m_id, "2025-11-17 15:30:00") == 0) {
            printf("  PASS: Last play time updated for m_id=%u\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to update last play time\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for play time update test\n");
        test_failed++;
    }
}

void test_update_member(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Update full member record\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        strncpy(member.nickname, "UpdatedNick", sizeof(member.nickname) - 1);
        member.slot = 12;

        if (MemberInfoEuckr_Update(manager, &member) == 0) {
            printf("  PASS: Member record updated\n");
            test_passed++;
        } else {
            printf("  FAIL: Failed to update member record\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for update\n");
        test_failed++;
    }
}

void test_delete_member(DBConnectionManager* manager) {
    MemberInfoEuckr member;

    printf("[TEST] Delete member\n");

    if (MemberInfoEuckr_GetByUserId(manager, "test_user_001", &member) == 0) {
        if (MemberInfoEuckr_Delete(manager, member.m_id) == 0) {
            printf("  PASS: Member deleted (m_id=%u)\n", member.m_id);
            test_passed++;
        } else {
            printf("  FAIL: Failed to delete member\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get member for delete test\n");
        test_failed++;
    }
}
