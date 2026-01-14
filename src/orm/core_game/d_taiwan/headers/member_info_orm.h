/**
 * DNF Game Server - MemberInfo ORM Header
 *
 * Function: Member account information management (registration, authentication, profile)
 * Database: test_d_taiwan
 * Table: member_info
 * Complexity: Medium (21 fields)
 */

#ifndef MEMBER_INFO_ORM_H
#define MEMBER_INFO_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Data Structure Definition
 * ======================================================================== */

/**
 * MemberInfo - Member account information
 *
 * Field descriptions:
 * - m_id: Member ID (Primary key, auto_increment)
 * - user_id: User login ID
 * - user_name: User real name
 * - first_ssn: First part of SSN (6 digits)
 * - second_ssn: Second part of SSN (7 digits)
 * - passwd: Password (MD5 hash, 32 chars)
 * - mobile_no: Mobile phone number
 * - reg_date: Registration date (Unix timestamp)
 * - email: Email address
 * - q_no: Security question number
 * - q_answer: Security question answer
 * - updt_date: Last update timestamp
 * - state: Account state (1=active, 0=inactive)
 * - nickname: Display nickname
 * - email_yn: Email notification (y/n)
 * - ssn_check: SSN verification status
 * - slot: Character slot count (default 8)
 * - last_play_time: Last play datetime
 * - hangame_flag: Hangame integration flag
 * - hanmon_flag: Hanmon integration flag
 * - m_type: Member type
 */
typedef struct {
    int m_id;                      /* Member ID (Primary key) */
    char user_id[31];              /* User login ID */
    char user_name[11];            /* User real name */
    char first_ssn[7];             /* First part of SSN */
    char second_ssn[8];            /* Second part of SSN */
    char passwd[33];               /* Password (MD5 hash) */
    char mobile_no[16];            /* Mobile phone number */
    int reg_date;                  /* Registration date (Unix timestamp) */
    char email[51];                /* Email address */
    int q_no;                      /* Security question number */
    char q_answer[31];             /* Security question answer */
    char updt_date[32];            /* Last update timestamp */
    int state;                     /* Account state */
    char nickname[17];             /* Display nickname */
    char email_yn[2];              /* Email notification (y/n) */
    int ssn_check;                 /* SSN verification status */
    unsigned int slot;             /* Character slot count */
    char last_play_time[32];       /* Last play datetime */
    int hangame_flag;              /* Hangame integration flag */
    int hanmon_flag;               /* Hanmon integration flag */
    int m_type;                    /* Member type */
} MemberInfo;

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

/**
 * Create a new member account
 *
 * @param manager Database connection manager
 * @param member Member information structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberInfo_Create(DBConnectionManager* manager, MemberInfo* member);

/**
 * Query member by m_id
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param member Output parameter to store query result
 * @return 0 on success, -1 on failure
 */
int MemberInfo_GetByID(DBConnectionManager* manager, int m_id, MemberInfo* member);

/**
 * Query member by user_id
 *
 * @param manager Database connection manager
 * @param user_id User login ID
 * @param member Output parameter to store query result
 * @return 0 on success, -1 on failure
 */
int MemberInfo_GetByUserID(DBConnectionManager* manager, const char* user_id, MemberInfo* member);

/**
 * Query member by nickname
 *
 * @param manager Database connection manager
 * @param nickname Display nickname
 * @param member Output parameter to store query result
 * @return 0 on success, -1 on failure
 */
int MemberInfo_GetByNickname(DBConnectionManager* manager, const char* nickname, MemberInfo* member);

/**
 * Update member information
 *
 * @param manager Database connection manager
 * @param member Member information structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberInfo_Update(DBConnectionManager* manager, MemberInfo* member);

/**
 * Delete member account
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberInfo_Delete(DBConnectionManager* manager, int m_id);

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

/**
 * Check if member exists by m_id
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return true if exists, false otherwise
 */
bool MemberInfo_Exists(DBConnectionManager* manager, int m_id);

/**
 * Check if user_id exists
 *
 * @param manager Database connection manager
 * @param user_id User login ID
 * @return true if exists, false otherwise
 */
bool MemberInfo_UserIDExists(DBConnectionManager* manager, const char* user_id);

/**
 * Check if nickname exists
 *
 * @param manager Database connection manager
 * @param nickname Display nickname
 * @return true if exists, false otherwise
 */
bool MemberInfo_NicknameExists(DBConnectionManager* manager, const char* nickname);

/**
 * Verify password
 *
 * @param manager Database connection manager
 * @param user_id User login ID
 * @param passwd Password to verify (MD5 hash)
 * @return true if password matches, false otherwise
 */
bool MemberInfo_VerifyPassword(DBConnectionManager* manager, const char* user_id, const char* passwd);

/**
 * Update password
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param new_passwd New password (MD5 hash)
 * @return 0 on success, -1 on failure
 */
int MemberInfo_UpdatePassword(DBConnectionManager* manager, int m_id, const char* new_passwd);

/**
 * Update account state
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param state New state (1=active, 0=inactive)
 * @return 0 on success, -1 on failure
 */
int MemberInfo_UpdateState(DBConnectionManager* manager, int m_id, int state);

/**
 * Update last play time
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberInfo_UpdateLastPlayTime(DBConnectionManager* manager, int m_id);

/**
 * Get total member count
 *
 * @param manager Database connection manager
 * @return Member count on success, -1 on failure
 */
int MemberInfo_GetCount(DBConnectionManager* manager);

/**
 * Get active member count
 *
 * @param manager Database connection manager
 * @return Active member count on success, -1 on failure
 */
int MemberInfo_GetActiveCount(DBConnectionManager* manager);

/**
 * Search members by user_name (fuzzy match)
 *
 * @param manager Database connection manager
 * @param user_name User real name to search
 * @param members Output array to store results
 * @param max_count Maximum number of results
 * @return Actual count on success, -1 on failure
 */
int MemberInfo_SearchByUserName(DBConnectionManager* manager, const char* user_name, MemberInfo* members, int max_count);

/**
 * Get recently registered members
 *
 * @param manager Database connection manager
 * @param members Output array to store results
 * @param max_count Maximum number of results
 * @return Actual count on success, -1 on failure
 */
int MemberInfo_GetRecentlyRegistered(DBConnectionManager* manager, MemberInfo* members, int max_count);

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

/**
 * Initialize MemberInfo structure
 *
 * @param member Structure pointer
 */
void MemberInfo_Init(MemberInfo* member);

/**
 * Print member information (for debugging)
 *
 * @param member Structure pointer
 */
void MemberInfo_Print(const MemberInfo* member);

/**
 * Copy member information
 *
 * @param dest Destination structure pointer
 * @param src Source structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberInfo_Copy(MemberInfo* dest, const MemberInfo* src);

#ifdef __cplusplus
}
#endif

#endif /* MEMBER_INFO_ORM_H */
