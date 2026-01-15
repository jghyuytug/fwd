/**
 * Party Manager Unit Tests
 * Purpose: Test party system core functionality
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#include "../../include/party/party_manager.h"
#include "../../include/party/party_exp.h"
#include "../../include/party/quick_party.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Test counter */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macros */
#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("Running test: %s\n", #name); \
    test_##name(); \
    tests_run++; \
} while(0)

#define ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("  [FAIL] Expected %d, got %d\n", (expected), (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        printf("  [FAIL] Condition failed: %s\n", #condition); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_GT(actual, threshold) do { \
    if ((actual) <= (threshold)) { \
        printf("  [FAIL] Expected > %d, got %d\n", (threshold), (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define TEST_PASS() do { \
    printf("  [PASS]\n"); \
    tests_passed++; \
} while(0)

/**
 * Test 1: Party manager initialization
 */
TEST(manager_initialization) {
    int result = PartyManager_Initialize(NULL, 100);
    ASSERT_EQ(result, ERR_SUCCESS);

    PartyManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 2: Create party
 */
TEST(create_party) {
    PartyManager_Initialize(NULL, 100);

    int party_id = PartyManager_Create(100, PARTY_TYPE_NORMAL, "Test Party", NULL);
    ASSERT_GT(party_id, 0);

    PartyManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 3: Invite and accept
 */
TEST(invite_accept) {
    PartyManager_Initialize(NULL, 100);

    int party_id = PartyManager_Create(100, PARTY_TYPE_NORMAL, NULL, NULL);
    ASSERT_GT(party_id, 0);

    int result = PartyManager_Invite(party_id, 100, 200);
    ASSERT_EQ(result, ERR_SUCCESS);

    result = PartyManager_AcceptInvite(party_id, 200);
    ASSERT_EQ(result, ERR_SUCCESS);

    PartyManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 4: Leave party
 */
TEST(leave_party) {
    PartyManager_Initialize(NULL, 100);

    int party_id = PartyManager_Create(100, PARTY_TYPE_NORMAL, NULL, NULL);
    PartyManager_Invite(party_id, 100, 200);
    PartyManager_AcceptInvite(party_id, 200);

    int result = PartyManager_Leave(party_id, 200);
    ASSERT_EQ(result, ERR_SUCCESS);

    PartyManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 5: Kick member
 */
TEST(kick_member) {
    PartyManager_Initialize(NULL, 100);

    int party_id = PartyManager_Create(100, PARTY_TYPE_NORMAL, NULL, NULL);
    PartyManager_Invite(party_id, 100, 200);
    PartyManager_AcceptInvite(party_id, 200);

    int result = PartyManager_Kick(party_id, 100, 200);
    ASSERT_EQ(result, ERR_SUCCESS);

    PartyManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 6: Party exp initialization
 */
TEST(exp_initialization) {
    int result = PartyExp_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);

    PartyExp_Cleanup();
    TEST_PASS();
}

/**
 * Test 7: Party bonus calculation
 */
TEST(party_bonus) {
    float bonus2 = PartyExp_GetPartyBonus(2);
    float bonus3 = PartyExp_GetPartyBonus(3);
    float bonus4 = PartyExp_GetPartyBonus(4);

    ASSERT_TRUE(bonus2 == 0.10f);  // +10%
    ASSERT_TRUE(bonus3 == 0.20f);  // +20%
    ASSERT_TRUE(bonus4 == 0.30f);  // +30%

    TEST_PASS();
}

/**
 * Test 8: Quick party initialization
 */
TEST(quick_party_init) {
    int result = QuickParty_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);

    QuickParty_Cleanup();
    TEST_PASS();
}

/**
 * Test 9: Enter queue
 */
TEST(enter_queue) {
    QuickParty_Initialize();

    int result = QuickParty_EnterQueue(100, DUNGEON_TYPE_NORMAL, 0);
    ASSERT_EQ(result, ERR_SUCCESS);

    int queue_size = QuickParty_GetQueueSize();
    ASSERT_EQ(queue_size, 1);

    QuickParty_Cleanup();
    TEST_PASS();
}

/**
 * Test 10: Leave queue
 */
TEST(leave_queue) {
    QuickParty_Initialize();

    QuickParty_EnterQueue(100, DUNGEON_TYPE_NORMAL, 0);
    int result = QuickParty_LeaveQueue(100);
    ASSERT_EQ(result, ERR_SUCCESS);

    int queue_size = QuickParty_GetQueueSize();
    ASSERT_EQ(queue_size, 0);

    QuickParty_Cleanup();
    TEST_PASS();
}

/**
 * Main test runner
 */
int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║         Party Manager Unit Tests                         ║\n");
    printf("║         Phase 6.3 - Party System                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Run all tests */
    RUN_TEST(manager_initialization);
    RUN_TEST(create_party);
    RUN_TEST(invite_accept);
    RUN_TEST(leave_party);
    RUN_TEST(kick_member);
    RUN_TEST(exp_initialization);
    RUN_TEST(party_bonus);
    RUN_TEST(quick_party_init);
    RUN_TEST(enter_queue);
    RUN_TEST(leave_queue);

    /* Print summary */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    Summary                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Tests Run:    %d\n", tests_run);
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("\n");

    if (tests_failed == 0) {
        printf("✅ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("❌ SOME TESTS FAILED\n");
        return 1;
    }
}
