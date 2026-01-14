/**
 * Events Module - Public Interface
 * Purpose: Game event system, scheduling, and event management
 *
 * Components:
 * - Event creation and dispatching
 * - Event scheduling and timing
 * - Event listener registration
 * - Event queue management
 */

#ifndef SRC_EVENTS_EVENTS_INTERFACE_H_
#define SRC_EVENTS_EVENTS_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Event types in the game system
 */
typedef enum {
    EVENT_TYPE_LOGIN = 1,
    EVENT_TYPE_LOGOUT = 2,
    EVENT_TYPE_LEVEL_UP = 3,
    EVENT_TYPE_ITEM_OBTAINED = 4,
    EVENT_TYPE_ITEM_LOST = 5,
    EVENT_TYPE_DUNGEON_ENTER = 6,
    EVENT_TYPE_DUNGEON_CLEAR = 7,
    EVENT_TYPE_DUNGEON_FAIL = 8,
    EVENT_TYPE_SKILL_LEARNED = 9,
    EVENT_TYPE_SKILL_UPGRADED = 10,
    EVENT_TYPE_QUEST_STARTED = 11,
    EVENT_TYPE_QUEST_COMPLETED = 12,
    EVENT_TYPE_QUEST_FAILED = 13,
    EVENT_TYPE_PARTY_FORMED = 14,
    EVENT_TYPE_PARTY_DISBANDED = 15,
    EVENT_TYPE_GUILD_JOINED = 16,
    EVENT_TYPE_GUILD_LEFT = 17,
    EVENT_TYPE_PVP_BATTLE = 18,
    EVENT_TYPE_PVP_WIN = 19,
    EVENT_TYPE_PVP_LOSE = 20,
    EVENT_TYPE_ATTENDANCE = 21,
    EVENT_TYPE_DAILY_REWARD = 22,
    EVENT_TYPE_SPECIAL_EVENT = 23,
    EVENT_TYPE_SYSTEM = 24,
    EVENT_TYPE_CUSTOM = 25
} EventType;

/**
 * Event priority levels
 */
typedef enum {
    EVENT_PRIORITY_LOW = 0,
    EVENT_PRIORITY_NORMAL = 1,
    EVENT_PRIORITY_HIGH = 2,
    EVENT_PRIORITY_CRITICAL = 3
} EventPriority;

/**
 * Event structure
 */
typedef struct {
    unsigned int event_id;       // Unique event identifier
    EventType event_type;        // Type of event
    EventPriority priority;      // Priority level
    unsigned int creator_id;     // ID of entity that created event
    unsigned int target_id;      // ID of target entity
    unsigned int timestamp;      // Event creation timestamp
    unsigned int scheduled_time; // When event should fire (0 = immediate)
    void* event_data;            // Pointer to event-specific data
    int is_processed;            // Processing status flag
} GameEvent;

/**
 * Event listener callback function signature
 *
 * Parameters:
 *   event: The event being triggered
 *   context: Context pointer passed during listener registration
 */
typedef void (*EventListener)(GameEvent* event, void* context);

/**
 * Initialize Events Module
 *
 * Sets up event system, creates event queues
 *
 * Parameters:
 *   max_pending_events: Maximum events to hold in queue
 *   max_listeners: Maximum event listeners
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Events_Initialize(int max_pending_events, int max_listeners);

/**
 * Cleanup Events Module
 *
 * Processes remaining events, frees resources
 */
void Events_Cleanup();

/**
 * Create and Queue Event
 *
 * Creates a new event and adds it to processing queue
 *
 * Parameters:
 *   event_type: Type of event to create
 *   creator_id: ID of event creator
 *   target_id: ID of event target
 *   priority: Event priority level
 *   event_data: Pointer to event-specific data
 *
 * Returns: Event ID on success, negative error code on failure
 */
int Events_CreateEvent(EventType event_type,
                       unsigned int creator_id,
                       unsigned int target_id,
                       EventPriority priority,
                       void* event_data);

/**
 * Schedule Event for Later Execution
 *
 * Creates event scheduled for specific time
 *
 * Parameters:
 *   event_type: Type of event
 *   creator_id: Event creator ID
 *   target_id: Event target ID
 *   priority: Priority level
 *   scheduled_time: Timestamp when event should fire
 *   event_data: Event-specific data
 *
 * Returns: Scheduled event ID, or negative error code
 */
int Events_ScheduleEvent(EventType event_type,
                         unsigned int creator_id,
                         unsigned int target_id,
                         EventPriority priority,
                         unsigned int scheduled_time,
                         void* event_data);

/**
 * Register Event Listener
 *
 * Registers a callback function for specific event type
 *
 * Parameters:
 *   event_type: Event type to listen for
 *   listener: Callback function
 *   context: Context pointer to pass to callback
 *
 * Returns: Listener ID, or negative error code on failure
 */
int Events_RegisterListener(EventType event_type,
                            EventListener listener,
                            void* context);

/**
 * Unregister Event Listener
 *
 * Removes a registered event listener
 *
 * Parameters:
 *   listener_id: Listener ID returned from Events_RegisterListener
 *
 * Returns: 0 on success, error code on failure
 */
int Events_UnregisterListener(int listener_id);

/**
 * Process Pending Events
 *
 * Processes events in queue and triggers registered listeners
 *
 * Parameters:
 *   max_events_to_process: Maximum events to process in one call
 *
 * Returns: Number of events processed
 */
int Events_ProcessPending(int max_events_to_process);

/**
 * Get Pending Event Count
 *
 * Returns number of events waiting in queue
 */
int Events_GetPendingCount();

/**
 * Cancel Scheduled Event
 *
 * Cancels a previously scheduled event
 *
 * Parameters:
 *   event_id: ID of event to cancel
 *
 * Returns: 0 on success, error code on failure
 */
int Events_CancelEvent(unsigned int event_id);

/**
 * Get Event by ID
 *
 * Retrieves event details from queue
 *
 * Parameters:
 *   event_id: Event ID to retrieve
 *
 * Returns: Pointer to GameEvent, or NULL if not found
 */
GameEvent* Events_GetEventByID(unsigned int event_id);

/**
 * Flush All Events
 *
 * Processes all pending events immediately
 *
 * Returns: Number of events flushed
 */
int Events_FlushAll();

/**
 * Enable/Disable Event Processing
 *
 * Pauses or resumes event processing
 *
 * Parameters:
 *   enabled: 1 to enable, 0 to disable
 *
 * Returns: 0 on success
 */
int Events_SetProcessingEnabled(int enabled);

/**
 * Get Event Processing Statistics
 *
 * Retrieves event system statistics
 *
 * Returns: Pointer to statistics structure
 */
void* Events_GetStatistics();

/**
 * Reset Event System Statistics
 *
 * Clears accumulated statistics
 */
void Events_ResetStatistics();

#endif // SRC_EVENTS_EVENTS_INTERFACE_H_