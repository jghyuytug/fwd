/**
 * @file guild_skill.h
 * @brief Guild Skill System - Skill Point and Skill Management
 *
 * This module manages guild skill points, skill learning,
 * and skill effects based on guild_skill table.
 *
 * Database Tables Used:
 * - guild_skill (GuildSkill ORM) - Skill points (remain_sp, used_sp)
 *
 * @version 1.0
 * @date 2025-11-23
 */

#ifndef GUILD_SKILL_H
#define GUILD_SKILL_H

#include "../../include/common/error_codes.h"
#include "../../src/orm/core_game/d_guild/headers/guild_skill_orm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Constants
 *============================================================================*/

#define GUILD_MAX_SKILLS            20      /* Maximum number of guild skills */
#define GUILD_MAX_SKILL_LEVEL       10      /* Maximum level per skill */

/*==============================================================================
 * Enumerations
 *============================================================================*/

/**
 * Guild Skill Type
 * These are common guild skills found in MMOs
 */
typedef enum {
    GUILD_SKILL_NONE = 0,

    /* Member Benefits */
    GUILD_SKILL_EXP_BOOST = 1,          /* Increases member exp gain */
    GUILD_SKILL_DROP_RATE = 2,          /* Increases item drop rate */
    GUILD_SKILL_GOLD_BOOST = 3,         /* Increases gold gain */

    /* Combat Bonuses */
    GUILD_SKILL_ATK_BOOST = 4,          /* Increases attack power */
    GUILD_SKILL_DEF_BOOST = 5,          /* Increases defense */
    GUILD_SKILL_HP_BOOST = 6,           /* Increases max HP */
    GUILD_SKILL_MP_BOOST = 7,           /* Increases max MP */

    /* Guild Features */
    GUILD_SKILL_MEMBER_LIMIT = 8,       /* Increases max member count */
    GUILD_SKILL_WAREHOUSE_SIZE = 9,     /* Increases warehouse capacity */
    GUILD_SKILL_RECALL = 10,            /* Guild member recall ability */

    /* Special Skills */
    GUILD_SKILL_PVP_DAMAGE = 11,        /* PvP damage bonus */
    GUILD_SKILL_PVE_DAMAGE = 12,        /* PvE damage bonus */
    GUILD_SKILL_CRIT_RATE = 13,         /* Critical hit rate */
    GUILD_SKILL_MOVE_SPEED = 14,        /* Movement speed */

    GUILD_SKILL_MAX = 15                /* Total skill types */
} GuildSkillType;

/*==============================================================================
 * Structures
 *============================================================================*/

/**
 * Guild Skill Configuration
 */
typedef struct {
    GuildSkillType skill_type;          /* Skill type */
    char skill_name[32];                /* Skill name */
    int max_level;                      /* Maximum skill level */
    int sp_cost_per_level;              /* SP cost per level */
    float effect_per_level;             /* Effect increase per level */
} GuildSkillConfig;

/**
 * Learned Guild Skill
 */
typedef struct {
    GuildSkillType skill_type;          /* Skill type */
    int skill_level;                    /* Current skill level (0-10) */
} LearnedSkill;

/**
 * Guild Skill Data
 */
typedef struct {
    int guild_id;                       /* Guild ID */
    int remain_sp;                      /* Remaining skill points */
    int used_sp;                        /* Used skill points */
    int skill_count;                    /* Number of learned skills */
    LearnedSkill skills[GUILD_MAX_SKILLS]; /* Learned skills */
} GuildSkillData;

/**
 * Guild Skill State
 */
typedef struct {
    int initialized;

    /* Skill configurations */
    GuildSkillConfig skill_configs[GUILD_SKILL_MAX];
    int config_count;

    /* Statistics */
    struct {
        unsigned int total_skills_learned;
        unsigned int total_sp_used;
        unsigned int total_skill_resets;
    } stats;
} GuildSkillState;

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

/**
 * Initialize Guild Skill System
 *
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_Initialize(void);

/**
 * Cleanup Guild Skill System
 */
void GuildSkill_Cleanup(void);

/*==============================================================================
 * Skill Point Management Functions
 *============================================================================*/

/**
 * Get Guild Skill Points
 *
 * @param guild_id Guild ID
 * @param out_remain_sp Output: remaining skill points
 * @param out_used_sp Output: used skill points
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_GetSkillPoints(int guild_id, int* out_remain_sp, int* out_used_sp);

/**
 * Add Skill Points (called when guild levels up)
 *
 * @param guild_id Guild ID
 * @param points Skill points to add
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_AddSkillPoints(int guild_id, int points);

/*==============================================================================
 * Skill Learning Functions
 *============================================================================*/

/**
 * Learn Skill
 *
 * Increases skill level by 1 if guild has enough SP.
 *
 * @param guild_id Guild ID
 * @param skill_type Skill to learn
 * @param remain_sp Current remaining SP (in/out)
 * @param used_sp Current used SP (in/out)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_LearnSkill(int guild_id,
                          GuildSkillType skill_type,
                          int* remain_sp,
                          int* used_sp);

/**
 * Get Skill Level
 *
 * @param guild_id Guild ID
 * @param skill_type Skill type
 * @return Skill level (0-10), or -1 on error
 */
int GuildSkill_GetSkillLevel(int guild_id, GuildSkillType skill_type);

/**
 * Get Skill Effect Value
 *
 * @param skill_type Skill type
 * @param skill_level Skill level
 * @return Effect value (e.g., 0.05 for 5% bonus)
 */
float GuildSkill_GetSkillEffect(GuildSkillType skill_type, int skill_level);

/*==============================================================================
 * Skill Reset Functions
 *============================================================================*/

/**
 * Reset All Skills
 *
 * Resets all learned skills and refunds SP.
 *
 * @param guild_id Guild ID
 * @param remain_sp Current remaining SP (in/out)
 * @param used_sp Current used SP (in/out)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_ResetSkills(int guild_id, int* remain_sp, int* used_sp);

/*==============================================================================
 * Query Functions
 *============================================================================*/

/**
 * Get All Learned Skills
 *
 * @param guild_id Guild ID
 * @param skills Output array for skills
 * @param max_skills Maximum skills to retrieve
 * @return Number of skills retrieved, or negative on error
 */
int GuildSkill_GetLearnedSkills(int guild_id,
                                LearnedSkill* skills,
                                int max_skills);

/**
 * Get Skill Cost
 *
 * @param skill_type Skill type
 * @param current_level Current skill level
 * @return SP cost to level up, or -1 if max level
 */
int GuildSkill_GetSkillCost(GuildSkillType skill_type, int current_level);

/**
 * Get Skill Name
 *
 * @param skill_type Skill type
 * @return Skill name string, or "Unknown" if invalid
 */
const char* GuildSkill_GetSkillName(GuildSkillType skill_type);

/**
 * Get Statistics
 *
 * @param total_skills_learned Output: total skills learned
 * @param total_sp_used Output: total SP used
 * @param total_resets Output: total skill resets
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildSkill_GetStatistics(unsigned int* total_skills_learned,
                              unsigned int* total_sp_used,
                              unsigned int* total_resets);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_SKILL_H */
