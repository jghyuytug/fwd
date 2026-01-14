/**
 * Quest Configuration Module - Implementation
 * Purpose: Load and manage quest templates, rewards, and prerequisites
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 * NOTE: Does NOT include game_config.h to avoid naming conflict
 */

#include "../../include/quest/quest_config.h"
#include "../orm/core_game/taiwan_cain/headers/new_charac_quest_orm.h"
#include "../orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Global state */
static struct {
    int initialized;
    QuestTemplate* templates;
    int template_count;
    int max_templates;
} g_config_state = {0};

/**
 * Initialize quest configuration system
 */
int QuestConfig_Initialize(void)
{
    if (g_config_state.initialized) {
        printf("[QuestConfig] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[QuestConfig] Initializing quest configuration system...\n");

    /* Allocate template cache (max 1000 templates) */
    g_config_state.max_templates = 1000;
    g_config_state.templates = (QuestTemplate*)malloc(
        sizeof(QuestTemplate) * g_config_state.max_templates
    );

    if (!g_config_state.templates) {
        printf("[QuestConfig] ERROR: Failed to allocate template cache\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    g_config_state.template_count = 0;
    g_config_state.initialized = 1;

    printf("[QuestConfig] Configuration system initialized (max %d templates)\n",
           g_config_state.max_templates);

    return ERR_SUCCESS;
}

/**
 * Load quest template from database
 *
 * NOTE: In a real implementation, this would query a quest_template table.
 * For now, we create hardcoded templates as examples.
 */
int QuestConfig_LoadTemplate(unsigned int quest_id, QuestTemplate* out_template)
{
    if (!g_config_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_template) {
        return ERR_INVALID_PARAMETER;
    }

    /* Clear output */
    memset(out_template, 0, sizeof(QuestTemplate));

    /* Example hardcoded templates */
    if (quest_id == 1001) {
        /* Quest 1001: Kill 10 Goblins */
        out_template->quest_id = 1001;
        snprintf(out_template->quest_name, sizeof(out_template->quest_name),
                 "Goblin Extermination");
        snprintf(out_template->quest_description, sizeof(out_template->quest_description),
                 "Kill 10 Goblins in the forest");
        out_template->quest_type = QUEST_TYPE_MAIN;
        out_template->duration_minutes = 0;  /* No time limit */

        /* Objective: Kill 10 Goblins (monster ID 101) */
        out_template->objective_count = 1;
        out_template->objective_types[0] = OBJECTIVE_KILL_MONSTER;
        out_template->objective_targets[0] = 101;  /* Goblin monster ID */
        out_template->objective_counts[0] = 10;

        /* Reward */
        out_template->reward.exp = 1000;
        out_template->reward.gold = 500;
        out_template->reward.skill_points = 1;
        out_template->reward.item_count = 1;
        out_template->reward.item_ids[0] = 5001;  /* Health Potion */
        out_template->reward.item_counts[0] = 5;

        /* Prerequisites */
        out_template->prerequisite.min_level = 5;
        out_template->prerequisite.max_level = 0;  /* No max level */
        out_template->prerequisite.required_quest_id = 0;  /* No prerequisite quest */

        return ERR_SUCCESS;

    } else if (quest_id == 1002) {
        /* Quest 1002: Collect 5 Herbs */
        out_template->quest_id = 1002;
        snprintf(out_template->quest_name, sizeof(out_template->quest_name),
                 "Herb Gathering");
        snprintf(out_template->quest_description, sizeof(out_template->quest_description),
                 "Collect 5 medicinal herbs");
        out_template->quest_type = QUEST_TYPE_SIDE;
        out_template->duration_minutes = 0;

        /* Objective: Collect 5 Herbs (item ID 2001) */
        out_template->objective_count = 1;
        out_template->objective_types[0] = OBJECTIVE_COLLECT_ITEM;
        out_template->objective_targets[0] = 2001;  /* Herb item ID */
        out_template->objective_counts[0] = 5;

        /* Reward */
        out_template->reward.exp = 500;
        out_template->reward.gold = 300;
        out_template->reward.skill_points = 0;
        out_template->reward.item_count = 0;

        /* Prerequisites */
        out_template->prerequisite.min_level = 3;
        out_template->prerequisite.max_level = 0;

        return ERR_SUCCESS;

    } else if (quest_id == 1003) {
        /* Quest 1003: Daily Quest - Clear Dungeon */
        out_template->quest_id = 1003;
        snprintf(out_template->quest_name, sizeof(out_template->quest_name),
                 "Daily Dungeon Challenge");
        snprintf(out_template->quest_description, sizeof(out_template->quest_description),
                 "Complete any dungeon once");
        out_template->quest_type = QUEST_TYPE_DAILY;
        out_template->duration_minutes = 1440;  /* 24 hours */

        /* Objective: Complete 1 dungeon */
        out_template->objective_count = 1;
        out_template->objective_types[0] = OBJECTIVE_COMPLETE_DUNGEON;
        out_template->objective_targets[0] = 0;  /* Any dungeon */
        out_template->objective_counts[0] = 1;

        /* Reward */
        out_template->reward.exp = 2000;
        out_template->reward.gold = 1000;
        out_template->reward.skill_points = 2;
        out_template->reward.item_count = 0;

        /* Prerequisites */
        out_template->prerequisite.min_level = 10;
        out_template->prerequisite.max_level = 0;

        return ERR_SUCCESS;
    }

    /* Quest template not found */
    printf("[QuestConfig] Quest template %u not found\n", quest_id);
    return ERR_QUEST_NOT_FOUND;
}

/**
 * Load all quest templates
 */
int QuestConfig_LoadAllTemplates(QuestTemplate* templates, int max_count)
{
    if (!g_config_state.initialized) {
        return 0;
    }

    if (!templates || max_count <= 0) {
        return 0;
    }

    /* Load hardcoded example templates */
    int loaded = 0;
    unsigned int quest_ids[] = {1001, 1002, 1003};
    int template_count = sizeof(quest_ids) / sizeof(quest_ids[0]);

    for (int i = 0; i < template_count && loaded < max_count; i++) {
        if (QuestConfig_LoadTemplate(quest_ids[i], &templates[loaded]) == ERR_SUCCESS) {
            loaded++;
        }
    }

    printf("[QuestConfig] Loaded %d quest templates\n", loaded);
    return loaded;
}

/**
 * Check if player meets quest prerequisites
 */
int QuestConfig_CheckPrerequisites(int character_id, unsigned int quest_id)
{
    if (!g_config_state.initialized) {
        return 0;
    }

    /* Load quest template */
    QuestTemplate template;
    if (QuestConfig_LoadTemplate(quest_id, &template) != ERR_SUCCESS) {
        return 0;
    }

    /*
     * Prerequisite checking: Query character data via ORM
     *
     * Production implementation should verify:
     * - CharacInfo level >= template.min_level
     * - CharacQuest has completed all prerequisite_quest_ids
     * - CharacInfo job matches template.job_requirement (if any)
     *
     * Example:
     *   CharacInfo char_info;
     *   if (CharacInfo_GetByCharacNo(g_db_manager, character_id, &char_info) == 0) {
     *       if (char_info.lev < template.min_level) return 0;
     *   }
     *
     * For now, assume prerequisites are met (stub implementation).
     */
    (void)character_id;

    printf("[QuestConfig] Prerequisites check for quest %u: PASSED (stub)\n", quest_id);
    return 1;
}

/**
 * Get quest reward information
 */
int QuestConfig_GetReward(unsigned int quest_id, QuestReward* out_reward)
{
    if (!g_config_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_reward) {
        return ERR_INVALID_PARAMETER;
    }

    /* Load quest template */
    QuestTemplate template;
    if (QuestConfig_LoadTemplate(quest_id, &template) != ERR_SUCCESS) {
        return ERR_QUEST_NOT_FOUND;
    }

    /* Copy reward data */
    memcpy(out_reward, &template.reward, sizeof(QuestReward));

    /* NOTE: Reward multipliers are applied in quest_manager.c to avoid naming conflict */

    return ERR_SUCCESS;
}

/**
 * Cleanup quest configuration system
 */
void QuestConfig_Cleanup(void)
{
    if (!g_config_state.initialized) {
        return;
    }

    printf("[QuestConfig] Cleaning up quest configuration system...\n");

    if (g_config_state.templates) {
        free(g_config_state.templates);
        g_config_state.templates = NULL;
    }

    g_config_state.template_count = 0;
    g_config_state.max_templates = 0;
    g_config_state.initialized = 0;

    printf("[QuestConfig] Cleanup complete\n");
}
