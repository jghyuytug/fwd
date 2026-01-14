#ifndef GAME_UPGRADE_UPGRADE_TYPES_H
#define GAME_UPGRADE_UPGRADE_TYPES_H



// Genuine damage upgrade material info
// Confidence: 60%
// Error count: 852
// Materials required for genuine damage upgrade
struct GenuineDamageUpgradeMaterialInfo {
    int materialId;     // Material item ID
    int materialCount;  // Required quantity
    int upgradeLevel;   // Target upgrade level
    int successRate;    // Success percentage (0-100)
    int goldCost;       // Gold cost for attempt
    /* Used by enhancement system for weapon upgrades */
};

// Genuine damage upgrade info
// Confidence: 60%
// Error count: 852
// Current upgrade state for genuine damage system
struct GenuineDamageUpgradeInfo {
    int itemId;         // Equipped item ID
    int currentLevel;   // Current upgrade level (0-10)
    int maxLevel;       // Maximum upgrade level
    int damageIncrease; // Flat damage bonus per level
    int failCount;      // Consecutive failures (for pity system)
    /* ORM: charac_item table - upgrade field */
};

// Investment amplify option
// Confidence: 40%
// Error count: 12
// Amplification investment tracking (Cera-based upgrade)
struct stInvestAmplifyOption_t {
    int optionId;       // Amplification option ID
    int amplifyRate;    // Amplification multiplier (100 = 1.0x)
    int investAmount;   // Cera invested
    int tier;           // Current tier (determines success rate)
    /* Premium upgrade system using real-money currency */
};

#endif // GAME_UPGRADE_UPGRADE_TYPES_H
