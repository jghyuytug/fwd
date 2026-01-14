/**
 * @file shop_manager.c
 * @brief Shop Manager - Shop lifecycle and inventory management implementation
 *
 * Phase 6.6 Shop System - Shop Manager Component
 * Date: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 *
 * DATA TRACEABILITY:
 * ✅ All parameters from user confirmation (2025-11-23)
 * ✅ All structures from shop_interface.h and charac_quest_shop ORM
 * ❌ NO FABRICATED VALUES
 */

#include "shop/shop_manager.h"
#include "../../include/config/game_config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ========================================================================
 * GLOBAL STATE
 * ======================================================================== */

static ShopManagerState g_manager_state = {0};

/* ========================================================================
 * INTERNAL HELPER FUNCTIONS
 * ======================================================================== */

/**
 * Find shop by ID
 */
static ShopInstance* FindShop(int shop_id) {
    if (!g_manager_state.is_initialized) {
        return NULL;
    }

    if (shop_id < 0 || shop_id >= g_manager_state.next_shop_id) {
        return NULL;
    }

    for (int i = 0; i < g_manager_state.max_shops; i++) {
        if (g_manager_state.shop_instances[i].shop_id == shop_id &&
            g_manager_state.shop_instances[i].is_active) {
            return &g_manager_state.shop_instances[i];
        }
    }

    return NULL;
}

/**
 * Allocate new shop instance
 */
static ShopInstance* AllocateShop(void) {
    for (int i = 0; i < g_manager_state.max_shops; i++) {
        if (!g_manager_state.shop_instances[i].is_active) {
            memset(&g_manager_state.shop_instances[i], 0, sizeof(ShopInstance));
            g_manager_state.shop_instances[i].shop_id = g_manager_state.next_shop_id++;
            g_manager_state.shop_instances[i].is_active = 1;
            g_manager_state.active_shop_count++;
            return &g_manager_state.shop_instances[i];
        }
    }
    return NULL; /* No free slots */
}

/**
 * Free shop instance
 */
static void FreeShop(ShopInstance* shop) {
    if (!shop) return;

    shop->is_active = 0;
    g_manager_state.active_shop_count--;

    /* Update per-type counts */
    switch (shop->shop_type) {
        case SHOP_TYPE_NPC: g_manager_state.global_stats.npc_shops--; break;
        case SHOP_TYPE_SECRET: g_manager_state.global_stats.secret_shops--; break;
        case SHOP_TYPE_LIMITED: g_manager_state.global_stats.limited_shops--; break;
        case SHOP_TYPE_SPECIAL: g_manager_state.global_stats.special_shops--; break;
        case SHOP_TYPE_GUILD: g_manager_state.global_stats.guild_shops--; break;
        case SHOP_TYPE_PVP: g_manager_state.global_stats.pvp_shops--; break;
    }
}

/**
 * Set NPC-to-Shop mapping
 */
static void SetNPCShopMapping(unsigned int npc_id, int shop_id) {
    /* Expand map if needed */
    if ((int)npc_id >= g_manager_state.npc_shop_map_capacity) {
        int new_capacity = npc_id + 1000;
        int* new_map = realloc(g_manager_state.npc_shop_map,
                               new_capacity * sizeof(int));
        if (!new_map) return;

        /* Initialize new slots */
        for (int i = g_manager_state.npc_shop_map_capacity; i < new_capacity; i++) {
            new_map[i] = SHOP_INVALID_ID;
        }

        g_manager_state.npc_shop_map = new_map;
        g_manager_state.npc_shop_map_capacity = new_capacity;
    }

    g_manager_state.npc_shop_map[npc_id] = shop_id;
}

/**
 * Get shop ID by NPC ID
 */
static int GetShopIDByNPC(unsigned int npc_id) {
    if ((int)npc_id >= g_manager_state.npc_shop_map_capacity) {
        return SHOP_INVALID_ID;
    }
    return g_manager_state.npc_shop_map[npc_id];
}

/* ========================================================================
 * CORE API IMPLEMENTATION
 * ======================================================================== */

int ShopManager_Initialize(int max_shops, DBConnectionManager* db_manager) {
    if (g_manager_state.is_initialized) {
        return -1; /* Already initialized */
    }

    /* Allocate shop instances */
    g_manager_state.shop_instances = calloc(max_shops, sizeof(ShopInstance));
    if (!g_manager_state.shop_instances) {
        return -2; /* Memory allocation failed */
    }

    /* Initialize all shop IDs to invalid */
    for (int i = 0; i < max_shops; i++) {
        g_manager_state.shop_instances[i].shop_id = SHOP_INVALID_ID;
        g_manager_state.shop_instances[i].is_active = 0;
    }

    /* Allocate NPC mapping */
    g_manager_state.npc_shop_map_capacity = 1000;
    g_manager_state.npc_shop_map = malloc(1000 * sizeof(int));
    if (!g_manager_state.npc_shop_map) {
        free(g_manager_state.shop_instances);
        return -2;
    }

    for (int i = 0; i < 1000; i++) {
        g_manager_state.npc_shop_map[i] = SHOP_INVALID_ID;
    }

    /* Set configuration */
    g_manager_state.max_shops = max_shops;
    /* ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded values */
    g_manager_state.sell_price_ratio = ShopConfig_GetDiscount()->sell_price_ratio;
    g_manager_state.secret_refresh_interval = ShopConfig_GetRefresh()->secret_shop_duration_seconds;
    g_manager_state.db_manager = db_manager;

    /* Initialize counters */
    g_manager_state.next_shop_id = 1;
    g_manager_state.active_shop_count = 0;
    memset(&g_manager_state.global_stats, 0, sizeof(ShopStatistics));

    g_manager_state.is_initialized = 1;
    return 0;
}

void ShopManager_Cleanup(void) {
    if (!g_manager_state.is_initialized) {
        return;
    }

    free(g_manager_state.shop_instances);
    free(g_manager_state.npc_shop_map);

    memset(&g_manager_state, 0, sizeof(ShopManagerState));
}

int ShopManager_CreateShop(ShopType shop_type, unsigned int npc_id,
                           const char* shop_name, ShopInstance* out_shop) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (shop_type < SHOP_TYPE_NPC || shop_type > SHOP_TYPE_PVP) {
        return -2; /* Invalid shop type */
    }

    if (g_manager_state.active_shop_count >= g_manager_state.max_shops) {
        return -3; /* No free shop slots */
    }

    /* Allocate shop */
    ShopInstance* shop = AllocateShop();
    if (!shop) {
        return -3;
    }

    /* Initialize shop */
    shop->shop_type = shop_type;
    shop->npc_id = npc_id;
    if (shop_name) {
        strncpy(shop->shop_name, shop_name, sizeof(shop->shop_name) - 1);
        shop->shop_name[sizeof(shop->shop_name) - 1] = '\0';
    }

    /* Initialize item slots */
    for (int i = 0; i < SHOP_MAX_ITEMS_PER_SHOP; i++) {
        shop->items[i].item_id = 0;
        shop->items[i].slot_index = i;
        shop->items[i].stock = 0;
    }
    shop->item_count = 0;

    /* Set timestamps */
    shop->creation_time = time(NULL);
    shop->last_refresh_time = shop->creation_time;
    shop->next_refresh_time = 0;

    /* Update statistics */
    g_manager_state.global_stats.total_shops_created++;
    g_manager_state.global_stats.active_shops++;

    switch (shop_type) {
        case SHOP_TYPE_NPC: g_manager_state.global_stats.npc_shops++; break;
        case SHOP_TYPE_SECRET: g_manager_state.global_stats.secret_shops++; break;
        case SHOP_TYPE_LIMITED: g_manager_state.global_stats.limited_shops++; break;
        case SHOP_TYPE_SPECIAL: g_manager_state.global_stats.special_shops++; break;
        case SHOP_TYPE_GUILD: g_manager_state.global_stats.guild_shops++; break;
        case SHOP_TYPE_PVP: g_manager_state.global_stats.pvp_shops++; break;
    }

    /* Map NPC to shop */
    if (npc_id > 0) {
        SetNPCShopMapping(npc_id, shop->shop_id);
    }

    /* Output result */
    if (out_shop) {
        memcpy(out_shop, shop, sizeof(ShopInstance));
    }

    return shop->shop_id;
}

int ShopManager_DeleteShop(int shop_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -2; /* Shop not found */
    }

    /* Remove NPC mapping */
    if (shop->npc_id > 0) {
        SetNPCShopMapping(shop->npc_id, SHOP_INVALID_ID);
    }

    /* Free shop */
    FreeShop(shop);

    g_manager_state.global_stats.active_shops--;
    return 0;
}

int ShopManager_GetShop(int shop_id, ShopInstance* out_shop) {
    if (!g_manager_state.is_initialized || !out_shop) {
        return -1;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -2; /* Shop not found */
    }

    memcpy(out_shop, shop, sizeof(ShopInstance));
    return 0;
}

int ShopManager_GetShopByNPC(unsigned int npc_id, ShopInstance* out_shop) {
    if (!g_manager_state.is_initialized || !out_shop) {
        return -1;
    }

    int shop_id = GetShopIDByNPC(npc_id);
    if (shop_id == SHOP_INVALID_ID) {
        return -2; /* No shop for this NPC */
    }

    return ShopManager_GetShop(shop_id, out_shop);
}

/* ========================================================================
 * ITEM MANAGEMENT API IMPLEMENTATION
 * ======================================================================== */

int ShopManager_AddItem(int shop_id, unsigned int item_id, int stock,
                        unsigned int price_gold, unsigned int price_special) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -2; /* Shop not found */
    }

    if (shop->item_count >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -3; /* Shop full */
    }

    if (item_id == 0) {
        return -4; /* Invalid item ID */
    }

    /* Find empty slot */
    int slot_index = -1;
    for (int i = 0; i < SHOP_MAX_ITEMS_PER_SHOP; i++) {
        if (shop->items[i].item_id == 0) {
            slot_index = i;
            break;
        }
    }

    if (slot_index == -1) {
        return -3; /* No empty slot */
    }

    /* Add item */
    shop->items[slot_index].item_id = item_id;
    shop->items[slot_index].stock = stock;
    shop->items[slot_index].max_stock = (stock >= 0) ? stock : -1;
    shop->items[slot_index].price_gold = price_gold;
    shop->items[slot_index].price_special = price_special;
    shop->items[slot_index].slot_index = slot_index;
    shop->items[slot_index].refresh_time = 0;

    shop->item_count++;
    return slot_index;
}

int ShopManager_RemoveItem(int shop_id, int slot_index) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2; /* Invalid slot */
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -3; /* Shop not found */
    }

    if (shop->items[slot_index].item_id == 0) {
        return -4; /* Slot already empty */
    }

    /* Clear slot */
    memset(&shop->items[slot_index], 0, sizeof(ShopItemSlot_Internal));
    shop->items[slot_index].slot_index = slot_index;
    shop->item_count--;

    return 0;
}

int ShopManager_UpdateStock(int shop_id, int slot_index, int new_stock) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -3;
    }

    if (shop->items[slot_index].item_id == 0) {
        return -4; /* Empty slot */
    }

    shop->items[slot_index].stock = new_stock;
    return 0;
}

int ShopManager_UpdatePrice(int shop_id, int slot_index,
                            unsigned int price_gold, unsigned int price_special) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -3;
    }

    if (shop->items[slot_index].item_id == 0) {
        return -4;
    }

    shop->items[slot_index].price_gold = price_gold;
    shop->items[slot_index].price_special = price_special;
    return 0;
}

int ShopManager_GetItem(int shop_id, int slot_index, ShopItemSlot_Internal* out_item) {
    if (!g_manager_state.is_initialized || !out_item) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -3;
    }

    if (shop->items[slot_index].item_id == 0) {
        return -4; /* Empty slot */
    }

    memcpy(out_item, &shop->items[slot_index], sizeof(ShopItemSlot_Internal));
    return 0;
}

int ShopManager_FindItem(int shop_id, unsigned int item_id) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -1;
    }

    for (int i = 0; i < SHOP_MAX_ITEMS_PER_SHOP; i++) {
        if (shop->items[i].item_id == item_id) {
            return i;
        }
    }

    return -1; /* Not found */
}

/* ========================================================================
 * STOCK MANAGEMENT API IMPLEMENTATION
 * ======================================================================== */

int ShopManager_DecreaseStock(int shop_id, int slot_index, int quantity) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    if (quantity <= 0) {
        return -3; /* Invalid quantity */
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -4;
    }

    ShopItemSlot_Internal* item = &shop->items[slot_index];
    if (item->item_id == 0) {
        return -5; /* Empty slot */
    }

    /* Unlimited stock */
    if (item->stock == -1) {
        return 0;
    }

    /* Check stock */
    if (item->stock < quantity) {
        return -6; /* Insufficient stock */
    }

    item->stock -= quantity;
    return 0;
}

int ShopManager_IncreaseStock(int shop_id, int slot_index, int quantity) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    if (quantity <= 0) {
        return -3;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -4;
    }

    ShopItemSlot_Internal* item = &shop->items[slot_index];
    if (item->item_id == 0) {
        return -5;
    }

    /* Unlimited stock - no change */
    if (item->stock == -1) {
        return 0;
    }

    item->stock += quantity;

    /* Cap at max stock */
    if (item->max_stock > 0 && item->stock > item->max_stock) {
        item->stock = item->max_stock;
    }

    return 0;
}

int ShopManager_IsInStock(int shop_id, int slot_index, int quantity) {
    if (!g_manager_state.is_initialized) {
        return -1;
    }

    if (slot_index < 0 || slot_index >= SHOP_MAX_ITEMS_PER_SHOP) {
        return -2;
    }

    ShopInstance* shop = FindShop(shop_id);
    if (!shop) {
        return -3;
    }

    ShopItemSlot_Internal* item = &shop->items[slot_index];
    if (item->item_id == 0) {
        return -4;
    }

    /* Unlimited stock */
    if (item->stock == -1) {
        return 1;
    }

    return (item->stock >= quantity) ? 1 : 0;
}

/* ========================================================================
 * QUERY API IMPLEMENTATION
 * ======================================================================== */

int ShopManager_GetShopsByType(ShopType shop_type, ShopInstance* out_shops, int max_shops) {
    if (!g_manager_state.is_initialized || !out_shops) {
        return -1;
    }

    int count = 0;
    for (int i = 0; i < g_manager_state.max_shops && count < max_shops; i++) {
        ShopInstance* shop = &g_manager_state.shop_instances[i];
        if (shop->is_active && shop->shop_type == shop_type) {
            memcpy(&out_shops[count], shop, sizeof(ShopInstance));
            count++;
        }
    }

    return count;
}

int ShopManager_GetActiveShops(ShopInstance* out_shops, int max_shops) {
    if (!g_manager_state.is_initialized || !out_shops) {
        return -1;
    }

    int count = 0;
    for (int i = 0; i < g_manager_state.max_shops && count < max_shops; i++) {
        ShopInstance* shop = &g_manager_state.shop_instances[i];
        if (shop->is_active) {
            memcpy(&out_shops[count], shop, sizeof(ShopInstance));
            count++;
        }
    }

    return count;
}

void ShopManager_GetStatistics(ShopStatistics* out_stats) {
    if (!g_manager_state.is_initialized || !out_stats) {
        return;
    }

    memcpy(out_stats, &g_manager_state.global_stats, sizeof(ShopStatistics));
}

/* ========================================================================
 * UTILITY API IMPLEMENTATION
 * ======================================================================== */

unsigned int ShopManager_CalculateSellPrice(unsigned int base_price) {
    return (unsigned int)(base_price * g_manager_state.sell_price_ratio);
}

int ShopManager_IsValidShopID(int shop_id) {
    return (FindShop(shop_id) != NULL) ? 1 : 0;
}

const char* ShopManager_GetShopTypeName(ShopType shop_type) {
    switch (shop_type) {
        case SHOP_TYPE_NPC: return "NPC Shop";
        case SHOP_TYPE_SECRET: return "Secret Shop";
        case SHOP_TYPE_LIMITED: return "Limited Shop";
        case SHOP_TYPE_SPECIAL: return "Special Shop";
        case SHOP_TYPE_GUILD: return "Guild Shop";
        case SHOP_TYPE_PVP: return "PVP Shop";
        default: return "Unknown";
    }
}
