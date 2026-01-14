/**
 * User Module Database Interface
 * Database operations for User module (Phase 6.1 - Week 3)
 */

#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <stdint.h>
#include "user/user_account.h"
#include "user/user_character.h"
#include "user/user_session.h"

/* ==================== Account Database Operations ==================== */

/**
 * Load account from database by account name
 * @param account_name Account name to search
 * @param account Output parameter for account data
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_LoadAccount(const char* account_name, UserAccount* account);

/**
 * Save account to database
 * @param account Account data to save
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_SaveAccount(const UserAccount* account);

/**
 * Create new account in database
 * @param account Account data to create
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_CreateAccount(const UserAccount* account);

/**
 * Delete account from database
 * @param account_id Account ID to delete
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_DeleteAccount(uint32_t account_id);

/* ==================== Character Database Operations ==================== */

/**
 * Load all characters for an account from database
 * @param account_id Account ID to load characters for
 * @param characters Output parameter for character array (caller must free)
 * @param character_count Output parameter for character count
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_LoadCharacters(uint32_t account_id, Character** characters, uint8_t* character_count);

/**
 * Load single character from database by character ID
 * @param character_id Character ID to load
 * @param character Output parameter for character data
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_LoadCharacter(uint32_t character_id, Character* character);

/**
 * Save character to database
 * @param character Character data to save
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_SaveCharacter(const Character* character);

/**
 * Create new character in database
 * @param character Character data to create
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_CreateCharacter(const Character* character);

/**
 * Delete character from database
 * @param character_id Character ID to delete
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_DeleteCharacter(uint32_t character_id);

/* ==================== Session Database Operations ==================== */

/**
 * Save session to database
 * @param session Session data to save
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_SaveSession(const UserSession* session);

/**
 * Load session from database by account ID
 * @param account_id Account ID to load session for
 * @param session Output parameter for session data
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_LoadSession(uint32_t account_id, UserSession* session);

/**
 * Delete session from database
 * @param account_id Account ID to delete session for
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_DeleteSession(uint32_t account_id);

/* ==================== Stored Procedure Wrappers ==================== */

/**
 * Authenticate account using stored procedure
 * @param account_name Account name
 * @param password_hash Password hash
 * @param client_ip Client IP address
 * @param account_id Output parameter for account ID
 * @return ERR_SUCCESS on success, error code otherwise
 */
int User_DB_AuthenticateAccount(const char* account_name, const char* password_hash,
                                  const char* client_ip, uint32_t* account_id);

#endif // USER_DATABASE_H
