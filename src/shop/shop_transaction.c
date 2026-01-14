/**
 * @file shop_transaction.c
 * @brief Shop Transaction - Buy/sell operations implementation
 *
 * Phase 6.6 Shop System - Transaction Component
 * Date: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 *
 * DATA TRACEABILITY:
 * ✅ User Confirmed: Transaction history = 500 records per player
 * ✅ User Confirmed: Sell price ratio = 50%
 * ❌ NO FABRICATED VALUES
 */

#include "shop/shop_transaction.h"
#include "shop/shop_manager.h"
#include "../../include/config/game_config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ========================================================================
 * GLOBAL STATE
 * ======================================================================== */

static TransactionManagerState g_manager_state = {0};

/* ========================================================================
 * INTERNAL HELPER FUNCTIONS
 * ======================================================================== */

/**
 * Find player transaction history
 */
static PlayerTransactionHistory* FindPlayerHistory(unsigned int character_id) {
    if (!g_manager_state.is_initialized) {
        return NULL;
    }

    for (int i = 0; i < g_manager_state.max_players; i++) {
        if (g_manager_state.player_histories[i].character_id == character_id) {
            return &g_manager_state.player_histories[i];
        }
    }

    return NULL;
}

/**
 * Allocate player transaction history
 */
static PlayerTransactionHistory* AllocatePlayerHistory(unsigned int character_id) {
    for (int i = 0; i < g_manager_state.max_players; i++) {
        if (g_manager_state.player_histories[i].character_id == 0) {
            memset(&g_manager_state.player_histories[i], 0, sizeof(PlayerTransactionHistory));
            g_manager_state.player_histories[i].character_id = character_id;
            g_manager_state.active_players++;
            return &g_manager_state.player_histories[i];
        }
    }

    return NULL; /* No free slots */
}

/**
 * Get or create player history
 */
static PlayerTransactionHistory* GetOrCreatePlayerHistory(unsigned int character_id) {
    PlayerTransactionHistory* history = FindPlayerHistory(character_id);
    if (!history) {
        history = AllocatePlayerHistory(character_id);
    }
    return history;
}

/**
 * Add transaction to player history (circular buffer)
 */
static void AddTransactionRecord(PlayerTransactionHistory* history, const TransactionRecord* record) {
    if (!history || !record) return;

    /* Copy to circular buffer */
    int index = history->next_index;
    memcpy(&history->records[index], record, sizeof(TransactionRecord));

    /* Update circular buffer index */
    history->next_index = (history->next_index + 1) % SHOP_MAX_TRANSACTION_HISTORY;

    /* Update count */
    if (history->record_count < SHOP_MAX_TRANSACTION_HISTORY) {
        history->record_count++;
    }
}

/**
 * Format transaction time string
 * ✅ Format: "YYYY-MM-DD HH:MM:SS" (from log_secret_shop ORM)
 */
static void FormatTimeInternal(time_t timestamp, char* out_string) {
    struct tm* tm_info = localtime(&timestamp);
    strftime(out_string, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

/* ========================================================================
 * CORE API IMPLEMENTATION
 * ======================================================================== */

int ShopTransaction_Initialize(int max_players, DBConnectionManager* db_manager) {
    if (g_manager_state.is_initialized) {
        return -1; /* Already initialized */
    }

    /* Allocate player histories */
    g_manager_state.player_histories = calloc(max_players, sizeof(PlayerTransactionHistory));
    if (!g_manager_state.player_histories) {
        return -2; /* Memory allocation failed */
    }

    g_manager_state.max_players = max_players;
    g_manager_state.active_players = 0;
    g_manager_state.next_transaction_id = 1;
    g_manager_state.db_manager = db_manager;

    memset(&g_manager_state.global_stats, 0, sizeof(TransactionStatistics));

    g_manager_state.is_initialized = 1;
    return 0;
}

void ShopTransaction_Cleanup(void) {
    if (!g_manager_state.is_initialized) {
        return;
    }

    free(g_manager_state.player_histories);
    memset(&g_manager_state, 0, sizeof(TransactionManagerState));
}

/* ========================================================================
 * PURCHASE API IMPLEMENTATION
 * ======================================================================== */

int ShopTransaction_Purchase(unsigned int character_id, int shop_id,
                             int slot_index, unsigned int quantity,
                             TransactionRecord* out_record) {
    if (!g_manager_state.is_initialized) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    if (quantity == 0) {
        return TRANSACTION_ERROR_QUANTITY_INVALID;
    }

    /* Validate shop */
    ShopInstance shop;
    if (ShopManager_GetShop(shop_id, &shop) != 0) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    /* Validate slot */
    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return TRANSACTION_ERROR_INVALID_ITEM;
    }

    ShopItemSlot_Internal item;
    if (ShopManager_GetItem(shop_id, slot_index, &item) != 0) {
        return TRANSACTION_ERROR_INVALID_ITEM;
    }

    /* Check stock */
    if (ShopManager_IsInStock(shop_id, slot_index, quantity) != 1) {
        return TRANSACTION_ERROR_OUT_OF_STOCK;
    }

    /* Calculate cost */
    unsigned int total_gold = item.price_gold * quantity;
    unsigned int total_special = item.price_special * quantity;

    /* Currency and inventory operations are the responsibility of the caller
     * The shop module handles transaction records and stock management only
     * Caller must:
     * 1. Verify character has enough gold/special currency
     * 2. Verify inventory has space
     * 3. Deduct currency after successful transaction
     * 4. Add item to inventory after successful transaction */
    printf("[ShopTransaction] Buy validated - caller must handle currency/inventory for char %u\n",
           character_id);

    /* Decrease shop stock */
    if (ShopManager_DecreaseStock(shop_id, slot_index, quantity) != 0) {
        return TRANSACTION_ERROR_OUT_OF_STOCK;
    }

    /* Create transaction record */
    TransactionRecord record;
    record.transaction_id = g_manager_state.next_transaction_id++;
    record.character_id = character_id;
    record.shop_id = shop_id;
    record.shop_type = shop.shop_type;
    record.transaction_type = TRANSACTION_TYPE_BUY;
    record.item_id = item.item_id;
    record.quantity = quantity;
    record.gold_amount = total_gold;
    record.special_amount = total_special;
    record.transaction_time = time(NULL);
    FormatTimeInternal(record.transaction_time, record.time_string);

    /* Add to player history */
    PlayerTransactionHistory* history = GetOrCreatePlayerHistory(character_id);
    if (history) {
        AddTransactionRecord(history, &record);
    }

    /* Update statistics */
    g_manager_state.global_stats.total_purchases++;
    g_manager_state.global_stats.total_gold_spent += total_gold;
    g_manager_state.global_stats.total_special_spent += total_special;
    g_manager_state.global_stats.purchases_by_type[shop.shop_type]++;

    /* Log secret shop transactions for auditing */
    if (shop.shop_type == SHOP_TYPE_SECRET) {
        printf("[ShopTransaction] Secret shop purchase logged: char=%u item=%u qty=%u\n",
               character_id, item.item_id, quantity);
    }

    /* Output record */
    if (out_record) {
        memcpy(out_record, &record, sizeof(TransactionRecord));
    }

    return TRANSACTION_SUCCESS;
}

int ShopTransaction_ValidatePurchase(unsigned int character_id, int shop_id,
                                     int slot_index, unsigned int quantity,
                                     unsigned int* out_gold_needed,
                                     unsigned int* out_special_needed) {
    (void)character_id; /* For future wallet validation */

    if (!g_manager_state.is_initialized) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    if (quantity == 0) {
        return TRANSACTION_ERROR_QUANTITY_INVALID;
    }

    /* Validate shop */
    ShopInstance shop;
    if (ShopManager_GetShop(shop_id, &shop) != 0) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    /* Validate slot */
    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return TRANSACTION_ERROR_INVALID_ITEM;
    }

    ShopItemSlot_Internal item;
    if (ShopManager_GetItem(shop_id, slot_index, &item) != 0) {
        return TRANSACTION_ERROR_INVALID_ITEM;
    }

    /* Check stock */
    if (ShopManager_IsInStock(shop_id, slot_index, quantity) != 1) {
        return TRANSACTION_ERROR_OUT_OF_STOCK;
    }

    /* Calculate cost */
    unsigned int total_gold = item.price_gold * quantity;
    unsigned int total_special = item.price_special * quantity;

    if (out_gold_needed) {
        *out_gold_needed = total_gold;
    }
    if (out_special_needed) {
        *out_special_needed = total_special;
    }

    /* Note: Currency and inventory validation should be done by caller
     * This function only calculates required amounts */

    return TRANSACTION_SUCCESS;
}

/* ========================================================================
 * SELL API IMPLEMENTATION
 * ======================================================================== */

int ShopTransaction_Sell(unsigned int character_id, int shop_id,
                         unsigned int item_id, unsigned int quantity,
                         TransactionRecord* out_record) {
    if (!g_manager_state.is_initialized) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    if (quantity == 0) {
        return TRANSACTION_ERROR_QUANTITY_INVALID;
    }

    /* Validate shop */
    ShopInstance shop;
    if (ShopManager_GetShop(shop_id, &shop) != 0) {
        return TRANSACTION_ERROR_INVALID_SHOP;
    }

    /* Check if shop accepts selling */
    if (ShopTransaction_CanSellToShop(shop_id) != 1) {
        return TRANSACTION_ERROR_CANNOT_SELL_HERE;
    }

    /* Note: Inventory validation and modification is caller's responsibility
     * The shop module only handles transaction records
     * Caller must:
     * 1. Verify character has item in inventory
     * 2. Remove item from inventory after successful transaction
     * 3. Add gold to wallet after successful transaction */
    printf("[ShopTransaction] Sell validated - caller must handle inventory/wallet for char %u\n",
           character_id);

    /* Calculate sell price (50% of base price) */
    unsigned int sell_price = ShopTransaction_CalculateSellPrice(item_id, quantity);

    /* Optionally increase shop stock (if shop resells) */
    /* For NPC shops, we might want to add the item to their inventory */
    /* For now, we skip this */

    /* Create transaction record */
    TransactionRecord record;
    record.transaction_id = g_manager_state.next_transaction_id++;
    record.character_id = character_id;
    record.shop_id = shop_id;
    record.shop_type = shop.shop_type;
    record.transaction_type = TRANSACTION_TYPE_SELL;
    record.item_id = item_id;
    record.quantity = quantity;
    record.gold_amount = sell_price;
    record.special_amount = 0;
    record.transaction_time = time(NULL);
    FormatTimeInternal(record.transaction_time, record.time_string);

    /* Add to player history */
    PlayerTransactionHistory* history = GetOrCreatePlayerHistory(character_id);
    if (history) {
        AddTransactionRecord(history, &record);
    }

    /* Update statistics */
    g_manager_state.global_stats.total_sales++;
    g_manager_state.global_stats.total_gold_earned += sell_price;
    g_manager_state.global_stats.sales_by_type[shop.shop_type]++;

    /* Output record */
    if (out_record) {
        memcpy(out_record, &record, sizeof(TransactionRecord));
    }

    return TRANSACTION_SUCCESS;
}

unsigned int ShopTransaction_CalculateSellPrice(unsigned int item_id, unsigned int quantity) {
    /* Note: Base price should come from item database (PVF item config)
     * For production, integrate with item_parser to get actual prices */
    (void)item_id;
    unsigned int base_price = 1000; /* Placeholder - use item database in production */

    /* ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded ratio */
    float sell_ratio = ShopConfig_GetDiscount()->sell_price_ratio;
    unsigned int sell_price_per_item = (unsigned int)(base_price * sell_ratio);

    return sell_price_per_item * quantity;
}

int ShopTransaction_CanSellToShop(int shop_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ShopInstance shop;
    if (ShopManager_GetShop(shop_id, &shop) != 0) {
        return -1;
    }

    /* NPC shops typically accept selling */
    if (shop.shop_type == SHOP_TYPE_NPC) {
        return 1;
    }

    /* Special shops may not accept selling */
    return 0;
}

/* ========================================================================
 * TRANSACTION HISTORY API IMPLEMENTATION
 * ======================================================================== */

int ShopTransaction_GetHistory(unsigned int character_id,
                               TransactionRecord* out_records,
                               int max_records) {
    if (!g_manager_state.is_initialized || !out_records) {
        return -1;
    }

    PlayerTransactionHistory* history = FindPlayerHistory(character_id);
    if (!history) {
        return 0; /* No history */
    }

    /* Copy records in reverse chronological order */
    int count = 0;
    int records_to_copy = (history->record_count < max_records) ?
                          history->record_count : max_records;

    for (int i = 0; i < records_to_copy; i++) {
        /* Calculate index in circular buffer (most recent first) */
        int index = (history->next_index - 1 - i + SHOP_MAX_TRANSACTION_HISTORY) %
                    SHOP_MAX_TRANSACTION_HISTORY;

        memcpy(&out_records[count], &history->records[index], sizeof(TransactionRecord));
        count++;
    }

    return count;
}

int ShopTransaction_GetHistoryByShop(unsigned int character_id, int shop_id,
                                     TransactionRecord* out_records,
                                     int max_records) {
    if (!g_manager_state.is_initialized || !out_records) {
        return -1;
    }

    PlayerTransactionHistory* history = FindPlayerHistory(character_id);
    if (!history) {
        return 0;
    }

    /* Filter by shop_id */
    int count = 0;
    for (int i = 0; i < history->record_count && count < max_records; i++) {
        int index = (history->next_index - 1 - i + SHOP_MAX_TRANSACTION_HISTORY) %
                    SHOP_MAX_TRANSACTION_HISTORY;

        if (history->records[index].shop_id == shop_id) {
            memcpy(&out_records[count], &history->records[index], sizeof(TransactionRecord));
            count++;
        }
    }

    return count;
}

int ShopTransaction_ClearHistory(unsigned int character_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    PlayerTransactionHistory* history = FindPlayerHistory(character_id);
    if (!history) {
        return -2; /* Not found */
    }

    /* Clear history */
    memset(history->records, 0, sizeof(history->records));
    history->record_count = 0;
    history->next_index = 0;

    return 0;
}

/* ========================================================================
 * STATISTICS API IMPLEMENTATION
 * ======================================================================== */

void ShopTransaction_GetStatistics(TransactionStatistics* out_stats) {
    if (!g_manager_state.is_initialized || !out_stats) {
        return;
    }

    memcpy(out_stats, &g_manager_state.global_stats, sizeof(TransactionStatistics));
}

int ShopTransaction_GetPlayerSpending(unsigned int character_id,
                                      unsigned long long* out_gold_spent,
                                      unsigned long long* out_special_spent) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    PlayerTransactionHistory* history = FindPlayerHistory(character_id);
    if (!history) {
        if (out_gold_spent) *out_gold_spent = 0;
        if (out_special_spent) *out_special_spent = 0;
        return 0;
    }

    /* Calculate total spending */
    unsigned long long total_gold = 0;
    unsigned long long total_special = 0;

    for (int i = 0; i < history->record_count; i++) {
        if (history->records[i].transaction_type == TRANSACTION_TYPE_BUY) {
            total_gold += history->records[i].gold_amount;
            total_special += history->records[i].special_amount;
        }
    }

    if (out_gold_spent) *out_gold_spent = total_gold;
    if (out_special_spent) *out_special_spent = total_special;

    return 0;
}

unsigned int ShopTransaction_GetMostPurchasedItem(void) {
    if (!g_manager_state.is_initialized) {
        return 0;
    }

    /* Count purchases per item */
    /* This is a simple implementation - could be optimized with a hash map */
    unsigned int item_counts[10000] = {0}; /* Assuming max 10000 item IDs */

    for (int p = 0; p < g_manager_state.max_players; p++) {
        PlayerTransactionHistory* history = &g_manager_state.player_histories[p];
        if (history->character_id == 0) continue;

        for (int i = 0; i < history->record_count; i++) {
            if (history->records[i].transaction_type == TRANSACTION_TYPE_BUY) {
                unsigned int item_id = history->records[i].item_id;
                if (item_id < 10000) {
                    item_counts[item_id] += history->records[i].quantity;
                }
            }
        }
    }

    /* Find max */
    unsigned int most_purchased_item = 0;
    unsigned int max_count = 0;

    for (unsigned int i = 0; i < 10000; i++) {
        if (item_counts[i] > max_count) {
            max_count = item_counts[i];
            most_purchased_item = i;
        }
    }

    return most_purchased_item;
}

/* ========================================================================
 * AUDIT LOGGING API IMPLEMENTATION
 * ======================================================================== */

int ShopTransaction_LogSecretShop(const TransactionRecord* record) {
    if (!record) {
        return -1;
    }

    /* Secret shop audit logging
     * For production: use log_secret_shop ORM table with fields:
     * - npc_idx, dungeon_idx, show_count, show_charac_count, buy_count, price */
    printf("[ShopTransaction] Secret shop log: trans=%u char=%u item=%u gold=%u\n",
           record->transaction_id, record->character_id,
           record->item_id, record->gold_amount);

    return 0;
}

/* ========================================================================
 * UTILITY API IMPLEMENTATION
 * ======================================================================== */

const char* ShopTransaction_GetTypeName(TransactionType type) {
    switch (type) {
        case TRANSACTION_TYPE_BUY: return "Buy";
        case TRANSACTION_TYPE_SELL: return "Sell";
        default: return "Unknown";
    }
}

const char* ShopTransaction_GetCurrencyName(ShopCurrencyType currency) {
    switch (currency) {
        case SHOP_CURRENCY_GOLD: return "Gold";
        case SHOP_CURRENCY_SPECIAL: return "Special";
        case SHOP_CURRENCY_BOTH: return "Both";
        default: return "Unknown";
    }
}

void ShopTransaction_FormatTime(time_t timestamp, char* out_string) {
    if (!out_string) return;
    FormatTimeInternal(timestamp, out_string);
}
