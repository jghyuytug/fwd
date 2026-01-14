/**
 * @file shop_restriction.h
 * @brief Shop Restrictions - Internal API for purchase limit system
 *
 * Phase 6.6 Shop System - Restrictions Component
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - Cera shop purchase limits enforcement
 * - Time-based purchase restrictions
 * - Daily/weekly/monthly purchase caps
 * - Integration with charac_cerashop_restrict ORM table
 * - Restriction reset mechanism
 *
 * DATA SOURCES:
 * ✅ src/orm/core_game/taiwan_cain/headers/charac_cerashop_restrict_orm.h (6 fields)
 */

#ifndef SHOP_RESTRICTION_H
#define SHOP_RESTRICTION_H

#include <time.h>
#include "database/headers/db_connection_manager.h"

/* ========================================================================
 * CONFIGURATION
 * ======================================================================== */

/**
 * Maximum tracked restrictions per player
 */
#define SHOP_MAX_RESTRICTIONS_PER_PLAYER   100

/**
 * Restriction check result codes
 */
#define RESTRICTION_OK                     0
#define RESTRICTION_ERROR_LIMIT_REACHED    -1
#define RESTRICTION_ERROR_TIME_LOCKED      -2
#define RESTRICTION_ERROR_NOT_FOUND        -3

/* ========================================================================
 * ENUMS
 * ======================================================================== */

/**
 * Restriction period type
 */
typedef enum {
    RESTRICTION_PERIOD_NONE = 0,        /**< No time restriction */
    RESTRICTION_PERIOD_DAILY = 1,       /**< Daily purchase limit */
    RESTRICTION_PERIOD_WEEKLY = 2,      /**< Weekly purchase limit */
    RESTRICTION_PERIOD_MONTHLY = 3,     /**< Monthly purchase limit */
    RESTRICTION_PERIOD_PERMANENT = 4,   /**< One-time purchase only */
    RESTRICTION_PERIOD_CUSTOM = 5       /**< Custom time period */
} RestrictionPeriod;

/**
 * Restriction type
 */
typedef enum {
    RESTRICTION_TYPE_COUNT = 0,         /**< Limit by purchase count */
    RESTRICTION_TYPE_TIME = 1,          /**< Time-based cooldown */
    RESTRICTION_TYPE_BOTH = 2           /**< Both count and time */
} RestrictionType;

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Purchase restriction record
 *
 * ✅ Based on charac_cerashop_restrict ORM table (6 fields)
 */
typedef struct {
    unsigned int charac_no;             /**< ✅ Character ID */
    unsigned int ipg_no;                /**< ✅ Item product ID */
    unsigned int count;                 /**< ✅ Current purchase count */
    unsigned int next_date;             /**< ✅ Next available purchase time */
    unsigned int end_date;              /**< ✅ Restriction end time */
    unsigned int last_access_date;      /**< ✅ Last access time */
} CharacCerashopRestrict;

/**
 * Item restriction configuration
 *
 * Defines purchase limits for an item
 */
typedef struct {
    unsigned int item_id;               /**< Item ID */
    RestrictionType type;               /**< Restriction type */
    RestrictionPeriod period;           /**< Restriction period */

    /* Count-based restrictions */
    unsigned int max_count;             /**< Max purchases per period */

    /* Time-based restrictions */
    unsigned int cooldown_seconds;      /**< Cooldown between purchases */

    /* Period duration (for custom periods) */
    unsigned int period_duration;       /**< Period duration in seconds */
} ItemRestrictionConfig;

/**
 * Player restriction data
 *
 * Tracks all restrictions for a single player
 */
typedef struct {
    unsigned int character_id;          /**< Character ID */
    CharacCerashopRestrict restrictions[SHOP_MAX_RESTRICTIONS_PER_PLAYER];
    int restriction_count;              /**< Number of active restrictions */
} PlayerRestrictionData;

/**
 * Restriction statistics
 */
typedef struct {
    unsigned int total_restrictions;    /**< Total active restrictions */
    unsigned int daily_restrictions;    /**< Daily limit restrictions */
    unsigned int weekly_restrictions;   /**< Weekly limit restrictions */
    unsigned int monthly_restrictions;  /**< Monthly limit restrictions */
    unsigned int permanent_restrictions;/**< One-time purchase restrictions */

    unsigned int total_denied_purchases;/**< Purchases denied due to limits */
} RestrictionStatistics;

/**
 * Restriction Manager state
 */
typedef struct {
    /* Player restriction storage */
    PlayerRestrictionData* player_data; /**< Per-player restriction data */
    int max_players;                    /**< Maximum tracked players */
    int active_players;                 /**< Currently tracked players */

    /* Item restriction configs */
    ItemRestrictionConfig* item_configs;/**< Per-item restriction configs */
    int max_items;                      /**< Maximum configured items */
    int active_configs;                 /**< Active configurations */

    /* Statistics */
    RestrictionStatistics global_stats;

    /* Database */
    DBConnectionManager* db_manager;    /**< Database connection */

    /* State */
    unsigned char is_initialized;
} RestrictionManagerState;

/* ========================================================================
 * CORE API - Restriction Manager Lifecycle
 * ======================================================================== */

/**
 * Initialize Shop Restriction Manager
 *
 * @param max_players Maximum players to track
 * @param max_items Maximum items to configure
 * @param db_manager Database connection manager (can be NULL)
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_Initialize(int max_players, int max_items,
                                DBConnectionManager* db_manager);

/**
 * Cleanup Shop Restriction Manager
 */
void ShopRestriction_Cleanup(void);

/* ========================================================================
 * RESTRICTION CONFIGURATION API
 * ======================================================================== */

/**
 * Configure item purchase restriction
 *
 * @param item_id Item ID (ipg_no)
 * @param type Restriction type
 * @param period Restriction period
 * @param max_count Max purchases per period (0 for unlimited)
 * @param cooldown_seconds Cooldown between purchases (0 for none)
 * @return 0 on success, negative error code on failure
 *
 * Example:
 *   // Daily limit: 5 purchases per day
 *   ShopRestriction_ConfigureItem(10001, RESTRICTION_TYPE_COUNT,
 *                                 RESTRICTION_PERIOD_DAILY, 5, 0);
 *
 *   // Cooldown: 1 hour between purchases
 *   ShopRestriction_ConfigureItem(20001, RESTRICTION_TYPE_TIME,
 *                                 RESTRICTION_PERIOD_NONE, 0, 3600);
 *
 *   // One-time purchase only
 *   ShopRestriction_ConfigureItem(30001, RESTRICTION_TYPE_COUNT,
 *                                 RESTRICTION_PERIOD_PERMANENT, 1, 0);
 */
int ShopRestriction_ConfigureItem(unsigned int item_id, RestrictionType type,
                                  RestrictionPeriod period,
                                  unsigned int max_count,
                                  unsigned int cooldown_seconds);

/**
 * Remove restriction configuration for item
 *
 * @param item_id Item ID
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_RemoveItemConfig(unsigned int item_id);

/**
 * Get item restriction configuration
 *
 * @param item_id Item ID
 * @param out_config Output parameter for configuration
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_GetItemConfig(unsigned int item_id, ItemRestrictionConfig* out_config);

/* ========================================================================
 * RESTRICTION CHECKING API
 * ======================================================================== */

/**
 * Check if purchase is allowed
 *
 * @param character_id Character ID
 * @param item_id Item ID (ipg_no)
 * @param quantity Quantity to purchase
 * @param current_time Current server time
 * @return RESTRICTION_OK if allowed, negative error code if denied
 *
 * Checks:
 * - Purchase count vs max_count
 * - Cooldown time vs current_time
 * - Period expiration
 *
 * Example:
 *   time_t now = time(NULL);
 *   int result = ShopRestriction_CheckPurchase(12345, 10001, 1, now);
 *   if (result == RESTRICTION_OK) {
 *       // Proceed with purchase
 *   } else if (result == RESTRICTION_ERROR_LIMIT_REACHED) {
 *       // Send "daily limit reached" message
 *   } else if (result == RESTRICTION_ERROR_TIME_LOCKED) {
 *       // Send "please wait" message
 *   }
 */
int ShopRestriction_CheckPurchase(unsigned int character_id, unsigned int item_id,
                                  unsigned int quantity, time_t current_time);

/**
 * Get remaining purchases for item
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @return Remaining purchase count (999999 if unlimited)
 *
 * Example:
 *   int remaining = ShopRestriction_GetRemainingPurchases(12345, 10001);
 *   printf("You can purchase %d more times today\n", remaining);
 */
int ShopRestriction_GetRemainingPurchases(unsigned int character_id, unsigned int item_id);

/**
 * Get time until next purchase is allowed
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @param current_time Current server time
 * @return Seconds until available (0 if available now)
 */
int ShopRestriction_GetTimeUntilAvailable(unsigned int character_id, unsigned int item_id,
                                          time_t current_time);

/* ========================================================================
 * RESTRICTION RECORDING API
 * ======================================================================== */

/**
 * Record a purchase (update restriction data)
 *
 * @param character_id Character ID
 * @param item_id Item ID (ipg_no)
 * @param quantity Quantity purchased
 * @param current_time Current server time
 * @return 0 on success, negative error code on failure
 *
 * Updates:
 * - count += quantity
 * - last_access_date = current_time
 * - next_date = current_time + cooldown (if applicable)
 * - Sync to charac_cerashop_restrict ORM table via CharacCerashopRestrict_Update()
 *
 * Example (called after successful purchase):
 *   ShopRestriction_RecordPurchase(12345, 10001, 1, time(NULL));
 */
int ShopRestriction_RecordPurchase(unsigned int character_id, unsigned int item_id,
                                   unsigned int quantity, time_t current_time);

/**
 * Get restriction record for player and item
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @param out_record Output parameter for restriction record
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_GetRecord(unsigned int character_id, unsigned int item_id,
                              CharacCerashopRestrict* out_record);

/* ========================================================================
 * RESTRICTION RESET API
 * ======================================================================== */

/**
 * Reset restrictions for daily period
 *
 * Call this once per day (server maintenance or scheduled task)
 *
 * @return Number of restrictions reset
 */
int ShopRestriction_ResetDaily(void);

/**
 * Reset restrictions for weekly period
 *
 * @return Number of restrictions reset
 */
int ShopRestriction_ResetWeekly(void);

/**
 * Reset restrictions for monthly period
 *
 * @return Number of restrictions reset
 */
int ShopRestriction_ResetMonthly(void);

/**
 * Reset all restrictions for a player
 *
 * @param character_id Character ID
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_ResetPlayer(unsigned int character_id);

/**
 * Reset specific item restriction for a player (GM command)
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_ResetPlayerItem(unsigned int character_id, unsigned int item_id);

/* ========================================================================
 * BATCH UPDATE API
 * ======================================================================== */

/**
 * Update all restrictions (call periodically)
 *
 * @param current_time Current server time
 * @return Number of restrictions updated/expired
 *
 * Checks for:
 * - Expired restriction periods
 * - Cooldowns that have expired
 * - Period rollovers (daily → next day, etc.)
 *
 * Example (in main game loop or scheduled task):
 *   time_t now = time(NULL);
 *   ShopRestriction_UpdateAll(now);
 */
int ShopRestriction_UpdateAll(time_t current_time);

/* ========================================================================
 * QUERY API
 * ======================================================================== */

/**
 * Get all restrictions for a player
 *
 * @param character_id Character ID
 * @param out_records Output array for restriction records
 * @param max_records Maximum records to return
 * @return Number of records returned, negative error code on failure
 */
int ShopRestriction_GetPlayerRestrictions(unsigned int character_id,
                                          CharacCerashopRestrict* out_records,
                                          int max_records);

/**
 * Get all players with restrictions on an item
 *
 * @param item_id Item ID
 * @param out_records Output array
 * @param max_records Maximum records
 * @return Number of records found
 */
int ShopRestriction_GetItemRestrictions(unsigned int item_id,
                                        CharacCerashopRestrict* out_records,
                                        int max_records);

/* ========================================================================
 * STATISTICS API
 * ======================================================================== */

/**
 * Get restriction statistics
 *
 * @param out_stats Output parameter for statistics
 */
void ShopRestriction_GetStatistics(RestrictionStatistics* out_stats);

/**
 * Get most restricted item (most purchase denials)
 *
 * @return Item ID
 */
unsigned int ShopRestriction_GetMostRestrictedItem(void);

/* ========================================================================
 * DATABASE INTEGRATION API
 * ======================================================================== */

/**
 * Load restriction from database
 *
 * ✅ Integrates with charac_cerashop_restrict ORM table
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @param out_record Output parameter
 * @return 0 on success, negative error code on failure
 *
 * ORM Integration:
 *   #include "orm/core_game/taiwan_cain/headers/charac_cerashop_restrict_orm.h"
 *   CharacCerashopRestrict_GetByCharacAndItem(db_manager, character_id, item_id, out_record);
 */
int ShopRestriction_LoadFromDB(unsigned int character_id, unsigned int item_id,
                               CharacCerashopRestrict* out_record);

/**
 * Save restriction to database
 *
 * @param record Restriction record to save
 * @return 0 on success, negative error code on failure
 *
 * ORM Integration:
 *   CharacCerashopRestrict_AddOrUpdate(db_manager, record);
 *   // Uses INSERT ON DUPLICATE KEY UPDATE for upsert behavior
 */
int ShopRestriction_SaveToDB(const CharacCerashopRestrict* record);

/**
 * Delete restriction from database
 *
 * @param character_id Character ID
 * @param item_id Item ID
 * @return 0 on success, negative error code on failure
 */
int ShopRestriction_DeleteFromDB(unsigned int character_id, unsigned int item_id);

/* ========================================================================
 * UTILITY API
 * ======================================================================== */

/**
 * Get restriction period name (for debugging/logging)
 *
 * @param period Restriction period
 * @return String name
 */
const char* ShopRestriction_GetPeriodName(RestrictionPeriod period);

/**
 * Get restriction type name (for debugging/logging)
 *
 * @param type Restriction type
 * @return String name
 */
const char* ShopRestriction_GetTypeName(RestrictionType type);

/**
 * Calculate period end time
 *
 * @param period Restriction period
 * @param start_time Period start time
 * @return Period end time
 *
 * Examples:
 * - DAILY: start_time + 86400 (next day midnight)
 * - WEEKLY: start_time + 604800 (next week)
 * - MONTHLY: start_time + ~2592000 (next month)
 */
time_t ShopRestriction_CalculatePeriodEnd(RestrictionPeriod period, time_t start_time);

/**
 * Check if restriction period has expired
 *
 * @param record Restriction record
 * @param current_time Current server time
 * @return 1 if expired, 0 if still active
 */
int ShopRestriction_IsPeriodExpired(const CharacCerashopRestrict* record, time_t current_time);

#endif /* SHOP_RESTRICTION_H */
