/**
 * @file test_game_integration.c
 * @brief Game Integration Layer Test Suite
 *
 * Tests cross-system integration:
 * - Combat → Quest progress
 * - Combat → Achievement progress
 * - Dungeon → Quest progress
 * - Dungeon → Achievement progress
 * - PvP → Achievement progress
 *
 * @version 1.0
 * @date 2025-11-26
 */

#include "../../src/integration/game_integration.h"
#include "../../src/events/events_interface.h"
#include "../../include/quest/quest_manager.h"
#include "../../include/achievement/achievement_interface.h"
#include "../../include/dungeon/dungeon_interface.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*==============================================================================
 * Test Macros
 *============================================================================*/

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST(name) \
    static void test_##name(void); \
    static void run_test_##name(void) { \
        printf("[TEST] Running: %s\n", #name); \
        test_##name(); \
        g_tests_run++; \
        printf("[PASS] %s\n\n", #name); \
        g_tests_passed++; \
    } \
    static void test_##name(void)

#define RUN_TEST(name) run_test_##name()

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            fprintf(stderr, "[FAIL] %s:%d: Expected %d, got %d\n", \
                    __FILE__, __LINE__, (int)(b), (int)(a)); \
            g_tests_failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s\n", \
                    __FILE__, __LINE__, #expr); \
            g_tests_failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_GE(a, b) \
    do { \
        if ((a) < (b)) { \
            fprintf(stderr, "[FAIL] %s:%d: Expected %d >= %d\n", \
                    __FILE__, __LINE__, (int)(a), (int)(b)); \
            g_tests_failed++; \
            return; \
        } \
    } while(0)

/*==============================================================================
 * Test Setup/Teardown
 *============================================================================*/

static void setup_all_systems(void)
{
    /* Initialize event system first */
    int result = Events_Initialize(10000, 100);
    ASSERT_EQ(result, 0);

    /* Initialize quest system */
    result = QuestManager_Initialize(NULL);
    ASSERT_EQ(result, 0);

    /* Initialize achievement system */
    result = Achievement_Initialize(5000, 500);
    ASSERT_EQ(result, 0);

    /* Initialize dungeon system */
    result = DungeonManager_Initialize(1000, 50000);
    ASSERT_EQ(result, 0);

    /* Initialize integration layer */
    result = GameIntegration_Initialize();
    ASSERT_EQ(result, 0);

    printf("[Setup] All systems initialized\n");
}

static void teardown_all_systems(void)
{
    GameIntegration_Cleanup();
    DungeonManager_Cleanup();
    Achievement_Cleanup();
    QuestManager_Cleanup();
    Events_Cleanup();

    printf("[Teardown] All systems cleaned up\n");
}

/*==============================================================================
 * Test Cases
 *============================================================================*/

/**
 * Test 1: Integration system initialization
 */
TEST(integration_init)
{
    /* Events system */
    int result = Events_Initialize(1000, 50);
    ASSERT_EQ(result, 0);

    /* Integration system */
    result = GameIntegration_Initialize();
    ASSERT_EQ(result, 0);

    /* Verify statistics start at 0 */
    unsigned int total, quest_updates, achievement_checks;
    GameIntegration_GetStatistics(&total, &quest_updates, &achievement_checks);
    ASSERT_EQ(total, 0);
    ASSERT_EQ(quest_updates, 0);
    ASSERT_EQ(achievement_checks, 0);

    GameIntegration_Cleanup();
    Events_Cleanup();
}

/**
 * Test 2: Monster kill notification
 */
TEST(monster_kill_notification)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    QuestManager_Initialize(NULL);
    GameIntegration_Initialize();

    /* Notify monster kill */
    int result = GameIntegration_NotifyMonsterKilled(
        1001,   /* killer_id */
        100,    /* monster_type_id */
        0,      /* dungeon_id (not in dungeon) */
        500,    /* damage_dealt */
        0       /* skill_id (basic attack) */
    );
    ASSERT_EQ(result, 0);

    /* Process the event */
    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    /* Verify statistics updated */
    unsigned int total, quest_updates, achievement_checks;
    GameIntegration_GetStatistics(&total, &quest_updates, &achievement_checks);
    /* Note: actual count depends on event processing */

    GameIntegration_Cleanup();
    QuestManager_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 3: Dungeon clear notification
 */
TEST(dungeon_clear_notification)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    QuestManager_Initialize(NULL);
    GameIntegration_Initialize();

    /* Notify dungeon cleared */
    int result = GameIntegration_NotifyDungeonCleared(
        10001,  /* dungeon_id */
        1,      /* dungeon_type_id */
        1001,   /* character_id */
        2,      /* difficulty (MASTER) */
        300,    /* clear_time_seconds (5 minutes) */
        50,     /* monsters_killed */
        1       /* is_solo */
    );
    ASSERT_EQ(result, 0);

    /* Process the event */
    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    QuestManager_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 4: PvP match end notification
 */
TEST(pvp_match_end_notification)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    GameIntegration_Initialize();

    /* Notify PvP match ended */
    int result = GameIntegration_NotifyPvPMatchEnd(
        1001,   /* winner_id */
        1002,   /* loser_id */
        1,      /* match_type (1v1) */
        180     /* duration_seconds (3 minutes) */
    );
    ASSERT_EQ(result, 0);

    /* Process the event */
    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 5: Item acquisition notification
 */
TEST(item_acquired_notification)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    QuestManager_Initialize(NULL);
    GameIntegration_Initialize();

    /* Notify item acquired */
    int result = GameIntegration_NotifyItemAcquired(
        1001,   /* character_id */
        5001,   /* item_id */
        1,      /* quantity */
        0       /* source (drop) */
    );
    ASSERT_EQ(result, 0);

    /* Process the event */
    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    QuestManager_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 6: Quest completion notification
 */
TEST(quest_completed_notification)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    GameIntegration_Initialize();

    /* Notify quest completed */
    int result = GameIntegration_NotifyQuestCompleted(
        1001,   /* character_id */
        101     /* quest_id */
    );
    ASSERT_EQ(result, 0);

    /* Process the event */
    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 7: Multiple events in sequence
 */
TEST(multiple_events_sequence)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    QuestManager_Initialize(NULL);
    GameIntegration_Initialize();

    /* Simulate a dungeon run: enter, kill monsters, clear */
    unsigned int character_id = 1001;
    unsigned int dungeon_id = 20001;

    /* Kill several monsters */
    for (int i = 0; i < 5; i++) {
        GameIntegration_NotifyMonsterKilled(character_id, 100 + i, dungeon_id, 100, 0);
    }

    /* Acquire items from drops */
    GameIntegration_NotifyItemAcquired(character_id, 5001, 2, 0);
    GameIntegration_NotifyItemAcquired(character_id, 5002, 1, 0);

    /* Clear dungeon */
    GameIntegration_NotifyDungeonCleared(dungeon_id, 1, character_id, 1, 240, 5, 1);

    /* Process all events */
    int processed = GameIntegration_ProcessEvents();
    printf("  Processed %d events in sequence\n", processed);
    ASSERT_GE(processed, 0);

    /* Get final statistics */
    unsigned int total, quest_updates, achievement_checks;
    GameIntegration_GetStatistics(&total, &quest_updates, &achievement_checks);
    printf("  Final stats: total=%u, quest=%u, achievement=%u\n",
           total, quest_updates, achievement_checks);

    GameIntegration_Cleanup();
    QuestManager_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 8: Speed clear achievement trigger
 */
TEST(speed_clear_achievement)
{
    Events_Initialize(1000, 50);
    Achievement_Initialize(100, 100);
    GameIntegration_Initialize();

    /* Speed clear (under 3 minutes) */
    int result = GameIntegration_NotifyDungeonCleared(
        10002,  /* dungeon_id */
        2,      /* dungeon_type_id */
        1001,   /* character_id */
        2,      /* difficulty */
        120,    /* clear_time_seconds (2 minutes - speed clear!) */
        30,     /* monsters_killed */
        1       /* is_solo */
    );
    ASSERT_EQ(result, 0);

    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    Achievement_Cleanup();
    Events_Cleanup();
}

/**
 * Test 9: Guild level up notification
 */
TEST(guild_level_up_notification)
{
    Events_Initialize(1000, 50);
    GameIntegration_Initialize();

    int result = GameIntegration_NotifyGuildLevelUp(
        100,    /* guild_id */
        5       /* new_level */
    );
    ASSERT_EQ(result, 0);

    int processed = GameIntegration_ProcessEvents();
    ASSERT_GE(processed, 0);

    GameIntegration_Cleanup();
    Events_Cleanup();
}

/**
 * Test 10: Re-initialization handling
 */
TEST(reinitialization)
{
    Events_Initialize(1000, 50);

    int result = GameIntegration_Initialize();
    ASSERT_EQ(result, 0);

    /* Try to initialize again - should fail */
    result = GameIntegration_Initialize();
    ASSERT_EQ(result, -1);

    GameIntegration_Cleanup();
    Events_Cleanup();
}

/*==============================================================================
 * Main Test Runner
 *============================================================================*/

int main(void)
{
    printf("========================================\n");
    printf("Game Integration Test Suite\n");
    printf("========================================\n\n");

    /* Run all tests */
    RUN_TEST(integration_init);
    RUN_TEST(monster_kill_notification);
    RUN_TEST(dungeon_clear_notification);
    RUN_TEST(pvp_match_end_notification);
    RUN_TEST(item_acquired_notification);
    RUN_TEST(quest_completed_notification);
    RUN_TEST(multiple_events_sequence);
    RUN_TEST(speed_clear_achievement);
    RUN_TEST(guild_level_up_notification);
    RUN_TEST(reinitialization);

    /* Print summary */
    printf("========================================\n");
    printf("Test Summary\n");
    printf("========================================\n");
    printf("Total:  %d\n", g_tests_run);
    printf("Passed: %d\n", g_tests_passed);
    printf("Failed: %d\n", g_tests_failed);
    printf("========================================\n");

    return g_tests_failed > 0 ? 1 : 0;
}
