#ifndef INVENTORY_INVENTORY_TYPES_H
#define INVENTORY_INVENTORY_TYPES_H

#include <map>
#include <ctime>

// Purchase limit tracking
// Confidence: 70%
// Error count: 996
// Source: charac_cerashop_restrict ORM table structure
struct PurchaseLimitItem {
    int itemId;                 // Item template ID
    int maxPurchase;            // Maximum allowed purchases per period
    int currentPurchase;        // Current purchase count
    time_t resetTime;           // When limit resets (daily/weekly/monthly)
    /* Fields verified against charac_cerashop_restrict table */
};

// Gift item set
// Confidence: 70%
// Error count: 996
// Source: Event reward and gift box systems
struct Arad_GiftItem_Set {
    int setId;                  // Gift set ID
    int itemIds[16];            // Item IDs in set (max 16 items per set)
    int itemCounts[16];         // Quantity of each item
    int numItems;               // Actual count of items in set
    /* Array size 16 matches typical gift box limits in DNF */
};

// Level-up reward table key
// Confidence: 55%
// Error count: 552
// Source: Level reward configuration system
struct LevelUpRewardItemTableKey {
    int characterLevel;         // Level threshold for reward
    int jobId;                  // Job class (0 = all jobs)
    int rewardTier;             // Reward tier (0 = base, 1+ = milestone)
    /* Used as map key for level reward lookup */
};

// Map item container
// Confidence: 50%
// Error count: 420
// Source: Generic item collection storage
struct _mapItem {
    std::map<int, int> items;   // itemId -> count mapping
    /* Simple container for item aggregation (rewards, mail, etc.) */
};

// Fatigue reward items
// Confidence: 50%
// Error count: 216
// Source: Fatigue point reward system
struct UsedFatigueRewardItem {
    int itemId;                 // Reward item ID
    int fatigueRequired;        // Fatigue points needed to claim
    bool claimed;               // Whether already claimed today
    /* Resets daily with fatigue system */
};

// Item script data
// Confidence: 50%
// Error count: 144
// Source: Item use scripts from PVF
struct STItemScript {
    int itemId;                 // Item template ID
    char scriptData[256];       // LST script path or inline script
    /* Script size 256 covers typical item use script references */
};

// Limited edition items
// Confidence: 40%
// Error count: 12
// Source: Cash shop limited quantity tracking
class CItemLimitEdition {
public:
    int itemId;                 // Item template ID
    int totalSupply;            // Original supply count
    int remainingSupply;        // Current remaining count
    /* Used for exclusive/numbered items in cash shop */
};

// Creature module type enum
// Confidence: 70% - Used in std::map
// Error count: 540
// REMOVED - Duplicate definition, see inventory/inventory_enums.h:28
/* enum ENUM_MODULE_TYPE {
    MODULE_TYPE_WEAPON = 0,
    MODULE_TYPE_ARMOR = 1,
    MODULE_TYPE_ACCESSORY = 2,
    MODULE_TYPE_SKILL = 3,
}; */

// Slot binding key
// Confidence: 45%
// Error count: 540
// Source: Equipment slot binding system
struct SlotBoundKey {
    int slotType;               // Slot category (equipment, avatar, etc.)
    int slotIndex;              // Slot position within category
    int bindType;               // 0=unbound, 1=character, 2=account
    /* Used for bind-on-equip/bind-on-pickup items */
};

#endif // INVENTORY_INVENTORY_TYPES_H
