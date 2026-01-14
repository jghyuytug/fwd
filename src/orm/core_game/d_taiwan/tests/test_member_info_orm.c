/**
 * DNF Game Server - MemberInfo ORM Test Program
 *
 * Features:
 * 1. Test member account CRUD operations
 * 2. Test authentication and password management
 * 3. Test member search and statistics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db_connection_manager.h"
#include "member_info_orm.h"

/* ANSI color codes */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_separator() {
    printf("%s================================================================================\n%s",
           COLOR_CYAN, COLOR_RESET);
}

void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

/* Test 1: Create member accounts */
int test_create_member(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 1: Create Member Accounts%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Create member 1 */
    print_info("Creating member 1: testuser1");
    MemberInfo_Init(&member);
    strcpy(member.user_id, "testuser1");
    strcpy(member.user_name, "TestUser");
    strcpy(member.first_ssn, "800101");
    strcpy(member.second_ssn, "1234567");
    strcpy(member.passwd, "5f4dcc3b5aa765d61d8327deb882cf99"); /* MD5 of "password" */
    strcpy(member.mobile_no, "13800138000");
    member.reg_date = (int)time(NULL);
    strcpy(member.email, "test1@example.com");
    member.q_no = 1;
    strcpy(member.q_answer, "beijing");
    strcpy(member.nickname, "Player1");
    strcpy(member.email_yn, "y");
    member.state = 1;
    member.slot = 8;
    strcpy(member.last_play_time, "2025-01-01 00:00:00");

    if (MemberInfo_Create(manager, &member) == 0) {
        print_success("Member 1 created successfully");
        printf("  User ID: %s\n", member.user_id);
        printf("  Nickname: %s\n", member.nickname);
        printf("  Email: %s\n", member.email);
    } else {
        print_error("Failed to create member 1");
        return -1;
    }

    /* Create member 2 */
    print_info("Creating member 2: testuser2");
    MemberInfo_Init(&member);
    strcpy(member.user_id, "testuser2");
    strcpy(member.user_name, "TestUser2");
    strcpy(member.first_ssn, "850615");
    strcpy(member.second_ssn, "7654321");
    strcpy(member.passwd, "5f4dcc3b5aa765d61d8327deb882cf99");
    strcpy(member.mobile_no, "13900139000");
    member.reg_date = (int)time(NULL) - 86400; /* 1 day ago */
    strcpy(member.email, "test2@example.com");
    member.q_no = 2;
    strcpy(member.q_answer, "shanghai");
    strcpy(member.nickname, "Player2");
    member.slot = 12;

    if (MemberInfo_Create(manager, &member) == 0) {
        print_success("Member 2 created successfully");
        printf("  User ID: %s\n", member.user_id);
        printf("  Slot: %u\n", member.slot);
    } else {
        print_error("Failed to create member 2");
        return -1;
    }

    /* Create member 3 (inactive) */
    print_info("Creating member 3: testuser3 (inactive)");
    MemberInfo_Init(&member);
    strcpy(member.user_id, "testuser3");
    strcpy(member.user_name, "TestUser3");
    strcpy(member.first_ssn, "900301");
    strcpy(member.second_ssn, "1111111");
    strcpy(member.passwd, "5f4dcc3b5aa765d61d8327deb882cf99");
    strcpy(member.mobile_no, "13700137000");
    member.reg_date = (int)time(NULL) - 172800; /* 2 days ago */
    strcpy(member.email, "test3@example.com");
    member.q_no = 1;
    strcpy(member.q_answer, "guangzhou");
    strcpy(member.nickname, "Player3");
    member.state = 0; /* Inactive */

    if (MemberInfo_Create(manager, &member) == 0) {
        print_success("Member 3 created successfully");
        printf("  User ID: %s\n", member.user_id);
        printf("  State: %d (inactive)\n", member.state);
    } else {
        print_error("Failed to create member 3");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 2: Query member accounts */
int test_query_member(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 2: Query Member Accounts%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Query by user_id */
    print_info("Query by user_id: testuser1");
    MemberInfo_Init(&member);

    if (MemberInfo_GetByUserID(manager, "testuser1", &member) == 0) {
        print_success("Query successful");
        printf("  Member ID: %d\n", member.m_id);
        printf("  User ID: %s\n", member.user_id);
        printf("  User Name: %s\n", member.user_name);
        printf("  Nickname: %s\n", member.nickname);
        printf("  Email: %s\n", member.email);
        printf("  State: %d\n", member.state);
        printf("  Slot: %u\n", member.slot);
    } else {
        print_error("Query failed");
        return -1;
    }

    /* Save m_id for later tests */
    int test_m_id = member.m_id;

    /* Query by m_id */
    print_info("Query by m_id");
    MemberInfo_Init(&member);

    if (MemberInfo_GetByID(manager, test_m_id, &member) == 0) {
        print_success("Query successful");
        printf("  Member ID: %d\n", member.m_id);
        printf("  User ID: %s\n", member.user_id);
    } else {
        print_error("Query failed");
        return -1;
    }

    /* Query by nickname */
    print_info("Query by nickname: Player2");
    MemberInfo_Init(&member);

    if (MemberInfo_GetByNickname(manager, "Player2", &member) == 0) {
        print_success("Query successful");
        printf("  Member ID: %d\n", member.m_id);
        printf("  User ID: %s\n", member.user_id);
        printf("  Nickname: %s\n", member.nickname);
    } else {
        print_error("Query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 3: Update member information */
int test_update_member(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 3: Update Member Information%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get original data */
    print_info("Getting original member data");
    if (MemberInfo_GetByUserID(manager, "testuser1", &member) != 0) {
        print_error("Failed to get original data");
        return -1;
    }

    printf("  Original email: %s\n", member.email);
    printf("  Original slot: %u\n", member.slot);

    /* Update data */
    print_info("Updating member data");
    strcpy(member.email, "newemail@example.com");
    member.slot = 16;

    if (MemberInfo_Update(manager, &member) == 0) {
        print_success("Update successful");
        printf("  New email: %s\n", member.email);
        printf("  New slot: %u\n", member.slot);
    } else {
        print_error("Update failed");
        return -1;
    }

    /* Verify update */
    print_info("Verifying update");
    MemberInfo_Init(&member);

    if (MemberInfo_GetByUserID(manager, "testuser1", &member) == 0) {
        if (strcmp(member.email, "newemail@example.com") == 0 && member.slot == 16) {
            print_success("Update verification passed");
        } else {
            print_error("Update verification failed");
            return -1;
        }
    } else {
        print_error("Verification query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 4: Existence checks */
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%sTest 4: Existence Checks%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Check user_id exists */
    print_info("Check if testuser1 exists");
    if (MemberInfo_UserIDExists(manager, "testuser1")) {
        print_success("Correctly identified user_id exists");
    } else {
        print_error("Incorrectly identified user_id does not exist");
        return -1;
    }

    /* Check user_id does not exist */
    print_info("Check if nonexistent user does not exist");
    if (!MemberInfo_UserIDExists(manager, "nonexistent")) {
        print_success("Correctly identified user_id does not exist");
    } else {
        print_error("Incorrectly identified user_id exists");
        return -1;
    }

    /* Check nickname exists */
    print_info("Check if Player1 nickname exists");
    if (MemberInfo_NicknameExists(manager, "Player1")) {
        print_success("Correctly identified nickname exists");
    } else {
        print_error("Incorrectly identified nickname does not exist");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 5: Password verification */
int test_password_verification(DBConnectionManager* manager) {
    print_separator();
    printf("%sTest 5: Password Verification%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Correct password */
    print_info("Verify correct password");
    if (MemberInfo_VerifyPassword(manager, "testuser1", "5f4dcc3b5aa765d61d8327deb882cf99")) {
        print_success("Password verification passed");
    } else {
        print_error("Password verification failed");
        return -1;
    }

    /* Wrong password */
    print_info("Verify wrong password");
    if (!MemberInfo_VerifyPassword(manager, "testuser1", "wrongpasswordhash")) {
        print_success("Correctly rejected wrong password");
    } else {
        print_error("Incorrectly accepted wrong password");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 6: Update password */
int test_update_password(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 6: Update Password%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get member */
    if (MemberInfo_GetByUserID(manager, "testuser2", &member) != 0) {
        print_error("Failed to get member");
        return -1;
    }

    /* Update password */
    print_info("Updating password for testuser2");
    const char* new_pass_hash = "098f6bcd4621d373cade4e832627b4f6"; /* MD5 of "test" */

    if (MemberInfo_UpdatePassword(manager, member.m_id, new_pass_hash) == 0) {
        print_success("Password updated successfully");

        /* Verify new password */
        if (MemberInfo_VerifyPassword(manager, "testuser2", new_pass_hash)) {
            print_success("New password verification passed");
        } else {
            print_error("New password verification failed");
            return -1;
        }
    } else {
        print_error("Password update failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 7: Update state */
int test_update_state(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 7: Update Account State%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get member */
    if (MemberInfo_GetByUserID(manager, "testuser1", &member) != 0) {
        print_error("Failed to get member");
        return -1;
    }

    printf("  Original state: %d\n", member.state);

    /* Save m_id for later use */
    int saved_m_id = member.m_id;

    /* Deactivate account */
    print_info("Deactivating account");
    if (MemberInfo_UpdateState(manager, saved_m_id, 0) == 0) {
        print_success("State updated successfully");

        /* Verify */
        MemberInfo_Init(&member);
        if (MemberInfo_GetByID(manager, saved_m_id, &member) == 0) {
            if (member.state == 0) {
                print_success("State verification passed");
            } else {
                print_error("State verification failed");
                return -1;
            }
        }
    } else {
        print_error("State update failed");
        return -1;
    }

    /* Reactivate account */
    print_info("Reactivating account");
    if (MemberInfo_UpdateState(manager, saved_m_id, 1) == 0) {
        print_success("State updated successfully");
    } else {
        print_error("State update failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 8: Statistics */
int test_statistics(DBConnectionManager* manager) {
    print_separator();
    printf("%sTest 8: Statistics%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Total count */
    print_info("Getting total member count");
    int total_count = MemberInfo_GetCount(manager);
    if (total_count >= 0) {
        print_success("Query successful");
        printf("  Total members: %d\n", total_count);
    } else {
        print_error("Query failed");
        return -1;
    }

    /* Active count */
    print_info("Getting active member count");
    int active_count = MemberInfo_GetActiveCount(manager);
    if (active_count >= 0) {
        print_success("Query successful");
        printf("  Active members: %d\n", active_count);
    } else {
        print_error("Query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 9: Search by user name */
int test_search(DBConnectionManager* manager) {
    MemberInfo members[10];

    print_separator();
    printf("%sTest 9: Search Members%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Searching for members with name containing 'Test'");
    int count = MemberInfo_SearchByUserName(manager, "Test", members, 10);

    if (count >= 0) {
        print_success("Search successful");
        printf("  Found %d members\n", count);

        for (int i = 0; i < count; i++) {
            printf("    %d. %s (%s)\n", i + 1, members[i].user_id, members[i].user_name);
        }
    } else {
        print_error("Search failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 10: Recently registered members */
int test_recently_registered(DBConnectionManager* manager) {
    MemberInfo members[10];

    print_separator();
    printf("%sTest 10: Recently Registered Members%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Getting top 5 recently registered members");
    int count = MemberInfo_GetRecentlyRegistered(manager, members, 5);

    if (count >= 0) {
        print_success("Query successful");
        printf("  Found %d members\n", count);

        for (int i = 0; i < count; i++) {
            printf("    %d. %s (reg_date: %d)\n", i + 1, members[i].user_id, members[i].reg_date);
        }
    } else {
        print_error("Query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 11: Delete member */
int test_delete_member(DBConnectionManager* manager) {
    MemberInfo member;

    print_separator();
    printf("%sTest 11: Delete Member%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get member to delete */
    if (MemberInfo_GetByUserID(manager, "testuser3", &member) != 0) {
        print_error("Failed to get member");
        return -1;
    }

    print_info("Deleting testuser3");
    if (MemberInfo_Delete(manager, member.m_id) == 0) {
        print_success("Member deleted successfully");

        /* Verify deletion */
        if (!MemberInfo_UserIDExists(manager, "testuser3")) {
            print_success("Deletion verification passed");
        } else {
            print_error("Member still exists after deletion");
            return -1;
        }
    } else {
        print_error("Delete failed");
        return -1;
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    printf("\n");
    print_separator();
    printf("%s  DNF Game Server - MemberInfo ORM Test  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("Usage: test_member_info_orm <config_file>");
        printf("  Example: ./test_member_info_orm ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("Config file: %s\n\n", config_file);

    /* Initialize database connection */
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("Initializing database connection...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("Initialization failed");
        return 1;
    }
    print_success("Initialization successful");

    /* Connect to account database */
    print_info("Connecting to account database (test_d_taiwan)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) != 0) {
        print_error("Connection failed");
        return 1;
    }
    print_success("Connection successful");
    printf("\n");

    /* Run tests */
    int success_count = 0;
    int total_tests = 11;

    if (test_create_member(&manager) == 0) success_count++;
    if (test_query_member(&manager) == 0) success_count++;
    if (test_update_member(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_password_verification(&manager) == 0) success_count++;
    if (test_update_password(&manager) == 0) success_count++;
    if (test_update_state(&manager) == 0) success_count++;
    if (test_statistics(&manager) == 0) success_count++;
    if (test_search(&manager) == 0) success_count++;
    if (test_recently_registered(&manager) == 0) success_count++;
    if (test_delete_member(&manager) == 0) success_count++;

    /* Cleanup */
    print_info("Cleaning up database connection...");
    DBConnectionManager_Cleanup(&manager);
    print_success("Cleanup complete");

    printf("\n");
    print_separator();
    printf("%sTest Result: %d/%d Passed%s\n",
           success_count == total_tests ? COLOR_GREEN : COLOR_YELLOW,
           success_count, total_tests, COLOR_RESET);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
