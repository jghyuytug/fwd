#ifndef COMMON_QUICKPARTY_NAMESPACE_H
#define COMMON_QUICKPARTY_NAMESPACE_H

// QuickParty namespace - Party matchmaking system
// Based on error analysis
// Top members: STQuickPartyPoolMap_Key (3,456), RandomBuffType (3,336), LevelDataHell (240), LevelData (240)

namespace QuickParty {

    // ========================================
    // Pool Map Key
    // ========================================

    // Quick party pool map key structure
    // Error count: 3,456 errors (10% of member errors!)
    // Used as map key for matchmaking pools
    struct STQuickPartyPoolMap_Key {
        int dungeon_id;         // Target dungeon ID
        int difficulty;         // Difficulty level (0=Normal, 1=Expert, 2=Master, 3=King)
        int level_min;          // Minimum player level
        int level_max;          // Maximum player level
        int region;             // Server region for matchmaking
        /* Key comparison: ordered by (dungeon_id, difficulty, level_range) */
    };

    // ========================================
    // Random Buff Type
    // ========================================

    // Random buff type enumeration
    // Error count: 3,336 errors (10% of member errors!)
    // Applied randomly on dungeon entry via quick party
    enum RandomBuffType {
        RANDOM_BUFF_NONE = 0,
        RANDOM_BUFF_ATK_UP = 1,         // Attack power +10%
        RANDOM_BUFF_DEF_UP = 2,         // Defense +10%
        RANDOM_BUFF_SPEED_UP = 3,       // Movement speed +15%
        RANDOM_BUFF_CRIT_UP = 4,        // Critical rate +5%
        RANDOM_BUFF_EXP_UP = 5,         // Experience +20%
        RANDOM_BUFF_DROP_UP = 6,        // Drop rate +10%
        RANDOM_BUFF_HP_REGEN = 7,       // HP regen +5/sec
        RANDOM_BUFF_MP_REGEN = 8,       // MP regen +3/sec
        RANDOM_BUFF_MAX = 9
        /* Values speculative based on common MMORPG patterns */
    };

    // ========================================
    // Level Data
    // ========================================

    // Normal level data
    // Error count: 240 errors
    // Level requirements for normal difficulty dungeons
    struct LevelData {
        int min_level;          // Minimum character level
        int max_level;          // Maximum character level (0=unlimited)
        int recommended_level;  // Recommended level for dungeon
        int fatigue_cost;       // Fatigue points consumed per entry
        /* Used by matchmaking to filter appropriate players */
    };

    // Hell mode level data
    // Error count: 240 errors
    // Extended level requirements for hell mode dungeons
    struct LevelDataHell {
        int min_level;          // Minimum character level (typically 70+)
        int max_level;          // Maximum character level
        int required_item;      // Hell orb or entry item required
        int required_item_count;// Number of entry items consumed
        int demon_invitation;   // Demon invitation required (1=yes, 0=no)
        int party_size_min;     // Minimum party members (typically 2+)
        /* Hell mode has stricter entry requirements */
    };

    // ========================================
    // Quick Party Manager
    // ========================================

    // Quick party system manager (likely already defined in game/party/quickparty.h)
    class CQuickPartySystemManager;

} // namespace QuickParty

#endif // COMMON_QUICKPARTY_NAMESPACE_H
