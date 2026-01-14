/**
 * User Module - Session Implementation
 * Purpose: UserSession structure functions implementation
 */

#include "user/user_session.h"
#include "user/user_account.h"
#include "user/user_character.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"

/* Database manager from user_module */
extern DBConnectionManager* g_db_manager;

/* ==================== Constants ==================== */

#define SESSION_TIMEOUT_SECONDS        300   // 5 minutes
#define MAX_INVALID_PACKETS            100   // Max invalid packets before kick
#define MAX_SUSPICIOUS_ACTIONS         50    // Max suspicious actions before kick

/* ==================== Helper Functions ==================== */

/**
 * Generate session token (simple implementation)
 */
static void generate_session_token(char* token, size_t token_size)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    size_t token_length = 32;

    for (size_t i = 0; i < token_length && i < token_size - 1; i++) {
        int key = rand() % (sizeof(charset) - 1);
        token[i] = charset[key];
    }
    token[token_length] = '\0';
}

/* ==================== Session Management Functions ==================== */

/**
 * Initialize UserSession structure
 */
void UserSession_Initialize(UserSession* session, int socket_fd,
                            const char* client_ip, uint16_t client_port)
{
    if (!session) return;

    memset(session, 0, sizeof(UserSession));

    // Network information
    session->socket_fd = socket_fd;
    if (client_ip) {
        strncpy(session->client_ip, client_ip, MAX_IP_ADDRESS_LENGTH - 1);
        session->client_ip[MAX_IP_ADDRESS_LENGTH - 1] = '\0';
    }
    session->client_port = client_port;

    // Session state
    session->is_authenticated = 0;
    session->session_state = SESSION_STATE_CONNECTING;

    // Timing
    uint32_t current_time = (uint32_t)time(NULL);
    session->login_time = current_time;
    session->last_activity = current_time;
    session->last_heartbeat = current_time;

    // Characters
    for (int i = 0; i < MAX_CHARACTERS_PER_ACCOUNT; i++) {
        session->characters[i] = NULL;
    }
    session->character_count = 0;
    session->active_character_index = -1;

    // Generate session token
    generate_session_token(session->session_token, sizeof(session->session_token));
    session->login_ticket = rand();

    // Create session lock
    pthread_mutex_t* lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (lock) {
        pthread_mutex_init(lock, NULL);
        session->session_lock = lock;
    }

    // Buffer allocation (simple fixed size)
    session->send_buffer_size = 4096;
    session->recv_buffer_size = 4096;
    session->send_buffer = malloc(session->send_buffer_size);
    session->recv_buffer = malloc(session->recv_buffer_size);
    session->recv_buffer_offset = 0;
}

/**
 * Cleanup UserSession and free resources
 */
void UserSession_Cleanup(UserSession* session)
{
    if (!session) return;

    printf("[UserSession] Cleaning up session ID %u (user: %s)\n",
           session->session_id, session->username);

    // Lock before cleanup
    UserSession_Lock(session);

    // Free character pointers (don't free the characters themselves,
    // they belong to the account)
    for (int i = 0; i < MAX_CHARACTERS_PER_ACCOUNT; i++) {
        session->characters[i] = NULL;
    }

    // Free buffers
    if (session->send_buffer) {
        free(session->send_buffer);
        session->send_buffer = NULL;
    }

    if (session->recv_buffer) {
        free(session->recv_buffer);
        session->recv_buffer = NULL;
    }

    // Clear account reference
    session->account = NULL;

    // Unlock and destroy lock
    if (session->session_lock) {
        pthread_mutex_unlock((pthread_mutex_t*)session->session_lock);
        pthread_mutex_destroy((pthread_mutex_t*)session->session_lock);
        free(session->session_lock);
        session->session_lock = NULL;
    }

    // Clear sensitive data
    memset(session->session_token, 0, sizeof(session->session_token));

    // Mark as cleaned
    session->is_authenticated = 0;
    session->socket_fd = -1;
}

/**
 * Set session as authenticated (after successful login)
 */
void UserSession_SetAuthenticated(UserSession* session, UserAccount* account)
{
    if (!session || !account) return;

    UserSession_Lock(session);

    session->is_authenticated = 1;
    session->account = account;
    session->account_id = account->account_id;
    session->user_id = account->account_id;

    strncpy(session->username, account->account_name, MAX_USERNAME_LENGTH - 1);
    session->username[MAX_USERNAME_LENGTH - 1] = '\0';

    session->user_type = account->user_type;
    session->is_gm = account->is_gm;
    session->gm_level = account->gm_level;

    session->pc_room_no = account->pc_room_no;

    // Change state to authenticated
    session->session_state = SESSION_STATE_AUTHENTICATED;

    UserSession_Unlock(session);

    printf("[UserSession] User authenticated: %s (ID: %u)\n",
           session->username, session->user_id);
}

/**
 * Load characters for session
 */
int UserSession_LoadCharacters(UserSession* session)
{
    if (!session || !session->account) {
        return 0;
    }

    UserSession_Lock(session);

    /* Initialize character array */
    session->character_count = 0;
    for (int i = 0; i < MAX_CHARACTERS_PER_ACCOUNT; i++) {
        session->characters[i] = NULL;
    }

    /* Load characters from database via ORM */
    if (g_db_manager && session->account) {
        CharacInfo db_chars[MAX_CHARACTERS_PER_ACCOUNT];
        int count = 0;

        /* Query characters for this account */
        if (CharacInfo_GetByMId(g_db_manager, session->account->account_id,
                                 db_chars, MAX_CHARACTERS_PER_ACCOUNT, &count) == 0) {
            for (int i = 0; i < count && i < MAX_CHARACTERS_PER_ACCOUNT; i++) {
                /* Skip deleted characters */
                if (db_chars[i].delete_flag > 0) {
                    continue;
                }

                /* Allocate and populate character */
                Character* ch = (Character*)calloc(1, sizeof(Character));
                if (ch) {
                    ch->character_id = (uint32_t)db_chars[i].charac_no;
                    strncpy(ch->name, db_chars[i].charac_name, sizeof(ch->name) - 1);
                    ch->level = (uint16_t)db_chars[i].lev;
                    ch->experience = (uint32_t)db_chars[i].exp;
                    ch->job_class = db_chars[i].job;
                    ch->grow_type = db_chars[i].grow_type;

                    session->characters[session->character_count] = ch;
                    session->character_count++;
                }
            }
            printf("[UserSession] Loaded %d characters for account %d\n",
                   session->character_count, session->account->account_id);
        }
    }

    UserSession_Unlock(session);

    return session->character_count;
}

/**
 * Get active character (currently selected)
 */
Character* UserSession_GetActiveCharacter(UserSession* session)
{
    if (!session) return NULL;

    if (session->active_character_index < 0 ||
        session->active_character_index >= MAX_CHARACTERS_PER_ACCOUNT) {
        return NULL;
    }

    return session->characters[session->active_character_index];
}

/**
 * Set active character by index
 */
int UserSession_SetActiveCharacter(UserSession* session, int8_t character_index)
{
    if (!session) return 0;

    if (character_index < 0 || character_index >= MAX_CHARACTERS_PER_ACCOUNT) {
        return 0;
    }

    if (character_index >= session->character_count) {
        return 0;
    }

    if (!session->characters[character_index]) {
        return 0;
    }

    UserSession_Lock(session);

    session->active_character_index = character_index;
    session->active_character_id = session->characters[character_index]->character_id;

    // Change state to IN_GAME
    session->session_state = SESSION_STATE_IN_GAME;

    UserSession_Unlock(session);

    printf("[UserSession] Character selected: %s\n",
           session->characters[character_index]->character_name);

    return 1;
}

/**
 * Set active character by character ID
 */
int UserSession_SetActiveCharacterByID(UserSession* session, uint32_t character_id)
{
    if (!session) return 0;

    // Find character with matching ID
    for (int i = 0; i < session->character_count; i++) {
        if (session->characters[i] &&
            session->characters[i]->character_id == character_id) {
            return UserSession_SetActiveCharacter(session, i);
        }
    }

    return 0;
}

/**
 * Update session heartbeat timestamp
 */
void UserSession_UpdateHeartbeat(UserSession* session)
{
    if (!session) return;

    uint32_t current_time = (uint32_t)time(NULL);

    session->last_heartbeat = current_time;
    session->last_activity = current_time;
    session->session_duration = current_time - session->login_time;
}

/**
 * Update session activity timestamp
 */
void UserSession_UpdateActivity(UserSession* session)
{
    if (!session) return;

    uint32_t current_time = (uint32_t)time(NULL);
    session->last_activity = current_time;
    session->session_duration = current_time - session->login_time;
}

/**
 * Check if session has timed out (no heartbeat)
 */
int UserSession_IsTimedOut(const UserSession* session, uint32_t timeout_seconds)
{
    if (!session) return 1;

    uint32_t current_time = (uint32_t)time(NULL);
    uint32_t elapsed = current_time - session->last_heartbeat;

    return (elapsed > timeout_seconds) ? 1 : 0;
}

/**
 * Check if user has permission
 */
int UserSession_HasPermission(const UserSession* session, UserType required_type)
{
    if (!session || !session->is_authenticated) {
        return 0;
    }

    return (session->user_type >= required_type) ? 1 : 0;
}

/**
 * Check if user is GM
 */
int UserSession_IsGM(const UserSession* session, uint8_t required_gm_level)
{
    if (!session || !session->is_authenticated) {
        return 0;
    }

    if (!session->is_gm) {
        return 0;
    }

    return (session->gm_level >= required_gm_level) ? 1 : 0;
}

/**
 * Change session state
 */
void UserSession_ChangeState(UserSession* session, SessionState new_state)
{
    if (!session) return;

    UserSession_Lock(session);

    SessionState old_state = session->session_state;
    session->session_state = new_state;

    UserSession_Unlock(session);

    printf("[UserSession] State changed: %d -> %d (user: %s)\n",
           old_state, new_state, session->username);
}

/**
 * Record suspicious activity
 */
void UserSession_RecordSuspiciousActivity(UserSession* session, int activity_type)
{
    if (!session) return;

    UserSession_Lock(session);

    session->suspicious_action_count++;

    // Set specific detection flags
    switch (activity_type) {
        case 1: // Speed hack
            session->speed_hack_detected = 1;
            break;
        case 2: // Teleport hack
            session->teleport_hack_detected = 1;
            break;
        case 3: // Duplicate login
            session->duplicate_login_detected = 1;
            break;
        default:
            break;
    }

    UserSession_Unlock(session);

    fprintf(stderr, "[UserSession] Suspicious activity detected: type=%d, user=%s, count=%u\n",
            activity_type, session->username, session->suspicious_action_count);
}

/**
 * Check if session should be kicked (too many violations)
 */
int UserSession_ShouldKick(const UserSession* session)
{
    if (!session) return 0;

    // Check invalid packet count
    if (session->invalid_packet_count >= MAX_INVALID_PACKETS) {
        return 1;
    }

    // Check suspicious action count
    if (session->suspicious_action_count >= MAX_SUSPICIOUS_ACTIONS) {
        return 1;
    }

    // Check specific hack detections
    if (session->speed_hack_detected || session->teleport_hack_detected) {
        return 1;
    }

    // Check force disconnect flag
    if (session->force_disconnect) {
        return 1;
    }

    return 0;
}

/**
 * Lock session (thread safety)
 */
void UserSession_Lock(UserSession* session)
{
    if (!session || !session->session_lock) return;
    pthread_mutex_lock((pthread_mutex_t*)session->session_lock);
}

/**
 * Unlock session (thread safety)
 */
void UserSession_Unlock(UserSession* session)
{
    if (!session || !session->session_lock) return;
    pthread_mutex_unlock((pthread_mutex_t*)session->session_lock);
}

/**
 * Get session statistics as string (for debugging)
 */
void UserSession_GetStatistics(const UserSession* session, char* buffer, size_t buffer_size)
{
    if (!session || !buffer) return;

    snprintf(buffer, buffer_size,
        "Session Statistics:\n"
        "  User: %s (ID: %u)\n"
        "  State: %d\n"
        "  Duration: %u seconds (%u minutes)\n"
        "  Packets: Sent=%u, Recv=%u\n"
        "  Bytes: Sent=%llu, Recv=%llu\n"
        "  Invalid Packets: %u\n"
        "  Suspicious Actions: %u\n"
        "  Active Character: %d\n",
        session->username,
        session->user_id,
        session->session_state,
        session->session_duration,
        session->session_duration / 60,
        session->packets_sent,
        session->packets_received,
        (unsigned long long)session->bytes_sent,
        (unsigned long long)session->bytes_received,
        session->invalid_packet_count,
        session->suspicious_action_count,
        session->active_character_index
    );
}

/**
 * Print session summary (for debugging)
 */
void UserSession_PrintSummary(const UserSession* session)
{
    if (!session) {
        printf("Session: NULL\n");
        return;
    }

    printf("========== Session Summary ==========\n");
    printf("Session ID: %u\n", session->session_id);
    printf("Username: %s (User ID: %u)\n", session->username, session->user_id);
    printf("Authenticated: %s\n", session->is_authenticated ? "Yes" : "No");
    printf("State: %d\n", session->session_state);
    printf("Client: %s:%u\n", session->client_ip, session->client_port);
    printf("Socket FD: %d\n", session->socket_fd);
    printf("Duration: %u seconds (%u minutes)\n",
           session->session_duration, session->session_duration / 60);
    printf("Active Character: %d\n", session->active_character_index);
    printf("Is GM: %s (Level %d)\n", session->is_gm ? "Yes" : "No", session->gm_level);
    printf("Packets: Sent=%u, Recv=%u\n",
           session->packets_sent, session->packets_received);
    printf("Suspicious Actions: %u\n", session->suspicious_action_count);
    printf("=====================================\n");
}

/**
 * Add character to session
 */
int UserSession_AddCharacter(UserSession* session, Character* character)
{
    if (!session || !character) return 0;

    UserSession_Lock(session);

    if (session->character_count >= MAX_CHARACTERS_PER_ACCOUNT) {
        UserSession_Unlock(session);
        return 0;
    }

    session->characters[session->character_count] = character;
    session->character_count++;

    UserSession_Unlock(session);

    return 1;
}

/**
 * Remove character from session
 */
int UserSession_RemoveCharacter(UserSession* session, uint32_t character_id)
{
    if (!session) return 0;

    UserSession_Lock(session);

    // Find character
    int found_index = -1;
    for (int i = 0; i < session->character_count; i++) {
        if (session->characters[i] &&
            session->characters[i]->character_id == character_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        UserSession_Unlock(session);
        return 0;
    }

    // Shift remaining characters
    for (int i = found_index; i < session->character_count - 1; i++) {
        session->characters[i] = session->characters[i + 1];
    }

    session->characters[session->character_count - 1] = NULL;
    session->character_count--;

    // If removed character was active, clear active index
    if (session->active_character_index == found_index) {
        session->active_character_index = -1;
        session->active_character_id = 0;
        session->session_state = SESSION_STATE_LOBBY;
    } else if (session->active_character_index > found_index) {
        session->active_character_index--;
    }

    UserSession_Unlock(session);

    return 1;
}

/**
 * Clear all characters from session
 */
void UserSession_ClearCharacters(UserSession* session)
{
    if (!session) return;

    UserSession_Lock(session);

    for (int i = 0; i < MAX_CHARACTERS_PER_ACCOUNT; i++) {
        session->characters[i] = NULL;
    }

    session->character_count = 0;
    session->active_character_index = -1;
    session->active_character_id = 0;

    UserSession_Unlock(session);
}

/**
 * Increment packet statistics
 */
void UserSession_IncrementPacketStats(UserSession* session, int is_sent,
                                     uint32_t packet_count, uint64_t byte_count)
{
    if (!session) return;

    if (is_sent) {
        session->packets_sent += packet_count;
        session->bytes_sent += byte_count;
    } else {
        session->packets_received += packet_count;
        session->bytes_received += byte_count;
    }
}

/**
 * Record invalid packet
 */
void UserSession_RecordInvalidPacket(UserSession* session)
{
    if (!session) return;

    session->invalid_packet_count++;

    fprintf(stderr, "[UserSession] Invalid packet: user=%s, count=%u\n",
            session->username, session->invalid_packet_count);
}
