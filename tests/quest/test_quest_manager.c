/**
 * Quest Manager Unit Tests
 * Purpose: Test quest system core functionality
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#include "../../include/quest/quest_config.h"
#include "../../include/quest/quest_manager.h"
#include "../../include/quest/quest_trigger.h"
#include "../../include/quest/quest_reward.h"
#include "../../src/quest/quest_interface.h"
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
 * Test 1: Quest config initialization
 */
TEST(config_initialization) {
    int result = QuestConfig_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);

    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 2: Quest manager initialization
 */
TEST(manager_initialization) {
    int result = QuestManager_Initialize(NULL);
    ASSERT_EQ(result, ERR_SUCCESS);

    QuestManager_Cleanup();
    TEST_PASS();
}

/**
 * Test 3: Quest trigger initialization
 */
TEST(trigger_initialization) {
    int result = QuestTrigger_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);

    QuestTrigger_Cleanup();
    TEST_PASS();
}

/**
 * Test 4: Quest reward initialization
 */
TEST(reward_initialization) {
    int result = QuestReward_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);

    QuestReward_Cleanup();
    TEST_PASS();
}

/**
 * Test 5: Load quest template
 */
TEST(load_quest_template) {
    QuestConfig_Initialize();

    QuestTemplate template;
    int result = QuestConfig_LoadTemplate(1001, &template);
    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_EQ(template.quest_id, 1001);
    ASSERT_EQ(template.quest_type, QUEST_TYPE_MAIN);
    ASSERT_EQ(template.objective_count, 1);
    ASSERT_EQ(template.objective_types[0], OBJECTIVE_KILL_MONSTER);
    ASSERT_EQ(template.objective_targets[0], 101);  // Goblin
    ASSERT_EQ(template.objective_counts[0], 10);    // Kill 10

    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 6: Accept quest
 */
TEST(accept_quest) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);

    int character_id = 100;
    unsigned int quest_template_id = 1001;

    QuestInstance instance;
    int quest_instance_id = QuestManager_Accept(character_id, quest_template_id, &instance);

    ASSERT_GT(quest_instance_id, 0);
    ASSERT_EQ(instance.character_id, character_id);
    ASSERT_EQ(instance.quest_template_id, quest_template_id);
    ASSERT_EQ(instance.state, QUEST_STATE_ACCEPTED);
    ASSERT_EQ(instance.objective_count, 1);

    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 7: Update quest progress
 */
TEST(update_quest_progress) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);

    int character_id = 100;
    int quest_instance_id = QuestManager_Accept(character_id, 1001, NULL);
    ASSERT_GT(quest_instance_id, 0);

    /* Quest 1001: Kill 10 Goblins (monster ID 101) */
    int quests_updated = QuestManager_UpdateProgress(character_id, OBJECTIVE_KILL_MONSTER, 101, 5);
    ASSERT_EQ(quests_updated, 1);

    /* Check progress */
    QuestInstance instance;
    QuestManager_GetInstance(quest_instance_id, &instance);
    ASSERT_EQ(instance.objective_progress[0], 5);
    ASSERT_EQ(instance.objective_completed[0], 0);  /* Not yet completed */

    /* Kill 5 more goblins - should complete objective */
    quests_updated = QuestManager_UpdateProgress(character_id, OBJECTIVE_KILL_MONSTER, 101, 5);
    ASSERT_EQ(quests_updated, 1);

    QuestManager_GetInstance(quest_instance_id, &instance);
    ASSERT_EQ(instance.objective_progress[0], 10);
    ASSERT_EQ(instance.objective_completed[0], 1);  /* Completed */

    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 8: Check quest completion
 */
TEST(check_quest_completion) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);

    int character_id = 100;
    int quest_instance_id = QuestManager_Accept(character_id, 1001, NULL);
    ASSERT_GT(quest_instance_id, 0);

    /* Not completed initially */
    int is_completed = QuestManager_CheckCompletion(quest_instance_id);
    ASSERT_EQ(is_completed, 0);

    /* Complete objective */
    QuestManager_UpdateProgress(character_id, OBJECTIVE_KILL_MONSTER, 101, 10);

    /* Check completion */
    is_completed = QuestManager_CheckCompletion(quest_instance_id);
    ASSERT_EQ(is_completed, 1);

    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 9: Submit quest and get reward
 */
TEST(submit_quest_reward) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);
    QuestReward_Initialize();

    int character_id = 100;
    int quest_instance_id = QuestManager_Accept(character_id, 1001, NULL);
    ASSERT_GT(quest_instance_id, 0);

    /* Complete quest */
    QuestManager_UpdateProgress(character_id, OBJECTIVE_KILL_MONSTER, 101, 10);
    QuestManager_CheckCompletion(quest_instance_id);

    /* Submit quest */
    QuestReward reward;
    int result = QuestManager_Submit(quest_instance_id, &reward);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Check reward */
    ASSERT_EQ(reward.exp, 1000);
    ASSERT_EQ(reward.gold, 500);
    ASSERT_EQ(reward.skill_points, 1);
    ASSERT_EQ(reward.item_count, 1);

    QuestReward_Cleanup();
    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 10: Abandon quest
 */
TEST(abandon_quest) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);

    int character_id = 100;
    int quest_instance_id = QuestManager_Accept(character_id, 1001, NULL);
    ASSERT_GT(quest_instance_id, 0);

    /* Abandon quest */
    int result = QuestManager_Abandon(quest_instance_id);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Check state */
    QuestInstance instance;
    QuestManager_GetInstance(quest_instance_id, &instance);
    ASSERT_EQ(instance.state, QUEST_STATE_FAILED);

    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 11: Quest trigger - monster killed
 */
TEST(trigger_monster_killed) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);
    QuestTrigger_Initialize();

    int character_id = 100;
    int quest_instance_id = QuestManager_Accept(character_id, 1001, NULL);
    ASSERT_GT(quest_instance_id, 0);

    /* Trigger monster killed event */
    QuestTrigger_OnMonsterKilled(character_id, 101);

    /* Check progress */
    QuestInstance instance;
    QuestManager_GetInstance(quest_instance_id, &instance);
    ASSERT_EQ(instance.objective_progress[0], 1);

    QuestTrigger_Cleanup();
    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Test 12: Multiple quests for one character
 */
TEST(multiple_quests) {
    QuestConfig_Initialize();
    QuestManager_Initialize(NULL);

    int character_id = 100;

    /* Accept multiple quests */
    int quest1 = QuestManager_Accept(character_id, 1001, NULL);
    int quest2 = QuestManager_Accept(character_id, 1002, NULL);
    int quest3 = QuestManager_Accept(character_id, 1003, NULL);

    ASSERT_GT(quest1, 0);
    ASSERT_GT(quest2, 0);
    ASSERT_GT(quest3, 0);

    /* Get active quests */
    QuestInstance instances[10];
    int count = QuestManager_GetActiveQuests(character_id, instances, 10);
    ASSERT_EQ(count, 3);

    QuestManager_Cleanup();
    QuestConfig_Cleanup();
    TEST_PASS();
}

/**
 * Main test runner
 */
int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║         Quest Manager Unit Tests                         ║\n");
    printf("║         Phase 6.2 - Quest System                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Run all tests */
    RUN_TEST(config_initialization);
    RUN_TEST(manager_initialization);
    RUN_TEST(trigger_initialization);
    RUN_TEST(reward_initialization);
    RUN_TEST(load_quest_template);
    RUN_TEST(accept_quest);
    RUN_TEST(update_quest_progress);
    RUN_TEST(check_quest_completion);
    RUN_TEST(submit_quest_reward);
    RUN_TEST(abandon_quest);
    RUN_TEST(trigger_monster_killed);
    RUN_TEST(multiple_quests);

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
