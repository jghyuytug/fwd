/**
 * @file pvp_manager.c
 * @brief PvP Match Manager - Full Implementation
 *
 * Phase: 6.9 - PvP System Implementation
 * Date: 2025-11-25
 *
 * Implements:
 * - Match creation and lifecycle management
 * - Player matchmaking with skill-based matching
 * - ELO-based rating system with tiers
 * - Match history and statistics tracking
 * - Seasonal rankings with resets
 */

#include "../../include/pvp/pvp_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

/* ========================================================================
 * CONFIGURATION CONSTANTS
 * ======================================================================== */

#define DEFAULT_MAX_MATCHES         500
#define DEFAULT_MAX_RANKED_PLAYERS  50000
#define MATCH_ID_BASE               20000
#define MAX_MATCH_DURATION_SECS     600     /* 10 minutes max */
#define MIN_MATCH_DURATION_SECS     60      /* 1 minute min */
#define MATCHMAKING_RATING_RANGE    200     /* Initial rating range for matching */
#define MATCHMAKING_EXPAND_STEP     50      /* Expand range per second waiting */

/* ELO Constants */
#define ELO_K_FACTOR               32
#define ELO_DEFAULT_RATING         1200
#define ELO_MIN_RATING             100
#define ELO_MAX_RATING             3000

/* Tier Rating Thresholds */
static const int g_tier_thresholds[] = {
    0,      /* BRONZE:      0-799 */
    800,    /* SILVER:      800-1199 */
    1200,   /* GOLD:        1200-1599 */
    1600,   /* PLATINUM:    1600-1999 */
    2000,   /* DIAMOND:     2000-2399 */
    2400,   /* MASTER:      2400-2799 */
    2800    /* GRANDMASTER: 2800+ */
};

/* Max participants per match type */
static const int g_max_participants[] = {
    2,  /* 1V1 */
    6,  /* 3V3 */
    8,  /* FFA */
    2,  /* RANKED (1v1) */
    2,  /* UNRANKED (1v1) */
    2   /* TOURNAMENT */
};

/* ========================================================================
 * INTERNAL DATA STRUCTURES
 * ======================================================================== */

/**
 * Extended match info with internal tracking
 */
typedef struct {
    PvPMatchInfo base;
    int in_use;
    int is_ranked;
    unsigned int creation_time;
    int ready_count;
} PvPMatchInternal;

/**
 * Player ranking entry
 */
typedef struct {
    unsigned int user_id;
    PvPRankingInfo ranking;
    PvPStats stats;
    int in_use;
    unsigned int last_match_time;
    int current_win_streak;
} PlayerRankingEntry;

/**
 * Matchmaking queue entry
 */
typedef struct {
    unsigned int user_id;
    unsigned int character_id;
    PvPMatchType match_type;
    int is_ranked;
    int rating;
    unsigned int queue_time;
    int in_queue;
} MatchmakingEntry;

/**
 * PvP module state
 */
typedef struct {
    int initialized;
    int max_matches;
    int max_ranked_players;
    int current_season_id;

    /* Storage arrays */
    PvPMatchInternal* matches;
    PlayerRankingEntry* rankings;
    MatchmakingEntry* matchmaking_queue;
    int queue_count;
    int ranking_count;

    /* ID generators */
    unsigned int next_match_id;

    /* Statistics */
    struct {
        unsigned int total_matches_created;
        unsigned int total_matches_completed;
        unsigned int total_matches_cancelled;
        unsigned int total_players_ranked;
        unsigned int total_rating_points_awarded;
        unsigned int season_resets;
    } stats;
} PvPModuleState;

static PvPModuleState g_state = {0};

/* ========================================================================
 * INTERNAL HELPERS
 * ======================================================================== */

static unsigned int get_current_time(void) {
    return (unsigned int)time(NULL);
}

/**
 * Calculate tier from rating
 */
static RankTier calculate_tier(int rating) {
    if (rating >= g_tier_thresholds[6]) return RANK_TIER_GRANDMASTER;
    if (rating >= g_tier_thresholds[5]) return RANK_TIER_MASTER;
    if (rating >= g_tier_thresholds[4]) return RANK_TIER_DIAMOND;
    if (rating >= g_tier_thresholds[3]) return RANK_TIER_PLATINUM;
    if (rating >= g_tier_thresholds[2]) return RANK_TIER_GOLD;
    if (rating >= g_tier_thresholds[1]) return RANK_TIER_SILVER;
    return RANK_TIER_BRONZE;
}

/**
 * Calculate ELO rating change
 * expected = 1 / (1 + 10^((opponent_rating - player_rating) / 400))
 * change = K * (actual - expected)
 */
static int calculate_elo_change(int player_rating, int opponent_rating, int did_win) {
    double expected = 1.0 / (1.0 + pow(10.0, (opponent_rating - player_rating) / 400.0));
    double actual = did_win ? 1.0 : 0.0;
    int change = (int)(ELO_K_FACTOR * (actual - expected));

    /* Minimum change */
    if (did_win && change < 5) change = 5;
    if (!did_win && change > -5) change = -5;

    return change;
}

/**
 * Find free match slot
 */
static int find_free_match_slot(void) {
    for (int i = 0; i < g_state.max_matches; i++) {
        if (!g_state.matches[i].in_use) {
            return i;
        }
    }
    return -1;
}

/**
 * Find match by ID
 */
static PvPMatchInternal* find_match(unsigned int match_id) {
    for (int i = 0; i < g_state.max_matches; i++) {
        if (g_state.matches[i].in_use &&
            g_state.matches[i].base.match_id == match_id) {
            return &g_state.matches[i];
        }
    }
    return NULL;
}

/**
 * Find or create player ranking entry
 */
static PlayerRankingEntry* find_or_create_player(unsigned int user_id) {
    /* Search existing */
    for (int i = 0; i < g_state.ranking_count; i++) {
        if (g_state.rankings[i].in_use &&
            g_state.rankings[i].user_id == user_id) {
            return &g_state.rankings[i];
        }
    }

    /* Create new */
    if (g_state.ranking_count < g_state.max_ranked_players) {
        PlayerRankingEntry* entry = &g_state.rankings[g_state.ranking_count];
        memset(entry, 0, sizeof(PlayerRankingEntry));
        entry->user_id = user_id;
        entry->in_use = 1;
        entry->ranking.user_id = user_id;
        entry->ranking.rank_points = ELO_DEFAULT_RATING;
        entry->ranking.rank_tier = calculate_tier(ELO_DEFAULT_RATING);
        entry->ranking.season_id = g_state.current_season_id;
        g_state.ranking_count++;
        g_state.stats.total_players_ranked++;
        return entry;
    }

    return NULL;
}

/**
 * Find participant in match
 */
static int find_participant_index(PvPMatchInternal* match, unsigned int user_id) {
    for (int i = 0; i < match->base.participant_count; i++) {
        if (match->base.participants[i].user_id == user_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Calculate average team rating
 */
static int calculate_team_rating(PvPMatchInternal* match, int team_id) {
    int total = 0;
    int count = 0;
    for (int i = 0; i < match->base.participant_count; i++) {
        if (match->base.participants[i].team_id == team_id) {
            PlayerRankingEntry* player = find_or_create_player(
                match->base.participants[i].user_id);
            if (player) {
                total += player->ranking.rank_points;
                count++;
            }
        }
    }
    return count > 0 ? total / count : ELO_DEFAULT_RATING;
}

/**
 * Update rankings after match
 */
static void process_match_results(PvPMatchInternal* match) {
    if (!match->is_ranked) {
        return;
    }

    int team0_rating = calculate_team_rating(match, 0);
    int team1_rating = calculate_team_rating(match, 1);

    for (int i = 0; i < match->base.participant_count; i++) {
        PvPParticipant* p = &match->base.participants[i];
        PlayerRankingEntry* player = find_or_create_player(p->user_id);
        if (!player) continue;

        int opponent_rating = (p->team_id == 0) ? team1_rating : team0_rating;
        int did_win = p->is_winner;

        int rating_change = calculate_elo_change(
            player->ranking.rank_points, opponent_rating, did_win);

        /* Apply rating change */
        player->ranking.rank_points += rating_change;
        if (player->ranking.rank_points < ELO_MIN_RATING) {
            player->ranking.rank_points = ELO_MIN_RATING;
        }
        if (player->ranking.rank_points > ELO_MAX_RATING) {
            player->ranking.rank_points = ELO_MAX_RATING;
        }

        /* Update tier */
        player->ranking.rank_tier = calculate_tier(player->ranking.rank_points);

        /* Update win/loss */
        if (did_win) {
            player->ranking.wins++;
            player->current_win_streak++;
            if (player->current_win_streak > (int)player->stats.highest_win_streak) {
                player->stats.highest_win_streak = player->current_win_streak;
            }
        } else {
            player->ranking.losses++;
            player->current_win_streak = 0;
        }
        player->ranking.win_streak = player->current_win_streak;

        /* Update stats */
        player->stats.total_matches_played++;
        if (did_win) player->stats.total_wins++;
        else player->stats.total_losses++;
        player->stats.total_kills += p->kills;
        player->stats.total_deaths += p->deaths;
        player->stats.total_damage_dealt += p->damage_dealt;
        player->stats.total_damage_received += p->damage_received;

        /* Calculate ratios */
        if (player->stats.total_matches_played > 0) {
            player->stats.win_rate = (float)player->stats.total_wins /
                                     player->stats.total_matches_played;
        }
        if (player->stats.total_deaths > 0) {
            player->stats.kd_ratio = (float)player->stats.total_kills /
                                     player->stats.total_deaths;
        }

        player->last_match_time = get_current_time();

        if (rating_change > 0) {
            g_state.stats.total_rating_points_awarded += rating_change;
        }

        printf("[PvP] Player %u: %s, rating %d -> %d (%+d), tier=%d\n",
               p->user_id, did_win ? "WIN" : "LOSS",
               player->ranking.rank_points - rating_change,
               player->ranking.rank_points, rating_change,
               player->ranking.rank_tier);
    }
}

/**
 * Sort rankings by rating (for position calculation)
 */
static int compare_rankings(const void* a, const void* b) {
    const PlayerRankingEntry* pa = (const PlayerRankingEntry*)a;
    const PlayerRankingEntry* pb = (const PlayerRankingEntry*)b;
    return pb->ranking.rank_points - pa->ranking.rank_points;
}

static void update_rank_positions(void) {
    /* Create temp array for sorting */
    PlayerRankingEntry** sorted = malloc(g_state.ranking_count * sizeof(PlayerRankingEntry*));
    if (!sorted) return;

    int count = 0;
    for (int i = 0; i < g_state.ranking_count; i++) {
        if (g_state.rankings[i].in_use) {
            sorted[count++] = &g_state.rankings[i];
        }
    }

    /* Sort by rating descending */
    qsort(sorted, count, sizeof(PlayerRankingEntry*),
          (int (*)(const void*, const void*))compare_rankings);

    /* Assign positions */
    for (int i = 0; i < count; i++) {
        sorted[i]->ranking.rank_position = i + 1;
    }

    free(sorted);
}

/* ========================================================================
 * PUBLIC API IMPLEMENTATION
 * ======================================================================== */

int PvP_Initialize(int max_concurrent_matches, int max_ranked_players) {
    if (g_state.initialized) {
        fprintf(stderr, "[PvP] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_concurrent_matches <= 0) {
        max_concurrent_matches = DEFAULT_MAX_MATCHES;
    }
    if (max_concurrent_matches > 10000) {
        return ERR_INVALID_PARAMETER;
    }

    if (max_ranked_players <= 0) {
        max_ranked_players = DEFAULT_MAX_RANKED_PLAYERS;
    }
    if (max_ranked_players > 100000) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[PvP] Initializing PvP manager...\n");

    /* Allocate matches */
    g_state.matches = (PvPMatchInternal*)calloc(
        max_concurrent_matches, sizeof(PvPMatchInternal));
    if (!g_state.matches) {
        return ERR_MEMORY;
    }

    /* Allocate rankings */
    g_state.rankings = (PlayerRankingEntry*)calloc(
        max_ranked_players, sizeof(PlayerRankingEntry));
    if (!g_state.rankings) {
        free(g_state.matches);
        g_state.matches = NULL;
        return ERR_MEMORY;
    }

    /* Allocate matchmaking queue */
    g_state.matchmaking_queue = (MatchmakingEntry*)calloc(
        max_ranked_players, sizeof(MatchmakingEntry));
    if (!g_state.matchmaking_queue) {
        free(g_state.matches);
        free(g_state.rankings);
        g_state.matches = NULL;
        g_state.rankings = NULL;
        return ERR_MEMORY;
    }

    g_state.max_matches = max_concurrent_matches;
    g_state.max_ranked_players = max_ranked_players;
    g_state.current_season_id = 1;
    g_state.next_match_id = MATCH_ID_BASE;
    g_state.queue_count = 0;
    g_state.ranking_count = 0;
    memset(&g_state.stats, 0, sizeof(g_state.stats));
    g_state.initialized = 1;

    printf("[PvP] Manager initialized. Max matches: %d, Max players: %d, Season: %d\n",
           max_concurrent_matches, max_ranked_players, g_state.current_season_id);

    return ERR_SUCCESS;
}

void PvP_Cleanup(void) {
    if (!g_state.initialized) {
        return;
    }

    printf("[PvP] Cleaning up PvP manager...\n");
    printf("[PvP] Final Statistics:\n");
    printf("  - Matches Created:    %u\n", g_state.stats.total_matches_created);
    printf("  - Matches Completed:  %u\n", g_state.stats.total_matches_completed);
    printf("  - Matches Cancelled:  %u\n", g_state.stats.total_matches_cancelled);
    printf("  - Players Ranked:     %u\n", g_state.stats.total_players_ranked);
    printf("  - Rating Points:      %u\n", g_state.stats.total_rating_points_awarded);
    printf("  - Season Resets:      %u\n", g_state.stats.season_resets);

    if (g_state.matches) {
        free(g_state.matches);
        g_state.matches = NULL;
    }
    if (g_state.rankings) {
        free(g_state.rankings);
        g_state.rankings = NULL;
    }
    if (g_state.matchmaking_queue) {
        free(g_state.matchmaking_queue);
        g_state.matchmaking_queue = NULL;
    }

    memset(&g_state, 0, sizeof(g_state));
    printf("[PvP] Manager cleanup complete\n");
}

int PvP_CreateMatch(PvPMatchType match_type, ArenaType arena_type, int is_ranked) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    int slot = find_free_match_slot();
    if (slot < 0) {
        return ERR_LIMIT_EXCEEDED;
    }

    PvPMatchInternal* match = &g_state.matches[slot];
    memset(match, 0, sizeof(PvPMatchInternal));

    unsigned int match_id = g_state.next_match_id++;

    match->base.match_id = match_id;
    match->base.match_type = match_type;
    match->base.match_state = PVP_STATE_WAITING;
    match->base.arena_type = arena_type;
    match->base.max_participants = g_max_participants[match_type];
    match->base.participant_count = 0;
    match->base.winning_team = -1;
    match->in_use = 1;
    match->is_ranked = is_ranked;
    match->creation_time = get_current_time();
    match->ready_count = 0;

    g_state.stats.total_matches_created++;

    printf("[PvP] Created match %u (type=%d, arena=%d, ranked=%d)\n",
           match_id, match_type, arena_type, is_ranked);

    return (int)match_id;
}

int PvP_JoinMatch(unsigned int match_id, unsigned int user_id,
                  unsigned int character_id, int team_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PvPMatchInternal* match = find_match(match_id);
    if (!match) {
        return ERR_NOT_FOUND;
    }

    if (match->base.match_state != PVP_STATE_WAITING) {
        return ERR_INVALID_STATE;
    }

    if (match->base.participant_count >= match->base.max_participants) {
        return ERR_LIMIT_EXCEEDED;
    }

    /* Check if already in match */
    if (find_participant_index(match, user_id) >= 0) {
        return ERR_DUPLICATE;
    }

    /* Auto-assign team if needed */
    if (team_id < 0) {
        int team0_count = 0, team1_count = 0;
        for (int i = 0; i < match->base.participant_count; i++) {
            if (match->base.participants[i].team_id == 0) team0_count++;
            else team1_count++;
        }
        team_id = (team0_count <= team1_count) ? 0 : 1;
    }

    /* Add participant */
    PvPParticipant* p = &match->base.participants[match->base.participant_count];
    memset(p, 0, sizeof(PvPParticipant));
    p->user_id = user_id;
    p->character_id = character_id;
    p->team_id = team_id;
    p->starting_hp = 10000;  /* Default HP */
    p->final_hp = 10000;

    match->base.participant_count++;

    printf("[PvP] Player %u joined match %u (team %d, %d/%d players)\n",
           user_id, match_id, team_id,
           match->base.participant_count, match->base.max_participants);

    /* Auto-start if full */
    if (match->base.participant_count >= match->base.max_participants) {
        match->base.match_state = PVP_STATE_READY;
        printf("[PvP] Match %u ready to start\n", match_id);
    }

    return ERR_SUCCESS;
}

int PvP_LeaveMatch(unsigned int match_id, unsigned int user_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PvPMatchInternal* match = find_match(match_id);
    if (!match) {
        return ERR_NOT_FOUND;
    }

    int idx = find_participant_index(match, user_id);
    if (idx < 0) {
        return ERR_NOT_FOUND;
    }

    /* If match in progress, count as loss */
    if (match->base.match_state == PVP_STATE_IN_PROGRESS) {
        match->base.participants[idx].is_winner = 0;
        match->base.participants[idx].final_hp = 0;
    }

    /* Remove participant (shift remaining) */
    for (int i = idx; i < match->base.participant_count - 1; i++) {
        match->base.participants[i] = match->base.participants[i + 1];
    }
    match->base.participant_count--;

    printf("[PvP] Player %u left match %u (%d players remaining)\n",
           user_id, match_id, match->base.participant_count);

    /* Cancel match if empty or in waiting */
    if (match->base.participant_count == 0 ||
        (match->base.match_state == PVP_STATE_WAITING &&
         match->base.participant_count < 2)) {
        match->base.match_state = PVP_STATE_CANCELLED;
        g_state.stats.total_matches_cancelled++;
    }

    return ERR_SUCCESS;
}

int PvP_StartMatch(unsigned int match_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PvPMatchInternal* match = find_match(match_id);
    if (!match) {
        return ERR_NOT_FOUND;
    }

    if (match->base.match_state != PVP_STATE_READY &&
        match->base.match_state != PVP_STATE_WAITING) {
        return ERR_INVALID_STATE;
    }

    if (match->base.participant_count < 2) {
        return ERR_INVALID_STATE;
    }

    match->base.match_state = PVP_STATE_IN_PROGRESS;
    match->base.start_time = get_current_time();

    printf("[PvP] Match %u started with %d players\n",
           match_id, match->base.participant_count);

    return ERR_SUCCESS;
}

int PvP_EndMatch(unsigned int match_id, int winning_team) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PvPMatchInternal* match = find_match(match_id);
    if (!match) {
        return ERR_NOT_FOUND;
    }

    if (match->base.match_state != PVP_STATE_IN_PROGRESS) {
        return ERR_INVALID_STATE;
    }

    match->base.match_state = PVP_STATE_FINISHED;
    match->base.end_time = get_current_time();
    match->base.winning_team = winning_team;

    /* Mark winners and losers */
    for (int i = 0; i < match->base.participant_count; i++) {
        match->base.participants[i].is_winner =
            (match->base.participants[i].team_id == winning_team) ? 1 : 0;
    }

    printf("[PvP] Match %u ended. Winning team: %d\n", match_id, winning_team);

    /* Process rankings */
    process_match_results(match);
    update_rank_positions();

    g_state.stats.total_matches_completed++;

    return ERR_SUCCESS;
}

int PvP_GetMatchInfo(unsigned int match_id, PvPMatchInfo* match_info) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!match_info) {
        return ERR_INVALID_PARAMETER;
    }

    PvPMatchInternal* match = find_match(match_id);
    if (!match) {
        return ERR_NOT_FOUND;
    }

    memcpy(match_info, &match->base, sizeof(PvPMatchInfo));

    return ERR_SUCCESS;
}

int PvP_GetPlayerRanking(unsigned int user_id, int season_id,
                         PvPRankingInfo* ranking_info) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!ranking_info) {
        return ERR_INVALID_PARAMETER;
    }

    if (season_id == 0) {
        season_id = g_state.current_season_id;
    }

    PlayerRankingEntry* player = find_or_create_player(user_id);
    if (!player) {
        return ERR_LIMIT_EXCEEDED;
    }

    /* Only return current season data */
    if (player->ranking.season_id == season_id) {
        memcpy(ranking_info, &player->ranking, sizeof(PvPRankingInfo));
    } else {
        /* Historical season - return empty for now */
        memset(ranking_info, 0, sizeof(PvPRankingInfo));
        ranking_info->user_id = user_id;
        ranking_info->season_id = season_id;
        ranking_info->rank_points = ELO_DEFAULT_RATING;
    }

    return ERR_SUCCESS;
}

int PvP_GetTopRankings(PvPRankingInfo* rankings, int max_rankings, int season_id) {
    if (!g_state.initialized) {
        return 0;
    }

    if (!rankings || max_rankings <= 0) {
        return 0;
    }

    if (season_id == 0) {
        season_id = g_state.current_season_id;
    }

    update_rank_positions();

    int count = 0;
    for (int pos = 1; pos <= max_rankings; pos++) {
        for (int i = 0; i < g_state.ranking_count && count < max_rankings; i++) {
            if (g_state.rankings[i].in_use &&
                g_state.rankings[i].ranking.rank_position == pos &&
                g_state.rankings[i].ranking.season_id == season_id) {
                memcpy(&rankings[count], &g_state.rankings[i].ranking,
                       sizeof(PvPRankingInfo));
                count++;
                break;
            }
        }
    }

    return count;
}

int PvP_UpdatePlayerRating(unsigned int user_id, int points_change, int did_win) {
    if (!g_state.initialized) {
        return ELO_DEFAULT_RATING;
    }

    PlayerRankingEntry* player = find_or_create_player(user_id);
    if (!player) {
        return ELO_DEFAULT_RATING;
    }

    player->ranking.rank_points += points_change;

    if (player->ranking.rank_points < ELO_MIN_RATING) {
        player->ranking.rank_points = ELO_MIN_RATING;
    }
    if (player->ranking.rank_points > ELO_MAX_RATING) {
        player->ranking.rank_points = ELO_MAX_RATING;
    }

    player->ranking.rank_tier = calculate_tier(player->ranking.rank_points);

    if (did_win) {
        player->ranking.wins++;
    } else {
        player->ranking.losses++;
    }

    if (points_change > 0) {
        g_state.stats.total_rating_points_awarded += points_change;
    }

    return player->ranking.rank_points;
}

int PvP_GetStatistics(unsigned int user_id, PvPStats* stats) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    PlayerRankingEntry* player = find_or_create_player(user_id);
    if (!player) {
        memset(stats, 0, sizeof(PvPStats));
        return ERR_SUCCESS;
    }

    memcpy(stats, &player->stats, sizeof(PvPStats));

    return ERR_SUCCESS;
}

int PvP_FindMatch(unsigned int user_id, PvPMatchType match_type, int is_ranked) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PlayerRankingEntry* player = find_or_create_player(user_id);
    int player_rating = player ? player->ranking.rank_points : ELO_DEFAULT_RATING;

    /* Look for existing match within rating range */
    int rating_range = MATCHMAKING_RATING_RANGE;
    unsigned int current_time = get_current_time();

    for (int i = 0; i < g_state.max_matches; i++) {
        PvPMatchInternal* match = &g_state.matches[i];

        if (!match->in_use) continue;
        if (match->base.match_state != PVP_STATE_WAITING) continue;
        if (match->base.match_type != match_type) continue;
        if (match->is_ranked != is_ranked) continue;
        if (match->base.participant_count >= match->base.max_participants) continue;

        /* Check rating range */
        if (match->base.participant_count > 0) {
            int match_rating = 0;
            for (int j = 0; j < match->base.participant_count; j++) {
                PlayerRankingEntry* other = find_or_create_player(
                    match->base.participants[j].user_id);
                if (other) {
                    match_rating += other->ranking.rank_points;
                }
            }
            match_rating /= match->base.participant_count;

            /* Expand range based on wait time */
            unsigned int wait_time = current_time - match->creation_time;
            int expanded_range = rating_range + (wait_time * MATCHMAKING_EXPAND_STEP);

            if (abs(player_rating - match_rating) <= expanded_range) {
                /* Found suitable match */
                printf("[PvP] Matchmaking: Found match %u for player %u (rating diff: %d)\n",
                       match->base.match_id, user_id, abs(player_rating - match_rating));
                return (int)match->base.match_id;
            }
        }
    }

    /* No suitable match found - create new one */
    printf("[PvP] Matchmaking: Creating new match for player %u\n", user_id);
    return PvP_CreateMatch(match_type, ARENA_TYPE_COLOSSEUM, is_ranked);
}

int PvP_ResetSeasonRankings(void) {
    if (!g_state.initialized) {
        return 0;
    }

    int reset_count = 0;

    printf("[PvP] Resetting season %d rankings...\n", g_state.current_season_id);

    for (int i = 0; i < g_state.ranking_count; i++) {
        if (g_state.rankings[i].in_use) {
            /* Soft reset - decay towards default */
            int current = g_state.rankings[i].ranking.rank_points;
            int new_rating = ELO_DEFAULT_RATING + (current - ELO_DEFAULT_RATING) / 2;

            g_state.rankings[i].ranking.rank_points = new_rating;
            g_state.rankings[i].ranking.rank_tier = calculate_tier(new_rating);
            g_state.rankings[i].ranking.wins = 0;
            g_state.rankings[i].ranking.losses = 0;
            g_state.rankings[i].ranking.win_streak = 0;
            g_state.rankings[i].current_win_streak = 0;
            reset_count++;
        }
    }

    g_state.current_season_id++;
    g_state.stats.season_resets++;

    /* Update all player season IDs */
    for (int i = 0; i < g_state.ranking_count; i++) {
        if (g_state.rankings[i].in_use) {
            g_state.rankings[i].ranking.season_id = g_state.current_season_id;
        }
    }

    update_rank_positions();

    printf("[PvP] Season %d started. %d players reset.\n",
           g_state.current_season_id, reset_count);

    return reset_count;
}

int PvP_Update(int delta_time_ms) {
    if (!g_state.initialized) {
        return 0;
    }

    int matches_updated = 0;
    unsigned int current_time = get_current_time();

    for (int i = 0; i < g_state.max_matches; i++) {
        PvPMatchInternal* match = &g_state.matches[i];

        if (!match->in_use) continue;

        /* Check match duration */
        if (match->base.match_state == PVP_STATE_IN_PROGRESS) {
            unsigned int duration = current_time - match->base.start_time;
            if ((int)duration >= MAX_MATCH_DURATION_SECS) {
                /* Time limit reached - draw or first team wins */
                printf("[PvP] Match %u time limit reached\n", match->base.match_id);
                PvP_EndMatch(match->base.match_id, 0);
            }
            matches_updated++;
        }

        /* Cancel stale waiting matches */
        if (match->base.match_state == PVP_STATE_WAITING) {
            unsigned int age = current_time - match->creation_time;
            if (age > 300) {  /* 5 minutes */
                printf("[PvP] Match %u cancelled (stale)\n", match->base.match_id);
                match->base.match_state = PVP_STATE_CANCELLED;
                g_state.stats.total_matches_cancelled++;
            }
        }

        /* Cleanup finished/cancelled matches after some time */
        if (match->base.match_state == PVP_STATE_FINISHED ||
            match->base.match_state == PVP_STATE_CANCELLED) {
            unsigned int age = current_time - match->base.end_time;
            if (match->base.end_time == 0) {
                age = current_time - match->creation_time;
            }
            if (age > 60) {  /* 1 minute after end */
                match->in_use = 0;
            }
        }
    }

    (void)delta_time_ms;
    return matches_updated;
}

/* ========================================================================
 * DEBUG/STATISTICS API
 * ======================================================================== */

void PvP_PrintStats(void) {
    if (!g_state.initialized) {
        printf("[PvP] Module not initialized\n");
        return;
    }

    int active_matches = 0;
    int waiting_matches = 0;
    for (int i = 0; i < g_state.max_matches; i++) {
        if (g_state.matches[i].in_use) {
            if (g_state.matches[i].base.match_state == PVP_STATE_IN_PROGRESS) {
                active_matches++;
            } else if (g_state.matches[i].base.match_state == PVP_STATE_WAITING) {
                waiting_matches++;
            }
        }
    }

    printf("\n=== PvP Module Statistics ===\n");
    printf("Current Season:      %d\n", g_state.current_season_id);
    printf("Active Matches:      %d\n", active_matches);
    printf("Waiting Matches:     %d\n", waiting_matches);
    printf("Ranked Players:      %d\n", g_state.ranking_count);
    printf("Queue Size:          %d\n", g_state.queue_count);
    printf("\n--- Lifetime Stats ---\n");
    printf("Matches Created:     %u\n", g_state.stats.total_matches_created);
    printf("Matches Completed:   %u\n", g_state.stats.total_matches_completed);
    printf("Matches Cancelled:   %u\n", g_state.stats.total_matches_cancelled);
    printf("Rating Points Given: %u\n", g_state.stats.total_rating_points_awarded);
    printf("Season Resets:       %u\n", g_state.stats.season_resets);
    printf("=============================\n\n");
}

/**
 * Get active match count
 */
int PvP_GetActiveMatchCount(void) {
    if (!g_state.initialized) return 0;

    int count = 0;
    for (int i = 0; i < g_state.max_matches; i++) {
        if (g_state.matches[i].in_use &&
            g_state.matches[i].base.match_state == PVP_STATE_IN_PROGRESS) {
            count++;
        }
    }
    return count;
}

/**
 * Get current season ID
 */
int PvP_GetCurrentSeason(void) {
    return g_state.initialized ? g_state.current_season_id : 0;
}
