/**
 * DNF Game Server - GuildMember ORM Header
 *
 * Function: Guild member management (members, grades, activity tracking)
 * Database: test_d_guild
 * Table: guild_member
 * Complexity: Medium-High (24 fields, composite key)
 */

#ifndef GUILD_MEMBER_ORM_H
#define GUILD_MEMBER_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Data Structure Definition
 * ======================================================================== */

/**
 * GuildMember - Guild member information
 *
 * Field descriptions:
 * - guild_id: Guild ID (Composite primary key)
 * - m_id: Member ID
 * - server_id: Server ID
 * - charac_no: Character number (Composite primary key)
 * - charac_name: Character name
 * - memo: Personal memo
 * - grade: Member grade/rank
 * - job: Character job class
 * - grow_type: Character growth type
 * - lev: Character level
 * - age: Character age
 * - born_year: Birth year
 * - sex: Character sex (M/F)
 * - apply_time: Application datetime
 * - member_time: Join datetime
 * - member_flag: Membership status flag
 * - bbs_cnt: BBS post count
 * - last_visit_time: Last visit datetime
 * - secede_type: Secession type
 * - secede_time: Secession datetime
 * - member_point: Current member points
 * - member_point_prev: Previous member points
 * - last_play_time: Last play datetime
 * - nick_name: Display nickname
 */
typedef struct {
    int guild_id;                  /* Guild ID (Composite PK) */
    int m_id;                      /* Member ID */
    char server_id;                /* Server ID */
    int charac_no;                 /* Character number (Composite PK) */
    char charac_name[21];          /* Character name */
    char memo[31];                 /* Personal memo */
    char grade;                    /* Member grade/rank */
    char job;                      /* Job class */
    char grow_type;                /* Growth type */
    char lev;                      /* Character level */
    char age;                      /* Character age */
    char born_year[3];             /* Birth year */
    char sex[2];                   /* Sex (M/F) */
    char apply_time[32];           /* Application datetime */
    char member_time[32];          /* Join datetime */
    char member_flag;              /* Membership status */
    unsigned short bbs_cnt;        /* BBS post count */
    char last_visit_time[32];      /* Last visit datetime */
    char secede_type;              /* Secession type */
    char secede_time[32];          /* Secession datetime */
    unsigned int member_point;     /* Current points */
    unsigned int member_point_prev;/* Previous points */
    char last_play_time[32];       /* Last play datetime */
    char nick_name[13];            /* Display nickname */
} GuildMember;

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

/**
 * Create a new guild member record
 *
 * @param manager Database connection manager
 * @param member Guild member structure pointer
 * @return 0 on success, -1 on failure
 */
int GuildMember_Create(DBConnectionManager* manager, GuildMember* member);

/**
 * Query guild member by composite key (guild_id, charac_no)
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param member Output parameter to store query result
 * @return 0 on success, -1 on failure
 */
int GuildMember_GetByKey(DBConnectionManager* manager, int guild_id, int charac_no, GuildMember* member);

/**
 * Update guild member information
 *
 * @param manager Database connection manager
 * @param member Guild member structure pointer
 * @return 0 on success, -1 on failure
 */
int GuildMember_Update(DBConnectionManager* manager, GuildMember* member);

/**
 * Delete guild member record
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int GuildMember_Delete(DBConnectionManager* manager, int guild_id, int charac_no);

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

/**
 * Check if guild member exists
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @return true if exists, false otherwise
 */
bool GuildMember_Exists(DBConnectionManager* manager, int guild_id, int charac_no);

/**
 * Get all members of a guild
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param members Output array to store members
 * @param max_count Maximum number of members to retrieve
 * @return Actual count on success, -1 on failure
 */
int GuildMember_GetByGuildID(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count);

/**
 * Get member by character number
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param member Output parameter to store result
 * @return 0 on success, -1 on failure
 */
int GuildMember_GetByCharacNo(DBConnectionManager* manager, int charac_no, GuildMember* member);

/**
 * Get member by character name
 *
 * @param manager Database connection manager
 * @param charac_name Character name
 * @param member Output parameter to store result
 * @return 0 on success, -1 on failure
 */
int GuildMember_GetByCharacName(DBConnectionManager* manager, const char* charac_name, GuildMember* member);

/**
 * Update member grade/rank
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param grade New grade
 * @return 0 on success, -1 on failure
 */
int GuildMember_UpdateGrade(DBConnectionManager* manager, int guild_id, int charac_no, char grade);

/**
 * Update member memo
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param memo New memo text
 * @return 0 on success, -1 on failure
 */
int GuildMember_UpdateMemo(DBConnectionManager* manager, int guild_id, int charac_no, const char* memo);

/**
 * Update member flag
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param flag New flag value
 * @return 0 on success, -1 on failure
 */
int GuildMember_UpdateFlag(DBConnectionManager* manager, int guild_id, int charac_no, char flag);

/**
 * Update last visit time to current time
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int GuildMember_UpdateLastVisit(DBConnectionManager* manager, int guild_id, int charac_no);

/**
 * Increment BBS post count
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int GuildMember_IncrementBBSCount(DBConnectionManager* manager, int guild_id, int charac_no);

/**
 * Update member points
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param points New point value
 * @return 0 on success, -1 on failure
 */
int GuildMember_UpdatePoints(DBConnectionManager* manager, int guild_id, int charac_no, unsigned int points);

/**
 * Get total member count for a guild
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @return Member count on success, -1 on failure
 */
int GuildMember_GetCount(DBConnectionManager* manager, int guild_id);

/**
 * Get active members (member_flag != 0)
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param members Output array to store members
 * @param max_count Maximum number of members
 * @return Actual count on success, -1 on failure
 */
int GuildMember_GetActiveMembers(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count);

/**
 * Get members by grade
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param grade Grade to filter
 * @param members Output array to store members
 * @param max_count Maximum number of members
 * @return Actual count on success, -1 on failure
 */
int GuildMember_GetByGrade(DBConnectionManager* manager, int guild_id, char grade, GuildMember* members, int max_count);

/**
 * Get top members by points
 *
 * @param manager Database connection manager
 * @param guild_id Guild ID
 * @param members Output array to store members
 * @param max_count Maximum number of members
 * @return Actual count on success, -1 on failure
 */
int GuildMember_GetTopByPoints(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count);

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

/**
 * Initialize GuildMember structure
 *
 * @param member Structure pointer
 */
void GuildMember_Init(GuildMember* member);

/**
 * Print guild member information (for debugging)
 *
 * @param member Structure pointer
 */
void GuildMember_Print(const GuildMember* member);

/**
 * Copy guild member information
 *
 * @param dest Destination structure pointer
 * @param src Source structure pointer
 * @return 0 on success, -1 on failure
 */
int GuildMember_Copy(GuildMember* dest, const GuildMember* src);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_MEMBER_ORM_H */
