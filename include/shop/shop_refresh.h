/**
 * @file shop_refresh.h
 * @brief Shop Refresh - Internal API for shop inventory refresh system
 *
 * Phase 6.6 Shop System - Refresh Component
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - Secret shop refresh mechanism (random items)
 * - Limited shop timer management
 * - Random item generation for secret shops
 * - Refresh interval tracking
 * - Scheduled refresh execution
 *
 * DATA SOURCES:
 * ✅ include/shop/shop_interface.h - Shop types and refresh_time
 * ✅ User confirmed parameters (2025-11-23):
 *    - Secret shop refresh interval: 7200 seconds (2 hours)
 */

#ifndef SHOP_REFRESH_H
#define SHOP_REFRESH_H

#include <time.h>
#include "shop/shop_interface.h"
#include "shop/shop_manager.h"

/* ========================================================================
 * CONFIGURATION - User Confirmed Parameters (2025-11-23)
 * ======================================================================== */

/**
 * ✅ User Confirmed: 问题2 - 选项B
 * Secret shop refresh interval in seconds (2 hours)
 */
#define SHOP_REFRESH_SECRET_INTERVAL       7200

/**
 * Limited shop default duration (24 hours)
 * Can be overridden per shop
 */
#define SHOP_REFRESH_LIMITED_DURATION      86400

/**
 * Maximum random items generated per secret shop refresh
 */
#define SHOP_REFRESH_MAX_RANDOM_ITEMS      20

/* ========================================================================
 * ENUMS
 * ======================================================================== */

/**
 * Refresh strategy for shops
 */
typedef enum {
    REFRESH_STRATEGY_NONE = 0,          /**< No automatic refresh (NPC shops) */
    REFRESH_STRATEGY_TIMED = 1,         /**< Time-based refresh (secret shops) */
    REFRESH_STRATEGY_ON_PURCHASE = 2,   /**< Refresh after purchases (limited stock) */
    REFRESH_STRATEGY_MANUAL = 3         /**< Manual GM refresh only */
} RefreshStrategy;

/**
 * Random item pool tier (for secret shops)
 */
typedef enum {
    RANDOM_TIER_COMMON = 0,             /**< Common items (70% chance) */
    RANDOM_TIER_RARE = 1,               /**< Rare items (20% chance) */
    RANDOM_TIER_EPIC = 2,               /**< Epic items (8% chance) */
    RANDOM_TIER_LEGENDARY = 3           /**< Legendary items (2% chance) */
} RandomItemTier;

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Refresh configuration for a shop
 */
typedef struct {
    int shop_id;                        /**< Shop ID */
    RefreshStrategy strategy;           /**< Refresh strategy */

    /* Timing */
    unsigned int refresh_interval;      /**< Interval in seconds (7200 for secret) */
    time_t last_refresh_time;           /**< Last refresh timestamp */
    time_t next_refresh_time;           /**< Next scheduled refresh */

    /* Random generation (for secret shops) */
    unsigned char use_random_items;     /**< Generate random items? */
    int min_random_items;               /**< Minimum items to generate */
    int max_random_items;               /**< Maximum items to generate */

    /* Item pool */
    unsigned int* item_pool;            /**< Pool of possible item IDs */
    int item_pool_size;                 /**< Size of item pool */
} ShopRefreshConfig;

/**
 * Random item generation parameters
 */
typedef struct {
    RandomItemTier tier;                /**< Item tier/rarity */
    unsigned int item_id;               /**< Generated item ID */
    int stock;                          /**< Stock amount (-1 = unlimited) */
    unsigned int price_gold;            /**< Gold price */
    unsigned int price_special;         /**< Special currency price */
} RandomItemSpec;

/**
 * Refresh statistics
 */
typedef struct {
    unsigned int total_refreshes;       /**< Total refreshes executed */
    unsigned int secret_refreshes;      /**< Secret shop refreshes */
    unsigned int limited_refreshes;     /**< Limited shop refreshes */
    unsigned int manual_refreshes;      /**< Manual GM refreshes */

    time_t last_refresh_time;           /**< Last refresh timestamp */
} RefreshStatistics;

/**
 * Refresh Manager state
 */
typedef struct {
    /* Refresh configurations */
    ShopRefreshConfig* refresh_configs; /**< Per-shop refresh configs */
    int max_shops;                      /**< Maximum shops */
    int active_configs;                 /**< Active refresh configs */

    /* Random item pools (for secret shops) */
    unsigned int* global_item_pool;     /**< Global random item pool */
    int global_pool_size;               /**< Pool size */

    /* Statistics */
    RefreshStatistics global_stats;

    /* State */
    unsigned char is_initialized;
} RefreshManagerState;

/* ========================================================================
 * CORE API - Refresh Manager Lifecycle
 * ======================================================================== */

/**
 * Initialize Shop Refresh Manager
 *
 * @param max_shops Maximum shops to manage
 * @return 0 on success, negative error code on failure
 */
int ShopRefresh_Initialize(int max_shops);

/**
 * Cleanup Shop Refresh Manager
 */
void ShopRefresh_Cleanup(void);

/* ========================================================================
 * REFRESH CONFIGURATION API
 * ======================================================================== */

/**
 * Configure shop refresh settings
 *
 * @param shop_id Shop ID
 * @param strategy Refresh strategy
 * @param refresh_interval Interval in seconds (use 7200 for secret shops)
 * @return 0 on success, negative error code on failure
 *
 * Example:
 *   // Configure secret shop to refresh every 2 hours
 *   ShopRefresh_ConfigureShop(shop_id, REFRESH_STRATEGY_TIMED,
 *                             SHOP_REFRESH_SECRET_INTERVAL);
 */
int ShopRefresh_ConfigureShop(int shop_id, RefreshStrategy strategy,
                              unsigned int refresh_interval);

/**
 * Set random item generation for shop
 *
 * @param shop_id Shop ID
 * @param min_items Minimum random items to generate
 * @param max_items Maximum random items to generate
 * @return 0 on success, negative error code on failure
 *
 * Example:
 *   // Secret shop generates 10-20 random items
 *   ShopRefresh_SetRandomGeneration(shop_id, 10, 20);
 */
int ShopRefresh_SetRandomGeneration(int shop_id, int min_items, int max_items);

/**
 * Set custom item pool for shop
 *
 * @param shop_id Shop ID
 * @param item_ids Array of item IDs
 * @param pool_size Size of array
 * @return 0 on success, negative error code on failure
 *
 * If not set, uses global item pool
 */
int ShopRefresh_SetItemPool(int shop_id, const unsigned int* item_ids, int pool_size);

/* ========================================================================
 * MANUAL REFRESH API
 * ======================================================================== */

/**
 * Manually refresh shop inventory
 *
 * @param shop_id Shop ID
 * @return 0 on success, negative error code on failure
 *
 * For secret shops: Generates new random items
 * For other shops: Restocks to max stock
 *
 * Example:
 *   ShopRefresh_RefreshNow(shop_id);  // GM command
 */
int ShopRefresh_RefreshNow(int shop_id);

/**
 * Reset shop to initial state
 *
 * @param shop_id Shop ID
 * @return 0 on success, negative error code on failure
 *
 * Clears all items and restocks from template
 */
int ShopRefresh_Reset(int shop_id);

/* ========================================================================
 * AUTOMATIC REFRESH API
 * ======================================================================== */

/**
 * Update refresh system (call every frame/tick)
 *
 * @param current_time Current server time
 * @return Number of shops refreshed
 *
 * Checks all shops with REFRESH_STRATEGY_TIMED and refreshes if needed
 *
 * Example (in main game loop):
 *   time_t now = time(NULL);
 *   int refreshed = ShopRefresh_Update(now);
 *   if (refreshed > 0) {
 *       printf("%d shops refreshed\n", refreshed);
 *   }
 */
int ShopRefresh_Update(time_t current_time);

/**
 * Check if shop needs refresh
 *
 * @param shop_id Shop ID
 * @param current_time Current server time
 * @return 1 if needs refresh, 0 if not, negative error code on failure
 */
int ShopRefresh_NeedsRefresh(int shop_id, time_t current_time);

/**
 * Get time until next refresh
 *
 * @param shop_id Shop ID
 * @param current_time Current server time
 * @return Seconds until refresh (0 if needs refresh now)
 */
int ShopRefresh_GetTimeUntilRefresh(int shop_id, time_t current_time);

/* ========================================================================
 * RANDOM ITEM GENERATION API (Secret Shops)
 * ======================================================================== */

/**
 * Generate random items for secret shop
 *
 * @param shop_id Shop ID
 * @param count Number of items to generate
 * @param out_specs Output array for generated items
 * @return Number of items generated, negative error code on failure
 *
 * Uses weighted random selection based on tier probabilities:
 * - Common: 70%
 * - Rare: 20%
 * - Epic: 8%
 * - Legendary: 2%
 *
 * Example:
 *   RandomItemSpec items[20];
 *   int count = ShopRefresh_GenerateRandomItems(shop_id, 20, items);
 *   for (int i = 0; i < count; i++) {
 *       ShopManager_AddItem(shop_id, items[i].item_id, items[i].stock,
 *                          items[i].price_gold, items[i].price_special);
 *   }
 */
int ShopRefresh_GenerateRandomItems(int shop_id, int count, RandomItemSpec* out_specs);

/**
 * Generate random item price
 *
 * @param item_id Item ID
 * @param tier Item tier
 * @param out_gold Output: gold price
 * @param out_special Output: special currency price
 *
 * Item Database Integration:
 *   Base prices loaded from item_data table via ItemData_GetByID()
 *   Price multipliers applied per tier:
 *   - Common: 1.0x base price
 *   - Rare: 1.2x base price
 *   - Epic: 1.5x base price
 *   - Legendary: 2.0x base price
 */
void ShopRefresh_GenerateRandomPrice(unsigned int item_id, RandomItemTier tier,
                                     unsigned int* out_gold,
                                     unsigned int* out_special);

/**
 * Select random tier based on probability
 *
 * @return Random tier (weighted)
 */
RandomItemTier ShopRefresh_SelectRandomTier(void);

/* ========================================================================
 * ITEM POOL MANAGEMENT API
 * ======================================================================== */

/**
 * Set global random item pool
 *
 * @param item_ids Array of item IDs
 * @param pool_size Array size
 * @return 0 on success, negative error code on failure
 *
 * This pool is used by all secret shops that don't have custom pools
 *
 * Example:
 *   unsigned int pool[] = {10001, 10002, 10003, 20001, 20002, ...};
 *   ShopRefresh_SetGlobalItemPool(pool, 100);
 */
int ShopRefresh_SetGlobalItemPool(const unsigned int* item_ids, int pool_size);

/**
 * Add item to global pool
 *
 * @param item_id Item ID to add
 * @return 0 on success, negative error code on failure
 */
int ShopRefresh_AddToGlobalPool(unsigned int item_id);

/**
 * Remove item from global pool
 *
 * @param item_id Item ID to remove
 * @return 0 on success, negative error code on failure
 */
int ShopRefresh_RemoveFromGlobalPool(unsigned int item_id);

/**
 * Get random item from pool
 *
 * @param pool Item pool (NULL for global pool)
 * @param pool_size Pool size (ignored if pool is NULL)
 * @return Random item ID
 */
unsigned int ShopRefresh_GetRandomItemFromPool(const unsigned int* pool, int pool_size);

/* ========================================================================
 * STATISTICS API
 * ======================================================================== */

/**
 * Get refresh statistics
 *
 * @param out_stats Output parameter for statistics
 */
void ShopRefresh_GetStatistics(RefreshStatistics* out_stats);

/**
 * Get shop's refresh history
 *
 * @param shop_id Shop ID
 * @param out_last_refresh Output: last refresh time (can be NULL)
 * @param out_next_refresh Output: next refresh time (can be NULL)
 * @return 0 on success, negative error code on failure
 */
int ShopRefresh_GetShopRefreshInfo(int shop_id, time_t* out_last_refresh,
                                   time_t* out_next_refresh);

/* ========================================================================
 * UTILITY API
 * ======================================================================== */

/**
 * Get refresh strategy name (for debugging/logging)
 *
 * @param strategy Refresh strategy
 * @return String name
 */
const char* ShopRefresh_GetStrategyName(RefreshStrategy strategy);

/**
 * Get tier name (for debugging/logging)
 *
 * @param tier Random item tier
 * @return String name
 */
const char* ShopRefresh_GetTierName(RandomItemTier tier);

/**
 * Format refresh time (for display)
 *
 * @param seconds Seconds until refresh
 * @param out_buffer Output buffer (e.g., "1h 23m 45s")
 * @param buffer_size Buffer size
 */
void ShopRefresh_FormatRefreshTime(int seconds, char* out_buffer, int buffer_size);

#endif /* SHOP_REFRESH_H */
