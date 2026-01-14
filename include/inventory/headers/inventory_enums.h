#ifndef INVENTORY_INVENTORY_ENUMS_H
#define INVENTORY_INVENTORY_ENUMS_H

// ENUM_INVEN_TYPE - Inventory type enumeration
// Confidence: 40% (no direct evidence found, speculative based on naming)
// Evidence: None found in grep results
// Impact: 1,236 errors
// Note: Common inventory types in DNF game
enum ENUM_INVEN_TYPE {
    INVEN_TYPE_EQUIPMENT = 0,       // Equipped items (armor, weapons)
    INVEN_TYPE_INVENTORY = 1,       // Main inventory (consumables, materials)
    INVEN_TYPE_WAREHOUSE = 2,       // Account warehouse (shared storage)
    INVEN_TYPE_AVATAR = 3,          // Avatar inventory (cosmetic items)
    INVEN_TYPE_CREATURE = 4,        // Creature inventory (pets, companions)
    INVEN_TYPE_PREMIUM = 5,         // Premium inventory (extended storage)
    INVEN_TYPE_QUEST = 6,           // Quest item inventory

    // Values speculative based on DNF inventory patterns
    // Extend as discovered from packet analysis
};

// ENUM_MODULE_TYPE - Creature module types (equipment/attachments for creatures)
// Confidence: 70% (found STL map usage with float values)
// Evidence: df_game_r_part06.c.bak_error:84537-85328, df_game_r_decl.h.backup:1009-1042
// Impact: 1,176 errors
// Used as: std::map<ENUM_MODULE_TYPE, float> g_creatureModuleDamageRateMap
// Also: std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>
enum ENUM_MODULE_TYPE {
    // Speculative values based on DNF creature system
    MODULE_TYPE_UNKNOWN = 0,
    MODULE_TYPE_WEAPON = 1,         // Creature weapon module
    MODULE_TYPE_ARMOR = 2,          // Creature armor module
    MODULE_TYPE_ACCESSORY = 3,      // Creature accessory module
    MODULE_TYPE_SKILL = 4,          // Creature skill module
    MODULE_TYPE_AURA = 5,           // Creature aura module

    // Values based on creature equipment system analysis
    // Maps to damage rate multipliers in g_creatureModuleDamageRateMap
};

#endif // INVENTORY_INVENTORY_ENUMS_H
