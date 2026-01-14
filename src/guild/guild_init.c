/**
 * @file guild_init.c
 * @brief Guild Module - Public Interface Implementation
 *
 * This file implements the public Guild API by delegating to
 * internal subsystems (GuildManager, GuildLevel, GuildSkill).
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "guild_interface.h"
#include "../../include/guild/guild_manager.h"
#include "../../include/guild/guild_level.h"
#include "../../include/guild/guild_skill.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

int Guild_Initialize(int max_guilds)
{
    printf("[Guild] Initializing guild module (Phase 6.4)...\n");

    /* Initialize Guild Manager */
    int result = GuildManager_Initialize(max_guilds);
    if (result != ERR_SUCCESS) {
        fprintf(stderr, "[Guild] Failed to initialize GuildManager: %d\n", result);
        return result;
    }

    /* Initialize Guild Level System */
    result = GuildLevel_Initialize();
    if (result != ERR_SUCCESS) {
        fprintf(stderr, "[Guild] Failed to initialize GuildLevel: %d\n", result);
        GuildManager_Cleanup();
        return result;
    }

    /* Initialize Guild Skill System */
    result = GuildSkill_Initialize();
    if (result != ERR_SUCCESS) {
        fprintf(stderr, "[Guild] Failed to initialize GuildSkill: %d\n", result);
        GuildLevel_Cleanup();
        GuildManager_Cleanup();
        return result;
    }

    printf("[Guild] Guild module initialized successfully (3 subsystems)\n");
    printf("[Guild] Settings: MAX_MEMBERS=200, MAX_LEVEL=10\n");

    return ERR_SUCCESS;
}

void Guild_Cleanup(void)
{
    printf("[Guild] Cleaning up guild module...\n");

    /* Cleanup subsystems (reverse order of initialization) */
    GuildSkill_Cleanup();
    GuildLevel_Cleanup();
    GuildManager_Cleanup();

    printf("[Guild] Guild module cleanup complete\n");
}

/*==============================================================================
 * Guild Lifecycle Functions
 *============================================================================*/

int Guild_Create(unsigned int leader_id, const char* guild_name)
{
    GuildData guild;
    int result = GuildManager_Create((int)leader_id, guild_name, &guild);

    if (result < 0) {
        return result; /* Return error code */
    }

    return guild.guild_id; /* Return guild ID on success */
}

int Guild_Disband(unsigned int guild_id)
{
    /* Get guild to find leader ID */
    GuildData guild;
    int result = GuildManager_GetGuild((int)guild_id, &guild);
    if (result != ERR_SUCCESS) {
        return result;
    }

    return GuildManager_Disband((int)guild_id, guild.leader_id);
}

/*==============================================================================
 * Member Management Functions
 *============================================================================*/

int Guild_Join(unsigned int user_id, unsigned int guild_id)
{
    /* This is accept invite operation */
    return GuildManager_AcceptInvite((int)user_id, (int)guild_id);
}

int Guild_Leave(unsigned int user_id)
{
    return GuildManager_Leave((int)user_id);
}

int Guild_KickMember(unsigned int guild_id,
                     unsigned int user_id,
                     unsigned int kicker_id)
{
    return GuildManager_Kick((int)guild_id, (int)kicker_id, (int)user_id);
}

/*==============================================================================
 * Role Management Functions
 *============================================================================*/

int Guild_PromoteMember(unsigned int guild_id,
                        unsigned int user_id,
                        GuildRole new_role)
{
    /* Map GuildRole enum to GuildGrade */
    signed char new_grade;
    switch (new_role) {
        case GUILD_ROLE_MEMBER:
            new_grade = 0; /* GUILD_GRADE_MEMBER */
            break;
        case GUILD_ROLE_OFFICER:
            new_grade = 1; /* GUILD_GRADE_OFFICER */
            break;
        case GUILD_ROLE_VICE_LEADER:
            new_grade = 2; /* GUILD_GRADE_VICE_LEADER */
            break;
        case GUILD_ROLE_LEADER:
            new_grade = 3; /* GUILD_GRADE_LEADER */
            break;
        default:
            return ERR_INVALID_PARAMETER;
    }

    /* Get guild to find promoter (leader) */
    GuildData guild;
    int result = GuildManager_GetGuild((int)guild_id, &guild);
    if (result != ERR_SUCCESS) {
        return result;
    }

    return GuildManager_ChangeGrade((int)guild_id, guild.leader_id, (int)user_id, new_grade);
}

int Guild_TransferLeadership(unsigned int guild_id,
                             unsigned int old_leader_id,
                             unsigned int new_leader_id)
{
    return GuildManager_TransferLeader((int)guild_id, (int)old_leader_id, (int)new_leader_id);
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int Guild_GetInfo(unsigned int guild_id, GuildInfo* guild_info)
{
    if (!guild_info) {
        return ERR_INVALID_PARAMETER;
    }

    /* Get guild data from manager */
    GuildData guild;
    int result = GuildManager_GetGuild((int)guild_id, &guild);
    if (result != ERR_SUCCESS) {
        return result;
    }

    /* Map GuildData to GuildInfo */
    memset(guild_info, 0, sizeof(GuildInfo));
    guild_info->guild_id = guild.guild_id;
    strncpy(guild_info->guild_name, guild.guild_name, sizeof(guild_info->guild_name) - 1);
    guild_info->guild_level = guild.guild_level;
    guild_info->member_count = guild.member_count;
    guild_info->max_members = guild.max_members;
    guild_info->guild_funds = guild.guild_fund;
    guild_info->creation_time = (unsigned int)guild.create_time;
    guild_info->leader_id = guild.leader_id;

    return ERR_SUCCESS;
}

int Guild_GetMembers(unsigned int guild_id,
                     GuildMember* members,
                     int max_members)
{
    if (!members || max_members <= 0) {
        return 0;
    }

    /* Get members from manager */
    GuildMemberData* member_data = (GuildMemberData*)malloc(max_members * sizeof(GuildMemberData));
    if (!member_data) {
        return 0;
    }

    int count = GuildManager_GetMembers((int)guild_id, member_data, max_members);

    if (count > 0) {
        /* Map GuildMemberData to GuildMember */
        for (int i = 0; i < count && i < max_members; i++) {
            members[i].user_id = member_data[i].character_id;
            members[i].guild_id = member_data[i].guild_id;

            /* Map grade to role */
            switch (member_data[i].grade) {
                case 0: members[i].role = GUILD_ROLE_MEMBER; break;
                case 1: members[i].role = GUILD_ROLE_OFFICER; break;
                case 2: members[i].role = GUILD_ROLE_VICE_LEADER; break;
                case 3: members[i].role = GUILD_ROLE_LEADER; break;
                default: members[i].role = GUILD_ROLE_MEMBER; break;
            }

            members[i].join_time = (unsigned int)member_data[i].join_time;
            members[i].contribution = member_data[i].member_point;
            members[i].is_online = member_data[i].is_online;
        }
    }

    free(member_data);

    return count;
}

int Guild_GetStatistics(GuildStats* stats)
{
    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    unsigned int created, disbanded, joined;
    int result = GuildManager_GetStatistics(&created, &disbanded, &joined);

    if (result == ERR_SUCCESS) {
        stats->total_guilds_created = created;
        stats->total_guilds_disbanded = disbanded;
        stats->total_members_joined = joined;
        /*
         * Extended statistics: These counters are tracked in GuildManagerState.stats
         * but not exposed via the current API. Values are returned as 0 until
         * GuildManager_GetExtendedStatistics() is implemented.
         *
         * In production, these are tracked by:
         * - total_members_left: GuildManager_Leave() increments stats.total_members_left
         * - total_members_kicked: GuildManager_Kick() increments stats.total_members_kicked
         * - leadership_changes: GuildManager_TransferLeader() increments stats.total_leadership_transfers
         */
        stats->total_members_left = 0;
        stats->total_members_kicked = 0;
        stats->leadership_changes = 0;
    }

    return result;
}
