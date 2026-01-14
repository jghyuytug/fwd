/**
 * @file game_config.h
 * @brief Game Configuration Module - Centralized configuration management
 *
 * Purpose:
 * - Centralize all hardcoded game parameters
 * - Provide clean interface for business logic
 * - Support PVF configuration loading
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 3)
 * Date: 2025-11-25
 *
 * Architecture:
 * - Stage 1: Return hardcoded values (fallback)
 * - Stage 2: Hash+LRU cache for skill config (implemented)
 * - Stage 3: Load from PVF (current implementation)
 */

#ifndef INCLUDE_CONFIG_GAME_CONFIG_H_
#define INCLUDE_CONFIG_GAME_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * CONFIGURATION SOURCE
 * ======================================================================== */

/**
 * Configuration source enumeration
 */
typedef enum {
    CONFIG_SOURCE_HARDCODED = 0,   /* Use hardcoded default values */
    CONFIG_SOURCE_PVF = 1,         /* Load from Script.pvf */
    CONFIG_SOURCE_FILE = 2         /* Load from loose files (future) */
} ConfigSource;

/**
 * Get current configuration source
 */
ConfigSource GameConfig_GetSource(void);

/* ========================================================================
 * COMBAT SYSTEM CONFIGURATION
 * ======================================================================== */

/**
 * Character growth configuration by job class
 */
typedef struct {
    /* Base attributes */
    int base_strength;
    int base_intelligence;
    int base_vitality;
    int base_spirit;

    /* Growth per level */
    int strength_per_level;
    int intelligence_per_level;
    int vitality_per_level;
    int spirit_per_level;

    /* Combat attributes */
    int base_critical_rate;      /* Base crit rate (e.g., 5 = 5%) */
    float critical_rate_per_level; /* Crit rate gain per level */
    int base_critical_damage;    /* Base crit damage bonus (e.g., 50 = 50%) */
    int base_accuracy;           /* Base accuracy (e.g., 95 = 95%) */
    int base_evasion;            /* Base evasion (e.g., 5 = 5%) */
} CharacterGrowthConfig;

/**
 * Damage formula configuration
 */
typedef struct {
    int defense_constant;        /* Defense formula constant (e.g., 1000) */
    float min_damage_rate;       /* Minimum damage rate (e.g., 0.01 = 1%) */
    int attribute_divisor;       /* Attribute bonus divisor (e.g., 200) */
} DamageFormulaConfig;

/**
 * Skill configuration (per skill ID and level)
 */
typedef struct {
    int skill_id;
    int level;
    int base_damage;
    int damage_multiplier;       /* Multiplier in percent (e.g., 150 = 150%) */
    int cooldown_ms;
    int mp_cost;
    int cast_time_ms;
} SkillConfig;

/**
 * Buff configuration
 */
typedef struct {
    int buff_id;
    int exclusion_group;         /* Buffs in same group are mutually exclusive */
    int max_stack_count;
    int can_stack;               /* 0=no, 1=yes */
    int default_duration_ms;
} BuffConfig;

/* ========================================================================
 * QUEST SYSTEM CONFIGURATION
 * ======================================================================== */

/**
 * Quest reward configuration
 */
typedef struct {
    float exp_multiplier;        /* Experience multiplier (e.g., 1.5 = 150%) */
    float gold_multiplier;       /* Gold multiplier (e.g., 1.2 = 120%) */
} QuestRewardConfig;

/**
 * Quest timeout configuration
 */
typedef struct {
    int default_timeout_seconds; /* Default quest timeout (e.g., 3600 = 1 hour) */
    int max_timeout_seconds;     /* Maximum allowed timeout */
} QuestTimeoutConfig;

/**
 * Quest limits configuration
 */
typedef struct {
    int max_active_quests_per_character;
    int max_quest_objectives;
} QuestLimitsConfig;

/* ========================================================================
 * SHOP SYSTEM CONFIGURATION
 * ======================================================================== */

/**
 * Shop discount configuration
 */
typedef struct {
    float default_discount;      /* Default discount (1.0 = no discount) */
    float vip_discount;          /* VIP discount (e.g., 0.9 = 10% off) */
    float event_discount;        /* Event discount (e.g., 0.8 = 20% off) */
    float sell_price_ratio;      /* Sell price ratio (e.g., 0.5 = 50% of buy price) */
} ShopDiscountConfig;

/**
 * Shop refresh configuration
 */
typedef struct {
    int daily_refresh_hour;      /* Hour for daily refresh (0-23) */
    int weekly_refresh_day;      /* Day for weekly refresh (0=Sun, 1=Mon, ...) */
    int secret_shop_duration_seconds; /* Secret shop duration */
} ShopRefreshConfig;

/**
 * Shop purchase limits configuration
 */
typedef struct {
    int max_purchase_per_day;
    int max_purchase_per_week;
} ShopLimitsConfig;

/* ========================================================================
 * PARTY SYSTEM CONFIGURATION
 * ======================================================================== */

/**
 * Party bonus configuration
 */
typedef struct {
    float bonus_2_members;       /* Bonus for 2 members (e.g., 1.1 = +10%) */
    float bonus_3_members;       /* Bonus for 3 members (e.g., 1.2 = +20%) */
    float bonus_4_members;       /* Bonus for 4 members (e.g., 1.3 = +30%) */
} PartyBonusConfig;

/**
 * Party level gap configuration
 */
typedef struct {
    int max_level_gap;           /* Maximum level difference allowed */
    float penalty_per_level;     /* EXP penalty per level gap (e.g., 0.05 = 5%) */
} PartyLevelGapConfig;

/* ========================================================================
 * MASTER CONFIGURATION STRUCTURE
 * ======================================================================== */

/**
 * Master game configuration containing all subsystems
 */
typedef struct {
    /* Combat configurations */
    DamageFormulaConfig damage_formula;
    CharacterGrowthConfig character_growth[10];  /* 10 job classes */
    BuffConfig buff_rules;

    /* Quest configurations */
    QuestRewardConfig quest_reward;
    QuestTimeoutConfig quest_timeout;
    QuestLimitsConfig quest_limits;

    /* Shop configurations */
    ShopDiscountConfig shop_discount;
    ShopRefreshConfig shop_refresh;
    ShopLimitsConfig shop_limits;

    /* Party configurations */
    PartyBonusConfig party_bonus;
    PartyLevelGapConfig party_level_gap;
} GameConfig;

/* ========================================================================
 * PUBLIC API - MODULE LIFECYCLE
 * ======================================================================== */

/**
 * Initialize game configuration module with hardcoded values
 *
 * This function initializes all configuration data using
 * hardcoded default values. Use GameConfig_InitializeFromPVF()
 * to load from PVF file instead.
 *
 * Returns: 0 on success, error code on failure
 */
int GameConfig_Initialize(void);

/**
 * Initialize game configuration module from PVF file
 *
 * This function loads configuration from Script.pvf.
 * If PVF loading fails, it falls back to hardcoded values.
 *
 * Parameters:
 *   pvf_path: Path to Script.pvf file
 *
 * Returns: 0 on success, negative error code on failure
 *          (still initializes with hardcoded values on PVF failure)
 */
int GameConfig_InitializeFromPVF(const char* pvf_path);

/**
 * Initialize game configuration module from config files
 *
 * This function loads configuration from text .etc files.
 * If file loading fails, it falls back to hardcoded values.
 *
 * Parameters:
 *   config_dir: Path to directory containing .etc config files
 *               (e.g., "config/etc" or "/home/dxf/game/config/etc")
 *
 * Expected files in config_dir:
 *   - combat.etc
 *   - quest.etc
 *   - shop.etc
 *   - party.etc
 *
 * Returns: 0 on success, negative error code on failure
 *          (still initializes with hardcoded values on file failure)
 */
int GameConfig_InitializeFromFile(const char* config_dir);

/**
 * Initialize game configuration with specified source
 *
 * Parameters:
 *   source: Configuration source (HARDCODED, PVF, or FILE)
 *   path: Path to configuration file (NULL for HARDCODED)
 *
 * Returns: 0 on success, negative error code on failure
 */
int GameConfig_InitializeEx(ConfigSource source, const char* path);

/**
 * Cleanup game configuration module
 */
void GameConfig_Cleanup(void);

/**
 * Get master configuration structure
 *
 * Returns: Pointer to global configuration (read-only)
 */
const GameConfig* GameConfig_Get(void);

/* ========================================================================
 * PUBLIC API - COMBAT CONFIGURATION
 * ======================================================================== */

/**
 * Get damage formula configuration
 */
const DamageFormulaConfig* CombatConfig_GetDamageFormula(void);

/**
 * Get character growth configuration by job class
 *
 * Parameters:
 *   job_class: Job class ID (0-9)
 *
 * Returns: Pointer to growth configuration (read-only)
 */
const CharacterGrowthConfig* CombatConfig_GetCharacterGrowth(int job_class);

/**
 * Get skill configuration
 *
 * Parameters:
 *   skill_id: Skill ID
 *   level: Skill level
 *
 * Returns: Pointer to skill configuration (read-only)
 * Note: In Stage 1, returns calculated values based on formulas
 *       In Stage 3, will return actual values from PVF
 */
const SkillConfig* CombatConfig_GetSkill(int skill_id, int level);

/**
 * Get buff configuration
 *
 * Parameters:
 *   buff_id: Buff ID
 *
 * Returns: Pointer to buff configuration (read-only)
 */
const BuffConfig* CombatConfig_GetBuffRules(void);

/* ========================================================================
 * PUBLIC API - QUEST CONFIGURATION
 * ======================================================================== */

/**
 * Get quest reward configuration
 * NOTE: Renamed to GameConfig_GetQuestReward() to avoid conflict with quest/quest_config.h
 */
const QuestRewardConfig* GameConfig_GetQuestReward(void);

/**
 * Get quest timeout configuration
 * NOTE: Renamed to GameConfig_GetQuestTimeout() to avoid conflict
 */
const QuestTimeoutConfig* GameConfig_GetQuestTimeout(void);

/**
 * Get quest limits configuration
 * NOTE: Renamed to GameConfig_GetQuestLimits() to avoid conflict
 */
const QuestLimitsConfig* GameConfig_GetQuestLimits(void);

/* ========================================================================
 * PUBLIC API - SHOP CONFIGURATION
 * ======================================================================== */

/**
 * Get shop discount configuration
 */
const ShopDiscountConfig* ShopConfig_GetDiscount(void);

/**
 * Get shop refresh configuration
 */
const ShopRefreshConfig* ShopConfig_GetRefresh(void);

/**
 * Get shop limits configuration
 */
const ShopLimitsConfig* ShopConfig_GetLimits(void);

/* ========================================================================
 * PUBLIC API - PARTY CONFIGURATION
 * ======================================================================== */

/**
 * Get party bonus configuration
 */
const PartyBonusConfig* PartyConfig_GetBonus(void);

/**
 * Get party level gap configuration
 */
const PartyLevelGapConfig* PartyConfig_GetLevelGap(void);

/* ========================================================================
 * CACHE STATISTICS (Stage 2)
 * ======================================================================== */

/**
 * Print skill configuration cache statistics
 * Shows hit rate, miss rate, and cache efficiency
 */
void GameConfig_PrintCacheStats(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CONFIG_GAME_CONFIG_H_ */
