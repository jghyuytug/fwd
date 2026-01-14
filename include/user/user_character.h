/**
 * User Module - Character Structure
 * Purpose: Character-level data structure definition
 *
 * Character represents an individual game character belonging to an account.
 * Each account can have multiple characters (up to MAX_CHARACTERS_PER_ACCOUNT).
 */

#ifndef USER_CHARACTER_H
#define USER_CHARACTER_H

#include "user_types.h"
#include <stdint.h>

/**
 * Character - Individual game character data
 *
 * This structure contains all character-specific information including:
 * - Basic character information
 * - Job class and growth type
 * - Attributes and combat stats
 * - Position and appearance
 * - Currency balances
 * - References to external systems (Inventory, Quests, etc.)
 */
typedef struct Character {
    /* ==================== Basic Information ==================== */

    uint32_t character_id;                       // 角色ID（全局唯一，数据库主键）
    uint32_t account_id;                         // 所属账号ID（外键）
    char character_name[MAX_CHARACTER_NAME_LENGTH]; // 角色名（全服唯一）

    CharacterStatus status;                      // 角色状态（活跃/待删除/已删除）

    /* ==================== Job and Level ==================== */

    JobClass job_class;                          // 职业类型
    JobGrowType job_grow_type;                   // 转职方向
    uint16_t level;                              // 当前等级（1-100）
    uint64_t experience;                         // 当前经验值
    uint64_t next_level_exp;                     // 下一级所需经验

    int32_t sp_points;                           // 技能点（Skill Points）
    int32_t tp_points;                           // TP点数（天赋点）
    uint16_t stat_points;                        // 属性点（未分配）

    /* ==================== Appearance ==================== */

    AppearanceInfo appearance;                   // 外观信息

    /* ==================== Position ==================== */

    PositionInfo position;                       // 位置信息

    /* ==================== Attributes ==================== */

    AttributeInfo attributes;                    // 属性信息

    /* ==================== Currency (Character-specific) ==================== */

    CurrencyInfo currency;                       // 货币信息

    /* ==================== Inventory Reference ==================== */

    uint32_t inventory_id;                       // 库存ID（关联Inventory模块）
    uint8_t inventory_slots_total;               // 总库存格子数
    uint8_t inventory_slots_used;                // 已使用格子数

    /* ==================== Equipment Slots ==================== */

    uint32_t equipment_weapon;                   // 武器槽（物品ID，0=空）
    uint32_t equipment_top;                      // 上衣槽
    uint32_t equipment_bottom;                   // 下装槽
    uint32_t equipment_head;                     // 头部槽
    uint32_t equipment_shoulder;                 // 肩部槽
    uint32_t equipment_belt;                     // 腰带槽
    uint32_t equipment_shoes;                    // 鞋子槽
    uint32_t equipment_bracelet;                 // 手镯槽
    uint32_t equipment_necklace;                 // 项链槽
    uint32_t equipment_ring;                     // 戒指槽
    uint32_t equipment_support;                  // 辅助装备槽
    uint32_t equipment_magic_stone;              // 魔法石槽

    /* ==================== Guild Information ==================== */

    uint32_t guild_id;                           // 公会ID（0=无公会）
    uint8_t guild_rank;                          // 公会职位（0-10）
    uint32_t guild_contribution;                 // 公会贡献度

    /* ==================== Party Information ==================== */

    int16_t party_index;                         // 队伍索引（-1=无队伍）
    uint8_t party_position;                      // 队伍中位置（0-3）

    /* ==================== Dungeon Progress ==================== */

    uint32_t dungeon_clear_count;                // 副本清除总次数
    uint16_t fatigue_value;                      // 疲劳值
    uint16_t fatigue_max;                        // 最大疲劳值
    uint32_t last_fatigue_reset;                 // 最后疲劳值重置时间

    void* dungeon_clear_data;                    // 副本清除记录（指向独立结构）

    /* ==================== Quest Progress ==================== */

    uint16_t quests_completed_count;             // 完成任务总数
    uint16_t quests_active_count;                // 当前进行中任务数

    void* quest_progress_data;                   // 任务进度数据（指向独立结构）

    /* ==================== PVP Information ==================== */

    int32_t pvp_ranking;                         // PVP排名（0=未排名）
    int32_t pvp_points;                          // PVP积分
    uint16_t pvp_win_count;                      // PVP胜场
    uint16_t pvp_lose_count;                     // PVP败场
    uint16_t pvp_draw_count;                     // PVP平局
    float pvp_win_rate;                          // PVP胜率

    /* ==================== Achievement Information ==================== */

    uint16_t achievements_unlocked_count;        // 解锁成就数量

    void* achievement_data;                      // 成就数据（指向独立结构）

    /* ==================== Skill Information ==================== */

    void* skill_tree_data;                       // 技能树数据（指向独立结构）

    /* ==================== Title and Badge ==================== */

    uint16_t current_title_id;                   // 当前称号ID（0=无称号）
    uint16_t title_count;                        // 拥有称号数量

    uint8_t badges[10];                          // 徽章（每日/周/月成就）

    /* ==================== Statistics ==================== */

    uint32_t total_play_time;                    // 角色总游戏时长（秒）
    uint32_t total_kills;                        // 总击杀数
    uint32_t total_deaths;                       // 总死亡数
    uint64_t total_damage_dealt;                 // 总造成伤害
    uint64_t total_damage_received;              // 总承受伤害

    /* ==================== Timestamps ==================== */

    uint32_t create_date;                        // 创建时间（Unix timestamp）
    uint32_t last_login;                         // 最后登录时间
    uint32_t last_logout;                        // 最后登出时间
    uint32_t delete_date;                        // 删除时间（0=未删除）
    uint32_t delete_expire;                      // 删除生效时间（7天后）

    uint32_t created_at;                         // 记录创建时间
    uint32_t updated_at;                         // 记录更新时间

    /* ==================== Special Flags ==================== */

    uint8_t is_awakened;                         // 是否觉醒
    uint8_t awakening_level;                     // 觉醒等级

    uint8_t is_in_combat;                        // 是否战斗中
    uint8_t is_in_dungeon;                       // 是否副本中
    uint8_t is_in_pvp;                           // 是否PVP中

    uint8_t is_trading;                          // 是否交易中
    uint32_t trade_partner_id;                   // 交易对象ID

    uint8_t can_trade;                           // 是否可交易（限制标记）
    uint8_t can_drop;                            // 是否可丢弃物品

    /* ==================== Revival Information ==================== */

    uint32_t revival_time;                       // 复活时间
    uint8_t revival_count;                       // 当前副本复活次数

    /* ==================== Buff/Debuff ==================== */

    void* buff_list;                             // Buff列表（指向独立结构）
    void* debuff_list;                           // Debuff列表（指向独立结构）

} Character;

/* ==================== Character Management Functions ==================== */

/**
 * Initialize Character structure with default values
 *
 * @param character - Character to initialize
 * @param account_id - Owner account ID
 * @param name - Character name
 * @param job_class - Job class
 * @param gender - Character gender
 */
void Character_Initialize(Character* character, uint32_t account_id,
                          const char* name, JobClass job_class, Gender gender);

/**
 * Calculate character's combat attributes based on base attributes
 *
 * @param character - Character to calculate
 */
void Character_CalculateAttributes(Character* character);

/**
 * Add experience to character (with level up check)
 *
 * @param character - Character to add exp
 * @param exp_amount - Experience amount to add
 * @return Number of levels gained (0 if no level up)
 */
int Character_AddExperience(Character* character, uint64_t exp_amount);

/**
 * Level up character (increase level by 1)
 *
 * @param character - Character to level up
 * @return 1 if successful, 0 if already max level
 */
int Character_LevelUp(Character* character);

/**
 * Add currency to character
 *
 * @param character - Character to add currency
 * @param currency_type - Currency type
 * @param amount - Amount to add
 * @return 1 if successful, 0 if failed (overflow, etc.)
 */
int Character_AddCurrency(Character* character, CurrencyType currency_type, uint64_t amount);

/**
 * Remove currency from character
 *
 * @param character - Character to remove currency
 * @param currency_type - Currency type
 * @param amount - Amount to remove
 * @return 1 if successful, 0 if insufficient balance
 */
int Character_RemoveCurrency(Character* character, CurrencyType currency_type, uint64_t amount);

/**
 * Check if character has sufficient currency
 *
 * @param character - Character to check
 * @param currency_type - Currency type
 * @param amount - Required amount
 * @return 1 if sufficient, 0 if insufficient
 */
int Character_HasCurrency(const Character* character, CurrencyType currency_type, uint64_t amount);

/**
 * Equip item to character slot
 *
 * @param character - Character to equip
 * @param slot - Equipment slot
 * @param item_id - Item ID to equip (0 = unequip)
 * @return 1 if successful, 0 if failed
 */
int Character_EquipItem(Character* character, int slot, uint32_t item_id);

/**
 * Check if character can enter dungeon (fatigue check)
 *
 * @param character - Character to check
 * @param fatigue_cost - Fatigue cost of dungeon
 * @return 1 if can enter, 0 if insufficient fatigue
 */
int Character_CanEnterDungeon(const Character* character, uint16_t fatigue_cost);

/**
 * Consume fatigue for dungeon entry
 *
 * @param character - Character to consume fatigue
 * @param fatigue_cost - Fatigue cost
 * @return 1 if successful, 0 if insufficient
 */
int Character_ConsumeFatigue(Character* character, uint16_t fatigue_cost);

/**
 * Reset daily fatigue
 *
 * @param character - Character to reset fatigue
 */
void Character_ResetFatigue(Character* character);

/**
 * Join guild
 *
 * @param character - Character to join guild
 * @param guild_id - Guild ID
 * @param rank - Guild rank
 */
void Character_JoinGuild(Character* character, uint32_t guild_id, uint8_t rank);

/**
 * Leave guild
 *
 * @param character - Character to leave guild
 */
void Character_LeaveGuild(Character* character);

/**
 * Join party
 *
 * @param character - Character to join party
 * @param party_index - Party index
 * @param position - Position in party
 */
void Character_JoinParty(Character* character, int16_t party_index, uint8_t position);

/**
 * Leave party
 *
 * @param character - Character to leave party
 */
void Character_LeaveParty(Character* character);

/**
 * Update PVP record (after match)
 *
 * @param character - Character to update
 * @param is_win - 1 if win, 0 if lose, -1 if draw
 * @param points_change - Points change
 */
void Character_UpdatePvPRecord(Character* character, int is_win, int32_t points_change);

/**
 * Mark character for deletion (soft delete with 7-day waiting period)
 *
 * @param character - Character to delete
 */
void Character_MarkForDeletion(Character* character);

/**
 * Cancel character deletion (before expire)
 *
 * @param character - Character to restore
 * @return 1 if successful, 0 if already permanently deleted
 */
int Character_CancelDeletion(Character* character);

/**
 * Check if character deletion has expired (ready for permanent delete)
 *
 * @param character - Character to check
 * @return 1 if expired, 0 if still in waiting period
 */
int Character_IsDeletionExpired(const Character* character);

/**
 * Update character activity timestamp
 *
 * @param character - Character to update
 */
void Character_UpdateActivity(Character* character);

/**
 * Get character's total power rating (综合战力)
 *
 * @param character - Character to calculate
 * @return Power rating value
 */
uint32_t Character_GetPowerRating(const Character* character);

/**
 * Validate character name
 *
 * @param name - Name to validate
 * @return 1 if valid, 0 if invalid
 */
int Character_ValidateName(const char* name);

#endif // USER_CHARACTER_H
