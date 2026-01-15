/**
 * Unit Tests for Combat Buff System
 *
 * Tests the buff/debuff management in combat_buff.c
 *
 * Test Coverage:
 * - Buff addition and removal
 * - Buff expiration (time-based)
 * - Buff stacking
 * - Multiple buffs per character
 * - Edge cases and boundary conditions
 *
 * Target: ≥70% code coverage
 * Created: 2025-11-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>  // for sleep()
#include "../../src/combat/combat_buff.h"

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
        CombatBuff_Initialize(); \
        test_##name(); \
        CombatBuff_Cleanup(); \
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
 * Test: Add single buff to character
 */
TEST(add_single_buff) {
    int character_id = 1;
    int buff_id = 101;
    int duration = 5000;  // 5 seconds

    int ret = CombatBuff_Add(character_id, buff_id, duration);

    ASSERT_EQ(ret, 0);

    // Verify buff is active
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 1);
    ASSERT_EQ(buffs[0].buff_id, buff_id);
    ASSERT_EQ(buffs[0].duration_ms, duration);
}

/**
 * Test: Add multiple buffs
 */
TEST(add_multiple_buffs) {
    int character_id = 1;

    CombatBuff_Add(character_id, 101, 5000);
    CombatBuff_Add(character_id, 102, 5000);
    CombatBuff_Add(character_id, 103, 5000);

    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 3);
}

/**
 * Test: Remove buff by ID
 */
TEST(remove_buff) {
    int character_id = 1;
    int buff_id = 101;

    CombatBuff_Add(character_id, buff_id, 5000);
    int ret = CombatBuff_Remove(character_id, buff_id);

    ASSERT_EQ(ret, 0);

    // Verify buff is removed
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 0);
}

/**
 * Test: Remove non-existent buff
 */
TEST(remove_nonexistent_buff) {
    int character_id = 1;
    int buff_id = 999;

    int ret = CombatBuff_Remove(character_id, buff_id);

    // Should return error
    ASSERT_TRUE(ret != 0);
}

/**
 * Test: Buff expiration
 */
TEST(buff_expiration) {
    int character_id = 1;
    int buff_id = 101;
    int duration = 2000;  // 2 seconds

    CombatBuff_Add(character_id, buff_id, duration);

    // Wait for buff to expire
    sleep(3);

    // Update buffs (should remove expired)
    int removed = CombatBuff_Update(character_id);

    ASSERT_EQ(removed, 1);

    // Verify buff is gone
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 0);
}

/**
 * Test: Max buffs per character
 */
TEST(max_buffs_limit) {
    int character_id = 1;

    // Add MAX_BUFFS_PER_CHARACTER buffs
    for (int i = 0; i < MAX_BUFFS_PER_CHARACTER; i++) {
        int ret = CombatBuff_Add(character_id, 100 + i, 5000);
        ASSERT_EQ(ret, 0);
    }

    // Try to add one more (should fail)
    int ret = CombatBuff_Add(character_id, 999, 5000);
    ASSERT_TRUE(ret != 0);

    // Verify count is at max
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, MAX_BUFFS_PER_CHARACTER);
}

/**
 * Test: Multiple characters
 */
TEST(multiple_characters) {
    int char1 = 1;
    int char2 = 2;

    CombatBuff_Add(char1, 101, 5000);
    CombatBuff_Add(char1, 102, 5000);
    CombatBuff_Add(char2, 201, 5000);

    // Verify char1 has 2 buffs
    ActiveBuff buffs1[MAX_BUFFS_PER_CHARACTER];
    int count1 = CombatBuff_GetActive(char1, buffs1, MAX_BUFFS_PER_CHARACTER);
    ASSERT_EQ(count1, 2);

    // Verify char2 has 1 buff
    ActiveBuff buffs2[MAX_BUFFS_PER_CHARACTER];
    int count2 = CombatBuff_GetActive(char2, buffs2, MAX_BUFFS_PER_CHARACTER);
    ASSERT_EQ(count2, 1);
}

/**
 * Test: Invalid character ID
 */
TEST(invalid_character_id) {
    int invalid_id = -1;
    int buff_id = 101;

    int ret = CombatBuff_Add(invalid_id, buff_id, 5000);

    // Should return error
    ASSERT_TRUE(ret != 0);
}

/**
 * Test: Buff update without expiration
 */
TEST(buff_update_no_expiration) {
    int character_id = 1;

    CombatBuff_Add(character_id, 101, 10000);  // 10 seconds

    // Update immediately (no expiration yet)
    int removed = CombatBuff_Update(character_id);

    ASSERT_EQ(removed, 0);

    // Verify buff still active
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 1);
}

/**
 * Test: Mixed expiration (some expire, some don't)
 */
TEST(mixed_expiration) {
    int character_id = 1;

    CombatBuff_Add(character_id, 101, 2000);  // 2 seconds
    CombatBuff_Add(character_id, 102, 10000); // 10 seconds

    // Wait for first buff to expire
    sleep(3);

    int removed = CombatBuff_Update(character_id);

    ASSERT_EQ(removed, 1);

    // Verify only one buff remains
    ActiveBuff buffs[MAX_BUFFS_PER_CHARACTER];
    int count = CombatBuff_GetActive(character_id, buffs, MAX_BUFFS_PER_CHARACTER);

    ASSERT_EQ(count, 1);
    ASSERT_EQ(buffs[0].buff_id, 102);
}

/**
 * Main test runner
 */
int main(void) {
    printf("=== Combat Buff System Unit Tests ===\n\n");

    // Run all tests
    run_test_add_single_buff();
    run_test_add_multiple_buffs();
    run_test_remove_buff();
    run_test_remove_nonexistent_buff();
    run_test_buff_expiration();
    run_test_max_buffs_limit();
    run_test_multiple_characters();
    run_test_invalid_character_id();
    run_test_buff_update_no_expiration();
    run_test_mixed_expiration();

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
