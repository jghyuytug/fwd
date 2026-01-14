/**
 * @file guild_manager.c
 * @brief Guild Manager Implementation
 *
 * Implements guild lifecycle management, member management,
 * and role assignments based on real database schema.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/guild/guild_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/d_guild/headers/guild_member_orm.h"

/*==============================================================================
 * Global State
 *============================================================================*/

static GuildManagerState g_manager_state = {0};
static DBConnectionManager* g_guild_db_manager = NULL;

/**
 * Set database manager for guild persistence
 */
void GuildManager_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_guild_db_manager = db_manager;
}

/*==============================================================================
 * Internal Helper Functions
 *============================================================================*/

/**
 * Find guild by ID
 */
static GuildData* FindGuild(int guild_id)
{
    for (int i = 0; i < g_manager_state.guild_count; i++) {
        if (g_manager_state.guilds[i].guild_id == guild_id) {
            return &g_manager_state.guilds[i];
        }
    }
    return NULL;
}

/**
 * Find guild by name
 */
static GuildData* FindGuildByName(const char* guild_name)
{
    if (!guild_name) return NULL;

    for (int i = 0; i < g_manager_state.guild_count; i++) {
        if (strcmp(g_manager_state.guilds[i].guild_name, guild_name) == 0) {
            return &g_manager_state.guilds[i];
        }
    }
    return NULL;
}

/**
 * Find pending invite
 */
static PendingInvite* FindPendingInvite(int guild_id, int invitee_id)
{
    for (int i = 0; i < g_manager_state.invite_count; i++) {
        PendingInvite* invite = &g_manager_state.pending_invites[i];
        if (invite->is_active &&
            invite->guild_id == guild_id &&
            invite->invitee_id == invitee_id) {
            return invite;
        }
    }
    return NULL;
}

/**
 * Get character's guild ID from mapping
 */
static int GetCharacterGuildIDFromMap(int character_id)
{
    if (character_id < 0 || character_id >= g_manager_state.character_map_size) {
        return 0;
    }
    return g_manager_state.character_guild_map[character_id];
}

/**
 * Set character's guild ID in mapping
 */
static void SetCharacterGuildIDInMap(int character_id, int guild_id)
{
    if (character_id >= 0 && character_id < g_manager_state.character_map_size) {
        g_manager_state.character_guild_map[character_id] = guild_id;
    }
}

/**
 * Validate guild name
 */
static int ValidateGuildName(const char* guild_name)
{
    if (!guild_name || guild_name[0] == '\0') {
        return 0;
    }

    int len = strlen(guild_name);
    if (len > GUILD_NAME_MAX_LENGTH) {
        return 0;
    }

    /* Check for valid characters (alphanumeric + spaces) */
    for (int i = 0; i < len; i++) {
        char c = guild_name[i];
        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              c == ' ' || c == '_' || c == '-' ||
              (c & 0x80))) { /* Allow UTF-8 multibyte */
            return 0;
        }
    }

    return 1;
}

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

int GuildManager_Initialize(int max_guilds)
{
    if (g_manager_state.initialized) {
        fprintf(stderr, "[GuildManager] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_guilds <= 0 || max_guilds > MAX_GUILDS) {
        fprintf(stderr, "[GuildManager] Invalid max_guilds: %d\n", max_guilds);
        return ERR_INVALID_PARAMETER;
    }

    printf("[GuildManager] Initializing...\n");

    /* Allocate guild storage */
    g_manager_state.guilds = (GuildData*)calloc(max_guilds, sizeof(GuildData));
    if (!g_manager_state.guilds) {
        fprintf(stderr, "[GuildManager] Failed to allocate guild storage\n");
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate character-guild mapping (10,000 characters) */
    g_manager_state.character_map_size = 10000;
    g_manager_state.character_guild_map = (int*)calloc(g_manager_state.character_map_size, sizeof(int));
    if (!g_manager_state.character_guild_map) {
        free(g_manager_state.guilds);
        fprintf(stderr, "[GuildManager] Failed to allocate character mapping\n");
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate pending invites */
    g_manager_state.pending_invites = (PendingInvite*)calloc(MAX_PENDING_INVITES, sizeof(PendingInvite));
    if (!g_manager_state.pending_invites) {
        free(g_manager_state.guilds);
        free(g_manager_state.character_guild_map);
        fprintf(stderr, "[GuildManager] Failed to allocate invite storage\n");
        return ERR_OUT_OF_MEMORY;
    }

    g_manager_state.max_guilds = max_guilds;
    g_manager_state.guild_count = 0;
    g_manager_state.invite_count = 0;
    memset(&g_manager_state.stats, 0, sizeof(g_manager_state.stats));

    g_manager_state.initialized = 1;

    printf("[GuildManager] Initialized successfully (max_guilds=%d)\n", max_guilds);
    printf("[GuildManager] Settings: MAX_MEMBERS=%d, MAX_LEVEL=%d\n",
           GUILD_MAX_MEMBERS, GUILD_MAX_LEVEL);

    return ERR_SUCCESS;
}

void GuildManager_Cleanup(void)
{
    if (!g_manager_state.initialized) {
        return;
    }

    printf("[GuildManager] Cleanup...\n");
    printf("[GuildManager] Statistics:\n");
    printf("  - Total Guilds Created: %u\n", g_manager_state.stats.total_guilds_created);
    printf("  - Total Guilds Disbanded: %u\n", g_manager_state.stats.total_guilds_disbanded);
    printf("  - Total Members Joined: %u\n", g_manager_state.stats.total_members_joined);
    printf("  - Total Members Left: %u\n", g_manager_state.stats.total_members_left);
    printf("  - Total Members Kicked: %u\n", g_manager_state.stats.total_members_kicked);
    printf("  - Total Promotions: %u\n", g_manager_state.stats.total_promotions);
    printf("  - Leadership Transfers: %u\n", g_manager_state.stats.total_leadership_transfers);

    if (g_manager_state.guilds) {
        free(g_manager_state.guilds);
    }

    if (g_manager_state.character_guild_map) {
        free(g_manager_state.character_guild_map);
    }

    if (g_manager_state.pending_invites) {
        free(g_manager_state.pending_invites);
    }

    memset(&g_manager_state, 0, sizeof(g_manager_state));

    printf("[GuildManager] Cleanup complete\n");
}

/*==============================================================================
 * Guild Lifecycle Functions
 *============================================================================*/

int GuildManager_Create(int leader_id, const char* guild_name, GuildData* out_guild)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Validate guild name */
    if (!ValidateGuildName(guild_name)) {
        fprintf(stderr, "[GuildManager] Invalid guild name\n");
        return ERR_INVALID_PARAMETER;
    }

    /* Check if leader already in a guild */
    if (GetCharacterGuildIDFromMap(leader_id) != 0) {
        fprintf(stderr, "[GuildManager] Leader %d already in a guild\n", leader_id);
        return ERR_ALREADY_EXISTS;
    }

    /* Check if guild name already exists */
    if (FindGuildByName(guild_name) != NULL) {
        fprintf(stderr, "[GuildManager] Guild name '%s' already exists\n", guild_name);
        return ERR_ALREADY_EXISTS;
    }

    /* Check guild count limit */
    if (g_manager_state.guild_count >= g_manager_state.max_guilds) {
        fprintf(stderr, "[GuildManager] Guild limit reached\n");
        return ERR_LIMIT_REACHED;
    }

    /* Create new guild */
    GuildData* guild = &g_manager_state.guilds[g_manager_state.guild_count];
    memset(guild, 0, sizeof(GuildData));

    guild->guild_id = g_manager_state.guild_count + 1;
    strncpy(guild->guild_name, guild_name, GUILD_NAME_MAX_LENGTH);
    guild->guild_name[GUILD_NAME_MAX_LENGTH] = '\0';
    guild->guild_level = 1;
    guild->guild_exp = 0;
    guild->member_count = 1; /* Leader is first member */
    guild->max_members = GUILD_MAX_MEMBERS; /* ✅ User confirmed: 200 */
    guild->guild_fund = 0;
    guild->guild_point = 0;
    guild->leader_id = leader_id;
    guild->create_time = time(NULL);
    guild->last_levelup_time = guild->create_time;

    /* Add leader to character-guild mapping */
    SetCharacterGuildIDInMap(leader_id, guild->guild_id);

    g_manager_state.guild_count++;
    g_manager_state.stats.total_guilds_created++;
    g_manager_state.stats.total_members_joined++;

    printf("[GuildManager] Created guild '%s' (ID=%d) by leader %d\n",
           guild->guild_name, guild->guild_id, leader_id);

    /* Copy to output if provided */
    if (out_guild) {
        memcpy(out_guild, guild, sizeof(GuildData));
    }

    return guild->guild_id;
}

int GuildManager_Disband(int guild_id, int leader_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Verify leadership */
    if (guild->leader_id != leader_id) {
        fprintf(stderr, "[GuildManager] Only leader can disband guild\n");
        return ERR_PERMISSION_DENIED;
    }

    printf("[GuildManager] Disbanding guild '%s' (ID=%d)\n",
           guild->guild_name, guild->guild_id);

    /* Remove all members from character-guild mapping */
    for (int i = 0; i < g_manager_state.character_map_size; i++) {
        if (g_manager_state.character_guild_map[i] == guild_id) {
            g_manager_state.character_guild_map[i] = 0;
        }
    }

    /* Remove guild by shifting array */
    int guild_index = guild - g_manager_state.guilds;
    if (guild_index < g_manager_state.guild_count - 1) {
        memmove(&g_manager_state.guilds[guild_index],
                &g_manager_state.guilds[guild_index + 1],
                (g_manager_state.guild_count - guild_index - 1) * sizeof(GuildData));
    }

    g_manager_state.guild_count--;
    g_manager_state.stats.total_guilds_disbanded++;

    printf("[GuildManager] Guild disbanded successfully\n");

    return ERR_SUCCESS;
}

/*==============================================================================
 * Member Management Functions
 *============================================================================*/

int GuildManager_Invite(int guild_id, int inviter_id, int invitee_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Check if inviter is in the guild */
    if (GetCharacterGuildIDFromMap(inviter_id) != guild_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Check if invitee already in a guild */
    if (GetCharacterGuildIDFromMap(invitee_id) != 0) {
        return ERR_ALREADY_EXISTS;
    }

    /* Check if already invited */
    if (FindPendingInvite(guild_id, invitee_id) != NULL) {
        return ERR_ALREADY_EXISTS;
    }

    /* Check member count limit */
    if (guild->member_count >= guild->max_members) {
        return ERR_LIMIT_REACHED;
    }

    /* Check invite count limit */
    if (g_manager_state.invite_count >= MAX_PENDING_INVITES) {
        return ERR_LIMIT_REACHED;
    }

    /* Create pending invite */
    PendingInvite* invite = &g_manager_state.pending_invites[g_manager_state.invite_count];
    invite->guild_id = guild_id;
    invite->inviter_id = inviter_id;
    invite->invitee_id = invitee_id;
    invite->invite_time = time(NULL);
    invite->is_active = 1;

    g_manager_state.invite_count++;

    printf("[GuildManager] Invited character %d to guild '%s' (ID=%d)\n",
           invitee_id, guild->guild_name, guild_id);

    return g_manager_state.invite_count; /* Return invite ID */
}

int GuildManager_AcceptInvite(int invitee_id, int guild_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Find pending invite */
    PendingInvite* invite = FindPendingInvite(guild_id, invitee_id);
    if (!invite) {
        return ERR_NOT_FOUND;
    }

    /* Check if invitee already in a guild */
    if (GetCharacterGuildIDFromMap(invitee_id) != 0) {
        invite->is_active = 0;
        return ERR_ALREADY_EXISTS;
    }

    /* Check member count limit */
    if (guild->member_count >= guild->max_members) {
        invite->is_active = 0;
        return ERR_LIMIT_REACHED;
    }

    /* Add member to guild */
    guild->member_count++;
    SetCharacterGuildIDInMap(invitee_id, guild_id);

    /* Mark invite as processed */
    invite->is_active = 0;

    g_manager_state.stats.total_members_joined++;

    printf("[GuildManager] Character %d joined guild '%s' (ID=%d)\n",
           invitee_id, guild->guild_name, guild_id);

    return ERR_SUCCESS;
}

int GuildManager_DeclineInvite(int invitee_id, int guild_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find pending invite */
    PendingInvite* invite = FindPendingInvite(guild_id, invitee_id);
    if (!invite) {
        return ERR_NOT_FOUND;
    }

    /* Mark invite as inactive */
    invite->is_active = 0;

    printf("[GuildManager] Character %d declined guild invitation (ID=%d)\n",
           invitee_id, guild_id);

    return ERR_SUCCESS;
}

int GuildManager_Leave(int character_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    int guild_id = GetCharacterGuildIDFromMap(character_id);
    if (guild_id == 0) {
        return ERR_NOT_FOUND;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Check if character is guild leader */
    if (guild->leader_id == character_id) {
        fprintf(stderr, "[GuildManager] Leader cannot leave guild (must transfer or disband)\n");
        return ERR_PERMISSION_DENIED;
    }

    /* Remove member */
    guild->member_count--;
    SetCharacterGuildIDInMap(character_id, 0);

    g_manager_state.stats.total_members_left++;

    printf("[GuildManager] Character %d left guild '%s' (ID=%d)\n",
           character_id, guild->guild_name, guild_id);

    /* Auto-disband if no members left */
    if (guild->member_count == 0) {
        GuildManager_Disband(guild_id, guild->leader_id);
    }

    return ERR_SUCCESS;
}

int GuildManager_Kick(int guild_id, int kicker_id, int target_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Check if kicker has permission (must be leader or vice-leader) */
    if (g_guild_db_manager) {
        GuildMember kicker_member;
        if (GuildMember_Get(g_guild_db_manager, guild_id, kicker_id, &kicker_member) == 0) {
            if (kicker_member.grade < GUILD_GRADE_VICE_LEADER) {
                return ERR_PERMISSION_DENIED;  /* Must be vice-leader or leader to kick */
            }
        }
    }

    /* Check if kicker is in the guild */
    if (GetCharacterGuildIDFromMap(kicker_id) != guild_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Check if target is in the guild */
    if (GetCharacterGuildIDFromMap(target_id) != guild_id) {
        return ERR_NOT_FOUND;
    }

    /* Cannot kick guild leader */
    if (target_id == guild->leader_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Remove member */
    guild->member_count--;
    SetCharacterGuildIDInMap(target_id, 0);

    g_manager_state.stats.total_members_kicked++;

    printf("[GuildManager] Character %d kicked from guild '%s' (ID=%d) by %d\n",
           target_id, guild->guild_name, guild_id, kicker_id);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Role Management Functions
 *============================================================================*/

int GuildManager_ChangeGrade(int guild_id, int promoter_id, int target_id, signed char new_grade)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Validate new grade */
    if (new_grade < GUILD_GRADE_MEMBER || new_grade > GUILD_GRADE_LEADER) {
        return ERR_INVALID_PARAMETER;
    }

    /* Only leader can change grades */
    if (guild->leader_id != promoter_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Check if target is in guild */
    if (GetCharacterGuildIDFromMap(target_id) != guild_id) {
        return ERR_NOT_FOUND;
    }

    /* Cannot change leader's grade (must use TransferLeader) */
    if (target_id == guild->leader_id && new_grade != GUILD_GRADE_LEADER) {
        return ERR_PERMISSION_DENIED;
    }

    /* Update member's grade in database */
    if (g_guild_db_manager) {
        GuildMember member;
        if (GuildMember_Get(g_guild_db_manager, guild_id, target_id, &member) == 0) {
            member.grade = new_grade;
            GuildMember_Update(g_guild_db_manager, &member);
        }
    }

    g_manager_state.stats.total_promotions++;

    printf("[GuildManager] Changed grade for character %d to %d in guild '%s'\n",
           target_id, new_grade, guild->guild_name);

    return ERR_SUCCESS;
}

int GuildManager_TransferLeader(int guild_id, int current_leader_id, int new_leader_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    /* Verify current leader */
    if (guild->leader_id != current_leader_id) {
        return ERR_PERMISSION_DENIED;
    }

    /* Check if new leader is in guild */
    if (GetCharacterGuildIDFromMap(new_leader_id) != guild_id) {
        return ERR_NOT_FOUND;
    }

    /* Transfer leadership */
    int old_leader = guild->leader_id;
    guild->leader_id = new_leader_id;

    g_manager_state.stats.total_leadership_transfers++;

    printf("[GuildManager] Transferred leadership from %d to %d in guild '%s'\n",
           old_leader, new_leader_id, guild->guild_name);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int GuildManager_GetGuild(int guild_id, GuildData* out_guild)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_guild) {
        return ERR_INVALID_PARAMETER;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    memcpy(out_guild, guild, sizeof(GuildData));

    return ERR_SUCCESS;
}

int GuildManager_GetCharacterGuild(int character_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    return GetCharacterGuildIDFromMap(character_id);
}

int GuildManager_GetMembers(int guild_id, GuildMemberData* members, int max_count)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!members || max_count <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    int member_count = 0;

    /* Retrieve member details from database */
    if (g_guild_db_manager) {
        GuildMember db_members[100];
        int actual_count = 0;
        if (GuildMember_GetAll(g_guild_db_manager, db_members, max_count < 100 ? max_count : 100, &actual_count) == 0) {
            for (int i = 0; i < actual_count && member_count < max_count; i++) {
                if (db_members[i].guild_id == guild_id) {
                    members[member_count].guild_id = db_members[i].guild_id;
                    members[member_count].character_id = db_members[i].charac_no;
                    members[member_count].account_id = db_members[i].m_id;
                    strncpy(members[member_count].character_name, db_members[i].charac_name, 20);
                    members[member_count].character_name[20] = '\0';
                    strncpy(members[member_count].nick_name, db_members[i].nick_name, 12);
                    members[member_count].nick_name[12] = '\0';
                    strncpy(members[member_count].memo, db_members[i].memo, 30);
                    members[member_count].memo[30] = '\0';
                    members[member_count].grade = db_members[i].grade;
                    members[member_count].job = db_members[i].job;
                    members[member_count].level = db_members[i].lev;
                    members[member_count].member_point = db_members[i].member_point;
                    members[member_count].is_online = 0;  /* Online status tracked in memory */
                    member_count++;
                }
            }
        }
    } else {
        /* Fallback: return count based on character-guild mapping */
        for (int i = 0; i < g_manager_state.character_map_size && member_count < max_count; i++) {
            if (g_manager_state.character_guild_map[i] == guild_id) {
                members[member_count].guild_id = guild_id;
                members[member_count].character_id = i;
                member_count++;
            }
        }
    }

    return member_count;
}

int GuildManager_GetMemberCount(int guild_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    return guild->member_count;
}

int GuildManager_IsLeader(int guild_id, int character_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    GuildData* guild = FindGuild(guild_id);
    if (!guild) {
        return ERR_NOT_FOUND;
    }

    return (guild->leader_id == character_id) ? 1 : 0;
}

int GuildManager_GuildNameExists(const char* guild_name)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!guild_name) {
        return ERR_INVALID_PARAMETER;
    }

    return (FindGuildByName(guild_name) != NULL) ? 1 : 0;
}

/*==============================================================================
 * Utility Functions
 *============================================================================*/

int GuildManager_UpdateMemberOnlineStatus(int character_id, int is_online)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    int guild_id = GetCharacterGuildIDFromMap(character_id);
    if (guild_id == 0) {
        return ERR_NOT_FOUND;
    }

    /* Update member's online status: Update last_visit_time in database */
    if (g_guild_db_manager && is_online) {
        GuildMember member;
        if (GuildMember_Get(g_guild_db_manager, guild_id, character_id, &member) == 0) {
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            strftime(member.last_visit_time, sizeof(member.last_visit_time),
                     "%Y-%m-%d %H:%M:%S", tm_info);
            GuildMember_Update(g_guild_db_manager, &member);
        }
    }
    /* Note: Real-time online status is tracked in memory via session management,
       not persisted to database on every status change */

    return ERR_SUCCESS;
}

int GuildManager_AddMemberPoints(int character_id, int points)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (points <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    int guild_id = GetCharacterGuildIDFromMap(character_id);
    if (guild_id == 0) {
        return ERR_NOT_FOUND;
    }

    /* Add points to member's contribution in database */
    if (g_guild_db_manager) {
        GuildMember member;
        if (GuildMember_Get(g_guild_db_manager, guild_id, character_id, &member) == 0) {
            member.member_point += points;
            GuildMember_Update(g_guild_db_manager, &member);
        }
    }

    return ERR_SUCCESS;
}

int GuildManager_GetStatistics(unsigned int* total_created,
                                unsigned int* total_disbanded,
                                unsigned int* total_joined)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (total_created) {
        *total_created = g_manager_state.stats.total_guilds_created;
    }

    if (total_disbanded) {
        *total_disbanded = g_manager_state.stats.total_guilds_disbanded;
    }

    if (total_joined) {
        *total_joined = g_manager_state.stats.total_members_joined;
    }

    return ERR_SUCCESS;
}
