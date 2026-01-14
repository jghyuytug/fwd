#ifndef GAME_DIMENSION_DIMENSION_TYPES_H
#define GAME_DIMENSION_DIMENSION_TYPES_H



// Dimension coin information
// Confidence: 45%
// Error count: 348
// Currency for Otherverse/dimensional content
struct DimensionCoinInfo {
    int coinType;       // 0=Normal, 1=Ancient, 2=Nihilistic
    int amount;         // Current held amount
    int maxAmount;      // Weekly cap (typically 600-1000)
    int earnRate;       // Coins per boss kill
    int weeklyEarned;   // Tracking for weekly reset
    /* Resets weekly on server maintenance */
};

#endif // GAME_DIMENSION_DIMENSION_TYPES_H
