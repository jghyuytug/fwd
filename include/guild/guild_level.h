/**
 * @file guild_level.h
 * @brief Guild Level System - Experience and Leveling Management
 *
 * This module manages guild experience gain, level progression,
 * and reads level configuration from guild_exp_ref table.
 *
 * Database Tables Used:
 * - guild_exp_ref (GuildExpRef ORM) - Level-up experience requirements
 *
 * Confirmed Parameters:
 * - Max Level: 10 (user confirmed)
 *
 * @version 1.0
 * @date 2025-11-23
 */

#ifndef GUILD_LEVEL_H
#define GUILD_LEVEL_H

#include "../../include/common/error_codes.h"
#include "../../src/orm/core_game/d_guild/headers/guild_exp_ref_orm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Constants
 *============================================================================*/

#define GUILD_MAX_LEVEL             10      /* ✅ User confirmed: max 10 levels */
#define GUILD_MIN_LEVEL             1       /* Minimum guild level */

/*==============================================================================
 * Structures
 *============================================================================*/

/**
 * Guild Level Configuration Entry
 * Loaded from guild_exp_ref table
 */
typedef struct {
    int level;                      /* Guild level (1-10) */
    int required_exp;               /* Experience required to reach this level */
} GuildLevelConfig;

/**
 * Guild Level State
 */
typedef struct {
    int initialized;

    /* Level configuration table (loaded from database) */
    GuildLevelConfig level_config[GUILD_MAX_LEVEL + 1];  /* Index 0 unused, 1-10 used */
    int config_loaded;

    /* Statistics */
    struct {
        unsigned int total_exp_gained;
        unsigned int total_level_ups;
        unsigned int total_skill_points_awarded;
    } stats;
} GuildLevelState;

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

/**
 * Initialize Guild Level System
 *
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildLevel_Initialize(void);

/**
 * Cleanup Guild Level System
 */
void GuildLevel_Cleanup(void);

/*==============================================================================
 * Configuration Functions
 *============================================================================*/

/**
 * Load Level Configuration from Database
 *
 * Reads guild_exp_ref table to populate level-up requirements.
 * If database is unavailable, uses hardcoded defaults.
 *
 * @param manager Database connection manager (can be NULL for defaults)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildLevel_LoadConfig(DBConnectionManager* manager);

/**
 * Get Experience Required for Level
 *
 * @param level Target level (1-10)
 * @return Experience required, or -1 if invalid level
 */
int GuildLevel_GetExpForLevel(int level);

/*==============================================================================
 * Experience Management Functions
 *============================================================================*/

/**
 * Add Experience to Guild
 *
 * Adds experience to guild and checks for level-up.
 * If level-up occurs, awards skill points.
 *
 * @param guild_id Guild ID
 * @param current_level Current guild level (in/out)
 * @param current_exp Current guild experience (in/out)
 * @param exp_to_add Experience points to add
 * @param out_leveled_up Output: 1 if leveled up, 0 if not (can be NULL)
 * @param out_new_level Output: new level if leveled up (can be NULL)
 * @param out_skill_points Output: skill points awarded (can be NULL)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildLevel_AddExp(int guild_id,
                      int* current_level,
                      int* current_exp,
                      int exp_to_add,
                      int* out_leveled_up,
                      int* out_new_level,
                      int* out_skill_points);

/**
 * Check if Can Level Up
 *
 * @param current_level Current guild level
 * @param current_exp Current guild experience
 * @return 1 if can level up, 0 if not
 */
int GuildLevel_CanLevelUp(int current_level, int current_exp);

/**
 * Calculate Skill Points for Level
 *
 * Awards skill points based on level reached:
 * - Level 2-5: 1 skill point per level
 * - Level 6-10: 2 skill points per level
 *
 * @param new_level New level reached
 * @return Number of skill points awarded
 */
int GuildLevel_CalculateSkillPoints(int new_level);

/*==============================================================================
 * Query Functions
 *============================================================================*/

/**
 * Get Experience to Next Level
 *
 * @param current_level Current guild level
 * @param current_exp Current guild experience
 * @return Experience needed for next level, or 0 if max level
 */
int GuildLevel_GetExpToNextLevel(int current_level, int current_exp);

/**
 * Get Progress Percentage
 *
 * @param current_level Current guild level
 * @param current_exp Current guild experience
 * @return Progress percentage (0-100), or 100 if max level
 */
int GuildLevel_GetProgressPercentage(int current_level, int current_exp);

/**
 * Get Statistics
 *
 * @param total_exp_gained Output: total experience gained
 * @param total_level_ups Output: total level-ups
 * @param total_skill_points Output: total skill points awarded
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildLevel_GetStatistics(unsigned int* total_exp_gained,
                              unsigned int* total_level_ups,
                              unsigned int* total_skill_points);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_LEVEL_H */
