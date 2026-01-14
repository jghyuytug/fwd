/**
 * @file test_pvp_result_orm.c
 * @brief Test suite for PVP Result ORM module
 *
 * Database: test_taiwan_cain (DB_TYPE_CAIN = 2)
 * Comprehensive tests for PVP statistics management
 */

#include "pvp_result_orm.h"
#include "db_connection_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ANSI color codes */
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

/* Print macros */
#define print_test(msg) printf(COLOR_BLUE "[TEST] %s" COLOR_RESET "\n", msg)
#define print_success(msg) printf(COLOR_GREEN "[PASS] %s" COLOR_RESET "\n", msg)
#define print_error(msg) printf(COLOR_RED "[FAIL] %s" COLOR_RESET "\n", msg)
#define print_info(msg) printf(COLOR_YELLOW "[INFO] %s" COLOR_RESET "\n", msg)

/* Helper function to print PVP result */
void print_pvp_result(const char* label, PvpResult* result) {
    printf("%s:\n", label);
    printf("  CharacNo: %d\n", result->charac_no);
    printf("  Win/Lose: %d/%d (%.1f%% win rate)\n",
           result->win, result->lose,
           (result->win + result->lose > 0) ?
           (result->win * 100.0 / (result->win + result->lose)) : 0.0);
    printf("  PVP Point: %d, Grade: %d.%u\n",
           result->pvp_point, result->pvp_grade, result->pvp_grade_ext);
    printf("  Avg Kill: %d, Combo: %d, Damage: %d\n",
           result->avg_kill_count, result->avg_combo_count, result->avg_deal_damage);
    printf("  PVP Count: %d, Win Point: %d\n",
           result->pvp_count, result->win_point);
    printf("  Play Count: %u, Play Time: %u seconds\n",
           result->play_count, result->play_time);
    printf("  Last Play: %s\n", result->last_play_time);
}

/* Test 1: Create PVP results */
int test_create_results(DBConnectionManager* manager) {
    PvpResult result1, result2, result3;
    int ret;

    print_test("Test 1: Create PVP Results");

    /* Initialize test data 1 - Strong player */
    memset(&result1, 0, sizeof(PvpResult));
    result1.charac_no = 10001;
    result1.win = 50;
    result1.lose = 10;
    result1.pvp_point = 2500;
    result1.pvp_grade = 5;
    result1.pvp_grade_ext = 3;
    result1.avg_kill_count = 15;
    result1.avg_combo_count = 120;
    result1.avg_deal_damage = 50000;
    result1.pvp_count = 60;
    result1.win_point = 1500;
    result1.play_count = 100;
    result1.play_time = 36000;
    strcpy(result1.last_play_time, "2025-01-13 12:00:00");
    strcpy(result1.pvp_grade_ext_update_time, "2025-01-13 12:00:00");

    /* Initialize test data 2 - Average player */
    memset(&result2, 0, sizeof(PvpResult));
    result2.charac_no = 10002;
    result2.win = 25;
    result2.lose = 25;
    result2.pvp_point = 1500;
    result2.pvp_grade = 3;
    result2.pvp_grade_ext = 0;
    result2.avg_kill_count = 8;
    result2.avg_combo_count = 80;
    result2.avg_deal_damage = 30000;
    result2.pvp_count = 50;
    result2.win_point = 750;
    result2.play_count = 60;
    result2.play_time = 18000;
    strcpy(result2.last_play_time, "2025-01-12 18:00:00");
    strcpy(result2.pvp_grade_ext_update_time, "2025-01-12 18:00:00");

    /* Initialize test data 3 - Beginner player */
    memset(&result3, 0, sizeof(PvpResult));
    result3.charac_no = 10003;
    result3.win = 5;
    result3.lose = 15;
    result3.pvp_point = 500;
    result3.pvp_grade = 1;
    result3.pvp_grade_ext = 2;
    result3.avg_kill_count = 5;
    result3.avg_combo_count = 50;
    result3.avg_deal_damage = 20000;
    result3.pvp_count = 20;
    result3.win_point = 150;
    result3.play_count = 25;
    result3.play_time = 7200;
    strcpy(result3.last_play_time, "2025-01-10 10:00:00");
    strcpy(result3.pvp_grade_ext_update_time, "2025-01-10 10:00:00");

    /* Create records */
    ret = PvpResult_Create(manager, &result1);
    if (ret != 0) {
        print_error("Failed to create result1");
        return 0;
    }

    ret = PvpResult_Create(manager, &result2);
    if (ret != 0) {
        print_error("Failed to create result2");
        return 0;
    }

    ret = PvpResult_Create(manager, &result3);
    if (ret != 0) {
        print_error("Failed to create result3");
        return 0;
    }

    print_success("Created 3 PVP result records");
    return 1;
}

/* Test 2: Query PVP results */
int test_query_results(DBConnectionManager* manager) {
    PvpResult result;
    int ret;

    print_test("Test 2: Query PVP Results");

    ret = PvpResult_GetByCharacNo(manager, 10001, &result);
    if (ret != 0) {
        print_error("Failed to query result for charac_no=10001");
        return 0;
    }

    print_pvp_result("Result for charac_no=10001", &result);

    if (result.win != 50 || result.lose != 10 || result.pvp_point != 2500) {
        print_error("Data mismatch for charac_no=10001");
        return 0;
    }

    print_success("Query results match expected data");
    return 1;
}

/* Test 3: Update PVP result */
int test_update_result(DBConnectionManager* manager) {
    PvpResult result;
    int ret;

    print_test("Test 3: Update PVP Result");

    /* Get existing result */
    ret = PvpResult_GetByCharacNo(manager, 10002, &result);
    if (ret != 0) {
        print_error("Failed to get result for update");
        return 0;
    }

    /* Update statistics */
    result.win = 30;
    result.lose = 25;
    result.pvp_point = 1700;
    result.avg_kill_count = 10;
    result.avg_combo_count = 90;

    ret = PvpResult_Update(manager, &result);
    if (ret != 0) {
        print_error("Failed to update result");
        return 0;
    }

    /* Verify update */
    ret = PvpResult_GetByCharacNo(manager, 10002, &result);
    if (ret != 0 || result.win != 30 || result.pvp_point != 1700) {
        print_error("Update verification failed");
        return 0;
    }

    print_success("PVP result updated successfully");
    return 1;
}

/* Test 4: Existence check */
int test_existence_check(DBConnectionManager* manager) {
    int ret;

    print_test("Test 4: Existence Check");

    ret = PvpResult_Exists(manager, 10001);
    if (ret != 1) {
        print_error("Existing record not found");
        return 0;
    }

    ret = PvpResult_Exists(manager, 99999);
    if (ret != 0) {
        print_error("Non-existing record found");
        return 0;
    }

    print_success("Existence checks passed");
    return 1;
}

/* Test 5: Record match result - Win */
int test_record_match_win(DBConnectionManager* manager) {
    PvpResult before, after;
    int ret;

    print_test("Test 5: Record Match Result (Win)");

    /* Get state before */
    ret = PvpResult_GetByCharacNo(manager, 10003, &before);
    if (ret != 0) {
        print_error("Failed to get before state");
        return 0;
    }

    /* Record a win with 100 points */
    ret = PvpResult_RecordMatch(manager, 10003, 1, 100);
    if (ret != 0) {
        print_error("Failed to record match win");
        return 0;
    }

    /* Get state after */
    ret = PvpResult_GetByCharacNo(manager, 10003, &after);
    if (ret != 0) {
        print_error("Failed to get after state");
        return 0;
    }

    /* Verify changes */
    if (after.win != before.win + 1 ||
        after.pvp_point != before.pvp_point + 100 ||
        after.win_point != before.win_point + 100 ||
        after.pvp_count != before.pvp_count + 1) {
        print_error("Match win statistics incorrect");
        printf("  Before: Win=%d, Point=%d, WinPoint=%d, Count=%d\n",
               before.win, before.pvp_point, before.win_point, before.pvp_count);
        printf("  After:  Win=%d, Point=%d, WinPoint=%d, Count=%d\n",
               after.win, after.pvp_point, after.win_point, after.pvp_count);
        return 0;
    }

    print_success("Match win recorded correctly");
    return 1;
}

/* Test 6: Record match result - Loss */
int test_record_match_loss(DBConnectionManager* manager) {
    PvpResult before, after;
    int ret;

    print_test("Test 6: Record Match Result (Loss)");

    /* Get state before */
    ret = PvpResult_GetByCharacNo(manager, 10003, &before);
    if (ret != 0) {
        print_error("Failed to get before state");
        return 0;
    }

    /* Record a loss with 50 points penalty */
    ret = PvpResult_RecordMatch(manager, 10003, 0, 50);
    if (ret != 0) {
        print_error("Failed to record match loss");
        return 0;
    }

    /* Get state after */
    ret = PvpResult_GetByCharacNo(manager, 10003, &after);
    if (ret != 0) {
        print_error("Failed to get after state");
        return 0;
    }

    /* Verify changes */
    if (after.lose != before.lose + 1 ||
        after.pvp_point != before.pvp_point - 50 ||
        after.pvp_count != before.pvp_count + 1) {
        print_error("Match loss statistics incorrect");
        return 0;
    }

    print_success("Match loss recorded correctly");
    return 1;
}

/* Test 7: Update PVP grade */
int test_update_grade(DBConnectionManager* manager) {
    PvpResult result;
    int ret;

    print_test("Test 7: Update PVP Grade");

    /* Update grade */
    ret = PvpResult_UpdateGrade(manager, 10001, 6, 1);
    if (ret != 0) {
        print_error("Failed to update grade");
        return 0;
    }

    /* Verify */
    ret = PvpResult_GetByCharacNo(manager, 10001, &result);
    if (ret != 0 || result.pvp_grade != 6 || result.pvp_grade_ext != 1) {
        print_error("Grade update verification failed");
        return 0;
    }

    print_success("PVP grade updated successfully");
    return 1;
}

/* Test 8: Update average statistics */
int test_update_averages(DBConnectionManager* manager) {
    PvpResult result;
    int ret;

    print_test("Test 8: Update Average Statistics");

    /* Get current result */
    ret = PvpResult_GetByCharacNo(manager, 10001, &result);
    if (ret != 0) {
        print_error("Failed to get result");
        return 0;
    }

    /* Update averages */
    result.avg_kill_count = 20;
    result.avg_combo_count = 150;
    result.avg_deal_damage = 60000;
    result.avg_technic = 95;
    result.avg_style = 90;

    ret = PvpResult_UpdateAverages(manager, 10001, &result);
    if (ret != 0) {
        print_error("Failed to update averages");
        return 0;
    }

    /* Verify */
    ret = PvpResult_GetByCharacNo(manager, 10001, &result);
    if (ret != 0 || result.avg_kill_count != 20 ||
        result.avg_combo_count != 150 || result.avg_deal_damage != 60000) {
        print_error("Average update verification failed");
        return 0;
    }

    print_success("Average statistics updated successfully");
    return 1;
}

/* Test 9: Get top players by points */
int test_get_top_by_points(DBConnectionManager* manager) {
    PvpResult results[10];
    int ret;

    print_test("Test 9: Get Top Players by Points");

    ret = PvpResult_GetTopByPoints(manager, results, 10);
    if (ret < 0) {
        print_error("Failed to get top players");
        return 0;
    }

    printf("  Found %d top players\n", ret);
    if (ret > 0) {
        print_pvp_result("  Rank 1", &results[0]);
        /* First should be charac_no=10001 with highest points */
        if (results[0].charac_no != 10001) {
            print_error("Top player ranking incorrect");
            return 0;
        }
    }

    print_success("Top players by points query successful");
    return 1;
}

/* Test 10: Get top players by win rate */
int test_get_top_by_win_rate(DBConnectionManager* manager) {
    PvpResult results[10];
    int ret;

    print_test("Test 10: Get Top Players by Win Rate");

    ret = PvpResult_GetTopByWinRate(manager, 10, results, 10);
    if (ret < 0) {
        print_error("Failed to get top by win rate");
        return 0;
    }

    printf("  Found %d players with 10+ matches\n", ret);
    if (ret > 0) {
        float win_rate = (results[0].win * 100.0) / (results[0].win + results[0].lose);
        printf("  Top win rate: %.1f%% (CharacNo=%d)\n", win_rate, results[0].charac_no);
    }

    print_success("Top players by win rate query successful");
    return 1;
}

/* Test 11: Get players by grade */
int test_get_by_grade(DBConnectionManager* manager) {
    PvpResult results[10];
    int ret;

    print_test("Test 11: Get Players by Grade");

    ret = PvpResult_GetByGrade(manager, 6, results, 10);
    if (ret < 0) {
        print_error("Failed to get players by grade");
        return 0;
    }

    printf("  Found %d players at grade 6\n", ret);
    if (ret > 0) {
        /* Should find charac_no=10001 which we updated to grade 6 */
        if (results[0].pvp_grade != 6) {
            print_error("Grade filter incorrect");
            return 0;
        }
    }

    print_success("Get players by grade successful");
    return 1;
}

/* Test 12: Get total count */
int test_get_total_count(DBConnectionManager* manager) {
    int count;

    print_test("Test 12: Get Total Count");

    count = PvpResult_GetTotalCount(manager);
    if (count < 0) {
        print_error("Failed to get total count");
        return 0;
    }

    printf("  Total PVP records: %d\n", count);
    if (count != 3) {
        print_error("Total count mismatch");
        return 0;
    }

    print_success("Total count query successful");
    return 1;
}

/* Test 13: Update last play time */
int test_update_last_play_time(DBConnectionManager* manager) {
    PvpResult before, after;
    int ret;

    print_test("Test 13: Update Last Play Time");

    /* Get before state */
    ret = PvpResult_GetByCharacNo(manager, 10002, &before);
    if (ret != 0) {
        print_error("Failed to get before state");
        return 0;
    }

    /* Sleep 1 second to ensure time difference */
    sleep(1);

    /* Update last play time */
    ret = PvpResult_UpdateLastPlayTime(manager, 10002);
    if (ret != 0) {
        print_error("Failed to update last play time");
        return 0;
    }

    /* Get after state */
    ret = PvpResult_GetByCharacNo(manager, 10002, &after);
    if (ret != 0) {
        print_error("Failed to get after state");
        return 0;
    }

    /* Verify time changed */
    if (strcmp(before.last_play_time, after.last_play_time) == 0) {
        print_error("Last play time not updated");
        return 0;
    }

    printf("  Before: %s\n", before.last_play_time);
    printf("  After:  %s\n", after.last_play_time);

    print_success("Last play time updated successfully");
    return 1;
}

/* Test 14: Increment play count */
int test_increment_play_count(DBConnectionManager* manager) {
    PvpResult before, after;
    int ret;

    print_test("Test 14: Increment Play Count");

    /* Get before state */
    ret = PvpResult_GetByCharacNo(manager, 10001, &before);
    if (ret != 0) {
        print_error("Failed to get before state");
        return 0;
    }

    /* Increment play count with 300 seconds */
    ret = PvpResult_IncrementPlayCount(manager, 10001, 300);
    if (ret != 0) {
        print_error("Failed to increment play count");
        return 0;
    }

    /* Get after state */
    ret = PvpResult_GetByCharacNo(manager, 10001, &after);
    if (ret != 0) {
        print_error("Failed to get after state");
        return 0;
    }

    /* Verify */
    if (after.play_count != before.play_count + 1 ||
        after.play_time != before.play_time + 300) {
        print_error("Play count/time increment incorrect");
        printf("  Before: Count=%u, Time=%u\n", before.play_count, before.play_time);
        printf("  After:  Count=%u, Time=%u\n", after.play_count, after.play_time);
        return 0;
    }

    print_success("Play count incremented successfully");
    return 1;
}

/* Test 15: Get high win rate players */
int test_get_high_win_rate(DBConnectionManager* manager) {
    PvpResult results[10];
    int ret;

    print_test("Test 15: Get High Win Rate Players");

    /* Get players with >= 70% win rate and >= 10 matches */
    ret = PvpResult_GetHighWinRate(manager, 70, 10, results, 10);
    if (ret < 0) {
        print_error("Failed to get high win rate players");
        return 0;
    }

    printf("  Found %d players with >=70%% win rate (10+ matches)\n", ret);
    for (int i = 0; i < ret; i++) {
        float win_rate = (results[i].win * 100.0) / (results[i].win + results[i].lose);
        printf("    CharacNo=%d: %.1f%% (%d/%d)\n",
               results[i].charac_no, win_rate, results[i].win,
               results[i].win + results[i].lose);
    }

    print_success("High win rate query successful");
    return 1;
}

/* Test 16: Get recently active players */
int test_get_recently_active(DBConnectionManager* manager) {
    PvpResult results[10];
    int ret;

    print_test("Test 16: Get Recently Active Players");

    /* Get players active in last 7 days */
    ret = PvpResult_GetRecentlyActive(manager, 7, results, 10);
    if (ret < 0) {
        print_error("Failed to get recently active players");
        return 0;
    }

    printf("  Found %d players active in last 7 days\n", ret);
    for (int i = 0; i < ret && i < 3; i++) {
        printf("    CharacNo=%d: Last play %s\n",
               results[i].charac_no, results[i].last_play_time);
    }

    print_success("Recently active players query successful");
    return 1;
}

/* Test 17: Query non-existing result */
int test_query_nonexisting(DBConnectionManager* manager) {
    PvpResult result;
    int ret;

    print_test("Test 17: Query Non-Existing Result");

    ret = PvpResult_GetByCharacNo(manager, 99999, &result);
    if (ret == 0) {
        print_error("Non-existing result returned success");
        return 0;
    }

    print_success("Non-existing query handled correctly");
    return 1;
}

/* Test 18: Delete PVP result */
int test_delete_result(DBConnectionManager* manager) {
    int ret;

    print_test("Test 18: Delete PVP Result");

    /* Delete charac_no=10003 */
    ret = PvpResult_Delete(manager, 10003);
    if (ret != 0) {
        print_error("Failed to delete result");
        return 0;
    }

    /* Verify deletion */
    ret = PvpResult_Exists(manager, 10003);
    if (ret != 0) {
        print_error("Result still exists after deletion");
        return 0;
    }

    print_success("PVP result deleted successfully");
    return 1;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    const char* config_file;
    int passed = 0;
    int total = 18;

    printf("\n");
    printf("========================================\n");
    printf("  PVP Result ORM Test Suite\n");
    printf("  Database: test_taiwan_cain\n");
    printf("========================================\n\n");

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    config_file = argv[1];
    printf("Config file: %s\n\n", config_file);

    /* Clear structure */
    memset(&manager, 0, sizeof(DBConnectionManager));

    /* Initialize connection manager */
    print_info("Initializing database connection...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("Failed to initialize database connection manager");
        return 1;
    }

    /* Connect to database */
    print_info("Connecting to test_taiwan_cain database...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        print_error("Failed to connect to test database");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    print_info("Database connection established");
    printf("\n");

    /* Clean up old test data */
    {
        DBQueryResult cleanup_result;
        memset(&cleanup_result, 0, sizeof(DBQueryResult));
        DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_CAIN,
                                          "DELETE FROM pvp_result WHERE charac_no IN (10001,10002,10003,20001)", &cleanup_result);
        DBQueryResult_Free(&cleanup_result);
    }

    /* Run tests */
    passed += test_create_results(&manager);
    passed += test_query_results(&manager);
    passed += test_update_result(&manager);
    passed += test_existence_check(&manager);
    passed += test_record_match_win(&manager);
    passed += test_record_match_loss(&manager);
    passed += test_update_grade(&manager);
    passed += test_update_averages(&manager);
    passed += test_get_top_by_points(&manager);
    passed += test_get_top_by_win_rate(&manager);
    passed += test_get_by_grade(&manager);
    passed += test_get_total_count(&manager);
    passed += test_update_last_play_time(&manager);
    passed += test_increment_play_count(&manager);
    passed += test_get_high_win_rate(&manager);
    passed += test_get_recently_active(&manager);
    passed += test_query_nonexisting(&manager);
    passed += test_delete_result(&manager);

    /* Cleanup */
    DBConnectionManager_Cleanup(&manager);

    /* Print results */
    printf("\n");
    printf("========================================\n");
    if (passed == total) {
        printf(COLOR_GREEN "  Test Result: %d/%d Passed\n" COLOR_RESET, passed, total);
        printf("========================================\n\n");
        return 0;
    } else {
        printf(COLOR_RED "  Test Result: %d/%d Passed\n" COLOR_RESET, passed, total);
        printf("========================================\n\n");
        return 1;
    }
}
