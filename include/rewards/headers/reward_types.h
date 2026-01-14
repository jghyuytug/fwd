#ifndef REWARDS_REWARD_TYPES_H
#define REWARDS_REWARD_TYPES_H



// Generic reward key
// Confidence: 65%
// Error count: 1,008
// Key for reward table lookups
struct rewardKey {
    int keyType;    // 0=level, 1=quest, 2=event, 3=achievement
    int keyId;      // Source ID (level number, quest ID, etc.)
    int keyValue;   // Additional filter (job class, tier, etc.)
    /* Used as map key: std::map<rewardKey, RewardData> */
};

// Level-up support scripts
// Confidence: 55%
// Error count: 684 each
struct LevelupSupportSciprt {  // Note: Original typo preserved
    int level;              // Target level for reward
    int jobId;              // Job class (0xFF = all)
    char scriptPath[256];   // PVF script path for rewards
    int rewardType;         // 0=items, 1=skills, 2=stats
    /* Loaded from levelup_support.lst */
};

struct STExpertJobScript {
    int expertJobId;        // Expert job class ID
    char scriptPath[256];   // Awakening script path
    int unlockLevel;        // Level required for awakening
    int questRequired;      // Quest ID required (0 = none)
    /* Expert job awakening configuration */
};

struct LevelupSupport2ndScript {
    int level;              // Target level
    int jobId;              // Job class
    char scriptPath[256];   // 2nd awakening script
    int tier;               // Tier level (0=base, 1=2nd, 2=neo)
    /* 2nd awakening level support rewards */
};

// Seria blessing script
// Confidence: 45%
// Error count: 60
// Seria's blessing buff configuration
struct stSeriaBlessingScript {
    int blessingLevel;      // Blessing tier (1-10)
    char scriptPath[256];   // Buff effect script
    int duration;           // Duration in seconds
    int expBonus;           // EXP bonus percentage
    /* NPC Seria's buff for new/returning players */
};

#endif // REWARDS_REWARD_TYPES_H
