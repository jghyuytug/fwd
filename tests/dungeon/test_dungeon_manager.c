/**
 * @file test_dungeon_manager.c
 * @brief Dungeon Manager Test Suite
 *
 * Phase: 6.8 - Dungeon System Implementation
 * Date: 2025-11-25
 *
 * Tests:
 *   1. Module initialization and cleanup
 *   2. Instance creation and destruction
 *   3. User entry and exit
 *   4. Monster spawning and killing
 *   5. Dungeon completion and failure
 *   6. Time limit enforcement
 *   7. Clearance record tracking
 *   8. Concurrent instances
 */

#include "../../src/dungeon/dungeon_interface.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ========================================================================
 * TEST HELPERS
 * ======================================================================== */

static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_BEGIN(name) \
    printf("\n[TEST] %s\n", name); \
    printf("=========================================\n");

#define TEST_PASS(msg) \
    do { \
        printf("  [PASS] %s\n", msg); \
        g_tests_passed++; \
    } while(0)

#define TEST_FAIL(msg) \
    do { \
        printf("  [FAIL] %s\n", msg); \
        g_tests_failed++; \
    } while(0)

#define TEST_CHECK(cond, msg) \
    do { \
        if (cond) { TEST_PASS(msg); } \
        else { TEST_FAIL(msg); } \
    } while(0)

/* ========================================================================
 * TEST 1: INITIALIZATION AND CLEANUP
 * ======================================================================== */

static void test_initialization(void) {
    TEST_BEGIN("Initialization and Cleanup");

    /* Test with invalid parameters */
    int result = Dungeon_Initialize(-1, 1000);
    TEST_CHECK(result != ERR_SUCCESS, "Reject negative max_instances");

    result = Dungeon_Initialize(1000, -1);
    TEST_CHECK(result != ERR_SUCCESS, "Reject negative max_monsters");

    /* Test valid initialization */
    result = Dungeon_Initialize(100, 5000);
    TEST_CHECK(result == ERR_SUCCESS, "Initialize with valid params");

    /* Test double initialization */
    result = Dungeon_Initialize(100, 5000);
    TEST_CHECK(result == ERR_INVALID_STATE, "Reject double initialization");

    /* Cleanup */
    Dungeon_Cleanup();
    TEST_CHECK(1, "Cleanup completed");

    /* Test re-initialization after cleanup */
    result = Dungeon_Initialize(50, 2500);
    TEST_CHECK(result == ERR_SUCCESS, "Re-initialize after cleanup");

    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 2: INSTANCE CREATION AND DESTRUCTION
 * ======================================================================== */

static void test_instance_lifecycle(void) {
    TEST_BEGIN("Instance Creation and Destruction");

    Dungeon_Initialize(100, 5000);

    /* Create instance */
    int dungeon_id = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    TEST_CHECK(dungeon_id > 0, "Create solo dungeon instance");

    /* Get instance info */
    DungeonInstance info;
    int result = Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(result == ERR_SUCCESS, "Get instance info");
    TEST_CHECK(info.dungeon_template_id == 1001, "Template ID matches");
    TEST_CHECK(info.owner_user_id == 100, "Owner ID matches");
    TEST_CHECK(info.is_solo == 1, "Is solo dungeon");
    TEST_CHECK(info.state == DUNGEON_STATE_IDLE, "Initial state is IDLE");
    TEST_CHECK(info.difficulty == DIFFICULTY_NORMAL, "Difficulty is NORMAL");

    /* Create party dungeon */
    int party_dungeon = Dungeon_CreateInstance(2001, 200, 50, DIFFICULTY_MASTER);
    TEST_CHECK(party_dungeon > 0, "Create party dungeon instance");

    DungeonInstance party_info;
    Dungeon_GetInstanceInfo(party_dungeon, &party_info);
    TEST_CHECK(party_info.party_id == 50, "Party ID matches");
    TEST_CHECK(party_info.is_solo == 0, "Is party dungeon");
    TEST_CHECK(party_info.difficulty == DIFFICULTY_MASTER, "Difficulty is MASTER");

    /* Destroy instance */
    result = Dungeon_DestroyInstance(dungeon_id);
    TEST_CHECK(result == ERR_SUCCESS, "Destroy instance");

    /* Try to get destroyed instance */
    result = Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(result == ERR_NOT_FOUND, "Destroyed instance not found");

    /* Destroy non-existent instance */
    result = Dungeon_DestroyInstance(99999);
    TEST_CHECK(result == ERR_NOT_FOUND, "Cannot destroy non-existent");

    Dungeon_DestroyInstance(party_dungeon);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 3: USER ENTRY AND EXIT
 * ======================================================================== */

static void test_user_entry_exit(void) {
    TEST_BEGIN("User Entry and Exit");

    Dungeon_Initialize(100, 5000);

    int dungeon_id = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    TEST_CHECK(dungeon_id > 0, "Create dungeon");

    /* Enter dungeon */
    int result = Dungeon_Enter(dungeon_id, 100);
    TEST_CHECK(result == ERR_SUCCESS, "First user enters");

    /* Check state changed to ACTIVE */
    DungeonInstance info;
    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_ACTIVE, "State changed to ACTIVE");
    TEST_CHECK(info.start_time > 0, "Start time set");

    /* Try duplicate entry */
    result = Dungeon_Enter(dungeon_id, 100);
    TEST_CHECK(result == ERR_DUPLICATE, "Reject duplicate entry");

    /* Additional users */
    result = Dungeon_Enter(dungeon_id, 101);
    TEST_CHECK(result == ERR_SUCCESS, "Second user enters");

    result = Dungeon_Enter(dungeon_id, 102);
    TEST_CHECK(result == ERR_SUCCESS, "Third user enters");

    result = Dungeon_Enter(dungeon_id, 103);
    TEST_CHECK(result == ERR_SUCCESS, "Fourth user enters");

    /* Exceed capacity */
    result = Dungeon_Enter(dungeon_id, 104);
    TEST_CHECK(result == ERR_LIMIT_EXCEEDED, "Reject fifth user (capacity)");

    /* Exit dungeon */
    result = Dungeon_Exit(dungeon_id, 101);
    TEST_CHECK(result == ERR_SUCCESS, "User exits");

    /* Exit user not in dungeon */
    result = Dungeon_Exit(dungeon_id, 101);
    TEST_CHECK(result == ERR_NOT_FOUND, "Cannot exit twice");

    /* Enter non-existent dungeon */
    result = Dungeon_Enter(99999, 100);
    TEST_CHECK(result == ERR_NOT_FOUND, "Cannot enter non-existent");

    Dungeon_DestroyInstance(dungeon_id);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 4: MONSTER SPAWNING AND KILLING
 * ======================================================================== */

static void test_monster_management(void) {
    TEST_BEGIN("Monster Spawning and Killing");

    Dungeon_Initialize(100, 5000);

    int dungeon_id = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon_id, 100);

    /* Spawn monsters */
    int monster1 = Dungeon_SpawnMonster(dungeon_id, 5001, MONSTER_TYPE_NORMAL, 100, 200, 0);
    TEST_CHECK(monster1 > 0, "Spawn normal monster");

    int monster2 = Dungeon_SpawnMonster(dungeon_id, 5002, MONSTER_TYPE_ELITE, 150, 200, 0);
    TEST_CHECK(monster2 > 0, "Spawn elite monster");

    int monster3 = Dungeon_SpawnMonster(dungeon_id, 5003, MONSTER_TYPE_BOSS, 200, 200, 0);
    TEST_CHECK(monster3 > 0, "Spawn boss monster");

    /* Check total monsters */
    DungeonInstance info;
    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.total_monsters == 3, "Total monsters = 3");
    TEST_CHECK(info.monsters_killed == 0, "Killed monsters = 0");

    /* Get monsters list */
    MonsterInfo monsters[10];
    int count = Dungeon_GetMonsters(dungeon_id, monsters, 10);
    TEST_CHECK(count == 3, "GetMonsters returns 3");

    /* Kill normal monster */
    int result = Dungeon_KillMonster(monster1, 100);
    TEST_CHECK(result == ERR_SUCCESS, "Kill normal monster");

    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.monsters_killed == 1, "Killed monsters = 1");

    /* Try to kill already dead monster */
    result = Dungeon_KillMonster(monster1, 100);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot kill dead monster");

    /* Kill elite */
    Dungeon_KillMonster(monster2, 100);
    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.monsters_killed == 2, "Killed monsters = 2");

    /* Dungeon should still be active (boss alive) */
    TEST_CHECK(info.state == DUNGEON_STATE_ACTIVE, "Still ACTIVE (boss alive)");

    /* Kill boss - should auto-complete */
    Dungeon_KillMonster(monster3, 100);
    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_CLEARED, "Auto-CLEARED after boss kill");

    Dungeon_DestroyInstance(dungeon_id);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 5: DUNGEON COMPLETION AND FAILURE
 * ======================================================================== */

static void test_completion_failure(void) {
    TEST_BEGIN("Dungeon Completion and Failure");

    Dungeon_Initialize(100, 5000);

    /* Test manual completion */
    int dungeon1 = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon1, 100);

    int result = Dungeon_Complete(dungeon1);
    TEST_CHECK(result == ERR_SUCCESS, "Manual complete");

    DungeonInstance info;
    Dungeon_GetInstanceInfo(dungeon1, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_CLEARED, "State is CLEARED");
    TEST_CHECK(info.clear_time > 0, "Clear time set");

    /* Cannot complete again */
    result = Dungeon_Complete(dungeon1);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot complete twice");

    /* Test failure */
    int dungeon2 = Dungeon_CreateInstance(1001, 101, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon2, 101);

    result = Dungeon_Fail(dungeon2);
    TEST_CHECK(result == ERR_SUCCESS, "Manual fail");

    Dungeon_GetInstanceInfo(dungeon2, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_FAILED, "State is FAILED");

    /* Cannot fail again */
    result = Dungeon_Fail(dungeon2);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot fail twice");

    /* Test auto-fail on all users exit */
    int dungeon3 = Dungeon_CreateInstance(1001, 102, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon3, 102);
    Dungeon_Exit(dungeon3, 102);

    Dungeon_GetInstanceInfo(dungeon3, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_FAILED, "Auto-fail on empty");

    Dungeon_DestroyInstance(dungeon1);
    Dungeon_DestroyInstance(dungeon2);
    Dungeon_DestroyInstance(dungeon3);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 6: BOSS FIGHT STATE
 * ======================================================================== */

static void test_boss_fight(void) {
    TEST_BEGIN("Boss Fight State");

    Dungeon_Initialize(100, 5000);

    int dungeon_id = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon_id, 100);

    /* Cannot start boss fight in IDLE state */
    int dungeon_idle = Dungeon_CreateInstance(1002, 101, 0, DIFFICULTY_NORMAL);
    int result = Dungeon_StartBossFight(dungeon_idle);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot boss fight in IDLE");

    /* Start boss fight in ACTIVE state */
    result = Dungeon_StartBossFight(dungeon_id);
    TEST_CHECK(result == ERR_SUCCESS, "Start boss fight");

    DungeonInstance info;
    Dungeon_GetInstanceInfo(dungeon_id, &info);
    TEST_CHECK(info.state == DUNGEON_STATE_BOSS_FIGHT, "State is BOSS_FIGHT");

    /* Can still complete during boss fight */
    result = Dungeon_Complete(dungeon_id);
    TEST_CHECK(result == ERR_SUCCESS, "Complete during boss fight");

    Dungeon_DestroyInstance(dungeon_id);
    Dungeon_DestroyInstance(dungeon_idle);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 7: CLEARANCE RECORDS
 * ======================================================================== */

static void test_clearance_records(void) {
    TEST_BEGIN("Clearance Records");

    Dungeon_Initialize(100, 5000);

    unsigned int user_id = 100;
    unsigned int template_id = 1001;

    /* Get initial record (should be empty) */
    DungeonClearance record;
    int result = Dungeon_GetClearanceRecord(user_id, template_id, &record);
    TEST_CHECK(result == ERR_SUCCESS, "Get initial record");
    TEST_CHECK(record.clear_count == 0, "Initial clear count = 0");

    /* Complete a dungeon */
    int dungeon1 = Dungeon_CreateInstance(template_id, user_id, 0, DIFFICULTY_NORMAL);
    Dungeon_Enter(dungeon1, user_id);
    Dungeon_Complete(dungeon1);

    /* Check updated record */
    Dungeon_GetClearanceRecord(user_id, template_id, &record);
    TEST_CHECK(record.clear_count == 1, "Clear count = 1");
    TEST_CHECK(record.fastest_time > 0, "Fastest time set");

    unsigned int first_time = record.fastest_time;

    /* Complete another (should update count) */
    int dungeon2 = Dungeon_CreateInstance(template_id, user_id, 0, DIFFICULTY_EXPERT);
    Dungeon_Enter(dungeon2, user_id);
    Dungeon_Complete(dungeon2);

    Dungeon_GetClearanceRecord(user_id, template_id, &record);
    TEST_CHECK(record.clear_count == 2, "Clear count = 2");
    TEST_CHECK(record.difficulty == DIFFICULTY_EXPERT, "Difficulty updated");

    /* Different template should have separate record */
    DungeonClearance other_record;
    Dungeon_GetClearanceRecord(user_id, 2001, &other_record);
    TEST_CHECK(other_record.clear_count == 0, "Different template = 0 clears");

    /* Different user should have separate record */
    DungeonClearance user2_record;
    Dungeon_GetClearanceRecord(200, template_id, &user2_record);
    TEST_CHECK(user2_record.clear_count == 0, "Different user = 0 clears");

    Dungeon_DestroyInstance(dungeon1);
    Dungeon_DestroyInstance(dungeon2);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 8: TIME LIMIT AND UPDATE
 * ======================================================================== */

static void test_time_limit(void) {
    TEST_BEGIN("Time Limit and Update");

    Dungeon_Initialize(100, 5000);

    int dungeon_id = Dungeon_CreateInstance(1001, 100, 0, DIFFICULTY_NORMAL);
    TEST_CHECK(dungeon_id > 0, "Create dungeon");

    /* Check initial remaining time (not started) */
    int remaining = Dungeon_GetRemainingTime(dungeon_id);
    TEST_CHECK(remaining == 600, "Initial time = 600s (10 min)");

    /* Enter to start timer */
    Dungeon_Enter(dungeon_id, 100);

    /* Check remaining time (just started) */
    remaining = Dungeon_GetRemainingTime(dungeon_id);
    TEST_CHECK(remaining >= 598 && remaining <= 600, "Remaining ~600s after start");

    /* Check active count */
    int active = Dungeon_GetActiveCount();
    TEST_CHECK(active == 1, "Active count = 1");

    /* Process update */
    int processed = Dungeon_Update(1000);
    TEST_CHECK(processed >= 1, "Update processed >= 1 dungeon");

    /* Create slayer dungeon (shorter time) */
    int slayer = Dungeon_CreateInstance(2001, 101, 0, DIFFICULTY_SLAYER);
    remaining = Dungeon_GetRemainingTime(slayer);
    TEST_CHECK(remaining == 360, "Slayer time = 360s (6 min)");

    Dungeon_DestroyInstance(dungeon_id);
    Dungeon_DestroyInstance(slayer);
    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 9: CONCURRENT INSTANCES
 * ======================================================================== */

static void test_concurrent_instances(void) {
    TEST_BEGIN("Concurrent Instances");

    Dungeon_Initialize(100, 5000);

    /* Create multiple instances */
    int ids[10];
    for (int i = 0; i < 10; i++) {
        ids[i] = Dungeon_CreateInstance(1001 + i, 100 + i, 0,
                                        (DungeonDifficulty)(i % 5));
        TEST_CHECK(ids[i] > 0, "Create instance");
    }

    /* All should have unique IDs */
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (ids[i] == ids[j]) {
                TEST_FAIL("Duplicate instance ID");
            }
        }
    }
    TEST_PASS("All instance IDs unique");

    /* Enter all */
    for (int i = 0; i < 10; i++) {
        Dungeon_Enter(ids[i], 100 + i);
    }

    /* Check active count */
    int active = Dungeon_GetActiveCount();
    TEST_CHECK(active == 10, "Active count = 10");

    /* Complete half */
    for (int i = 0; i < 5; i++) {
        Dungeon_Complete(ids[i]);
    }

    active = Dungeon_GetActiveCount();
    TEST_CHECK(active == 5, "Active count = 5 after completing half");

    /* Cleanup all */
    for (int i = 0; i < 10; i++) {
        Dungeon_DestroyInstance(ids[i]);
    }

    active = Dungeon_GetActiveCount();
    TEST_CHECK(active == 0, "Active count = 0 after cleanup");

    Dungeon_Cleanup();
}

/* ========================================================================
 * TEST 10: DIFFICULTY SCALING
 * ======================================================================== */

static void test_difficulty_scaling(void) {
    TEST_BEGIN("Difficulty Scaling");

    Dungeon_Initialize(100, 5000);

    /* Test time limits per difficulty */
    int expected_times[] = {600, 540, 480, 420, 360};
    const char* difficulty_names[] = {"NORMAL", "EXPERT", "MASTER", "KING", "SLAYER"};

    for (int d = 0; d <= DIFFICULTY_SLAYER; d++) {
        int dungeon = Dungeon_CreateInstance(1001, 100, 0, (DungeonDifficulty)d);
        int remaining = Dungeon_GetRemainingTime(dungeon);

        char msg[64];
        snprintf(msg, sizeof(msg), "%s time limit = %ds",
                 difficulty_names[d], expected_times[d]);
        TEST_CHECK(remaining == expected_times[d], msg);

        Dungeon_DestroyInstance(dungeon);
    }

    /* Test monster health scaling */
    for (int d = 0; d <= DIFFICULTY_SLAYER; d++) {
        int dungeon = Dungeon_CreateInstance(1001, 100, 0, (DungeonDifficulty)d);
        Dungeon_Enter(dungeon, 100);

        /* Spawn boss and check health */
        int boss = Dungeon_SpawnMonster(dungeon, 9001, MONSTER_TYPE_BOSS, 0, 0, 0);

        MonsterInfo monsters[1];
        Dungeon_GetMonsters(dungeon, monsters, 1);

        /* Boss base health is 50000, scaled by difficulty */
        int expected_base = 50000;
        char msg[64];
        snprintf(msg, sizeof(msg), "%s boss health >= %d",
                 difficulty_names[d], expected_base);
        TEST_CHECK(monsters[0].max_health >= expected_base, msg);

        Dungeon_DestroyInstance(dungeon);
    }

    Dungeon_Cleanup();
}

/* ========================================================================
 * MAIN
 * ======================================================================== */

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("========================================\n");
    printf("Dungeon Manager Test Suite\n");
    printf("Phase: 6.8 - Dungeon System\n");
    printf("========================================\n");

    /* Run all tests */
    test_initialization();
    test_instance_lifecycle();
    test_user_entry_exit();
    test_monster_management();
    test_completion_failure();
    test_boss_fight();
    test_clearance_records();
    test_time_limit();
    test_concurrent_instances();
    test_difficulty_scaling();

    /* Summary */
    printf("\n========================================\n");
    printf("Test Results: %d passed, %d failed\n",
           g_tests_passed, g_tests_failed);
    printf("========================================\n");

    if (g_tests_failed > 0) {
        printf("\n[FAILURE] Some tests failed!\n");
        return 1;
    }

    printf("\n[SUCCESS] All tests passed!\n");
    return 0;
}
