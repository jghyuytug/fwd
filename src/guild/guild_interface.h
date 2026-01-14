/**
 * Guild Module - Public Interface
 * Purpose: Guild (公会) management, member coordination, guild progression
 *
 * Components:
 * - Guild creation and management
 * - Member recruitment and roles
 * - Guild permissions and hierarchy
 * - Guild funds and resources
 * - Guild statistics tracking
 */

#ifndef SRC_GUILD_GUILD_INTERFACE_H_
#define SRC_GUILD_GUILD_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Guild member roles
 */
typedef enum {
    GUILD_ROLE_MEMBER = 0,       // Regular member
    GUILD_ROLE_OFFICER = 1,      // Officer with limited permissions
    GUILD_ROLE_VICE_LEADER = 2,  // Vice leader with elevated permissions
    GUILD_ROLE_LEADER = 3        // Guild leader with full permissions
} GuildRole;

/**
 * Guild permissions
 */
typedef enum {
    GUILD_PERM_INVITE = 0,           // Invite new members
    GUILD_PERM_KICK = 1,             // Kick members
    GUILD_PERM_PROMOTE = 2,          // Promote members
    GUILD_PERM_DEMOTE = 3,           // Demote members
    GUILD_PERM_WITHDRAW_FUNDS = 4,   // Withdraw guild funds
    GUILD_PERM_MODIFY_SETTINGS = 5   // Modify guild settings
} GuildPermission;

/**
 * Guild member information structure
 */
typedef struct {
    unsigned int user_id;
    unsigned int guild_id;
    GuildRole role;
    unsigned int join_time;
    unsigned int contribution;      // Member contribution points
    int is_online;                  // 1 if member is online
} GuildMember;

/**
 * Guild information structure
 */
typedef struct {
    unsigned int guild_id;
    char guild_name[64];
    int guild_level;
    int member_count;
    int max_members;
    unsigned int guild_funds;       // Guild treasury
    unsigned int creation_time;
    unsigned int leader_id;
} GuildInfo;

/**
 * Guild statistics structure
 */
typedef struct {
    unsigned int total_guilds_created;
    unsigned int total_guilds_disbanded;
    unsigned int total_members_joined;
    unsigned int total_members_left;
    unsigned int total_members_kicked;
    unsigned int leadership_changes;
} GuildStats;

/**
 * Initialize Guild Module
 *
 * Sets up guild management system
 *
 * Parameters:
 *   max_guilds: Maximum number of guilds allowed
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_Initialize(int max_guilds);

/**
 * Cleanup Guild Module
 *
 * Releases guild system resources
 */
void Guild_Cleanup();

/**
 * Create Guild
 *
 * Creates a new guild with the given leader
 *
 * Parameters:
 *   leader_id: User ID of the guild leader
 *   guild_name: Name of the guild (max 63 characters)
 *
 * Returns: Guild ID on success, negative error code on failure
 */
int Guild_Create(unsigned int leader_id, const char* guild_name);

/**
 * Disband Guild
 *
 * Disbands an existing guild
 *
 * Parameters:
 *   guild_id: Guild ID to disband
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_Disband(unsigned int guild_id);

/**
 * Join Guild
 *
 * User joins a guild
 *
 * Parameters:
 *   user_id: User ID
 *   guild_id: Guild ID to join
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_Join(unsigned int user_id, unsigned int guild_id);

/**
 * Leave Guild
 *
 * User leaves their current guild
 *
 * Parameters:
 *   user_id: User ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_Leave(unsigned int user_id);

/**
 * Kick Member
 *
 * Removes a member from the guild
 *
 * Parameters:
 *   guild_id: Guild ID
 *   user_id: User ID to kick
 *   kicker_id: User ID of the person kicking
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_KickMember(unsigned int guild_id,
                     unsigned int user_id,
                     unsigned int kicker_id);

/**
 * Promote Member
 *
 * Changes a member's role in the guild
 *
 * Parameters:
 *   guild_id: Guild ID
 *   user_id: User ID to promote/demote
 *   new_role: New role to assign
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_PromoteMember(unsigned int guild_id,
                        unsigned int user_id,
                        GuildRole new_role);

/**
 * Transfer Leadership
 *
 * Transfers guild leadership to another member
 *
 * Parameters:
 *   guild_id: Guild ID
 *   old_leader_id: Current leader's user ID
 *   new_leader_id: New leader's user ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_TransferLeadership(unsigned int guild_id,
                             unsigned int old_leader_id,
                             unsigned int new_leader_id);

/**
 * Get Guild Info
 *
 * Retrieves guild information
 *
 * Parameters:
 *   guild_id: Guild ID
 *   guild_info: Output guild structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_GetInfo(unsigned int guild_id, GuildInfo* guild_info);

/**
 * Get Guild Members
 *
 * Retrieves list of guild members
 *
 * Parameters:
 *   guild_id: Guild ID
 *   members: Output array of member info
 *   max_members: Maximum members to retrieve
 *
 * Returns: Number of members retrieved
 */
int Guild_GetMembers(unsigned int guild_id,
                     GuildMember* members,
                     int max_members);

/**
 * Get Guild Statistics
 *
 * Retrieves global guild statistics
 *
 * Parameters:
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Guild_GetStatistics(GuildStats* stats);

#endif // SRC_GUILD_GUILD_INTERFACE_H_
