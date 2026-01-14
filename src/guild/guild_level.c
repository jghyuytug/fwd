/**
 * @file guild_level.c
 * @brief Guild Level System Implementation
 *
 * Implements guild experience management, level progression,
 * and configuration loading from guild_exp_ref table.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/guild/guild_level.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Database Integration */
#include "../../src/orm/core_game/d_guild/headers/guild_exp_ref_orm.h"

/*==============================================================================
 * Global State
 *============================================================================*/

static GuildLevelState g_level_state = {0};

/*==============================================================================
 * Default Level Configuration (Fallback)
 *
 * These are used if database is unavailable.
 * Based on typical MMO progression curves.
 *============================================================================*/

static const int DEFAULT_EXP_TABLE[GUILD_MAX_LEVEL + 1] = {
    0,          /* Level 0 (unused) */
    0,          /* Level 1 (starting level, no exp required) */
    1000,       /* Level 2: 1,000 exp */
    3000,       /* Level 3: 3,000 exp (累计4,000) */
    6000,       /* Level 4: 6,000 exp (累计10,000) */
    10000,      /* Level 5: 10,000 exp (累计20,000) */
    15000,      /* Level 6: 15,000 exp (累计35,000) */
    21000,      /* Level 7: 21,000 exp (累计56,000) */
    28000,      /* Level 8: 28,000 exp (累计84,000) */
    36000,      /* Level 9: 36,000 exp (累计120,000) */
    45000       /* Level 10: 45,000 exp (累计165,000) */
};

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

int GuildLevel_Initialize(void)
{
    if (g_level_state.initialized) {
        fprintf(stderr, "[GuildLevel] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[GuildLevel] Initializing...\n");

    memset(&g_level_state, 0, sizeof(g_level_state));

    /* Load default configuration */
    for (int i = 0; i <= GUILD_MAX_LEVEL; i++) {
        g_level_state.level_config[i].level = i;
        g_level_state.level_config[i].required_exp = DEFAULT_EXP_TABLE[i];
    }

    g_level_state.config_loaded = 1;
    g_level_state.initialized = 1;

    printf("[GuildLevel] Initialized with default exp table (max level=%d)\n", GUILD_MAX_LEVEL);

    return ERR_SUCCESS;
}

void GuildLevel_Cleanup(void)
{
    if (!g_level_state.initialized) {
        return;
    }

    printf("[GuildLevel] Cleanup...\n");
    printf("[GuildLevel] Statistics:\n");
    printf("  - Total Exp Gained: %u\n", g_level_state.stats.total_exp_gained);
    printf("  - Total Level-Ups: %u\n", g_level_state.stats.total_level_ups);
    printf("  - Total Skill Points Awarded: %u\n", g_level_state.stats.total_skill_points_awarded);

    memset(&g_level_state, 0, sizeof(g_level_state));

    printf("[GuildLevel] Cleanup complete\n");
}

/*==============================================================================
 * Configuration Functions
 *============================================================================*/

int GuildLevel_LoadConfig(DBConnectionManager* manager)
{
    if (!g_level_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    printf("[GuildLevel] Loading level configuration...\n");

    if (!manager) {
        printf("[GuildLevel] No database manager provided, using defaults\n");
        /* Already loaded defaults in Initialize */
        return ERR_SUCCESS;
    }

    /* Load from guild_exp_ref table using ORM */
    GuildExpRef exp_refs[GUILD_MAX_LEVEL + 1];
    int actual_count = 0;
    int result = GuildExpRef_GetAll(manager, exp_refs, GUILD_MAX_LEVEL + 1, &actual_count);

    if (result == 0 && actual_count > 0) {
        for (int i = 0; i < actual_count && i <= GUILD_MAX_LEVEL; i++) {
            int level = exp_refs[i].grade;
            if (level >= 1 && level <= GUILD_MAX_LEVEL) {
                g_level_state.level_config[level].level = level;
                g_level_state.level_config[level].required_exp = exp_refs[i].exp;
            }
        }
        printf("[GuildLevel] Loaded %d level configurations from database\n", actual_count);
    } else {
        printf("[GuildLevel] Failed to load from database (result=%d, count=%d), using defaults\n",
               result, actual_count);
    }

    return ERR_SUCCESS;
}

int GuildLevel_GetExpForLevel(int level)
{
    if (!g_level_state.initialized) {
        return -1;
    }

    if (level < GUILD_MIN_LEVEL || level > GUILD_MAX_LEVEL) {
        return -1;
    }

    return g_level_state.level_config[level].required_exp;
}

/*==============================================================================
 * Experience Management Functions
 *============================================================================*/

int GuildLevel_AddExp(int guild_id,
                      int* current_level,
                      int* current_exp,
                      int exp_to_add,
                      int* out_leveled_up,
                      int* out_new_level,
                      int* out_skill_points)
{
    if (!g_level_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!current_level || !current_exp) {
        return ERR_INVALID_PARAMETER;
    }

    if (exp_to_add <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    if (*current_level < GUILD_MIN_LEVEL || *current_level > GUILD_MAX_LEVEL) {
        return ERR_INVALID_PARAMETER;
    }

    /* Add experience */
    *current_exp += exp_to_add;
    g_level_state.stats.total_exp_gained += exp_to_add;

    int leveled_up = 0;
    int new_level = *current_level;
    int total_skill_points = 0;

    /* Check for level-ups (can level up multiple times) */
    while (new_level < GUILD_MAX_LEVEL) {
        int required_exp = g_level_state.level_config[new_level + 1].required_exp;

        if (*current_exp >= required_exp) {
            /* Level up! */
            new_level++;
            leveled_up = 1;

            /* Award skill points */
            int skill_points = GuildLevel_CalculateSkillPoints(new_level);
            total_skill_points += skill_points;

            g_level_state.stats.total_level_ups++;
            g_level_state.stats.total_skill_points_awarded += skill_points;

            printf("[GuildLevel] Guild %d leveled up to %d! (Skill points: %d)\n",
                   guild_id, new_level, skill_points);
        } else {
            break; /* No more level-ups possible */
        }
    }

    /* Update output parameters */
    if (leveled_up) {
        *current_level = new_level;
    }

    if (out_leveled_up) {
        *out_leveled_up = leveled_up;
    }

    if (out_new_level) {
        *out_new_level = new_level;
    }

    if (out_skill_points) {
        *out_skill_points = total_skill_points;
    }

    return ERR_SUCCESS;
}

int GuildLevel_CanLevelUp(int current_level, int current_exp)
{
    if (!g_level_state.initialized) {
        return 0;
    }

    if (current_level < GUILD_MIN_LEVEL || current_level >= GUILD_MAX_LEVEL) {
        return 0; /* Already at max level or invalid */
    }

    int required_exp = g_level_state.level_config[current_level + 1].required_exp;

    return (current_exp >= required_exp) ? 1 : 0;
}

int GuildLevel_CalculateSkillPoints(int new_level)
{
    if (new_level < GUILD_MIN_LEVEL || new_level > GUILD_MAX_LEVEL) {
        return 0;
    }

    /* Skill point award formula:
     * - Level 2-5: 1 skill point per level
     * - Level 6-10: 2 skill points per level
     */
    if (new_level >= 6) {
        return 2;
    } else if (new_level >= 2) {
        return 1;
    } else {
        return 0; /* Level 1 gives no skill points */
    }
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int GuildLevel_GetExpToNextLevel(int current_level, int current_exp)
{
    if (!g_level_state.initialized) {
        return 0;
    }

    if (current_level < GUILD_MIN_LEVEL || current_level >= GUILD_MAX_LEVEL) {
        return 0; /* Already at max level or invalid */
    }

    int required_exp = g_level_state.level_config[current_level + 1].required_exp;
    int remaining = required_exp - current_exp;

    return (remaining > 0) ? remaining : 0;
}

int GuildLevel_GetProgressPercentage(int current_level, int current_exp)
{
    if (!g_level_state.initialized) {
        return 0;
    }

    if (current_level < GUILD_MIN_LEVEL || current_level > GUILD_MAX_LEVEL) {
        return 0;
    }

    if (current_level >= GUILD_MAX_LEVEL) {
        return 100; /* Max level reached */
    }

    int current_level_exp = (current_level > 1) ?
        g_level_state.level_config[current_level].required_exp : 0;
    int next_level_exp = g_level_state.level_config[current_level + 1].required_exp;

    int exp_in_current_level = current_exp - current_level_exp;
    int exp_needed_for_level = next_level_exp - current_level_exp;

    if (exp_needed_for_level <= 0) {
        return 100;
    }

    int percentage = (exp_in_current_level * 100) / exp_needed_for_level;

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    return percentage;
}

int GuildLevel_GetStatistics(unsigned int* total_exp_gained,
                              unsigned int* total_level_ups,
                              unsigned int* total_skill_points)
{
    if (!g_level_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (total_exp_gained) {
        *total_exp_gained = g_level_state.stats.total_exp_gained;
    }

    if (total_level_ups) {
        *total_level_ups = g_level_state.stats.total_level_ups;
    }

    if (total_skill_points) {
        *total_skill_points = g_level_state.stats.total_skill_points_awarded;
    }

    return ERR_SUCCESS;
}
