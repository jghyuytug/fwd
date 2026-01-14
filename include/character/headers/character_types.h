#ifndef CHARACTER_CHARACTER_TYPES_H
#define CHARACTER_CHARACTER_TYPES_H

// Character manager request result codes
// Confidence: 35% - Speculative based on naming
// REMOVED - Duplicate definition, see character/character_enums.h:9
/* enum ENUM_CHARAC_MANAGER_REQUEST_SUCCESS_IDX {
    CHARAC_REQ_SUCCESS = 0,
    CHARAC_REQ_FAIL_UNKNOWN = 1,
    CHARAC_REQ_FAIL_NOT_FOUND = 2,
    CHARAC_REQ_FAIL_LOCKED = 3,
    // Values 4+ reserved for specific error conditions
}; */

// Level-up condition
// Confidence: 45%
// Error count: 348
// Source: Level progression system, quest-gated advancement
struct Condition_LevelUp {
    int requiredLevel;      // Current level requirement
    int requiredExp;        // Experience points needed
    int questId;            // Quest that must be completed (0 = none)
    /* Used by advancement system for job changes and awakening */
};

#endif // CHARACTER_CHARACTER_TYPES_H
