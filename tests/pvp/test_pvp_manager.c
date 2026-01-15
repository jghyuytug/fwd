/**
 * @file test_pvp_manager.c
 * @brief PvP Manager Test Suite
 *
 * Phase: 6.9 - PvP System Implementation
 * Date: 2025-11-25
 *
 * Tests:
 *   1. Module initialization and cleanup
 *   2. Match creation and destruction
 *   3. Player join and leave
 *   4. Match lifecycle (start, end)
 *   5. ELO rating system
 *   6. Ranking and tiers
 *   7. Statistics tracking
 *   8. Matchmaking
 *   9. Season management
 */

#include "../../include/pvp/pvp_interface.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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
    int result = PvP_Initialize(-1, 1000);
    TEST_CHECK(result != ERR_SUCCESS, "Reject negative max_matches");

    result = PvP_Initialize(100, -1);
    TEST_CHECK(result != ERR_SUCCESS, "Reject negative max_players");

    /* Test valid initialization */
    result = PvP_Initialize(100, 5000);
    TEST_CHECK(result == ERR_SUCCESS, "Initialize with valid params");

    /* Test double initialization */
    result = PvP_Initialize(100, 5000);
    TEST_CHECK(result == ERR_INVALID_STATE, "Reject double initialization");

    /* Cleanup */
    PvP_Cleanup();
    TEST_CHECK(1, "Cleanup completed");

    /* Test re-initialization */
    result = PvP_Initialize(50, 2500);
    TEST_CHECK(result == ERR_SUCCESS, "Re-initialize after cleanup");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 2: MATCH CREATION
 * ======================================================================== */

static void test_match_creation(void) {
    TEST_BEGIN("Match Creation");

    PvP_Initialize(100, 5000);

    /* Create 1v1 match */
    int match_id = PvP_CreateMatch(PVP_TYPE_1V1, ARENA_TYPE_DUEL_RING, 0);
    TEST_CHECK(match_id > 0, "Create 1v1 match");

    /* Get match info */
    PvPMatchInfo info;
    int result = PvP_GetMatchInfo(match_id, &info);
    TEST_CHECK(result == ERR_SUCCESS, "Get match info");
    TEST_CHECK(info.match_type == PVP_TYPE_1V1, "Match type is 1V1");
    TEST_CHECK(info.arena_type == ARENA_TYPE_DUEL_RING, "Arena type correct");
    TEST_CHECK(info.match_state == PVP_STATE_WAITING, "State is WAITING");
    TEST_CHECK(info.max_participants == 2, "Max participants = 2");

    /* Create 3v3 match */
    int match_3v3 = PvP_CreateMatch(PVP_TYPE_3V3, ARENA_TYPE_TEAM_ARENA, 1);
    TEST_CHECK(match_3v3 > 0, "Create 3v3 ranked match");

    PvPMatchInfo info_3v3;
    PvP_GetMatchInfo(match_3v3, &info_3v3);
    TEST_CHECK(info_3v3.max_participants == 6, "3v3 max participants = 6");

    /* Non-existent match */
    result = PvP_GetMatchInfo(99999, &info);
    TEST_CHECK(result == ERR_NOT_FOUND, "Non-existent match not found");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 3: PLAYER JOIN AND LEAVE
 * ======================================================================== */

static void test_player_management(void) {
    TEST_BEGIN("Player Join and Leave");

    PvP_Initialize(100, 5000);

    int match_id = PvP_CreateMatch(PVP_TYPE_1V1, ARENA_TYPE_DUEL_RING, 0);
    TEST_CHECK(match_id > 0, "Create match");

    /* First player joins */
    int result = PvP_JoinMatch(match_id, 100, 1001, 0);
    TEST_CHECK(result == ERR_SUCCESS, "Player 1 joins team 0");

    /* Second player joins */
    result = PvP_JoinMatch(match_id, 101, 1002, 1);
    TEST_CHECK(result == ERR_SUCCESS, "Player 2 joins team 1");

    /* Check match state */
    PvPMatchInfo info;
    PvP_GetMatchInfo(match_id, &info);
    TEST_CHECK(info.participant_count == 2, "2 participants");
    TEST_CHECK(info.match_state == PVP_STATE_READY, "State changed to READY");

    /* Cannot join full match */
    result = PvP_JoinMatch(match_id, 102, 1003, 0);
    TEST_CHECK(result == ERR_LIMIT_EXCEEDED, "Cannot join full match");

    /* Duplicate join */
    result = PvP_JoinMatch(match_id, 100, 1001, 0);
    TEST_CHECK(result == ERR_DUPLICATE, "Cannot join twice");

    /* Leave match */
    result = PvP_LeaveMatch(match_id, 100);
    TEST_CHECK(result == ERR_SUCCESS, "Player leaves");

    PvP_GetMatchInfo(match_id, &info);
    TEST_CHECK(info.participant_count == 1, "1 participant after leave");

    /* Cannot leave twice */
    result = PvP_LeaveMatch(match_id, 100);
    TEST_CHECK(result == ERR_NOT_FOUND, "Cannot leave twice");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 4: MATCH LIFECYCLE
 * ======================================================================== */

static void test_match_lifecycle(void) {
    TEST_BEGIN("Match Lifecycle");

    PvP_Initialize(100, 5000);

    int match_id = PvP_CreateMatch(PVP_TYPE_1V1, ARENA_TYPE_DUEL_RING, 0);
    PvP_JoinMatch(match_id, 100, 1001, 0);
    PvP_JoinMatch(match_id, 101, 1002, 1);

    /* Start match */
    int result = PvP_StartMatch(match_id);
    TEST_CHECK(result == ERR_SUCCESS, "Start match");

    PvPMatchInfo info;
    PvP_GetMatchInfo(match_id, &info);
    TEST_CHECK(info.match_state == PVP_STATE_IN_PROGRESS, "State is IN_PROGRESS");
    TEST_CHECK(info.start_time > 0, "Start time set");

    /* Cannot start twice */
    result = PvP_StartMatch(match_id);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot start twice");

    /* End match */
    result = PvP_EndMatch(match_id, 0);  /* Team 0 wins */
    TEST_CHECK(result == ERR_SUCCESS, "End match");

    PvP_GetMatchInfo(match_id, &info);
    TEST_CHECK(info.match_state == PVP_STATE_FINISHED, "State is FINISHED");
    TEST_CHECK(info.winning_team == 0, "Team 0 won");
    TEST_CHECK(info.end_time > 0, "End time set");

    /* Check winners marked */
    TEST_CHECK(info.participants[0].is_winner == 1, "Player 1 is winner");
    TEST_CHECK(info.participants[1].is_winner == 0, "Player 2 is loser");

    /* Cannot end twice */
    result = PvP_EndMatch(match_id, 1);
    TEST_CHECK(result == ERR_INVALID_STATE, "Cannot end twice");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 5: ELO RATING SYSTEM
 * ======================================================================== */

static void test_elo_rating(void) {
    TEST_BEGIN("ELO Rating System");

    PvP_Initialize(100, 5000);

    /* Initial rating should be 1200 */
    PvPRankingInfo ranking;
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_points == 1200, "Initial rating = 1200");
    TEST_CHECK(ranking.rank_tier == RANK_TIER_GOLD, "Initial tier = GOLD");

    /* Create ranked match */
    int match_id = PvP_CreateMatch(PVP_TYPE_RANKED, ARENA_TYPE_COLOSSEUM, 1);
    PvP_JoinMatch(match_id, 100, 1001, 0);
    PvP_JoinMatch(match_id, 101, 1002, 1);
    PvP_StartMatch(match_id);
    PvP_EndMatch(match_id, 0);  /* Player 100 wins */

    /* Check rating changed */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_points > 1200, "Winner rating increased");
    TEST_CHECK(ranking.wins == 1, "Winner has 1 win");

    PvPRankingInfo loser_ranking;
    PvP_GetPlayerRanking(101, 0, &loser_ranking);
    TEST_CHECK(loser_ranking.rank_points < 1200, "Loser rating decreased");
    TEST_CHECK(loser_ranking.losses == 1, "Loser has 1 loss");

    /* Win streak tracking */
    TEST_CHECK(ranking.win_streak == 1, "Win streak = 1");

    /* Another win */
    int match2 = PvP_CreateMatch(PVP_TYPE_RANKED, ARENA_TYPE_COLOSSEUM, 1);
    PvP_JoinMatch(match2, 100, 1001, 0);
    PvP_JoinMatch(match2, 102, 1003, 1);
    PvP_StartMatch(match2);
    PvP_EndMatch(match2, 0);

    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.wins == 2, "2 wins total");
    TEST_CHECK(ranking.win_streak == 2, "Win streak = 2");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 6: RANKING TIERS
 * ======================================================================== */

static void test_ranking_tiers(void) {
    TEST_BEGIN("Ranking Tiers");

    PvP_Initialize(100, 5000);

    /* Test tier calculation via rating updates */
    int new_rating;

    /* Bronze: 0-799 */
    new_rating = PvP_UpdatePlayerRating(100, -600, 0);  /* 1200 - 600 = 600 */
    PvPRankingInfo ranking;
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_BRONZE, "Bronze tier at 600");

    /* Silver: 800-1199 */
    new_rating = PvP_UpdatePlayerRating(100, 300, 1);  /* 600 + 300 = 900 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_SILVER, "Silver tier at 900");

    /* Gold: 1200-1599 */
    new_rating = PvP_UpdatePlayerRating(100, 400, 1);  /* 900 + 400 = 1300 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_GOLD, "Gold tier at 1300");

    /* Platinum: 1600-1999 */
    new_rating = PvP_UpdatePlayerRating(100, 400, 1);  /* 1300 + 400 = 1700 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_PLATINUM, "Platinum tier at 1700");

    /* Diamond: 2000-2399 */
    new_rating = PvP_UpdatePlayerRating(100, 400, 1);  /* 1700 + 400 = 2100 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_DIAMOND, "Diamond tier at 2100");

    /* Master: 2400-2799 */
    new_rating = PvP_UpdatePlayerRating(100, 400, 1);  /* 2100 + 400 = 2500 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_MASTER, "Master tier at 2500");

    /* Grandmaster: 2800+ */
    new_rating = PvP_UpdatePlayerRating(100, 400, 1);  /* 2500 + 400 = 2900 */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_tier == RANK_TIER_GRANDMASTER, "Grandmaster tier at 2900");

    /* Rating cap at 3000 */
    new_rating = PvP_UpdatePlayerRating(100, 500, 1);
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.rank_points <= 3000, "Rating capped at 3000");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 7: STATISTICS TRACKING
 * ======================================================================== */

static void test_statistics(void) {
    TEST_BEGIN("Statistics Tracking");

    PvP_Initialize(100, 5000);

    /* Initial stats */
    PvPStats stats;
    PvP_GetStatistics(100, &stats);
    TEST_CHECK(stats.total_matches_played == 0, "Initial matches = 0");
    TEST_CHECK(stats.total_wins == 0, "Initial wins = 0");

    /* Play some matches */
    for (int i = 0; i < 5; i++) {
        int match = PvP_CreateMatch(PVP_TYPE_RANKED, ARENA_TYPE_COLOSSEUM, 1);
        PvP_JoinMatch(match, 100, 1001, 0);
        PvP_JoinMatch(match, 200 + i, 2001 + i, 1);
        PvP_StartMatch(match);
        PvP_EndMatch(match, (i < 3) ? 0 : 1);  /* Win first 3, lose 2 */
    }

    PvP_GetStatistics(100, &stats);
    TEST_CHECK(stats.total_matches_played == 5, "5 matches played");
    TEST_CHECK(stats.total_wins == 3, "3 wins");
    TEST_CHECK(stats.total_losses == 2, "2 losses");
    TEST_CHECK(stats.win_rate > 0.59f && stats.win_rate < 0.61f, "Win rate = 60%");
    TEST_CHECK(stats.highest_win_streak == 3, "Highest streak = 3");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 8: MATCHMAKING
 * ======================================================================== */

static void test_matchmaking(void) {
    TEST_BEGIN("Matchmaking");

    PvP_Initialize(100, 5000);

    /* Find match for player - should create new */
    int match1 = PvP_FindMatch(100, PVP_TYPE_1V1, 0);
    TEST_CHECK(match1 > 0, "Find match creates new match");

    /* Join the match */
    PvP_JoinMatch(match1, 100, 1001, 0);

    /* Another player finds match - should join existing */
    int match2 = PvP_FindMatch(101, PVP_TYPE_1V1, 0);
    TEST_CHECK(match2 == match1, "Second player joins existing match");

    /* Different match type - creates new */
    int match3 = PvP_FindMatch(102, PVP_TYPE_3V3, 0);
    TEST_CHECK(match3 != match1, "Different type creates new match");

    /* Ranked vs unranked - creates new */
    int match4 = PvP_FindMatch(103, PVP_TYPE_1V1, 1);
    TEST_CHECK(match4 != match1, "Ranked creates separate match");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 9: SEASON MANAGEMENT
 * ======================================================================== */

static void test_season_management(void) {
    TEST_BEGIN("Season Management");

    PvP_Initialize(100, 5000);

    /* Initial season */
    PvPRankingInfo ranking;
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.season_id == 1, "Initial season = 1");

    /* Play matches to change rating */
    for (int i = 0; i < 3; i++) {
        int match = PvP_CreateMatch(PVP_TYPE_RANKED, ARENA_TYPE_COLOSSEUM, 1);
        PvP_JoinMatch(match, 100, 1001, 0);
        PvP_JoinMatch(match, 200, 2001, 1);
        PvP_StartMatch(match);
        PvP_EndMatch(match, 0);  /* Player 100 wins */
    }

    PvP_GetPlayerRanking(100, 0, &ranking);
    int pre_reset_rating = ranking.rank_points;
    int pre_reset_wins = ranking.wins;
    TEST_CHECK(pre_reset_rating > 1200, "Rating increased before reset");
    TEST_CHECK(pre_reset_wins == 3, "3 wins before reset");

    /* Reset season */
    int reset_count = PvP_ResetSeasonRankings();
    TEST_CHECK(reset_count >= 1, "At least 1 player reset");

    /* Check soft reset */
    PvP_GetPlayerRanking(100, 0, &ranking);
    TEST_CHECK(ranking.season_id == 2, "Season = 2 after reset");
    TEST_CHECK(ranking.wins == 0, "Wins reset to 0");
    TEST_CHECK(ranking.losses == 0, "Losses reset to 0");
    /* Rating should decay towards 1200 but not fully reset */
    TEST_CHECK(ranking.rank_points < pre_reset_rating, "Rating decayed");
    TEST_CHECK(ranking.rank_points > 1200, "Rating above default (soft reset)");

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 10: TOP RANKINGS
 * ======================================================================== */

static void test_top_rankings(void) {
    TEST_BEGIN("Top Rankings");

    PvP_Initialize(100, 5000);

    /* Create several players with different ratings */
    PvP_UpdatePlayerRating(100, 800, 1);   /* 2000 */
    PvP_UpdatePlayerRating(101, 600, 1);   /* 1800 */
    PvP_UpdatePlayerRating(102, 400, 1);   /* 1600 */
    PvP_UpdatePlayerRating(103, 200, 1);   /* 1400 */
    PvP_UpdatePlayerRating(104, 0, 0);     /* 1200 */

    /* Get top 3 */
    PvPRankingInfo top[3];
    int count = PvP_GetTopRankings(top, 3, 0);
    TEST_CHECK(count == 3, "Got 3 rankings");

    /* Check order (highest first) */
    if (count >= 3) {
        TEST_CHECK(top[0].rank_points >= top[1].rank_points, "Rank 1 >= Rank 2");
        TEST_CHECK(top[1].rank_points >= top[2].rank_points, "Rank 2 >= Rank 3");
        TEST_CHECK(top[0].rank_position == 1, "First has position 1");
        TEST_CHECK(top[1].rank_position == 2, "Second has position 2");
        TEST_CHECK(top[2].rank_position == 3, "Third has position 3");
    }

    PvP_Cleanup();
}

/* ========================================================================
 * TEST 11: AUTO-TEAM ASSIGNMENT
 * ======================================================================== */

static void test_auto_team(void) {
    TEST_BEGIN("Auto Team Assignment");

    PvP_Initialize(100, 5000);

    int match_id = PvP_CreateMatch(PVP_TYPE_3V3, ARENA_TYPE_TEAM_ARENA, 0);

    /* Join with auto-team assignment (-1) */
    PvP_JoinMatch(match_id, 100, 1001, -1);
    PvP_JoinMatch(match_id, 101, 1002, -1);
    PvP_JoinMatch(match_id, 102, 1003, -1);
    PvP_JoinMatch(match_id, 103, 1004, -1);

    /* Check teams are balanced */
    PvPMatchInfo info;
    PvP_GetMatchInfo(match_id, &info);

    int team0_count = 0, team1_count = 0;
    for (int i = 0; i < info.participant_count; i++) {
        if (info.participants[i].team_id == 0) team0_count++;
        else team1_count++;
    }

    TEST_CHECK(team0_count == 2, "Team 0 has 2 players");
    TEST_CHECK(team1_count == 2, "Team 1 has 2 players");

    PvP_Cleanup();
}

/* ========================================================================
 * MAIN
 * ======================================================================== */

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("========================================\n");
    printf("PvP Manager Test Suite\n");
    printf("Phase: 6.9 - PvP System\n");
    printf("========================================\n");

    /* Run all tests */
    test_initialization();
    test_match_creation();
    test_player_management();
    test_match_lifecycle();
    test_elo_rating();
    test_ranking_tiers();
    test_statistics();
    test_matchmaking();
    test_season_management();
    test_top_rankings();
    test_auto_team();

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
