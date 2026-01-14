/**
 * @file test_charac_stat_orm.c
 * @brief Character Statistics ORM Test Suite
 *
 * Tests all 12 CharacStat ORM functions:
 * 1. CharacStat_Create
 * 2. CharacStat_GetByCharacNo
 * 3. CharacStat_Update
 * 4. CharacStat_Delete
 * 5. CharacStat_Exists
 * 6. CharacStat_UpdateExp
 * 7. CharacStat_UpdateFatigue
 * 8. CharacStat_UpdateLastPlayTime
 * 9. CharacStat_IncrementDungeonPlayCount
 * 10. CharacStat_UpdateChaosStats
 * 11. CharacStat_GetByExpRange
 * 12. CharacStat_GetTotalCount
 *
 * Database: test_taiwan_cain (DB_TYPE_CAIN)
 * Test ID: 888888
 *
 * @version 1.0
 * @date 2025-01-13
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "charac_stat_orm.h"

#define TEST_CHARAC_NO 888888
#define TEST_CHARAC_NO2 888889
#define TEST_CHARAC_NO3 888890

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

/* ANSI color codes */
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

void print_test_result(const char* test_name, int passed) {
    if (passed) {
        printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", test_name);
        tests_passed++;
    } else {
        printf(COLOR_RED "[FAIL]" COLOR_RESET " %s\n", test_name);
        tests_failed++;
    }
}

/**
 * @brief Cleanup test data before tests
 */
void cleanup_before_tests(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult cleanup_result;

    printf(COLOR_YELLOW "\n[INFO] Cleaning up test data before tests...\n" COLOR_RESET);

    snprintf(query, sizeof(query),
             "DELETE FROM charac_stat WHERE charac_no IN (%d, %d, %d)",
             TEST_CHARAC_NO, TEST_CHARAC_NO2, TEST_CHARAC_NO3);

    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);

    printf(COLOR_YELLOW "[INFO] Cleanup complete.\n" COLOR_RESET);
}

/**
 * @brief Cleanup test data after tests
 */
void cleanup_after_tests(DBConnectionManager* manager) {
    char query[256];
    DBQueryResult cleanup_result;

    printf(COLOR_YELLOW "\n[INFO] Cleaning up test data after tests...\n" COLOR_RESET);

    snprintf(query, sizeof(query),
             "DELETE FROM charac_stat WHERE charac_no IN (%d, %d, %d)",
             TEST_CHARAC_NO, TEST_CHARAC_NO2, TEST_CHARAC_NO3);

    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);

    printf(COLOR_YELLOW "[INFO] Final cleanup complete.\n" COLOR_RESET);
}

/**
 * @brief Test 1: CharacStat_Create
 */
void test_create(DBConnectionManager* manager) {
    CharacStat test_stat;
    int ret;

    memset(&test_stat, 0, sizeof(CharacStat));

    /* Fill test data */
    test_stat.charac_no = TEST_CHARAC_NO;
    test_stat.village = 1;
    test_stat.exp = 100000;
    test_stat.HP = 100;
    test_stat.fatigue = 156;
    test_stat.used_fatigue = 0;
    test_stat.premium_fatigue = 0;
    test_stat.dungeon_clear_point = 500;
    strcpy(test_stat.last_play_time, "2025-01-13 10:00:00");
    strcpy(test_stat.forbidden_to_play, "N");
    strcpy(test_stat.forbidden_due_to, "0000-00-00 00:00:00");
    test_stat.tutorial_flag = 1;
    test_stat.trade_gold_total = 1000000;
    test_stat.trade_gold_total_billion = 0;
    test_stat.trade_gold_daily = 50000;
    test_stat.dungeon_map_pass_cnt = 100;
    test_stat.dungeon_map_help_pass_cnt = 50;
    test_stat.help_abuse_point = 0;
    test_stat.chaos_point = 200;
    test_stat.chaos_exp = 5000;
    test_stat.chaos_mode_count = 10;
    test_stat.chaos_kill_count = 5;
    test_stat.chaos_die_count = 2;
    strcpy(test_stat.chaos_die_time, "0000-00-00 00:00:00");
    strcpy(test_stat.chaos_kill_time, "0000-00-00 00:00:00");
    test_stat.assault_count = 3;
    test_stat.luck_point = 50;
    test_stat.dungeon_play_count = 150;
    test_stat.help_abuse_ratio = 0;
    test_stat.help_abuse_exp = 0;
    test_stat.expert_job_exp = 0;
    test_stat.fatigue_battery_charging = 0;
    strcpy(test_stat.escalade_tutorial_flag, "0");
    test_stat.power_war_point = 100;
    test_stat.power_war_assault_count = 5;
    test_stat.power_war_assault_victory_count = 3;
    test_stat.fatigue_grownup_buff = 0;
    test_stat.village_prev = 1;
    strcpy(test_stat.last_play_time_powerwar, "0000-00-00 00:00:00");
    test_stat.emotion = 0;
    test_stat.add_slot_flag = 0;
    test_stat.member_dungeon_flag = 0;
    test_stat.open_flag = 1;
    test_stat.member_bonus_fatigue = 0;
    strcpy(test_stat.birthday_effect_time, "0000-00-00 00:00:00");
    test_stat.visible_flags = 0;
    test_stat.add_equipslot_flag = 0;
    test_stat.channel_equipslot_switch = 0;
    test_stat.expand_equipslot_switch = 0;
    test_stat.growth_power_reward = 0;
    strcpy(test_stat.chaos_respon_time, "0000-00-00 00:00:00");
    test_stat.last_play_dungeon_index = 0;
    test_stat.total_play_time = 3600;

    ret = CharacStat_Create(manager, &test_stat);
    print_test_result("CharacStat_Create", ret == 0);
}

/**
 * @brief Test 2: CharacStat_GetByCharacNo
 */
void test_get_by_charac_no(DBConnectionManager* manager) {
    CharacStat result;
    int ret;

    memset(&result, 0, sizeof(CharacStat));
    ret = CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);

    int passed = (ret == 0 &&
                  result.charac_no == TEST_CHARAC_NO &&
                  result.village == 1 &&
                  result.exp == 100000 &&
                  result.HP == 100 &&
                  result.fatigue == 156);

    print_test_result("CharacStat_GetByCharacNo", passed);
}

/**
 * @brief Test 3: CharacStat_Exists (should exist)
 */
void test_exists_true(DBConnectionManager* manager) {
    int ret = CharacStat_Exists(manager, TEST_CHARAC_NO);
    print_test_result("CharacStat_Exists (TRUE)", ret == 1);
}

/**
 * @brief Test 4: CharacStat_UpdateExp
 */
void test_update_exp(DBConnectionManager* manager) {
    CharacStat result;
    int ret;

    ret = CharacStat_UpdateExp(manager, TEST_CHARAC_NO, 200000);

    if (ret == 0) {
        memset(&result, 0, sizeof(CharacStat));
        CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
        print_test_result("CharacStat_UpdateExp", result.exp == 200000);
    } else {
        print_test_result("CharacStat_UpdateExp", 0);
    }
}

/**
 * @brief Test 5: CharacStat_UpdateFatigue
 */
void test_update_fatigue(DBConnectionManager* manager) {
    CharacStat result;
    int ret;

    ret = CharacStat_UpdateFatigue(manager, TEST_CHARAC_NO, 100, 56, 50);

    if (ret == 0) {
        memset(&result, 0, sizeof(CharacStat));
        CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
        print_test_result("CharacStat_UpdateFatigue",
                         result.fatigue == 100 &&
                         result.used_fatigue == 56 &&
                         result.premium_fatigue == 50);
    } else {
        print_test_result("CharacStat_UpdateFatigue", 0);
    }
}

/**
 * @brief Test 6: CharacStat_UpdateLastPlayTime
 */
void test_update_last_play_time(DBConnectionManager* manager) {
    int ret = CharacStat_UpdateLastPlayTime(manager, TEST_CHARAC_NO);
    print_test_result("CharacStat_UpdateLastPlayTime", ret == 0);
}

/**
 * @brief Test 7: CharacStat_IncrementDungeonPlayCount
 */
void test_increment_dungeon_play_count(DBConnectionManager* manager) {
    CharacStat result;
    int ret;
    unsigned int old_count;

    memset(&result, 0, sizeof(CharacStat));
    CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
    old_count = result.dungeon_play_count;

    ret = CharacStat_IncrementDungeonPlayCount(manager, TEST_CHARAC_NO);

    if (ret == 0) {
        memset(&result, 0, sizeof(CharacStat));
        CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
        print_test_result("CharacStat_IncrementDungeonPlayCount",
                         result.dungeon_play_count == old_count + 1);
    } else {
        print_test_result("CharacStat_IncrementDungeonPlayCount", 0);
    }
}

/**
 * @brief Test 8: CharacStat_UpdateChaosStats
 */
void test_update_chaos_stats(DBConnectionManager* manager) {
    CharacStat result;
    int ret;

    ret = CharacStat_UpdateChaosStats(manager, TEST_CHARAC_NO, 500, 10000, 15, 5);

    if (ret == 0) {
        memset(&result, 0, sizeof(CharacStat));
        CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
        print_test_result("CharacStat_UpdateChaosStats",
                         result.chaos_point == 500 &&
                         result.chaos_exp == 10000 &&
                         result.chaos_kill_count == 15 &&
                         result.chaos_die_count == 5);
    } else {
        print_test_result("CharacStat_UpdateChaosStats", 0);
    }
}

/**
 * @brief Test 9: CharacStat_Update (full update)
 */
void test_update(DBConnectionManager* manager) {
    CharacStat test_stat, result;
    int ret;

    memset(&test_stat, 0, sizeof(CharacStat));
    CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &test_stat);

    /* Modify some fields */
    test_stat.village = 2;
    test_stat.HP = 80;
    test_stat.luck_point = 100;

    ret = CharacStat_Update(manager, &test_stat);

    if (ret == 0) {
        memset(&result, 0, sizeof(CharacStat));
        CharacStat_GetByCharacNo(manager, TEST_CHARAC_NO, &result);
        print_test_result("CharacStat_Update",
                         result.village == 2 &&
                         result.HP == 80 &&
                         result.luck_point == 100);
    } else {
        print_test_result("CharacStat_Update", 0);
    }
}

/**
 * @brief Test 10: CharacStat_GetByExpRange (create additional test records)
 */
void test_get_by_exp_range(DBConnectionManager* manager) {
    CharacStat results[10];
    int count;

    /* Query range: 50000 - 200000 (should get at least the 888888 record with exp=100000) */
    memset(results, 0, sizeof(results));
    count = CharacStat_GetByExpRange(manager, 50000, 200000, results, 10);
    printf("[DEBUG] GetByExpRange returned count=%d (expected >= 1)\n", count);
    if (count > 0) {
        for (int i = 0; i < count && i < 3; i++) {
            printf("[DEBUG]   Result[%d]: charac_no=%d, exp=%d\n", i, results[i].charac_no, results[i].exp);
        }
    }

    print_test_result("CharacStat_GetByExpRange", count >= 1);
}

/**
 * @brief Test 11: CharacStat_GetTotalCount
 */
void test_get_total_count(DBConnectionManager* manager) {
    int count = CharacStat_GetTotalCount(manager);
    printf("[DEBUG] GetTotalCount returned: %d (expected >= 1)\n", count);
    print_test_result("CharacStat_GetTotalCount", count >= 1);
}

/**
 * @brief Test 12: CharacStat_Delete
 */
void test_delete(DBConnectionManager* manager) {
    int ret;

    ret = CharacStat_Delete(manager, TEST_CHARAC_NO);

    if (ret == 0) {
        int exists = CharacStat_Exists(manager, TEST_CHARAC_NO);
        print_test_result("CharacStat_Delete", exists == 0);
    } else {
        print_test_result("CharacStat_Delete", 0);
    }
}

/**
 * @brief Test 13: CharacStat_Exists (should not exist after delete)
 */
void test_exists_false(DBConnectionManager* manager) {
    int ret = CharacStat_Exists(manager, TEST_CHARAC_NO);
    print_test_result("CharacStat_Exists (FALSE)", ret == 0);
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    const char* config_file;
    int ret;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        printf("Example: %s ../config/database.cfg\n", argv[0]);
        return 1;
    }

    config_file = argv[1];

    printf(COLOR_BLUE "========================================\n");
    printf("  CharacStat ORM Test Suite\n");
    printf("========================================\n" COLOR_RESET);
    printf("Database: test_taiwan_cain (DB_TYPE_CAIN)\n");
    printf("Test IDs: %d, %d, %d\n", TEST_CHARAC_NO, TEST_CHARAC_NO2, TEST_CHARAC_NO3);
    printf("Total Tests: 13\n\n");

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    ret = DBConnectionManager_Initialize(&manager, config_file);
    if (ret < 0) {
        printf(COLOR_RED "[ERROR] Failed to initialize database connection\n" COLOR_RESET);
        return 1;
    }

    printf(COLOR_GREEN "[OK] Database connection initialized\n" COLOR_RESET);

    /* Connect to database */
    printf(COLOR_YELLOW "[INFO] Connecting to test_taiwan_cain database...\n" COLOR_RESET);
    ret = DBConnectionManager_Connect(&manager, DB_TYPE_CAIN);
    if (ret < 0) {
        printf(COLOR_RED "[ERROR] Failed to connect to database\n" COLOR_RESET);
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }
    printf(COLOR_GREEN "[OK] Database connected successfully\n" COLOR_RESET);

    /* Cleanup before tests */
    cleanup_before_tests(&manager);

    /* Run all tests */
    printf(COLOR_BLUE "\n========================================\n");
    printf("  Running Tests\n");
    printf("========================================\n" COLOR_RESET);

    test_create(&manager);                      /* Test 1 */
    test_get_by_charac_no(&manager);            /* Test 2 */
    test_exists_true(&manager);                 /* Test 3 */
    test_update_exp(&manager);                  /* Test 4 */
    test_update_fatigue(&manager);              /* Test 5 */
    test_update_last_play_time(&manager);       /* Test 6 */
    test_increment_dungeon_play_count(&manager);/* Test 7 */
    test_update_chaos_stats(&manager);          /* Test 8 */
    test_update(&manager);                      /* Test 9 */
    test_get_by_exp_range(&manager);            /* Test 10 */
    test_get_total_count(&manager);             /* Test 11 */
    test_delete(&manager);                      /* Test 12 */
    test_exists_false(&manager);                /* Test 13 */

    /* Cleanup after tests */
    cleanup_after_tests(&manager);

    /* Print results */
    printf(COLOR_BLUE "\n========================================\n");
    printf("  Test Results\n");
    printf("========================================\n" COLOR_RESET);
    printf("Total:  %d\n", tests_passed + tests_failed);
    printf(COLOR_GREEN "Passed: %d\n" COLOR_RESET, tests_passed);
    printf(COLOR_RED "Failed: %d\n" COLOR_RESET, tests_failed);
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);

    if (tests_failed == 0) {
        printf(COLOR_GREEN "  ✓ All tests passed!\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "  ✗ Some tests failed\n" COLOR_RESET);
    }
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);

    /* Cleanup */
    DBConnectionManager_Cleanup(&manager);

    return (tests_failed == 0) ? 0 : 1;
}
