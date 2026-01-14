/**
 * @file trade_manager.h
 * @brief Trade Manager - Internal API
 *
 * Manages player-to-player trading system with item and gold exchange.
 *
 * Key Features:
 * - Trade lifecycle management (request → accept → lock → complete/cancel)
 * - Item and gold exchange with validation
 * - Trade timeout handling (300 seconds)
 * - Daily trade limits (200M gold per player)
 * - Concurrent trade management (1000 max trades)
 * - Trade history and statistics
 *
 * @version 1.0
 * @date 2025-11-23
 */

#ifndef INCLUDE_TRADE_TRADE_MANAGER_H_
#define INCLUDE_TRADE_TRADE_MANAGER_H_

#include <common/defs.h>
#include <common/error_codes.h>
#include <time.h>

/*==============================================================================
 * Constants and Configuration
 *============================================================================*/

/* ✅ User confirmed configuration */
#define TRADE_MAX_CONCURRENT        1000    /* ✅ 最大并发交易数：1000笔 */
#define TRADE_TIMEOUT_SECONDS       300     /* ✅ 交易超时：300秒（5分钟） */
#define TRADE_DAILY_GOLD_LIMIT      200000000 /* ✅ 每日交易金额上限：2亿金币 */

/* ✅ From trade_interface.h */
#define TRADE_MAX_ITEMS_PER_SIDE    10      /* ✅ 每方最多10个物品槽 */

/* Internal constants */
#define TRADE_INVALID_ID            -1      /* 无效交易ID */
#define TRADE_HISTORY_MAX_SIZE      100     /* 每用户最多保存100条交易历史 */

/*==============================================================================
 * Data Types
 *============================================================================*/

/**
 * Trade states
 * ✅ 5个状态：pending → accepted → locked → completed/cancelled
 */
typedef enum {
    TRADE_STATE_PENDING = 0,     /* 交易请求待接受 */
    TRADE_STATE_ACCEPTED = 1,    /* 双方已接受交易 */
    TRADE_STATE_LOCKED = 2,      /* 交易已锁定（双方确认） */
    TRADE_STATE_COMPLETED = 3,   /* 交易成功完成 */
    TRADE_STATE_CANCELLED = 4    /* 交易已取消 */
} TradeState;

/**
 * Trade item slot
 * Represents one item slot in the trade window
 */
typedef struct {
    unsigned int item_id;        /* 物品ID（0表示空槽） */
    int quantity;                /* 数量 */
    int is_confirmed;            /* 是否已被所有者确认 */
} TradeItemSlot;

/**
 * Trade instance
 * ✅ Based on trade_interface.h structure
 */
typedef struct {
    /* Trade identification */
    int trade_id;                /* 交易ID */
    unsigned int initiator_id;   /* 发起者角色编号 */
    unsigned int target_id;      /* 目标者角色编号 */
    TradeState state;            /* 当前状态 */

    /* Initiator's offer (发起者的报价) */
    TradeItemSlot initiator_items[TRADE_MAX_ITEMS_PER_SIDE];
    int initiator_item_count;    /* 物品数量 */
    unsigned int initiator_gold; /* 金币数量 */
    int initiator_confirmed;     /* 是否已确认 */

    /* Target's offer (目标者的报价) */
    TradeItemSlot target_items[TRADE_MAX_ITEMS_PER_SIDE];
    int target_item_count;       /* 物品数量 */
    unsigned int target_gold;    /* 金币数量 */
    int target_confirmed;        /* 是否已确认 */

    /* Timestamps (✅ Based on charac_trade_limit_info.last_trade_time[20]) */
    time_t request_time;         /* 发起时间 */
    time_t accept_time;          /* 接受时间 */
    time_t complete_time;        /* 完成时间 */
    time_t timeout_time;         /* 超时时间 (request_time + 300秒) */

    /* Flags */
    int is_active;               /* 是否活跃 */
} TradeInstance;

/**
 * Trade history entry
 * Records completed or cancelled trades
 */
typedef struct {
    int trade_id;                /* 交易ID */
    unsigned int other_player_id; /* 对方角色ID */
    unsigned int gold_sent;      /* 发送的金币 */
    unsigned int gold_received;  /* 接收的金币 */
    int items_sent;              /* 发送的物品数量 */
    int items_received;          /* 接收的物品数量 */
    time_t trade_time;           /* 交易时间 */
    TradeState final_state;      /* 最终状态 */
} TradeHistoryEntry;

/**
 * Player trade statistics
 * ✅ Based on charac_trade_limit_info ORM table
 */
typedef struct {
    unsigned int character_id;        /* ✅ charac_no */
    char last_trade_time[20];         /* ✅ last_trade_time[20] */
    unsigned int total_trade_gold;    /* ✅ total_trade_gold */
    unsigned short trade_count;       /* ✅ trade_count */

    /* Daily limits tracking */
    unsigned int daily_gold_traded;   /* 今日已交易金币 */
    time_t daily_reset_time;          /* 每日重置时间 */

    /* Trade history */
    TradeHistoryEntry history[TRADE_HISTORY_MAX_SIZE];
    int history_count;
} PlayerTradeStats;

/**
 * Trade Manager global statistics
 */
typedef struct {
    unsigned int total_trades_requested;   /* 总请求交易次数 */
    unsigned int total_trades_completed;   /* 总完成交易次数 */
    unsigned int total_trades_cancelled;   /* 总取消交易次数 */
    unsigned int total_trades_timeout;     /* 总超时交易次数 */
    unsigned int total_items_exchanged;    /* 总交换物品数 */
    unsigned int total_gold_exchanged;     /* 总交换金币数 */
} TradeManagerStats;

/*==============================================================================
 * Trade Manager API
 *============================================================================*/

/**
 * Initialize Trade Manager
 *
 * Sets up trade management system with user-confirmed parameters.
 *
 * Parameters:
 *   max_concurrent_trades: Maximum concurrent trades (✅ default: 1000)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_Initialize(int max_concurrent_trades);

/**
 * Cleanup Trade Manager
 *
 * Releases all resources and cancels active trades.
 */
void TradeManager_Cleanup(void);

/*==============================================================================
 * Trade Lifecycle Functions
 *============================================================================*/

/**
 * Request Trade
 *
 * Player initiates a trade request with another player.
 *
 * Validations:
 * - Both players must be online and available
 * - Initiator cannot already be in a trade
 * - Target cannot already be in a trade
 * - Initiator has not exceeded daily gold limit
 *
 * Parameters:
 *   initiator_id: Character ID of player initiating trade
 *   target_id: Character ID of target player
 *   out_trade: Optional output trade instance
 *
 * Returns: Trade ID on success, negative error code on failure
 */
int TradeManager_RequestTrade(unsigned int initiator_id,
                               unsigned int target_id,
                               TradeInstance* out_trade);

/**
 * Accept Trade
 *
 * Target player accepts a trade request.
 *
 * State transition: PENDING → ACCEPTED
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID accepting (must be target)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_AcceptTrade(int trade_id, unsigned int user_id);

/**
 * Decline Trade
 *
 * Target player declines a trade request.
 *
 * State transition: PENDING → CANCELLED
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID declining (must be target)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_DeclineTrade(int trade_id, unsigned int user_id);

/**
 * Cancel Trade
 *
 * Either player cancels an active trade.
 *
 * State transition: ANY → CANCELLED
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID cancelling
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_CancelTrade(int trade_id, unsigned int user_id);

/*==============================================================================
 * Trade Offer Functions
 *============================================================================*/

/**
 * Add Item to Trade
 *
 * Player adds an item to their side of the trade.
 * Un-confirms both sides if previously confirmed.
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID adding item
 *   item_id: Item ID to add
 *   quantity: Item quantity
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_AddItem(int trade_id,
                          unsigned int user_id,
                          unsigned int item_id,
                          int quantity);

/**
 * Remove Item from Trade
 *
 * Player removes an item from their side.
 * Un-confirms both sides if previously confirmed.
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID removing item
 *   slot_index: Item slot index (0-9)
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_RemoveItem(int trade_id,
                             unsigned int user_id,
                             int slot_index);

/**
 * Set Gold Amount
 *
 * Player sets gold amount to trade.
 * Un-confirms both sides if previously confirmed.
 *
 * Validation:
 * - Player must have sufficient gold
 * - Gold amount + today's traded gold <= daily limit (200M)
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID
 *   gold_amount: Gold amount to offer
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_SetGold(int trade_id,
                          unsigned int user_id,
                          unsigned int gold_amount);

/**
 * Confirm Trade
 *
 * Player confirms their side of the trade (locks their offer).
 *
 * If both sides confirmed: state → LOCKED
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID confirming
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_ConfirmTrade(int trade_id, unsigned int user_id);

/**
 * Unconfirm Trade
 *
 * Player un-confirms their side (unlocks to make changes).
 *
 * If trade was LOCKED: state → ACCEPTED
 *
 * Parameters:
 *   trade_id: Trade ID
 *   user_id: Character ID un-confirming
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_UnconfirmTrade(int trade_id, unsigned int user_id);

/**
 * Complete Trade
 *
 * Executes the trade and exchanges items/gold.
 *
 * Requirements:
 * - Trade must be in LOCKED state
 * - Both players must have confirmed
 * - Final validation of items and gold
 *
 * Actions:
 * - Transfer items between players
 * - Transfer gold between players
 * - Update trade statistics
 * - Log to trade_gold_daily table
 * - Update charac_trade_limit_info
 * - State → COMPLETED
 *
 * Parameters:
 *   trade_id: Trade ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_CompleteTrade(int trade_id);

/*==============================================================================
 * Query Functions
 *============================================================================*/

/**
 * Get Trade Info
 *
 * Retrieves trade information.
 *
 * Parameters:
 *   trade_id: Trade ID
 *   out_trade: Output trade instance
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_GetTrade(int trade_id, TradeInstance* out_trade);

/**
 * Get Active Trade for Player
 *
 * Gets the active trade a player is participating in.
 *
 * Parameters:
 *   user_id: Character ID
 *   out_trade: Output trade instance
 *
 * Returns: Trade ID if player is in trade, TRADE_INVALID_ID otherwise
 */
int TradeManager_GetActiveTrade(unsigned int user_id, TradeInstance* out_trade);

/**
 * Check if Player is in Trade
 *
 * Parameters:
 *   user_id: Character ID
 *
 * Returns: 1 if in trade, 0 otherwise
 */
int TradeManager_IsPlayerInTrade(unsigned int user_id);

/**
 * Get Player Trade Statistics
 *
 * Retrieves trade statistics for a player.
 *
 * Parameters:
 *   user_id: Character ID
 *   out_stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_GetPlayerStats(unsigned int user_id, PlayerTradeStats* out_stats);

/**
 * Get Trade History
 *
 * Retrieves player's trade history.
 *
 * Parameters:
 *   user_id: Character ID
 *   out_history: Output array of history entries
 *   max_count: Maximum entries to retrieve
 *
 * Returns: Number of history entries retrieved
 */
int TradeManager_GetTradeHistory(unsigned int user_id,
                                  TradeHistoryEntry* out_history,
                                  int max_count);

/**
 * Get Global Statistics
 *
 * Retrieves global trade statistics.
 *
 * Parameters:
 *   out_stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int TradeManager_GetStatistics(TradeManagerStats* out_stats);

/*==============================================================================
 * System Functions
 *============================================================================*/

/**
 * Update Trade Manager
 *
 * Processes trade timeouts and cleanup.
 * Should be called periodically (e.g., every second).
 *
 * Actions:
 * - Check for timeout trades (> 300 seconds)
 * - Cancel timeout trades
 * - Reset daily limits at midnight
 *
 * Parameters:
 *   current_time: Current server time
 *
 * Returns: Number of trades processed
 */
int TradeManager_Update(time_t current_time);

/**
 * Validate Trade Limits
 *
 * Checks if player has exceeded daily trade limits.
 *
 * Parameters:
 *   user_id: Character ID
 *   additional_gold: Additional gold to trade
 *
 * Returns: ERR_SUCCESS if within limits, error code otherwise
 */
int TradeManager_ValidateLimits(unsigned int user_id, unsigned int additional_gold);

#endif // INCLUDE_TRADE_TRADE_MANAGER_H_
