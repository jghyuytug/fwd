/**
 * User Module - Initialization and Management
 *
 * Handles user session management, authentication, and character handling
 *
 * Updated: 2025-11-25 - Database authentication integration
 */

#include "user_interface.h"
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/d_taiwan/headers/accounts_orm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Database manager reference */
static DBConnectionManager* g_user_db_manager = NULL;

/**
 * Set database manager for user module
 */
void User_SetDatabaseManager(DBConnectionManager* db_manager)
{
    g_user_db_manager = db_manager;
    if (db_manager) {
        printf("[User] Database manager configured for authentication\n");
    }
}

/* User module global state */
static struct {
    int initialized;
    int max_users;
    int current_active_users;
    char auth_backend_path[256];

    // Session management
    UserSession* active_sessions;

    // Statistics tracking
    struct {
        unsigned int total_logins;
        unsigned int total_logouts;
        unsigned int max_concurrent_users;
        unsigned int authentication_failures;
    } stats;
} g_user_state = {0};

/**
 * Initialize User Module
 */
int User_Initialize(int max_concurrent_users, const char* auth_backend_path)
{
    if (g_user_state.initialized) {
        fprintf(stderr, "[User] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_concurrent_users <= 0 || max_concurrent_users > 100000) {
        fprintf(stderr, "[User] Invalid max concurrent users: %d\n", max_concurrent_users);
        return ERR_INVALID_PARAMETER;
    }

    if (!auth_backend_path || strlen(auth_backend_path) == 0) {
        fprintf(stderr, "[User] Invalid authentication backend path\n");
        return ERR_INVALID_PARAMETER;
    }

    // Allocate session management resources
    g_user_state.active_sessions = calloc(max_concurrent_users, sizeof(UserSession));
    if (!g_user_state.active_sessions) {
        fprintf(stderr, "[User] Memory allocation failed for sessions\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Copy configuration
    g_user_state.max_users = max_concurrent_users;
    strncpy(g_user_state.auth_backend_path, auth_backend_path, sizeof(g_user_state.auth_backend_path) - 1);

    // Reset statistics
    memset(&g_user_state.stats, 0, sizeof(g_user_state.stats));

    g_user_state.initialized = 1;
    printf("[User] Module initialized. Max users: %d, Auth backend: %s\n",
           max_concurrent_users, auth_backend_path);

    return ERR_SUCCESS;
}

/**
 * Cleanup User Module
 */
void User_Cleanup()
{
    if (!g_user_state.initialized) {
        return;
    }

    printf("[User] Cleaning up user module...\n");
    printf("[User] Active sessions at cleanup: %d\n", g_user_state.current_active_users);
    printf("[User] Login statistics - Total: %u, Concurrent max: %u\n",
           g_user_state.stats.total_logins, g_user_state.stats.max_concurrent_users);

    // Terminate all active sessions
    for (int i = 0; i < g_user_state.max_users; i++) {
        if (g_user_state.active_sessions[i].is_authenticated) {
            User_TerminateSession(&g_user_state.active_sessions[i]);
        }
    }

    // Free session resources
    free(g_user_state.active_sessions);
    g_user_state.active_sessions = NULL;

    g_user_state.initialized = 0;
    g_user_state.current_active_users = 0;

    printf("[User] Module cleanup complete\n");
}

/**
 * Authenticate User
 */
UserSession* User_Authenticate(const char* username, const char* password)
{
    if (!g_user_state.initialized) {
        fprintf(stderr, "[User] Module not initialized\n");
        return NULL;
    }

    if (!username || !password) {
        fprintf(stderr, "[User] Invalid authentication credentials\n");
        g_user_state.stats.authentication_failures++;
        return NULL;
    }

    // Check if we have space for new session
    if (g_user_state.current_active_users >= g_user_state.max_users) {
        fprintf(stderr, "[User] Maximum concurrent users reached\n");
        return NULL;
    }

    // Find first free session slot
    for (int i = 0; i < g_user_state.max_users; i++) {
        if (!g_user_state.active_sessions[i].is_authenticated) {
            UserSession* session = &g_user_state.active_sessions[i];

            /*
             * Database authentication via Accounts ORM
             * Account_Verify checks username/password against accounts table
             */
            int verified_uid = 0;
            int auth_success = 0;

            if (g_user_db_manager) {
                /* Database authentication */
                verified_uid = Account_Verify(g_user_db_manager, username, password);
                if (verified_uid > 0) {
                    auth_success = 1;
                }
            } else {
                /* Fallback: Test credentials for development without database */
                if (strcmp(username, "test_user") == 0 && strcmp(password, "test_pass") == 0) {
                    verified_uid = i + 1;
                    auth_success = 1;
                }
            }

            if (auth_success) {
                session->user_id = verified_uid;
                strncpy(session->username, username, sizeof(session->username) - 1);
                session->user_type = USER_TYPE_PLAYER;
                session->is_authenticated = 1;
                session->login_time = time(NULL);
                session->last_activity = session->login_time;

                g_user_state.current_active_users++;
                g_user_state.stats.total_logins++;

                /* Update max concurrent users */
                if (g_user_state.current_active_users > g_user_state.stats.max_concurrent_users) {
                    g_user_state.stats.max_concurrent_users = g_user_state.current_active_users;
                }

                printf("[User] Authentication successful for user '%s' (uid=%d)\n",
                       username, verified_uid);
                return session;
            }

            g_user_state.stats.authentication_failures++;
            break;
        }
    }

    fprintf(stderr, "[User] Authentication failed for user: %s\n", username);
    return NULL;
}

/**
 * Terminate User Session
 */
int User_TerminateSession(UserSession* session)
{
    if (!g_user_state.initialized || !session) {
        return ERR_INVALID_PARAMETER;
    }

    if (!session->is_authenticated) {
        return ERR_INVALID_STATE;
    }

    // Log session information
    printf("[User] Terminating session for user: %s (ID: %u)\n",
           session->username, session->user_id);

    // Clear session data
    memset(session->username, 0, sizeof(session->username));
    session->user_id = 0;
    session->is_authenticated = 0;
    session->user_type = USER_TYPE_GUEST;

    g_user_state.current_active_users--;
    g_user_state.stats.total_logouts++;

    return ERR_SUCCESS;
}

/**
 * Get User Session by ID
 */
UserSession* User_GetSessionByID(unsigned int user_id)
{
    if (!g_user_state.initialized) {
        return NULL;
    }

    for (int i = 0; i < g_user_state.max_users; i++) {
        if (g_user_state.active_sessions[i].is_authenticated &&
            g_user_state.active_sessions[i].user_id == user_id) {
            return &g_user_state.active_sessions[i];
        }
    }

    return NULL;
}

/**
 * Get Current Active User Count
 */
int User_GetActiveSessionCount()
{
    return g_user_state.current_active_users;
}

/**
 * Reset User Module Statistics
 */
void User_ResetStatistics()
{
    if (!g_user_state.initialized) {
        return;
    }

    memset(&g_user_state.stats, 0, sizeof(g_user_state.stats));
    printf("[User] Module statistics reset\n");
}

/**
 * Get User Module Statistics
 */
void* User_GetStatistics()
{
    if (!g_user_state.initialized) {
        return NULL;
    }

    return &g_user_state.stats;
}