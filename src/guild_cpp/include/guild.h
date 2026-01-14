/*
 * guild.h - Guild Service Definitions
 *
 * Guild service handles all guild-related operations across the server cluster.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#ifndef GUILD_H
#define GUILD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Configuration Constants
 *============================================================================*/

#define GUILD_MAX_NAME_LEN      32
#define GUILD_MAX_MEMBERS       50
#define GUILD_MAX_INTRO_LEN     256
#define GUILD_MAX_NOTICE_LEN    512
#define GUILD_MAX_SKILLS        20
#define GUILD_MAX_WAREHOUSE     100

/*==============================================================================
 * Guild Rank Definitions
 *============================================================================*/

typedef enum {
    GUILD_RANK_MASTER = 0,       /* Guild master */
    GUILD_RANK_VICE_MASTER = 1,  /* Vice master */
    GUILD_RANK_ELITE = 2,        /* Elite member */
    GUILD_RANK_MEMBER = 3,       /* Normal member */
    GUILD_RANK_NONE = -1         /* Not in guild */
} GuildRank;

/*==============================================================================
 * Guild Status
 *============================================================================*/

typedef enum {
    GUILD_STATUS_ACTIVE = 0,     /* Active guild */
    GUILD_STATUS_DISBANDED = 1,  /* Disbanded */
    GUILD_STATUS_PENDING = 2     /* Pending creation */
} GuildStatus;

/*==============================================================================
 * Guild Structures
 *============================================================================*/

/* Guild basic information */
typedef struct {
    uint32_t guild_id;
    int8_t   server_id;
    char     guild_name[GUILD_MAX_NAME_LEN + 1];
    uint32_t master_id;          /* Account ID of master */
    uint32_t master_charac_no;   /* Character ID of master */
    char     master_name[21];
    int32_t  level;
    uint32_t exp;
    uint32_t guild_point;
    int32_t  member_count;
    int32_t  max_members;
    int8_t   status;             /* GuildStatus */
    uint32_t create_time;
    char     introduction[GUILD_MAX_INTRO_LEN + 1];
    char     notice[GUILD_MAX_NOTICE_LEN + 1];
    uint32_t fund;               /* Guild treasury */
} GuildInfo;

/* Guild member information */
typedef struct {
    uint32_t charac_no;
    uint32_t guild_id;
    char     charac_name[21];
    int8_t   rank;               /* GuildRank */
    uint32_t contribution;
    uint32_t join_time;
    uint32_t last_online;
    uint8_t  is_online;
} GuildMember;

/* Guild skill */
typedef struct {
    uint32_t guild_id;
    uint16_t skill_id;
    uint8_t  skill_level;
} GuildSkill;

/* Guild warehouse item */
typedef struct {
    uint32_t guild_id;
    int32_t  slot;
    uint32_t item_id;
    int32_t  count;
    uint32_t depositor_id;       /* Who deposited */
    uint32_t deposit_time;
} GuildWarehouseItem;

/* Guild join request */
typedef struct {
    uint32_t request_id;
    uint32_t guild_id;
    uint32_t charac_no;
    char     charac_name[21];
    uint8_t  level;
    uint8_t  job;
    uint32_t request_time;
    uint8_t  status;             /* 0=pending, 1=accepted, 2=rejected */
} GuildJoinRequest;

/*==============================================================================
 * Guild Service Message Types (IPC between Game <-> Guild)
 *============================================================================*/

typedef enum {
    GMSG_CREATE_GUILD = 1,
    GMSG_DISBAND_GUILD,
    GMSG_GET_GUILD_INFO,
    GMSG_JOIN_GUILD,
    GMSG_LEAVE_GUILD,
    GMSG_KICK_MEMBER,
    GMSG_CHANGE_RANK,
    GMSG_UPDATE_NOTICE,
    GMSG_UPDATE_INTRO,
    GMSG_ADD_EXP,
    GMSG_LEARN_SKILL,
    GMSG_DEPOSIT_ITEM,
    GMSG_WITHDRAW_ITEM,
    GMSG_DEPOSIT_FUND,
    GMSG_WITHDRAW_FUND,
    GMSG_GET_MEMBER_LIST,
    GMSG_GET_JOIN_REQUESTS,
    GMSG_ACCEPT_JOIN,
    GMSG_REJECT_JOIN,
    GMSG_SEARCH_GUILD,
    GMSG_MEMBER_ONLINE,
    GMSG_MEMBER_OFFLINE,
    GMSG_BROADCAST          /* Broadcast to all guild members */
} GuildMessageType;

/*==============================================================================
 * Guild Service Functions
 *============================================================================*/

/* Initialization */
int guild_service_init(const char* config_file);
void guild_service_cleanup(void);
int guild_service_run(void);

/* Guild CRUD operations */
int guild_create(uint32_t master_id, uint32_t master_charac_no,
                 const char* master_name, const char* guild_name,
                 uint32_t* out_guild_id);
int guild_disband(uint32_t guild_id, uint32_t requester_id);
int guild_get_info(uint32_t guild_id, GuildInfo* info);
int guild_get_by_name(const char* guild_name, GuildInfo* info);
int guild_update_info(const GuildInfo* info);

/* Member operations */
int guild_add_member(uint32_t guild_id, uint32_t charac_no,
                     const char* charac_name, int8_t rank);
int guild_remove_member(uint32_t guild_id, uint32_t charac_no);
int guild_change_member_rank(uint32_t guild_id, uint32_t charac_no, int8_t new_rank);
int guild_get_members(uint32_t guild_id, GuildMember* members, int max_count);
int guild_get_member_count(uint32_t guild_id);
int guild_is_member(uint32_t guild_id, uint32_t charac_no);
int8_t guild_get_member_rank(uint32_t guild_id, uint32_t charac_no);

/* Join request operations */
int guild_request_join(uint32_t guild_id, uint32_t charac_no,
                       const char* charac_name, uint8_t level, uint8_t job);
int guild_accept_join(uint32_t guild_id, uint32_t charac_no);
int guild_reject_join(uint32_t guild_id, uint32_t charac_no);
int guild_get_join_requests(uint32_t guild_id, GuildJoinRequest* requests, int max_count);

/* Guild progression */
int guild_add_exp(uint32_t guild_id, uint32_t exp);
int guild_add_exp_full(uint32_t guild_id, uint32_t exp, uint8_t* out_level, uint8_t* out_leveled_up);
int guild_add_point(uint32_t guild_id, uint32_t point);
int guild_check_level_up(uint32_t guild_id);

/* Guild skills */
int guild_learn_skill(uint32_t guild_id, uint16_t skill_id);
int guild_get_skills(uint32_t guild_id, GuildSkill* skills, int max_count);
int guild_get_skill_level(uint32_t guild_id, uint16_t skill_id);

/* Guild warehouse */
int guild_deposit_item(uint32_t guild_id, uint32_t charac_no,
                       uint32_t item_id, int count, int32_t* out_slot);
int guild_withdraw_item(uint32_t guild_id, uint32_t charac_no, int32_t slot, int count);
int guild_get_warehouse(uint32_t guild_id, GuildWarehouseItem* items, int max_count);
int guild_deposit_fund(uint32_t guild_id, uint32_t charac_no, uint32_t amount);
int guild_withdraw_fund(uint32_t guild_id, uint32_t charac_no, uint32_t amount);

/* Member status */
int guild_set_member_online(uint32_t guild_id, uint32_t charac_no);
int guild_set_member_offline(uint32_t guild_id, uint32_t charac_no);
int guild_add_contribution(uint32_t guild_id, uint32_t charac_no, uint32_t amount);

/* Notice and introduction */
int guild_set_notice(uint32_t guild_id, const char* notice);
int guild_set_introduction(uint32_t guild_id, const char* intro);

/* Search */
int guild_search(const char* keyword, GuildInfo* results, int max_count);
int guild_get_recommend_list(GuildInfo* results, int max_count);

/* Utility */
int guild_get_exp_for_level(int level);
int guild_get_max_members_for_level(int level);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_H */
