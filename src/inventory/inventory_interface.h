/**
 * Inventory Module - Public Interface
 * Purpose: Inventory management, item handling, and storage system
 *
 * Components:
 * - Item management
 * - Inventory slots
 * - Item transactions
 * - Storage limits
 */

#ifndef SRC_INVENTORY_INVENTORY_INTERFACE_H_
#define SRC_INVENTORY_INVENTORY_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Item types and categories
 */
typedef enum {
    ITEM_TYPE_EQUIPMENT = 1,
    ITEM_TYPE_CONSUMABLE = 2,
    ITEM_TYPE_MATERIAL = 3,
    ITEM_TYPE_QUEST = 4,
    ITEM_TYPE_CURRENCY = 5,
    ITEM_TYPE_PET = 6,
    ITEM_TYPE_SKILL_BOOK = 7
} ItemType;

/**
 * Item rarity levels
 */
typedef enum {
    ITEM_RARITY_COMMON = 0,
    ITEM_RARITY_UNCOMMON = 1,
    ITEM_RARITY_RARE = 2,
    ITEM_RARITY_EPIC = 3,
    ITEM_RARITY_LEGENDARY = 4
} ItemRarity;

/**
 * Item structure representing basic item properties
 */
typedef struct {
    unsigned int item_id;        // Unique item identifier
    char name[64];               // Item name
    ItemType type;               // Item type
    ItemRarity rarity;           // Item rarity
    unsigned int quantity;       // Current item quantity
    unsigned int max_stack;      // Maximum stackable amount
    void* item_data;             // Pointer to type-specific item data
} InventoryItem;

/**
 * Inventory slot structure
 */
typedef struct {
    int slot_index;              // Inventory slot index
    InventoryItem* item;         // Item in this slot
    int is_locked;               // Slot lock status
} InventorySlot;

/**
 * Inventory structure
 */
typedef struct {
    unsigned int user_id;        // Owner's user ID
    unsigned int max_slots;      // Total inventory slots
    unsigned int used_slots;     // Currently used slots
    InventorySlot* slots;        // Inventory slots array

    // Currency tracking
    unsigned int gold;           // In-game gold
    unsigned int silver;         // In-game silver
    unsigned int premium_coins;  // Premium currency
} Inventory;

/**
 * Initialize Inventory Module
 *
 * Sets up inventory management system
 *
 * Parameters:
 *   max_user_inventories: Maximum number of user inventories to support
 *   default_inventory_slots: Default number of inventory slots per user
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Inventory_Initialize(int max_user_inventories, int default_inventory_slots);

/**
 * Cleanup Inventory Module
 *
 * Frees all inventory resources
 */
void Inventory_Cleanup();

/**
 * Create Inventory for User
 *
 * Allocates and initializes an inventory for a specific user
 *
 * Parameters:
 *   user_id: User's unique identifier
 *
 * Returns: Pointer to created Inventory, or NULL on failure
 */
Inventory* Inventory_CreateForUser(unsigned int user_id);

/**
 * Destroy User Inventory
 *
 * Frees resources associated with a user's inventory
 *
 * Parameters:
 *   inventory: Inventory to destroy
 *
 * Returns: 0 on success, error code on failure
 */
int Inventory_Destroy(Inventory* inventory);

/**
 * Add Item to Inventory
 *
 * Adds an item to user's inventory
 *
 * Parameters:
 *   inventory: Target inventory
 *   item: Item to add
 *
 * Returns: Slot index where item was placed, or negative error code
 */
int Inventory_AddItem(Inventory* inventory, InventoryItem* item);

/**
 * Remove Item from Inventory
 *
 * Removes an item from a specific slot
 *
 * Parameters:
 *   inventory: Source inventory
 *   slot_index: Inventory slot index
 *
 * Returns: 0 on success, error code on failure
 */
int Inventory_RemoveItem(Inventory* inventory, int slot_index);

/**
 * Move Item Between Slots
 *
 * Transfers an item from one inventory slot to another
 *
 * Parameters:
 *   inventory: Target inventory
 *   from_slot: Source slot index
 *   to_slot: Destination slot index
 *
 * Returns: 0 on success, error code on failure
 */
int Inventory_MoveItem(Inventory* inventory, int from_slot, int to_slot);

/**
 * Get Item from Inventory
 *
 * Retrieves an item from a specific slot
 *
 * Parameters:
 *   inventory: Source inventory
 *   slot_index: Inventory slot index
 *
 * Returns: Pointer to InventoryItem, or NULL if slot is empty
 */
InventoryItem* Inventory_GetItem(Inventory* inventory, int slot_index);

/**
 * Check Inventory Slot Availability
 *
 * Determines if a slot is available or occupied
 *
 * Parameters:
 *   inventory: Target inventory
 *   slot_index: Slot to check
 *
 * Returns: 1 if slot is empty, 0 if occupied
 */
int Inventory_IsSlotEmpty(Inventory* inventory, int slot_index);

/**
 * Expand Inventory Slots
 *
 * Increases total number of available inventory slots
 *
 * Parameters:
 *   inventory: Target inventory
 *   additional_slots: Number of slots to add
 *
 * Returns: New total number of slots, or negative error code
 */
int Inventory_ExpandSlots(Inventory* inventory, unsigned int additional_slots);

/**
 * Add Currency
 *
 * Adds specified amount of currency to inventory
 *
 * Parameters:
 *   inventory: Target inventory
 *   gold: Gold amount to add
 *   silver: Silver amount to add
 *   premium_coins: Premium coins to add
 *
 * Returns: 0 on success, error code on failure
 */
int Inventory_AddCurrency(Inventory* inventory,
                           unsigned int gold,
                           unsigned int silver,
                           unsigned int premium_coins);

/**
 * Subtract Currency
 *
 * Deducts specified amount of currency from inventory
 *
 * Parameters:
 *   inventory: Source inventory
 *   gold: Gold amount to subtract
 *   silver: Silver amount to subtract
 *   premium_coins: Premium coins to subtract
 *
 * Returns: 0 on success, error code on failure
 */
int Inventory_SubtractCurrency(Inventory* inventory,
                                unsigned int gold,
                                unsigned int silver,
                                unsigned int premium_coins);

/**
 * Get Inventory Statistics
 *
 * Retrieves current inventory module statistics
 *
 * Returns: Pointer to statistics structure
 */
void* Inventory_GetStatistics();

/**
 * Reset Inventory Module Statistics
 *
 * Clears all accumulated statistics
 */
void Inventory_ResetStatistics();

#endif // SRC_INVENTORY_INVENTORY_INTERFACE_H_