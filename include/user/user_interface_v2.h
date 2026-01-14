/**
 * User Module - Public Interface (Updated for Phase 6.1)
 * Purpose: User management, character handling, and authentication
 *
 * This is the main public API for the User module. All external modules
 * should use these functions instead of directly accessing internal structures.
 *
 * Components:
 * - Account management (registration, authentication, blocking)
 * - Character management (create, delete, select)
 * - Session management (login, logout, heartbeat)
 * - Permission checking
 *
 * Design Philosophy:
 * - Clean separation of account, character, and session concerns
 * - Avoid God Object anti-pattern
 * - Easy integration with Database module
 * - Thread-safe operations
 */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "user_types.h"
#include "user_account.h"
#include "user_character.h"
#include "user_session.h"
#include <common/defs.h>
#include <common/error_codes.h>

/* ==================== Module Initialization ==================== */

/**
 * Initialize User Module
 *
 * Sets up user management system, prepares authentication backends,
 * initializes session pool, and connects to database.
 *
 * Parameters:
 *   max_concurrent_sessions: Maximum number of concurrent user sessions
 *   database_handle: Database module handle (from Database module)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_Initialize(int max_concurrent_sessions, void* database_handle);

/**
 * Cleanup User Module
 *
 * Closes all active sessions, saves pending data, frees resources
 */
void User_Cleanup();

/* ==================== Account Management ==================== */

/**
 * Register New Account
 *
 * Creates a new user account in the database
 *
 * Parameters:
 *   username: Unique username (3-20 characters)
 *   password: Password (6-32 characters, will be hashed)
 *   email: Email address (optional, can be NULL)
 *   out_account_id: Output parameter for new account ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_RegisterAccount(const char* username, const char* password,
                         const char* email, uint32_t* out_account_id);

/**
 * Authenticate User (Login)
 *
 * Validates user credentials and creates an active session
 *
 * Parameters:
 *   username: User's login name
 *   password: User's password (plain text, will be hashed for comparison)
 *   client_ip: Client IP address
 *   client_port: Client port
 *   socket_fd: Socket file descriptor
 *
 * Returns: UserSession pointer on success, NULL on failure
 */
UserSession* User_Authenticate(const char* username, const char* password,
                               const char* client_ip, uint16_t client_port,
                               int socket_fd);

/**
 * Logout User (Terminate Session)
 *
 * Closes and invalidates a user session, saves character data
 *
 * Parameters:
 *   session: Session to terminate
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_Logout(UserSession* session);

/**
 * Block Account
 *
 * Blocks an account with reason (GM function)
 *
 * Parameters:
 *   account_id: Account ID to block
 *   duration_seconds: Block duration (0 = permanent)
 *   reason: Block reason
 *   gm_id: GM ID who issued the block
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_BlockAccount(uint32_t account_id, uint32_t duration_seconds,
                      const char* reason, uint32_t gm_id);

/**
 * Unblock Account
 *
 * Removes block from an account (GM function)
 *
 * Parameters:
 *   account_id: Account ID to unblock
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_UnblockAccount(uint32_t account_id);

/**
 * Change Password
 *
 * Changes account password (requires old password verification)
 *
 * Parameters:
 *   session: User session
 *   old_password: Current password
 *   new_password: New password
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_ChangePassword(UserSession* session, const char* old_password,
                        const char* new_password);

/**
 * Set Account VIP
 *
 * Sets or extends VIP status for account
 *
 * Parameters:
 *   account_id: Account ID
 *   vip_level: VIP level to set
 *   duration_seconds: VIP duration
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_SetAccountVIP(uint32_t account_id, VIPLevel vip_level,
                       uint32_t duration_seconds);

/* ==================== Character Management ==================== */

/**
 * Create New Character
 *
 * Creates a new character for the logged-in user
 *
 * Parameters:
 *   session: User session
 *   character_name: Name of new character (unique, 2-16 characters)
 *   job_class: Character job class
 *   gender: Character gender
 *   appearance: Character appearance settings
 *   out_character_id: Output parameter for new character ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_CreateCharacter(UserSession* session, const char* character_name,
                         JobClass job_class, Gender gender,
                         const AppearanceInfo* appearance,
                         uint32_t* out_character_id);

/**
 * Delete Character
 *
 * Marks a character for deletion (7-day waiting period)
 *
 * Parameters:
 *   session: User session
 *   character_id: ID of character to delete
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_DeleteCharacter(UserSession* session, uint32_t character_id);

/**
 * Cancel Character Deletion
 *
 * Cancels pending character deletion (before 7-day period expires)
 *
 * Parameters:
 *   session: User session
 *   character_id: ID of character to restore
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_CancelCharacterDeletion(UserSession* session, uint32_t character_id);

/**
 * Select Character (Enter Game)
 *
 * Selects a character and transitions to IN_GAME state
 *
 * Parameters:
 *   session: User session
 *   character_id: ID of character to select
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_SelectCharacter(UserSession* session, uint32_t character_id);

/**
 * List Account Characters
 *
 * Retrieves list of characters for the current account
 *
 * Parameters:
 *   session: User session
 *   out_characters: Output array of character pointers
 *   out_count: Output count of characters
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_ListCharacters(UserSession* session, Character*** out_characters,
                        int* out_count);

/**
 * Get Active Character
 *
 * Retrieves currently selected character for a session
 *
 * Parameters:
 *   session: User session
 *
 * Returns: Pointer to active character, or NULL if none selected
 */
Character* User_GetActiveCharacter(UserSession* session);

/**
 * Save Character Data
 *
 * Saves character data to database (position, stats, currency, etc.)
 *
 * Parameters:
 *   character: Character to save
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_SaveCharacter(Character* character);

/* ==================== Session Management ==================== */

/**
 * Get Session by ID
 *
 * Retrieves an active user session by session ID
 *
 * Parameters:
 *   session_id: Session ID to retrieve
 *
 * Returns: UserSession pointer, or NULL if not found
 */
UserSession* User_GetSessionByID(uint32_t session_id);

/**
 * Get Session by Account ID
 *
 * Retrieves an active user session by account ID
 *
 * Parameters:
 *   account_id: Account ID to search for
 *
 * Returns: UserSession pointer, or NULL if not online
 */
UserSession* User_GetSessionByAccountID(uint32_t account_id);

/**
 * Get Session by Character ID
 *
 * Retrieves an active user session by character ID
 *
 * Parameters:
 *   character_id: Character ID to search for
 *
 * Returns: UserSession pointer, or NULL if not online
 */
UserSession* User_GetSessionByCharacterID(uint32_t character_id);

/**
 * Update Session Heartbeat
 *
 * Updates session heartbeat timestamp (call regularly from network layer)
 *
 * Parameters:
 *   session: Session to update
 */
void User_UpdateHeartbeat(UserSession* session);

/**
 * Check Session Timeouts
 *
 * Checks all sessions for timeout and kicks inactive users
 * Should be called periodically (e.g., every 30 seconds)
 *
 * Returns: Number of sessions kicked
 */
int User_CheckSessionTimeouts();

/**
 * Kick Session
 *
 * Forcefully disconnects a session (GM function or timeout)
 *
 * Parameters:
 *   session: Session to kick
 *   reason: Kick reason
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_KickSession(UserSession* session, const char* reason);

/**
 * Get Active Session Count
 *
 * Returns number of currently active user sessions
 *
 * Returns: Number of active sessions
 */
int User_GetActiveSessionCount();

/**
 * Get Total Account Count
 *
 * Returns total number of registered accounts (from database)
 *
 * Returns: Number of accounts, or -1 on error
 */
int User_GetTotalAccountCount();

/* ==================== Permission and Security ==================== */

/**
 * Check User Permissions
 *
 * Verifies if user has specific permission level
 *
 * Parameters:
 *   session: User session
 *   required_type: Minimum user type required
 *
 * Returns: 1 if permitted, 0 if not
 */
int User_CheckPermissions(UserSession* session, UserType required_type);

/**
 * Check GM Permissions
 *
 * Verifies if user is GM with sufficient level
 *
 * Parameters:
 *   session: User session
 *   required_gm_level: Minimum GM level required (0-10)
 *
 * Returns: 1 if permitted, 0 if not
 */
int User_CheckGMPermissions(UserSession* session, uint8_t required_gm_level);

/**
 * Record Suspicious Activity
 *
 * Records suspicious activity for anti-cheat system
 *
 * Parameters:
 *   session: User session
 *   activity_type: Type of suspicious activity
 *   description: Activity description
 */
void User_RecordSuspiciousActivity(UserSession* session, int activity_type,
                                   const char* description);

/* ==================== Currency Operations ==================== */

/**
 * Add Currency to Character
 *
 * Adds currency to character's balance
 *
 * Parameters:
 *   character: Character to add currency to
 *   currency_type: Type of currency
 *   amount: Amount to add
 *   reason: Reason for adding (for logging)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_AddCurrency(Character* character, CurrencyType currency_type,
                     uint64_t amount, const char* reason);

/**
 * Remove Currency from Character
 *
 * Removes currency from character's balance
 *
 * Parameters:
 *   character: Character to remove currency from
 *   currency_type: Type of currency
 *   amount: Amount to remove
 *   reason: Reason for removing (for logging)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_RemoveCurrency(Character* character, CurrencyType currency_type,
                        uint64_t amount, const char* reason);

/**
 * Check Currency Balance
 *
 * Checks if character has sufficient currency
 *
 * Parameters:
 *   character: Character to check
 *   currency_type: Type of currency
 *   amount: Required amount
 *
 * Returns: 1 if sufficient, 0 if insufficient
 */
int User_HasCurrency(const Character* character, CurrencyType currency_type,
                     uint64_t amount);

/* ==================== Experience and Level ==================== */

/**
 * Add Experience to Character
 *
 * Adds experience points to character (with auto level-up)
 *
 * Parameters:
 *   character: Character to add exp to
 *   exp_amount: Experience amount
 *   reason: Reason for adding (for logging)
 *
 * Returns: Number of levels gained
 */
int User_AddExperience(Character* character, uint64_t exp_amount,
                       const char* reason);

/**
 * Set Character Level (GM function)
 *
 * Directly sets character level
 *
 * Parameters:
 *   character: Character to set level for
 *   new_level: New level (1-100)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_SetCharacterLevel(Character* character, uint16_t new_level);

/* ==================== Statistics and Queries ==================== */

/**
 * Get User Module Statistics
 *
 * Retrieves current user module statistics
 *
 * Returns: Pointer to statistics structure (read-only)
 */
void* User_GetStatistics();

/**
 * Reset User Module Statistics
 *
 * Clears session and authentication statistics
 */
void User_ResetStatistics();

/**
 * Get Character by ID (Database query)
 *
 * Loads character data from database
 *
 * Parameters:
 *   character_id: Character ID to load
 *   out_character: Output character structure (caller must free)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_GetCharacterByID(uint32_t character_id, Character** out_character);

/**
 * Get Account by ID (Database query)
 *
 * Loads account data from database
 *
 * Parameters:
 *   account_id: Account ID to load
 *   out_account: Output account structure (caller must free)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_GetAccountByID(uint32_t account_id, UserAccount** out_account);

/**
 * Check Character Name Available
 *
 * Checks if a character name is available (not taken)
 *
 * Parameters:
 *   character_name: Name to check
 *
 * Returns: 1 if available, 0 if taken or invalid
 */
int User_IsCharacterNameAvailable(const char* character_name);

/**
 * Check Account Name Available
 *
 * Checks if an account name is available (not taken)
 *
 * Parameters:
 *   account_name: Name to check
 *
 * Returns: 1 if available, 0 if taken or invalid
 */
int User_IsAccountNameAvailable(const char* account_name);

#endif // USER_INTERFACE_H
