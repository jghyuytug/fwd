/**
 * @file shop_transaction.h
 * @brief Shop Transaction - Internal API for buy/sell operations
 *
 * Phase 6.6 Shop System - Transaction Component
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - Purchase items from shops
 * - Sell items to shops
 * - Transaction validation (currency, stock, permissions)
 * - Transaction history tracking
 * - Integration with log_secret_shop audit table
 *
 * DATA SOURCES:
 * ✅ include/shop/shop_interface.h - Public API
 * ✅ src/orm/audit_logging/taiwan_cain_log/headers/log_secret_shop_orm.h
 * ✅ User confirmed parameters (2025-11-23):
 *    - Transaction history: 500 records per player
 *    - Sell price ratio: 50%
 */

#ifndef SHOP_TRANSACTION_H
#define SHOP_TRANSACTION_H

#include <time.h>
#include "shop/shop_interface.h"
#include "shop/shop_manager.h"
#include "database/headers/db_connection_manager.h"

/* ========================================================================
 * CONFIGURATION - User Confirmed Parameters (2025-11-23)
 * ======================================================================== */

/**
 * ✅ User Confirmed: 问题6 - 选项B
 * Maximum transaction history records per player
 */
#define SHOP_MAX_TRANSACTION_HISTORY       500

/**
 * ✅ User Confirmed: 问题3 - 选项A
 * Sell price ratio (50% of base price)
 */
#define SHOP_SELL_PRICE_RATIO_PERCENT      50

/* ========================================================================
 * ENUMS
 * ======================================================================== */

/**
 * Transaction type
 */
typedef enum {
    TRANSACTION_TYPE_BUY = 0,           /**< Player buys from shop */
    TRANSACTION_TYPE_SELL = 1           /**< Player sells to shop */
} TransactionType;

/**
 * Currency type for shop transactions
 */
typedef enum {
    SHOP_CURRENCY_GOLD = 0,             /**< Normal gold */
    SHOP_CURRENCY_SPECIAL = 1,          /**< Special currency (guild points, PVP points, etc.) */
    SHOP_CURRENCY_BOTH = 2              /**< Requires both currencies */
} ShopCurrencyType;

/**
 * Transaction result codes
 */
typedef enum {
    TRANSACTION_SUCCESS = 0,            /**< Transaction completed successfully */
    TRANSACTION_ERROR_INSUFFICIENT_GOLD = -1,
    TRANSACTION_ERROR_INSUFFICIENT_SPECIAL = -2,
    TRANSACTION_ERROR_OUT_OF_STOCK = -3,
    TRANSACTION_ERROR_INVALID_SHOP = -4,
    TRANSACTION_ERROR_INVALID_ITEM = -5,
    TRANSACTION_ERROR_INVENTORY_FULL = -6,
    TRANSACTION_ERROR_CANNOT_SELL_HERE = -7,
    TRANSACTION_ERROR_QUANTITY_INVALID = -8
} TransactionResult;

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Transaction record
 *
 * Used for transaction history tracking
 */
typedef struct {
    unsigned int transaction_id;        /**< Unique transaction ID */
    unsigned int character_id;          /**< Character ID */
    int shop_id;                        /**< Shop ID */
    ShopType shop_type;                 /**< Shop type */

    TransactionType transaction_type;   /**< Buy or sell */
    unsigned int item_id;               /**< Item ID */
    unsigned int quantity;              /**< Quantity bought/sold */

    /* Currency involved */
    unsigned int gold_amount;           /**< Gold spent/earned */
    unsigned int special_amount;        /**< Special currency spent/earned */

    /* Timestamp */
    time_t transaction_time;            /**< When transaction occurred */
    char time_string[20];               /**< ✅ Format: "YYYY-MM-DD HH:MM:SS" (from ORM) */
} TransactionRecord;

/**
 * Player transaction history
 *
 * Tracks last 500 transactions per player (user confirmed)
 */
typedef struct {
    unsigned int character_id;          /**< Character ID */
    TransactionRecord records[SHOP_MAX_TRANSACTION_HISTORY];  /**< 500 records */
    int record_count;                   /**< Current number of records */
    int next_index;                     /**< Next slot for circular buffer */
} PlayerTransactionHistory;

/**
 * Transaction statistics - global metrics
 */
typedef struct {
    /* Purchase statistics */
    unsigned long long total_purchases;         /**< Total items purchased */
    unsigned long long total_gold_spent;        /**< Total gold spent on purchases */
    unsigned long long total_special_spent;     /**< Total special currency spent */

    /* Sell statistics */
    unsigned long long total_sales;             /**< Total items sold */
    unsigned long long total_gold_earned;       /**< Total gold earned from selling */

    /* Per shop type metrics */
    unsigned long long purchases_by_type[6];    /**< Purchases per shop type */
    unsigned long long sales_by_type[6];        /**< Sales per shop type */
} TransactionStatistics;

/**
 * Transaction Manager state
 */
typedef struct {
    /* Transaction history storage */
    PlayerTransactionHistory* player_histories;  /**< Per-player histories */
    int max_players;                             /**< Maximum tracked players */
    int active_players;                          /**< Currently tracked players */

    /* ID management */
    unsigned int next_transaction_id;            /**< Next transaction ID */

    /* Statistics */
    TransactionStatistics global_stats;

    /* Database */
    DBConnectionManager* db_manager;             /**< Database connection */

    /* State */
    unsigned char is_initialized;
} TransactionManagerState;

/* ========================================================================
 * CORE API - Transaction Lifecycle
 * ======================================================================== */

/**
 * Initialize Transaction Manager
 *
 * @param max_players Maximum players to track history for
 * @param db_manager Database connection manager (can be NULL)
 * @return 0 on success, negative error code on failure
 */
int ShopTransaction_Initialize(int max_players, DBConnectionManager* db_manager);

/**
 * Cleanup Transaction Manager
 */
void ShopTransaction_Cleanup(void);

/* ========================================================================
 * PURCHASE API
 * ======================================================================== */

/**
 * Purchase item from shop
 *
 * @param character_id Character ID
 * @param shop_id Shop ID
 * @param slot_index Item slot in shop (0-49)
 * @param quantity Quantity to purchase
 * @param out_record Output parameter for transaction record (can be NULL)
 * @return TRANSACTION_SUCCESS on success, negative error code on failure
 *
 * Steps:
 * 1. Validate shop and item exist
 * 2. Check stock availability
 * 3. Calculate total cost (gold + special currency)
 * 4. Validate character has enough currency
 *    - Implementation: User_GetCurrency(character_id, CURRENCY_GOLD) >= cost
 * 5. Validate character inventory has space
 *    - Implementation: Inventory_HasFreeSlot(character_id, item_id, quantity)
 * 6. Deduct currency from character
 *    - Implementation: User_SubtractCurrency(character_id, CURRENCY_GOLD, cost)
 * 7. Add item to character inventory
 *    - Implementation: Inventory_AddItem(character_id, item_id, quantity)
 * 8. Decrease shop stock
 * 9. Record transaction
 * 10. Log to log_secret_shop table (if secret shop)
 *     - Implementation: LogSecretShop_Add(db_manager, &log_record)
 *
 * Example:
 *   TransactionRecord record;
 *   int result = ShopTransaction_Purchase(12345, shop_id, 0, 1, &record);
 *   if (result == TRANSACTION_SUCCESS) {
 *       printf("Spent %u gold\n", record.gold_amount);
 *   }
 */
int ShopTransaction_Purchase(unsigned int character_id, int shop_id,
                             int slot_index, unsigned int quantity,
                             TransactionRecord* out_record);

/**
 * Validate purchase before attempting
 *
 * @param character_id Character ID
 * @param shop_id Shop ID
 * @param slot_index Item slot
 * @param quantity Quantity to purchase
 * @param out_gold_needed Output: gold needed (can be NULL)
 * @param out_special_needed Output: special currency needed (can be NULL)
 * @return TRANSACTION_SUCCESS if valid, negative error code if invalid
 *
 * Use this to check if purchase is possible before actually purchasing
 */
int ShopTransaction_ValidatePurchase(unsigned int character_id, int shop_id,
                                     int slot_index, unsigned int quantity,
                                     unsigned int* out_gold_needed,
                                     unsigned int* out_special_needed);

/* ========================================================================
 * SELL API
 * ======================================================================== */

/**
 * Sell item to shop
 *
 * @param character_id Character ID
 * @param shop_id Shop ID (must accept selling)
 * @param item_id Item ID to sell
 * @param quantity Quantity to sell
 * @param out_record Output parameter for transaction record (can be NULL)
 * @return TRANSACTION_SUCCESS on success, negative error code on failure
 *
 * ✅ Sell price = base price × 50% (user confirmed)
 *
 * Steps:
 * 1. Validate shop accepts selling
 * 2. Validate character has item in inventory
 *    - Implementation: Inventory_HasItem(character_id, item_id, quantity)
 * 3. Calculate sell price (base_price × 0.5)
 * 4. Remove item from inventory
 *    - Implementation: Inventory_RemoveItem(character_id, item_id, quantity)
 * 5. Add gold to character wallet
 *    - Implementation: User_AddCurrency(character_id, CURRENCY_GOLD, sell_price)
 * 6. Optionally increase shop stock (if shop resells)
 * 7. Record transaction
 *
 * Example:
 *   TransactionRecord record;
 *   int result = ShopTransaction_Sell(12345, shop_id, 10001, 1, &record);
 *   if (result == TRANSACTION_SUCCESS) {
 *       printf("Earned %u gold\n", record.gold_amount);
 *   }
 */
int ShopTransaction_Sell(unsigned int character_id, int shop_id,
                         unsigned int item_id, unsigned int quantity,
                         TransactionRecord* out_record);

/**
 * Calculate sell price for item
 *
 * @param item_id Item ID
 * @param quantity Quantity to sell
 * @return Gold amount (base_price × quantity × 0.5)
 *
 * ✅ Uses 50% sell ratio (user confirmed)
 *
 * Price lookup: Uses PVF item data or ItemInfo ORM table
 * - Implementation: PVF_GetItemInfo(item_id) or ItemInfo_GetByItemId(item_id)
 */
unsigned int ShopTransaction_CalculateSellPrice(unsigned int item_id, unsigned int quantity);

/**
 * Check if shop accepts selling
 *
 * @param shop_id Shop ID
 * @return 1 if accepts, 0 if not, negative error code on failure
 *
 * Typically NPC shops accept selling, but special shops may not
 */
int ShopTransaction_CanSellToShop(int shop_id);

/* ========================================================================
 * TRANSACTION HISTORY API
 * ======================================================================== */

/**
 * Get transaction history for player
 *
 * @param character_id Character ID
 * @param out_records Output array for transaction records
 * @param max_records Maximum records to return (≤ 500)
 * @return Number of records returned, negative error code on failure
 *
 * Returns most recent transactions first (reverse chronological)
 *
 * Example:
 *   TransactionRecord history[100];
 *   int count = ShopTransaction_GetHistory(12345, history, 100);
 *   for (int i = 0; i < count; i++) {
 *       printf("Item %u: %u gold\n", history[i].item_id, history[i].gold_amount);
 *   }
 */
int ShopTransaction_GetHistory(unsigned int character_id,
                               TransactionRecord* out_records,
                               int max_records);

/**
 * Get recent purchases from a specific shop
 *
 * @param character_id Character ID
 * @param shop_id Shop ID to filter by
 * @param out_records Output array
 * @param max_records Maximum records
 * @return Number of records found
 */
int ShopTransaction_GetHistoryByShop(unsigned int character_id, int shop_id,
                                     TransactionRecord* out_records,
                                     int max_records);

/**
 * Clear transaction history for player
 *
 * @param character_id Character ID
 * @return 0 on success, negative error code on failure
 */
int ShopTransaction_ClearHistory(unsigned int character_id);

/* ========================================================================
 * STATISTICS API
 * ======================================================================== */

/**
 * Get global transaction statistics
 *
 * @param out_stats Output parameter for statistics
 */
void ShopTransaction_GetStatistics(TransactionStatistics* out_stats);

/**
 * Get player's total spending
 *
 * @param character_id Character ID
 * @param out_gold_spent Output: total gold spent (can be NULL)
 * @param out_special_spent Output: total special spent (can be NULL)
 * @return 0 on success, negative error code on failure
 */
int ShopTransaction_GetPlayerSpending(unsigned int character_id,
                                      unsigned long long* out_gold_spent,
                                      unsigned long long* out_special_spent);

/**
 * Get most purchased item globally
 *
 * @return Item ID of most purchased item
 */
unsigned int ShopTransaction_GetMostPurchasedItem(void);

/* ========================================================================
 * AUDIT LOGGING API
 * ======================================================================== */

/**
 * Log secret shop transaction to database
 *
 * ✅ Integrates with log_secret_shop ORM table
 *
 * @param record Transaction record to log
 * @return 0 on success, negative error code on failure
 *
 * Fields logged (from log_secret_shop ORM):
 * - npc_idx: Shop NPC index
 * - dungeon_idx: Dungeon index (if applicable)
 * - show_count: Display count
 * - show_charac_count: Character view count
 * - buy_count: Purchase count
 * - price: Purchase price
 *
 * Database integration: LogSecretShop_Add(db_manager, &log_entry)
 * See: src/orm/audit_logging/taiwan_cain_log/headers/log_secret_shop_orm.h
 */
int ShopTransaction_LogSecretShop(const TransactionRecord* record);

/* ========================================================================
 * UTILITY API
 * ======================================================================== */

/**
 * Get transaction type name (for debugging/logging)
 *
 * @param type Transaction type
 * @return String name
 */
const char* ShopTransaction_GetTypeName(TransactionType type);

/**
 * Get currency type name (for debugging/logging)
 *
 * @param currency Currency type
 * @return String name
 */
const char* ShopTransaction_GetCurrencyName(ShopCurrencyType currency);

/**
 * Format transaction time string
 *
 * @param timestamp Unix timestamp
 * @param out_string Output buffer (must be ≥ 20 bytes)
 *
 * ✅ Format: "YYYY-MM-DD HH:MM:SS" (from ORM log_secret_shop)
 */
void ShopTransaction_FormatTime(time_t timestamp, char* out_string);

#endif /* SHOP_TRANSACTION_H */
