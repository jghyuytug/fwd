/**
 * Inventory Module - Full Implementation
 *
 * Handles inventory management, item storage, and currency
 * Features:
 * - Per-user inventory tracking
 * - Item slot management
 * - Item stacking
 * - Currency tracking (gold, silver, premium)
 * - Slot expansion
 */

#include "inventory_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Constants */
#define MAX_USERS 10000
#define MAX_SLOTS 500
#define DEFAULT_SLOTS 50
#define MAX_GOLD 0xFFFFFFFF

/* Internal inventory structure */
typedef struct {
    unsigned int user_id;
    int max_slots;
    int used_slots;

    /* Items array (slot_index -> item) */
    InventoryItem items[MAX_SLOTS];
    int slot_occupied[MAX_SLOTS];

    /* Currency */
    unsigned int gold;
    unsigned int silver;
    unsigned int premium_coins;

    int is_active;
} InventoryInternal;

/* Module statistics */
typedef struct {
    unsigned int total_inventories_created;
    unsigned int total_inventories_destroyed;
    unsigned int total_items_added;
    unsigned int total_items_removed;
    unsigned int total_items_moved;
    unsigned int total_gold_added;
    unsigned int total_gold_removed;
    unsigned int total_slots_expanded;
} InventoryStatistics;

/* Module global state */
static struct {
    int initialized;
    int max_user_inventories;
    int default_inventory_slots;

    /* Inventory storage */
    InventoryInternal* inventories;
    int inventory_count;

    /* Statistics */
    InventoryStatistics stats;
} g_inv_state = {0};

/* Forward declarations */
static InventoryInternal* FindInventory(unsigned int user_id);
static InventoryInternal* FindOrCreateInventory(unsigned int user_id);
static int FindEmptySlot(InventoryInternal* inv);
static int FindStackableSlot(InventoryInternal* inv, unsigned int item_id);

/**
 * Initialize Inventory Module
 */
int Inventory_Initialize(int max_user_inventories, int default_inventory_slots)
{
    if (g_inv_state.initialized) {
        fprintf(stderr, "[Inventory] Module already initialized\n");
        return ERR_ALREADY_INITIALIZED;
    }

    if (max_user_inventories <= 0 || max_user_inventories > MAX_USERS) {
        fprintf(stderr, "[Inventory] Invalid max_user_inventories: %d\n", max_user_inventories);
        return ERR_INVALID_PARAMETER;
    }

    if (default_inventory_slots <= 0 || default_inventory_slots > MAX_SLOTS) {
        fprintf(stderr, "[Inventory] Invalid default_inventory_slots: %d\n", default_inventory_slots);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Inventory] Initializing inventory module...\n");

    /* Allocate inventories */
    g_inv_state.inventories = (InventoryInternal*)calloc(max_user_inventories, sizeof(InventoryInternal));
    if (!g_inv_state.inventories) {
        fprintf(stderr, "[Inventory] Failed to allocate inventory storage\n");
        return ERR_MEMORY;
    }

    g_inv_state.max_user_inventories = max_user_inventories;
    g_inv_state.default_inventory_slots = default_inventory_slots;
    g_inv_state.inventory_count = 0;
    g_inv_state.initialized = 1;

    printf("[Inventory] Module initialized (max_users: %d, default_slots: %d)\n",
           max_user_inventories, default_inventory_slots);

    return ERR_SUCCESS;
}

/**
 * Cleanup Inventory Module
 */
void Inventory_Cleanup()
{
    if (!g_inv_state.initialized) {
        return;
    }

    printf("[Inventory] Cleaning up inventory module...\n");
    printf("[Inventory] Statistics:\n");
    printf("  - Total Inventories Created: %u\n", g_inv_state.stats.total_inventories_created);
    printf("  - Total Inventories Destroyed: %u\n", g_inv_state.stats.total_inventories_destroyed);
    printf("  - Total Items Added: %u\n", g_inv_state.stats.total_items_added);
    printf("  - Total Items Removed: %u\n", g_inv_state.stats.total_items_removed);
    printf("  - Total Items Moved: %u\n", g_inv_state.stats.total_items_moved);
    printf("  - Total Gold Added: %u\n", g_inv_state.stats.total_gold_added);
    printf("  - Total Gold Removed: %u\n", g_inv_state.stats.total_gold_removed);
    printf("  - Total Slots Expanded: %u\n", g_inv_state.stats.total_slots_expanded);
    printf("  - Active Inventories: %d\n", g_inv_state.inventory_count);

    if (g_inv_state.inventories) {
        free(g_inv_state.inventories);
        g_inv_state.inventories = NULL;
    }

    memset(&g_inv_state, 0, sizeof(g_inv_state));

    printf("[Inventory] Module cleanup complete\n");
}

/**
 * Create Inventory for User
 */
Inventory* Inventory_CreateForUser(unsigned int user_id)
{
    if (!g_inv_state.initialized) {
        fprintf(stderr, "[Inventory] Module not initialized\n");
        return NULL;
    }

    if (user_id == 0) {
        fprintf(stderr, "[Inventory] Invalid user_id\n");
        return NULL;
    }

    /* Check if already exists */
    InventoryInternal* existing = FindInventory(user_id);
    if (existing) {
        /* Return existing as pseudo-Inventory pointer */
        return (Inventory*)existing;
    }

    /* Check capacity */
    if (g_inv_state.inventory_count >= g_inv_state.max_user_inventories) {
        fprintf(stderr, "[Inventory] Max inventories reached\n");
        return NULL;
    }

    /* Find free slot */
    InventoryInternal* inv = NULL;
    for (int i = 0; i < g_inv_state.max_user_inventories; i++) {
        if (!g_inv_state.inventories[i].is_active) {
            inv = &g_inv_state.inventories[i];
            break;
        }
    }

    if (!inv) {
        return NULL;
    }

    /* Initialize inventory */
    memset(inv, 0, sizeof(InventoryInternal));
    inv->user_id = user_id;
    inv->max_slots = g_inv_state.default_inventory_slots;
    inv->used_slots = 0;
    inv->gold = 0;
    inv->silver = 0;
    inv->premium_coins = 0;
    inv->is_active = 1;

    g_inv_state.inventory_count++;
    g_inv_state.stats.total_inventories_created++;

    printf("[Inventory] Created inventory for user %u (slots: %d)\n",
           user_id, inv->max_slots);

    return (Inventory*)inv;
}

/**
 * Destroy User Inventory
 */
int Inventory_Destroy(Inventory* inventory)
{
    if (!g_inv_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!inventory) {
        return ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return ERR_NOT_FOUND;
    }

    printf("[Inventory] Destroying inventory for user %u\n", inv->user_id);

    inv->is_active = 0;
    g_inv_state.inventory_count--;
    g_inv_state.stats.total_inventories_destroyed++;

    return ERR_SUCCESS;
}

/**
 * Add Item to Inventory
 */
int Inventory_AddItem(Inventory* inventory, InventoryItem* item)
{
    if (!g_inv_state.initialized) {
        return -ERR_NOT_INITIALIZED;
    }

    if (!inventory || !item) {
        return -ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return -ERR_NOT_FOUND;
    }

    /* Try to stack with existing item */
    if (item->max_stack > 1) {
        int stack_slot = FindStackableSlot(inv, item->item_id);
        if (stack_slot >= 0) {
            InventoryItem* existing = &inv->items[stack_slot];
            unsigned int can_add = existing->max_stack - existing->quantity;

            if (can_add >= item->quantity) {
                existing->quantity += item->quantity;
                g_inv_state.stats.total_items_added++;
                return stack_slot;
            }
        }
    }

    /* Find empty slot */
    int slot = FindEmptySlot(inv);
    if (slot < 0) {
        return -ERR_LIMIT_EXCEEDED;  /* Inventory full */
    }

    /* Add item to slot */
    memcpy(&inv->items[slot], item, sizeof(InventoryItem));
    inv->slot_occupied[slot] = 1;
    inv->used_slots++;

    g_inv_state.stats.total_items_added++;

    return slot;
}

/**
 * Remove Item from Inventory
 */
int Inventory_RemoveItem(Inventory* inventory, int slot_index)
{
    if (!g_inv_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!inventory) {
        return ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return ERR_NOT_FOUND;
    }

    if (slot_index < 0 || slot_index >= inv->max_slots) {
        return ERR_INVALID_PARAMETER;
    }

    if (!inv->slot_occupied[slot_index]) {
        return ERR_NOT_FOUND;  /* Slot already empty */
    }

    /* Clear slot */
    memset(&inv->items[slot_index], 0, sizeof(InventoryItem));
    inv->slot_occupied[slot_index] = 0;
    inv->used_slots--;

    g_inv_state.stats.total_items_removed++;

    return ERR_SUCCESS;
}

/**
 * Move Item Between Slots
 */
int Inventory_MoveItem(Inventory* inventory, int from_slot, int to_slot)
{
    if (!g_inv_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!inventory) {
        return ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return ERR_NOT_FOUND;
    }

    if (from_slot < 0 || from_slot >= inv->max_slots ||
        to_slot < 0 || to_slot >= inv->max_slots) {
        return ERR_INVALID_PARAMETER;
    }

    if (from_slot == to_slot) {
        return ERR_SUCCESS;  /* No move needed */
    }

    if (!inv->slot_occupied[from_slot]) {
        return ERR_NOT_FOUND;  /* Source slot empty */
    }

    /* Swap items */
    InventoryItem temp;
    int temp_occupied;

    memcpy(&temp, &inv->items[from_slot], sizeof(InventoryItem));
    temp_occupied = inv->slot_occupied[from_slot];

    memcpy(&inv->items[from_slot], &inv->items[to_slot], sizeof(InventoryItem));
    inv->slot_occupied[from_slot] = inv->slot_occupied[to_slot];

    memcpy(&inv->items[to_slot], &temp, sizeof(InventoryItem));
    inv->slot_occupied[to_slot] = temp_occupied;

    g_inv_state.stats.total_items_moved++;

    return ERR_SUCCESS;
}

/**
 * Get Item from Inventory
 */
InventoryItem* Inventory_GetItem(Inventory* inventory, int slot_index)
{
    if (!g_inv_state.initialized || !inventory) {
        return NULL;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return NULL;
    }

    if (slot_index < 0 || slot_index >= inv->max_slots) {
        return NULL;
    }

    if (!inv->slot_occupied[slot_index]) {
        return NULL;
    }

    return &inv->items[slot_index];
}

/**
 * Check Inventory Slot Availability
 */
int Inventory_IsSlotEmpty(Inventory* inventory, int slot_index)
{
    if (!g_inv_state.initialized || !inventory) {
        return 0;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return 0;
    }

    if (slot_index < 0 || slot_index >= inv->max_slots) {
        return 0;
    }

    return !inv->slot_occupied[slot_index];
}

/**
 * Expand Inventory Slots
 */
int Inventory_ExpandSlots(Inventory* inventory, unsigned int additional_slots)
{
    if (!g_inv_state.initialized) {
        return -ERR_NOT_INITIALIZED;
    }

    if (!inventory || additional_slots == 0) {
        return -ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return -ERR_NOT_FOUND;
    }

    int new_total = inv->max_slots + additional_slots;
    if (new_total > MAX_SLOTS) {
        new_total = MAX_SLOTS;
        additional_slots = MAX_SLOTS - inv->max_slots;
    }

    if (additional_slots == 0) {
        return inv->max_slots;  /* Already at max */
    }

    inv->max_slots = new_total;
    g_inv_state.stats.total_slots_expanded += additional_slots;

    printf("[Inventory] Expanded inventory for user %u to %d slots\n",
           inv->user_id, inv->max_slots);

    return inv->max_slots;
}

/**
 * Add Currency
 */
int Inventory_AddCurrency(Inventory* inventory,
                           unsigned int gold,
                           unsigned int silver,
                           unsigned int premium_coins)
{
    if (!g_inv_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!inventory) {
        return ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return ERR_NOT_FOUND;
    }

    /* Add with overflow protection */
    if (gold > 0) {
        if (inv->gold > MAX_GOLD - gold) {
            inv->gold = MAX_GOLD;
        } else {
            inv->gold += gold;
        }
        g_inv_state.stats.total_gold_added += gold;
    }

    if (silver > 0) {
        if (inv->silver > MAX_GOLD - silver) {
            inv->silver = MAX_GOLD;
        } else {
            inv->silver += silver;
        }
    }

    if (premium_coins > 0) {
        if (inv->premium_coins > MAX_GOLD - premium_coins) {
            inv->premium_coins = MAX_GOLD;
        } else {
            inv->premium_coins += premium_coins;
        }
    }

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
    if (!g_inv_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!inventory) {
        return ERR_INVALID_PARAMETER;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;

    if (!inv->is_active) {
        return ERR_NOT_FOUND;
    }

    /* Check sufficient funds */
    if (gold > inv->gold || silver > inv->silver || premium_coins > inv->premium_coins) {
        return ERR_INVALID_STATE;  /* Insufficient funds */
    }

    inv->gold -= gold;
    inv->silver -= silver;
    inv->premium_coins -= premium_coins;

    g_inv_state.stats.total_gold_removed += gold;

    return ERR_SUCCESS;
}

/**
 * Get Inventory by User ID
 */
Inventory* Inventory_GetByUserId(unsigned int user_id)
{
    if (!g_inv_state.initialized) {
        return NULL;
    }

    InventoryInternal* inv = FindInventory(user_id);
    return (Inventory*)inv;
}

/**
 * Get Available Slots
 */
int Inventory_GetAvailableSlots(Inventory* inventory)
{
    if (!g_inv_state.initialized || !inventory) {
        return 0;
    }

    InventoryInternal* inv = (InventoryInternal*)inventory;
    if (!inv->is_active) {
        return 0;
    }

    return inv->max_slots - inv->used_slots;
}

/**
 * Get Inventory Statistics
 */
void* Inventory_GetStatistics()
{
    if (!g_inv_state.initialized) {
        return NULL;
    }

    return &g_inv_state.stats;
}

/**
 * Reset Inventory Module Statistics
 */
void Inventory_ResetStatistics()
{
    if (!g_inv_state.initialized) {
        return;
    }

    memset(&g_inv_state.stats, 0, sizeof(InventoryStatistics));
}

/**
 * Print Module Statistics
 */
void Inventory_PrintStats()
{
    if (!g_inv_state.initialized) {
        printf("[Inventory] Module not initialized\n");
        return;
    }

    printf("=== Inventory System Statistics ===\n");
    printf("Active Inventories: %d / %d\n",
           g_inv_state.inventory_count, g_inv_state.max_user_inventories);
    printf("\nGlobal Stats:\n");
    printf("  Inventories Created: %u\n", g_inv_state.stats.total_inventories_created);
    printf("  Inventories Destroyed: %u\n", g_inv_state.stats.total_inventories_destroyed);
    printf("  Items Added: %u\n", g_inv_state.stats.total_items_added);
    printf("  Items Removed: %u\n", g_inv_state.stats.total_items_removed);
    printf("  Items Moved: %u\n", g_inv_state.stats.total_items_moved);
    printf("  Gold Added: %u\n", g_inv_state.stats.total_gold_added);
    printf("  Gold Removed: %u\n", g_inv_state.stats.total_gold_removed);
    printf("  Slots Expanded: %u\n", g_inv_state.stats.total_slots_expanded);
}

/* ============================================================
 * Internal Helper Functions
 * ============================================================ */

static InventoryInternal* FindInventory(unsigned int user_id)
{
    for (int i = 0; i < g_inv_state.max_user_inventories; i++) {
        if (g_inv_state.inventories[i].is_active &&
            g_inv_state.inventories[i].user_id == user_id) {
            return &g_inv_state.inventories[i];
        }
    }
    return NULL;
}

static InventoryInternal* FindOrCreateInventory(unsigned int user_id)
{
    InventoryInternal* inv = FindInventory(user_id);
    if (inv) {
        return inv;
    }

    return (InventoryInternal*)Inventory_CreateForUser(user_id);
}

static int FindEmptySlot(InventoryInternal* inv)
{
    for (int i = 0; i < inv->max_slots; i++) {
        if (!inv->slot_occupied[i]) {
            return i;
        }
    }
    return -1;
}

static int FindStackableSlot(InventoryInternal* inv, unsigned int item_id)
{
    for (int i = 0; i < inv->max_slots; i++) {
        if (inv->slot_occupied[i] &&
            inv->items[i].item_id == item_id &&
            inv->items[i].quantity < inv->items[i].max_stack) {
            return i;
        }
    }
    return -1;
}
