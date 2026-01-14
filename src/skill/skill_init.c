/**
 * Skill Module - Implementation
 * Stub implementation for skill system
 */

#include <stdio.h>
#include <string.h>
#include "skill_interface.h"

/**
 * Module global state
 */
typedef struct {
    int initialized;
    int max_users;

    // Global statistics
    unsigned int total_skills_learned;
    unsigned int total_skills_used;
    unsigned int total_skills_upgraded;
    unsigned int total_skills_reset;
    unsigned int total_sp_spent;
} SkillModuleState;

static SkillModuleState g_skill_state = {0};

/**
 * Initialize Skill Module
 */
int Skill_Initialize(int max_users) {
    if (g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module already initialized\n");
        return ERR_ALREADY_INITIALIZED;
    }

    if (max_users <= 0) {
        fprintf(stderr, "[Skill] Invalid max_users: %d\n", max_users);
        return ERR_INVALID_PARAMETER;
    }

    memset(&g_skill_state, 0, sizeof(SkillModuleState));
    g_skill_state.max_users = max_users;
    g_skill_state.initialized = 1;

    printf("[Skill] Module initialized (max_users: %d)\n", max_users);
    return ERR_SUCCESS;
}

/**
 * Cleanup Skill Module
 */
void Skill_Cleanup() {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return;
    }

    printf("[Skill] Module cleanup (stats - learned: %u, used: %u, upgraded: %u, reset: %u, sp_spent: %u)\n",
           g_skill_state.total_skills_learned,
           g_skill_state.total_skills_used,
           g_skill_state.total_skills_upgraded,
           g_skill_state.total_skills_reset,
           g_skill_state.total_sp_spent);

    memset(&g_skill_state, 0, sizeof(SkillModuleState));
}

/**
 * Learn Skill
 */
int Skill_LearnSkill(unsigned int user_id, unsigned int skill_template_id) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return -ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || skill_template_id == 0) {
        fprintf(stderr, "[Skill] Invalid parameters (user_id: %u, template_id: %u)\n",
                user_id, skill_template_id);
        return -ERR_INVALID_PARAMETER;
    }

    // Stub: Generate skill ID
    unsigned int skill_id = (user_id * 10000) + skill_template_id;
    g_skill_state.total_skills_learned++;

    printf("[Skill] User %u learned skill template %u (skill_id: %u)\n",
           user_id, skill_template_id, skill_id);

    return (int)skill_id;
}

/**
 * Upgrade Skill
 */
int Skill_UpgradeSkill(unsigned int user_id, unsigned int skill_id) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || skill_id == 0) {
        fprintf(stderr, "[Skill] Invalid parameters (user_id: %u, skill_id: %u)\n",
                user_id, skill_id);
        return ERR_INVALID_PARAMETER;
    }

    g_skill_state.total_skills_upgraded++;
    g_skill_state.total_sp_spent += 1; // Stub: 1 SP per upgrade

    printf("[Skill] User %u upgraded skill %u\n", user_id, skill_id);
    return ERR_SUCCESS;
}

/**
 * Use Skill
 */
int Skill_UseSkill(unsigned int user_id,
                   unsigned int skill_id,
                   unsigned int target_id) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || skill_id == 0) {
        fprintf(stderr, "[Skill] Invalid parameters (user_id: %u, skill_id: %u)\n",
                user_id, skill_id);
        return ERR_INVALID_PARAMETER;
    }

    g_skill_state.total_skills_used++;

    printf("[Skill] User %u used skill %u on target %u\n",
           user_id, skill_id, target_id);
    return ERR_SUCCESS;
}

/**
 * Get Skill Info
 */
int Skill_GetSkillInfo(unsigned int skill_id, SkillInfo* skill_info) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return ERR_NOT_INITIALIZED;
    }

    if (skill_id == 0 || skill_info == NULL) {
        fprintf(stderr, "[Skill] Invalid parameters\n");
        return ERR_INVALID_PARAMETER;
    }

    // Stub: Fill with dummy data
    memset(skill_info, 0, sizeof(SkillInfo));
    skill_info->skill_id = skill_id;
    skill_info->skill_template_id = skill_id % 10000;
    skill_info->user_id = skill_id / 10000;
    skill_info->skill_level = 1;
    skill_info->max_level = 20;
    skill_info->cooldown_remaining = 0;
    skill_info->sp_cost = 1;
    skill_info->skill_type = SKILL_TYPE_ACTIVE;
    skill_info->element = SKILL_ELEMENT_PHYSICAL;
    skill_info->is_learned = 1;
    skill_info->is_equipped = 1;

    return ERR_SUCCESS;
}

/**
 * Get User Skills
 */
int Skill_GetUserSkills(unsigned int user_id,
                        SkillInfo* skills,
                        int max_skills) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return 0;
    }

    if (user_id == 0 || skills == NULL || max_skills <= 0) {
        fprintf(stderr, "[Skill] Invalid parameters\n");
        return 0;
    }

    // Stub: Return 0 skills
    printf("[Skill] Retrieved skills for user %u (limit: %d)\n",
           user_id, max_skills);
    return 0;
}

/**
 * Reset Skills
 */
int Skill_ResetSkills(unsigned int user_id) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return -ERR_NOT_INITIALIZED;
    }

    if (user_id == 0) {
        fprintf(stderr, "[Skill] Invalid user_id: %u\n", user_id);
        return -ERR_INVALID_PARAMETER;
    }

    g_skill_state.total_skills_reset++;

    // Stub: Refund 100 SP
    int sp_refunded = 100;
    printf("[Skill] User %u reset skills, refunded %d SP\n",
           user_id, sp_refunded);

    return sp_refunded;
}

/**
 * Update Skill State
 */
int Skill_Update(int delta_time_ms) {
    if (!g_skill_state.initialized) {
        return 0;
    }

    if (delta_time_ms < 0) {
        fprintf(stderr, "[Skill] Invalid delta_time_ms: %d\n", delta_time_ms);
        return 0;
    }

    // Stub: Process cooldowns and state updates
    // Return number of skills updated (stub value)
    return 0;
}

/**
 * Get Skill Statistics
 */
int Skill_GetStatistics(unsigned int user_id, SkillStats* stats) {
    if (!g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module not initialized\n");
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || stats == NULL) {
        fprintf(stderr, "[Skill] Invalid parameters\n");
        return ERR_INVALID_PARAMETER;
    }

    // Stub: Return dummy statistics
    memset(stats, 0, sizeof(SkillStats));
    stats->total_skills_learned = 0;
    stats->total_skills_used = 0;
    stats->total_sp_spent = 0;
    stats->skills_reset = 0;

    return ERR_SUCCESS;
}
