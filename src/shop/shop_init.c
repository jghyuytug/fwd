/**
 * @file shop_init.c
 * @brief Shop System - Unified initialization and cleanup
 *
 * Phase 6.6 Shop System - Integration Module
 * Date: 2025-11-23
 *
 * RESPONSIBILITY:
 * - Initialize all 5 shop subsystems
 * - Cleanup all shop subsystems
 * - Provide unified Shop_Initialize() / Shop_Cleanup() API
 *
 * SUBSYSTEMS:
 * 1. Shop Manager (shop_manager.c)
 * 2. Quest Shop (quest_shop.c)
 * 3. Shop Transaction (shop_transaction.c)
 * 4. Shop Refresh (shop_refresh.c)
 * 5. Shop Restrictions (shop_restriction.c)
 */

#include "shop/shop_interface.h"
#include "shop/shop_manager.h"
#include "shop/quest_shop.h"
#include "shop/shop_transaction.h"
#include "shop/shop_refresh.h"
#include "shop/shop_restriction.h"
#include <stdio.h>

/* ========================================================================
 * GLOBAL STATE
 * ======================================================================== */

static unsigned char g_shop_initialized = 0;

/* ========================================================================
 * UNIFIED INITIALIZATION
 * ======================================================================== */

/**
 * Initialize entire Shop System
 *
 * @param max_shops Maximum shops (use SHOP_MAX_SHOPS = 500)
 * @param max_characters Maximum characters for quest shop cache
 * @param max_transaction_players Maximum players for transaction history
 * @param max_restriction_players Maximum players for restriction tracking
 * @param max_restriction_items Maximum items with restrictions
 * @param db_manager Database connection manager (can be NULL)
 * @return 0 on success, negative error code on failure
 *
 * Example:
 *   int result = Shop_Initialize(500, 10000, 10000, 10000, 1000, &db_manager);
 */
int Shop_Initialize(int max_shops, int max_characters,
                   int max_transaction_players, int max_restriction_players,
                   int max_restriction_items, DBConnectionManager* db_manager) {
    if (g_shop_initialized) {
        printf("[Shop] Already initialized\n");
        return -1;
    }

    int result;

    /* Initialize Shop Manager */
    printf("[Shop] Initializing Shop Manager (max_shops=%d)...\n", max_shops);
    result = ShopManager_Initialize(max_shops, db_manager);
    if (result != 0) {
        printf("[Shop] ERROR: Shop Manager initialization failed (code=%d)\n", result);
        return -2;
    }

    /* Initialize Quest Shop */
    printf("[Shop] Initializing Quest Shop (max_characters=%d)...\n", max_characters);
    result = QuestShop_Initialize(max_characters, db_manager);
    if (result != 0) {
        printf("[Shop] ERROR: Quest Shop initialization failed (code=%d)\n", result);
        ShopManager_Cleanup();
        return -3;
    }

    /* Initialize Shop Transaction */
    printf("[Shop] Initializing Shop Transaction (max_players=%d)...\n", max_transaction_players);
    result = ShopTransaction_Initialize(max_transaction_players, db_manager);
    if (result != 0) {
        printf("[Shop] ERROR: Shop Transaction initialization failed (code=%d)\n", result);
        QuestShop_Cleanup();
        ShopManager_Cleanup();
        return -4;
    }

    /* Initialize Shop Refresh */
    printf("[Shop] Initializing Shop Refresh (max_shops=%d)...\n", max_shops);
    result = ShopRefresh_Initialize(max_shops);
    if (result != 0) {
        printf("[Shop] ERROR: Shop Refresh initialization failed (code=%d)\n", result);
        ShopTransaction_Cleanup();
        QuestShop_Cleanup();
        ShopManager_Cleanup();
        return -5;
    }

    /* Initialize Shop Restrictions */
    printf("[Shop] Initializing Shop Restrictions (max_players=%d, max_items=%d)...\n",
           max_restriction_players, max_restriction_items);
    result = ShopRestriction_Initialize(max_restriction_players, max_restriction_items, db_manager);
    if (result != 0) {
        printf("[Shop] ERROR: Shop Restrictions initialization failed (code=%d)\n", result);
        ShopRefresh_Cleanup();
        ShopTransaction_Cleanup();
        QuestShop_Cleanup();
        ShopManager_Cleanup();
        return -6;
    }

    g_shop_initialized = 1;
    printf("[Shop] ✅ All subsystems initialized successfully!\n");

    /* Print configuration */
    printf("[Shop] Configuration:\n");
    printf("  - Max Shops: %d\n", max_shops);
    printf("  - Secret Refresh Interval: %d seconds (2 hours)\n", SHOP_SECRET_REFRESH_INTERVAL);
    printf("  - Sell Price Ratio: %.0f%%\n", SHOP_SELL_PRICE_RATIO * 100);
    printf("  - QP per Quest: %d\n", QUEST_SHOP_QP_PER_QUEST);
    printf("  - QP Prices: HP/MP(%d), Attack/Defense(%d), Critical(%d), Others(%d)\n",
           QUEST_SHOP_PRICE_HP_MP, QUEST_SHOP_PRICE_ATTACK_DEFENSE,
           QUEST_SHOP_PRICE_CRITICAL, QUEST_SHOP_PRICE_STANDARD);
    printf("  - Transaction History: %d records/player\n", SHOP_MAX_TRANSACTION_HISTORY);

    return 0;
}

/**
 * Cleanup entire Shop System
 */
void Shop_Cleanup(void) {
    if (!g_shop_initialized) {
        return;
    }

    printf("[Shop] Cleaning up all subsystems...\n");

    /* Cleanup in reverse order */
    ShopRestriction_Cleanup();
    ShopRefresh_Cleanup();
    ShopTransaction_Cleanup();
    QuestShop_Cleanup();
    ShopManager_Cleanup();

    g_shop_initialized = 0;
    printf("[Shop] ✅ All subsystems cleaned up\n");
}

/**
 * Check if Shop System is initialized
 *
 * @return 1 if initialized, 0 if not
 */
int Shop_IsInitialized(void) {
    return g_shop_initialized;
}

/* ========================================================================
 * UNIFIED UPDATE (for main game loop)
 * ======================================================================== */

/**
 * Update Shop System (call every frame/tick)
 *
 * This handles:
 * - Secret shop auto-refresh (every 2 hours)
 * - Restriction period expiration
 *
 * @param current_time Current server time
 * @return 0 on success
 */
int Shop_Update(time_t current_time) {
    if (!g_shop_initialized) {
        return -1;
    }

    /* Update shop refresh (secret shops) */
    int refreshed_shops = ShopRefresh_Update(current_time);
    if (refreshed_shops > 0) {
        printf("[Shop] Auto-refreshed %d shops\n", refreshed_shops);
    }

    /* Update restrictions (check for expired periods) */
    int updated_restrictions = ShopRestriction_UpdateAll(current_time);
    if (updated_restrictions > 0) {
        printf("[Shop] Updated %d expired restrictions\n", updated_restrictions);
    }

    return 0;
}

/* ========================================================================
 * UNIFIED STATISTICS
 * ======================================================================== */

/**
 * Print Shop System statistics (for debugging/monitoring)
 */
void Shop_PrintStatistics(void) {
    if (!g_shop_initialized) {
        printf("[Shop] Not initialized\n");
        return;
    }

    printf("\n========== Shop System Statistics ==========\n");

    /* Shop Manager Stats */
    ShopStatistics shop_stats;
    ShopManager_GetStatistics(&shop_stats);
    printf("[Shop Manager]\n");
    printf("  Total Shops Created: %u\n", shop_stats.total_shops_created);
    printf("  Active Shops: %u\n", shop_stats.active_shops);
    printf("    - NPC Shops: %u\n", shop_stats.npc_shops);
    printf("    - Secret Shops: %u\n", shop_stats.secret_shops);
    printf("    - Limited Shops: %u\n", shop_stats.limited_shops);
    printf("    - Special Shops: %u\n", shop_stats.special_shops);
    printf("    - Guild Shops: %u\n", shop_stats.guild_shops);
    printf("    - PVP Shops: %u\n", shop_stats.pvp_shops);
    printf("  Total Purchases: %llu\n", shop_stats.total_purchases);
    printf("  Total Gold Traded: %llu\n", shop_stats.total_gold_traded);

    /* Quest Shop Stats */
    QuestShopStatistics quest_stats;
    QuestShop_GetStatistics(&quest_stats);
    printf("\n[Quest Shop]\n");
    printf("  Total QP Earned: %llu\n", quest_stats.total_qp_earned);
    printf("  Total QP Spent: %llu\n", quest_stats.total_qp_spent);
    printf("  Total Attributes Purchased: %llu\n", quest_stats.total_attributes_purchased);

    /* Transaction Stats */
    TransactionStatistics transaction_stats;
    ShopTransaction_GetStatistics(&transaction_stats);
    printf("\n[Transactions]\n");
    printf("  Total Purchases: %llu\n", transaction_stats.total_purchases);
    printf("  Total Gold Spent: %llu\n", transaction_stats.total_gold_spent);
    printf("  Total Special Currency Spent: %llu\n", transaction_stats.total_special_spent);
    printf("  Total Sales: %llu\n", transaction_stats.total_sales);
    printf("  Total Gold Earned from Sales: %llu\n", transaction_stats.total_gold_earned);

    /* Refresh Stats */
    RefreshStatistics refresh_stats;
    ShopRefresh_GetStatistics(&refresh_stats);
    printf("\n[Refresh]\n");
    printf("  Total Refreshes: %u\n", refresh_stats.total_refreshes);
    printf("    - Secret Shop Refreshes: %u\n", refresh_stats.secret_refreshes);
    printf("    - Limited Shop Refreshes: %u\n", refresh_stats.limited_refreshes);
    printf("    - Manual Refreshes: %u\n", refresh_stats.manual_refreshes);

    /* Restriction Stats */
    RestrictionStatistics restriction_stats;
    ShopRestriction_GetStatistics(&restriction_stats);
    printf("\n[Restrictions]\n");
    printf("  Total Active Restrictions: %u\n", restriction_stats.total_restrictions);
    printf("    - Daily Restrictions: %u\n", restriction_stats.daily_restrictions);
    printf("    - Weekly Restrictions: %u\n", restriction_stats.weekly_restrictions);
    printf("    - Monthly Restrictions: %u\n", restriction_stats.monthly_restrictions);
    printf("    - Permanent Restrictions: %u\n", restriction_stats.permanent_restrictions);
    printf("  Total Denied Purchases: %u\n", restriction_stats.total_denied_purchases);

    printf("===========================================\n\n");
}
