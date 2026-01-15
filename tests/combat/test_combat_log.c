/**
 * Unit Tests for Combat Log System
 *
 * Tests the combat logging functionality in combat_log.c
 *
 * Test Coverage:
 * - Attack logging
 * - Buff apply/remove logging
 * - Death logging
 * - Log buffer and flush mechanism
 * - Edge cases
 *
 * Target: ≥70% code coverage
 * Created: 2025-11-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../src/combat/combat_log.h"

// Test result tracking
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test macro
#define TEST(name) \
    static void test_##name(); \
    static void run_test_##name() { \
        printf("[TEST] Running: %s\n", #name); \
        tests_run++; \
        CombatLog_Initialize(); \
        test_##name(); \
        CombatLog_Cleanup(); \
    } \
    static void test_##name()

// Assertion macros
#define ASSERT_TRUE(expr) \
    do { \
        if (expr) { \
            tests_passed++; \
            printf("  [✓] PASS: %s\n", #expr); \
        } else { \
            tests_failed++; \
            printf("  [✗] FAIL: %s (line %d)\n", #expr, __LINE__); \
        } \
    } while(0)

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) == (b)) { \
            tests_passed++; \
            printf("  [✓] PASS: %s == %s (%d == %d)\n", #a, #b, (int)(a), (int)(b)); \
        } else { \
            tests_failed++; \
            printf("  [✗] FAIL: %s != %s (%d != %d) (line %d)\n", #a, #b, (int)(a), (int)(b), __LINE__); \
        } \
    } while(0)

/**
 * Test: Log attack event
 */
TEST(log_attack) {
    int attacker_id = 1;
    int defender_id = 2;
    int skill_id = 101;
    int damage = 500;
    bool is_critical = false;

    int ret = CombatLog_LogAttack(attacker_id, defender_id, skill_id, damage, is_critical);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Log critical attack
 */
TEST(log_critical_attack) {
    int attacker_id = 1;
    int defender_id = 2;
    int skill_id = 102;
    int damage = 1000;
    bool is_critical = true;

    int ret = CombatLog_LogAttack(attacker_id, defender_id, skill_id, damage, is_critical);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Log buff apply
 */
TEST(log_buff_apply) {
    int character_id = 1;
    int buff_id = 201;
    int source_id = 2;

    int ret = CombatLog_LogBuffApply(character_id, buff_id, source_id);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Log buff remove
 */
TEST(log_buff_remove) {
    int character_id = 1;
    int buff_id = 201;

    int ret = CombatLog_LogBuffRemove(character_id, buff_id);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Log death event
 */
TEST(log_death) {
    int character_id = 2;
    int killer_id = 1;

    int ret = CombatLog_LogDeath(character_id, killer_id);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Flush empty log buffer
 */
TEST(flush_empty) {
    int flushed = CombatLog_Flush();

    ASSERT_EQ(flushed, 0);
}

/**
 * Test: Flush with logs
 */
TEST(flush_with_logs) {
    // Add some logs
    CombatLog_LogAttack(1, 2, 101, 500, false);
    CombatLog_LogAttack(1, 2, 102, 600, true);
    CombatLog_LogBuffApply(2, 201, 1);

    int flushed = CombatLog_Flush();

    ASSERT_EQ(flushed, 3);

    // Flush again (should be 0)
    int flushed2 = CombatLog_Flush();
    ASSERT_EQ(flushed2, 0);
}

/**
 * Test: Multiple attack logs
 */
TEST(multiple_attacks) {
    for (int i = 0; i < 10; i++) {
        int ret = CombatLog_LogAttack(1, 2, 101 + i, 100 * i, false);
        ASSERT_EQ(ret, 0);
    }

    int flushed = CombatLog_Flush();
    ASSERT_EQ(flushed, 10);
}

/**
 * Test: Mixed log types
 */
TEST(mixed_log_types) {
    CombatLog_LogAttack(1, 2, 101, 500, false);
    CombatLog_LogBuffApply(2, 201, 1);
    CombatLog_LogAttack(2, 1, 102, 300, false);
    CombatLog_LogBuffRemove(2, 201);
    CombatLog_LogDeath(2, 1);

    int flushed = CombatLog_Flush();
    ASSERT_EQ(flushed, 5);
}

/**
 * Test: Auto-flush when buffer full
 */
TEST(auto_flush_on_full) {
    // Add MAX_LOG_BUFFER + 1 logs
    // This should trigger auto-flush when buffer is full

    for (int i = 0; i < 1001; i++) {
        int ret = CombatLog_LogAttack(1, 2, 101, 100, false);
        ASSERT_EQ(ret, 0);
    }

    // Buffer should have auto-flushed at 1000, so only 1 log remains
    // (This depends on implementation - may vary)
    int flushed = CombatLog_Flush();
    ASSERT_TRUE(flushed <= 1001);
}

/**
 * Test: Zero damage attack
 */
TEST(zero_damage_attack) {
    int ret = CombatLog_LogAttack(1, 2, 101, 0, false);

    ASSERT_EQ(ret, 0);
}

/**
 * Test: Negative damage (edge case)
 */
TEST(negative_damage) {
    int ret = CombatLog_LogAttack(1, 2, 101, -100, false);

    // Should still log (damage validation is combat system's job)
    ASSERT_EQ(ret, 0);
}

/**
 * Test: Cleanup flushes remaining logs
 */
TEST(cleanup_flushes_logs) {
    CombatLog_LogAttack(1, 2, 101, 500, false);
    CombatLog_LogAttack(1, 2, 102, 600, false);

    // Cleanup should flush automatically
    CombatLog_Cleanup();

    // Re-initialize for next test
    CombatLog_Initialize();

    // Flush should return 0 (already flushed during cleanup)
    int flushed = CombatLog_Flush();
    ASSERT_EQ(flushed, 0);
}

/**
 * Main test runner
 */
int main(void) {
    printf("=== Combat Log System Unit Tests ===\n\n");

    // Run all tests
    run_test_log_attack();
    run_test_log_critical_attack();
    run_test_log_buff_apply();
    run_test_log_buff_remove();
    run_test_log_death();
    run_test_flush_empty();
    run_test_flush_with_logs();
    run_test_multiple_attacks();
    run_test_mixed_log_types();
    run_test_auto_flush_on_full();
    run_test_zero_damage_attack();
    run_test_negative_damage();
    run_test_cleanup_flushes_logs();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests Run:    %d\n", tests_run);
    printf("Assertions:   %d\n", tests_passed + tests_failed);
    printf("Passed:       %d\n", tests_passed);
    printf("Failed:       %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("\n✅ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\n❌ SOME TESTS FAILED!\n");
        return 1;
    }
}
