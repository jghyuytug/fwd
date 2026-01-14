#ifndef EVENTS_EVENT_TYPES_H
#define EVENTS_EVENT_TYPES_H

#include "event_base.h"
#include <ctime>

// Forward declaration
class CUser;

// Bingo event
// Confidence: 80% - Complete interface found in Phase 2
// Error count: 1,080
// Evidence: part01.c:68755, decl.h:1944-1956
class BingoEvent : public CEventBase {
public:
    BingoEvent();
    virtual ~BingoEvent();

    // Event lifecycle
    int StartEvent();
    static int StartAction();
    int EndEvent();
    int dailyresetData();

    // Bingo system
    char* getBingoSystem();
    int reqBingoMark(int mark);
    int reqBingoQuizMark(CUser *user, int mark);

    // Rewards
    void sendOneMatchLineReward(CUser *user);
    void sendMaxMatchLineReward(CUser *user);

private:
    // Member variables (speculative):
    int bingoBoard[25];     // 5x5 bingo grid (marked=1, unmarked=0)
    int matchedLines;       // Number of completed lines
    int maxLines;           // Maximum possible lines (12)
    /* Full layout requires event handler analysis */
};

// In-game event info
// Confidence: 70%
// Error count: 912
// Active event metadata
struct InGameEventInfo {
    int eventId;        // Event unique ID
    time_t startTime;   // Event start timestamp
    time_t endTime;     // Event end timestamp
    int eventType;      // 0=attendance, 1=bingo, 2=collect, 3=dungeon
    int priority;       // Display priority in event list
    int serverGroup;    // Affected server group (0xFF = all)
    /* ORM: taiwan_game_event.event_info table */
};

// Growth creature event
// Confidence: 50%
// Error count: 216
// Creature growth event tracking
struct GrowthCreatureEvent {
    int eventId;        // Parent event ID
    int creatureId;     // Target creature type
    int growthStage;    // Current growth stage (0-5)
    int rewardTier;     // Reward tier for stage completion
    int feedCount;      // Feed items consumed
    /* Limited-time creature raising events */
};

#endif // EVENTS_EVENT_TYPES_H
