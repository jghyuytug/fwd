/**
 * @file trade_manager.c
 * @brief Trade Manager Implementation
 *
 * Implements player-to-player trading system with validation and limits.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/trade/trade_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*==============================================================================
 * Internal State Management
 *============================================================================*/

/**
 * Trade Manager internal state
 */
typedef struct {
    /* Configuration */
    int max_concurrent_trades;       /* ✅ 最大并发交易数：1000 */
    int is_initialized;

    /* Trade storage */
    TradeInstance* trade_instances;  /* 交易实例数组 */
    int trade_count;                 /* 当前活跃交易数 */
    int next_trade_id;               /* 下一个交易ID */

    /* Player trade mapping (character_id → trade_id) */
    int* player_trade_map;           /* 玩家到交易的映射 */
    int player_map_capacity;         /* 映射容量 */

    /* Player statistics */
    PlayerTradeStats* player_stats;  /* 玩家交易统计 */
    int player_stats_capacity;       /* 统计容量 */
    int player_stats_count;          /* 统计数量 */

    /* Global statistics */
    TradeManagerStats global_stats;
} TradeManager_State;

/* Global state instance */
static TradeManager_State g_manager_state = {0};

/*==============================================================================
 * Internal Helper Functions
 *============================================================================*/

/**
 * Find trade by ID
 */
static TradeInstance* FindTradeByID(int trade_id)
{
    for (int i = 0; i < g_manager_state.trade_count; i++) {
        if (g_manager_state.trade_instances[i].trade_id == trade_id &&
            g_manager_state.trade_instances[i].is_active) {
            return &g_manager_state.trade_instances[i];
        }
    }
    return NULL;
}

/**
 * Find active trade for player
 */
static TradeInstance* FindPlayerActiveTrade(unsigned int player_id)
{
    if (player_id >= (unsigned int)g_manager_state.player_map_capacity) {
        return NULL;
    }

    int trade_id = g_manager_state.player_trade_map[player_id];
    if (trade_id == TRADE_INVALID_ID) {
        return NULL;
    }

    return FindTradeByID(trade_id);
}

/**
 * Set player trade mapping
 */
static void SetPlayerTradeMapping(unsigned int player_id, int trade_id)
{
    if (player_id >= (unsigned int)g_manager_state.player_map_capacity) {
        /* Expand mapping array if needed */
        int new_capacity = player_id + 1000;
        int* new_map = (int*)realloc(g_manager_state.player_trade_map,
                                      new_capacity * sizeof(int));
        if (!new_map) {
            fprintf(stderr, "[TradeManager] Failed to expand player map\n");
            return;
        }

        /* Initialize new slots */
        for (int i = g_manager_state.player_map_capacity; i < new_capacity; i++) {
            new_map[i] = TRADE_INVALID_ID;
        }

        g_manager_state.player_trade_map = new_map;
        g_manager_state.player_map_capacity = new_capacity;
    }

    g_manager_state.player_trade_map[player_id] = trade_id;
}

/**
 * Get or create player statistics
 */
static PlayerTradeStats* GetOrCreatePlayerStats(unsigned int player_id)
{
    /* Search for existing stats */
    for (int i = 0; i < g_manager_state.player_stats_count; i++) {
        if (g_manager_state.player_stats[i].character_id == player_id) {
            return &g_manager_state.player_stats[i];
        }
    }

    /* Check capacity */
    if (g_manager_state.player_stats_count >= g_manager_state.player_stats_capacity) {
        fprintf(stderr, "[TradeManager] Player stats capacity exceeded\n");
        return NULL;
    }

    /* Create new stats */
    PlayerTradeStats* stats = &g_manager_state.player_stats[g_manager_state.player_stats_count++];
    memset(stats, 0, sizeof(PlayerTradeStats));
    stats->character_id = player_id;
    stats->daily_reset_time = time(NULL) + 86400; /* Tomorrow */

    return stats;
}

/**
 * Allocate new trade slot
 */
static TradeInstance* AllocateTrade(void)
{
    if (g_manager_state.trade_count >= g_manager_state.max_concurrent_trades) {
        fprintf(stderr, "[TradeManager] Max concurrent trades reached (%d)\n",
                g_manager_state.max_concurrent_trades);
        return NULL;
    }

    TradeInstance* trade = &g_manager_state.trade_instances[g_manager_state.trade_count++];
    memset(trade, 0, sizeof(TradeInstance));
    trade->trade_id = g_manager_state.next_trade_id++;
    trade->is_active = 1;
    trade->state = TRADE_STATE_PENDING;

    return trade;
}

/**
 * Free trade slot
 */
static void FreeTrade(TradeInstance* trade)
{
    if (!trade) {
        return;
    }

    /* Clear player mappings */
    SetPlayerTradeMapping(trade->initiator_id, TRADE_INVALID_ID);
    SetPlayerTradeMapping(trade->target_id, TRADE_INVALID_ID);

    trade->is_active = 0;
    trade->state = TRADE_STATE_CANCELLED;
}

/**
 * Reset daily trade limits if needed
 */
static void ResetDailyLimitsIfNeeded(PlayerTradeStats* stats, time_t current_time)
{
    if (current_time >= stats->daily_reset_time) {
        stats->daily_gold_traded = 0;
        stats->daily_reset_time = current_time + 86400; /* Next day */
    }
}

/**
 * Check if player side of trade
 */
static int IsInitiator(const TradeInstance* trade, unsigned int player_id)
{
    return trade->initiator_id == player_id;
}

/**
 * Unconfirm both sides
 */
static void UnconfirmBothSides(TradeInstance* trade)
{
    trade->initiator_confirmed = 0;
    trade->target_confirmed = 0;
    if (trade->state == TRADE_STATE_LOCKED) {
        trade->state = TRADE_STATE_ACCEPTED;
    }
}

/*==============================================================================
 * Initialization and Cleanup
 *============================================================================*/

int TradeManager_Initialize(int max_concurrent_trades)
{
    if (g_manager_state.is_initialized) {
        fprintf(stderr, "[TradeManager] Already initialized\n");
        return ERR_ALREADY_INITIALIZED;
    }

    printf("[TradeManager] Initializing trade manager...\n");

    /* Validate parameters */
    if (max_concurrent_trades <= 0 || max_concurrent_trades > 10000) {
        fprintf(stderr, "[TradeManager] Invalid max_concurrent_trades: %d\n",
                max_concurrent_trades);
        return ERR_INVALID_PARAMETER;
    }

    /* Initialize state */
    memset(&g_manager_state, 0, sizeof(TradeManager_State));
    g_manager_state.max_concurrent_trades = max_concurrent_trades;

    /* Allocate trade instances */
    g_manager_state.trade_instances = (TradeInstance*)calloc(
        max_concurrent_trades, sizeof(TradeInstance));
    if (!g_manager_state.trade_instances) {
        fprintf(stderr, "[TradeManager] Failed to allocate trade instances\n");
        return ERR_OUT_OF_MEMORY;
    }

    /* Allocate player trade mapping (initial capacity: 10000 players) */
    g_manager_state.player_map_capacity = 10000;
    g_manager_state.player_trade_map = (int*)malloc(
        g_manager_state.player_map_capacity * sizeof(int));
    if (!g_manager_state.player_trade_map) {
        fprintf(stderr, "[TradeManager] Failed to allocate player map\n");
        free(g_manager_state.trade_instances);
        return ERR_OUT_OF_MEMORY;
    }

    /* Initialize player map */
    for (int i = 0; i < g_manager_state.player_map_capacity; i++) {
        g_manager_state.player_trade_map[i] = TRADE_INVALID_ID;
    }

    /* Allocate player statistics (initial capacity: 5000 players) */
    g_manager_state.player_stats_capacity = 5000;
    g_manager_state.player_stats = (PlayerTradeStats*)calloc(
        g_manager_state.player_stats_capacity, sizeof(PlayerTradeStats));
    if (!g_manager_state.player_stats) {
        fprintf(stderr, "[TradeManager] Failed to allocate player stats\n");
        free(g_manager_state.trade_instances);
        free(g_manager_state.player_trade_map);
        return ERR_OUT_OF_MEMORY;
    }

    g_manager_state.next_trade_id = 1;
    g_manager_state.is_initialized = 1;

    printf("[TradeManager] Initialized successfully\n");
    printf("[TradeManager] Max concurrent trades: %d\n", max_concurrent_trades);
    printf("[TradeManager] Trade timeout: %d seconds\n", TRADE_TIMEOUT_SECONDS);
    printf("[TradeManager] Daily gold limit: %u\n", TRADE_DAILY_GOLD_LIMIT);

    return ERR_SUCCESS;
}

void TradeManager_Cleanup(void)
{
    if (!g_manager_state.is_initialized) {
        return;
    }

    printf("[TradeManager] Cleaning up trade manager...\n");

    /* Cancel all active trades */
    for (int i = 0; i < g_manager_state.trade_count; i++) {
        if (g_manager_state.trade_instances[i].is_active) {
            FreeTrade(&g_manager_state.trade_instances[i]);
        }
    }

    /* Free memory */
    free(g_manager_state.trade_instances);
    free(g_manager_state.player_trade_map);
    free(g_manager_state.player_stats);

    memset(&g_manager_state, 0, sizeof(TradeManager_State));

    printf("[TradeManager] Cleanup complete\n");
}

/*==============================================================================
 * Trade Lifecycle Functions
 *============================================================================*/

int TradeManager_RequestTrade(unsigned int initiator_id,
                               unsigned int target_id,
                               TradeInstance* out_trade)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Validate parameters */
    if (initiator_id == 0 || target_id == 0 || initiator_id == target_id) {
        return ERR_INVALID_PARAMETER;
    }

    /* Check if initiator is already in a trade */
    if (FindPlayerActiveTrade(initiator_id) != NULL) {
        return ERR_ALREADY_IN_TRADE;
    }

    /* Check if target is already in a trade */
    if (FindPlayerActiveTrade(target_id) != NULL) {
        return ERR_ALREADY_IN_TRADE;
    }

    /* Check daily limits for initiator */
    PlayerTradeStats* initiator_stats = GetOrCreatePlayerStats(initiator_id);
    if (!initiator_stats) {
        return ERR_OUT_OF_MEMORY;
    }

    ResetDailyLimitsIfNeeded(initiator_stats, time(NULL));

    /* Allocate new trade */
    TradeInstance* trade = AllocateTrade();
    if (!trade) {
        return ERR_LIMIT_REACHED;
    }

    /* Initialize trade */
    trade->initiator_id = initiator_id;
    trade->target_id = target_id;
    trade->state = TRADE_STATE_PENDING;
    trade->request_time = time(NULL);
    trade->timeout_time = trade->request_time + TRADE_TIMEOUT_SECONDS; /* ✅ 300 seconds */

    /* Set player mappings */
    SetPlayerTradeMapping(initiator_id, trade->trade_id);
    SetPlayerTradeMapping(target_id, trade->trade_id);

    /* Update statistics */
    g_manager_state.global_stats.total_trades_requested++;

    /* Output trade if requested */
    if (out_trade) {
        memcpy(out_trade, trade, sizeof(TradeInstance));
    }

    printf("[TradeManager] Trade #%d requested: %u → %u\n",
           trade->trade_id, initiator_id, target_id);

    return trade->trade_id;
}

int TradeManager_AcceptTrade(int trade_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Only target can accept */
    if (trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Must be in PENDING state */
    if (trade->state != TRADE_STATE_PENDING) {
        return ERR_INVALID_STATE;
    }

    /* Accept trade */
    trade->state = TRADE_STATE_ACCEPTED;
    trade->accept_time = time(NULL);

    printf("[TradeManager] Trade #%d accepted by %u\n", trade_id, user_id);

    return ERR_SUCCESS;
}

int TradeManager_DeclineTrade(int trade_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Only target can decline */
    if (trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Must be in PENDING state */
    if (trade->state != TRADE_STATE_PENDING) {
        return ERR_INVALID_STATE;
    }

    /* Decline trade */
    trade->state = TRADE_STATE_CANCELLED;
    FreeTrade(trade);

    g_manager_state.global_stats.total_trades_cancelled++;

    printf("[TradeManager] Trade #%d declined by %u\n", trade_id, user_id);

    return ERR_SUCCESS;
}

int TradeManager_CancelTrade(int trade_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Either player can cancel */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Cannot cancel if already completed */
    if (trade->state == TRADE_STATE_COMPLETED) {
        return ERR_INVALID_STATE;
    }

    /* Cancel trade */
    trade->state = TRADE_STATE_CANCELLED;
    FreeTrade(trade);

    g_manager_state.global_stats.total_trades_cancelled++;

    printf("[TradeManager] Trade #%d cancelled by %u\n", trade_id, user_id);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Trade Offer Functions
 *============================================================================*/

int TradeManager_AddItem(int trade_id,
                          unsigned int user_id,
                          unsigned int item_id,
                          int quantity)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Must be in ACCEPTED state */
    if (trade->state != TRADE_STATE_ACCEPTED) {
        return ERR_INVALID_STATE;
    }

    /* Validate user is participant */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Determine which side */
    int is_initiator = IsInitiator(trade, user_id);
    TradeItemSlot* items = is_initiator ? trade->initiator_items : trade->target_items;
    int* item_count = is_initiator ? &trade->initiator_item_count : &trade->target_item_count;

    /* Check if slot available */
    if (*item_count >= TRADE_MAX_ITEMS_PER_SIDE) {
        return ERR_LIMIT_REACHED;
    }

    /* Validate item and quantity */
    if (item_id == 0 || quantity <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    /*
     * Item ownership validation: Integration with inventory module
     *
     * Production code should verify:
     *   if (!Inventory_HasItem(player_id, item_id, quantity)) {
     *       return ERR_ITEM_NOT_FOUND;
     *   }
     *   if (!Inventory_IsItemTradeable(item_id)) {
     *       return ERR_ITEM_NOT_TRADEABLE;
     *   }
     */

    /* Add item */
    TradeItemSlot* slot = &items[*item_count];
    slot->item_id = item_id;
    slot->quantity = quantity;
    slot->is_confirmed = 0;

    (*item_count)++;

    /* Un-confirm both sides */
    UnconfirmBothSides(trade);

    printf("[TradeManager] Trade #%d: User %u added item %u x%d\n",
           trade_id, user_id, item_id, quantity);

    return ERR_SUCCESS;
}

int TradeManager_RemoveItem(int trade_id,
                             unsigned int user_id,
                             int slot_index)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Must be in ACCEPTED state */
    if (trade->state != TRADE_STATE_ACCEPTED) {
        return ERR_INVALID_STATE;
    }

    /* Validate user is participant */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Determine which side */
    int is_initiator = IsInitiator(trade, user_id);
    TradeItemSlot* items = is_initiator ? trade->initiator_items : trade->target_items;
    int* item_count = is_initiator ? &trade->initiator_item_count : &trade->target_item_count;

    /* Validate slot index */
    if (slot_index < 0 || slot_index >= *item_count) {
        return ERR_INVALID_PARAMETER;
    }

    /* Remove item by shifting */
    for (int i = slot_index; i < *item_count - 1; i++) {
        items[i] = items[i + 1];
    }

    (*item_count)--;

    /* Un-confirm both sides */
    UnconfirmBothSides(trade);

    printf("[TradeManager] Trade #%d: User %u removed item at slot %d\n",
           trade_id, user_id, slot_index);

    return ERR_SUCCESS;
}

int TradeManager_SetGold(int trade_id,
                          unsigned int user_id,
                          unsigned int gold_amount)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Must be in ACCEPTED state */
    if (trade->state != TRADE_STATE_ACCEPTED) {
        return ERR_INVALID_STATE;
    }

    /* Validate user is participant */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Check daily limit */
    PlayerTradeStats* stats = GetOrCreatePlayerStats(user_id);
    if (!stats) {
        return ERR_OUT_OF_MEMORY;
    }

    ResetDailyLimitsIfNeeded(stats, time(NULL));

    if (stats->daily_gold_traded + gold_amount > TRADE_DAILY_GOLD_LIMIT) {
        fprintf(stderr, "[TradeManager] User %u exceeded daily gold limit\n", user_id);
        return ERR_DAILY_LIMIT_EXCEEDED;
    }

    /*
     * Gold balance validation: Integration with wallet/user module
     *
     * Production code should verify:
     *   uint32_t current_gold = User_GetCurrency(user_id, CURRENCY_GOLD);
     *   if (current_gold < gold_amount) {
     *       return ERR_INSUFFICIENT_FUNDS;
     *   }
     */

    /* Set gold */
    if (IsInitiator(trade, user_id)) {
        trade->initiator_gold = gold_amount;
    } else {
        trade->target_gold = gold_amount;
    }

    /* Un-confirm both sides */
    UnconfirmBothSides(trade);

    printf("[TradeManager] Trade #%d: User %u set gold to %u\n",
           trade_id, user_id, gold_amount);

    return ERR_SUCCESS;
}

int TradeManager_ConfirmTrade(int trade_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Must be in ACCEPTED state */
    if (trade->state != TRADE_STATE_ACCEPTED) {
        return ERR_INVALID_STATE;
    }

    /* Validate user is participant */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Confirm user's side */
    if (IsInitiator(trade, user_id)) {
        trade->initiator_confirmed = 1;
    } else {
        trade->target_confirmed = 1;
    }

    printf("[TradeManager] Trade #%d: User %u confirmed\n", trade_id, user_id);

    /* If both sides confirmed, lock trade */
    if (trade->initiator_confirmed && trade->target_confirmed) {
        trade->state = TRADE_STATE_LOCKED;
        printf("[TradeManager] Trade #%d locked (both sides confirmed)\n", trade_id);
    }

    return ERR_SUCCESS;
}

int TradeManager_UnconfirmTrade(int trade_id, unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Can unconfirm in ACCEPTED or LOCKED state */
    if (trade->state != TRADE_STATE_ACCEPTED && trade->state != TRADE_STATE_LOCKED) {
        return ERR_INVALID_STATE;
    }

    /* Validate user is participant */
    if (trade->initiator_id != user_id && trade->target_id != user_id) {
        return ERR_NO_PERMISSION;
    }

    /* Unconfirm both sides */
    trade->initiator_confirmed = 0;
    trade->target_confirmed = 0;
    trade->state = TRADE_STATE_ACCEPTED;

    printf("[TradeManager] Trade #%d: User %u unconfirmed\n", trade_id, user_id);

    return ERR_SUCCESS;
}

int TradeManager_CompleteTrade(int trade_id)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    /* Must be in LOCKED state */
    if (trade->state != TRADE_STATE_LOCKED) {
        return ERR_INVALID_STATE;
    }

    /* Both sides must be confirmed */
    if (!trade->initiator_confirmed || !trade->target_confirmed) {
        return ERR_INVALID_STATE;
    }

    /*
     * Final trade execution: Integration with inventory/wallet/database modules
     *
     * Production implementation:
     * 1. Re-validate items and gold (atomic check):
     *    - Verify initiator still owns all offered items
     *    - Verify target still owns all offered items
     *    - Verify initiator has initiator_gold
     *    - Verify target has target_gold
     *
     * 2. Transfer items (use database transaction):
     *    for each initiator_item: Inventory_TransferItem(initiator_id, target_id, item)
     *    for each target_item: Inventory_TransferItem(target_id, initiator_id, item)
     *
     * 3. Transfer gold:
     *    User_RemoveCurrency(initiator_id, CURRENCY_GOLD, initiator_gold);
     *    User_AddCurrency(target_id, CURRENCY_GOLD, initiator_gold);
     *    User_RemoveCurrency(target_id, CURRENCY_GOLD, target_gold);
     *    User_AddCurrency(initiator_id, CURRENCY_GOLD, target_gold);
     *
     * 4. Database logging via ORM:
     *    TradeGoldDaily_Add() for audit trail
     *    CharacTradeLimitInfo_Update() for limit tracking
     *
     * 5. Commit transaction or rollback on failure
     */

    /* Update player statistics */
    PlayerTradeStats* initiator_stats = GetOrCreatePlayerStats(trade->initiator_id);
    PlayerTradeStats* target_stats = GetOrCreatePlayerStats(trade->target_id);

    if (initiator_stats) {
        initiator_stats->trade_count++;
        initiator_stats->total_trade_gold += trade->initiator_gold;
        initiator_stats->daily_gold_traded += trade->initiator_gold;
        snprintf(initiator_stats->last_trade_time, 20, "%ld", (long)time(NULL));
    }

    if (target_stats) {
        target_stats->trade_count++;
        target_stats->total_trade_gold += trade->target_gold;
        target_stats->daily_gold_traded += trade->target_gold;
        snprintf(target_stats->last_trade_time, 20, "%ld", (long)time(NULL));
    }

    /* Update global statistics */
    g_manager_state.global_stats.total_trades_completed++;
    g_manager_state.global_stats.total_items_exchanged +=
        trade->initiator_item_count + trade->target_item_count;
    g_manager_state.global_stats.total_gold_exchanged +=
        trade->initiator_gold + trade->target_gold;

    /* Complete trade */
    trade->state = TRADE_STATE_COMPLETED;
    trade->complete_time = time(NULL);

    printf("[TradeManager] Trade #%d completed successfully\n", trade_id);
    printf("[TradeManager]   Initiator %u: %d items, %u gold\n",
           trade->initiator_id, trade->initiator_item_count, trade->initiator_gold);
    printf("[TradeManager]   Target %u: %d items, %u gold\n",
           trade->target_id, trade->target_item_count, trade->target_gold);

    /* Free trade */
    FreeTrade(trade);

    return ERR_SUCCESS;
}

/*==============================================================================
 * Query Functions
 *============================================================================*/

int TradeManager_GetTrade(int trade_id, TradeInstance* out_trade)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_trade) {
        return ERR_INVALID_PARAMETER;
    }

    TradeInstance* trade = FindTradeByID(trade_id);
    if (!trade) {
        return ERR_NOT_FOUND;
    }

    memcpy(out_trade, trade, sizeof(TradeInstance));

    return ERR_SUCCESS;
}

int TradeManager_GetActiveTrade(unsigned int user_id, TradeInstance* out_trade)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    TradeInstance* trade = FindPlayerActiveTrade(user_id);
    if (!trade) {
        return TRADE_INVALID_ID;
    }

    if (out_trade) {
        memcpy(out_trade, trade, sizeof(TradeInstance));
    }

    return trade->trade_id;
}

int TradeManager_IsPlayerInTrade(unsigned int user_id)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    return FindPlayerActiveTrade(user_id) != NULL ? 1 : 0;
}

int TradeManager_GetPlayerStats(unsigned int user_id, PlayerTradeStats* out_stats)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_stats) {
        return ERR_INVALID_PARAMETER;
    }

    PlayerTradeStats* stats = GetOrCreatePlayerStats(user_id);
    if (!stats) {
        return ERR_OUT_OF_MEMORY;
    }

    memcpy(out_stats, stats, sizeof(PlayerTradeStats));

    return ERR_SUCCESS;
}

int TradeManager_GetTradeHistory(unsigned int user_id,
                                  TradeHistoryEntry* out_history,
                                  int max_count)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_history || max_count <= 0) {
        return 0;
    }

    PlayerTradeStats* stats = GetOrCreatePlayerStats(user_id);
    if (!stats) {
        return 0;
    }

    int count = stats->history_count < max_count ? stats->history_count : max_count;
    if (count > 0) {
        memcpy(out_history, stats->history, count * sizeof(TradeHistoryEntry));
    }

    return count;
}

int TradeManager_GetStatistics(TradeManagerStats* out_stats)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_stats) {
        return ERR_INVALID_PARAMETER;
    }

    memcpy(out_stats, &g_manager_state.global_stats, sizeof(TradeManagerStats));

    return ERR_SUCCESS;
}

/*==============================================================================
 * System Functions
 *============================================================================*/

int TradeManager_Update(time_t current_time)
{
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    int processed = 0;

    /* Check for timeout trades */
    for (int i = 0; i < g_manager_state.trade_count; i++) {
        TradeInstance* trade = &g_manager_state.trade_instances[i];

        if (!trade->is_active) {
            continue;
        }

        /* Check timeout (✅ 300 seconds) */
        if (current_time >= trade->timeout_time) {
            printf("[TradeManager] Trade #%d timed out\n", trade->trade_id);

            trade->state = TRADE_STATE_CANCELLED;
            FreeTrade(trade);

            g_manager_state.global_stats.total_trades_timeout++;
            processed++;
        }
    }

    return processed;
}

int TradeManager_ValidateLimits(unsigned int user_id, unsigned int additional_gold)
{
    if (!g_manager_state.is_initialized) {
        return ERR_NOT_INITIALIZED;
    }

    PlayerTradeStats* stats = GetOrCreatePlayerStats(user_id);
    if (!stats) {
        return ERR_OUT_OF_MEMORY;
    }

    ResetDailyLimitsIfNeeded(stats, time(NULL));

    /* Check daily gold limit (✅ 200M) */
    if (stats->daily_gold_traded + additional_gold > TRADE_DAILY_GOLD_LIMIT) {
        return ERR_DAILY_LIMIT_EXCEEDED;
    }

    return ERR_SUCCESS;
}
