#ifndef GAME_ALTAR_ALTAR_TYPES_H
#define GAME_ALTAR_ALTAR_TYPES_H

#include <ctime>

// Forward declarations
class CUser;

// Advance Altar entrance data
// Confidence: 80% - Clear usage pattern
// Error count: 1,248
// Altar dungeon entry requirements
struct STAdvanceAltarEntranceData {
    int altarId;        // Altar instance ID
    int requiredLevel;  // Minimum character level
    int entryFee;       // Gold/item cost to enter
    int maxPartySize;   // Maximum party members (1-4)
    time_t openTime;    // Daily open time (server time)
    time_t closeTime;   // Daily close time
    int weeklyLimit;    // Weekly entry limit
    /* Altar system - endgame content with daily windows */
};

// Advance Altar layer statistics
// Confidence: 80% - Clear usage pattern
// Error count: 1,248
// Per-layer statistics for altar progression
struct STAdvanceAltarLayerStatData {
    int layerId;        // Layer/floor ID
    int completedCount; // Times this layer completed
    int bestTime;       // Best clear time (seconds)
    int bestScore;      // Best score achieved
    int clearCount;     // Total clears this season
    int rank;           // Server rank for this layer
    /* Leaderboard integration for altar layers */
};

#endif // GAME_ALTAR_ALTAR_TYPES_H
