#include "event_quest_party_member_web_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test functions */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

/* Test statistics */
static int test_passed = 0;
static int test_failed = 0;
static int test_total = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        test_total++; \
        if (condition) { \
            printf("[PASS] %s\n", message); \
            test_passed++; \
        } else { \
            printf("[FAIL] %s\n", message); \
            test_failed++; \
        } \
    } while(0)

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

    printf("=== Event Quest Party Member Web ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n", test_total, test_passed, test_failed);
    printf("Success Rate: %.1f%%\n", test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    EventQuestPartyMemberWeb member, retrieved;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    /* Test 1: Add new member */
    memset(&member, 0, sizeof(EventQuestPartyMemberWeb));
    member.m_id = 100001;
    member.server_id = 1;
    member.charac_no = 500001;
    member.quest_no = 10001;
    strcpy(member.occ_time, "2025-11-17 10:00:00");
    member.send_charac_no = 500002;

    ret = EventQuestPartyMemberWeb_Add(manager, &member);
    TEST_ASSERT(ret == 0, "Add new quest party member");

    /* Test 2: Check existence */
    ret = EventQuestPartyMemberWeb_Exists(manager, 100001, 1, 500001, 10001);
    TEST_ASSERT(ret == 1, "Member exists after add");

    /* Test 3: Get member */
    ret = EventQuestPartyMemberWeb_Get(manager, 100001, 1, 500001, 10001, &retrieved);
    TEST_ASSERT(ret == 0 && retrieved.m_id == 100001, "Get member by composite key");
    TEST_ASSERT(strcmp(retrieved.occ_time, "2025-11-17 10:00:00") == 0, "Verify occ_time");
    TEST_ASSERT(retrieved.send_charac_no == 500002, "Verify send_charac_no");

    /* Test 4: Update member */
    strcpy(member.occ_time, "2025-11-17 12:00:00");
    member.send_charac_no = 500003;
    ret = EventQuestPartyMemberWeb_Update(manager, &member);
    TEST_ASSERT(ret == 0, "Update member");

    ret = EventQuestPartyMemberWeb_Get(manager, 100001, 1, 500001, 10001, &retrieved);
    TEST_ASSERT(ret == 0 && strcmp(retrieved.occ_time, "2025-11-17 12:00:00") == 0,
                "Verify updated occ_time");
    TEST_ASSERT(retrieved.send_charac_no == 500003, "Verify updated send_charac_no");

    /* Test 5: Delete member */
    ret = EventQuestPartyMemberWeb_Delete(manager, 100001, 1, 500001, 10001);
    TEST_ASSERT(ret == 0, "Delete member");

    ret = EventQuestPartyMemberWeb_Exists(manager, 100001, 1, 500001, 10001);
    TEST_ASSERT(ret == 0, "Member does not exist after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventQuestPartyMemberWeb member;
    EventQuestPartyMemberWeb members[10];
    int actual_count = 0;
    int ret;

    printf("--- Test: Business Operations ---\n");

    /* Add test data - same member, different quests */
    for (int i = 0; i < 3; i++) {
        memset(&member, 0, sizeof(EventQuestPartyMemberWeb));
        member.m_id = 200001;
        member.server_id = 2;
        member.charac_no = 600001;
        member.quest_no = 20001 + i;
        strcpy(member.occ_time, "2025-11-17 10:00:00");
        member.send_charac_no = 600002 + i;
        EventQuestPartyMemberWeb_Add(manager, &member);
    }

    /* Test 1: Get by member (should return 3 quests) */
    ret = EventQuestPartyMemberWeb_GetByMember(manager, 200001, 2, 600001,
                                                members, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all quests for a member");

    /* Add more members for the same quest */
    for (int i = 0; i < 2; i++) {
        memset(&member, 0, sizeof(EventQuestPartyMemberWeb));
        member.m_id = 200002 + i;
        member.server_id = 2;
        member.charac_no = 600010 + i;
        member.quest_no = 20001;
        strcpy(member.occ_time, "2025-11-17 11:00:00");
        member.send_charac_no = 600020 + i;
        EventQuestPartyMemberWeb_Add(manager, &member);
    }

    /* Test 2: Get by quest (should return 3 members for quest 20001) */
    ret = EventQuestPartyMemberWeb_GetByQuest(manager, 20001, members, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 3, "Get all members for a quest");

    /* Test 3: Delete by member (should delete all 3 quests for member 200001) */
    ret = EventQuestPartyMemberWeb_DeleteByMember(manager, 200001, 2, 600001);
    TEST_ASSERT(ret == 0, "Delete all quests for a member");

    ret = EventQuestPartyMemberWeb_GetByMember(manager, 200001, 2, 600001,
                                                members, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 0, "Verify member quests deleted");

    /* Now quest 20001 should only have 2 members left */
    ret = EventQuestPartyMemberWeb_GetByQuest(manager, 20001, members, 10, &actual_count);
    TEST_ASSERT(ret == 0 && actual_count == 2, "Verify quest members count after delete");

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_quest_party_member_web WHERE m_id >= 100000");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
