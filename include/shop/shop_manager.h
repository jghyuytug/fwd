/**
 * @file shop_manager.h
 * @brief Shop Manager - Internal API for shop lifecycle management
 *
 * Phase 6.6 Shop System - Shop Manager Component
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - Shop instance creation and destruction (6 shop types)
 * - Item inventory management (50 items per shop)
 * - Stock tracking (unlimited or limited)
 * - Price management (gold + special currency)
 * - Shop lookup and querying
 *
 * DATA SOURCES:
 * ✅ include/shop/shop_interface.h - Public API (262 lines)
 * ✅ User confirmed parameters (2025-11-23):
 *    - Max shops: 500
 *    - Secret shop refresh: 7200 seconds
 *    - Sell price ratio: 50%
 */

#ifndef SHOP_MANAGER_H
#define SHOP_MANAGER_H

#include <time.h>
#include "shop/shop_interface.h"
#include "database/headers/db_connection_manager.h"

/* ========================================================================
 * CONFIGURATION - User Confirmed Parameters (2025-11-23)
 * ======================================================================== */

/**
 * ✅ User Confirmed: 问题1 - 方案B
 * Maximum number of shops that can exist simultaneously across all types
 */
#define SHOP_MAX_SHOPS                      500

/**
 * ✅ From shop_interface.h
 * Maximum items per shop inventory
 */
#define SHOP_MAX_ITEMS_PER_SHOP            50

/**
 * ✅ User Confirmed: 问题3 - 选项A
 * Sell price ratio when players sell items to shop
 * 50% means shop pays half of the item's base price
 */
#define SHOP_SELL_PRICE_RATIO              0.5f

/**
 * ✅ User Confirmed: 问题2 - 选项B
 * Secret shop refresh interval in seconds (2 hours)
 */
#define SHOP_SECRET_REFRESH_INTERVAL       7200

/**
 * Invalid shop ID marker
 */
#define SHOP_INVALID_ID                    -1

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Shop item slot - represents one item in shop inventory
 *
 * ✅ Based on shop_interface.h ShopItemSlot definition
 */
typedef struct {
    unsigned int item_id;           /**< Item template ID (0 = empty slot) */
    int stock;                      /**< Current stock (-1 = unlimited) */
    int max_stock;                  /**< Maximum stock for limited items */
    unsigned int price_gold;        /**< Gold price */
    unsigned int price_special;     /**< Special currency price (guild points, PVP points, etc.) */
    unsigned int refresh_time;      /**< Next refresh time (for secret/limited shops) */
    unsigned char slot_index;       /**< Slot position (0-49) */
} ShopItemSlot_Internal;

/**
 * Shop instance - complete shop state
 *
 * ✅ Extends shop_interface.h ShopInfo structure
 */
typedef struct {
    int shop_id;                            /**< Unique shop ID */
    ShopType shop_type;                     /**< Shop type (0-5) */
    unsigned int npc_id;                    /**< Associated NPC ID (for NPC shops) */
    char shop_name[64];                     /**< Shop display name */

    /* Item inventory (50 slots) */
    ShopItemSlot_Internal items[SHOP_MAX_ITEMS_PER_SHOP];
    int item_count;                         /**< Number of active items */

    /* Shop state */
    unsigned char is_active;                /**< Shop enabled/disabled */
    time_t creation_time;                   /**< When shop was created */
    time_t last_refresh_time;               /**< Last inventory refresh */
    time_t next_refresh_time;               /**< Next scheduled refresh */

    /* Statistics */
    unsigned int total_purchases;           /**< Total successful purchases */
    unsigned int total_gold_earned;         /**< Total gold collected */
    unsigned int total_special_earned;      /**< Total special currency collected */
} ShopInstance;

/**
 * Shop statistics - global shop system metrics
 */
typedef struct {
    unsigned int total_shops_created;       /**< Total shops created */
    unsigned int active_shops;              /**< Currently active shops */

    /* Per-type counts */
    unsigned int npc_shops;                 /**< Active NPC shops */
    unsigned int secret_shops;              /**< Active secret shops */
    unsigned int limited_shops;             /**< Active limited shops */
    unsigned int special_shops;             /**< Active special shops */
    unsigned int guild_shops;               /**< Active guild shops */
    unsigned int pvp_shops;                 /**< Active PVP shops */

    /* Transaction metrics */
    unsigned long long total_purchases;     /**< All-time purchases */
    unsigned long long total_gold_traded;   /**< All-time gold spent */
} ShopStatistics;

/**
 * Shop Manager global state
 */
typedef struct {
    /* Shop storage */
    ShopInstance* shop_instances;           /**< Array of shops (500 capacity) */
    int max_shops;                          /**< Maximum shops (500) */
    int active_shop_count;                  /**< Current active shops */

    /* ID management */
    int next_shop_id;                       /**< Next available shop ID */

    /* Lookup tables */
    int* npc_shop_map;                      /**< NPC ID → shop_id mapping */
    int npc_shop_map_capacity;              /**< NPC map capacity */

    /* Statistics */
    ShopStatistics global_stats;            /**< Global metrics */

    /* Configuration */
    float sell_price_ratio;                 /**< Sell price ratio (0.5 = 50%) */
    unsigned int secret_refresh_interval;   /**< Secret shop refresh (7200s) */

    /* Database */
    DBConnectionManager* db_manager;        /**< Database connection */

    /* State */
    unsigned char is_initialized;           /**< Initialization flag */
} ShopManagerState;

/* ========================================================================
 * CORE API - Shop Lifecycle Management
 * ======================================================================== */

/**
 * Initialize Shop Manager
 *
 * @param max_shops Maximum number of shops (use SHOP_MAX_SHOPS = 500)
 * @param db_manager Database connection manager (can be NULL)
 * @return 0 on success, negative error code on failure
 *
 * Example:
 *   ShopManager_Initialize(SHOP_MAX_SHOPS, &db_manager);
 */
int ShopManager_Initialize(int max_shops, DBConnectionManager* db_manager);

/**
 * Cleanup Shop Manager
 *
 * Frees all allocated resources
 */
void ShopManager_Cleanup(void);

/**
 * Create a new shop
 *
 * @param shop_type Shop type (SHOP_TYPE_NPC, etc.)
 * @param npc_id Associated NPC ID (0 for non-NPC shops)
 * @param shop_name Shop display name
 * @param out_shop Output parameter for created shop
 * @return shop_id on success, negative error code on failure
 *
 * Example:
 *   ShopInstance shop;
 *   int shop_id = ShopManager_CreateShop(SHOP_TYPE_NPC, 1001, "武器商店", &shop);
 */
int ShopManager_CreateShop(ShopType shop_type, unsigned int npc_id,
                           const char* shop_name, ShopInstance* out_shop);

/**
 * Delete a shop
 *
 * @param shop_id Shop ID to delete
 * @return 0 on success, negative error code on failure
 */
int ShopManager_DeleteShop(int shop_id);

/**
 * Get shop by ID
 *
 * @param shop_id Shop ID
 * @param out_shop Output parameter for shop data
 * @return 0 on success, negative error code on failure
 */
int ShopManager_GetShop(int shop_id, ShopInstance* out_shop);

/**
 * Get shop by NPC ID
 *
 * @param npc_id NPC ID
 * @param out_shop Output parameter for shop data
 * @return 0 on success, negative error code on failure
 */
int ShopManager_GetShopByNPC(unsigned int npc_id, ShopInstance* out_shop);

/* ========================================================================
 * ITEM MANAGEMENT API
 * ======================================================================== */

/**
 * Add item to shop inventory
 *
 * @param shop_id Shop ID
 * @param item_id Item template ID
 * @param stock Initial stock (-1 for unlimited)
 * @param price_gold Gold price
 * @param price_special Special currency price
 * @return Slot index (0-49) on success, negative error code on failure
 *
 * Example:
 *   // Add unlimited stock item
 *   ShopManager_AddItem(shop_id, 10001, -1, 50000, 0);
 *
 *   // Add limited stock item (100 units)
 *   ShopManager_AddItem(shop_id, 20001, 100, 100000, 500);
 */
int ShopManager_AddItem(int shop_id, unsigned int item_id, int stock,
                        unsigned int price_gold, unsigned int price_special);

/**
 * Remove item from shop inventory
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @return 0 on success, negative error code on failure
 */
int ShopManager_RemoveItem(int shop_id, int slot_index);

/**
 * Update item stock
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param new_stock New stock amount (-1 for unlimited)
 * @return 0 on success, negative error code on failure
 */
int ShopManager_UpdateStock(int shop_id, int slot_index, int new_stock);

/**
 * Update item price
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param price_gold New gold price
 * @param price_special New special currency price
 * @return 0 on success, negative error code on failure
 */
int ShopManager_UpdatePrice(int shop_id, int slot_index,
                            unsigned int price_gold, unsigned int price_special);

/**
 * Get item from shop inventory
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param out_item Output parameter for item data
 * @return 0 on success, negative error code on failure
 */
int ShopManager_GetItem(int shop_id, int slot_index, ShopItemSlot_Internal* out_item);

/**
 * Find item in shop by item_id
 *
 * @param shop_id Shop ID
 * @param item_id Item template ID to search for
 * @return Slot index (0-49) on success, -1 if not found
 */
int ShopManager_FindItem(int shop_id, unsigned int item_id);

/* ========================================================================
 * STOCK MANAGEMENT API
 * ======================================================================== */

/**
 * Decrease item stock (called during purchase)
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param quantity Quantity to decrease
 * @return 0 on success, negative error code on failure
 *
 * Note: Does nothing if stock is -1 (unlimited)
 */
int ShopManager_DecreaseStock(int shop_id, int slot_index, int quantity);

/**
 * Increase item stock (called during sell or restock)
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param quantity Quantity to increase
 * @return 0 on success, negative error code on failure
 */
int ShopManager_IncreaseStock(int shop_id, int slot_index, int quantity);

/**
 * Check if item is in stock
 *
 * @param shop_id Shop ID
 * @param slot_index Slot index (0-49)
 * @param quantity Quantity needed
 * @return 1 if in stock, 0 if out of stock, negative error code on failure
 */
int ShopManager_IsInStock(int shop_id, int slot_index, int quantity);

/* ========================================================================
 * QUERY API
 * ======================================================================== */

/**
 * Get all shops of a specific type
 *
 * @param shop_type Shop type to filter by
 * @param out_shops Output array for shop instances
 * @param max_shops Maximum shops to return
 * @return Number of shops found, negative error code on failure
 *
 * Example:
 *   ShopInstance secret_shops[100];
 *   int count = ShopManager_GetShopsByType(SHOP_TYPE_SECRET, secret_shops, 100);
 */
int ShopManager_GetShopsByType(ShopType shop_type, ShopInstance* out_shops, int max_shops);

/**
 * Get all active shops
 *
 * @param out_shops Output array for shop instances
 * @param max_shops Maximum shops to return
 * @return Number of shops found, negative error code on failure
 */
int ShopManager_GetActiveShops(ShopInstance* out_shops, int max_shops);

/**
 * Get shop statistics
 *
 * @param out_stats Output parameter for statistics
 */
void ShopManager_GetStatistics(ShopStatistics* out_stats);

/* ========================================================================
 * UTILITY API
 * ======================================================================== */

/**
 * Calculate sell price for an item
 *
 * @param base_price Item's base purchase price
 * @return Sell price (base_price × 0.5)
 *
 * ✅ User Confirmed: 50% sell price ratio
 */
unsigned int ShopManager_CalculateSellPrice(unsigned int base_price);

/**
 * Validate shop ID
 *
 * @param shop_id Shop ID to validate
 * @return 1 if valid, 0 if invalid
 */
int ShopManager_IsValidShopID(int shop_id);

/**
 * Get shop type name (for debugging/logging)
 *
 * @param shop_type Shop type
 * @return String name of shop type
 */
const char* ShopManager_GetShopTypeName(ShopType shop_type);

#endif /* SHOP_MANAGER_H */
