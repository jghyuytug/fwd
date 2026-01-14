/**
 * Party Manager Module - Implementation
 * Purpose: Manage party lifecycle (create, join, leave, disband)
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#include "../../include/party/party_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"

/* Database manager for character lookups */
static DBConnectionManager* g_party_db_manager = NULL;

/**
 * Set database manager for party system
 */
void PartyManager_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_party_db_manager = db_manager;
}

/* Maximum parties */
#define MAX_PARTIES 2000
#define MAX_PENDING_INVITES 1000

/* Pending invitation structure */
typedef struct {
    int party_id;
    int inviter_id;
    int invitee_id;
    time_t invite_time;
    int active;
} PendingInvite;

/* Global state */
static struct {
    int initialized;
    DBConnectionManager* db_manager;

    /* Party storage */
    PartyInfo* parties;
    int party_count;
    int next_party_id;
    int max_parties;

    /* Character to party mapping */
    int* character_party_map;  /* character_id -> party_id */
    int character_map_size;

    /* Pending invites */
    PendingInvite* pending_invites;
    int pending_invite_count;

    /* Statistics */
    struct {
        unsigned int parties_created;
        unsigned int parties_disbanded;
        unsigned int members_joined;
        unsigned int members_left;
        unsigned int members_kicked;
        unsigned int leaders_transferred;
    } stats;
} g_manager_state = {0};

/**
 * Initialize party manager
 */
int PartyManager_Initialize(DBConnectionManager* db_manager, int max_parties)
{
    if (g_manager_state.initialized) {
        printf("[PartyManager] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_parties <= 0 || max_parties > MAX_PARTIES) {
        max_parties = MAX_PARTIES;
    }

    printf("[PartyManager] Initializing party manager...\n");

    g_manager_state.db_manager = db_manager;
    g_manager_state.max_parties = max_parties;

    /* Allocate party storage */
    g_manager_state.parties = (PartyInfo*)malloc(sizeof(PartyInfo) * max_parties);
    if (!g_manager_state.parties) {
        printf("[PartyManager] ERROR: Failed to allocate party storage\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    /* Allocate character-party mapping (assume max 10000 characters) */
    g_manager_state.character_map_size = 10000;
    g_manager_state.character_party_map = (int*)calloc(g_manager_state.character_map_size, sizeof(int));
    if (!g_manager_state.character_party_map) {
        free(g_manager_state.parties);
        printf("[PartyManager] ERROR: Failed to allocate character map\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    /* Allocate pending invites */
    g_manager_state.pending_invites = (PendingInvite*)malloc(sizeof(PendingInvite) * MAX_PENDING_INVITES);
    if (!g_manager_state.pending_invites) {
        free(g_manager_state.character_party_map);
        free(g_manager_state.parties);
        printf("[PartyManager] ERROR: Failed to allocate pending invites\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    g_manager_state.party_count = 0;
    g_manager_state.next_party_id = 1;
    g_manager_state.pending_invite_count = 0;
    g_manager_state.initialized = 1;

    printf("[PartyManager] Party manager initialized (max %d parties)\n", max_parties);

    return ERR_SUCCESS;
}

/**
 * Set database manager
 */
void PartyManager_SetDatabaseManager(DBConnectionManager* db_manager)
{
    g_manager_state.db_manager = db_manager;
    if (db_manager) {
        printf("[PartyManager] Database manager configured\n");
    }
}

/**
 * Find party index by ID
 */
static int FindPartyIndex(int party_id)
{
    for (int i = 0; i < g_manager_state.party_count; i++) {
        if (g_manager_state.parties[i].party_id == party_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Check if character is in a party
 */
static int IsCharacterInParty(int character_id)
{
    if (character_id < 0 || character_id >= g_manager_state.character_map_size) {
        return 0;
    }
    return g_manager_state.character_party_map[character_id] != 0;
}

/**
 * Add character to party mapping
 */
static void AddCharacterToMap(int character_id, int party_id)
{
    if (character_id >= 0 && character_id < g_manager_state.character_map_size) {
        g_manager_state.character_party_map[character_id] = party_id;
    }
}

/**
 * Remove character from party mapping
 */
static void RemoveCharacterFromMap(int character_id)
{
    if (character_id >= 0 && character_id < g_manager_state.character_map_size) {
        g_manager_state.character_party_map[character_id] = 0;
    }
}

/**
 * Create a new party
 */
int PartyManager_Create(int leader_id, PartyType party_type, const char* party_name, PartyInfo* out_party)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Check if storage is full */
    if (g_manager_state.party_count >= g_manager_state.max_parties) {
        printf("[PartyManager] ERROR: Party storage full\n");
        return ERR_PARTY_STORAGE_FULL;
    }

    /* Check if leader is already in a party */
    if (IsCharacterInParty(leader_id)) {
        printf("[PartyManager] ERROR: Character %d is already in a party\n", leader_id);
        return ERR_PARTY_ALREADY_IN_PARTY;
    }

    /* Create new party */
    PartyInfo* party = &g_manager_state.parties[g_manager_state.party_count];
    memset(party, 0, sizeof(PartyInfo));

    party->party_id = g_manager_state.next_party_id++;
    party->party_type = party_type;
    party->leader_id = leader_id;
    party->member_count = 1;
    party->created_time = (unsigned int)time(NULL);
    party->dungeon_id = 0;

    /* Set party name */
    if (party_name && strlen(party_name) > 0) {
        strncpy(party->party_name, party_name, PARTY_NAME_MAX_LENGTH - 1);
    } else {
        snprintf(party->party_name, PARTY_NAME_MAX_LENGTH, "Party_%d", party->party_id);
    }

    /* Add leader as first member */
    PartyMember* leader = &party->members[0];
    leader->character_id = leader_id;
    leader->role = PARTY_ROLE_LEADER;
    leader->join_time = party->created_time;
    leader->is_online = 1;

    /* Get leader character data from database */
    leader->level = 1;        /* Default level */
    leader->job_class = 0;    /* Default job class */
    snprintf(leader->character_name, sizeof(leader->character_name), "Char_%d", leader_id);

    if (g_party_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_party_db_manager, leader_id, &charac_info) == 0) {
            leader->level = charac_info.lev;
            leader->job_class = charac_info.job;
            strncpy(leader->character_name, charac_info.charac_name, sizeof(leader->character_name) - 1);
            leader->character_name[sizeof(leader->character_name) - 1] = '\0';
        }
    }

    /* Initialize default settings */
    party->settings.loot_mode = LOOT_MODE_FREE_FOR_ALL;
    party->settings.permission = PARTY_PERMISSION_APPROVAL;
    party->settings.auto_accept_requests = 0;
    party->settings.allow_cross_channel = 1;

    /* Initialize bonuses (based on member count) */
    party->bonuses.exp_bonus_rate = 0.0f;  /* No bonus for 1 member */
    party->bonuses.drop_rate_bonus = 0.0f;
    party->bonuses.gold_bonus_rate = 0.0f;
    party->bonuses.damage_bonus = 0;
    party->bonuses.defense_bonus = 0;

    /* Add to character map */
    AddCharacterToMap(leader_id, party->party_id);

    g_manager_state.party_count++;
    g_manager_state.stats.parties_created++;

    printf("[PartyManager] Party %d created by character %d (type=%d)\n",
           party->party_id, leader_id, party_type);

    /* Return party info */
    if (out_party) {
        memcpy(out_party, party, sizeof(PartyInfo));
    }

    return party->party_id;
}

/**
 * Update party bonuses based on member count
 */
static void UpdatePartyBonuses(PartyInfo* party)
{
    switch (party->member_count) {
        case 2:
            party->bonuses.exp_bonus_rate = 0.10f;  /* +10% */
            party->bonuses.gold_bonus_rate = 0.05f;
            break;
        case 3:
            party->bonuses.exp_bonus_rate = 0.20f;  /* +20% */
            party->bonuses.gold_bonus_rate = 0.10f;
            break;
        case 4:
            party->bonuses.exp_bonus_rate = 0.30f;  /* +30% */
            party->bonuses.gold_bonus_rate = 0.15f;
            break;
        default:
            party->bonuses.exp_bonus_rate = 0.0f;
            party->bonuses.gold_bonus_rate = 0.0f;
            break;
    }
}

/**
 * Invite character to party
 */
int PartyManager_Invite(int party_id, int leader_id, int invitee_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Verify leader */
    if (party->leader_id != (unsigned int)leader_id) {
        printf("[PartyManager] ERROR: Character %d is not the party leader\n", leader_id);
        return ERR_PARTY_NOT_LEADER;
    }

    /* Check if party is full */
    if (party->member_count >= PARTY_MAX_MEMBERS) {
        printf("[PartyManager] ERROR: Party %d is full\n", party_id);
        return ERR_PARTY_FULL;
    }

    /* Check if invitee is already in a party */
    if (IsCharacterInParty(invitee_id)) {
        printf("[PartyManager] ERROR: Character %d is already in a party\n", invitee_id);
        return ERR_PARTY_ALREADY_IN_PARTY;
    }

    /* Create pending invite */
    if (g_manager_state.pending_invite_count >= MAX_PENDING_INVITES) {
        printf("[PartyManager] WARNING: Pending invite storage full\n");
        return ERR_PARTY_INVITE_FULL;
    }

    PendingInvite* invite = &g_manager_state.pending_invites[g_manager_state.pending_invite_count];
    invite->party_id = party_id;
    invite->inviter_id = leader_id;
    invite->invitee_id = invitee_id;
    invite->invite_time = time(NULL);
    invite->active = 1;

    g_manager_state.pending_invite_count++;

    printf("[PartyManager] Party %d: Character %d invited character %d\n",
           party_id, leader_id, invitee_id);

    return ERR_SUCCESS;
}

/**
 * Find pending invite
 */
static int FindPendingInvite(int party_id, int invitee_id)
{
    for (int i = 0; i < g_manager_state.pending_invite_count; i++) {
        PendingInvite* invite = &g_manager_state.pending_invites[i];
        if (invite->active && invite->party_id == party_id && invite->invitee_id == invitee_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Accept party invitation
 */
int PartyManager_AcceptInvite(int party_id, int character_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Check if party is full */
    if (party->member_count >= PARTY_MAX_MEMBERS) {
        return ERR_PARTY_FULL;
    }

    /* Check for pending invite */
    int invite_index = FindPendingInvite(party_id, character_id);
    if (invite_index < 0) {
        printf("[PartyManager] ERROR: No pending invite for character %d\n", character_id);
        return ERR_PARTY_NO_INVITE;
    }

    /* Check if already in a party */
    if (IsCharacterInParty(character_id)) {
        return ERR_PARTY_ALREADY_IN_PARTY;
    }

    /* Add member to party */
    PartyMember* member = &party->members[party->member_count];
    member->character_id = character_id;
    member->role = PARTY_ROLE_MEMBER;
    member->join_time = (unsigned int)time(NULL);
    member->is_online = 1;

    /* Get member character data from database */
    member->level = 1;        /* Default level */
    member->job_class = 0;    /* Default job class */
    snprintf(member->character_name, sizeof(member->character_name), "Char_%d", character_id);

    if (g_party_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_party_db_manager, character_id, &charac_info) == 0) {
            member->level = charac_info.lev;
            member->job_class = charac_info.job;
            strncpy(member->character_name, charac_info.charac_name, sizeof(member->character_name) - 1);
            member->character_name[sizeof(member->character_name) - 1] = '\0';
        }
    }

    party->member_count++;
    UpdatePartyBonuses(party);

    /* Add to character map */
    AddCharacterToMap(character_id, party_id);

    /* Remove pending invite */
    g_manager_state.pending_invites[invite_index].active = 0;

    g_manager_state.stats.members_joined++;

    printf("[PartyManager] Character %d joined party %d (%d/%d members)\n",
           character_id, party_id, party->member_count, PARTY_MAX_MEMBERS);

    return ERR_SUCCESS;
}

/**
 * Join party (for open parties)
 */
int PartyManager_Join(int party_id, int character_id, const char* password)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Check permission */
    if (party->settings.permission == PARTY_PERMISSION_CLOSED) {
        return ERR_PARTY_CLOSED;
    }

    if (party->settings.permission == PARTY_PERMISSION_PASSWORD) {
        if (!password || strcmp(party->settings.password, password) != 0) {
            return ERR_PARTY_WRONG_PASSWORD;
        }
    }

    if (party->settings.permission == PARTY_PERMISSION_APPROVAL) {
        /* Should use invite system */
        return ERR_PARTY_REQUIRES_APPROVAL;
    }

    /* Check if party is full */
    if (party->member_count >= PARTY_MAX_MEMBERS) {
        return ERR_PARTY_FULL;
    }

    /* Check if already in a party */
    if (IsCharacterInParty(character_id)) {
        return ERR_PARTY_ALREADY_IN_PARTY;
    }

    /* Add member (same as accept invite) */
    PartyMember* member = &party->members[party->member_count];
    member->character_id = character_id;
    member->role = PARTY_ROLE_MEMBER;
    member->join_time = (unsigned int)time(NULL);
    member->is_online = 1;
    member->level = 1;
    member->job_class = 0;
    snprintf(member->character_name, sizeof(member->character_name), "Char_%d", character_id);

    party->member_count++;
    UpdatePartyBonuses(party);

    AddCharacterToMap(character_id, party_id);

    g_manager_state.stats.members_joined++;

    printf("[PartyManager] Character %d joined party %d (%d/%d members)\n",
           character_id, party_id, party->member_count, PARTY_MAX_MEMBERS);

    return ERR_SUCCESS;
}

/**
 * Leave party
 */
int PartyManager_Leave(int party_id, int character_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Find member */
    int member_index = -1;
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == (unsigned int)character_id) {
            member_index = i;
            break;
        }
    }

    if (member_index < 0) {
        return ERR_PARTY_NOT_MEMBER;
    }

    /* Check if leader is leaving */
    if (party->leader_id == (unsigned int)character_id) {
        if (party->member_count > 1) {
            /* Transfer leadership to next member */
            party->leader_id = party->members[1].character_id;
            party->members[1].role = PARTY_ROLE_LEADER;
            printf("[PartyManager] Leadership transferred to character %u\n", party->leader_id);
        } else {
            /* Last member leaving, disband party */
            RemoveCharacterFromMap(character_id);
            g_manager_state.stats.members_left++;
            return PartyManager_Disband(party_id, character_id);
        }
    }

    /* Remove member from array (shift remaining members) */
    for (int i = member_index; i < party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }

    party->member_count--;
    UpdatePartyBonuses(party);

    /* Remove from character map */
    RemoveCharacterFromMap(character_id);

    g_manager_state.stats.members_left++;

    printf("[PartyManager] Character %d left party %d (%d/%d members remaining)\n",
           character_id, party_id, party->member_count, PARTY_MAX_MEMBERS);

    return ERR_SUCCESS;
}

/**
 * Kick member from party
 */
int PartyManager_Kick(int party_id, int leader_id, int target_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Verify leader */
    if (party->leader_id != (unsigned int)leader_id) {
        return ERR_PARTY_NOT_LEADER;
    }

    /* Cannot kick leader */
    if (party->leader_id == (unsigned int)target_id) {
        return ERR_PARTY_CANNOT_KICK_LEADER;
    }

    /* Find member */
    int member_index = -1;
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == (unsigned int)target_id) {
            member_index = i;
            break;
        }
    }

    if (member_index < 0) {
        return ERR_PARTY_NOT_MEMBER;
    }

    /* Remove member (shift remaining members) */
    for (int i = member_index; i < party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }

    party->member_count--;
    UpdatePartyBonuses(party);

    RemoveCharacterFromMap(target_id);

    g_manager_state.stats.members_kicked++;

    printf("[PartyManager] Character %d kicked from party %d by leader %d\n",
           target_id, party_id, leader_id);

    return ERR_SUCCESS;
}

/**
 * Disband party
 */
int PartyManager_Disband(int party_id, int leader_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Verify leader */
    if (party->leader_id != (unsigned int)leader_id) {
        return ERR_PARTY_NOT_LEADER;
    }

    /* Remove all members from character map */
    for (int i = 0; i < party->member_count; i++) {
        RemoveCharacterFromMap(party->members[i].character_id);
    }

    /* Remove party from array (shift remaining parties) */
    for (int i = index; i < g_manager_state.party_count - 1; i++) {
        g_manager_state.parties[i] = g_manager_state.parties[i + 1];
    }

    g_manager_state.party_count--;
    g_manager_state.stats.parties_disbanded++;

    printf("[PartyManager] Party %d disbanded by leader %d\n", party_id, leader_id);

    return ERR_SUCCESS;
}

/**
 * Transfer leader role
 */
int PartyManager_TransferLeader(int party_id, int current_leader_id, int new_leader_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Verify current leader */
    if (party->leader_id != (unsigned int)current_leader_id) {
        return ERR_PARTY_NOT_LEADER;
    }

    /* Find new leader in members */
    int old_leader_index = -1;
    int new_leader_index = -1;

    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == (unsigned int)current_leader_id) {
            old_leader_index = i;
        }
        if (party->members[i].character_id == (unsigned int)new_leader_id) {
            new_leader_index = i;
        }
    }

    if (new_leader_index < 0) {
        return ERR_PARTY_NOT_MEMBER;
    }

    /* Transfer leadership */
    party->leader_id = new_leader_id;
    if (old_leader_index >= 0) {
        party->members[old_leader_index].role = PARTY_ROLE_MEMBER;
    }
    party->members[new_leader_index].role = PARTY_ROLE_LEADER;

    g_manager_state.stats.leaders_transferred++;

    printf("[PartyManager] Party %d: Leadership transferred from %d to %d\n",
           party_id, current_leader_id, new_leader_id);

    return ERR_SUCCESS;
}

/**
 * Get party information
 */
int PartyManager_GetParty(int party_id, PartyInfo* out_party)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_party) {
        return ERR_INVALID_PARAMETER;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    memcpy(out_party, &g_manager_state.parties[index], sizeof(PartyInfo));
    return ERR_SUCCESS;
}

/**
 * Get character's party ID
 */
int PartyManager_GetCharacterParty(int character_id)
{
    if (!g_manager_state.initialized) {
        return 0;
    }

    if (character_id < 0 || character_id >= g_manager_state.character_map_size) {
        return 0;
    }

    return g_manager_state.character_party_map[character_id];
}

/**
 * Get all party members
 */
int PartyManager_GetMembers(int party_id, PartyMember* members, int max_count)
{
    if (!g_manager_state.initialized) {
        return 0;
    }

    if (!members || max_count <= 0) {
        return 0;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return 0;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    int count = (party->member_count < max_count) ? party->member_count : max_count;
    for (int i = 0; i < count; i++) {
        memcpy(&members[i], &party->members[i], sizeof(PartyMember));
    }

    return count;
}

/**
 * Set party settings
 */
int PartyManager_SetSettings(int party_id, int leader_id, const PartySettings* settings)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!settings) {
        return ERR_INVALID_PARAMETER;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Verify leader */
    if (party->leader_id != (unsigned int)leader_id) {
        return ERR_PARTY_NOT_LEADER;
    }

    /* Update settings */
    memcpy(&party->settings, settings, sizeof(PartySettings));

    printf("[PartyManager] Party %d settings updated by leader %d\n", party_id, leader_id);

    return ERR_SUCCESS;
}

/**
 * Update member status (HP/MP)
 */
int PartyManager_UpdateMemberStatus(int character_id, int current_hp, int max_hp, int current_mp, int max_mp)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find character's party */
    int party_id = PartyManager_GetCharacterParty(character_id);
    if (party_id == 0) {
        return ERR_PARTY_NOT_IN_PARTY;
    }

    /* Find party */
    int index = FindPartyIndex(party_id);
    if (index < 0) {
        return ERR_PARTY_NOT_FOUND;
    }

    PartyInfo* party = &g_manager_state.parties[index];

    /* Find member */
    for (int i = 0; i < party->member_count; i++) {
        if (party->members[i].character_id == (unsigned int)character_id) {
            party->members[i].current_hp = current_hp;
            party->members[i].max_hp = max_hp;
            party->members[i].current_mp = current_mp;
            party->members[i].max_mp = max_mp;
            return ERR_SUCCESS;
        }
    }

    return ERR_PARTY_NOT_MEMBER;
}

/**
 * Cleanup party manager
 */
void PartyManager_Cleanup(void)
{
    if (!g_manager_state.initialized) {
        return;
    }

    printf("[PartyManager] Cleaning up party manager...\n");
    printf("[PartyManager] Statistics:\n");
    printf("  - Parties Created: %u\n", g_manager_state.stats.parties_created);
    printf("  - Parties Disbanded: %u\n", g_manager_state.stats.parties_disbanded);
    printf("  - Members Joined: %u\n", g_manager_state.stats.members_joined);
    printf("  - Members Left: %u\n", g_manager_state.stats.members_left);
    printf("  - Members Kicked: %u\n", g_manager_state.stats.members_kicked);
    printf("  - Leaders Transferred: %u\n", g_manager_state.stats.leaders_transferred);

    if (g_manager_state.parties) {
        free(g_manager_state.parties);
        g_manager_state.parties = NULL;
    }

    if (g_manager_state.character_party_map) {
        free(g_manager_state.character_party_map);
        g_manager_state.character_party_map = NULL;
    }

    if (g_manager_state.pending_invites) {
        free(g_manager_state.pending_invites);
        g_manager_state.pending_invites = NULL;
    }

    g_manager_state.party_count = 0;
    g_manager_state.next_party_id = 0;
    g_manager_state.pending_invite_count = 0;
    g_manager_state.db_manager = NULL;
    g_manager_state.initialized = 0;

    printf("[PartyManager] Cleanup complete\n");
}
