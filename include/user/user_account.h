/**
 * User Module - Account Structure
 * Purpose: Account-level data structure definition
 *
 * UserAccount represents a player's game account (not individual characters).
 * One account can have multiple characters (up to MAX_CHARACTERS_PER_ACCOUNT).
 */

#ifndef USER_ACCOUNT_H
#define USER_ACCOUNT_H

#include "user_types.h"
#include <stdint.h>

/**
 * UserAccount - Account-level data
 *
 * This structure contains all account-wide information including:
 * - Authentication credentials
 * - Account status and permissions
 * - VIP/Premium information
 * - Security settings
 * - Login history and statistics
 * - Block/ban information
 */
typedef struct UserAccount {
    /* ==================== Basic Information ==================== */

    uint32_t account_id;                         // 账号ID（主键，数据库自增）
    char account_name[MAX_USERNAME_LENGTH];      // 账号名（唯一）
    char password_hash[MAX_PASSWORD_HASH_LENGTH];// 密码哈希（SHA-256或bcrypt）

    AccountStatus status;                        // 账号状态
    UserType user_type;                          // 用户类型/权限等级

    /* ==================== Registration Information ==================== */

    uint32_t register_date;                      // 注册日期（Unix timestamp）
    char register_ip[MAX_IP_ADDRESS_LENGTH];     // 注册IP

    /* ==================== Login Information ==================== */

    uint32_t last_login_date;                    // 最后登录日期
    char last_login_ip[MAX_IP_ADDRESS_LENGTH];   // 最后登录IP

    uint32_t current_login_time;                 // 本次登录时间
    uint8_t is_currently_online;                 // 是否当前在线

    /* ==================== Block/Ban Information ==================== */

    uint8_t is_blocked;                          // 是否封禁
    uint32_t block_start_time;                   // 封禁开始时间
    uint32_t block_expire_time;                  // 封禁到期时间（0=永久封禁）
    char block_reason[MAX_BLOCK_REASON_LENGTH];  // 封禁原因
    uint32_t block_by_gm_id;                     // 执行封禁的GM ID

    /* ==================== VIP Information ==================== */

    VIPInfo vip_info;                            // VIP信息

    /* ==================== Security Information ==================== */

    SecurityInfo security_info;                  // 安全信息

    /* ==================== Statistics ==================== */

    StatisticsInfo statistics;                   // 统计信息

    /* ==================== Warehouse/Storage ==================== */

    uint32_t warehouse_id;                       // 账号仓库ID（关联独立表）
    uint8_t warehouse_slots_unlocked;            // 仓库格子数（0-200）
    uint64_t warehouse_gold;                     // 仓库中的金币

    /* ==================== Special Flags ==================== */

    uint8_t is_gm;                               // 是否GM账号
    uint8_t gm_level;                            // GM等级（0-10）

    uint8_t is_return_user;                      // 是否回归用户
    uint32_t return_user_expire;                 // 回归用户福利到期时间

    uint8_t is_first_login;                      // 是否首次登录（新手引导）
    uint8_t tutorial_completed;                  // 是否完成新手教程

    /* ==================== PC Room Information (网吧) ==================== */

    uint32_t pc_room_no;                         // 网吧编号（0=非网吧登录）
    uint8_t pc_room_benefits_flags;              // 网吧特权标记

    /* ==================== School/Organization ==================== */

    uint32_t school_no;                          // 学校编号
    char school_name[128];                       // 学校名称

    /* ==================== Economic Controls ==================== */

    uint8_t gold_trade_restricted;               // 金币交易限制
    uint32_t gold_trade_restrict_expire;         // 限制到期时间

    uint8_t item_trade_restricted;               // 物品交易限制
    uint32_t item_trade_restrict_expire;         // 限制到期时间

    /* ==================== Anti-Cheat Flags ==================== */

    uint32_t hack_detection_flags;               // 作弊检测标记
    uint32_t last_hack_detection_time;           // 最后检测到作弊时间
    uint8_t is_blacklisted;                      // 是否黑名单

    /* ==================== Timestamps ==================== */

    uint32_t created_at;                         // 记录创建时间
    uint32_t updated_at;                         // 记录更新时间

} UserAccount;

/* ==================== Account Management Functions ==================== */

/**
 * Initialize UserAccount structure with default values
 */
void UserAccount_Initialize(UserAccount* account);

/**
 * Validate account credentials (password check)
 *
 * @param account - Account to validate
 * @param password_plain - Plain text password to check
 * @return 1 if valid, 0 if invalid
 */
int UserAccount_ValidatePassword(const UserAccount* account, const char* password_plain);

/**
 * Check if account is currently blocked
 *
 * @param account - Account to check
 * @return 1 if blocked, 0 if not blocked
 */
int UserAccount_IsBlocked(const UserAccount* account);

/**
 * Check if account VIP is valid (not expired)
 *
 * @param account - Account to check
 * @return 1 if VIP valid, 0 if expired or not VIP
 */
int UserAccount_IsVIPValid(const UserAccount* account);

/**
 * Check if account has sufficient permissions
 *
 * @param account - Account to check
 * @param required_type - Minimum required user type
 * @return 1 if permitted, 0 if not
 */
int UserAccount_HasPermission(const UserAccount* account, UserType required_type);

/**
 * Update last login information
 *
 * @param account - Account to update
 * @param ip_address - Login IP address
 */
void UserAccount_UpdateLoginInfo(UserAccount* account, const char* ip_address);

/**
 * Block account with reason
 *
 * @param account - Account to block
 * @param duration_seconds - Block duration (0 = permanent)
 * @param reason - Block reason
 * @param gm_id - GM ID who issued the block
 */
void UserAccount_Block(UserAccount* account, uint32_t duration_seconds,
                       const char* reason, uint32_t gm_id);

/**
 * Unblock account
 *
 * @param account - Account to unblock
 */
void UserAccount_Unblock(UserAccount* account);

/**
 * Set VIP status
 *
 * @param account - Account to update
 * @param vip_level - VIP level
 * @param duration_seconds - VIP duration
 */
void UserAccount_SetVIP(UserAccount* account, VIPLevel vip_level, uint32_t duration_seconds);

/**
 * Add account statistics (login, play time, etc.)
 *
 * @param account - Account to update
 * @param field - Statistics field to increment
 * @param value - Value to add
 */
void UserAccount_AddStatistic(UserAccount* account, int field, uint32_t value);

#endif // USER_ACCOUNT_H
