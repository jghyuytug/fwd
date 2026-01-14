/**
 * @file event_bus.h
 * @brief Event bus for publish/subscribe pattern
 *
 * Decoupled event system for game logic communication.
 */

#ifndef ENGINE_CORE_EVENT_EVENT_BUS_H
#define ENGINE_CORE_EVENT_EVENT_BUS_H

#include "../types.h"
#include "event_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_BUS_MAX_HANDLERS     64
#define EVENT_BUS_MAX_QUEUED       1024
#define EVENT_BUS_MAX_DATA_SIZE    256

/* Forward declaration */
struct EventBus;
typedef struct EventBus EventBus;

/**
 * Event handler callback
 * @param bus       Event bus instance
 * @param event     Event data
 * @param user_data User context
 */
typedef void (*EventHandler)(EventBus* bus, const EventBase* event,
                             void* user_data);

/* Handler entry */
typedef struct {
    EventHandler handler;
    void*        user_data;
    EventType    event_type;    /* EVENT_NONE = all events */
    EventCategory category;     /* EVENT_CATEGORY_NONE = all categories */
    u8           active;
    u8           once;          /* Remove after first call */
} EventHandlerEntry;

/* Queued event */
typedef struct {
    EventType type;
    u32       timestamp;
    u32       source_id;
    u32       target_id;
    u8        data[EVENT_BUS_MAX_DATA_SIZE];
    u32       data_size;
} QueuedEvent;

/* Event bus structure */
struct EventBus {
    EventHandlerEntry handlers[EVENT_BUS_MAX_HANDLERS];
    int               handler_count;
    QueuedEvent       queue[EVENT_BUS_MAX_QUEUED];
    int               queue_head;
    int               queue_tail;
    int               queue_size;
    u8                processing;     /* Prevent recursive dispatch */
    /* Statistics */
    u64               events_published;
    u64               events_dispatched;
    u64               events_dropped;
};

/**
 * Initialize event bus
 * @param bus Event bus instance
 * @return ENGINE_OK on success
 */
int event_bus_init(EventBus* bus);

/**
 * Cleanup event bus
 * @param bus Event bus instance
 */
void event_bus_cleanup(EventBus* bus);

/**
 * Subscribe to events
 * @param bus       Event bus instance
 * @param type      Event type to subscribe (EVENT_NONE = all)
 * @param handler   Handler callback
 * @param user_data User context
 * @return Handler ID (>=0) on success, negative error code on failure
 */
int event_subscribe(EventBus* bus, EventType type,
                    EventHandler handler, void* user_data);

/**
 * Subscribe to event category
 * @param bus       Event bus instance
 * @param category  Event category to subscribe
 * @param handler   Handler callback
 * @param user_data User context
 * @return Handler ID (>=0) on success, negative error code on failure
 */
int event_subscribe_category(EventBus* bus, EventCategory category,
                             EventHandler handler, void* user_data);

/**
 * Subscribe for one-time event
 * @param bus       Event bus instance
 * @param type      Event type to subscribe
 * @param handler   Handler callback
 * @param user_data User context
 * @return Handler ID (>=0) on success, negative error code on failure
 */
int event_subscribe_once(EventBus* bus, EventType type,
                         EventHandler handler, void* user_data);

/**
 * Unsubscribe handler by ID
 * @param bus        Event bus instance
 * @param handler_id Handler ID from subscribe
 * @return ENGINE_OK on success
 */
int event_unsubscribe(EventBus* bus, int handler_id);

/**
 * Unsubscribe all handlers for a type
 * @param bus  Event bus instance
 * @param type Event type
 * @return Number of handlers removed
 */
int event_unsubscribe_type(EventBus* bus, EventType type);

/**
 * Publish event immediately (synchronous)
 * @param bus   Event bus instance
 * @param event Event data (must have type, timestamp filled)
 * @return Number of handlers called
 */
int event_publish(EventBus* bus, const EventBase* event);

/**
 * Publish simple event (no extra data)
 * @param bus       Event bus instance
 * @param type      Event type
 * @param source_id Source entity ID
 * @param target_id Target entity ID
 * @return Number of handlers called
 */
int event_publish_simple(EventBus* bus, EventType type,
                         u32 source_id, u32 target_id);

/**
 * Queue event for deferred dispatch
 * @param bus       Event bus instance
 * @param event     Event data
 * @param data      Extra event data (optional)
 * @param data_size Extra data size
 * @return ENGINE_OK on success
 */
int event_queue(EventBus* bus, const EventBase* event,
                const void* data, u32 data_size);

/**
 * Queue simple event
 * @param bus       Event bus instance
 * @param type      Event type
 * @param source_id Source entity ID
 * @param target_id Target entity ID
 * @return ENGINE_OK on success
 */
int event_queue_simple(EventBus* bus, EventType type,
                       u32 source_id, u32 target_id);

/**
 * Process queued events
 * @param bus       Event bus instance
 * @param max_count Maximum events to process (0 = all)
 * @return Number of events processed
 */
int event_dispatch_queued(EventBus* bus, int max_count);

/**
 * Get number of queued events
 * @param bus Event bus instance
 * @return Number of queued events
 */
int event_queue_count(const EventBus* bus);

/**
 * Clear all queued events
 * @param bus Event bus instance
 */
void event_queue_clear(EventBus* bus);

/**
 * Get statistics
 * @param bus              Event bus instance
 * @param published        Output: total published
 * @param dispatched       Output: total dispatched
 * @param dropped          Output: total dropped
 * @param handler_count    Output: current handler count
 */
void event_get_stats(const EventBus* bus,
                     u64* published, u64* dispatched,
                     u64* dropped, int* handler_count);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_EVENT_EVENT_BUS_H */
