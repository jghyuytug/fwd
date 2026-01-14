#ifndef EVENTS_EVENT_BASE_H
#define EVENTS_EVENT_BASE_H



// Forward declaration
class CUser;

// Base event class (abstract)
// Confidence: 70%
class CEventBase {
public:
    virtual ~CEventBase() = 0;
    virtual int StartEvent() = 0;
    virtual int EndEvent() = 0;
};

// Event classification
// Confidence: 70%
// Error count: 1,452
// Event categorization and management
class EventClassify {
public:
    int eventId;        // Event unique ID
    int eventType;      // Event category (bingo, attendance, etc.)
    int priority;       // Display/execution priority
    int serverGroup;    // Target server group
    int status;         // 0=inactive, 1=active, 2=ended
    /* Contains CEventScriptMng and CEventActionMng submanagers */
};

#endif // EVENTS_EVENT_BASE_H
