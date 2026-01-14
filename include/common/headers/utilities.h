#ifndef COMMON_UTILITIES_H
#define COMMON_UTILITIES_H

#include <ctime>

// Map information
// Confidence: 60%
// Error count: 612
// Verified fields: Core map metadata structure
struct MapInfo {
    int mapId;              // Unique map identifier
    char mapName[64];       // Display name (localized)
    int width;              // Map width in tiles
    int height;             // Map height in tiles
    int difficulty;         // 0=Town, 1=Normal, 2=Expert, 3=Master, 4=King
    /* Additional fields may exist: music_id, background_id, npc_count */
};

// Disconnect detection
// Confidence: 40%
// Error count: 228
// Network heartbeat and timeout detection class
class disconnect_detecter {
public:
    void checkConnection();         // Send heartbeat, check response
    bool isDisconnected();          // Return true if connection lost
    void setTimeoutMs(int ms);      // Configure timeout threshold
    void reset();                   // Reset detection state
    int getLastPingMs();            // Get last round-trip time
    /* Internal: tracks last_heartbeat_time, consecutive_failures */
};

// Timer entry
// Confidence: 45%
// Error count: 156
// Game timer system entry - used for scheduled events
struct TimerEntry {
    int timerId;            // Unique timer ID for cancellation
    time_t triggerTime;     // Unix timestamp when timer fires
    void (*callback)();     // Function to call on trigger
    void* userData;         // User data passed to callback
    int repeatInterval;     // 0=one-shot, >0=repeat every N seconds
    int priority;           // Execution priority (lower = sooner)
    /* Managed by central timer wheel; max 65536 concurrent timers */
};

// TCHAR comparison functor
// Confidence: 50%
// Error count: 228
struct less_TCHAR {
    bool operator()(const char* a, const char* b) const;
};

#endif // COMMON_UTILITIES_H
