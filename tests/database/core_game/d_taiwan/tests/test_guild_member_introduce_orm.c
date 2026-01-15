#include "guild_member_introduce_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void test_get_by_guild(DBConnectionManager* manager);
void test_count_by_guild(DBConnectionManager* manager);
void test_delete_by_guild(DBConnectionManager* manager);

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Member Introduce ORM Test Suite ===\n\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_add_member(&manager);
    test_get_member(&manager);
    test_update_member(&manager);
    test_exists_member(&manager);
    test_get_by_guild(&manager);
    test_count_by_guild(&manager);
    test_delete_member(&manager);
    test_delete_by_guild(&manager);

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
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_member_introduce WHERE guild_id IN (100001, 100002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_member(DBConnectionManager* manager) {
    GuildMemberIntroduce member;

    printf("[TEST] Add member introduction\n");

    memset(&member, 0, sizeof(GuildMemberIntroduce));
    member.guild_id = 100001;
    member.charac_no = 20001;
    strncpy(member.introduce, "Hello, I am a test member!", sizeof(member.introduce) - 1);

    if (GuildMemberIntroduce_Add(manager, &member) == 0) {
        printf("  PASS: Member introduction added\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add member introduction\n");
        test_failed++;
    }
}

void test_get_member(DBConnectionManager* manager) {
    GuildMemberIntroduce member;

    printf("[TEST] Get member introduction\n");

    if (GuildMemberIntroduce_Get(manager, 100001, 20001, &member) == 0) {
        printf("  PASS: Retrieved member (guild_id=%d, charac_no=%d)\n",
               member.guild_id, member.charac_no);
        printf("  Introduction: %s\n", member.introduce);
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve member\n");
        test_failed++;
    }
}

void test_update_member(DBConnectionManager* manager) {
    GuildMemberIntroduce member;

    printf("[TEST] Update member introduction\n");

    memset(&member, 0, sizeof(GuildMemberIntroduce));
    member.guild_id = 100001;
    member.charac_no = 20001;
    strncpy(member.introduce, "Updated introduction!", sizeof(member.introduce) - 1);

    if (GuildMemberIntroduce_Update(manager, &member) == 0) {
        printf("  PASS: Member introduction updated\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to update member introduction\n");
        test_failed++;
    }
}

void test_exists_member(DBConnectionManager* manager) {
    printf("[TEST] Check member exists\n");

    if (GuildMemberIntroduce_Exists(manager, 100001, 20001)) {
        printf("  PASS: Member exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Member should exist\n");
        test_failed++;
    }
}

void test_get_by_guild(DBConnectionManager* manager) {
    GuildMemberIntroduce members[10];
    int count = 0;

    printf("[TEST] Get members by guild\n");

    /* Add more test members */
    GuildMemberIntroduce member2;
    memset(&member2, 0, sizeof(GuildMemberIntroduce));
    member2.guild_id = 100001;
    member2.charac_no = 20002;
    strncpy(member2.introduce, "Second member", sizeof(member2.introduce) - 1);
    GuildMemberIntroduce_Add(manager, &member2);

    if (GuildMemberIntroduce_GetByGuild(manager, 100001, members, 10, &count) == 0) {
        printf("  PASS: Retrieved %d members from guild 100001\n", count);
        for (int i = 0; i < count; i++) {
            printf("    Member %d: charac_no=%d, intro=%s\n",
                   i + 1, members[i].charac_no, members[i].introduce);
        }
        test_passed++;
    } else {
        printf("  FAIL: Failed to get members by guild\n");
        test_failed++;
    }
}

void test_count_by_guild(DBConnectionManager* manager) {
    int count = 0;

    printf("[TEST] Count members by guild\n");

    if (GuildMemberIntroduce_CountByGuild(manager, 100001, &count) == 0) {
        printf("  PASS: Guild 100001 has %d members\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to count members\n");
        test_failed++;
    }
}

void test_delete_member(DBConnectionManager* manager) {
    printf("[TEST] Delete member introduction\n");

    if (GuildMemberIntroduce_Delete(manager, 100001, 20002) == 0) {
        printf("  PASS: Member introduction deleted\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete member introduction\n");
        test_failed++;
    }
}

void test_delete_by_guild(DBConnectionManager* manager) {
    int count = 0;

    printf("[TEST] Delete all members by guild\n");

    /* Add test guild */
    GuildMemberIntroduce member;
    memset(&member, 0, sizeof(GuildMemberIntroduce));
    member.guild_id = 100002;
    member.charac_no = 30001;
    strncpy(member.introduce, "To be deleted", sizeof(member.introduce) - 1);
    GuildMemberIntroduce_Add(manager, &member);

    if (GuildMemberIntroduce_DeleteByGuild(manager, 100002) == 0) {
        GuildMemberIntroduce_CountByGuild(manager, 100002, &count);
        printf("  PASS: Deleted all members from guild 100002 (remaining: %d)\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete members by guild\n");
        test_failed++;
    }
}
