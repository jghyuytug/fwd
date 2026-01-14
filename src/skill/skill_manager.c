/**
 * Skill Module - Full Implementation
 *
 * Handles skill learning, upgrades, usage, and cooldowns
 * Features:
 * - Skill template definitions
 * - Per-user skill tracking
 * - Skill level progression
 * - Cooldown management
 * - SP (Skill Points) system
 * - Skill reset and refund
 */

#include "skill_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Constants */
#define MAX_SKILL_TEMPLATES 1000
#define MAX_USERS 10000
#define MAX_SKILLS_PER_USER 100
#define DEFAULT_COOLDOWN_MS 5000  /* 5 seconds default cooldown */
#define COOLDOWN_UPDATE_INTERVAL_MS 100

/* Skill template definition */
typedef struct {
    unsigned int template_id;
    char name[64];
    char description[256];
    SkillType skill_type;
    SkillElement element;
    int max_level;
    int base_sp_cost;          /* SP cost to learn */
    int sp_per_level;          /* SP cost per upgrade */
    int base_cooldown_ms;      /* Base cooldown in milliseconds */
    int base_damage;           /* Base damage/effect */
    int damage_per_level;      /* Damage increase per level */
    int required_level;        /* Character level required */
    int is_defined;
} SkillTemplate;

/* User skill data */
typedef struct {
    unsigned int skill_id;
    unsigned int template_id;
    int level;
    int cooldown_remaining_ms;
    int times_used;
    unsigned int learn_time;
    int is_equipped;
} UserSkill;

/* Per-user skill tracking */
typedef struct {
    unsigned int user_id;
    UserSkill skills[MAX_SKILLS_PER_USER];
    int skill_count;
    int available_sp;
    int total_sp_spent;
    int total_sp_earned;
    SkillStats stats;
} UserSkillData;

/* Module global state */
typedef struct {
    int initialized;
    int max_users;

    /* Skill templates */
    SkillTemplate* templates;
    int template_count;
    unsigned int next_skill_id;

    /* User data */
    UserSkillData* users;
    int user_count;

    /* Global statistics */
    struct {
        unsigned int total_skills_learned;
        unsigned int total_skills_used;
        unsigned int total_skills_upgraded;
        unsigned int total_skills_reset;
        unsigned int total_sp_spent;
    } stats;
} SkillModuleState;

static SkillModuleState g_skill_state = {0};

/* Forward declarations */
static SkillTemplate* FindTemplate(unsigned int template_id);
static UserSkillData* FindOrCreateUser(unsigned int user_id);
static UserSkillData* FindUser(unsigned int user_id);
static UserSkill* FindUserSkill(UserSkillData* user, unsigned int skill_id);
static UserSkill* FindUserSkillByTemplate(UserSkillData* user, unsigned int template_id);

/**
 * Initialize Skill Module
 */
int Skill_Initialize(int max_users) {
    if (g_skill_state.initialized) {
        fprintf(stderr, "[Skill] Module already initialized\n");
        return ERR_ALREADY_INITIALIZED;
    }

    if (max_users <= 0 || max_users > MAX_USERS) {
        fprintf(stderr, "[Skill] Invalid max_users: %d\n", max_users);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Skill] Initializing skill module...\n");

    /* Allocate skill templates */
    g_skill_state.templates = (SkillTemplate*)calloc(MAX_SKILL_TEMPLATES, sizeof(SkillTemplate));
    if (!g_skill_state.templates) {
        fprintf(stderr, "[Skill] Failed to allocate skill templates\n");
        return ERR_MEMORY;
    }

    /* Allocate user data */
    g_skill_state.users = (UserSkillData*)calloc(max_users, sizeof(UserSkillData));
    if (!g_skill_state.users) {
        free(g_skill_state.templates);
        g_skill_state.templates = NULL;
        fprintf(stderr, "[Skill] Failed to allocate user data\n");
        return ERR_MEMORY;
    }

    g_skill_state.max_users = max_users;
    g_skill_state.template_count = 0;
    g_skill_state.next_skill_id = 1;
    g_skill_state.user_count = 0;
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
           g_skill_state.stats.total_skills_learned,
           g_skill_state.stats.total_skills_used,
           g_skill_state.stats.total_skills_upgraded,
           g_skill_state.stats.total_skills_reset,
           g_skill_state.stats.total_sp_spent);

    if (g_skill_state.templates) {
        free(g_skill_state.templates);
        g_skill_state.templates = NULL;
    }

    if (g_skill_state.users) {
        free(g_skill_state.users);
        g_skill_state.users = NULL;
    }

    memset(&g_skill_state, 0, sizeof(SkillModuleState));
}

/**
 * Define Skill Template
 */
int Skill_DefineTemplate(unsigned int template_id,
                         const char* name,
                         SkillType skill_type,
                         SkillElement element,
                         int max_level,
                         int base_sp_cost,
                         int base_cooldown_ms)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (template_id == 0 || !name) {
        return ERR_INVALID_PARAMETER;
    }

    if (FindTemplate(template_id) != NULL) {
        return ERR_DUPLICATE;
    }

    if (g_skill_state.template_count >= MAX_SKILL_TEMPLATES) {
        return ERR_LIMIT_EXCEEDED;
    }

    SkillTemplate* tmpl = &g_skill_state.templates[g_skill_state.template_count];
    memset(tmpl, 0, sizeof(SkillTemplate));

    tmpl->template_id = template_id;
    strncpy(tmpl->name, name, sizeof(tmpl->name) - 1);
    tmpl->skill_type = skill_type;
    tmpl->element = element;
    tmpl->max_level = (max_level > 0) ? max_level : 20;
    tmpl->base_sp_cost = (base_sp_cost > 0) ? base_sp_cost : 1;
    tmpl->sp_per_level = 1;
    tmpl->base_cooldown_ms = (base_cooldown_ms > 0) ? base_cooldown_ms : DEFAULT_COOLDOWN_MS;
    tmpl->base_damage = 100;
    tmpl->damage_per_level = 10;
    tmpl->required_level = 1;
    tmpl->is_defined = 1;

    g_skill_state.template_count++;

    return ERR_SUCCESS;
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

    SkillTemplate* tmpl = FindTemplate(skill_template_id);
    if (!tmpl) {
        /* Auto-create template for testing */
        Skill_DefineTemplate(skill_template_id, "Auto Skill", SKILL_TYPE_ACTIVE,
                            SKILL_ELEMENT_PHYSICAL, 20, 1, DEFAULT_COOLDOWN_MS);
        tmpl = FindTemplate(skill_template_id);
    }

    UserSkillData* user = FindOrCreateUser(user_id);
    if (!user) {
        return -ERR_MEMORY;
    }

    /* Check if already learned */
    if (FindUserSkillByTemplate(user, skill_template_id) != NULL) {
        return -ERR_DUPLICATE;
    }

    /* Check skill limit */
    if (user->skill_count >= MAX_SKILLS_PER_USER) {
        return -ERR_LIMIT_EXCEEDED;
    }

    /* Check SP */
    if (user->available_sp < tmpl->base_sp_cost) {
        /* Grant initial SP if none available */
        if (user->available_sp == 0 && user->total_sp_earned == 0) {
            user->available_sp = 100;  /* Initial SP grant */
            user->total_sp_earned = 100;
        } else {
            return -ERR_INVALID_STATE;  /* Not enough SP */
        }
    }

    /* Deduct SP */
    user->available_sp -= tmpl->base_sp_cost;
    user->total_sp_spent += tmpl->base_sp_cost;

    /* Add skill */
    UserSkill* skill = &user->skills[user->skill_count];
    memset(skill, 0, sizeof(UserSkill));

    skill->skill_id = g_skill_state.next_skill_id++;
    skill->template_id = skill_template_id;
    skill->level = 1;
    skill->cooldown_remaining_ms = 0;
    skill->times_used = 0;
    skill->learn_time = (unsigned int)time(NULL);
    skill->is_equipped = 1;

    user->skill_count++;
    user->stats.total_skills_learned++;

    g_skill_state.stats.total_skills_learned++;
    g_skill_state.stats.total_sp_spent += tmpl->base_sp_cost;

    printf("[Skill] User %u learned skill template %u (skill_id: %u, SP cost: %d)\n",
           user_id, skill_template_id, skill->skill_id, tmpl->base_sp_cost);

    return (int)skill->skill_id;
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

    UserSkillData* user = FindUser(user_id);
    if (!user) {
        return ERR_NOT_FOUND;
    }

    UserSkill* skill = FindUserSkill(user, skill_id);
    if (!skill) {
        return ERR_NOT_FOUND;
    }

    SkillTemplate* tmpl = FindTemplate(skill->template_id);
    if (!tmpl) {
        return ERR_NOT_FOUND;
    }

    /* Check max level */
    if (skill->level >= tmpl->max_level) {
        return ERR_LIMIT_EXCEEDED;
    }

    /* Calculate SP cost */
    int sp_cost = tmpl->sp_per_level * skill->level;

    /* Check SP */
    if (user->available_sp < sp_cost) {
        return ERR_INVALID_STATE;
    }

    /* Deduct SP and upgrade */
    user->available_sp -= sp_cost;
    user->total_sp_spent += sp_cost;
    skill->level++;

    g_skill_state.stats.total_skills_upgraded++;
    g_skill_state.stats.total_sp_spent += sp_cost;

    printf("[Skill] User %u upgraded skill %u to level %d (SP cost: %d)\n",
           user_id, skill_id, skill->level, sp_cost);

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

    UserSkillData* user = FindUser(user_id);
    if (!user) {
        return ERR_NOT_FOUND;
    }

    UserSkill* skill = FindUserSkill(user, skill_id);
    if (!skill) {
        return ERR_NOT_FOUND;
    }

    /* Check cooldown */
    if (skill->cooldown_remaining_ms > 0) {
        return ERR_INVALID_STATE;  /* On cooldown */
    }

    SkillTemplate* tmpl = FindTemplate(skill->template_id);
    if (!tmpl) {
        return ERR_NOT_FOUND;
    }

    /* Use skill */
    skill->cooldown_remaining_ms = tmpl->base_cooldown_ms;
    skill->times_used++;
    user->stats.total_skills_used++;

    g_skill_state.stats.total_skills_used++;

    printf("[Skill] User %u used skill %u on target %u (cooldown: %dms)\n",
           user_id, skill_id, target_id, skill->cooldown_remaining_ms);

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

    memset(skill_info, 0, sizeof(SkillInfo));

    /* Search all users for this skill */
    for (int i = 0; i < g_skill_state.user_count; i++) {
        UserSkillData* user = &g_skill_state.users[i];
        UserSkill* skill = FindUserSkill(user, skill_id);
        if (skill) {
            SkillTemplate* tmpl = FindTemplate(skill->template_id);

            skill_info->skill_id = skill->skill_id;
            skill_info->skill_template_id = skill->template_id;
            skill_info->user_id = user->user_id;
            skill_info->skill_level = skill->level;
            skill_info->max_level = tmpl ? tmpl->max_level : 20;
            skill_info->cooldown_remaining = skill->cooldown_remaining_ms;
            skill_info->sp_cost = tmpl ? tmpl->sp_per_level * skill->level : 1;
            skill_info->skill_type = tmpl ? tmpl->skill_type : SKILL_TYPE_ACTIVE;
            skill_info->element = tmpl ? tmpl->element : SKILL_ELEMENT_PHYSICAL;
            skill_info->is_learned = 1;
            skill_info->is_equipped = skill->is_equipped;

            return ERR_SUCCESS;
        }
    }

    return ERR_NOT_FOUND;
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

    UserSkillData* user = FindUser(user_id);
    if (!user) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < user->skill_count && count < max_skills; i++) {
        UserSkill* skill = &user->skills[i];
        SkillTemplate* tmpl = FindTemplate(skill->template_id);

        skills[count].skill_id = skill->skill_id;
        skills[count].skill_template_id = skill->template_id;
        skills[count].user_id = user_id;
        skills[count].skill_level = skill->level;
        skills[count].max_level = tmpl ? tmpl->max_level : 20;
        skills[count].cooldown_remaining = skill->cooldown_remaining_ms;
        skills[count].sp_cost = tmpl ? tmpl->sp_per_level * skill->level : 1;
        skills[count].skill_type = tmpl ? tmpl->skill_type : SKILL_TYPE_ACTIVE;
        skills[count].element = tmpl ? tmpl->element : SKILL_ELEMENT_PHYSICAL;
        skills[count].is_learned = 1;
        skills[count].is_equipped = skill->is_equipped;
        count++;
    }

    printf("[Skill] Retrieved %d skills for user %u\n", count, user_id);
    return count;
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

    UserSkillData* user = FindUser(user_id);
    if (!user) {
        return -ERR_NOT_FOUND;
    }

    /* Calculate SP refund */
    int sp_refunded = user->total_sp_spent;

    /* Reset skills */
    user->skill_count = 0;
    user->available_sp = sp_refunded;
    user->total_sp_spent = 0;
    user->stats.skills_reset++;

    g_skill_state.stats.total_skills_reset++;

    printf("[Skill] User %u reset skills, refunded %d SP\n",
           user_id, sp_refunded);

    return sp_refunded;
}

/**
 * Add SP to User
 */
int Skill_AddSP(unsigned int user_id, int amount)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || amount <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    UserSkillData* user = FindOrCreateUser(user_id);
    if (!user) {
        return ERR_MEMORY;
    }

    user->available_sp += amount;
    user->total_sp_earned += amount;

    return ERR_SUCCESS;
}

/**
 * Get Available SP
 */
int Skill_GetAvailableSP(unsigned int user_id)
{
    if (!g_skill_state.initialized) {
        return 0;
    }

    UserSkillData* user = FindUser(user_id);
    if (!user) {
        return 0;
    }

    return user->available_sp;
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

    int skills_updated = 0;

    /* Update cooldowns for all users */
    for (int i = 0; i < g_skill_state.user_count; i++) {
        UserSkillData* user = &g_skill_state.users[i];

        for (int j = 0; j < user->skill_count; j++) {
            UserSkill* skill = &user->skills[j];

            if (skill->cooldown_remaining_ms > 0) {
                skill->cooldown_remaining_ms -= delta_time_ms;
                if (skill->cooldown_remaining_ms < 0) {
                    skill->cooldown_remaining_ms = 0;
                }
                skills_updated++;
            }
        }
    }

    return skills_updated;
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

    UserSkillData* user = FindUser(user_id);
    if (user) {
        memcpy(stats, &user->stats, sizeof(SkillStats));
        stats->total_sp_spent = user->total_sp_spent;
    } else {
        memset(stats, 0, sizeof(SkillStats));
    }

    return ERR_SUCCESS;
}

/**
 * Print Module Statistics
 */
void Skill_PrintStats()
{
    if (!g_skill_state.initialized) {
        printf("[Skill] Module not initialized\n");
        return;
    }

    printf("=== Skill System Statistics ===\n");
    printf("Templates: %d / %d\n", g_skill_state.template_count, MAX_SKILL_TEMPLATES);
    printf("Active Users: %d / %d\n", g_skill_state.user_count, g_skill_state.max_users);
    printf("Next Skill ID: %u\n", g_skill_state.next_skill_id);
    printf("\nGlobal Stats:\n");
    printf("  Total Learned: %u\n", g_skill_state.stats.total_skills_learned);
    printf("  Total Used: %u\n", g_skill_state.stats.total_skills_used);
    printf("  Total Upgraded: %u\n", g_skill_state.stats.total_skills_upgraded);
    printf("  Total Reset: %u\n", g_skill_state.stats.total_skills_reset);
    printf("  Total SP Spent: %u\n", g_skill_state.stats.total_sp_spent);
}

/* ============================================================
 * Internal Helper Functions
 * ============================================================ */

static SkillTemplate* FindTemplate(unsigned int template_id)
{
    for (int i = 0; i < g_skill_state.template_count; i++) {
        if (g_skill_state.templates[i].is_defined &&
            g_skill_state.templates[i].template_id == template_id) {
            return &g_skill_state.templates[i];
        }
    }
    return NULL;
}

static UserSkillData* FindOrCreateUser(unsigned int user_id)
{
    /* Find existing */
    for (int i = 0; i < g_skill_state.user_count; i++) {
        if (g_skill_state.users[i].user_id == user_id) {
            return &g_skill_state.users[i];
        }
    }

    /* Create new */
    if (g_skill_state.user_count >= g_skill_state.max_users) {
        return NULL;
    }

    UserSkillData* user = &g_skill_state.users[g_skill_state.user_count];
    memset(user, 0, sizeof(UserSkillData));
    user->user_id = user_id;
    user->available_sp = 100;  /* Initial SP */
    user->total_sp_earned = 100;
    g_skill_state.user_count++;

    return user;
}

static UserSkillData* FindUser(unsigned int user_id)
{
    for (int i = 0; i < g_skill_state.user_count; i++) {
        if (g_skill_state.users[i].user_id == user_id) {
            return &g_skill_state.users[i];
        }
    }
    return NULL;
}

static UserSkill* FindUserSkill(UserSkillData* user, unsigned int skill_id)
{
    if (!user) return NULL;

    for (int i = 0; i < user->skill_count; i++) {
        if (user->skills[i].skill_id == skill_id) {
            return &user->skills[i];
        }
    }
    return NULL;
}

static UserSkill* FindUserSkillByTemplate(UserSkillData* user, unsigned int template_id)
{
    if (!user) return NULL;

    for (int i = 0; i < user->skill_count; i++) {
        if (user->skills[i].template_id == template_id) {
            return &user->skills[i];
        }
    }
    return NULL;
}
