#include "member_info_mileage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

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
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Member Info Mileage ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_mileage WHERE user_id IN ('testuser001', 'testuser002')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberInfoMileage member, retrieved;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("--- Test 1: Add Member ---\n");
    memset(&member, 0, sizeof(MemberInfoMileage));
    strcpy(member.user_id, "testuser001");
    strcpy(member.user_name, "TestUser");
    strcpy(member.first_ssn, "123456");
    strcpy(member.second_ssn, "1234567");
    strcpy(member.passwd, "hashed_password_here_123456789");
    strcpy(member.mobile_no, "010-1234-5678");
    member.reg_date = current_time;
    strcpy(member.email, "test@example.com");
    member.q_no = 1;
    strcpy(member.q_answer, "my answer");
    member.state = 1;
    strcpy(member.nickname, "TestNick");
    member.email_yn = 'y';
    member.ssn_check = 1;
    member.slot = 8;
    strcpy(member.last_play_time, "2025-11-17 10:00:00");
    member.hangame_flag = 0;
    member.hanmon_flag = 0;
    member.mileage = 1000;

    ret = MemberInfoMileage_Add(manager, &member);
    if (ret == 0) {
        printf("PASS: Added member successfully\n");
    } else {
        printf("FAIL: Failed to add member (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 2: Get Member by User ID ---\n");
    memset(&retrieved, 0, sizeof(MemberInfoMileage));
    ret = MemberInfoMileage_GetByUserId(manager, "testuser001", &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved member (m_id=%u, user_id=%s)\n",
               retrieved.m_id, retrieved.user_id);
        MemberInfoMileage_PrintInfo(&retrieved);
        /* Save m_id for later tests */
        member.m_id = retrieved.m_id;
    } else {
        printf("FAIL: Failed to retrieve member (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 3: Get Member by m_id ---\n");
    memset(&retrieved, 0, sizeof(MemberInfoMileage));
    ret = MemberInfoMileage_Get(manager, member.m_id, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved member by m_id=%u\n", member.m_id);
    } else {
        printf("FAIL: Failed to retrieve member by m_id (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Update Member ---\n");
    strcpy(retrieved.email, "newemail@example.com");
    retrieved.slot = 12;
    retrieved.mileage = 2000;
    ret = MemberInfoMileage_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated member successfully\n");
    } else {
        printf("FAIL: Failed to update member (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 5: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(MemberInfoMileage));
    ret = MemberInfoMileage_Get(manager, member.m_id, &retrieved);
    if (ret == 0 && strcmp(retrieved.email, "newemail@example.com") == 0 &&
        retrieved.slot == 12 && retrieved.mileage == 2000) {
        printf("PASS: Update verified (email=%s, slot=%u, mileage=%d)\n",
               retrieved.email, retrieved.slot, retrieved.mileage);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 6: Check Exists ---\n");
    ret = MemberInfoMileage_Exists(manager, member.m_id);
    if (ret == 1) {
        printf("PASS: Member exists (m_id=%u)\n", member.m_id);
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 7: Delete Member ---\n");
    ret = MemberInfoMileage_Delete(manager, member.m_id);
    if (ret == 0) {
        printf("PASS: Deleted member successfully\n");
    } else {
        printf("FAIL: Failed to delete member (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 8: Verify Deletion ---\n");
    ret = MemberInfoMileage_Exists(manager, member.m_id);
    if (ret == 0) {
        printf("PASS: Member no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    MemberInfoMileage member;
    int ret;
    int count = 0;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("\n--- Test 9: Add Member for Business Operations ---\n");
    memset(&member, 0, sizeof(MemberInfoMileage));
    strcpy(member.user_id, "testuser002");
    strcpy(member.user_name, "BizTest");
    strcpy(member.first_ssn, "654321");
    strcpy(member.second_ssn, "7654321");
    strcpy(member.passwd, "another_hashed_password_12345");
    strcpy(member.mobile_no, "010-9876-5432");
    member.reg_date = current_time;
    strcpy(member.email, "biz@example.com");
    member.q_no = 2;
    strcpy(member.q_answer, "another answer");
    member.state = 1;
    strcpy(member.nickname, "BizNick");
    member.email_yn = 'n';
    member.ssn_check = 1;
    member.slot = 8;
    strcpy(member.last_play_time, "2025-11-17 09:00:00");
    member.hangame_flag = 1;
    member.hanmon_flag = 0;
    member.mileage = 500;

    ret = MemberInfoMileage_Add(manager, &member);
    if (ret == 0) {
        printf("PASS: Added member for business operations\n");
    } else {
        printf("FAIL: Failed to add member (ret=%d)\n", ret);
        return;
    }

    /* Retrieve to get m_id */
    MemberInfoMileage_GetByUserId(manager, "testuser002", &member);

    printf("\n--- Test 10: Get Member by Nickname ---\n");
    MemberInfoMileage retrieved;
    memset(&retrieved, 0, sizeof(MemberInfoMileage));
    ret = MemberInfoMileage_GetByNickname(manager, "BizNick", &retrieved);
    if (ret == 0 && strcmp(retrieved.user_id, "testuser002") == 0) {
        printf("PASS: Retrieved member by nickname (user_id=%s)\n", retrieved.user_id);
    } else {
        printf("FAIL: Failed to retrieve by nickname (ret=%d)\n", ret);
    }

    printf("\n--- Test 11: Update Mileage (Add Points) ---\n");
    int old_mileage = member.mileage;
    ret = MemberInfoMileage_UpdateMileage(manager, member.m_id, 250);
    if (ret == 0) {
        MemberInfoMileage_Get(manager, member.m_id, &retrieved);
        if (retrieved.mileage == old_mileage + 250) {
            printf("PASS: Mileage updated (old=%d, new=%d)\n",
                   old_mileage, retrieved.mileage);
        } else {
            printf("FAIL: Mileage not updated correctly (old=%d, new=%d)\n",
                   old_mileage, retrieved.mileage);
        }
    } else {
        printf("FAIL: Failed to update mileage (ret=%d)\n", ret);
    }

    printf("\n--- Test 12: Update Mileage (Subtract Points) ---\n");
    old_mileage = retrieved.mileage;
    ret = MemberInfoMileage_UpdateMileage(manager, member.m_id, -100);
    if (ret == 0) {
        MemberInfoMileage_Get(manager, member.m_id, &retrieved);
        if (retrieved.mileage == old_mileage - 100) {
            printf("PASS: Mileage decreased (old=%d, new=%d)\n",
                   old_mileage, retrieved.mileage);
        } else {
            printf("FAIL: Mileage not decreased correctly (old=%d, new=%d)\n",
                   old_mileage, retrieved.mileage);
        }
    } else {
        printf("FAIL: Failed to decrease mileage (ret=%d)\n", ret);
    }

    printf("\n--- Test 13: Update Password ---\n");
    ret = MemberInfoMileage_UpdatePassword(manager, member.m_id, "new_hashed_password_000");
    if (ret == 0) {
        MemberInfoMileage_Get(manager, member.m_id, &retrieved);
        if (strcmp(retrieved.passwd, "new_hashed_password_000") == 0) {
            printf("PASS: Password updated successfully\n");
        } else {
            printf("FAIL: Password not updated correctly\n");
        }
    } else {
        printf("FAIL: Failed to update password (ret=%d)\n", ret);
    }

    printf("\n--- Test 14: Update Email ---\n");
    ret = MemberInfoMileage_UpdateEmail(manager, member.m_id, "updated@example.com");
    if (ret == 0) {
        MemberInfoMileage_Get(manager, member.m_id, &retrieved);
        if (strcmp(retrieved.email, "updated@example.com") == 0) {
            printf("PASS: Email updated successfully\n");
        } else {
            printf("FAIL: Email not updated correctly\n");
        }
    } else {
        printf("FAIL: Failed to update email (ret=%d)\n", ret);
    }

    printf("\n--- Test 15: Update Last Play Time ---\n");
    ret = MemberInfoMileage_UpdateLastPlayTime(manager, member.m_id);
    if (ret == 0) {
        printf("PASS: Last play time updated\n");
    } else {
        printf("FAIL: Failed to update last play time (ret=%d)\n", ret);
    }

    printf("\n--- Test 16: Count Members by State ---\n");
    ret = MemberInfoMileage_CountByState(manager, 1, &count);
    if (ret == 0) {
        printf("PASS: Found %d members with state=1\n", count);
    } else {
        printf("FAIL: Failed to count members (ret=%d)\n", ret);
    }
}
