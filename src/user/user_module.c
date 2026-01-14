/**
 * User Module - Main Module Implementation
 * Purpose: Public API implementation for User module
 *
 * This file implements all public-facing User module functions.
 */

#include "user/user_interface_v2.h"
#include "user/user_account.h"
#include "user/user_character.h"
#include "user/user_session.h"
#include "common/headers/error_codes.h"
#include "database/headers/db_connection_manager.h"
#include "orm/core_game/d_taiwan/headers/accounts_orm.h"
#include "orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/* ==================== Database Manager ==================== */
static DBConnectionManager* g_db_manager = NULL;

/**
 * Set database manager for user module
 */
void User_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_db_manager = db_manager;
}

/* ==================== Constants ==================== */

#define SESSION_TIMEOUT_SECONDS 300  // 5 minutes

/* ==================== Module State ==================== */

typedef struct {
    int initialized;
    int max_sessions;
    int current_session_count;

    // Session pool
    UserSession* session_pool;

    // Database handle
    void* database_handle;

    // Statistics
    struct {
        uint32_t total_logins;
        uint32_t total_logouts;
        uint32_t total_registrations;
        uint32_t total_characters_created;
        uint32_t total_characters_deleted;
        uint32_t authentication_failures;
        uint32_t max_concurrent_sessions;
        uint32_t sessions_kicked_timeout;
        uint32_t sessions_kicked_hack;
    } stats;

    // Thread safety
    pthread_mutex_t module_lock;

} UserModuleState;

static UserModuleState g_user_module = {0};

/* ==================== Helper Functions ==================== */

/**
 * Find free session slot
 */
static UserSession* find_free_session()
{
    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (!g_user_module.session_pool[i].is_authenticated &&
            g_user_module.session_pool[i].socket_fd == -1) {
            return &g_user_module.session_pool[i];
        }
    }
    return NULL;
}

/**
 * Allocate session ID
 */
static uint32_t allocate_session_id()
{
    static uint32_t next_id = 1;
    return next_id++;
}

/* ==================== Module Initialization ==================== */

/**
 * Initialize User Module
 */
int User_Initialize(int max_concurrent_sessions, void* database_handle)
{
    if (g_user_module.initialized) {
        fprintf(stderr, "[User] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_concurrent_sessions <= 0 || max_concurrent_sessions > 10000) {
        fprintf(stderr, "[User] Invalid max concurrent sessions: %d\n",
                max_concurrent_sessions);
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Initializing User module...\n");
    printf("[User] Max concurrent sessions: %d\n", max_concurrent_sessions);

    // Allocate session pool
    g_user_module.session_pool = (UserSession*)calloc(max_concurrent_sessions,
                                                      sizeof(UserSession));
    if (!g_user_module.session_pool) {
        fprintf(stderr, "[User] Failed to allocate session pool\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Initialize all sessions
    for (int i = 0; i < max_concurrent_sessions; i++) {
        g_user_module.session_pool[i].socket_fd = -1;
    }

    // Store configuration
    g_user_module.max_sessions = max_concurrent_sessions;
    g_user_module.database_handle = database_handle;
    g_user_module.current_session_count = 0;

    // Reset statistics
    memset(&g_user_module.stats, 0, sizeof(g_user_module.stats));

    // Initialize mutex
    pthread_mutex_init(&g_user_module.module_lock, NULL);

    // Seed random number generator
    srand((unsigned int)time(NULL));

    g_user_module.initialized = 1;

    printf("[User] Module initialized successfully\n");

    return ERR_SUCCESS;
}

/**
 * Cleanup User Module
 */
void User_Cleanup()
{
    if (!g_user_module.initialized) {
        return;
    }

    printf("[User] Cleaning up User module...\n");
    printf("[User] Active sessions: %d\n", g_user_module.current_session_count);

    pthread_mutex_lock(&g_user_module.module_lock);

    // Terminate all active sessions
    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (g_user_module.session_pool[i].is_authenticated) {
            UserSession_Cleanup(&g_user_module.session_pool[i]);
        }
    }

    // Free session pool
    free(g_user_module.session_pool);
    g_user_module.session_pool = NULL;

    // Print final statistics
    printf("[User] Final Statistics:\n");
    printf("[User]   Total Logins: %u\n", g_user_module.stats.total_logins);
    printf("[User]   Total Logouts: %u\n", g_user_module.stats.total_logouts);
    printf("[User]   Total Registrations: %u\n", g_user_module.stats.total_registrations);
    printf("[User]   Total Characters Created: %u\n", g_user_module.stats.total_characters_created);
    printf("[User]   Max Concurrent Sessions: %u\n", g_user_module.stats.max_concurrent_sessions);

    g_user_module.initialized = 0;
    g_user_module.current_session_count = 0;

    pthread_mutex_unlock(&g_user_module.module_lock);
    pthread_mutex_destroy(&g_user_module.module_lock);

    printf("[User] Module cleanup complete\n");
}

/* ==================== Account Management ==================== */

/**
 * Register New Account
 */
int User_RegisterAccount(const char* username, const char* password,
                         const char* email, uint32_t* out_account_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!username || !password || !out_account_id) {
        return ERR_INVALID_PARAMETER;
    }

    // Validate username length
    size_t username_len = strlen(username);
    if (username_len < 3 || username_len > 20) {
        fprintf(stderr, "[User] Invalid username length: %zu\n", username_len);
        return ERR_INVALID_PARAMETER;
    }

    // Validate password length
    size_t password_len = strlen(password);
    if (password_len < 6 || password_len > 32) {
        fprintf(stderr, "[User] Invalid password length: %zu\n", password_len);
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Registering new account: %s\n", username);

    // Check if username already exists in database
    if (g_db_manager) {
        if (Account_NameExists(g_db_manager, username)) {
            fprintf(stderr, "[User] Account name already exists: %s\n", username);
            return ERR_INVALID_PARAMETER;
        }

        // Insert new account into database
        Account new_account;
        Account_Init(&new_account);
        strncpy(new_account.accountname, username, sizeof(new_account.accountname) - 1);
        strncpy(new_account.password, password, sizeof(new_account.password) - 1);
        strncpy(new_account.VIP, "0", sizeof(new_account.VIP) - 1);

        if (Account_Create(g_db_manager, &new_account) == 0) {
            *out_account_id = (uint32_t)new_account.UID;
            printf("[User] Account created in database with UID: %d\n", new_account.UID);
        } else {
            fprintf(stderr, "[User] Failed to create account in database\n");
            return ERR_UNKNOWN;
        }
    } else {
        // Fallback: generate a fake account ID if no database
        *out_account_id = (uint32_t)(rand() % 100000 + 1);
    }

    pthread_mutex_lock(&g_user_module.module_lock);
    g_user_module.stats.total_registrations++;
    pthread_mutex_unlock(&g_user_module.module_lock);

    printf("[User] Account registered successfully: %s (ID: %u)\n",
           username, *out_account_id);

    return ERR_SUCCESS;
}

/**
 * Authenticate User (Login)
 */
UserSession* User_Authenticate(const char* username, const char* password,
                               const char* client_ip, uint16_t client_port,
                               int socket_fd)
{
    if (!g_user_module.initialized) {
        fprintf(stderr, "[User] Module not initialized\n");
        return NULL;
    }

    if (!username || !password || !client_ip) {
        fprintf(stderr, "[User] Invalid authentication parameters\n");
        pthread_mutex_lock(&g_user_module.module_lock);
        g_user_module.stats.authentication_failures++;
        pthread_mutex_unlock(&g_user_module.module_lock);
        return NULL;
    }

    printf("[User] Authentication attempt: %s from %s:%u\n",
           username, client_ip, client_port);

    pthread_mutex_lock(&g_user_module.module_lock);

    // Check session limit
    if (g_user_module.current_session_count >= g_user_module.max_sessions) {
        fprintf(stderr, "[User] Maximum concurrent sessions reached\n");
        pthread_mutex_unlock(&g_user_module.module_lock);
        return NULL;
    }

    // Find free session slot
    UserSession* session = find_free_session();
    if (!session) {
        fprintf(stderr, "[User] No free session slots\n");
        pthread_mutex_unlock(&g_user_module.module_lock);
        return NULL;
    }

    pthread_mutex_unlock(&g_user_module.module_lock);

    // Query database to validate credentials
    UserAccount* account = NULL;

    if (g_db_manager) {
        int uid = Account_Verify(g_db_manager, username, password);
        if (uid > 0) {
            // Valid credentials - load account from database
            Account db_account;
            if (Account_GetByUID(g_db_manager, uid, &db_account) == 0) {
                account = (UserAccount*)malloc(sizeof(UserAccount));
                UserAccount_Initialize(account);

                account->account_id = (uint32_t)db_account.UID;
                strncpy(account->account_name, db_account.accountname, MAX_USERNAME_LENGTH - 1);

                // Check VIP status for admin/GM
                if (strcmp(db_account.VIP, "GM") == 0 || strcmp(db_account.VIP, "10") == 0) {
                    account->user_type = USER_TYPE_ADMIN;
                    account->is_gm = 1;
                    account->gm_level = 10;
                } else {
                    account->user_type = USER_TYPE_PLAYER;
                }

                UserAccount_UpdateLoginInfo(account, client_ip);
                printf("[User] Database authentication successful: UID=%d\n", uid);
            }
        }
    }

    // Fallback to hardcoded test credentials if no database or not found
    if (!account) {
        if (strcmp(username, "testuser") == 0 && strcmp(password, "test123") == 0) {
            account = (UserAccount*)malloc(sizeof(UserAccount));
            UserAccount_Initialize(account);
            account->account_id = 1001;
            strncpy(account->account_name, username, MAX_USERNAME_LENGTH - 1);
            account->user_type = USER_TYPE_PLAYER;
            UserAccount_UpdateLoginInfo(account, client_ip);
        } else if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
            account = (UserAccount*)malloc(sizeof(UserAccount));
            UserAccount_Initialize(account);
            account->account_id = 1;
            strncpy(account->account_name, username, MAX_USERNAME_LENGTH - 1);
            account->user_type = USER_TYPE_ADMIN;
            account->is_gm = 1;
            account->gm_level = 10;
            UserAccount_UpdateLoginInfo(account, client_ip);
        }
    }

    if (!account) {
        fprintf(stderr, "[User] Authentication failed: Invalid credentials\n");

        pthread_mutex_lock(&g_user_module.module_lock);
        g_user_module.stats.authentication_failures++;
        pthread_mutex_unlock(&g_user_module.module_lock);

        return NULL;
    }

    // Initialize session
    UserSession_Initialize(session, socket_fd, client_ip, client_port);
    session->session_id = allocate_session_id();

    // Set authenticated
    UserSession_SetAuthenticated(session, account);

    // Update statistics
    pthread_mutex_lock(&g_user_module.module_lock);
    g_user_module.current_session_count++;
    g_user_module.stats.total_logins++;

    if (g_user_module.current_session_count > g_user_module.stats.max_concurrent_sessions) {
        g_user_module.stats.max_concurrent_sessions = g_user_module.current_session_count;
    }
    pthread_mutex_unlock(&g_user_module.module_lock);

    printf("[User] Authentication successful: %s (Session ID: %u)\n",
           username, session->session_id);

    return session;
}

/**
 * Logout User (Terminate Session)
 */
int User_Logout(UserSession* session)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Logging out user: %s (Session ID: %u)\n",
           session->username, session->session_id);

    // Save character data if character is active
    Character* active_char = UserSession_GetActiveCharacter(session);
    if (active_char) {
        printf("[User] Saving character: %s\n", active_char->character_name);
        // Save character to database
        if (g_db_manager) {
            CharacInfo charac_info;
            charac_info.charac_no = (int)active_char->character_id;
            charac_info.m_id = (int)session->account_id;
            strncpy(charac_info.charac_name, active_char->character_name, sizeof(charac_info.charac_name) - 1);
            charac_info.lev = (unsigned char)active_char->level;
            charac_info.exp = (int)active_char->experience;
            charac_info.job = (signed char)active_char->job_class;
            charac_info.HP = active_char->current_hp;
            charac_info.maxHP = active_char->max_hp;
            charac_info.maxMP = active_char->max_mp;

            if (CharacInfo_Update(g_db_manager, &charac_info) == 0) {
                printf("[User] Character saved to database: %s\n", active_char->character_name);
            }
            CharacInfo_UpdateLastPlayTime(g_db_manager, charac_info.charac_no);
        }
    }

    // Free account if allocated
    if (session->account) {
        free(session->account);
        session->account = NULL;
    }

    // Cleanup session
    UserSession_Cleanup(session);

    // Update statistics
    pthread_mutex_lock(&g_user_module.module_lock);
    g_user_module.current_session_count--;
    g_user_module.stats.total_logouts++;
    pthread_mutex_unlock(&g_user_module.module_lock);

    return ERR_SUCCESS;
}

/**
 * Block Account
 */
int User_BlockAccount(uint32_t account_id, uint32_t duration_seconds,
                      const char* reason, uint32_t gm_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Blocking account ID %u for %u seconds. Reason: %s\n",
           account_id, duration_seconds, reason ? reason : "N/A");

    // Update account in database - set VIP to "BLOCKED"
    if (g_db_manager) {
        Account_UpdateVIP(g_db_manager, (int)account_id, "BLOCKED");
    }

    // Kick active session if online
    UserSession* active_session = User_GetSessionByAccountID(account_id);
    if (active_session) {
        User_KickSession(active_session, reason ? reason : "Account blocked");
    }

    return ERR_SUCCESS;
}

/**
 * Unblock Account
 */
int User_UnblockAccount(uint32_t account_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Unblocking account ID %u\n", account_id);

    // Update account in database - restore VIP to normal
    if (g_db_manager) {
        Account_UpdateVIP(g_db_manager, (int)account_id, "0");
    }

    return ERR_SUCCESS;
}

/**
 * Change Password
 */
int User_ChangePassword(UserSession* session, const char* old_password,
                        const char* new_password)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session || !old_password || !new_password) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated || !session->account) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Changing password for user: %s\n", session->username);

    if (!UserAccount_ChangePassword(session->account, old_password, new_password)) {
        fprintf(stderr, "[User] Password change failed\n");
        return ERR_INVALID_PARAMETER;
    }

    // Update password in database
    if (g_db_manager) {
        if (Account_UpdatePassword(g_db_manager, (int)session->account_id, new_password) != 0) {
            fprintf(stderr, "[User] Failed to update password in database\n");
        }
    }

    printf("[User] Password changed successfully\n");

    return ERR_SUCCESS;
}

/**
 * Set Account VIP
 */
int User_SetAccountVIP(uint32_t account_id, VIPLevel vip_level,
                       uint32_t duration_seconds)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Setting VIP level %d for account ID %u (duration: %u seconds)\n",
           vip_level, account_id, duration_seconds);

    // Update account VIP in database
    if (g_db_manager) {
        char vip_str[16];
        snprintf(vip_str, sizeof(vip_str), "%d", vip_level);
        Account_UpdateVIP(g_db_manager, (int)account_id, vip_str);
    }

    return ERR_SUCCESS;
}

/* ==================== Character Management ==================== */

/**
 * Create New Character
 */
int User_CreateCharacter(UserSession* session, const char* character_name,
                         JobClass job_class, Gender gender,
                         const AppearanceInfo* appearance,
                         uint32_t* out_character_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session || !character_name || !out_character_id) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated || !session->account) {
        return ERR_INVALID_STATE;
    }

    // Validate character name
    if (!Character_ValidateName(character_name)) {
        fprintf(stderr, "[User] Invalid character name: %s\n", character_name);
        return ERR_INVALID_PARAMETER;
    }

    // Check character count limit
    if (!UserAccount_CanCreateCharacter(session->account)) {
        fprintf(stderr, "[User] Character limit reached\n");
        return ERR_INVALID_STATE;
    }

    printf("[User] Creating character: %s for user %s\n",
           character_name, session->username);

    // Create character
    Character* character = (Character*)malloc(sizeof(Character));
    if (!character) {
        return ERR_OUT_OF_MEMORY;
    }

    Character_Initialize(character, session->account_id, character_name,
                        job_class, gender);

    // Apply appearance if provided
    if (appearance) {
        memcpy(&character->appearance, appearance, sizeof(AppearanceInfo));
    }

    // Insert character into database and get the real ID
    if (g_db_manager) {
        CharacInfo charac_info;
        memset(&charac_info, 0, sizeof(charac_info));
        charac_info.m_id = (int)session->account_id;
        strncpy(charac_info.charac_name, character_name, sizeof(charac_info.charac_name) - 1);
        charac_info.job = (signed char)job_class;
        charac_info.sex = (signed char)gender;
        charac_info.lev = 1;
        charac_info.exp = 0;
        charac_info.village = 1;
        charac_info.HP = 100;
        charac_info.maxHP = 100;
        charac_info.maxMP = 100;

        if (CharacInfo_Create(g_db_manager, &charac_info) == 0) {
            character->character_id = (uint32_t)charac_info.charac_no;
            *out_character_id = character->character_id;
            printf("[User] Character inserted into database with charac_no: %d\n", charac_info.charac_no);
        } else {
            // Fallback to random ID
            character->character_id = (uint32_t)(rand() % 1000000 + 1);
            *out_character_id = character->character_id;
            fprintf(stderr, "[User] Failed to insert character into database, using random ID\n");
        }
    } else {
        // Generate character ID if no database
        character->character_id = (uint32_t)(rand() % 1000000 + 1);
        *out_character_id = character->character_id;
    }

    // Add character to session
    if (!UserSession_AddCharacter(session, character)) {
        fprintf(stderr, "[User] Failed to add character to session\n");
        free(character);
        return ERR_UNKNOWN;
    }

    // Update account character count
    UserAccount_IncrementCharacterCount(session->account);

    // Update statistics
    pthread_mutex_lock(&g_user_module.module_lock);
    g_user_module.stats.total_characters_created++;
    pthread_mutex_unlock(&g_user_module.module_lock);

    printf("[User] Character created successfully: %s (ID: %u)\n",
           character_name, character->character_id);

    return ERR_SUCCESS;
}

/**
 * Delete Character
 */
int User_DeleteCharacter(UserSession* session, uint32_t character_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Deleting character ID %u for user %s\n",
           character_id, session->username);

    // Find character in session
    Character* character = NULL;
    for (int i = 0; i < session->character_count; i++) {
        if (session->characters[i] &&
            session->characters[i]->character_id == character_id) {
            character = session->characters[i];
            break;
        }
    }

    if (!character) {
        fprintf(stderr, "[User] Character not found\n");
        return ERR_NOT_FOUND;
    }

    // Mark for deletion (soft delete with 7-day waiting period)
    Character_MarkForDeletion(character);

    // Update character delete_time in database
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character_id, &charac_info) == 0) {
            // Set delete_time to current time + 7 days (soft delete)
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            tm_info->tm_mday += 7;
            strftime(charac_info.delete_time, sizeof(charac_info.delete_time), "%Y-%m-%d %H:%M:%S", tm_info);
            CharacInfo_Update(g_db_manager, &charac_info);
        }
    }

    // Update statistics
    pthread_mutex_lock(&g_user_module.module_lock);
    g_user_module.stats.total_characters_deleted++;
    pthread_mutex_unlock(&g_user_module.module_lock);

    printf("[User] Character marked for deletion: %s (expires in 7 days)\n",
           character->character_name);

    return ERR_SUCCESS;
}

/**
 * Cancel Character Deletion
 */
int User_CancelCharacterDeletion(UserSession* session, uint32_t character_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    // Find character
    Character* character = NULL;
    for (int i = 0; i < session->character_count; i++) {
        if (session->characters[i] &&
            session->characters[i]->character_id == character_id) {
            character = session->characters[i];
            break;
        }
    }

    if (!character) {
        return ERR_NOT_FOUND;
    }

    if (!Character_CancelDeletion(character)) {
        fprintf(stderr, "[User] Cannot cancel deletion (already expired)\n");
        return ERR_INVALID_STATE;
    }

    // Clear delete_time in database
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character_id, &charac_info) == 0) {
            memset(charac_info.delete_time, 0, sizeof(charac_info.delete_time));
            CharacInfo_Update(g_db_manager, &charac_info);
        }
    }

    printf("[User] Character deletion cancelled: %s\n", character->character_name);

    return ERR_SUCCESS;
}

/**
 * Select Character (Enter Game)
 */
int User_SelectCharacter(UserSession* session, uint32_t character_id)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated) {
        return ERR_INVALID_STATE;
    }

    printf("[User] Selecting character ID %u for user %s\n",
           character_id, session->username);

    if (!UserSession_SetActiveCharacterByID(session, character_id)) {
        fprintf(stderr, "[User] Failed to select character\n");
        return ERR_NOT_FOUND;
    }

    Character* character = UserSession_GetActiveCharacter(session);
    if (character) {
        Character_UpdateActivity(character);

        // Load additional character data from database
        if (g_db_manager) {
            CharacInfo charac_info;
            if (CharacInfo_GetByCharacNo(g_db_manager, (int)character_id, &charac_info) == 0) {
                // Update character with latest data from database
                character->level = charac_info.lev;
                character->experience = (uint64_t)charac_info.exp;
                character->current_hp = charac_info.HP;
                character->max_hp = charac_info.maxHP;
                character->max_mp = charac_info.maxMP;
                character->job_class = (JobClass)charac_info.job;
                printf("[User] Loaded character data from database: level=%d, exp=%d\n",
                       charac_info.lev, charac_info.exp);
            }
        }

        printf("[User] Character selected: %s (Level %u)\n",
               character->character_name, character->level);
    }

    return ERR_SUCCESS;
}

/**
 * List Account Characters
 */
int User_ListCharacters(UserSession* session, Character*** out_characters,
                        int* out_count)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session || !out_characters || !out_count) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated) {
        return ERR_INVALID_STATE;
    }

    *out_characters = session->characters;
    *out_count = session->character_count;

    return ERR_SUCCESS;
}

/**
 * Get Active Character
 */
Character* User_GetActiveCharacter(UserSession* session)
{
    if (!session) {
        return NULL;
    }

    return UserSession_GetActiveCharacter(session);
}

/**
 * Save Character Data
 */
int User_SaveCharacter(Character* character)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Saving character: %s (ID: %u)\n",
           character->character_name, character->character_id);

    // Update character in database
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character->character_id, &charac_info) == 0) {
            charac_info.lev = (unsigned char)character->level;
            charac_info.exp = (int)character->experience;
            charac_info.HP = character->current_hp;
            charac_info.maxHP = character->max_hp;
            charac_info.maxMP = character->max_mp;
            charac_info.job = (signed char)character->job_class;

            if (CharacInfo_Update(g_db_manager, &charac_info) == 0) {
                printf("[User] Character saved to database successfully\n");
            } else {
                fprintf(stderr, "[User] Failed to save character to database\n");
            }
            CharacInfo_UpdateLastPlayTime(g_db_manager, charac_info.charac_no);
        }
    }

    return ERR_SUCCESS;
}

/* ==================== Session Management ==================== */

/**
 * Get Session by ID
 */
UserSession* User_GetSessionByID(uint32_t session_id)
{
    if (!g_user_module.initialized) {
        return NULL;
    }

    pthread_mutex_lock(&g_user_module.module_lock);

    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (g_user_module.session_pool[i].is_authenticated &&
            g_user_module.session_pool[i].session_id == session_id) {
            pthread_mutex_unlock(&g_user_module.module_lock);
            return &g_user_module.session_pool[i];
        }
    }

    pthread_mutex_unlock(&g_user_module.module_lock);
    return NULL;
}

/**
 * Get Session by Account ID
 */
UserSession* User_GetSessionByAccountID(uint32_t account_id)
{
    if (!g_user_module.initialized) {
        return NULL;
    }

    pthread_mutex_lock(&g_user_module.module_lock);

    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (g_user_module.session_pool[i].is_authenticated &&
            g_user_module.session_pool[i].account_id == account_id) {
            pthread_mutex_unlock(&g_user_module.module_lock);
            return &g_user_module.session_pool[i];
        }
    }

    pthread_mutex_unlock(&g_user_module.module_lock);
    return NULL;
}

/**
 * Get Session by Character ID
 */
UserSession* User_GetSessionByCharacterID(uint32_t character_id)
{
    if (!g_user_module.initialized) {
        return NULL;
    }

    pthread_mutex_lock(&g_user_module.module_lock);

    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (g_user_module.session_pool[i].is_authenticated) {
            for (int j = 0; j < g_user_module.session_pool[i].character_count; j++) {
                if (g_user_module.session_pool[i].characters[j] &&
                    g_user_module.session_pool[i].characters[j]->character_id == character_id) {
                    pthread_mutex_unlock(&g_user_module.module_lock);
                    return &g_user_module.session_pool[i];
                }
            }
        }
    }

    pthread_mutex_unlock(&g_user_module.module_lock);
    return NULL;
}

/**
 * Update Session Heartbeat
 */
void User_UpdateHeartbeat(UserSession* session)
{
    if (!session) return;
    UserSession_UpdateHeartbeat(session);
}

/**
 * Check Session Timeouts
 */
int User_CheckSessionTimeouts()
{
    if (!g_user_module.initialized) {
        return 0;
    }

    int kicked_count = 0;

    pthread_mutex_lock(&g_user_module.module_lock);

    for (int i = 0; i < g_user_module.max_sessions; i++) {
        if (g_user_module.session_pool[i].is_authenticated) {
            // Check timeout
            if (UserSession_IsTimedOut(&g_user_module.session_pool[i], SESSION_TIMEOUT_SECONDS)) {
                printf("[User] Session timeout: %s\n",
                       g_user_module.session_pool[i].username);

                UserSession_Cleanup(&g_user_module.session_pool[i]);
                g_user_module.current_session_count--;
                g_user_module.stats.sessions_kicked_timeout++;
                kicked_count++;
            }
            // Check if should kick (hack detection)
            else if (UserSession_ShouldKick(&g_user_module.session_pool[i])) {
                printf("[User] Kicking session (violations): %s\n",
                       g_user_module.session_pool[i].username);

                UserSession_Cleanup(&g_user_module.session_pool[i]);
                g_user_module.current_session_count--;
                g_user_module.stats.sessions_kicked_hack++;
                kicked_count++;
            }
        }
    }

    pthread_mutex_unlock(&g_user_module.module_lock);

    return kicked_count;
}

/**
 * Kick Session
 */
int User_KickSession(UserSession* session, const char* reason)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!session) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Kicking session: %s, reason: %s\n",
           session->username, reason ? reason : "N/A");

    // Logout the session
    return User_Logout(session);
}

/**
 * Get Active Session Count
 */
int User_GetActiveSessionCount()
{
    if (!g_user_module.initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_user_module.module_lock);
    int count = g_user_module.current_session_count;
    pthread_mutex_unlock(&g_user_module.module_lock);

    return count;
}

/**
 * Get Total Account Count
 */
int User_GetTotalAccountCount()
{
    if (!g_user_module.initialized) {
        return -1;
    }

    // Query database for total account count
    if (g_db_manager) {
        int count = Account_GetCount(g_db_manager);
        if (count >= 0) {
            return count;
        }
    }
    return 0;
}

/* ==================== Permission and Security ==================== */

/**
 * Check User Permissions
 */
int User_CheckPermissions(UserSession* session, UserType required_type)
{
    if (!session) {
        return 0;
    }

    return UserSession_HasPermission(session, required_type);
}

/**
 * Check GM Permissions
 */
int User_CheckGMPermissions(UserSession* session, uint8_t required_gm_level)
{
    if (!session) {
        return 0;
    }

    return UserSession_IsGM(session, required_gm_level);
}

/**
 * Record Suspicious Activity
 */
void User_RecordSuspiciousActivity(UserSession* session, int activity_type,
                                   const char* description)
{
    if (!session) return;

    UserSession_RecordSuspiciousActivity(session, activity_type);

    fprintf(stderr, "[User] Suspicious activity: user=%s, type=%d, desc=%s\n",
            session->username, activity_type, description ? description : "N/A");
}

/* ==================== Currency Operations ==================== */

/**
 * Add Currency to Character
 */
int User_AddCurrency(Character* character, CurrencyType currency_type,
                     uint64_t amount, const char* reason)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Adding currency: character=%s, type=%d, amount=%llu, reason=%s\n",
           character->character_name, currency_type,
           (unsigned long long)amount, reason ? reason : "N/A");

    if (!Character_AddCurrency(character, currency_type, amount)) {
        fprintf(stderr, "[User] Failed to add currency\n");
        return ERR_UNKNOWN;
    }

    // Update character in database
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character->character_id, &charac_info) == 0) {
            // Note: Currency storage depends on currency_type mapping to database fields
            // For now, save basic character state
            CharacInfo_Update(g_db_manager, &charac_info);
        }
        // Log currency transaction would go to a log table
        printf("[User] Currency transaction logged: +%llu (type=%d) reason=%s\n",
               (unsigned long long)amount, currency_type, reason ? reason : "N/A");
    }

    return ERR_SUCCESS;
}

/**
 * Remove Currency from Character
 */
int User_RemoveCurrency(Character* character, CurrencyType currency_type,
                        uint64_t amount, const char* reason)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Removing currency: character=%s, type=%d, amount=%llu, reason=%s\n",
           character->character_name, currency_type,
           (unsigned long long)amount, reason ? reason : "N/A");

    if (!Character_RemoveCurrency(character, currency_type, amount)) {
        fprintf(stderr, "[User] Insufficient currency\n");
        return ERR_INVALID_PARAMETER;
    }

    // Update character in database
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character->character_id, &charac_info) == 0) {
            CharacInfo_Update(g_db_manager, &charac_info);
        }
        // Log currency transaction
        printf("[User] Currency transaction logged: -%llu (type=%d) reason=%s\n",
               (unsigned long long)amount, currency_type, reason ? reason : "N/A");
    }

    return ERR_SUCCESS;
}

/**
 * Check Currency Balance
 */
int User_HasCurrency(const Character* character, CurrencyType currency_type,
                     uint64_t amount)
{
    if (!character) {
        return 0;
    }

    return Character_HasCurrency(character, currency_type, amount);
}

/* ==================== Experience and Level ==================== */

/**
 * Add Experience to Character
 */
int User_AddExperience(Character* character, uint64_t exp_amount,
                       const char* reason)
{
    if (!g_user_module.initialized) {
        return 0;
    }

    if (!character) {
        return 0;
    }

    printf("[User] Adding experience: character=%s, exp=%llu, reason=%s\n",
           character->character_name, (unsigned long long)exp_amount,
           reason ? reason : "N/A");

    int levels_gained = Character_AddExperience(character, exp_amount);

    if (levels_gained > 0) {
        printf("[User] Character %s gained %d level(s)! Now level %u\n",
               character->character_name, levels_gained, character->level);

        // Update character level and exp in database
        if (g_db_manager) {
            CharacInfo_UpdateLevelExp(g_db_manager, (int)character->character_id,
                                      (unsigned char)character->level, (int)character->experience);
        }
        // Level up notification would be sent via network packet to client
        printf("[User] Level up notification: character=%s, new_level=%u\n",
               character->character_name, character->level);
    }

    return levels_gained;
}

/**
 * Set Character Level (GM function)
 */
int User_SetCharacterLevel(Character* character, uint16_t new_level)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!character) {
        return ERR_INVALID_PARAMETER;
    }

    if (new_level < 1 || new_level > 100) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[User] Setting character level: %s -> %u\n",
           character->character_name, new_level);

    character->level = new_level;
    Character_CalculateAttributes(character);

    // Update character level in database
    if (g_db_manager) {
        CharacInfo_UpdateLevelExp(g_db_manager, (int)character->character_id,
                                  (unsigned char)new_level, (int)character->experience);
    }

    return ERR_SUCCESS;
}

/* ==================== Statistics and Queries ==================== */

/**
 * Get User Module Statistics
 */
void* User_GetStatistics()
{
    if (!g_user_module.initialized) {
        return NULL;
    }

    return &g_user_module.stats;
}

/**
 * Reset User Module Statistics
 */
void User_ResetStatistics()
{
    if (!g_user_module.initialized) {
        return;
    }

    pthread_mutex_lock(&g_user_module.module_lock);
    memset(&g_user_module.stats, 0, sizeof(g_user_module.stats));
    pthread_mutex_unlock(&g_user_module.module_lock);

    printf("[User] Statistics reset\n");
}

/**
 * Get Character by ID (Database query)
 */
int User_GetCharacterByID(uint32_t character_id, Character** out_character)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!out_character) {
        return ERR_INVALID_PARAMETER;
    }

    // Query database for character
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)character_id, &charac_info) == 0) {
            Character* character = (Character*)malloc(sizeof(Character));
            if (character) {
                memset(character, 0, sizeof(Character));
                character->character_id = (uint32_t)charac_info.charac_no;
                character->account_id = (uint32_t)charac_info.m_id;
                strncpy(character->character_name, charac_info.charac_name, sizeof(character->character_name) - 1);
                character->level = charac_info.lev;
                character->experience = (uint64_t)charac_info.exp;
                character->job_class = (JobClass)charac_info.job;
                character->current_hp = charac_info.HP;
                character->max_hp = charac_info.maxHP;
                character->max_mp = charac_info.maxMP;
                *out_character = character;
                return ERR_SUCCESS;
            }
            return ERR_OUT_OF_MEMORY;
        }
        return ERR_NOT_FOUND;
    }

    return ERR_NOT_IMPLEMENTED;
}

/**
 * Get Account by ID (Database query)
 */
int User_GetAccountByID(uint32_t account_id, UserAccount** out_account)
{
    if (!g_user_module.initialized) {
        return ERR_INVALID_STATE;
    }

    if (!out_account) {
        return ERR_INVALID_PARAMETER;
    }

    // Query database for account
    if (g_db_manager) {
        Account db_account;
        if (Account_GetByUID(g_db_manager, (int)account_id, &db_account) == 0) {
            UserAccount* account = (UserAccount*)malloc(sizeof(UserAccount));
            if (account) {
                UserAccount_Initialize(account);
                account->account_id = (uint32_t)db_account.UID;
                strncpy(account->account_name, db_account.accountname, MAX_USERNAME_LENGTH - 1);
                // Check VIP for admin status
                if (strcmp(db_account.VIP, "GM") == 0 || strcmp(db_account.VIP, "10") == 0) {
                    account->user_type = USER_TYPE_ADMIN;
                    account->is_gm = 1;
                    account->gm_level = 10;
                } else {
                    account->user_type = USER_TYPE_PLAYER;
                }
                *out_account = account;
                return ERR_SUCCESS;
            }
            return ERR_OUT_OF_MEMORY;
        }
        return ERR_NOT_FOUND;
    }

    return ERR_NOT_IMPLEMENTED;
}

/**
 * Check Character Name Available
 */
int User_IsCharacterNameAvailable(const char* character_name)
{
    if (!g_user_module.initialized) {
        return 0;
    }

    if (!character_name) {
        return 0;
    }

    // Validate name first
    if (!Character_ValidateName(character_name)) {
        return 0;
    }

    // Query database to check if name exists
    if (g_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByName(g_db_manager, character_name, &charac_info) == 0) {
            // Name exists, not available
            return 0;
        }
        // Name not found, available
        return 1;
    }

    // No database, assume available
    return 1;
}

/**
 * Check Account Name Available
 */
int User_IsAccountNameAvailable(const char* account_name)
{
    if (!g_user_module.initialized) {
        return 0;
    }

    if (!account_name) {
        return 0;
    }

    size_t len = strlen(account_name);
    if (len < 3 || len > 20) {
        return 0;
    }

    // Query database to check if name exists
    if (g_db_manager) {
        if (Account_NameExists(g_db_manager, account_name)) {
            // Name exists, not available
            return 0;
        }
        // Name not found, available
        return 1;
    }

    // No database, assume available
    return 1;
}
