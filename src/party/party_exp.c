/**
 * Party Experience Distribution System - Implementation
 * Purpose: Calculate and distribute experience among party members
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 */

#include "../../include/party/party_exp.h"
#include "../../include/party/party_manager.h"
#include "../../include/config/game_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Maximum damage contributions to track */
#define MAX_DAMAGE_CONTRIBUTIONS 1000

/* Damage contribution entry */
typedef struct {
    int character_id;
    int damage;
    int active;
} DamageContribution;

/* Global state */
static struct {
    int initialized;

    /* Damage tracking (for damage-based distribution) */
    DamageContribution* damage_contributions;
    int contribution_count;

    /* Statistics */
    struct {
        unsigned long long total_exp_distributed;
        unsigned int distributions;
        unsigned int average_distributions;
        unsigned int damage_distributions;
        unsigned int level_distributions;
    } stats;
} g_exp_state = {0};

/**
 * Initialize party experience system
 */
int PartyExp_Initialize(void)
{
    if (g_exp_state.initialized) {
        printf("[PartyExp] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[PartyExp] Initializing party experience system...\n");

    /* Allocate damage contribution tracking */
    g_exp_state.damage_contributions = (DamageContribution*)malloc(
        sizeof(DamageContribution) * MAX_DAMAGE_CONTRIBUTIONS
    );

    if (!g_exp_state.damage_contributions) {
        printf("[PartyExp] ERROR: Failed to allocate damage tracking\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    memset(g_exp_state.damage_contributions, 0, sizeof(DamageContribution) * MAX_DAMAGE_CONTRIBUTIONS);
    g_exp_state.contribution_count = 0;
    g_exp_state.initialized = 1;

    printf("[PartyExp] Experience system initialized\n");

    return ERR_SUCCESS;
}

/**
 * Get party experience bonus based on member count
 * ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded values
 * Returns bonus multiplier as increment (e.g., 0.10 for +10%)
 */
float PartyExp_GetPartyBonus(int member_count)
{
    const PartyBonusConfig* config = PartyConfig_GetBonus();

    switch (member_count) {
        case 2:
            return config->bonus_2_members - 1.0f;  /* Config: 1.1 → returns 0.10 */
        case 3:
            return config->bonus_3_members - 1.0f;  /* Config: 1.2 → returns 0.20 */
        case 4:
            return config->bonus_4_members - 1.0f;  /* Config: 1.3 → returns 0.30 */
        default:
            return 0.0f;   /* No bonus for 1 or 5+ members */
    }
}

/**
 * Calculate level difference penalty
 */
float PartyExp_GetLevelPenalty(int member_level, int monster_level)
{
    int level_diff = abs(member_level - monster_level);

    if (level_diff <= 5) {
        return 1.0f;  /* No penalty */
    } else if (level_diff <= 10) {
        return 0.75f;  /* -25% */
    } else {
        return 0.50f;  /* -50% for large level difference */
    }
}

/**
 * Set member damage contribution
 */
int PartyExp_SetMemberDamage(int character_id, int damage)
{
    if (!g_exp_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find existing contribution */
    for (int i = 0; i < g_exp_state.contribution_count; i++) {
        if (g_exp_state.damage_contributions[i].active &&
            g_exp_state.damage_contributions[i].character_id == character_id) {
            g_exp_state.damage_contributions[i].damage += damage;
            return ERR_SUCCESS;
        }
    }

    /* Add new contribution */
    if (g_exp_state.contribution_count >= MAX_DAMAGE_CONTRIBUTIONS) {
        printf("[PartyExp] WARNING: Damage contribution storage full\n");
        return ERR_STORAGE_FULL;
    }

    DamageContribution* contrib = &g_exp_state.damage_contributions[g_exp_state.contribution_count];
    contrib->character_id = character_id;
    contrib->damage = damage;
    contrib->active = 1;

    g_exp_state.contribution_count++;

    return ERR_SUCCESS;
}

/**
 * Clear damage contributions
 */
int PartyExp_ClearDamageContributions(void)
{
    if (!g_exp_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    memset(g_exp_state.damage_contributions, 0, sizeof(DamageContribution) * MAX_DAMAGE_CONTRIBUTIONS);
    g_exp_state.contribution_count = 0;

    return ERR_SUCCESS;
}

/**
 * Get damage contribution for character
 */
static int GetDamageContribution(int character_id)
{
    for (int i = 0; i < g_exp_state.contribution_count; i++) {
        if (g_exp_state.damage_contributions[i].active &&
            g_exp_state.damage_contributions[i].character_id == character_id) {
            return g_exp_state.damage_contributions[i].damage;
        }
    }
    return 0;
}

/**
 * Calculate total damage from all contributions
 */
static int GetTotalDamage(void)
{
    int total = 0;
    for (int i = 0; i < g_exp_state.contribution_count; i++) {
        if (g_exp_state.damage_contributions[i].active) {
            total += g_exp_state.damage_contributions[i].damage;
        }
    }
    return total;
}

/**
 * Calculate experience distribution for party members
 */
int PartyExp_Calculate(int party_id, int base_exp, ExpDistributeType distribute_type,
                       MemberExpShare* out_shares, int max_shares)
{
    if (!g_exp_state.initialized) {
        return 0;
    }

    if (!out_shares || max_shares <= 0) {
        return 0;
    }

    /* Get party info */
    PartyInfo party;
    if (PartyManager_GetParty(party_id, &party) != ERR_SUCCESS) {
        return 0;
    }

    /* Calculate party bonus */
    float party_bonus = PartyExp_GetPartyBonus(party.member_count);
    int bonus_exp = (int)(base_exp * party_bonus);
    int total_exp = base_exp + bonus_exp;

    int share_count = 0;

    switch (distribute_type) {
        case EXP_DISTRIBUTE_AVERAGE: {
            /* Average distribution */
            int exp_per_member = total_exp / party.member_count;

            for (int i = 0; i < party.member_count && share_count < max_shares; i++) {
                out_shares[share_count].character_id = party.members[i].character_id;
                out_shares[share_count].exp_amount = exp_per_member;
                out_shares[share_count].share_percentage = 1.0f / party.member_count;
                share_count++;
            }

            g_exp_state.stats.average_distributions++;
            break;
        }

        case EXP_DISTRIBUTE_DAMAGE: {
            /* Damage-based distribution */
            int total_damage = GetTotalDamage();

            if (total_damage == 0) {
                /* No damage data, fall back to average */
                int exp_per_member = total_exp / party.member_count;

                for (int i = 0; i < party.member_count && share_count < max_shares; i++) {
                    out_shares[share_count].character_id = party.members[i].character_id;
                    out_shares[share_count].exp_amount = exp_per_member;
                    out_shares[share_count].share_percentage = 1.0f / party.member_count;
                    share_count++;
                }
            } else {
                /* Distribute based on damage contribution */
                for (int i = 0; i < party.member_count && share_count < max_shares; i++) {
                    int character_id = party.members[i].character_id;
                    int damage = GetDamageContribution(character_id);

                    float share = (float)damage / (float)total_damage;
                    int exp_amount = (int)(total_exp * share);

                    out_shares[share_count].character_id = character_id;
                    out_shares[share_count].exp_amount = exp_amount;
                    out_shares[share_count].share_percentage = share;
                    share_count++;
                }
            }

            g_exp_state.stats.damage_distributions++;
            break;
        }

        case EXP_DISTRIBUTE_LEVEL: {
            /* Level-adjusted distribution */
            /* For simplicity, assuming content level = average party level */
            int avg_level = 0;
            for (int i = 0; i < party.member_count; i++) {
                avg_level += party.members[i].level;
            }
            avg_level /= party.member_count;

            /* Calculate total weight with level penalties */
            float total_weight = 0.0f;
            float weights[PARTY_MAX_MEMBERS] = {0};

            for (int i = 0; i < party.member_count; i++) {
                float penalty = PartyExp_GetLevelPenalty(party.members[i].level, avg_level);
                weights[i] = penalty;
                total_weight += penalty;
            }

            /* Distribute based on weighted shares */
            for (int i = 0; i < party.member_count && share_count < max_shares; i++) {
                float share = weights[i] / total_weight;
                int exp_amount = (int)(total_exp * share);

                out_shares[share_count].character_id = party.members[i].character_id;
                out_shares[share_count].exp_amount = exp_amount;
                out_shares[share_count].share_percentage = share;
                share_count++;
            }

            g_exp_state.stats.level_distributions++;
            break;
        }

        default:
            return 0;
    }

    return share_count;
}

/**
 * Distribute experience to party members
 */
int PartyExp_Distribute(int party_id, int base_exp, ExpDistributeType distribute_type)
{
    if (!g_exp_state.initialized) {
        return 0;
    }

    /* Calculate shares */
    MemberExpShare shares[PARTY_MAX_MEMBERS];
    int share_count = PartyExp_Calculate(party_id, base_exp, distribute_type, shares, PARTY_MAX_MEMBERS);

    if (share_count == 0) {
        return 0;
    }

    /* Distribute experience */
    int total_distributed = 0;

    for (int i = 0; i < share_count; i++) {
        printf("[PartyExp] Character %d receives %d exp (%.1f%%)\n",
               shares[i].character_id,
               shares[i].exp_amount,
               shares[i].share_percentage * 100.0f);

        /*
         * Experience distribution: Integrate with user/character module
         *
         * In production, call the character experience API:
         *   User_AddCharacterExperience(shares[i].character_id, shares[i].exp_amount);
         *
         * The character module handles:
         * - Level-up checks and processing
         * - Database sync via CharacInfo_Update()
         * - Experience cap validation
         * - Level-up notifications to client
         */

        total_distributed += shares[i].exp_amount;
    }

    g_exp_state.stats.total_exp_distributed += total_distributed;
    g_exp_state.stats.distributions++;

    printf("[PartyExp] Distributed %d total exp to party %d (%d members)\n",
           total_distributed, party_id, share_count);

    return total_distributed;
}

/**
 * Cleanup party experience system
 */
void PartyExp_Cleanup(void)
{
    if (!g_exp_state.initialized) {
        return;
    }

    printf("[PartyExp] Cleaning up party experience system...\n");
    printf("[PartyExp] Statistics:\n");
    printf("  - Total Exp Distributed: %llu\n", g_exp_state.stats.total_exp_distributed);
    printf("  - Total Distributions: %u\n", g_exp_state.stats.distributions);
    printf("  - Average Distributions: %u\n", g_exp_state.stats.average_distributions);
    printf("  - Damage Distributions: %u\n", g_exp_state.stats.damage_distributions);
    printf("  - Level Distributions: %u\n", g_exp_state.stats.level_distributions);

    if (g_exp_state.damage_contributions) {
        free(g_exp_state.damage_contributions);
        g_exp_state.damage_contributions = NULL;
    }

    g_exp_state.contribution_count = 0;
    g_exp_state.initialized = 0;

    printf("[PartyExp] Cleanup complete\n");
}
