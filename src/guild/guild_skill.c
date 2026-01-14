/**
 * @file guild_skill.c
 * @brief Guild Skill System Implementation
 *
 * Implements guild skill point management, skill learning,
 * and skill effect calculations.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/guild/guild_skill.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*==============================================================================
 * Global State
 *============================================================================*/

static GuildSkillState g_skill_state = {0};

/* In-memory storage for guild skills (up to 10,000 guilds) */
static GuildSkillData g_guild_skills[10000] = {0};
static int g_guild_skill_count = 0;

/*==============================================================================
 * Skill Configuration Table
 *
 * Defines all guild skills, their costs, and effects.
 *============================================================================*/

static const GuildSkillConfig SKILL_CONFIG_TABLE[] = {
    /* Member Benefits */
    {GUILD_SKILL_EXP_BOOST,     "Exp Boost",        10, 1, 0.05f},      /* +5% per level */
    {GUILD_SKILL_DROP_RATE,     "Drop Rate",        10, 1, 0.03f},      /* +3% per level */
    {GUILD_SKILL_GOLD_BOOST,    "Gold Boost",       10, 1, 0.05f},      /* +5% per level */

    /* Combat Bonuses */
    {GUILD_SKILL_ATK_BOOST,     "Attack Boost",     10, 2, 50.0f},      /* +50 per level */
    {GUILD_SKILL_DEF_BOOST,     "Defense Boost",    10, 2, 30.0f},      /* +30 per level */
    {GUILD_SKILL_HP_BOOST,      "HP Boost",         10, 2, 100.0f},     /* +100 per level */
    {GUILD_SKILL_MP_BOOST,      "MP Boost",         10, 2, 50.0f},      /* +50 per level */

    /* Guild Features */
    {GUILD_SKILL_MEMBER_LIMIT,  "Member Limit",     10, 3, 5.0f},       /* +5 members/lvl */
    {GUILD_SKILL_WAREHOUSE_SIZE,"Warehouse Size",   10, 2, 10.0f},      /* +10 slots/lvl */
    {GUILD_SKILL_RECALL,        "Guild Recall",     1,  5, 1.0f},       /* Binary skill */

    /* Special Skills */
    {GUILD_SKILL_PVP_DAMAGE,    "PvP Damage",       10, 3, 0.02f},      /* +2% per level */
    {GUILD_SKILL_PVE_DAMAGE,    "PvE Damage",       10, 3, 0.02f},      /* +2% per level */
    {GUILD_SKILL_CRIT_RATE,     "Crit Rate",        10, 2, 0.01f},      /* +1% per level */
    {GUILD_SKILL_MOVE_SPEED,    "Move Speed",       5,  2, 0.05f}       /* +5% per level */
};

static const int SKILL_CONFIG_COUNT = sizeof(SKILL_CONFIG_TABLE) / sizeof(SKILL_CONFIG_TABLE[0]);

/*==============================================================================
 * Internal Helper Functions
 *============================================================================*/

/**
 * Find guild skill data by ID
 */
static GuildSkillData* FindGuildSkillData(int guild_id)
{
    for (int i = 0; i < g_guild_skill_count; i++) {
        if (g_guild_skills[i].guild_id == guild_id) {
            return &g_guild_skills[i];
        }
    }
    return NULL;
}

/**
 * Get or create guild skill data
 */
static GuildSkillData* GetOrCreateGuildSkillData(int guild_id)
{
    GuildSkillData* data = FindGuildSkillData(guild_id);
    if (data) {
        return data;
    }

    /* Create new entry */
    if (g_guild_skill_count >= 10000) {
        return NULL; /* Storage full */
    }

    data = &g_guild_skills[g_guild_skill_count];
    memset(data, 0, sizeof(GuildSkillData));
    data->guild_id = guild_id;
    data->remain_sp = 0;
    data->used_sp = 0;
    data->skill_count = 0;

    g_guild_skill_count++;

    return data;
}

/**
 * Find skill configuration
 */
static const GuildSkillConfig* FindSkillConfig(GuildSkillType skill_type)
{
    for (int i = 0; i < SKILL_CONFIG_COUNT; i++) {
        if (SKILL_CONFIG_TABLE[i].skill_type == skill_type) {
            return &SKILL_CONFIG_TABLE[i];
        }
    }
    return NULL;
}

/**
 * Find learned skill in guild
 */
static LearnedSkill* FindLearnedSkill(GuildSkillData* data, GuildSkillType skill_type)
{
    for (int i = 0; i < data->skill_count; i++) {
        if (data->skills[i].skill_type == skill_type) {
            return &data->skills[i];
        }
    }
    return NULL;
}

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

int GuildSkill_Initialize(void)
{
    if (g_skill_state.initialized) {
        fprintf(stderr, "[GuildSkill] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[GuildSkill] Initializing...\n");

    memset(&g_skill_state, 0, sizeof(g_skill_state));
    memset(g_guild_skills, 0, sizeof(g_guild_skills));
    g_guild_skill_count = 0;

    /* Load skill configurations */
    memcpy(g_skill_state.skill_configs, SKILL_CONFIG_TABLE,
           sizeof(GuildSkillConfig) * SKILL_CONFIG_COUNT);
    g_skill_state.config_count = SKILL_CONFIG_COUNT;

    g_skill_state.initialized = 1;

    printf("[GuildSkill] Initialized with %d skill types\n", SKILL_CONFIG_COUNT);

    return ERR_SUCCESS;
}

void GuildSkill_Cleanup(void)
{
    if (!g_skill_state.initialized) {
        return;
    }

    printf("[GuildSkill] Cleanup...\n");
    printf("[GuildSkill] Statistics:\n");
    printf("  - Total Skills Learned: %u\n", g_skill_state.stats.total_skills_learned);
    printf("  - Total SP Used: %u\n", g_skill_state.stats.total_sp_used);
    printf("  - Total Skill Resets: %u\n", g_skill_state.stats.total_skill_resets);

    memset(&g_skill_state, 0, sizeof(g_skill_state));
    memset(g_guild_skills, 0, sizeof(g_guild_skills));
    g_guild_skill_count = 0;

    printf("[GuildSkill] Cleanup complete\n");
}

/*==============================================================================
 * Skill Point Management Functions
 *============================================================================*/

int GuildSkill_GetSkillPoints(int guild_id, int* out_remain_sp, int* out_used_sp)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildSkillData* data = FindGuildSkillData(guild_id);
    if (!data) {
        /* No skills learned yet, return 0/0 */
        if (out_remain_sp) *out_remain_sp = 0;
        if (out_used_sp) *out_used_sp = 0;
        return ERR_SUCCESS;
    }

    if (out_remain_sp) {
        *out_remain_sp = data->remain_sp;
    }

    if (out_used_sp) {
        *out_used_sp = data->used_sp;
    }

    return ERR_SUCCESS;
}

int GuildSkill_AddSkillPoints(int guild_id, int points)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (points <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    GuildSkillData* data = GetOrCreateGuildSkillData(guild_id);
    if (!data) {
        return ERR_OUT_OF_MEMORY;
    }

    data->remain_sp += points;

    printf("[GuildSkill] Added %d SP to guild %d (total: %d)\n",
           points, guild_id, data->remain_sp);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Skill Learning Functions
 *============================================================================*/

int GuildSkill_LearnSkill(int guild_id,
                          GuildSkillType skill_type,
                          int* remain_sp,
                          int* used_sp)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!remain_sp || !used_sp) {
        return ERR_INVALID_PARAMETER;
    }

    /* Validate skill type */
    const GuildSkillConfig* config = FindSkillConfig(skill_type);
    if (!config) {
        return ERR_INVALID_PARAMETER;
    }

    GuildSkillData* data = GetOrCreateGuildSkillData(guild_id);
    if (!data) {
        return ERR_OUT_OF_MEMORY;
    }

    /* Find or create learned skill */
    LearnedSkill* learned = FindLearnedSkill(data, skill_type);

    int current_level = 0;
    if (learned) {
        current_level = learned->skill_level;
    }

    /* Check if max level reached */
    if (current_level >= config->max_level) {
        fprintf(stderr, "[GuildSkill] Skill %d already at max level\n", skill_type);
        return ERR_LIMIT_REACHED;
    }

    /* Calculate SP cost */
    int sp_cost = config->sp_cost_per_level;

    /* Check if enough SP */
    if (data->remain_sp < sp_cost) {
        fprintf(stderr, "[GuildSkill] Not enough SP (need %d, have %d)\n",
                sp_cost, data->remain_sp);
        return ERR_INSUFFICIENT_RESOURCES;
    }

    /* Learn/upgrade skill */
    if (!learned) {
        /* Add new skill */
        if (data->skill_count >= GUILD_MAX_SKILLS) {
            return ERR_LIMIT_REACHED;
        }

        learned = &data->skills[data->skill_count];
        learned->skill_type = skill_type;
        learned->skill_level = 0;
        data->skill_count++;
    }

    /* Upgrade skill level */
    learned->skill_level++;

    /* Deduct SP */
    data->remain_sp -= sp_cost;
    data->used_sp += sp_cost;

    /* Update output parameters */
    *remain_sp = data->remain_sp;
    *used_sp = data->used_sp;

    g_skill_state.stats.total_skills_learned++;
    g_skill_state.stats.total_sp_used += sp_cost;

    printf("[GuildSkill] Guild %d learned %s (level %d -> %d, cost %d SP)\n",
           guild_id, config->skill_name, current_level, learned->skill_level, sp_cost);

    return ERR_SUCCESS;
}

int GuildSkill_GetSkillLevel(int guild_id, GuildSkillType skill_type)
{
    if (!g_skill_state.initialized) {
        return -1;
    }

    GuildSkillData* data = FindGuildSkillData(guild_id);
    if (!data) {
        return 0; /* No skills learned */
    }

    LearnedSkill* learned = FindLearnedSkill(data, skill_type);
    if (!learned) {
        return 0; /* Skill not learned */
    }

    return learned->skill_level;
}

float GuildSkill_GetSkillEffect(GuildSkillType skill_type, int skill_level)
{
    const GuildSkillConfig* config = FindSkillConfig(skill_type);
    if (!config) {
        return 0.0f;
    }

    if (skill_level < 0 || skill_level > config->max_level) {
        return 0.0f;
    }

    return config->effect_per_level * skill_level;
}

/*==============================================================================
 * Skill Reset Functions
 *============================================================================*/

int GuildSkill_ResetSkills(int guild_id, int* remain_sp, int* used_sp)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!remain_sp || !used_sp) {
        return ERR_INVALID_PARAMETER;
    }

    GuildSkillData* data = FindGuildSkillData(guild_id);
    if (!data) {
        /* No skills to reset */
        return ERR_SUCCESS;
    }

    /* Refund all used SP */
    data->remain_sp += data->used_sp;
    data->used_sp = 0;

    /* Clear all learned skills */
    memset(data->skills, 0, sizeof(data->skills));
    data->skill_count = 0;

    /* Update output parameters */
    *remain_sp = data->remain_sp;
    *used_sp = data->used_sp;

    g_skill_state.stats.total_skill_resets++;

    printf("[GuildSkill] Guild %d reset all skills (refunded %d SP)\n",
           guild_id, data->remain_sp);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int GuildSkill_GetLearnedSkills(int guild_id,
                                LearnedSkill* skills,
                                int max_skills)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!skills || max_skills <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    GuildSkillData* data = FindGuildSkillData(guild_id);
    if (!data) {
        return 0; /* No skills learned */
    }

    int count = (data->skill_count < max_skills) ? data->skill_count : max_skills;

    memcpy(skills, data->skills, count * sizeof(LearnedSkill));

    return count;
}

int GuildSkill_GetSkillCost(GuildSkillType skill_type, int current_level)
{
    const GuildSkillConfig* config = FindSkillConfig(skill_type);
    if (!config) {
        return -1;
    }

    if (current_level >= config->max_level) {
        return -1; /* Max level reached */
    }

    return config->sp_cost_per_level;
}

const char* GuildSkill_GetSkillName(GuildSkillType skill_type)
{
    const GuildSkillConfig* config = FindSkillConfig(skill_type);
    if (!config) {
        return "Unknown";
    }

    return config->skill_name;
}

int GuildSkill_GetStatistics(unsigned int* total_skills_learned,
                              unsigned int* total_sp_used,
                              unsigned int* total_resets)
{
    if (!g_skill_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (total_skills_learned) {
        *total_skills_learned = g_skill_state.stats.total_skills_learned;
    }

    if (total_sp_used) {
        *total_sp_used = g_skill_state.stats.total_sp_used;
    }

    if (total_resets) {
        *total_resets = g_skill_state.stats.total_skill_resets;
    }

    return ERR_SUCCESS;
}
