#ifndef CHARACTER_CHARACTER_REWARDS_H
#define CHARACTER_CHARACTER_REWARDS_H



// User reward data
// Confidence: 70%
// Error count: 1,140
// Tracks reward claims per user/character
struct RewardUserData {
    int userId;         // Account ID (m_id)
    int characterId;    // Character ID (charac_no)
    int rewardType;     // 0=level, 1=attendance, 2=event, 3=achievement
    int rewardValue;    // Reward amount or item ID
    bool claimed;       // Whether reward has been claimed
    time_t claimTime;   // Timestamp of claim
    int eventId;        // Source event ID (0 if non-event)
    /* ORM: event_reward_log table */
};

// Character creation creature gift
// Confidence: 50%
// Error count: 48
// Starter pet gift based on job class
struct CharacterCreateCreatureGiftScript {
    int jobId;              // Job class ID (0-15)
    int creatureId;         // Starter creature ID
    char scriptPath[256];   // PVF script path (256 matches DNF path limits)
    int duration;           // Gift duration in days (0=permanent)
    /* Loaded from character/create_creature_gift.lst */
};

#endif // CHARACTER_CHARACTER_REWARDS_H
