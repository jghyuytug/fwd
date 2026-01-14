#ifndef EVENTS_EVENT_SCRIPTS_H
#define EVENTS_EVENT_SCRIPTS_H



// Event script data
// Confidence: 65%
// Error count: 744
// Inline script for event logic
struct InGameEventScript {
    int scriptId;           // Script unique ID
    int eventId;            // Parent event ID
    char scriptData[1024];  // LST script content (1KB typical max)
    /* 1024 bytes matches PVF inline script limits */
};

// Upgrade jar event NPCs
// Confidence: 45%
// Error count: 120
// NPC configuration for jar event
struct ST_Event_Upgrade_Jar_EventNPCs_Script {
    int npcId;              // NPC ID in town
    int jarLevel;           // Jar tier (1-10)
    char scriptPath[256];   // NPC dialog script
    int rewardMultiplier;   // Reward scale (100 = 1.0x)
    int goldCost;           // Gold cost per upgrade attempt
    /* Jar event - upgrade jar for random rewards */
};

#endif // EVENTS_EVENT_SCRIPTS_H
