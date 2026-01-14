/**
 * Events Module - Initialization and Management
 *
 * Handles event creation, queuing, scheduling, and listener management
 */

#include "events_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Event listener entry */
typedef struct {
    int listener_id;
    EventType event_type;
    EventListener callback;
    void* context;
    int is_active;
} EventListenerEntry;

/* Events module global state */
static struct {
    int initialized;
    unsigned int max_pending_events;  // Changed to unsigned
    int max_listeners;

    // Event queue management
    GameEvent* event_queue;
    int queue_head;
    int queue_tail;
    unsigned int queue_count;  // Changed to unsigned to match stats type

    // Listener management
    EventListenerEntry* listeners;
    int next_listener_id;

    // Processing control
    int processing_enabled;

    // Statistics tracking
    struct {
        unsigned int total_events_created;
        unsigned int total_events_processed;
        unsigned int total_listeners_registered;
        unsigned int max_queue_depth;
        unsigned int scheduled_events_count;
        unsigned int listeners_triggered;
    } stats;
} g_events_state = {0};

/**
 * Initialize Events Module
 */
int Events_Initialize(int max_pending_events, int max_listeners)
{
    if (g_events_state.initialized) {
        fprintf(stderr, "[Events] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_pending_events <= 0 || max_pending_events > 100000) {
        fprintf(stderr, "[Events] Invalid max pending events: %d\n", max_pending_events);
        return ERR_INVALID_PARAMETER;
    }

    if (max_listeners <= 0 || max_listeners > 10000) {
        fprintf(stderr, "[Events] Invalid max listeners: %d\n", max_listeners);
        return ERR_INVALID_PARAMETER;
    }

    // Allocate event queue
    g_events_state.event_queue = calloc(max_pending_events, sizeof(GameEvent));
    if (!g_events_state.event_queue) {
        fprintf(stderr, "[Events] Memory allocation failed for event queue\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Allocate listener registry
    g_events_state.listeners = calloc(max_listeners, sizeof(EventListenerEntry));
    if (!g_events_state.listeners) {
        fprintf(stderr, "[Events] Memory allocation failed for listeners\n");
        free(g_events_state.event_queue);
        return ERR_OUT_OF_MEMORY;
    }

    // Initialize module state
    g_events_state.max_pending_events = max_pending_events;
    g_events_state.max_listeners = max_listeners;
    g_events_state.queue_head = 0;
    g_events_state.queue_tail = 0;
    g_events_state.queue_count = 0;
    g_events_state.next_listener_id = 1;
    g_events_state.processing_enabled = 1;

    // Reset statistics
    memset(&g_events_state.stats, 0, sizeof(g_events_state.stats));

    g_events_state.initialized = 1;
    printf("[Events] Module initialized. Max events: %d, Max listeners: %d\n",
           max_pending_events, max_listeners);

    return ERR_SUCCESS;
}

/**
 * Cleanup Events Module
 */
void Events_Cleanup()
{
    if (!g_events_state.initialized) {
        return;
    }

    printf("[Events] Cleaning up events module...\n");

    // Process all remaining events before cleanup
    if (g_events_state.queue_count > 0) {
        printf("[Events] Processing %d remaining events before shutdown...\n",
               g_events_state.queue_count);
        Events_FlushAll();
    }

    printf("[Events] Statistics:\n");
    printf("  - Total Events Created: %u\n", g_events_state.stats.total_events_created);
    printf("  - Total Events Processed: %u\n", g_events_state.stats.total_events_processed);
    printf("  - Total Listeners Registered: %u\n", g_events_state.stats.total_listeners_registered);
    printf("  - Max Queue Depth: %u\n", g_events_state.stats.max_queue_depth);
    printf("  - Listeners Triggered: %u\n", g_events_state.stats.listeners_triggered);

    // Free resources
    free(g_events_state.event_queue);
    free(g_events_state.listeners);

    g_events_state.event_queue = NULL;
    g_events_state.listeners = NULL;
    g_events_state.initialized = 0;
    g_events_state.queue_count = 0;

    printf("[Events] Module cleanup complete\n");
}

/**
 * Create and Queue Event
 */
int Events_CreateEvent(EventType event_type,
                       unsigned int creator_id,
                       unsigned int target_id,
                       EventPriority priority,
                       void* event_data)
{
    if (!g_events_state.initialized) {
        fprintf(stderr, "[Events] Module not initialized\n");
        return ERR_INVALID_STATE;
    }

    // Check queue capacity
    if (g_events_state.queue_count >= g_events_state.max_pending_events) {
        fprintf(stderr, "[Events] Event queue is full\n");
        return ERR_QUEUE_FULL;
    }

    // Get next queue slot
    GameEvent* event = &g_events_state.event_queue[g_events_state.queue_tail];

    // Initialize event
    static unsigned int event_id_counter = 1;
    event->event_id = event_id_counter++;
    event->event_type = event_type;
    event->priority = priority;
    event->creator_id = creator_id;
    event->target_id = target_id;
    event->timestamp = time(NULL);
    event->scheduled_time = 0;
    event->event_data = event_data;
    event->is_processed = 0;

    // Update queue pointers
    g_events_state.queue_tail = (g_events_state.queue_tail + 1) % g_events_state.max_pending_events;
    g_events_state.queue_count++;

    // Update statistics
    g_events_state.stats.total_events_created++;
    if (g_events_state.queue_count > g_events_state.stats.max_queue_depth) {
        g_events_state.stats.max_queue_depth = g_events_state.queue_count;
    }

    printf("[Events] Created event %u (type=%d, creator=%u, target=%u, queue_size=%d)\n",
           event->event_id, event_type, creator_id, target_id, g_events_state.queue_count);

    return event->event_id;
}

/**
 * Schedule Event for Later Execution
 */
int Events_ScheduleEvent(EventType event_type,
                         unsigned int creator_id,
                         unsigned int target_id,
                         EventPriority priority,
                         unsigned int scheduled_time,
                         void* event_data)
{
    if (!g_events_state.initialized) {
        fprintf(stderr, "[Events] Module not initialized\n");
        return ERR_INVALID_STATE;
    }

    // Check queue capacity
    if (g_events_state.queue_count >= g_events_state.max_pending_events) {
        fprintf(stderr, "[Events] Event queue is full\n");
        return ERR_QUEUE_FULL;
    }

    // Get next queue slot
    GameEvent* event = &g_events_state.event_queue[g_events_state.queue_tail];

    // Initialize scheduled event
    static unsigned int event_id_counter = 1;
    event->event_id = event_id_counter++;
    event->event_type = event_type;
    event->priority = priority;
    event->creator_id = creator_id;
    event->target_id = target_id;
    event->timestamp = time(NULL);
    event->scheduled_time = scheduled_time;
    event->event_data = event_data;
    event->is_processed = 0;

    // Update queue pointers
    g_events_state.queue_tail = (g_events_state.queue_tail + 1) % g_events_state.max_pending_events;
    g_events_state.queue_count++;
    g_events_state.stats.scheduled_events_count++;

    printf("[Events] Scheduled event %u for time %u (type=%d, queue_size=%d)\n",
           event->event_id, scheduled_time, event_type, g_events_state.queue_count);

    return event->event_id;
}

/**
 * Register Event Listener
 */
int Events_RegisterListener(EventType event_type,
                            EventListener listener,
                            void* context)
{
    if (!g_events_state.initialized || !listener) {
        return ERR_INVALID_PARAMETER;
    }

    // Find free listener slot
    for (int i = 0; i < g_events_state.max_listeners; i++) {
        if (!g_events_state.listeners[i].is_active) {
            EventListenerEntry* entry = &g_events_state.listeners[i];
            entry->listener_id = g_events_state.next_listener_id++;
            entry->event_type = event_type;
            entry->callback = listener;
            entry->context = context;
            entry->is_active = 1;

            g_events_state.stats.total_listeners_registered++;

            printf("[Events] Registered listener %d for event type %d\n",
                   entry->listener_id, event_type);

            return entry->listener_id;
        }
    }

    fprintf(stderr, "[Events] Maximum listeners reached\n");
    return ERR_MAX_LISTENERS_REACHED;
}

/**
 * Unregister Event Listener
 */
int Events_UnregisterListener(int listener_id)
{
    if (!g_events_state.initialized) {
        return ERR_INVALID_STATE;
    }

    for (int i = 0; i < g_events_state.max_listeners; i++) {
        if (g_events_state.listeners[i].listener_id == listener_id &&
            g_events_state.listeners[i].is_active) {
            g_events_state.listeners[i].is_active = 0;
            printf("[Events] Unregistered listener %d\n", listener_id);
            return 0;
        }
    }

    fprintf(stderr, "[Events] Listener %d not found\n", listener_id);
    return ERR_NOT_FOUND;
}

/**
 * Process Pending Events
 */
int Events_ProcessPending(int max_events_to_process)
{
    if (!g_events_state.initialized || !g_events_state.processing_enabled) {
        return 0;
    }

    int events_processed = 0;
    unsigned int events_to_check = (max_events_to_process > 0) ?
                          (unsigned int)min(max_events_to_process, (int)g_events_state.queue_count) :
                          g_events_state.queue_count;

    for (unsigned int i = 0; i < events_to_check && g_events_state.queue_count > 0; i++) {
        GameEvent* event = &g_events_state.event_queue[g_events_state.queue_head];

        // Check if event is scheduled for later
        if (event->scheduled_time > 0 && event->scheduled_time > (unsigned int)time(NULL)) {
            // Move to next event in queue
            g_events_state.queue_head = (g_events_state.queue_head + 1) %
                                       g_events_state.max_pending_events;
            g_events_state.queue_count--;
            continue;
        }

        // Trigger registered listeners for this event type
        for (int j = 0; j < g_events_state.max_listeners; j++) {
            if (g_events_state.listeners[j].is_active &&
                g_events_state.listeners[j].event_type == event->event_type) {
                g_events_state.listeners[j].callback(event, g_events_state.listeners[j].context);
                g_events_state.stats.listeners_triggered++;
            }
        }

        event->is_processed = 1;
        g_events_state.stats.total_events_processed++;

        // Move to next event in queue
        g_events_state.queue_head = (g_events_state.queue_head + 1) %
                                   g_events_state.max_pending_events;
        g_events_state.queue_count--;
        events_processed++;
    }

    if (events_processed > 0) {
        printf("[Events] Processed %d events (queue_size=%d)\n",
               events_processed, g_events_state.queue_count);
    }

    return events_processed;
}

/**
 * Get Pending Event Count
 */
int Events_GetPendingCount()
{
    if (!g_events_state.initialized) {
        return 0;
    }

    return g_events_state.queue_count;
}

/**
 * Flush All Events
 */
int Events_FlushAll()
{
    if (!g_events_state.initialized) {
        return 0;
    }

    return Events_ProcessPending(g_events_state.max_pending_events);
}

/**
 * Enable/Disable Event Processing
 */
int Events_SetProcessingEnabled(int enabled)
{
    g_events_state.processing_enabled = (enabled != 0) ? 1 : 0;
    printf("[Events] Event processing %s\n", enabled ? "enabled" : "disabled");
    return 0;
}

/**
 * Get Event Processing Statistics
 */
void* Events_GetStatistics()
{
    if (!g_events_state.initialized) {
        return NULL;
    }

    return &g_events_state.stats;
}

/**
 * Reset Event System Statistics
 */
void Events_ResetStatistics()
{
    if (!g_events_state.initialized) {
        return;
    }

    memset(&g_events_state.stats, 0, sizeof(g_events_state.stats));
    printf("[Events] Statistics reset\n");
}