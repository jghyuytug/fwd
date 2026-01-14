/**
 * Inventory Module - Initialization and Management
 *
 * Handles inventory creation, item management, and currency tracking
 */

#include "inventory_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Inventory module global state */
static struct {
    int initialized;
    int max_user_inventories;
    int default_inventory_slots;
    Inventory* active_inventories;

    // Statistics tracking
    struct {
        unsigned int total_inventories_created;
        unsigned int total_inventories_destroyed;
        unsigned int items_added;
        unsigned int items_removed;
        unsigned int currency_transactions;
        unsigned int inventory_expansions;
        unsigned int max_concurrent_inventories;
    } stats;
} g_inventory_state = {0};

/**
 * Initialize Inventory Module
 */
int Inventory_Initialize(int max_user_inventories, int default_inventory_slots)
{
    if (g_inventory_state.initialized) {
        fprintf(stderr, "[Inventory] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_user_inventories <= 0 || max_user_inventories > 100000) {
        fprintf(stderr, "[Inventory] Invalid max user inventories: %d\n", max_user_inventories);
        return ERR_INVALID_PARAMETER;
    }

    if (default_inventory_slots <= 0 || default_inventory_slots > 1000) {
        fprintf(stderr, "[Inventory] Invalid default inventory slots: %d\n", default_inventory_slots);
        return ERR_INVALID_PARAMETER;
    }

    // Allocate inventory management resources
    g_inventory_state.active_inventories = calloc(max_user_inventories, sizeof(Inventory));
    if (!g_inventory_state.active_inventories) {
        fprintf(stderr, "[Inventory] Memory allocation failed for inventories\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Copy configuration
    g_inventory_state.max_user_inventories = max_user_inventories;
    g_inventory_state.default_inventory_slots = default_inventory_slots;

    // Reset statistics
    memset(&g_inventory_state.stats, 0, sizeof(g_inventory_state.stats));

    g_inventory_state.initialized = 1;
    printf("[Inventory] Module initialized. Max users: %d, Default slots: %d\n",
           max_user_inventories, default_inventory_slots);

    return ERR_SUCCESS;
}

/**
 * Cleanup Inventory Module
 */
void Inventory_Cleanup()
{
    if (!g_inventory_state.initialized) {
        return;
    }

    printf("[Inventory] Cleaning up inventory module...\n");
    printf("[Inventory] Active inventories at cleanup: %u\n",
           g_inventory_state.stats.total_inventories_created -
           g_inventory_state.stats.total_inventories_destroyed);

    printf("[Inventory] Statistics:\n");
    printf("  - Total Inventories Created: %u\n", g_inventory_state.stats.total_inventories_created);
    printf("  - Total Inventories Destroyed: %u\n", g_inventory_state.stats.total_inventories_destroyed);
    printf("  - Items Added: %u\n", g_inventory_state.stats.items_added);
    printf("  - Items Removed: %u\n", g_inventory_state.stats.items_removed);
    printf("  - Currency Transactions: %u\n", g_inventory_state.stats.currency_transactions);
    printf("  - Inventory Expansions: %u\n", g_inventory_state.stats.inventory_expansions);
    printf("  - Max Concurrent Inventories: %u\n", g_inventory_state.stats.max_concurrent_inventories);

    // Free inventory resources
    for (int i = 0; i < g_inventory_state.max_user_inventories; i++) {
        if (g_inventory_state.active_inventories[i].slots) {
            free(g_inventory_state.active_inventories[i].slots);
        }
    }

    free(g_inventory_state.active_inventories);
    g_inventory_state.active_inventories = NULL;

    g_inventory_state.initialized = 0;

    printf("[Inventory] Module cleanup complete\n");
}

/**
 * Create Inventory for User
 */
Inventory* Inventory_CreateForUser(unsigned int user_id)
{
    if (!g_inventory_state.initialized) {
        fprintf(stderr, "[Inventory] Module not initialized\n");
        return NULL;
    }

    // Find first free inventory slot
    for (int i = 0; i < g_inventory_state.max_user_inventories; i++) {
        if (g_inventory_state.active_inventories[i].user_id == 0) {
            Inventory* inventory = &g_inventory_state.active_inventories[i];

            // Allocate inventory slots
            inventory->slots = calloc(g_inventory_state.default_inventory_slots, sizeof(InventorySlot));
            if (!inventory->slots) {
                fprintf(stderr, "[Inventory] Memory allocation failed for inventory slots\n");
                return NULL;
            }

            // Initialize inventory properties
            inventory->user_id = user_id;
            inventory->max_slots = g_inventory_state.default_inventory_slots;
            inventory->used_slots = 0;
            inventory->gold = 0;
            inventory->silver = 0;
            inventory->premium_coins = 0;

            // Update statistics
            g_inventory_state.stats.total_inventories_created++;
            if (g_inventory_state.stats.total_inventories_created >
                g_inventory_state.stats.max_concurrent_inventories) {
                g_inventory_state.stats.max_concurrent_inventories =
                    g_inventory_state.stats.total_inventories_created;
            }

            printf("[Inventory] Created inventory for user %u (slots: %u)\n",
                   user_id, g_inventory_state.default_inventory_slots);

            return inventory;
        }
    }

    fprintf(stderr, "[Inventory] Maximum user inventories reached\n");
    return NULL;
}

/**
 * Destroy User Inventory
 */
int Inventory_Destroy(Inventory* inventory)
{
    if (!g_inventory_state.initialized || !inventory) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[Inventory] Destroying inventory for user %u\n", inventory->user_id);

    // Free inventory slots and clear resources
    if (inventory->slots) {
        // Free all items in the inventory
        for (unsigned int i = 0; i < inventory->max_slots; i++) {
            if (inventory->slots[i].item) {
                free(inventory->slots[i].item);
                inventory->slots[i].item = NULL;
            }
        }

        free(inventory->slots);
        inventory->slots = NULL;
    }

    // Reset inventory properties
    inventory->user_id = 0;
    inventory->max_slots = 0;
    inventory->used_slots = 0;
    inventory->gold = 0;
    inventory->silver = 0;
    inventory->premium_coins = 0;

    // Update statistics
    g_inventory_state.stats.total_inventories_destroyed++;

    return ERR_SUCCESS;
}

/**
 * Add Item to Inventory
 */
int Inventory_AddItem(Inventory* inventory, InventoryItem* item)
{
    if (!g_inventory_state.initialized || !inventory || !item) {
        return ERR_INVALID_PARAMETER;
    }

    // Find first empty slot
    for (unsigned int i = 0; i < inventory->max_slots; i++) {
        if (inventory->slots[i].item == NULL) {
            // Allocate and copy item
            inventory->slots[i].item = malloc(sizeof(InventoryItem));
            if (!inventory->slots[i].item) {
                fprintf(stderr, "[Inventory] Failed to allocate memory for item\n");
                return ERR_OUT_OF_MEMORY;
            }

            memcpy(inventory->slots[i].item, item, sizeof(InventoryItem));
            inventory->slots[i].slot_index = i;
            inventory->used_slots++;

            g_inventory_state.stats.items_added++;

            printf("[Inventory] Added item %u to slot %u for user %u\n",
                   item->item_id, i, inventory->user_id);

            return i;
        }
    }

    fprintf(stderr, "[Inventory] No empty slots available for item\n");
    return ERR_INVENTORY_FULL;
}

/**
 * Remove Item from Inventory
 */
int Inventory_RemoveItem(Inventory* inventory, int slot_index)
{
    if (!g_inventory_state.initialized || !inventory) {
        return ERR_INVALID_PARAMETER;
    }

    if (slot_index < 0 || slot_index >= inventory->max_slots) {
        fprintf(stderr, "[Inventory] Invalid slot index: %d\n", slot_index);
        return ERR_INVALID_PARAMETER;
    }

    if (!inventory->slots[slot_index].item) {
        fprintf(stderr, "[Inventory] Slot %d is already empty\n", slot_index);
        return ERR_INVALID_STATE;
    }

    printf("[Inventory] Removing item %u from slot %d for user %u\n",
           inventory->slots[slot_index].item->item_id, slot_index, inventory->user_id);

    // Free item memory
    free(inventory->slots[slot_index].item);
    inventory->slots[slot_index].item = NULL;
    inventory->used_slots--;

    g_inventory_state.stats.items_removed++;

    return ERR_SUCCESS;
}

/**
 * Add Currency
 */
int Inventory_AddCurrency(Inventory* inventory,
                           unsigned int gold,
                           unsigned int silver,
                           unsigned int premium_coins)
{
    if (!g_inventory_state.initialized || !inventory) {
        return ERR_INVALID_PARAMETER;
    }

    /*
     * Currency caps: Prevent overflow and enforce game economy limits
     * - Gold cap: 2,000,000,000 (2 billion) - standard MMORPG limit
     * - Silver cap: 999,999 - typically lower tier currency
     * - Premium coins: No practical cap (server-side tracked)
     */
    #define CURRENCY_CAP_GOLD    2000000000U
    #define CURRENCY_CAP_SILVER  999999U

    /* Apply caps to prevent overflow */
    if (inventory->gold > CURRENCY_CAP_GOLD - gold) {
        inventory->gold = CURRENCY_CAP_GOLD;
    } else {
        inventory->gold += gold;
    }

    if (inventory->silver > CURRENCY_CAP_SILVER - silver) {
        inventory->silver = CURRENCY_CAP_SILVER;
    } else {
        inventory->silver += silver;
    }

    inventory->premium_coins += premium_coins;

    g_inventory_state.stats.currency_transactions++;

    printf("[Inventory] Currency added to user %u: %u gold, %u silver, %u premium coins\n",
           inventory->user_id, gold, silver, premium_coins);

    return ERR_SUCCESS;
}

/**
 * Subtract Currency
 */
int Inventory_SubtractCurrency(Inventory* inventory,
                                unsigned int gold,
                                unsigned int silver,
                                unsigned int premium_coins)
{
    if (!g_inventory_state.initialized || !inventory) {
        return ERR_INVALID_PARAMETER;
    }

    // Check if sufficient currency
    if (inventory->gold < gold ||
        inventory->silver < silver ||
        inventory->premium_coins < premium_coins) {
        fprintf(stderr, "[Inventory] Insufficient currency for user %u\n", inventory->user_id);
        return ERR_INSUFFICIENT_FUNDS;
    }

    inventory->gold -= gold;
    inventory->silver -= silver;
    inventory->premium_coins -= premium_coins;

    g_inventory_state.stats.currency_transactions++;

    printf("[Inventory] Currency subtracted from user %u: %u gold, %u silver, %u premium coins\n",
           inventory->user_id, gold, silver, premium_coins);

    return ERR_SUCCESS;
}

/**
 * Get Inventory Module Statistics
 */
void* Inventory_GetStatistics()
{
    if (!g_inventory_state.initialized) {
        return NULL;
    }

    return &g_inventory_state.stats;
}

/**
 * Reset Inventory Module Statistics
 */
void Inventory_ResetStatistics()
{
    if (!g_inventory_state.initialized) {
        return;
    }

    memset(&g_inventory_state.stats, 0, sizeof(g_inventory_state.stats));
    printf("[Inventory] Module statistics reset\n");
}