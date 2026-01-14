/**
 * User Module - Type Definitions
 * Purpose: Core data types, enumerations, and constants for User module
 *
 * Design Philosophy:
 * - Avoid God Object anti-pattern
 * - Clear separation of concerns
 * - Layered architecture
 * - Easy to test and maintain
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include <stdint.h>

/* ========== Configuration Constants ========== */

#define MAX_USERNAME_LENGTH            64
#define MAX_PASSWORD_HASH_LENGTH       128
#define MAX_EMAIL_LENGTH               128
#define MAX_PHONE_LENGTH               32
#define MAX_IP_ADDRESS_LENGTH          64
#define MAX_SECURITY_QUESTION_LENGTH   256
#define MAX_BLOCK_REASON_LENGTH        256
#define MAX_CHARACTERS_PER_ACCOUNT     4
#define MAX_CHARACTER_NAME_LENGTH      64

/* ========== User Type Enumerations ========== */

/**
 * User account types and permission levels
 */
typedef enum {
    USER_TYPE_GUEST = 0,      // Guest account (limited access)
    USER_TYPE_PLAYER = 1,     // Regular player
    USER_TYPE_PREMIUM = 2,    // Premium/VIP player
    USER_TYPE_GM = 10,        // Game Master (basic)
    USER_TYPE_ADMIN = 11      // Administrator (full access)
} UserType;

/**
 * User account status
 */
typedef enum {
    ACCOUNT_STATUS_ACTIVE = 0,        // Active account
    ACCOUNT_STATUS_BLOCKED = 1,       // Temporarily blocked
    ACCOUNT_STATUS_BANNED = 2,        // Permanently banned
    ACCOUNT_STATUS_DELETED = 3,       // Soft deleted
    ACCOUNT_STATUS_PENDING = 4        // Pending activation
} AccountStatus;

/**
 * User session state
 */
typedef enum {
    SESSION_STATE_CONNECTING = 0,     // Connecting
    SESSION_STATE_AUTHENTICATED = 1,  // Authenticated
    SESSION_STATE_LOBBY = 2,          // In lobby (character select)
    SESSION_STATE_IN_GAME = 3,        // In game world
    SESSION_STATE_IN_DUNGEON = 4,     // In dungeon instance
    SESSION_STATE_IN_PVP = 5,         // In PVP match
    SESSION_STATE_IN_TRADE = 6,       // Trading with another player
    SESSION_STATE_DISCONNECTING = 7   // Disconnecting
} SessionState;

/**
 * VIP levels
 */
typedef enum {
    VIP_LEVEL_NONE = 0,
    VIP_LEVEL_BRONZE = 1,
    VIP_LEVEL_SILVER = 2,
    VIP_LEVEL_GOLD = 3,
    VIP_LEVEL_PLATINUM = 4,
    VIP_LEVEL_DIAMOND = 5
} VIPLevel;

/**
 * Character job classes (DNF职业)
 */
typedef enum {
    JOB_CLASS_NONE = 0,

    // 鬼剑士 (Slayer)
    JOB_CLASS_SLAYER = 0,

    // 格斗家 (Fighter)
    JOB_CLASS_FIGHTER = 1,

    // 神枪手 (Gunner)
    JOB_CLASS_GUNNER = 2,

    // 魔法师 (Mage)
    JOB_CLASS_MAGE = 3,

    // 圣职者 (Priest)
    JOB_CLASS_PRIEST = 4,

    // 暗夜使者 (Thief)
    JOB_CLASS_THIEF = 5,

    // 守护者 (Knight)
    JOB_CLASS_KNIGHT = 6,

    // 魔枪士 (Lancer)
    JOB_CLASS_LANCER = 7,

    // 枪剑士 (Agent)
    JOB_CLASS_AGENT = 8

} JobClass;

/**
 * Character job grow types (转职方向)
 */
typedef enum {
    JOB_GROW_TYPE_NONE = 0,

    // 鬼剑士转职
    JOB_GROW_WEAPON_MASTER = 1,    // 剑魂
    JOB_GROW_BERSERKER = 2,        // 狂战士
    JOB_GROW_SOUL_BENDER = 3,      // 鬼泣
    JOB_GROW_ASURA = 4,            // 阿修罗

    // 格斗家转职
    JOB_GROW_NEN_MASTER = 10,      // 气功师
    JOB_GROW_STRIKER = 11,         // 散打
    JOB_GROW_STREET_FIGHTER = 12,  // 街霸
    JOB_GROW_GRAPPLER = 13,        // 柔道

    // 神枪手转职
    JOB_GROW_RANGER = 20,          // 漫游枪手
    JOB_GROW_LAUNCHER = 21,        // 重火器
    JOB_GROW_MECHANIC = 22,        // 机械师
    JOB_GROW_SPITFIRE = 23,        // 弹药专家

    // 魔法师转职
    JOB_GROW_ELEMENTALIST = 30,    // 元素师
    JOB_GROW_SUMMONER = 31,        // 召唤师
    JOB_GROW_BATTLE_MAGE = 32,     // 战斗法师
    JOB_GROW_WITCH = 33,           // 魔道学者

    // (更多转职...)

} JobGrowType;

/**
 * Character gender
 */
typedef enum {
    GENDER_MALE = 0,
    GENDER_FEMALE = 1,
    GENDER_UNKNOWN = 2
} Gender;

/**
 * Character deletion status
 */
typedef enum {
    CHARACTER_ACTIVE = 0,          // Active character
    CHARACTER_PENDING_DELETE = 1,  // Pending deletion (7-day waiting)
    CHARACTER_DELETED = 2          // Permanently deleted
} CharacterStatus;

/**
 * Login failure reasons
 */
typedef enum {
    LOGIN_FAIL_INVALID_CREDENTIALS = 1,  // Wrong username/password
    LOGIN_FAIL_ACCOUNT_BLOCKED = 2,      // Account blocked
    LOGIN_FAIL_ACCOUNT_BANNED = 3,       // Account banned
    LOGIN_FAIL_SERVER_FULL = 4,          // Server at capacity
    LOGIN_FAIL_DUPLICATE_LOGIN = 5,      // Already logged in
    LOGIN_FAIL_VERSION_MISMATCH = 6,     // Client version mismatch
    LOGIN_FAIL_MAINTENANCE = 7,          // Server maintenance
    LOGIN_FAIL_VIP_EXPIRED = 8          // VIP required but expired
} LoginFailReason;

/**
 * Currency types
 */
typedef enum {
    CURRENCY_GOLD = 0,           // 金币
    CURRENCY_SILVER = 1,         // 银币
    CURRENCY_CERA_POINT = 2,     // 点券
    CURRENCY_MILEAGE = 3,        // 里程积分
    CURRENCY_WIN_POINT = 4       // 胜利点数
} CurrencyType;

/**
 * Attribute types
 */
typedef enum {
    ATTR_STRENGTH = 0,      // 力量
    ATTR_INTELLIGENCE = 1,  // 智力
    ATTR_VITALITY = 2,      // 体力
    ATTR_SPIRIT = 3         // 精神
} AttributeType;

/* ========== Structure Forward Declarations ========== */

typedef struct UserAccount UserAccount;
typedef struct Character Character;
typedef struct UserSession UserSession;
typedef struct CurrencyInfo CurrencyInfo;
typedef struct AttributeInfo AttributeInfo;
typedef struct PositionInfo PositionInfo;
typedef struct AppearanceInfo AppearanceInfo;
typedef struct VIPInfo VIPInfo;
typedef struct SecurityInfo SecurityInfo;
typedef struct StatisticsInfo StatisticsInfo;

/* ========== Currency Information ========== */

/**
 * Currency balances
 */
struct CurrencyInfo {
    uint64_t gold;              // 金币
    uint64_t silver;            // 银币
    uint32_t cera_point;        // 点券（现金货币）
    uint32_t used_cera;         // 累计使用点券
    int32_t mileage;            // 里程积分
    int32_t win_point;          // 胜利点数
};

/* ========== Character Attributes ========== */

/**
 * Basic and combat attributes
 */
struct AttributeInfo {
    // 基础属性
    uint16_t strength;          // 力量
    uint16_t intelligence;      // 智力
    uint16_t vitality;          // 体力
    uint16_t spirit;            // 精神

    // 战斗属性（计算得出）
    int32_t max_hp;             // 最大HP
    int32_t current_hp;         // 当前HP
    int32_t max_mp;             // 最大MP
    int32_t current_mp;         // 当前MP

    int32_t physical_attack;    // 物理攻击
    int32_t magical_attack;     // 魔法攻击
    int32_t physical_defense;   // 物理防御
    int32_t magical_defense;    // 魔法防御

    int32_t attack_speed;       // 攻击速度
    int32_t move_speed;         // 移动速度
    int32_t cast_speed;         // 施法速度

    float critical_rate;        // 暴击率
    float critical_damage;      // 暴击伤害

    int32_t hp_regen;           // HP恢复
    int32_t mp_regen;           // MP恢复
};

/* ========== Position and Appearance ========== */

/**
 * Character position in game world
 */
struct PositionInfo {
    int32_t current_zone;       // 当前区域
    int32_t current_map;        // 当前地图
    float position_x;           // X坐标
    float position_y;           // Y坐标
    float position_z;           // Z坐标（高度）
    float facing_angle;         // 朝向角度
};

/**
 * Character appearance settings
 */
struct AppearanceInfo {
    Gender gender;              // 性别
    uint8_t skin_color;         // 肤色
    uint8_t hair_style;         // 发型
    uint8_t hair_color;         // 发色
    uint8_t face_style;         // 脸型
    uint8_t eye_color;          // 眼睛颜色
    uint8_t body_type;          // 体型
    uint8_t voice_type;         // 声音类型
};

/* ========== VIP Information ========== */

/**
 * VIP/Premium account information
 */
struct VIPInfo {
    VIPLevel vip_level;         // VIP等级
    uint32_t vip_expire_time;   // VIP到期时间（Unix timestamp）
    uint32_t vip_points;        // VIP积分
    uint8_t vip_benefits_flags; // VIP特权标记（位图）
};

/* ========== Security Information ========== */

/**
 * Account security and anti-cheat data
 */
struct SecurityInfo {
    char security_question[MAX_SECURITY_QUESTION_LENGTH];  // 安全问题
    char security_answer_hash[MAX_PASSWORD_HASH_LENGTH];   // 安全答案哈希
    char email[MAX_EMAIL_LENGTH];                          // 绑定邮箱
    char phone[MAX_PHONE_LENGTH];                          // 绑定手机

    uint8_t two_factor_enabled;                            // 双因素认证
    uint32_t last_password_change;                         // 最后修改密码时间

    uint32_t login_attempt_count;                          // 登录尝试次数
    uint32_t last_failed_login;                            // 最后失败登录时间
    char last_failed_ip[MAX_IP_ADDRESS_LENGTH];            // 最后失败登录IP
};

/* ========== Statistics Information ========== */

/**
 * User statistics and tracking
 */
struct StatisticsInfo {
    uint32_t total_login_count;     // 总登录次数
    uint32_t total_play_time;       // 总游戏时长（秒）
    uint32_t character_count;       // 角色数量
    uint32_t deleted_char_count;    // 已删除角色数量

    uint64_t total_gold_earned;     // 累计获得金币
    uint64_t total_gold_spent;      // 累计花费金币

    uint32_t dungeon_cleared;       // 副本清除次数
    uint32_t pvp_matches;           // PVP比赛次数
    uint32_t pvp_wins;              // PVP胜场
    uint32_t pvp_losses;            // PVP败场

    uint32_t quests_completed;      // 完成任务数
    uint32_t achievements_unlocked; // 解锁成就数
};

#endif // USER_TYPES_H
