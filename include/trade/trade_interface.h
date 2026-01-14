/**
 * Trade Module - Public Interface
 * Purpose: Player-to-player trading system with item and gold exchange
 *
 * Components:
 * - Trade request and acceptance
 * - Item and gold exchange
 * - Trade confirmation and completion
 * - Trade history and logging
 * - Security and validation
 */

#ifndef INCLUDE_TRADE_TRADE_INTERFACE_H_
#define INCLUDE_TRADE_TRADE_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Trade states
 */
typedef enum {
    TRADE_STATE_PENDING = 0,     // Trade request pending
    TRADE_STATE_ACCEPTED = 1,    // Trade accepted by both parties
    TRADE_STATE_LOCKED = 2,      // Trade locked (items confirmed)
    TRADE_STATE_COMPLETED = 3,   // Trade successfully completed
    TRADE_STATE_CANCELLED = 4    // Trade cancelled
} TradeState;

/**
 * Trade item slot
 */
typedef struct {
    unsigned int item_id;
    int quantity;
    int is_confirmed;            // Has this item been confirmed by owner
} TradeItemSlot;

/**
 * Trade information
 */
typedef struct {
    unsigned int trade_id;
    unsigned int initiator_id;   // Player who started the trade
    unsigned int target_id;      // Player who received trade request
    TradeState state;

    // Initiator's offer
    TradeItemSlot initiator_items[10];
    int initiator_item_count;
    unsigned int initiator_gold;
    int initiator_confirmed;

    // Target's offer
    TradeItemSlot target_items[10];
    int target_item_count;
    unsigned int target_gold;
    int target_confirmed;

    // Timestamps
    unsigned int request_time;
    unsigned int accept_time;
    unsigned int complete_time;
    unsigned int timeout_time;
} TradeInfo;

/**
 * Trade statistics
 */
typedef struct {
    unsigned int total_trades_completed;
    unsigned int total_trades_cancelled;
    unsigned int total_items_exchanged;
    unsigned int total_gold_exchanged;
    unsigned int total_trade_requests_sent;
    unsigned int total_trade_requests_declined;
} TradeStats;

/**
 * Initialize Trade Module
 *
 * Sets up trading system
 *
 * Parameters:
 *   max_concurrent_trades: Maximum concurrent trades
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_Initialize(int max_concurrent_trades);

/**
 * Cleanup Trade Module
 *
 * Releases trading system resources
 */
void Trade_Cleanup();

/**
 * Request Trade
 *
 * Player initiates a trade with another player
 *
 * Parameters:
 *   initiator_id: User ID of player initiating trade
 *   target_id: User ID of target player
 *
 * Returns: Trade ID on success, negative error code on failure
 */
int Trade_RequestTrade(unsigned int initiator_id, unsigned int target_id);

/**
 * Accept Trade
 *
 * Player accepts a trade request
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID accepting
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_AcceptTrade(unsigned int trade_id, unsigned int user_id);

/**
 * Decline Trade
 *
 * Player declines a trade request
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID declining
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_DeclineTrade(unsigned int trade_id, unsigned int user_id);

/**
 * Add Item to Trade
 *
 * Player adds an item to their side of the trade
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID adding item
 *   item_id: Item ID to add
 *   quantity: Item quantity
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_AddItem(unsigned int trade_id, unsigned int user_id,
                  unsigned int item_id, int quantity);

/**
 * Remove Item from Trade
 *
 * Player removes an item from their side
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID removing item
 *   slot_index: Item slot index to remove
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_RemoveItem(unsigned int trade_id, unsigned int user_id, int slot_index);

/**
 * Set Gold Amount
 *
 * Player sets gold amount to trade
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID
 *   gold_amount: Gold amount to offer
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_SetGold(unsigned int trade_id, unsigned int user_id,
                  unsigned int gold_amount);

/**
 * Confirm Trade
 *
 * Player confirms their side of the trade
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID confirming
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_ConfirmTrade(unsigned int trade_id, unsigned int user_id);

/**
 * Complete Trade
 *
 * Completes the trade and exchanges items/gold
 *
 * Parameters:
 *   trade_id: Trade ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_CompleteTrade(unsigned int trade_id);

/**
 * Cancel Trade
 *
 * Cancels an active trade
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: User ID cancelling
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_CancelTrade(unsigned int trade_id, unsigned int user_id);

/**
 * Get Trade Info
 *
 * Retrieves trade information
 *
 * Parameters:
 *   trade_id: Trade ID
 *   trade_info: Output trade structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_GetInfo(unsigned int trade_id, TradeInfo* trade_info);

/**
 * Get Trade History
 *
 * Retrieves player's trade history
 *
 * Parameters:
 *   user_id: User ID
 *   trades: Output array of trade info
 *   max_trades: Maximum trades to retrieve
 *
 * Returns: Number of trades retrieved
 */
int Trade_GetHistory(unsigned int user_id, TradeInfo* trades, int max_trades);

/**
 * Get Trade Statistics
 *
 * Retrieves trade statistics for a player
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Trade_GetStatistics(unsigned int user_id, TradeStats* stats);

/**
 * Update Trade State
 *
 * Processes trade tick, checks timeouts
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of trades processed
 */
int Trade_Update(int delta_time_ms);

#endif // INCLUDE_TRADE_TRADE_INTERFACE_H_
