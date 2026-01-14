/**
 * Party System Interface
 *
 * Provides party/group management functionality including:
 * - Party creation and dissolution
 * - Member management (join, leave, kick)
 * - Leader management
 * - Party attributes (exp bonus, drop rate bonus, etc.)
 * - Party settings and permissions
 */

#ifndef PARTY_INTERFACE_H
#define PARTY_INTERFACE_H

#include "../../include/common/error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Constants
 *============================================================================*/

#define PARTY_MAX_MEMBERS           4      /* Maximum party size */
#define PARTY_NAME_MAX_LENGTH       32     /* Maximum party name length */
#define PARTY_PASSWORD_MAX_LENGTH   16     /* Maximum party password length */

/*==============================================================================
 * Enumerations
 *============================================================================*/

/**
 * Party Type
 */
typedef enum {
    PARTY_TYPE_NORMAL = 0,          /* Normal party */
    PARTY_TYPE_QUICK = 1,           /* Quick match party */
    PARTY_TYPE_GUILD = 2,           /* Guild party */
    PARTY_TYPE_RAID = 3             /* Raid party (not used in DNF) */
} PartyType;

/**
 * Loot Distribution Mode
 */
typedef enum {
    LOOT_MODE_FREE_FOR_ALL = 0,     /* Anyone can loot */
    LOOT_MODE_ROUND_ROBIN = 1,      /* Round-robin distribution */
    LOOT_MODE_MASTER_LOOT = 2,      /* Leader distributes */
    LOOT_MODE_NEED_BEFORE_GREED = 3 /* Need/Greed system */
} LootMode;

/**
 * Party Member Role
 */
typedef enum {
    PARTY_ROLE_MEMBER = 0,          /* Regular member */
    PARTY_ROLE_LEADER = 1           /* Party leader */
} PartyRole;

/**
 * Party Join Permission
 */
typedef enum {
    PARTY_PERMISSION_OPEN = 0,      /* Anyone can join */
    PARTY_PERMISSION_APPROVAL = 1,  /* Requires approval */
    PARTY_PERMISSION_PASSWORD = 2,  /* Requires password */
    PARTY_PERMISSION_CLOSED = 3     /* No one can join */
} PartyPermission;

/*==============================================================================
 * Structures
 *============================================================================*/

/**
 * Party Member Information
 */
typedef struct {
    unsigned int character_id;      /* Character ID */
    char character_name[32];        /* Character name */
    int job_class;                  /* Job class */
    int level;                      /* Character level */
    PartyRole role;                 /* Member role */
    unsigned int join_time;         /* Join timestamp */
    int is_online;                  /* Online status */
    int current_hp;                 /* Current HP */
    int max_hp;                     /* Maximum HP */
    int current_mp;                 /* Current MP */
    int max_mp;                     /* Maximum MP */
} PartyMember;

/**
 * Party Settings
 */
typedef struct {
    LootMode loot_mode;             /* Loot distribution mode */
    PartyPermission permission;     /* Join permission */
    char password[PARTY_PASSWORD_MAX_LENGTH]; /* Party password */
    int auto_accept_requests;       /* Auto-accept join requests */
    int allow_cross_channel;        /* Allow cross-channel members */
} PartySettings;

/**
 * Party Bonuses
 */
typedef struct {
    float exp_bonus_rate;           /* Experience bonus (percentage) */
    float drop_rate_bonus;          /* Drop rate bonus (percentage) */
    float gold_bonus_rate;          /* Gold bonus (percentage) */
    int damage_bonus;               /* Damage bonus (flat) */
    int defense_bonus;              /* Defense bonus (flat) */
} PartyBonuses;

/**
 * Party Information
 */
typedef struct {
    int party_id;                   /* Party ID */
    char party_name[PARTY_NAME_MAX_LENGTH]; /* Party name */
    PartyType party_type;           /* Party type */
    unsigned int leader_id;         /* Leader character ID */
    int member_count;               /* Current member count */
    PartyMember members[PARTY_MAX_MEMBERS]; /* Member array */
    PartySettings settings;         /* Party settings */
    PartyBonuses bonuses;           /* Party bonuses */
    unsigned int created_time;      /* Creation timestamp */
    int dungeon_id;                 /* Current dungeon (0 if none) */
} PartyInfo;

/**
 * Party Statistics
 */
typedef struct {
    unsigned int total_exp_gained;  /* Total experience gained */
    unsigned int total_gold_gained; /* Total gold gained */
    unsigned int total_items_looted; /* Total items looted */
    unsigned int dungeons_completed; /* Dungeons completed together */
    unsigned int monsters_killed;   /* Monsters killed */
    unsigned int total_playtime;    /* Total playtime (seconds) */
} PartyStats;

/*==============================================================================
 * Module Management Functions
 *============================================================================*/

/**
 * Initialize Party Module
 *
 * @param max_parties Maximum number of concurrent parties
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_Initialize(int max_parties);

/**
 * Cleanup Party Module
 */
void Party_Cleanup(void);

/*==============================================================================
 * Party Management Functions
 *============================================================================*/

/**
 * Create Party
 *
 * @param leader_id Leader character ID
 * @param party_name Party name (can be NULL)
 * @param party_type Party type
 * @return Party ID on success, negative error code otherwise
 */
int Party_Create(unsigned int leader_id,
                 const char* party_name,
                 PartyType party_type);

/**
 * Disband Party
 *
 * @param party_id Party ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_Disband(int party_id);

/**
 * Join Party
 *
 * @param party_id Party ID
 * @param character_id Character ID
 * @param password Party password (can be NULL)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_Join(int party_id,
               unsigned int character_id,
               const char* password);

/**
 * Leave Party
 *
 * @param party_id Party ID
 * @param character_id Character ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_Leave(int party_id, unsigned int character_id);

/**
 * Kick Member
 *
 * @param party_id Party ID
 * @param kicker_id Character ID of the kicker (must be leader)
 * @param target_id Character ID to kick
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_KickMember(int party_id,
                     unsigned int kicker_id,
                     unsigned int target_id);

/**
 * Transfer Leadership
 *
 * @param party_id Party ID
 * @param current_leader_id Current leader character ID
 * @param new_leader_id New leader character ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_TransferLeadership(int party_id,
                             unsigned int current_leader_id,
                             unsigned int new_leader_id);

/*==============================================================================
 * Party Settings Functions
 *============================================================================*/

/**
 * Set Party Settings
 *
 * @param party_id Party ID
 * @param settings Party settings
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_SetSettings(int party_id, const PartySettings* settings);

/**
 * Get Party Settings
 *
 * @param party_id Party ID
 * @param settings Output buffer for settings
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_GetSettings(int party_id, PartySettings* settings);

/**
 * Set Loot Mode
 *
 * @param party_id Party ID
 * @param loot_mode Loot distribution mode
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_SetLootMode(int party_id, LootMode loot_mode);

/**
 * Set Party Password
 *
 * @param party_id Party ID
 * @param password New password (NULL to remove)
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_SetPassword(int party_id, const char* password);

/*==============================================================================
 * Party Information Functions
 *============================================================================*/

/**
 * Get Party Info
 *
 * @param party_id Party ID
 * @param info Output buffer for party information
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_GetInfo(int party_id, PartyInfo* info);

/**
 * Get Party Members
 *
 * @param party_id Party ID
 * @param members Output buffer for member array
 * @param max_members Maximum number of members to retrieve
 * @return Number of members on success, negative error code otherwise
 */
int Party_GetMembers(int party_id,
                     PartyMember* members,
                     int max_members);

/**
 * Get Party Count
 *
 * @return Current number of active parties
 */
int Party_GetActiveCount(void);

/**
 * Is Member In Party
 *
 * @param character_id Character ID
 * @return Party ID if in a party, 0 if not in party, negative on error
 */
int Party_IsCharacterInParty(unsigned int character_id);

/**
 * Is Party Leader
 *
 * @param party_id Party ID
 * @param character_id Character ID
 * @return 1 if leader, 0 if not leader, negative on error
 */
int Party_IsLeader(int party_id, unsigned int character_id);

/*==============================================================================
 * Party Bonuses Functions
 *============================================================================*/

/**
 * Calculate Party Bonuses
 *
 * Automatically calculates bonuses based on party composition
 *
 * @param party_id Party ID
 * @param bonuses Output buffer for bonuses
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_CalculateBonuses(int party_id, PartyBonuses* bonuses);

/**
 * Get Party Bonuses
 *
 * @param party_id Party ID
 * @param bonuses Output buffer for bonuses
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_GetBonuses(int party_id, PartyBonuses* bonuses);

/*==============================================================================
 * Party Statistics Functions
 *============================================================================*/

/**
 * Get Party Statistics
 *
 * @param party_id Party ID
 * @param stats Output buffer for statistics
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_GetStatistics(int party_id, PartyStats* stats);

/**
 * Update Party Statistics
 *
 * Called when party members gain exp, gold, or complete dungeons
 *
 * @param party_id Party ID
 * @param exp_gained Experience gained
 * @param gold_gained Gold gained
 * @param items_looted Items looted
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_UpdateStatistics(int party_id,
                          unsigned int exp_gained,
                          unsigned int gold_gained,
                          unsigned int items_looted);

/*==============================================================================
 * Party Utility Functions
 *============================================================================*/

/**
 * Update Party State
 *
 * Called periodically to update party state
 *
 * @param delta_time_ms Time elapsed since last update (milliseconds)
 * @return Number of parties updated
 */
int Party_Update(int delta_time_ms);

/**
 * Broadcast to Party
 *
 * Sends a message to all party members
 *
 * @param party_id Party ID
 * @param message Message to broadcast
 * @return Number of members notified
 */
int Party_Broadcast(int party_id, const char* message);

/**
 * Update Member Status
 *
 * Updates a member's HP/MP status
 *
 * @param party_id Party ID
 * @param character_id Character ID
 * @param current_hp Current HP
 * @param max_hp Maximum HP
 * @param current_mp Current MP
 * @param max_mp Maximum MP
 * @return ERR_SUCCESS on success, error code otherwise
 */
int Party_UpdateMemberStatus(int party_id,
                             unsigned int character_id,
                             int current_hp,
                             int max_hp,
                             int current_mp,
                             int max_mp);

#ifdef __cplusplus
}
#endif

#endif /* PARTY_INTERFACE_H */
