/**
 * Party Module - Implementation
 *
 * Handles party/group management, member coordination, and party bonuses
 */

#include "party_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Party module global state */
static struct {
    int initialized;
    int max_parties;

    // Party instances
    PartyInfo* parties;
    int party_count;
    int next_party_id;

    // Party statistics
    PartyStats* party_stats;

    // Global statistics
    struct {
        unsigned int total_parties_created;
        unsigned int total_parties_disbanded;
        unsigned int total_members_joined;
        unsigned int total_members_left;
        unsigned int total_members_kicked;
        unsigned int total_leadership_transfers;
    } stats;
} g_party_state = {0};

/**
 * Initialize Party Module
 */
int Party_Initialize(int max_parties)
{
    if (g_party_state.initialized) {
        fprintf(stderr, "[Party] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_parties <= 0 || max_parties > 10000) {
        fprintf(stderr, "[Party] Invalid max parties: %d\n", max_parties);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Party] Initializing party module...\n");

    // Allocate party array
    g_party_state.parties = (PartyInfo*)calloc(max_parties, sizeof(PartyInfo));
    if (!g_party_state.parties) {
        fprintf(stderr, "[Party] Failed to allocate party array\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Allocate party statistics array
    g_party_state.party_stats = (PartyStats*)calloc(max_parties, sizeof(PartyStats));
    if (!g_party_state.party_stats) {
        fprintf(stderr, "[Party] Failed to allocate party stats array\n");
        free(g_party_state.parties);
        return ERR_OUT_OF_MEMORY;
    }

    g_party_state.max_parties = max_parties;
    g_party_state.party_count = 0;
    g_party_state.next_party_id = 1;
    g_party_state.initialized = 1;

    printf("[Party] Module initialized. Max parties: %d\n", max_parties);

    return ERR_SUCCESS;
}

/**
 * Cleanup Party Module
 */
void Party_Cleanup(void)
{
    if (!g_party_state.initialized) {
        return;
    }

    printf("[Party] Cleaning up party module...\n");
    printf("[Party] Statistics:\n");
    printf("  - Total Parties Created: %u\n", g_party_state.stats.total_parties_created);
    printf("  - Total Parties Disbanded: %u\n", g_party_state.stats.total_parties_disbanded);
    printf("  - Total Members Joined: %u\n", g_party_state.stats.total_members_joined);
    printf("  - Total Members Left: %u\n", g_party_state.stats.total_members_left);
    printf("  - Total Members Kicked: %u\n", g_party_state.stats.total_members_kicked);
    printf("  - Total Leadership Transfers: %u\n", g_party_state.stats.total_leadership_transfers);
    printf("  - Active Parties: %d\n", g_party_state.party_count);

    free(g_party_state.party_stats);
    free(g_party_state.parties);

    memset(&g_party_state, 0, sizeof(g_party_state));

    printf("[Party] Module cleanup complete\n");
}

/**
 * Create Party
 */
int Party_Create(unsigned int leader_id,
                 const char* party_name,
                 PartyType party_type)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (g_party_state.party_count >= g_party_state.max_parties) {
        fprintf(stderr, "[Party] Party limit reached\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Check if leader is already in a party
    for (int i = 0; i < g_party_state.party_count; i++) {
        for (int j = 0; j < g_party_state.parties[i].member_count; j++) {
            if (g_party_state.parties[i].members[j].character_id == leader_id) {
                fprintf(stderr, "[Party] Character %u already in a party\n", leader_id);
                return ERR_INVALID_STATE;
            }
        }
    }

    int party_id = g_party_state.next_party_id++;

    PartyInfo* party = &g_party_state.parties[g_party_state.party_count];
    memset(party, 0, sizeof(PartyInfo));

    party->party_id = party_id;
    party->party_type = party_type;
    party->leader_id = leader_id;
    party->member_count = 1;
    party->created_time = (unsigned int)time(NULL);
    party->dungeon_id = 0;

    // Set party name
    if (party_name && party_name[0] != '\0') {
        strncpy(party->party_name, party_name, PARTY_NAME_MAX_LENGTH - 1);
        party->party_name[PARTY_NAME_MAX_LENGTH - 1] = '\0';
    } else {
        snprintf(party->party_name, PARTY_NAME_MAX_LENGTH, "Party_%d", party_id);
    }

    // Initialize leader as first member
    party->members[0].character_id = leader_id;
    party->members[0].role = PARTY_ROLE_LEADER;
    party->members[0].join_time = party->created_time;
    party->members[0].is_online = 1;

    // Default settings
    party->settings.loot_mode = LOOT_MODE_FREE_FOR_ALL;
    party->settings.permission = PARTY_PERMISSION_OPEN;
    party->settings.auto_accept_requests = 1;
    party->settings.allow_cross_channel = 1;

    // Default bonuses
    party->bonuses.exp_bonus_rate = 0.0f;
    party->bonuses.drop_rate_bonus = 0.0f;
    party->bonuses.gold_bonus_rate = 0.0f;

    g_party_state.party_count++;
    g_party_state.stats.total_parties_created++;

    printf("[Party] Created party %d (leader=%u, type=%d, name='%s')\n",
           party_id, leader_id, party_type, party->party_name);

    return party_id;
}

/**
 * Disband Party
 */
int Party_Disband(int party_id)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find and remove the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            int member_count = g_party_state.parties[i].member_count;

            // Remove the party
            if (i < g_party_state.party_count - 1) {
                memmove(&g_party_state.parties[i],
                       &g_party_state.parties[i + 1],
                       (g_party_state.party_count - i - 1) * sizeof(PartyInfo));
                memmove(&g_party_state.party_stats[i],
                       &g_party_state.party_stats[i + 1],
                       (g_party_state.party_count - i - 1) * sizeof(PartyStats));
            }
            g_party_state.party_count--;
            g_party_state.stats.total_parties_disbanded++;

            printf("[Party] Disbanded party %d (%d members removed)\n", party_id, member_count);
            return ERR_SUCCESS;
        }
    }

    fprintf(stderr, "[Party] Party %d not found\n", party_id);
    return ERR_INVALID_PARAMETER;
}

/**
 * Join Party
 */
int Party_Join(int party_id,
               unsigned int character_id,
               const char* password)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    PartyInfo* party = NULL;
    int party_index = -1;
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            party = &g_party_state.parties[i];
            party_index = i;
            break;
        }
    }

    if (!party) {
        fprintf(stderr, "[Party] Party %d not found\n", party_id);
        return ERR_INVALID_PARAMETER;
    }

    // Check if party is full
    if (party->member_count >= PARTY_MAX_MEMBERS) {
        fprintf(stderr, "[Party] Party %d is full\n", party_id);
        return ERR_OUT_OF_MEMORY;
    }

    // Check if character is already in this party
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == character_id) {
            fprintf(stderr, "[Party] Character %u already in party %d\n", character_id, party_id);
            return ERR_INVALID_STATE;
        }
    }

    // Check if character is in another party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (i == party_index) continue;
        for (int j = 0; j < g_party_state.parties[i].member_count; j++) {
            if (g_party_state.parties[i].members[j].character_id == character_id) {
                fprintf(stderr, "[Party] Character %u already in another party\n", character_id);
                return ERR_INVALID_STATE;
            }
        }
    }

    // Check permission
    if (party->settings.permission == PARTY_PERMISSION_CLOSED) {
        fprintf(stderr, "[Party] Party %d is closed\n", party_id);
        return ERR_PERMISSION_DENIED;
    }

    // Check password
    if (party->settings.permission == PARTY_PERMISSION_PASSWORD) {
        if (!password || strcmp(party->settings.password, password) != 0) {
            fprintf(stderr, "[Party] Invalid password for party %d\n", party_id);
            return ERR_PERMISSION_DENIED;
        }
    }

    // Add member
    PartyMember* member = &party->members[party->member_count];
    memset(member, 0, sizeof(PartyMember));
    member->character_id = character_id;
    member->role = PARTY_ROLE_MEMBER;
    member->join_time = (unsigned int)time(NULL);
    member->is_online = 1;

    party->member_count++;
    g_party_state.stats.total_members_joined++;

    printf("[Party] Character %u joined party %d (members: %d/%d)\n",
           character_id, party_id, party->member_count, PARTY_MAX_MEMBERS);

    return ERR_SUCCESS;
}

/**
 * Leave Party
 */
int Party_Leave(int party_id, unsigned int character_id)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    PartyInfo* party = NULL;
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            party = &g_party_state.parties[i];
            break;
        }
    }

    if (!party) {
        fprintf(stderr, "[Party] Party %d not found\n", party_id);
        return ERR_INVALID_PARAMETER;
    }

    // Find and remove member
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == character_id) {
            int was_leader = (party->members[i].role == PARTY_ROLE_LEADER);

            // Remove member
            if (i < party->member_count - 1) {
                memmove(&party->members[i],
                       &party->members[i + 1],
                       (party->member_count - i - 1) * sizeof(PartyMember));
            }
            party->member_count--;
            g_party_state.stats.total_members_left++;

            // If leader left and party still has members, promote first member
            if (was_leader && party->member_count > 0) {
                party->members[0].role = PARTY_ROLE_LEADER;
                party->leader_id = party->members[0].character_id;
                printf("[Party] Leadership transferred to character %u\n", party->leader_id);
            }

            // If party is empty, disband it
            if (party->member_count == 0) {
                Party_Disband(party_id);
            }

            printf("[Party] Character %u left party %d\n", character_id, party_id);
            return ERR_SUCCESS;
        }
    }

    fprintf(stderr, "[Party] Character %u not in party %d\n", character_id, party_id);
    return ERR_INVALID_PARAMETER;
}

/**
 * Kick Member
 */
int Party_KickMember(int party_id,
                     unsigned int kicker_id,
                     unsigned int target_id)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    PartyInfo* party = NULL;
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            party = &g_party_state.parties[i];
            break;
        }
    }

    if (!party) {
        fprintf(stderr, "[Party] Party %d not found\n", party_id);
        return ERR_INVALID_PARAMETER;
    }

    // Verify kicker is the leader
    if (party->leader_id != kicker_id) {
        fprintf(stderr, "[Party] Character %u is not the leader of party %d\n", kicker_id, party_id);
        return ERR_PERMISSION_DENIED;
    }

    // Can't kick yourself
    if (kicker_id == target_id) {
        fprintf(stderr, "[Party] Cannot kick yourself\n");
        return ERR_INVALID_PARAMETER;
    }

    // Find and remove target
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == target_id) {
            // Remove member
            if (i < party->member_count - 1) {
                memmove(&party->members[i],
                       &party->members[i + 1],
                       (party->member_count - i - 1) * sizeof(PartyMember));
            }
            party->member_count--;
            g_party_state.stats.total_members_kicked++;

            printf("[Party] Character %u kicked from party %d by %u\n",
                   target_id, party_id, kicker_id);
            return ERR_SUCCESS;
        }
    }

    fprintf(stderr, "[Party] Character %u not in party %d\n", target_id, party_id);
    return ERR_INVALID_PARAMETER;
}

/**
 * Transfer Leadership
 */
int Party_TransferLeadership(int party_id,
                             unsigned int current_leader_id,
                             unsigned int new_leader_id)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    PartyInfo* party = NULL;
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            party = &g_party_state.parties[i];
            break;
        }
    }

    if (!party) {
        fprintf(stderr, "[Party] Party %d not found\n", party_id);
        return ERR_INVALID_PARAMETER;
    }

    // Verify current leader
    if (party->leader_id != current_leader_id) {
        fprintf(stderr, "[Party] Character %u is not the leader\n", current_leader_id);
        return ERR_PERMISSION_DENIED;
    }

    // Find both members
    int old_leader_idx = -1;
    int new_leader_idx = -1;

    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == current_leader_id) {
            old_leader_idx = i;
        }
        if (party->members[i].character_id == new_leader_id) {
            new_leader_idx = i;
        }
    }

    if (old_leader_idx == -1 || new_leader_idx == -1) {
        fprintf(stderr, "[Party] One or both characters not in party %d\n", party_id);
        return ERR_INVALID_PARAMETER;
    }

    // Transfer leadership
    party->members[old_leader_idx].role = PARTY_ROLE_MEMBER;
    party->members[new_leader_idx].role = PARTY_ROLE_LEADER;
    party->leader_id = new_leader_id;

    g_party_state.stats.total_leadership_transfers++;

    printf("[Party] Leadership transferred from %u to %u in party %d\n",
           current_leader_id, new_leader_id, party_id);

    return ERR_SUCCESS;
}

/**
 * Set Party Settings
 */
int Party_SetSettings(int party_id, const PartySettings* settings)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!settings) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            g_party_state.parties[i].settings = *settings;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Party Settings
 */
int Party_GetSettings(int party_id, PartySettings* settings)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!settings) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            *settings = g_party_state.parties[i].settings;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Set Loot Mode
 */
int Party_SetLootMode(int party_id, LootMode loot_mode)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            g_party_state.parties[i].settings.loot_mode = loot_mode;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Set Party Password
 */
int Party_SetPassword(int party_id, const char* password)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            if (password && password[0] != '\0') {
                strncpy(g_party_state.parties[i].settings.password, password,
                       PARTY_PASSWORD_MAX_LENGTH - 1);
                g_party_state.parties[i].settings.password[PARTY_PASSWORD_MAX_LENGTH - 1] = '\0';
                g_party_state.parties[i].settings.permission = PARTY_PERMISSION_PASSWORD;
            } else {
                memset(g_party_state.parties[i].settings.password, 0,
                      PARTY_PASSWORD_MAX_LENGTH);
                g_party_state.parties[i].settings.permission = PARTY_PERMISSION_OPEN;
            }
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Party Info
 */
int Party_GetInfo(int party_id, PartyInfo* info)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!info) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            *info = g_party_state.parties[i];
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Party Members
 */
int Party_GetMembers(int party_id,
                     PartyMember* members,
                     int max_members)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!members || max_members <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            int count = (g_party_state.parties[i].member_count < max_members) ?
                        g_party_state.parties[i].member_count : max_members;
            memcpy(members, g_party_state.parties[i].members, count * sizeof(PartyMember));
            return count;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Active Party Count
 */
int Party_GetActiveCount(void)
{
    if (!g_party_state.initialized) {
        return 0;
    }

    return g_party_state.party_count;
}

/**
 * Is Character In Party
 */
int Party_IsCharacterInParty(unsigned int character_id)
{
    if (!g_party_state.initialized) {
        return 0;
    }

    for (int i = 0; i < g_party_state.party_count; i++) {
        for (int j = 0; j < g_party_state.parties[i].member_count; j++) {
            if (g_party_state.parties[i].members[j].character_id == character_id) {
                return g_party_state.parties[i].party_id;
            }
        }
    }

    return 0;
}

/**
 * Is Party Leader
 */
int Party_IsLeader(int party_id, unsigned int character_id)
{
    if (!g_party_state.initialized) {
        return 0;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            return (g_party_state.parties[i].leader_id == character_id) ? 1 : 0;
        }
    }

    return 0;
}

/**
 * Calculate Party Bonuses (stub)
 */
int Party_CalculateBonuses(int party_id, PartyBonuses* bonuses)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!bonuses) {
        return ERR_INVALID_PARAMETER;
    }

    // Stub: Simple bonuses based on party size
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            int member_count = g_party_state.parties[i].member_count;
            bonuses->exp_bonus_rate = (member_count - 1) * 10.0f;  // 10% per extra member
            bonuses->drop_rate_bonus = (member_count - 1) * 5.0f;  // 5% per extra member
            bonuses->gold_bonus_rate = (member_count - 1) * 5.0f;
            bonuses->damage_bonus = 0;
            bonuses->defense_bonus = 0;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Party Bonuses
 */
int Party_GetBonuses(int party_id, PartyBonuses* bonuses)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!bonuses) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            *bonuses = g_party_state.parties[i].bonuses;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Get Party Statistics
 */
int Party_GetStatistics(int party_id, PartyStats* stats)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            *stats = g_party_state.party_stats[i];
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Update Party Statistics
 */
int Party_UpdateStatistics(int party_id,
                          unsigned int exp_gained,
                          unsigned int gold_gained,
                          unsigned int items_looted)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            g_party_state.party_stats[i].total_exp_gained += exp_gained;
            g_party_state.party_stats[i].total_gold_gained += gold_gained;
            g_party_state.party_stats[i].total_items_looted += items_looted;
            return ERR_SUCCESS;
        }
    }

    return ERR_INVALID_PARAMETER;
}

/**
 * Update Party State
 */
int Party_Update(int delta_time_ms)
{
    if (!g_party_state.initialized) {
        return 0;
    }

    // Stub: Would update party timers, bonuses, etc.
    (void)delta_time_ms;
    return g_party_state.party_count;
}

/**
 * Broadcast to Party
 */
int Party_Broadcast(int party_id, const char* message)
{
    if (!g_party_state.initialized) {
        return 0;
    }

    if (!message) {
        return 0;
    }

    // Find the party
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            // Stub: Would send message to all members
            printf("[Party] Broadcast to party %d: %s\n", party_id, message);
            return g_party_state.parties[i].member_count;
        }
    }

    return 0;
}

/**
 * Update Member Status
 */
int Party_UpdateMemberStatus(int party_id,
                             unsigned int character_id,
                             int current_hp,
                             int max_hp,
                             int current_mp,
                             int max_mp)
{
    if (!g_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Find the party and member
    for (int i = 0; i < g_party_state.party_count; i++) {
        if (g_party_state.parties[i].party_id == party_id) {
            for (int j = 0; j < g_party_state.parties[i].member_count; j++) {
                if (g_party_state.parties[i].members[j].character_id == character_id) {
                    g_party_state.parties[i].members[j].current_hp = current_hp;
                    g_party_state.parties[i].members[j].max_hp = max_hp;
                    g_party_state.parties[i].members[j].current_mp = current_mp;
                    g_party_state.parties[i].members[j].max_mp = max_mp;
                    return ERR_SUCCESS;
                }
            }
        }
    }

    return ERR_INVALID_PARAMETER;
}
