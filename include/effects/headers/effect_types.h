#ifndef EFFECTS_EFFECT_TYPES_H
#define EFFECTS_EFFECT_TYPES_H

#include "../network/packet_types.h"
#include <ctime>

// Global effect structure
// Confidence: 45%
// Error count: 180
// Server-wide buff/event effect tracking
struct stGlobalEffect_t {
    ENUM_GLOBAL_EFFECT_ARAD effectType; // Effect category (EXP, DROP, etc.)
    int duration;                        // Total duration in seconds
    float value;                         // Multiplier (1.5 = 50% boost)
    time_t startTime;                    // Unix timestamp of activation
    time_t endTime;                      // Unix timestamp of expiration
    int sourceEventId;                   // Event ID that triggered this effect
    /* Used by event system for server-wide bonuses */
};

#endif // EFFECTS_EFFECT_TYPES_H
