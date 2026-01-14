/**
 * @file quest_shop.h
 * @brief Quest Shop - Internal API for QP-based attribute purchase system
 *
 * Phase 6.6 Shop System - Quest Shop Component
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - QP (Quest Points) management
 * - Character attribute purchase system (27 attributes)
 * - QP earning and spending tracking
 * - Integration with charac_quest_shop ORM table
 * - Attribute validation and limits
 *
 * DATA SOURCES:
 * ✅ src/orm/core_game/taiwan_cain/headers/charac_quest_shop_orm.h (27 fields)
 * ✅ User confirmed parameters (2025-11-23):
 *    - QP per quest: 200 QP
 *    - Attribute prices: Tiered (30/25/22/20 QP per point)
 */

#ifndef QUEST_SHOP_H
#define QUEST_SHOP_H

#include "database/headers/db_connection_manager.h"

/* ========================================================================
 * CONFIGURATION - User Confirmed Parameters (2025-11-23)
 * ======================================================================== */

/**
 * ✅ User Confirmed: 问题4 - 选项B
 * QP earned per quest completion
 */
#define QUEST_SHOP_QP_PER_QUEST            200

/**
 * ✅ User Confirmed: 问题5 - 选项A (分层定价)
 * QP price categories for different attribute tiers
 */
#define QUEST_SHOP_PRICE_HP_MP             30   /**< HP/MP attributes (核心生存) */
#define QUEST_SHOP_PRICE_ATTACK_DEFENSE    25   /**< Attack/Defense attributes (主要战斗) */
#define QUEST_SHOP_PRICE_CRITICAL          22   /**< Critical attributes (暴击系统) */
#define QUEST_SHOP_PRICE_STANDARD          20   /**< All other attributes (元素/速度/恢复等) */

/**
 * Maximum attribute points (safety limits)
 */
#define QUEST_SHOP_MAX_SINGLE_ATTRIBUTE    9999  /**< Max value for any single attribute */
#define QUEST_SHOP_MAX_QP                  999999999  /**< Max QP a player can hold */

/* ========================================================================
 * ATTRIBUTE ENUMS
 * ======================================================================== */

/**
 * Quest Shop attribute types
 *
 * ✅ Matches charac_quest_shop ORM table structure (27 attributes)
 */
typedef enum {
    /* ===== 核心生存属性 (30 QP/点) ===== */
    QUEST_ATTR_MAX_HP = 0,              /**< 最大HP */
    QUEST_ATTR_MAX_MP,                  /**< 最大MP */

    /* ===== 主要战斗属性 (25 QP/点) ===== */
    QUEST_ATTR_PSY_ATTACK,              /**< 物理攻击 */
    QUEST_ATTR_PSY_DEFENSE,             /**< 物理防御 */
    QUEST_ATTR_MAG_ATTACK,              /**< 魔法攻击 */
    QUEST_ATTR_MAG_DEFENSE,             /**< 魔法防御 */

    /* ===== 暴击系统 (22 QP/点) ===== */
    QUEST_ATTR_PSY_CRITICAL,            /**< 物理暴击 */
    QUEST_ATTR_MAG_CRITICAL,            /**< 魔法暴击 */

    /* ===== 速度属性 (20 QP/点) ===== */
    QUEST_ATTR_MOVE_SPEED,              /**< 移动速度 */
    QUEST_ATTR_ATTACK_SPEED,            /**< 攻击速度 */

    /* ===== 恢复属性 (20 QP/点) ===== */
    QUEST_ATTR_HP_REGEN,                /**< HP恢复 */
    QUEST_ATTR_MP_REGEN,                /**< MP恢复 */

    /* ===== 元素抗性 (20 QP/点) ===== */
    QUEST_ATTR_ALL_ELEMENT_RESIST,      /**< 全元素抗性 */
    QUEST_ATTR_FIRE_RESIST,             /**< 火元素抗性 */
    QUEST_ATTR_WATER_RESIST,            /**< 水元素抗性 */
    QUEST_ATTR_LIGHT_RESIST,            /**< 光元素抗性 */
    QUEST_ATTR_DARK_RESIST,             /**< 暗元素抗性 */

    /* ===== 元素攻击 (20 QP/点) ===== */
    QUEST_ATTR_ALL_ELEMENT_ATTACK,      /**< 全元素攻击 */
    QUEST_ATTR_FIRE_ATTACK,             /**< 火元素攻击 */
    QUEST_ATTR_WATER_ATTACK,            /**< 水元素攻击 */
    QUEST_ATTR_LIGHT_ATTACK,            /**< 光元素攻击 */
    QUEST_ATTR_DARK_ATTACK,             /**< 暗元素攻击 */

    /* ===== 命中/闪避 (20 QP/点) ===== */
    QUEST_ATTR_GOOD_HIT,                /**< 命中率 */
    QUEST_ATTR_EVASION,                 /**< 闪避率 */

    /* ===== 特殊属性 (20 QP/点) ===== */
    QUEST_ATTR_AWAKENING_DAMAGE,        /**< 觉醒伤害 */
    QUEST_ATTR_AWAKENING_HIT,           /**< 觉醒命中 */
    QUEST_ATTR_ABNORMAL_RESIST,         /**< 异常状态抗性 */

    QUEST_ATTR_COUNT                    /**< Total attribute count = 27 */
} QuestShopAttribute;

/**
 * QP price category for attribute
 */
typedef enum {
    QP_CATEGORY_HP_MP = 0,              /**< 30 QP/点 */
    QP_CATEGORY_ATTACK_DEFENSE,         /**< 25 QP/点 */
    QP_CATEGORY_CRITICAL,               /**< 22 QP/点 */
    QP_CATEGORY_STANDARD                /**< 20 QP/点 */
} QPPriceCategory;

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Quest Shop character data
 *
 * ✅ Mirrors charac_quest_shop ORM table structure
 */
typedef struct {
    unsigned int charac_no;             /**< Character ID */
    unsigned int qp;                    /**< Quest Points balance */

    /* ===== 核心生存属性 ===== */
    unsigned short max_hp;
    unsigned short max_mp;

    /* ===== 主要战斗属性 ===== */
    unsigned short psy_attack;
    unsigned short psy_defense;
    unsigned short mag_attack;
    unsigned short mag_defence;

    /* ===== 速度属性 ===== */
    unsigned short move_speed;
    unsigned short attack_speed;

    /* ===== 恢复属性 ===== */
    unsigned short hp_regen;
    unsigned short mp_regen;

    /* ===== 元素抗性 ===== */
    unsigned short all_element_resist;
    unsigned short fire_element_resist;
    unsigned short water_element_resist;
    unsigned short light_element_resist;
    unsigned short dark_element_resist;

    /* ===== 元素攻击 ===== */
    unsigned short all_element_attack;
    unsigned short fire_element_attack;
    unsigned short water_element_attack;
    unsigned short light_element_attack;
    unsigned short dark_element_attack;

    /* ===== 暴击属性 ===== */
    unsigned short psy_critical;
    unsigned short mag_critical;

    /* ===== 命中/闪避 ===== */
    unsigned short good_hit;
    unsigned short evasion;

    /* ===== 特殊属性 ===== */
    unsigned short awakening_damage;
    unsigned short awakening_hit;
    unsigned short abnormal_resist;
} QuestShopCharacterData;

/**
 * Quest Shop statistics
 */
typedef struct {
    unsigned long long total_qp_earned;     /**< All-time QP earned */
    unsigned long long total_qp_spent;      /**< All-time QP spent */
    unsigned long long total_attributes_purchased;  /**< Total attribute points purchased */

    /* Per-attribute statistics */
    unsigned int purchases_per_attribute[QUEST_ATTR_COUNT];  /**< Purchase count per attribute */
} QuestShopStatistics;

/**
 * Quest Shop Manager state
 */
typedef struct {
    /* Character data storage (in-memory cache) */
    QuestShopCharacterData* character_data;  /**< Character data array */
    int max_characters;                      /**< Maximum cached characters */
    int active_characters;                   /**< Currently cached characters */

    /* Statistics */
    QuestShopStatistics global_stats;

    /* Database */
    DBConnectionManager* db_manager;         /**< Database connection */

    /* State */
    unsigned char is_initialized;
} QuestShopManagerState;

/* ========================================================================
 * CORE API - Quest Shop Lifecycle
 * ======================================================================== */

/**
 * Initialize Quest Shop Manager
 *
 * @param max_characters Maximum characters to cache
 * @param db_manager Database connection manager (can be NULL)
 * @return 0 on success, negative error code on failure
 */
int QuestShop_Initialize(int max_characters, DBConnectionManager* db_manager);

/**
 * Cleanup Quest Shop Manager
 */
void QuestShop_Cleanup(void);

/* ========================================================================
 * QP MANAGEMENT API
 * ======================================================================== */

/**
 * Earn QP from quest completion
 *
 * @param charac_no Character ID
 * @param quest_id Quest ID (for logging)
 * @return 0 on success, negative error code on failure
 *
 * ✅ Awards 200 QP per quest (user confirmed)
 *
 * Example:
 *   QuestShop_EarnQP(12345, 1001);  // Character earns 200 QP
 */
int QuestShop_EarnQP(unsigned int charac_no, unsigned int quest_id);

/**
 * Manually add QP to character (GM command, events, etc.)
 *
 * @param charac_no Character ID
 * @param qp_amount QP to add
 * @return 0 on success, negative error code on failure
 */
int QuestShop_AddQP(unsigned int charac_no, unsigned int qp_amount);

/**
 * Get character's current QP balance
 *
 * @param charac_no Character ID
 * @param out_qp Output parameter for QP balance
 * @return 0 on success, negative error code on failure
 */
int QuestShop_GetQP(unsigned int charac_no, unsigned int* out_qp);

/**
 * Spend QP (internal function called by PurchaseAttribute)
 *
 * @param charac_no Character ID
 * @param qp_amount QP to spend
 * @return 0 on success, negative error code on failure
 */
int QuestShop_SpendQP(unsigned int charac_no, unsigned int qp_amount);

/* ========================================================================
 * ATTRIBUTE PURCHASE API
 * ======================================================================== */

/**
 * Purchase attribute points
 *
 * @param charac_no Character ID
 * @param attribute Attribute to purchase (QUEST_ATTR_MAX_HP, etc.)
 * @param points Number of points to purchase
 * @return 0 on success, negative error code on failure
 *
 * ✅ Prices: HP/MP(30), Attack/Defense(25), Critical(22), Others(20)
 *
 * Example:
 *   // Purchase 10 HP points (costs 10 × 30 = 300 QP)
 *   QuestShop_PurchaseAttribute(12345, QUEST_ATTR_MAX_HP, 10);
 *
 *   // Purchase 5 fire resist points (costs 5 × 20 = 100 QP)
 *   QuestShop_PurchaseAttribute(12345, QUEST_ATTR_FIRE_RESIST, 5);
 */
int QuestShop_PurchaseAttribute(unsigned int charac_no, QuestShopAttribute attribute, unsigned int points);

/**
 * Get attribute value for character
 *
 * @param charac_no Character ID
 * @param attribute Attribute to query
 * @param out_value Output parameter for attribute value
 * @return 0 on success, negative error code on failure
 */
int QuestShop_GetAttribute(unsigned int charac_no, QuestShopAttribute attribute, unsigned short* out_value);

/**
 * Get all attributes for character
 *
 * @param charac_no Character ID
 * @param out_data Output parameter for all character data
 * @return 0 on success, negative error code on failure
 */
int QuestShop_GetCharacterData(unsigned int charac_no, QuestShopCharacterData* out_data);

/**
 * Reset all attributes for character (refund QP)
 *
 * @param charac_no Character ID
 * @param refund_qp If 1, refund all spent QP; if 0, no refund
 * @return 0 on success, negative error code on failure
 */
int QuestShop_ResetAttributes(unsigned int charac_no, int refund_qp);

/* ========================================================================
 * PRICING API
 * ======================================================================== */

/**
 * Get QP price for an attribute
 *
 * @param attribute Attribute type
 * @return QP cost per point (30/25/22/20)
 *
 * ✅ Returns tiered pricing based on user confirmation
 */
unsigned int QuestShop_GetAttributePrice(QuestShopAttribute attribute);

/**
 * Calculate total cost for purchasing multiple points
 *
 * @param attribute Attribute type
 * @param points Number of points to purchase
 * @return Total QP cost
 *
 * Example:
 *   unsigned int cost = QuestShop_CalculatePurchaseCost(QUEST_ATTR_MAX_HP, 10);
 *   // Returns 300 (10 × 30 QP)
 */
unsigned int QuestShop_CalculatePurchaseCost(QuestShopAttribute attribute, unsigned int points);

/**
 * Get price category for an attribute
 *
 * @param attribute Attribute type
 * @return Price category enum
 */
QPPriceCategory QuestShop_GetPriceCategory(QuestShopAttribute attribute);

/* ========================================================================
 * VALIDATION API
 * ======================================================================== */

/**
 * Check if character has enough QP for purchase
 *
 * @param charac_no Character ID
 * @param qp_needed QP amount needed
 * @return 1 if sufficient, 0 if insufficient, negative error code on failure
 */
int QuestShop_HasEnoughQP(unsigned int charac_no, unsigned int qp_needed);

/**
 * Validate attribute purchase
 *
 * @param charac_no Character ID
 * @param attribute Attribute type
 * @param points Number of points
 * @return 0 if valid, negative error code if invalid
 *
 * Checks:
 * - Sufficient QP
 * - Attribute not exceeding max (9999)
 * - Valid attribute enum
 */
int QuestShop_ValidatePurchase(unsigned int charac_no, QuestShopAttribute attribute, unsigned int points);

/* ========================================================================
 * STATISTICS API
 * ======================================================================== */

/**
 * Get quest shop statistics
 *
 * @param out_stats Output parameter for statistics
 */
void QuestShop_GetStatistics(QuestShopStatistics* out_stats);

/**
 * Get most popular attribute
 *
 * @return Most frequently purchased attribute
 */
QuestShopAttribute QuestShop_GetMostPopularAttribute(void);

/* ========================================================================
 * UTILITY API
 * ======================================================================== */

/**
 * Get attribute name (for debugging/logging)
 *
 * @param attribute Attribute type
 * @return String name of attribute
 */
const char* QuestShop_GetAttributeName(QuestShopAttribute attribute);

/**
 * Get attribute name in Chinese (for display)
 *
 * @param attribute Attribute type
 * @return Chinese name of attribute
 */
const char* QuestShop_GetAttributeNameChinese(QuestShopAttribute attribute);

/**
 * Validate attribute enum
 *
 * @param attribute Attribute type
 * @return 1 if valid, 0 if invalid
 */
int QuestShop_IsValidAttribute(QuestShopAttribute attribute);

#endif /* QUEST_SHOP_H */
