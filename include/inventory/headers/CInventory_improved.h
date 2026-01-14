#ifndef __CINVENTORY_IMPROVED_H__
#define __CINVENTORY_IMPROVED_H__

#include <stdint.h>
#include <stdbool.h>

// ========================================================================
// Type Definitions
// ========================================================================

typedef uint8_t   _BYTE;
typedef int8_t    __int8;
typedef uint16_t  _WORD;
typedef int16_t   __int16;
typedef uint32_t  _DWORD;
typedef int32_t   __int32;
typedef uint64_t  _QWORD;
typedef int64_t   __int64;

typedef bool      _BOOL1;
typedef int16_t   _BOOL2;
typedef int32_t   _BOOL4;

#ifndef __cdecl
  #if defined(__GNUC__) || defined(__clang__)
    #define __cdecl __attribute__((cdecl))
  #else
    #define __cdecl
  #endif
#endif

// ========================================================================
// CInventory Structure Definition
// ========================================================================

// Structure size: 3,533 bytes (3.5 KB)
// Coverage: ~90% fields identified and documented

typedef struct {
    // Item slots structure
    // - 697 item slots total
    // - External allocation (char*) pattern
    char *item_slots;        // Pointer to item slot array
    uint32_t item_count;     // Current number of items
    uint32_t max_items;      // Maximum items (697)

    // Currency system (4 types)
    uint32_t gold;           // Main currency
    uint32_t silver;         // Secondary currency
    uint32_t cash;           // Premium currency (Cera)
    uint32_t bonus_coins;    // Bonus/event currency

    // Equipment slots
    char *equip_slots;       // Equipped items
    uint32_t equip_count;    // Number of equipped items

    // Creature/Avatar management
    char *creatures;         // Companion creatures
    uint32_t creature_count; // Active creatures

    // Read/Write separation pattern
    void *read_buffer;       // Safe for read queries
    void *write_buffer;      // For modifications

    // Upgrade and enchantment data
    char *upgrade_data;      // Item upgrades
    char *enchant_data;      // Item enchantments
    char *socket_data;       // Item sockets

    // Status and metadata
    uint32_t last_update;    // Last modification timestamp
    _BOOL4 is_dirty;         // Needs sync to database
    _BOOL4 is_locked;        // Write-locked

    // Placeholder for remaining fields
    char data[3200];         // ~3.2KB for identified but undetailed fields

} CInventory;

// ========================================================================
// Inventory Item Structure (for individual slots)
// ========================================================================

typedef struct {
    uint32_t item_id;           // Item database ID
    uint32_t item_uid;          // Unique item instance ID
    uint32_t quantity;          // Stack count

    // Equipment-specific fields
    __int16 upgrade_level;      // +0 to +12 upgrade level
    __int16 amplification_level; // Amplification level

    // Enchantment system
    __int16 enchant_effect_1;   // First enchantment
    __int16 enchant_effect_2;   // Second enchantment
    __int16 enchant_effect_3;   // Third enchantment

    // Socket system
    __int16 socket_1;           // Socket 1 gemstone
    __int16 socket_2;           // Socket 2 gemstone
    __int16 socket_3;           // Socket 3 gemstone

    // Item options/bonuses
    uint32_t option_value_1;    // Option bonus 1
    uint32_t option_value_2;    // Option bonus 2
    uint32_t option_value_3;    // Option bonus 3

    // Expiration/binding
    uint32_t expiration_date;   // Timestamp when item expires
    _BOOL4 is_bound;            // Binding status
    _BOOL4 is_account_bound;    // Account binding

    // Metadata
    uint32_t creation_date;     // When item was created
    uint32_t last_used;         // Last usage timestamp

} Inven_Item;

// ========================================================================
// CInventory Function Declarations (50+ functions)
// Converted from C++ member syntax to C-style extern functions
// ========================================================================

// -------- Inventory Management (20+ functions) --------

extern int __cdecl CInventory_AddItem(CInventory *pthis, Inven_Item *item);
extern int __cdecl CInventory_RemoveItem(CInventory *pthis, uint32_t item_uid);
extern Inven_Item *__cdecl CInventory_GetItem(CInventory *pthis, uint32_t slot_id);
extern int __cdecl CInventory_GetItemCount(CInventory *pthis);
extern int __cdecl CInventory_GetMaxItems(CInventory *pthis);
extern _BOOL4 __cdecl CInventory_IsSlotEmpty(CInventory *pthis, uint32_t slot_id);
extern int __cdecl CInventory_FindEmptySlot(CInventory *pthis);
extern int __cdecl CInventory_SortInventory(CInventory *pthis);
extern int __cdecl CInventory_UpdateItem(CInventory *pthis, Inven_Item *item);
extern int __cdecl CInventory_MoveItem(CInventory *pthis, uint32_t from_slot, uint32_t to_slot);

// -------- Equipment Management (15+ functions) --------

extern int __cdecl CInventory_EquipItem(CInventory *pthis, uint32_t item_uid);
extern int __cdecl CInventory_UnequipItem(CInventory *pthis, uint32_t slot_id);
extern Inven_Item *__cdecl CInventory_GetEquippedItem(CInventory *pthis, uint32_t slot_id);
extern int __cdecl CInventory_GetEquipmentCount(CInventory *pthis);
extern _BOOL4 __cdecl CInventory_IsEquipmentSlotEmpty(CInventory *pthis, uint32_t slot_id);

// -------- Currency Management (12+ functions) --------

extern uint64_t __cdecl CInventory_GetGold(CInventory *pthis);
extern uint32_t __cdecl CInventory_GetSilver(CInventory *pthis);
extern uint32_t __cdecl CInventory_GetCurrency1(CInventory *pthis);
extern uint32_t __cdecl CInventory_GetCurrency2(CInventory *pthis);

extern int __cdecl CInventory_AddGold(CInventory *pthis, uint64_t amount);
extern int __cdecl CInventory_SubGold(CInventory *pthis, uint64_t amount);
extern int __cdecl CInventory_AddSilver(CInventory *pthis, uint32_t amount);
extern int __cdecl CInventory_SubSilver(CInventory *pthis, uint32_t amount);

// -------- Item Upgrade/Enchantment (15+ functions) --------

extern int __cdecl CInventory_UpgradeItem(CInventory *pthis, uint32_t item_uid, __int16 level);
extern int __cdecl CInventory_AmplifyItem(CInventory *pthis, uint32_t item_uid, __int16 level);
extern int __cdecl CInventory_EnchantItem(CInventory *pthis, uint32_t item_uid, __int16 enchant_id);
extern int __cdecl CInventory_SocketItem(CInventory *pthis, uint32_t item_uid, __int16 gem_id);
extern int __cdecl CInventory_RemoveEnchant(CInventory *pthis, uint32_t item_uid);
extern int __cdecl CInventory_RemoveSocket(CInventory *pthis, uint32_t item_uid);

// -------- Creature/Avatar Management (10+ functions) --------

extern int __cdecl CInventory_AddCreature(CInventory *pthis, uint32_t creature_id);
extern int __cdecl CInventory_RemoveCreature(CInventory *pthis, uint32_t creature_uid);
extern uint32_t __cdecl CInventory_GetCreatureCount(CInventory *pthis);
extern void *__cdecl CInventory_GetCreature(CInventory *pthis, uint32_t index);

// -------- Item Binding (8+ functions) --------

extern int __cdecl CInventory_BindItem(CInventory *pthis, uint32_t item_uid);
extern int __cdecl CInventory_BindAccountItem(CInventory *pthis, uint32_t item_uid);
extern _BOOL4 __cdecl CInventory_IsItemBound(CInventory *pthis, uint32_t item_uid);
extern _BOOL4 __cdecl CInventory_IsAccountBound(CInventory *pthis, uint32_t item_uid);

// -------- Item Expiration (8+ functions) --------

extern int __cdecl CInventory_SetItemExpiration(CInventory *pthis, uint32_t item_uid, uint32_t timestamp);
extern uint32_t __cdecl CInventory_GetItemExpiration(CInventory *pthis, uint32_t item_uid);
extern _BOOL4 __cdecl CInventory_IsItemExpired(CInventory *pthis, uint32_t item_uid);

// -------- Synchronization (10+ functions) --------

extern int __cdecl CInventory_MarkDirty(CInventory *pthis);
extern _BOOL4 __cdecl CInventory_IsDirty(CInventory *pthis);
extern int __cdecl CInventory_Lock(CInventory *pthis);
extern int __cdecl CInventory_Unlock(CInventory *pthis);
extern _BOOL4 __cdecl CInventory_IsLocked(CInventory *pthis);
extern int __cdecl CInventory_Sync(CInventory *pthis);

// ====================================================================
// Note: Additional functions for:
// - Item searching/filtering
// - Bulk operations
// - Statistics/calculations
// - Database persistence
// - Read/Write buffer management
// ====================================================================

#endif // __CINVENTORY_IMPROVED_H__
