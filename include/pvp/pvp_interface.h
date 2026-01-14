/**
 * PvP Module - Public Interface
 * Purpose: Player vs Player combat, rankings, arenas, matchmaking
 *
 * Components:
 * - PvP match creation and management
 * - Arena system with different modes
 * - Ranking and rating system
 * - Match history and statistics
 * - Rewards and point distribution
 */

#ifndef INCLUDE_PVP_PVP_INTERFACE_H_
#define INCLUDE_PVP_PVP_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * PvP match types
 */
typedef enum {
    PVP_TYPE_1V1 = 0,           // 1v1 duel
    PVP_TYPE_3V3 = 1,           // 3v3 team battle
    PVP_TYPE_FREE_FOR_ALL = 2,  // Free for all
    PVP_TYPE_RANKED = 3,        // Ranked match
    PVP_TYPE_UNRANKED = 4,      // Casual match
    PVP_TYPE_TOURNAMENT = 5     // Tournament match
} PvPMatchType;

/**
 * PvP match states
 */
typedef enum {
    PVP_STATE_WAITING = 0,      // Waiting for players
    PVP_STATE_READY = 1,        // All players ready
    PVP_STATE_IN_PROGRESS = 2,  // Match ongoing
    PVP_STATE_FINISHED = 3,     // Match completed
    PVP_STATE_CANCELLED = 4     // Match cancelled
} PvPMatchState;

/**
 * PvP arena types
 */
typedef enum {
    ARENA_TYPE_COLOSSEUM = 0,   // Standard arena
    ARENA_TYPE_BATTLEFIELD = 1, // Large battlefield
    ARENA_TYPE_DUEL_RING = 2,   // Small duel area
    ARENA_TYPE_TEAM_ARENA = 3   // Team arena
} ArenaType;

/**
 * PvP ranking tiers
 */
typedef enum {
    RANK_TIER_BRONZE = 0,
    RANK_TIER_SILVER = 1,
    RANK_TIER_GOLD = 2,
    RANK_TIER_PLATINUM = 3,
    RANK_TIER_DIAMOND = 4,
    RANK_TIER_MASTER = 5,
    RANK_TIER_GRANDMASTER = 6
} RankTier;

/**
 * PvP match participant information
 */
typedef struct {
    unsigned int user_id;
    unsigned int character_id;
    int team_id;                // 0 or 1 for team matches, -1 for FFA
    int starting_hp;
    int final_hp;
    int damage_dealt;
    int damage_received;
    int kills;
    int deaths;
    int is_winner;
} PvPParticipant;

/**
 * PvP match information
 */
typedef struct {
    unsigned int match_id;
    PvPMatchType match_type;
    PvPMatchState match_state;
    ArenaType arena_type;
    unsigned int start_time;
    unsigned int end_time;
    int participant_count;
    int max_participants;
    PvPParticipant participants[8];  // Up to 8 players max
    int winning_team;                // 0, 1, or -1 for FFA winner
} PvPMatchInfo;

/**
 * PvP ranking information
 */
typedef struct {
    unsigned int user_id;
    RankTier rank_tier;
    int rank_points;
    int rank_position;           // Global position
    int wins;
    int losses;
    int win_streak;
    int season_id;
} PvPRankingInfo;

/**
 * PvP statistics
 */
typedef struct {
    unsigned int total_matches_played;
    unsigned int total_wins;
    unsigned int total_losses;
    unsigned int total_kills;
    unsigned int total_deaths;
    unsigned int highest_win_streak;
    unsigned int total_damage_dealt;
    unsigned int total_damage_received;
    float win_rate;
    float kd_ratio;
} PvPStats;

/**
 * Initialize PvP Module
 *
 * Sets up PvP system with arenas and ranking
 *
 * Parameters:
 *   max_concurrent_matches: Maximum concurrent PvP matches
 *   max_ranked_players: Maximum players in ranking system
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_Initialize(int max_concurrent_matches, int max_ranked_players);

/**
 * Cleanup PvP Module
 *
 * Releases PvP system resources
 */
void PvP_Cleanup();

/**
 * Create PvP Match
 *
 * Creates a new PvP match
 *
 * Parameters:
 *   match_type: Type of match to create
 *   arena_type: Arena to use
 *   is_ranked: Whether this is a ranked match
 *
 * Returns: Match ID on success, negative error code on failure
 */
int PvP_CreateMatch(PvPMatchType match_type, ArenaType arena_type, int is_ranked);

/**
 * Join PvP Match
 *
 * Player joins an existing match
 *
 * Parameters:
 *   match_id: Match ID to join
 *   user_id: User ID
 *   character_id: Character ID
 *   team_id: Team to join (0 or 1), -1 for auto-assign
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_JoinMatch(unsigned int match_id, unsigned int user_id,
                  unsigned int character_id, int team_id);

/**
 * Leave PvP Match
 *
 * Player leaves a match
 *
 * Parameters:
 *   match_id: Match ID
 *   user_id: User ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_LeaveMatch(unsigned int match_id, unsigned int user_id);

/**
 * Start PvP Match
 *
 * Starts a match (all players ready)
 *
 * Parameters:
 *   match_id: Match ID to start
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_StartMatch(unsigned int match_id);

/**
 * End PvP Match
 *
 * Ends a match and distributes rewards
 *
 * Parameters:
 *   match_id: Match ID to end
 *   winning_team: Winning team (0, 1) or winner user_id for FFA
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_EndMatch(unsigned int match_id, int winning_team);

/**
 * Get Match Info
 *
 * Retrieves match information
 *
 * Parameters:
 *   match_id: Match ID
 *   match_info: Output match structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_GetMatchInfo(unsigned int match_id, PvPMatchInfo* match_info);

/**
 * Get Player Ranking
 *
 * Retrieves player's ranking information
 *
 * Parameters:
 *   user_id: User ID
 *   season_id: Season ID (0 for current)
 *   ranking_info: Output ranking structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_GetPlayerRanking(unsigned int user_id, int season_id,
                         PvPRankingInfo* ranking_info);

/**
 * Get Top Rankings
 *
 * Retrieves top ranked players
 *
 * Parameters:
 *   rankings: Output array of ranking info
 *   max_rankings: Maximum rankings to retrieve
 *   season_id: Season ID (0 for current)
 *
 * Returns: Number of rankings retrieved
 */
int PvP_GetTopRankings(PvPRankingInfo* rankings, int max_rankings, int season_id);

/**
 * Update Player Rating
 *
 * Updates player rating after a match
 *
 * Parameters:
 *   user_id: User ID
 *   points_change: Points gained or lost (can be negative)
 *   did_win: Whether player won
 *
 * Returns: New rating points
 */
int PvP_UpdatePlayerRating(unsigned int user_id, int points_change, int did_win);

/**
 * Get PvP Statistics
 *
 * Retrieves PvP statistics for a player
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int PvP_GetStatistics(unsigned int user_id, PvPStats* stats);

/**
 * Find Match (Matchmaking)
 *
 * Finds a suitable match for player
 *
 * Parameters:
 *   user_id: User ID
 *   match_type: Desired match type
 *   is_ranked: Whether looking for ranked match
 *
 * Returns: Match ID on success, negative error code if no match found
 */
int PvP_FindMatch(unsigned int user_id, PvPMatchType match_type, int is_ranked);

/**
 * Reset Season Rankings
 *
 * Resets rankings for new season
 *
 * Returns: Number of players reset
 */
int PvP_ResetSeasonRankings();

/**
 * Update PvP State
 *
 * Processes PvP tick, updates ongoing matches
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of matches updated
 */
int PvP_Update(int delta_time_ms);

#endif // INCLUDE_PVP_PVP_INTERFACE_H_
