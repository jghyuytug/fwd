#ifndef GAME_DUNGEON_DUNGEON_TYPES_H
#define GAME_DUNGEON_DUNGEON_TYPES_H

#include <ctime>

// Boss dungeon tower log
// Confidence: 75% - Common logging pattern
// Error count: 1,128
// Tower climb progress tracking per character
struct STBossDungeonTowerLog {
    int characterId;        // Character ID (charac_no)
    int towerId;            // Tower instance ID
    int floorReached;       // Current/highest floor
    int clearTime;          // Best clear time (seconds)
    int attempts;           // Total attempts this week
    int bestScore;          // Highest score achieved
    time_t lastAttemptTime; // Last attempt timestamp
    /* ORM: tower_progress table - weekly reset */
};

// Death tower structure
// Confidence: 50% - Speculative based on naming
// Error count: 336
// Despair/Death tower progression state
struct stDeathTower_t {
    int towerId;        // Tower ID
    int currentFloor;   // Current floor in progress
    int highestFloor;   // Personal best floor
    int lives;          // Remaining lives (0 = failed)
    int weeklyEntries;  // Entries this week (limited)
    int buffStacks;     // Accumulated buff stacks
    /* Roguelike tower mode - reset on death */
};

#endif // GAME_DUNGEON_DUNGEON_TYPES_H
