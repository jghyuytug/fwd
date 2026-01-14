/**
 * DNF Game Server - MemberLogin ORM Header
 *
 * Function: Member login session management (login tracking, security, limits)
 * Database: test_taiwan_login
 * Table: member_login
 * Complexity: Medium (21 fields)
 */

#ifndef MEMBER_LOGIN_ORM_H
#define MEMBER_LOGIN_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Data Structure Definition
 * ======================================================================== */

/**
 * MemberLogin - Member login session information
 *
 * Field descriptions:
 * - m_id: Member ID (Primary key)
 * - login_time: Login timestamp (Unix time)
 * - expire_time: Session expire timestamp
 * - last_play_time: Last play timestamp
 * - total_account_fail: Total login failure count
 * - account_fail: Current login failure count
 * - report_cnt: Report count
 * - reliable_flag: Reliability flag
 * - trade_gold_daily: Daily trade gold limit
 * - last_gift_time: Last gift time
 * - gift_cnt: Gift count
 * - login_ip: Login IP address
 * - security_flag: Security flag
 * - power_side: Power side/faction
 * - dungeon_gain_gold: Dungeon gold earnings
 * - school_id: School ID
 * - rating: Rating score
 * - cleanpad_point: Cleanpad points
 * - tutorial_skipable: Tutorial skip flag
 * - event_charac_flag: Event character flag
 * - garena_token_key: Garena token key
 */
typedef struct {
    int m_id;                      /* Member ID (Primary key) */
    unsigned int login_time;       /* Login timestamp */
    unsigned int expire_time;      /* Session expire timestamp */
    unsigned int last_play_time;   /* Last play timestamp */
    unsigned int total_account_fail; /* Total login failure count */
    int account_fail;              /* Current login failure count */
    int report_cnt;                /* Report count */
    int reliable_flag;             /* Reliability flag */
    unsigned int trade_gold_daily; /* Daily trade gold limit */
    unsigned int last_gift_time;   /* Last gift time */
    unsigned short gift_cnt;       /* Gift count */
    char login_ip[16];             /* Login IP address */
    int security_flag;             /* Security flag */
    int power_side;                /* Power side/faction */
    unsigned int dungeon_gain_gold;/* Dungeon gold earnings */
    int school_id;                 /* School ID */
    float rating;                  /* Rating score */
    unsigned int cleanpad_point;   /* Cleanpad points */
    char tutorial_skipable[2];     /* Tutorial skip flag */
    unsigned int event_charac_flag;/* Event character flag */
    long long garena_token_key;    /* Garena token key */
} MemberLogin;

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

/**
 * Create a new member login session
 *
 * @param manager Database connection manager
 * @param login Login session structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberLogin_Create(DBConnectionManager* manager, MemberLogin* login);

/**
 * Query member login by m_id
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param login Output parameter to store query result
 * @return 0 on success, -1 on failure
 */
int MemberLogin_GetByID(DBConnectionManager* manager, int m_id, MemberLogin* login);

/**
 * Update member login information
 *
 * @param manager Database connection manager
 * @param login Login session structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberLogin_Update(DBConnectionManager* manager, MemberLogin* login);

/**
 * Delete member login session
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberLogin_Delete(DBConnectionManager* manager, int m_id);

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

/**
 * Check if member login session exists
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return true if exists, false otherwise
 */
bool MemberLogin_Exists(DBConnectionManager* manager, int m_id);

/**
 * Update login time to current time
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param login_ip Login IP address
 * @return 0 on success, -1 on failure
 */
int MemberLogin_UpdateLoginTime(DBConnectionManager* manager, int m_id, const char* login_ip);

/**
 * Update last play time to current time
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberLogin_UpdateLastPlayTime(DBConnectionManager* manager, int m_id);

/**
 * Increment login failure count
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberLogin_IncrementFailCount(DBConnectionManager* manager, int m_id);

/**
 * Reset login failure count
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @return 0 on success, -1 on failure
 */
int MemberLogin_ResetFailCount(DBConnectionManager* manager, int m_id);

/**
 * Update trade gold daily limit
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param gold_amount Gold amount
 * @return 0 on success, -1 on failure
 */
int MemberLogin_UpdateTradeGold(DBConnectionManager* manager, int m_id, unsigned int gold_amount);

/**
 * Update dungeon gold earnings
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param gold_amount Gold amount to add
 * @return 0 on success, -1 on failure
 */
int MemberLogin_AddDungeonGold(DBConnectionManager* manager, int m_id, unsigned int gold_amount);

/**
 * Update security flag
 *
 * @param manager Database connection manager
 * @param m_id Member ID
 * @param flag Security flag value
 * @return 0 on success, -1 on failure
 */
int MemberLogin_UpdateSecurityFlag(DBConnectionManager* manager, int m_id, int flag);

/**
 * Get members by login IP (security check)
 *
 * @param manager Database connection manager
 * @param login_ip Login IP address
 * @param m_ids Output array to store member IDs
 * @param max_count Maximum number of results
 * @return Actual count on success, -1 on failure
 */
int MemberLogin_GetByLoginIP(DBConnectionManager* manager, const char* login_ip, int* m_ids, int max_count);

/**
 * Get total session count
 *
 * @param manager Database connection manager
 * @return Session count on success, -1 on failure
 */
int MemberLogin_GetCount(DBConnectionManager* manager);

/**
 * Get active session count (not expired)
 *
 * @param manager Database connection manager
 * @param current_time Current timestamp for comparison
 * @return Active session count on success, -1 on failure
 */
int MemberLogin_GetActiveCount(DBConnectionManager* manager, unsigned int current_time);

/**
 * Get members with high failure count (security check)
 *
 * @param manager Database connection manager
 * @param threshold Failure count threshold
 * @param logins Output array to store results
 * @param max_count Maximum number of results
 * @return Actual count on success, -1 on failure
 */
int MemberLogin_GetHighFailureAccounts(DBConnectionManager* manager, int threshold, MemberLogin* logins, int max_count);

/**
 * Get recently logged in members
 *
 * @param manager Database connection manager
 * @param logins Output array to store results
 * @param max_count Maximum number of results
 * @return Actual count on success, -1 on failure
 */
int MemberLogin_GetRecentlyLoggedIn(DBConnectionManager* manager, MemberLogin* logins, int max_count);

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

/**
 * Initialize MemberLogin structure
 *
 * @param login Structure pointer
 */
void MemberLogin_Init(MemberLogin* login);

/**
 * Print member login information (for debugging)
 *
 * @param login Structure pointer
 */
void MemberLogin_Print(const MemberLogin* login);

/**
 * Copy member login information
 *
 * @param dest Destination structure pointer
 * @param src Source structure pointer
 * @return 0 on success, -1 on failure
 */
int MemberLogin_Copy(MemberLogin* dest, const MemberLogin* src);

#ifdef __cplusplus
}
#endif

#endif /* MEMBER_LOGIN_ORM_H */
