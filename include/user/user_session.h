/**
 * User Module - Session Structure
 * Purpose: User session management for online players
 *
 * UserSession represents an active connection from a player.
 * It links the account, active character, and network connection.
 */

#ifndef USER_SESSION_H
#define USER_SESSION_H

#include "user_types.h"
#include "user_account.h"
#include "user_character.h"
#include <stdint.h>
#include <stddef.h>  // For size_t

/**
 * UserSession - Active user session data
 *
 * This structure represents an active user connection including:
 * - Network connection information
 * - Associated account and characters
 * - Session state and activity tracking
 * - Temporary runtime data
 */
typedef struct UserSession {
    /* ==================== Session Identification ==================== */

    uint32_t session_id;                         // 会话ID（唯一）
    uint32_t user_id;                            // 用户ID（与account_id相同）

    /* ==================== Authentication Status ==================== */

    uint8_t is_authenticated;                    // 是否已认证
    SessionState session_state;                  // 会话状态

    /* ==================== Account Reference ==================== */

    UserAccount* account;                        // 账号指针（指向内存中的账号数据）
    uint32_t account_id;                         // 账号ID（冗余，便于快速访问）
    char username[MAX_USERNAME_LENGTH];          // 用户名（冗余）

    /* ==================== Character Management ==================== */

    Character* characters[MAX_CHARACTERS_PER_ACCOUNT]; // 角色列表（最多4个）
    uint8_t character_count;                     // 角色数量
    int8_t active_character_index;               // 当前选中角色索引（-1=未选择）
    uint32_t active_character_id;                // 当前选中角色ID（0=未选择）

    /* ==================== Network Information ==================== */

    int socket_fd;                               // 套接字文件描述符
    char client_ip[MAX_IP_ADDRESS_LENGTH];       // 客户端IP
    uint16_t client_port;                        // 客户端端口

    /* ==================== Timing Information ==================== */

    uint32_t login_time;                         // 登录时间（Unix timestamp）
    uint32_t last_activity;                      // 最后活动时间
    uint32_t last_heartbeat;                     // 最后心跳时间

    uint32_t session_duration;                   // 会话持续时间（秒）

    /* ==================== Permission Flags ==================== */

    UserType user_type;                          // 用户类型（冗余，便于权限检查）
    uint8_t is_gm;                               // 是否GM
    uint8_t gm_level;                            // GM等级（0-10）

    /* ==================== Client Information ==================== */

    uint32_t client_version;                     // 客户端版本号
    char client_hash[64];                        // 客户端哈希（防篡改）

    /* ==================== Anti-Cheat ==================== */

    uint32_t packet_sequence;                    // 包序列号（防重放攻击）
    uint32_t invalid_packet_count;               // 无效包计数
    uint32_t suspicious_action_count;            // 可疑行为计数

    uint8_t speed_hack_detected;                 // 检测到加速外挂
    uint8_t teleport_hack_detected;              // 检测到瞬移外挂
    uint8_t duplicate_login_detected;            // 检测到重复登录

    /* ==================== Session Statistics ==================== */

    uint32_t packets_sent;                       // 发送包数量
    uint32_t packets_received;                   // 接收包数量
    uint64_t bytes_sent;                         // 发送字节数
    uint64_t bytes_received;                     // 接收字节数

    /* ==================== Temporary Flags ==================== */

    uint8_t is_in_transaction;                   // 是否数据库事务中
    uint8_t pending_disconnect;                  // 待断开连接
    uint8_t force_disconnect;                    // 强制断开连接

    /* ==================== Send Buffer ==================== */

    void* send_buffer;                           // 发送缓冲区（指针）
    uint32_t send_buffer_size;                   // 发送缓冲区大小

    /* ==================== Recv Buffer ==================== */

    void* recv_buffer;                           // 接收缓冲区（指针）
    uint32_t recv_buffer_size;                   // 接收缓冲区大小
    uint32_t recv_buffer_offset;                 // 接收缓冲区偏移

    /* ==================== Session Metadata ==================== */

    char session_token[128];                     // 会话令牌（验证用）
    uint32_t login_ticket;                       // 登录票据

    /* ==================== PC Room Information ==================== */

    uint32_t pc_room_no;                         // 网吧编号（0=非网吧）

    /* ==================== Thread Safety ==================== */

    void* session_lock;                          // 会话锁（pthread_mutex_t*）

} UserSession;

/* ==================== Session Management Functions ==================== */

/**
 * Initialize UserSession structure
 *
 * @param session - Session to initialize
 * @param socket_fd - Socket file descriptor
 * @param client_ip - Client IP address
 * @param client_port - Client port
 */
void UserSession_Initialize(UserSession* session, int socket_fd,
                            const char* client_ip, uint16_t client_port);

/**
 * Cleanup UserSession and free resources
 *
 * @param session - Session to cleanup
 */
void UserSession_Cleanup(UserSession* session);

/**
 * Set session as authenticated (after successful login)
 *
 * @param session - Session to authenticate
 * @param account - Associated account
 */
void UserSession_SetAuthenticated(UserSession* session, UserAccount* account);

/**
 * Load characters for session
 *
 * @param session - Session to load characters for
 * @return Number of characters loaded
 */
int UserSession_LoadCharacters(UserSession* session);

/**
 * Get active character (currently selected)
 *
 * @param session - Session to get character from
 * @return Pointer to active character, or NULL if none selected
 */
Character* UserSession_GetActiveCharacter(UserSession* session);

/**
 * Set active character by index
 *
 * @param session - Session to set character for
 * @param character_index - Index of character to activate (0-3)
 * @return 1 if successful, 0 if invalid index
 */
int UserSession_SetActiveCharacter(UserSession* session, int8_t character_index);

/**
 * Set active character by character ID
 *
 * @param session - Session to set character for
 * @param character_id - Character ID to activate
 * @return 1 if successful, 0 if character not found
 */
int UserSession_SetActiveCharacterByID(UserSession* session, uint32_t character_id);

/**
 * Update session heartbeat timestamp
 *
 * @param session - Session to update
 */
void UserSession_UpdateHeartbeat(UserSession* session);

/**
 * Update session activity timestamp
 *
 * @param session - Session to update
 */
void UserSession_UpdateActivity(UserSession* session);

/**
 * Check if session has timed out (no heartbeat)
 *
 * @param session - Session to check
 * @param timeout_seconds - Timeout threshold in seconds
 * @return 1 if timed out, 0 if still active
 */
int UserSession_IsTimedOut(const UserSession* session, uint32_t timeout_seconds);

/**
 * Check if user has permission
 *
 * @param session - Session to check
 * @param required_type - Minimum required user type
 * @return 1 if has permission, 0 if not
 */
int UserSession_HasPermission(const UserSession* session, UserType required_type);

/**
 * Check if user is GM
 *
 * @param session - Session to check
 * @param required_gm_level - Minimum required GM level
 * @return 1 if is GM with sufficient level, 0 if not
 */
int UserSession_IsGM(const UserSession* session, uint8_t required_gm_level);

/**
 * Change session state
 *
 * @param session - Session to change state
 * @param new_state - New session state
 */
void UserSession_ChangeState(UserSession* session, SessionState new_state);

/**
 * Record suspicious activity
 *
 * @param session - Session to record activity
 * @param activity_type - Type of suspicious activity
 */
void UserSession_RecordSuspiciousActivity(UserSession* session, int activity_type);

/**
 * Check if session should be kicked (too many violations)
 *
 * @param session - Session to check
 * @return 1 if should kick, 0 if OK
 */
int UserSession_ShouldKick(const UserSession* session);

/**
 * Lock session (thread safety)
 *
 * @param session - Session to lock
 */
void UserSession_Lock(UserSession* session);

/**
 * Unlock session (thread safety)
 *
 * @param session - Session to unlock
 */
void UserSession_Unlock(UserSession* session);

/**
 * Get session statistics as string (for debugging)
 *
 * @param session - Session to get stats from
 * @param buffer - Output buffer
 * @param buffer_size - Buffer size
 */
void UserSession_GetStatistics(const UserSession* session, char* buffer, size_t buffer_size);

#endif // USER_SESSION_H
