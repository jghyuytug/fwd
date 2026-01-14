/**
 * User Module - Public Interface
 * Purpose: User management, character handling, and authentication
 *
 * Components:
 * - User authentication
 * - Character management
 * - User state tracking
 * - Session handling
 */

#ifndef SRC_USER_USER_INTERFACE_H_
#define SRC_USER_USER_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * User types and permission levels
 */
typedef enum {
    USER_TYPE_GUEST = 0,
    USER_TYPE_PLAYER = 1,
    USER_TYPE_PREMIUM = 2,
    USER_TYPE_ADMIN = 10,
    USER_TYPE_GM = 11
} UserType;

/**
 * User session structure
 */
typedef struct {
    unsigned int user_id;
    char username[64];
    UserType user_type;
    int is_authenticated;
    void* character_list;  // Pointer to character list
    unsigned int login_time;
    unsigned int last_activity;
} UserSession;

/**
 * Initialize User Module
 *
 * Sets up user management system, prepares authentication backends
 *
 * Parameters:
 *   max_concurrent_users: Maximum number of concurrent user sessions
 *   auth_backend_path: Path to authentication backend configuration
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int User_Initialize(int max_concurrent_users, const char* auth_backend_path);

/**
 * Cleanup User Module
 *
 * Closes all active sessions, frees resources
 */
void User_Cleanup();

/**
 * Authenticate User
 *
 * Validates user credentials against backend
 *
 * Parameters:
 *   username: User's login name
 *   password: User's password
 *
 * Returns: UserSession pointer on success, NULL on failure
 */
UserSession* User_Authenticate(const char* username, const char* password);

/**
 * Terminate User Session
 *
 * Closes and invalidates a user session
 *
 * Parameters:
 *   session: Session to terminate
 *
 * Returns: 0 on success, error code on failure
 */
int User_TerminateSession(UserSession* session);

/**
 * Get User Session by ID
 *
 * Retrieves an active user session
 *
 * Parameters:
 *   user_id: Unique user identifier
 *
 * Returns: UserSession pointer, or NULL if not found
 */
UserSession* User_GetSessionByID(unsigned int user_id);

/**
 * Create New Character
 *
 * Creates a new character for a user
 *
 * Parameters:
 *   session: User session
 *   character_name: Name of new character
 *   character_class: Character class/job type
 *
 * Returns: Pointer to new character, or NULL on failure
 */
void* User_CreateCharacter(UserSession* session, const char* character_name, int character_class);

/**
 * Delete Character
 *
 * Removes a character from user's character list
 *
 * Parameters:
 *   session: User session
 *   character_id: ID of character to delete
 *
 * Returns: 0 on success, error code on failure
 */
int User_DeleteCharacter(UserSession* session, unsigned int character_id);

/**
 * List User's Characters
 *
 * Retrieves list of characters for a user
 *
 * Parameters:
 *   session: User session
 *
 * Returns: Pointer to character list, or NULL on failure
 */
void* User_ListCharacters(UserSession* session);

/**
 * Get Active Character
 *
 * Retrieves currently active character for a session
 *
 * Parameters:
 *   session: User session
 *
 * Returns: Pointer to active character, or NULL
 */
void* User_GetActiveCharacter(UserSession* session);

/**
 * Set Active Character
 *
 * Changes active character for a user session
 *
 * Parameters:
 *   session: User session
 *   character_id: ID of character to activate
 *
 * Returns: 0 on success, error code on failure
 */
int User_SetActiveCharacter(UserSession* session, unsigned int character_id);

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
 * Get Current Active User Count
 *
 * Returns number of currently active user sessions
 */
int User_GetActiveSessionCount();

/**
 * Reset User Module Statistics
 *
 * Clears session and authentication statistics
 */
void User_ResetStatistics();

/**
 * Get User Module Statistics
 *
 * Retrieves current user module statistics
 *
 * Returns: Pointer to statistics structure
 */
void* User_GetStatistics();

#endif // SRC_USER_USER_INTERFACE_H_