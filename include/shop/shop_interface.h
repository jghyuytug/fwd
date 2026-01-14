/**
 * Shop Module - Public Interface
 * Purpose: NPC shops, secret shops, item buying/selling system
 *
 * Components:
 * - NPC shop management
 * - Secret shop (random items)
 * - Item purchase and sale
 * - Shop inventory and refresh
 * - Price calculation
 * - Limited-time shops
 */

#ifndef INCLUDE_SHOP_SHOP_INTERFACE_H_
#define INCLUDE_SHOP_SHOP_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Shop types
 */
typedef enum {
    SHOP_TYPE_NPC = 0,          // Regular NPC shop
    SHOP_TYPE_SECRET = 1,       // Secret shop (random items)
    SHOP_TYPE_LIMITED = 2,      // Limited-time shop
    SHOP_TYPE_SPECIAL = 3,      // Special event shop
    SHOP_TYPE_GUILD = 4,        // Guild shop
    SHOP_TYPE_PVP = 5           // PVP reward shop
} ShopType;

/**
 * Shop item slot
 */
typedef struct {
    unsigned int item_id;
    int stock;                  // -1 for unlimited
    unsigned int price_gold;
    unsigned int price_special; // Special currency (guild points, PVP points, etc.)
    int is_available;           // Can be purchased
    unsigned int refresh_time;  // When item refreshes (0 for permanent)
} ShopItemSlot;

/**
 * Shop information
 */
typedef struct {
    unsigned int shop_id;
    ShopType shop_type;
    unsigned int npc_id;        // 0 for non-NPC shops
    char shop_name[64];
    int item_count;
    ShopItemSlot items[50];     // Up to 50 items per shop
    unsigned int last_refresh;
    unsigned int next_refresh;
    int refresh_interval_sec;   // -1 for no auto-refresh
} ShopInfo;

/**
 * Shop transaction record
 */
typedef struct {
    unsigned int transaction_id;
    unsigned int user_id;
    unsigned int shop_id;
    unsigned int item_id;
    int quantity;
    unsigned int price_paid;
    unsigned int transaction_time;
    int is_purchase;            // 1 for buy, 0 for sell
} ShopTransaction;

/**
 * Shop statistics
 */
typedef struct {
    unsigned int total_purchases;
    unsigned int total_sales;
    unsigned int total_gold_spent;
    unsigned int total_gold_earned;
    unsigned int items_purchased;
    unsigned int items_sold;
    unsigned int secret_shop_refreshes;
} ShopStats;

/**
 * Initialize Shop Module
 *
 * Sets up shop system
 *
 * Parameters:
 *   max_shops: Maximum number of shops
 *   max_transactions_history: Max transaction records to keep
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_Initialize(int max_shops, int max_transactions_history);

/**
 * Cleanup Shop Module
 *
 * Releases shop system resources
 */
void Shop_Cleanup();

/**
 * Create Shop
 *
 * Creates a new shop instance
 *
 * Parameters:
 *   shop_type: Type of shop
 *   npc_id: NPC ID (0 for non-NPC shops)
 *   shop_name: Shop name
 *
 * Returns: Shop ID on success, negative error code on failure
 */
int Shop_Create(ShopType shop_type, unsigned int npc_id, const char* shop_name);

/**
 * Add Item to Shop
 *
 * Adds an item to shop inventory
 *
 * Parameters:
 *   shop_id: Shop ID
 *   item_id: Item ID
 *   stock: Stock amount (-1 for unlimited)
 *   price_gold: Price in gold
 *   price_special: Price in special currency (0 if not used)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_AddItem(unsigned int shop_id, unsigned int item_id, int stock,
                 unsigned int price_gold, unsigned int price_special);

/**
 * Remove Item from Shop
 *
 * Removes an item from shop inventory
 *
 * Parameters:
 *   shop_id: Shop ID
 *   item_id: Item ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_RemoveItem(unsigned int shop_id, unsigned int item_id);

/**
 * Purchase Item
 *
 * Player purchases item from shop
 *
 * Parameters:
 *   user_id: User ID
 *   shop_id: Shop ID
 *   item_id: Item ID
 *   quantity: Quantity to purchase
 *
 * Returns: Transaction ID on success, negative error code on failure
 */
int Shop_PurchaseItem(unsigned int user_id, unsigned int shop_id,
                      unsigned int item_id, int quantity);

/**
 * Sell Item
 *
 * Player sells item to shop
 *
 * Parameters:
 *   user_id: User ID
 *   shop_id: Shop ID
 *   item_id: Item ID
 *   quantity: Quantity to sell
 *
 * Returns: Transaction ID on success, negative error code on failure
 */
int Shop_SellItem(unsigned int user_id, unsigned int shop_id,
                  unsigned int item_id, int quantity);

/**
 * Get Shop Info
 *
 * Retrieves shop information
 *
 * Parameters:
 *   shop_id: Shop ID
 *   shop_info: Output shop structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_GetInfo(unsigned int shop_id, ShopInfo* shop_info);

/**
 * Refresh Shop
 *
 * Refreshes shop inventory (for secret/limited shops)
 *
 * Parameters:
 *   shop_id: Shop ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_Refresh(unsigned int shop_id);

/**
 * Get Transaction History
 *
 * Retrieves player's transaction history
 *
 * Parameters:
 *   user_id: User ID
 *   transactions: Output array of transactions
 *   max_transactions: Maximum transactions to retrieve
 *
 * Returns: Number of transactions retrieved
 */
int Shop_GetTransactionHistory(unsigned int user_id,
                                ShopTransaction* transactions,
                                int max_transactions);

/**
 * Calculate Sale Price
 *
 * Calculates sell price for an item
 *
 * Parameters:
 *   item_id: Item ID
 *   quantity: Quantity to sell
 *
 * Returns: Sale price in gold
 */
unsigned int Shop_CalculateSalePrice(unsigned int item_id, int quantity);

/**
 * Get Shop Statistics
 *
 * Retrieves shop statistics for a player
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Shop_GetStatistics(unsigned int user_id, ShopStats* stats);

/**
 * Update Shop State
 *
 * Processes shop tick, handles refresh timers
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of shops updated
 */
int Shop_Update(int delta_time_ms);

#endif // INCLUDE_SHOP_SHOP_INTERFACE_H_
