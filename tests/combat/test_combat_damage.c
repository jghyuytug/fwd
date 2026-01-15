/**
 * Unit Tests for Combat Damage Calculation
 *
 * Tests the damage formula implementation in combat_damage.c
 *
 * Test Coverage:
 * - Base damage calculation
 * - Attribute modifiers
 * - Defense reduction
 * - Critical hits
 * - Edge cases and boundary conditions
 *
 * Target: ≥70% code coverage
 * Created: 2025-11-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../src/combat/combat_damage.h"
#include "../../src/combat/combat_data_loader.h"

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
        test_##name(); \
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

#define ASSERT_NEAR(a, b, tolerance) \
    do { \
        int diff = abs((int)(a) - (int)(b)); \
        if (diff <= (tolerance)) { \
            tests_passed++; \
            printf("  [✓] PASS: %s ≈ %s (diff: %d <= %d)\n", #a, #b, diff, tolerance); \
        } else { \
            tests_failed++; \
            printf("  [✗] FAIL: %s ≈ %s (diff: %d > %d) (line %d)\n", #a, #b, diff, tolerance, __LINE__); \
        } \
    } while(0)

/**
 * Test: Basic damage calculation without modifiers
 */
TEST(basic_damage) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    // Setup: Simple attack, no attributes
    skill.base_damage = 100;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 0;
    attacker.critical_rate = 0;

    int ret = CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    ASSERT_EQ(ret, 0);
    ASSERT_EQ(result.final_damage, 100);
    ASSERT_TRUE(!result.is_critical);
    ASSERT_TRUE(!result.is_evaded);
}

/**
 * Test: Damage with skill multiplier
 */
TEST(skill_multiplier) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    // Skill: 100 base damage, 50% multiplier
    skill.base_damage = 100;
    skill.damage_multiplier = 50;  // +50%
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 0;
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Expected: 100 * 1.5 = 150
    ASSERT_EQ(result.final_damage, 150);
}

/**
 * Test: Attribute modifiers (strength + intelligence)
 */
TEST(attribute_modifiers) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 100;
    skill.damage_multiplier = 0;
    attacker.strength = 100;
    attacker.intelligence = 100;
    defender.physical_defense = 0;
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Expected: 100 * (1 + (100+100)/200) = 100 * 2 = 200
    ASSERT_EQ(result.final_damage, 200);
}

/**
 * Test: Defense reduction
 */
TEST(defense_reduction) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 1000;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 1000;
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Expected: 1000 - (1000 * 1000/(1000+1000)) = 1000 - 500 = 500
    ASSERT_EQ(result.final_damage, 500);
}

/**
 * Test: High defense edge case
 */
TEST(high_defense) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 100;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 9999;
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Very high defense should reduce damage significantly
    ASSERT_TRUE(result.final_damage < 100);
    ASSERT_TRUE(result.final_damage >= 1);  // But never 0
}

/**
 * Test: Critical hit (100% crit rate)
 */
TEST(critical_hit) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 100;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 0;
    attacker.critical_rate = 100;  // 100% crit
    attacker.critical_damage = 50;  // +50% crit damage

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Expected: 100 * 1.5 = 150 (if crit)
    ASSERT_TRUE(result.is_critical);
    ASSERT_EQ(result.final_damage, 150);
}

/**
 * Test: Combined modifiers (complex calculation)
 */
TEST(combined_modifiers) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    // Complex scenario:
    // - Skill: 100 base, 50% multiplier
    // - Attacker: 100 STR, 100 INT
    // - Defender: 500 DEF
    // - Crit: 100% rate, 50% damage

    skill.base_damage = 100;
    skill.damage_multiplier = 50;
    attacker.strength = 100;
    attacker.intelligence = 100;
    defender.physical_defense = 500;
    attacker.critical_rate = 100;
    attacker.critical_damage = 50;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Step-by-step:
    // 1. Base: 100 * 1.5 = 150
    // 2. Attributes: 150 * (1 + 200/200) = 150 * 2 = 300
    // 3. Defense: 300 - (300 * 500/1500) = 300 - 100 = 200
    // 4. Critical: 200 * 1.5 = 300

    ASSERT_EQ(result.final_damage, 300);
    ASSERT_TRUE(result.is_critical);
}

/**
 * Test: Zero damage edge case
 */
TEST(zero_damage) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 0;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = 0;
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    ASSERT_EQ(result.final_damage, 0);
}

/**
 * Test: Negative defense (should be treated as 0)
 */
TEST(negative_defense) {
    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    skill.base_damage = 100;
    skill.damage_multiplier = 0;
    attacker.strength = 0;
    attacker.intelligence = 0;
    defender.physical_defense = -100;  // Invalid input
    attacker.critical_rate = 0;

    CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

    // Negative defense should be clamped to 0
    ASSERT_TRUE(result.final_damage >= 100);
}

/**
 * Main test runner
 */
int main(void) {
    printf("=== Combat Damage Calculation Unit Tests ===\n\n");

    // Initialize damage system
    CombatDamage_Initialize();

    // Run all tests
    run_test_basic_damage();
    run_test_skill_multiplier();
    run_test_attribute_modifiers();
    run_test_defense_reduction();
    run_test_high_defense();
    run_test_critical_hit();
    run_test_combined_modifiers();
    run_test_zero_damage();
    run_test_negative_defense();

    // Cleanup
    CombatDamage_Cleanup();

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
