/**
 * @file guild_manager.h
 * @brief Guild Manager Module - Guild Lifecycle Management
 *
 * This module manages guild creation, dissolution, member management,
 * and role assignments based on real database schema (d_guild).
 *
 * Database Tables Used:
 * - guild_info (GuildInfo ORM)
 * - guild_member (GuildMember ORM)
 * - guild_grade_log (GuildGradeLog ORM)
 *
 * Confirmed Parameters (User Specified):
 * - Max Members: 200 per guild
 * - Max Level: 10
 *
 * @version 1.0
 * @date 2025-11-23
 */

#ifndef GUILD_MANAGER_H
#define GUILD_MANAGER_H

#include "../../include/common/error_codes.h"
#include "../../src/orm/core_game/d_guild/headers/guild_info_orm.h"
#include "../../src/orm/core_game/d_guild/headers/guild_member_orm.h"
#include "../../src/orm/core_game/d_guild/headers/guild_grade_log_orm.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Constants (Based on User Confirmation)
 *============================================================================*/

#define GUILD_MAX_MEMBERS           200     /* ✅ User confirmed: 200 members */
#define GUILD_MAX_LEVEL             10      /* ✅ User confirmed: 10 levels */
#define GUILD_NAME_MAX_LENGTH       40      /* ✅ From guild_info.guild_name[41] */
#define GUILD_MEMBER_MEMO_LENGTH    30      /* ✅ From guild_member.memo[31] */
#define GUILD_NICKNAME_LENGTH       12      /* ✅ From guild_member.nick_name[13] */

#define MAX_GUILDS                  10000   /* Maximum concurrent guilds */
#define MAX_PENDING_INVITES         5000    /* Maximum pending invitations */

/*==============================================================================
 * Enumerations
 *============================================================================*/

/**
 * Guild Member Grade (职位)
 * Based on guild_member.grade (signed char, 0-9)
 * Using 0-3 for core roles, 4-9 reserved for future expansion
 */
typedef enum {
    GUILD_GRADE_MEMBER = 0,         /* 普通成员 */
    GUILD_GRADE_OFFICER = 1,        /* 干部 */
    GUILD_GRADE_VICE_LEADER = 2,    /* 副会长 */
    GUILD_GRADE_LEADER = 3,         /* 会长 */
    /* 4-9 reserved for future use */
} GuildGrade;

/**
 * Guild Member Status
 */
typedef enum {
    GUILD_MEMBER_ACTIVE = 0,        /* Active member */
    GUILD_MEMBER_INVITED = 1,       /* Pending invitation */
    GUILD_MEMBER_LEFT = 2,          /* Left guild */
    GUILD_MEMBER_KICKED = 3         /* Kicked from guild */
} GuildMemberStatus;

/**
 * Guild Invite Result
 */
typedef enum {
    GUILD_INVITE_PENDING = 0,
    GUILD_INVITE_ACCEPTED = 1,
    GUILD_INVITE_DECLINED = 2,
    GUILD_INVITE_EXPIRED = 3
} GuildInviteResult;

/*==============================================================================
 * Structures
 *============================================================================*/

/**
 * Guild Data (In-Memory Representation)
 * Based on GuildInfo ORM structure
 */
typedef struct {
    int guild_id;                               /* Guild ID */
    char guild_name[GUILD_NAME_MAX_LENGTH + 1]; /* Guild name */
    int guild_level;                            /* Current level (1-10) */
    int guild_exp;                              /* Current experience */
    int member_count;                           /* Current member count */
    int max_members;                            /* Max members (200) */
    int guild_fund;                             /* Guild treasury (gold) */
    int guild_point;                            /* Guild contribution points */
    int leader_id;                              /* Leader character ID */
    char leader_name[21];                       /* Leader name */
    time_t create_time;                         /* Creation timestamp */
    time_t last_levelup_time;                   /* Last level up time */
} GuildData;

/**
 * Guild Member Data (In-Memory Representation)
 * Based on GuildMember ORM structure
 */
typedef struct {
    int guild_id;                               /* Guild ID */
    int character_id;                           /* Character ID (charac_no) */
    int account_id;                             /* Account ID (m_id) */
    char character_name[21];                    /* Character name */
    char nick_name[GUILD_NICKNAME_LENGTH + 1];  /* Guild nickname */
    char memo[GUILD_MEMBER_MEMO_LENGTH + 1];    /* Member memo */
    signed char grade;                          /* Guild grade (0-3) */
    signed char job;                            /* Job class */
    signed char level;                          /* Character level */
    int member_point;                           /* Contribution points */
    time_t join_time;                           /* Join timestamp */
    time_t last_visit_time;                     /* Last visit time */
    int is_online;                              /* Online status */
} GuildMemberData;

/**
 * Pending Guild Invitation
 */
typedef struct {
    int guild_id;                               /* Guild ID */
    int inviter_id;                             /* Inviter character ID */
    int invitee_id;                             /* Invitee character ID */
    time_t invite_time;                         /* Invitation timestamp */
    int is_active;                              /* Active flag */
} PendingInvite;

/**
 * Guild Manager Global State
 */
typedef struct {
    int initialized;
    int max_guilds;

    /* Guild storage */
    GuildData* guilds;                          /* Guild array */
    int guild_count;                            /* Current guild count */

    /* Member-to-guild mapping (character_id -> guild_id) */
    int* character_guild_map;                   /* Array[10000] */
    int character_map_size;

    /* Pending invitations */
    PendingInvite* pending_invites;             /* Invite array */
    int invite_count;

    /* Statistics */
    struct {
        unsigned int total_guilds_created;
        unsigned int total_guilds_disbanded;
        unsigned int total_members_joined;
        unsigned int total_members_left;
        unsigned int total_members_kicked;
        unsigned int total_promotions;
        unsigned int total_leadership_transfers;
    } stats;
} GuildManagerState;

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

/**
 * Initialize Guild Manager
 *
 * @param max_guilds Maximum number of concurrent guilds
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_Initialize(int max_guilds);

/**
 * Cleanup Guild Manager
 */
void GuildManager_Cleanup(void);

/*==============================================================================
 * Guild Lifecycle Functions
 *============================================================================*/

/**
 * Create Guild
 *
 * @param leader_id Leader character ID
 * @param guild_name Guild name (max 40 chars)
 * @param out_guild Output buffer for created guild
 * @return Guild ID on success, negative error code otherwise
 */
int GuildManager_Create(int leader_id, const char* guild_name, GuildData* out_guild);

/**
 * Disband Guild
 *
 * @param guild_id Guild ID
 * @param leader_id Leader character ID (must match)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_Disband(int guild_id, int leader_id);

/*==============================================================================
 * Member Management Functions
 *============================================================================*/

/**
 * Invite Member
 *
 * @param guild_id Guild ID
 * @param inviter_id Inviter character ID
 * @param invitee_id Invitee character ID
 * @return Invite ID on success, negative error code otherwise
 */
int GuildManager_Invite(int guild_id, int inviter_id, int invitee_id);

/**
 * Accept Invitation
 *
 * @param invitee_id Invitee character ID
 * @param guild_id Guild ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_AcceptInvite(int invitee_id, int guild_id);

/**
 * Decline Invitation
 *
 * @param invitee_id Invitee character ID
 * @param guild_id Guild ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_DeclineInvite(int invitee_id, int guild_id);

/**
 * Leave Guild
 *
 * @param character_id Character ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_Leave(int character_id);

/**
 * Kick Member
 *
 * @param guild_id Guild ID
 * @param kicker_id Kicker character ID (must be leader/vice-leader)
 * @param target_id Target character ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_Kick(int guild_id, int kicker_id, int target_id);

/*==============================================================================
 * Role Management Functions
 *============================================================================*/

/**
 * Promote/Demote Member
 *
 * @param guild_id Guild ID
 * @param promoter_id Promoter character ID (must be leader)
 * @param target_id Target character ID
 * @param new_grade New guild grade (0-3)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_ChangeGrade(int guild_id, int promoter_id, int target_id, signed char new_grade);

/**
 * Transfer Leadership
 *
 * @param guild_id Guild ID
 * @param current_leader_id Current leader character ID
 * @param new_leader_id New leader character ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_TransferLeader(int guild_id, int current_leader_id, int new_leader_id);

/*==============================================================================
 * Query Functions
 *============================================================================*/

/**
 * Get Guild by ID
 *
 * @param guild_id Guild ID
 * @param out_guild Output buffer for guild data
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_GetGuild(int guild_id, GuildData* out_guild);

/**
 * Get Character's Guild ID
 *
 * @param character_id Character ID
 * @return Guild ID if member of a guild, 0 if not in guild, negative on error
 */
int GuildManager_GetCharacterGuild(int character_id);

/**
 * Get Guild Members
 *
 * @param guild_id Guild ID
 * @param members Output array for member data
 * @param max_count Maximum members to retrieve
 * @return Number of members retrieved, negative on error
 */
int GuildManager_GetMembers(int guild_id, GuildMemberData* members, int max_count);

/**
 * Get Member Count
 *
 * @param guild_id Guild ID
 * @return Member count on success, negative on error
 */
int GuildManager_GetMemberCount(int guild_id);

/**
 * Check if Character is Leader
 *
 * @param guild_id Guild ID
 * @param character_id Character ID
 * @return 1 if leader, 0 if not, negative on error
 */
int GuildManager_IsLeader(int guild_id, int character_id);

/**
 * Check if Guild Name Exists
 *
 * @param guild_name Guild name
 * @return 1 if exists, 0 if not, negative on error
 */
int GuildManager_GuildNameExists(const char* guild_name);

/*==============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * Update Member Online Status
 *
 * @param character_id Character ID
 * @param is_online Online status (1 = online, 0 = offline)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_UpdateMemberOnlineStatus(int character_id, int is_online);

/**
 * Add Member Contribution Points
 *
 * @param character_id Character ID
 * @param points Points to add
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_AddMemberPoints(int character_id, int points);

/**
 * Get Guild Statistics
 *
 * @param total_created Output: total guilds created
 * @param total_disbanded Output: total guilds disbanded
 * @param total_joined Output: total members joined
 * @return ERR_SUCCESS on success, error code otherwise
 */
int GuildManager_GetStatistics(unsigned int* total_created,
                                unsigned int* total_disbanded,
                                unsigned int* total_joined);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_MANAGER_H */
