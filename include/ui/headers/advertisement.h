#ifndef UI_ADVERTISEMENT_H
#define UI_ADVERTISEMENT_H

#include <ctime>

// Advertisement display info
// Confidence: 60%
// Error count: 1,056
// In-game advertisement tracking
class CAdvertisementExposeInfo {
public:
    int adId;               // Advertisement ID
    int exposureType;       // 0=login, 1=channel, 2=dungeon, 3=shop
    time_t startTime;       // Campaign start time
    time_t endTime;         // Campaign end time
    char imageUrl[256];     // Image resource URL
    int priority;           // Display priority (higher = shown first)
    int targetLevel;        // Minimum player level to show
    int clickCount;         // Click tracking counter

    void recordExposure(int userId);  // Log ad exposure
    void recordClick(int userId);     // Log ad click
    /* Analytics integration for marketing campaigns */
};

#endif // UI_ADVERTISEMENT_H
