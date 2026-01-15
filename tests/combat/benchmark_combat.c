/**
 * Combat System Performance Benchmark
 *
 * Tests:
 * - Damage calculation performance (target: <1ms per calculation)
 * - Buff update performance (target: <0.5ms for 100 buffs)
 * - Combat log performance (target: <5ms for 100 entries)
 *
 * Created: 2025-11-23
 * Phase: 6.1 - Combat System Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../src/combat/combat_damage.h"
#include "../../src/combat/combat_buff.h"
#include "../../src/combat/combat_log.h"
#include "../../src/combat/combat_data_loader.h"

// Performance measurement helpers
#define BENCHMARK_ITERATIONS 10000

typedef struct {
    const char* test_name;
    int iterations;
    double total_time_ms;
    double avg_time_ms;
    double target_ms;
    int passed;
} BenchmarkResult;

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * Get current time in microseconds
 */
static double get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000.0) + (ts.tv_nsec / 1000.0);
}

/**
 * Print benchmark result
 */
static void print_result(BenchmarkResult* result) {
    tests_run++;

    result->avg_time_ms = result->total_time_ms / result->iterations;
    result->passed = (result->avg_time_ms <= result->target_ms);

    if (result->passed) {
        tests_passed++;
        printf("✅ [PASS] %s\n", result->test_name);
    } else {
        tests_failed++;
        printf("❌ [FAIL] %s\n", result->test_name);
    }

    printf("   Iterations: %d\n", result->iterations);
    printf("   Total time: %.2f ms\n", result->total_time_ms);
    printf("   Avg time:   %.6f ms (target: %.6f ms)\n", result->avg_time_ms, result->target_ms);

    if (result->passed) {
        printf("   Performance: %.2fx faster than target\n", result->target_ms / result->avg_time_ms);
    } else {
        printf("   Performance: %.2fx slower than target ⚠️\n", result->avg_time_ms / result->target_ms);
    }
    printf("\n");
}

/**
 * Benchmark: Damage Calculation
 * Target: <1ms per calculation (actually much faster, target <0.001ms)
 */
static void benchmark_damage_calculation(void) {
    printf("=== Damage Calculation Benchmark ===\n\n");

    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    // Setup test data
    attacker.level = 60;
    attacker.strength = 200;
    attacker.intelligence = 150;
    attacker.physical_attack = 1000;
    attacker.critical_rate = 30;
    attacker.critical_damage = 50;

    defender.level = 60;
    defender.physical_defense = 500;
    defender.magical_defense = 400;

    skill.skill_id = 101;
    skill.base_damage = 1000;
    skill.damage_multiplier = 150;

    // Initialize system
    CombatDamage_Initialize();

    // Benchmark
    double start = get_time_us();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);
    }
    double end = get_time_us();

    // Cleanup
    CombatDamage_Cleanup();

    // Report
    BenchmarkResult bench = {
        .test_name = "Damage Calculation",
        .iterations = BENCHMARK_ITERATIONS,
        .total_time_ms = (end - start) / 1000.0,
        .target_ms = 0.001  // 1 microsecond = 0.001 ms
    };

    print_result(&bench);
}

/**
 * Benchmark: Buff Update (100 buffs)
 * Target: <0.5ms
 */
static void benchmark_buff_update(void) {
    printf("=== Buff Update Benchmark ===\n\n");

    // Initialize system
    CombatBuff_Initialize();

    int character_id = 1;

    // Add 100 buffs with varying durations
    for (int i = 0; i < 100; i++) {
        int buff_id = 100 + i;
        int duration = 5000 + (i * 100);  // 5s to 15s
        CombatBuff_Add(character_id, buff_id, duration);
    }

    // Benchmark: Update all buffs (check for expiration)
    int iterations = 1000;
    double start = get_time_us();
    for (int i = 0; i < iterations; i++) {
        CombatBuff_Update(character_id);
    }
    double end = get_time_us();

    // Cleanup
    CombatBuff_Cleanup();

    // Report
    BenchmarkResult bench = {
        .test_name = "Buff Update (100 buffs)",
        .iterations = iterations,
        .total_time_ms = (end - start) / 1000.0,
        .target_ms = 0.5  // 0.5ms
    };

    print_result(&bench);
}

/**
 * Benchmark: Combat Log (100 entries)
 * Target: <5ms for batch insert
 */
static void benchmark_combat_log(void) {
    printf("=== Combat Log Benchmark ===\n\n");

    // Initialize system
    CombatLog_Initialize();

    // Benchmark: Log 100 attack events
    int iterations = 100;
    double start = get_time_us();

    for (int i = 0; i < iterations; i++) {
        CombatLog_LogAttack(1, 2, 101 + i, 500 + i, (i % 10 == 0));
    }

    // Flush logs (this is the critical operation)
    double flush_start = get_time_us();
    int flushed = CombatLog_Flush();
    double flush_end = get_time_us();

    double end = get_time_us();

    // Cleanup
    CombatLog_Cleanup();

    // Report total logging time
    BenchmarkResult bench_total = {
        .test_name = "Combat Log - Total (100 entries)",
        .iterations = 1,
        .total_time_ms = (end - start) / 1000.0,
        .target_ms = 5.0  // 5ms for 100 entries
    };

    printf("Flushed %d log entries\n\n", flushed);
    print_result(&bench_total);

    // Report flush time separately
    BenchmarkResult bench_flush = {
        .test_name = "Combat Log - Flush only",
        .iterations = 1,
        .total_time_ms = (flush_end - flush_start) / 1000.0,
        .target_ms = 5.0  // 5ms for flush
    };

    print_result(&bench_flush);
}

/**
 * Benchmark: Combined Combat Scenario
 * Simulates realistic combat with all systems
 */
static void benchmark_combined_scenario(void) {
    printf("=== Combined Combat Scenario Benchmark ===\n\n");

    // Initialize all systems
    CombatDamage_Initialize();
    CombatBuff_Initialize();
    CombatLog_Initialize();

    CharacterCombatData attacker = {0};
    CharacterCombatData defender = {0};
    SkillCombatData skill = {0};
    CombatDamageResult result = {0};

    // Setup
    attacker.strength = 200;
    attacker.intelligence = 150;
    attacker.physical_attack = 1000;
    attacker.critical_rate = 30;
    attacker.critical_damage = 50;

    defender.physical_defense = 500;

    skill.base_damage = 1000;
    skill.damage_multiplier = 150;

    int character_id = 1;

    // Benchmark: Realistic combat turn
    int iterations = 1000;
    double start = get_time_us();

    for (int i = 0; i < iterations; i++) {
        // 1. Calculate damage
        CombatDamage_CalculateFinal(&attacker, &defender, &skill, &result);

        // 2. Update buffs
        CombatBuff_Update(character_id);

        // 3. Apply a new buff every 10 iterations
        if (i % 10 == 0) {
            CombatBuff_Add(character_id, 100 + (i / 10), 5000);
        }

        // 4. Log the attack
        CombatLog_LogAttack(1, 2, 101, result.final_damage, result.is_critical);

        // 5. Flush logs every 100 iterations
        if (i % 100 == 0 && i > 0) {
            CombatLog_Flush();
        }
    }

    double end = get_time_us();

    // Cleanup
    CombatLog_Flush();
    CombatBuff_Cleanup();
    CombatDamage_Cleanup();
    CombatLog_Cleanup();

    // Report
    BenchmarkResult bench = {
        .test_name = "Combined Combat Turn",
        .iterations = iterations,
        .total_time_ms = (end - start) / 1000.0,
        .target_ms = 0.01  // 10 microseconds per turn
    };

    print_result(&bench);
}

/**
 * Main benchmark runner
 */
int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║        Combat System Performance Benchmark               ║\n");
    printf("║        Phase 6.1 - Performance Verification              ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");

    // Run benchmarks
    benchmark_damage_calculation();
    benchmark_buff_update();
    benchmark_combat_log();
    benchmark_combined_scenario();

    // Print summary
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    Summary                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Tests Run:    %d\n", tests_run);
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("\n");

    if (tests_failed == 0) {
        printf("✅ ALL PERFORMANCE TARGETS MET!\n");
        printf("\nThe Combat System meets or exceeds all performance requirements.\n");
        return 0;
    } else {
        printf("❌ SOME PERFORMANCE TARGETS NOT MET\n");
        printf("\nOptimization may be required for production use.\n");
        return 1;
    }
}
