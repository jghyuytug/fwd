/**
 * Skill Module - Public Interface
 * Purpose: Skill system, skill trees, upgrades, and execution
 *
 * Components:
 * - Skill learning and progression
 * - Skill upgrades and level management
 * - Skill usage and cooldown tracking
 * - SP (Skill Points) management
 * - Skill tree and specialization
 */

#ifndef SRC_SKILL_SKILL_INTERFACE_H_
#define SRC_SKILL_SKILL_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Skill types
 */
typedef enum {
    SKILL_TYPE_ACTIVE = 0,      // Active skill (requires casting)
    SKILL_TYPE_PASSIVE = 1,     // Passive skill (always active)
    SKILL_TYPE_BUFF = 2,        // Buff skill (enhances attributes)
    SKILL_TYPE_DEBUFF = 3,      // Debuff skill (weakens enemies)
    SKILL_TYPE_ULTIMATE = 4     // Ultimate skill (high power, long cooldown)
} SkillType;

/**
 * Skill elements
 */
typedef enum {
    SKILL_ELEMENT_PHYSICAL = 0, // Physical damage
    SKILL_ELEMENT_FIRE = 1,     // Fire element
    SKILL_ELEMENT_ICE = 2,      // Ice element
    SKILL_ELEMENT_LIGHTNING = 3,// Lightning element
    SKILL_ELEMENT_POISON = 4,   // Poison element
    SKILL_ELEMENT_LIGHT = 5,    // Light/Holy element
    SKILL_ELEMENT_DARK = 6      // Dark/Shadow element
} SkillElement;

/**
 * Skill information structure
 */
typedef struct {
    unsigned int skill_id;
    unsigned int skill_template_id;
    unsigned int user_id;
    int skill_level;
    int max_level;
    int cooldown_remaining;        // Cooldown remaining (milliseconds)
    int sp_cost;                   // SP required for next level
    SkillType skill_type;
    SkillElement element;
    int is_learned;                // 1 if skill is learned
    int is_equipped;               // 1 if skill is equipped/active
} SkillInfo;

/**
 * Skill statistics structure
 */
typedef struct {
    unsigned int total_skills_learned;
    unsigned int total_skills_used;
    unsigned int total_sp_spent;
    unsigned int skills_reset;
} SkillStats;

/**
 * Initialize Skill Module
 *
 * Sets up skill management system
 *
 * Parameters:
 *   max_users: Maximum number of users that can have skills
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Skill_Initialize(int max_users);

/**
 * Cleanup Skill Module
 *
 * Releases skill system resources
 */
void Skill_Cleanup();

/**
 * Learn Skill
 *
 * User learns a new skill
 *
 * Parameters:
 *   user_id: User ID
 *   skill_template_id: Skill template ID
 *
 * Returns: Skill ID on success, negative error code on failure
 */
int Skill_LearnSkill(unsigned int user_id, unsigned int skill_template_id);

/**
 * Upgrade Skill
 *
 * Increases skill level
 *
 * Parameters:
 *   user_id: User ID
 *   skill_id: Skill ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Skill_UpgradeSkill(unsigned int user_id, unsigned int skill_id);

/**
 * Use Skill
 *
 * Activates a skill on target
 *
 * Parameters:
 *   user_id: User ID
 *   skill_id: Skill ID to use
 *   target_id: Target entity ID (0 for self-cast)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Skill_UseSkill(unsigned int user_id,
                   unsigned int skill_id,
                   unsigned int target_id);

/**
 * Get Skill Info
 *
 * Retrieves skill information
 *
 * Parameters:
 *   skill_id: Skill ID
 *   skill_info: Output skill structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Skill_GetSkillInfo(unsigned int skill_id, SkillInfo* skill_info);

/**
 * Get User Skills
 *
 * Retrieves list of user's learned skills
 *
 * Parameters:
 *   user_id: User ID
 *   skills: Output array of skill info
 *   max_skills: Maximum skills to retrieve
 *
 * Returns: Number of learned skills
 */
int Skill_GetUserSkills(unsigned int user_id,
                        SkillInfo* skills,
                        int max_skills);

/**
 * Reset Skills
 *
 * Resets all skills for user, refunds SP
 *
 * Parameters:
 *   user_id: User ID
 *
 * Returns: Amount of SP refunded on success, negative error code on failure
 */
int Skill_ResetSkills(unsigned int user_id);

/**
 * Update Skill State
 *
 * Processes skill cooldowns and state updates
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of skills updated
 */
int Skill_Update(int delta_time_ms);

/**
 * Get Skill Statistics
 *
 * Retrieves skill statistics for user
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Skill_GetStatistics(unsigned int user_id, SkillStats* stats);

#endif // SRC_SKILL_SKILL_INTERFACE_H_
