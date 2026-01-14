/**
 * @file pvp_result_orm.h
 * @brief PVP Result ORM Module - Player PVP Statistics Management
 *
 * This module provides Object-Relational Mapping for the pvp_result table,
 * which stores player PVP (Player vs Player) match statistics and rankings.
 *
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 * Table: pvp_result
 * Primary Key: charac_no
 *
 * Features:
 * - Win/Loss record tracking
 * - PVP ranking and grade system
 * - Detailed combat statistics (kills, combos, damage, etc.)
 * - Average performance metrics
 * - Play time and count tracking
 *
 * @version 1.0
 * @date 2025-01-13
 */

#ifndef PVP_RESULT_ORM_H
#define PVP_RESULT_ORM_H

#include "db_connection_manager.h"
#include <time.h>

/**
 * @struct PvpResult
 * @brief PVP statistics structure for player
 *
 * This structure contains comprehensive PVP statistics including:
 * - Win/Loss records
 * - PVP points and grade
 * - Average combat statistics (kills, buffs, combos, etc.)
 * - Play time and activity tracking
 */
typedef struct {
    int charac_no;                     /**< Character number (Primary Key) */
    int win;                           /**< Total wins */
    int lose;                          /**< Total losses */
    int pvp_point;                     /**< Current PVP points */
    int pvp_grade;                     /**< PVP grade/rank */
    unsigned char pvp_grade_ext;       /**< Extended PVP grade */

    /* Average Combat Statistics */
    int avg_kill_count;                /**< Average kills per match */
    int avg_buf_count;                 /**< Average buffs applied */
    int avg_debuf_count;               /**< Average debuffs applied */
    int avg_heal_count;                /**< Average heals performed */
    int avg_counter_count;             /**< Average counter attacks */
    int avg_back_atk_count;            /**< Average back attacks */
    int avg_union_hit_count;           /**< Average union hits */
    int avg_overkill_count;            /**< Average overkills */
    int avg_aerial_count;              /**< Average aerial combos */
    int avg_combo_count;               /**< Average combo count */
    int avg_attacked_count;            /**< Average times attacked */
    int avg_deal_damage;               /**< Average damage dealt */
    int avg_technic;                   /**< Average technique score */
    int avg_style;                     /**< Average style score */
    int avg_hit_penalty;               /**< Average hit penalty */

    /* Match Statistics */
    int pvp_count;                     /**< Total PVP matches */
    int win_point;                     /**< Win points accumulated */
    char last_play_time[32];           /**< Last play datetime */
    unsigned int play_count;           /**< Total play count */
    unsigned int play_time;            /**< Total play time (seconds) */
    char pvp_grade_ext_update_time[32]; /**< Last grade update time */
} PvpResult;

/**
 * @brief Create a new PVP result record
 *
 * @param manager Database connection manager
 * @param result PVP result data to insert
 * @return 0 on success, -1 on failure
 */
int PvpResult_Create(DBConnectionManager* manager, PvpResult* result);

/**
 * @brief Get PVP result by character number
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param result Output buffer for result data
 * @return 0 on success, -1 on failure
 */
int PvpResult_GetByCharacNo(DBConnectionManager* manager, int charac_no, PvpResult* result);

/**
 * @brief Update existing PVP result record
 *
 * @param manager Database connection manager
 * @param result Updated PVP result data
 * @return 0 on success, -1 on failure
 */
int PvpResult_Update(DBConnectionManager* manager, PvpResult* result);

/**
 * @brief Delete PVP result record
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int PvpResult_Delete(DBConnectionManager* manager, int charac_no);

/**
 * @brief Check if PVP result exists for character
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int PvpResult_Exists(DBConnectionManager* manager, int charac_no);

/**
 * @brief Record match result (win/loss)
 *
 * This function updates win or loss count and recalculates statistics
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param is_win 1 for win, 0 for loss
 * @param points Points gained/lost
 * @return 0 on success, -1 on failure
 */
int PvpResult_RecordMatch(DBConnectionManager* manager, int charac_no, int is_win, int points);

/**
 * @brief Update PVP grade
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param grade New grade value
 * @param grade_ext New extended grade value
 * @return 0 on success, -1 on failure
 */
int PvpResult_UpdateGrade(DBConnectionManager* manager, int charac_no, int grade, unsigned char grade_ext);

/**
 * @brief Update average combat statistics
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param result PVP result with updated average statistics
 * @return 0 on success, -1 on failure
 */
int PvpResult_UpdateAverages(DBConnectionManager* manager, int charac_no, PvpResult* result);

/**
 * @brief Get top players by PVP points
 *
 * @param manager Database connection manager
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int PvpResult_GetTopByPoints(DBConnectionManager* manager, PvpResult* results, int max_count);

/**
 * @brief Get top players by win rate
 *
 * @param manager Database connection manager
 * @param min_matches Minimum matches required
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int PvpResult_GetTopByWinRate(DBConnectionManager* manager, int min_matches, PvpResult* results, int max_count);

/**
 * @brief Get players by grade
 *
 * @param manager Database connection manager
 * @param grade Grade to filter by
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int PvpResult_GetByGrade(DBConnectionManager* manager, int grade, PvpResult* results, int max_count);

/**
 * @brief Get total PVP statistics count
 *
 * @param manager Database connection manager
 * @return Total count, or -1 on error
 */
int PvpResult_GetTotalCount(DBConnectionManager* manager);

/**
 * @brief Update last play time
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int PvpResult_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no);

/**
 * @brief Increment play count and add play time
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param play_time_seconds Play time to add (in seconds)
 * @return 0 on success, -1 on failure
 */
int PvpResult_IncrementPlayCount(DBConnectionManager* manager, int charac_no, unsigned int play_time_seconds);

/**
 * @brief Get players with high win rates
 *
 * @param manager Database connection manager
 * @param min_win_rate Minimum win rate (0-100)
 * @param min_matches Minimum matches required
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int PvpResult_GetHighWinRate(DBConnectionManager* manager, int min_win_rate, int min_matches,
                             PvpResult* results, int max_count);

/**
 * @brief Get recently active players
 *
 * @param manager Database connection manager
 * @param days Days to look back
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int PvpResult_GetRecentlyActive(DBConnectionManager* manager, int days, PvpResult* results, int max_count);

#endif /* PVP_RESULT_ORM_H */
