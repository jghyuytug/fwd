/**
 * @file event_bus.c
 * @brief Event bus implementation
 */

#include "event_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Get current timestamp */
static u32 get_timestamp(void)
{
    return (u32)time(NULL);
}

int event_bus_init(EventBus* bus)
{
    if (!bus) {
        return ENGINE_ERROR_PARAM;
    }

    memset(bus, 0, sizeof(EventBus));
    return ENGINE_OK;
}

void event_bus_cleanup(EventBus* bus)
{
    if (!bus) {
        return;
    }

    /* Clear all handlers */
    memset(bus->handlers, 0, sizeof(bus->handlers));
    bus->handler_count = 0;

    /* Clear queue */
    bus->queue_head = 0;
    bus->queue_tail = 0;
    bus->queue_size = 0;
}

static int find_free_handler_slot(EventBus* bus)
{
    int i;

    for (i = 0; i < EVENT_BUS_MAX_HANDLERS; i++) {
        if (!bus->handlers[i].active) {
            return i;
        }
    }

    return -1;
}

int event_subscribe(EventBus* bus, EventType type,
                    EventHandler handler, void* user_data)
{
    int slot;
    EventHandlerEntry* entry;

    if (!bus || !handler) {
        return ENGINE_ERROR_PARAM;
    }

    slot = find_free_handler_slot(bus);
    if (slot < 0) {
        return ENGINE_ERROR_FULL;
    }

    entry = &bus->handlers[slot];
    entry->handler = handler;
    entry->user_data = user_data;
    entry->event_type = type;
    entry->category = EVENT_CATEGORY_NONE;
    entry->active = 1;
    entry->once = 0;

    bus->handler_count++;

    return slot;
}

int event_subscribe_category(EventBus* bus, EventCategory category,
                             EventHandler handler, void* user_data)
{
    int slot;
    EventHandlerEntry* entry;

    if (!bus || !handler) {
        return ENGINE_ERROR_PARAM;
    }

    slot = find_free_handler_slot(bus);
    if (slot < 0) {
        return ENGINE_ERROR_FULL;
    }

    entry = &bus->handlers[slot];
    entry->handler = handler;
    entry->user_data = user_data;
    entry->event_type = EVENT_NONE;
    entry->category = category;
    entry->active = 1;
    entry->once = 0;

    bus->handler_count++;

    return slot;
}

int event_subscribe_once(EventBus* bus, EventType type,
                         EventHandler handler, void* user_data)
{
    int slot;

    slot = event_subscribe(bus, type, handler, user_data);
    if (slot >= 0) {
        bus->handlers[slot].once = 1;
    }

    return slot;
}

int event_unsubscribe(EventBus* bus, int handler_id)
{
    if (!bus || handler_id < 0 || handler_id >= EVENT_BUS_MAX_HANDLERS) {
        return ENGINE_ERROR_PARAM;
    }

    if (!bus->handlers[handler_id].active) {
        return ENGINE_ERROR_NOTFOUND;
    }

    memset(&bus->handlers[handler_id], 0, sizeof(EventHandlerEntry));

    if (bus->handler_count > 0) {
        bus->handler_count--;
    }

    return ENGINE_OK;
}

int event_unsubscribe_type(EventBus* bus, EventType type)
{
    int i;
    int count = 0;

    if (!bus) {
        return 0;
    }

    for (i = 0; i < EVENT_BUS_MAX_HANDLERS; i++) {
        if (bus->handlers[i].active && bus->handlers[i].event_type == type) {
            memset(&bus->handlers[i], 0, sizeof(EventHandlerEntry));
            count++;

            if (bus->handler_count > 0) {
                bus->handler_count--;
            }
        }
    }

    return count;
}

static int should_handle(const EventHandlerEntry* entry, EventType type)
{
    if (!entry->active) {
        return 0;
    }

    /* Handle all events */
    if (entry->event_type == EVENT_NONE &&
        entry->category == EVENT_CATEGORY_NONE) {
        return 1;
    }

    /* Match specific type */
    if (entry->event_type != EVENT_NONE &&
        entry->event_type == type) {
        return 1;
    }

    /* Match category */
    if (entry->category != EVENT_CATEGORY_NONE &&
        event_is_category(type, entry->category)) {
        return 1;
    }

    return 0;
}

int event_publish(EventBus* bus, const EventBase* event)
{
    int i;
    int count = 0;
    EventHandlerEntry* entry;
    int to_remove[EVENT_BUS_MAX_HANDLERS];
    int remove_count = 0;

    if (!bus || !event) {
        return 0;
    }

    bus->events_published++;

    /* Prevent recursive dispatch */
    if (bus->processing) {
        /* Queue it instead */
        event_queue(bus, event, NULL, 0);
        return 0;
    }

    bus->processing = 1;

    for (i = 0; i < EVENT_BUS_MAX_HANDLERS; i++) {
        entry = &bus->handlers[i];

        if (!should_handle(entry, event->type)) {
            continue;
        }

        /* Call handler */
        entry->handler(bus, event, entry->user_data);
        count++;
        bus->events_dispatched++;

        /* Mark for removal if once */
        if (entry->once) {
            to_remove[remove_count++] = i;
        }
    }

    /* Remove once handlers */
    for (i = 0; i < remove_count; i++) {
        event_unsubscribe(bus, to_remove[i]);
    }

    bus->processing = 0;

    /* Process any queued events */
    event_dispatch_queued(bus, 0);

    return count;
}

int event_publish_simple(EventBus* bus, EventType type,
                         u32 source_id, u32 target_id)
{
    EventBase event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.timestamp = get_timestamp();
    event.source_id = source_id;
    event.target_id = target_id;

    return event_publish(bus, &event);
}

int event_queue(EventBus* bus, const EventBase* event,
                const void* data, u32 data_size)
{
    QueuedEvent* qe;

    if (!bus || !event) {
        return ENGINE_ERROR_PARAM;
    }

    if (bus->queue_size >= EVENT_BUS_MAX_QUEUED) {
        bus->events_dropped++;
        return ENGINE_ERROR_FULL;
    }

    if (data_size > EVENT_BUS_MAX_DATA_SIZE) {
        data_size = EVENT_BUS_MAX_DATA_SIZE;
    }

    qe = &bus->queue[bus->queue_tail];
    qe->type = event->type;
    qe->timestamp = event->timestamp ? event->timestamp : get_timestamp();
    qe->source_id = event->source_id;
    qe->target_id = event->target_id;

    if (data && data_size > 0) {
        memcpy(qe->data, data, data_size);
        qe->data_size = data_size;
    } else {
        qe->data_size = 0;
    }

    bus->queue_tail = (bus->queue_tail + 1) % EVENT_BUS_MAX_QUEUED;
    bus->queue_size++;

    return ENGINE_OK;
}

int event_queue_simple(EventBus* bus, EventType type,
                       u32 source_id, u32 target_id)
{
    EventBase event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.timestamp = get_timestamp();
    event.source_id = source_id;
    event.target_id = target_id;

    return event_queue(bus, &event, NULL, 0);
}

int event_dispatch_queued(EventBus* bus, int max_count)
{
    int processed = 0;
    QueuedEvent* qe;
    EventBase event;

    if (!bus || bus->queue_size == 0) {
        return 0;
    }

    /* Prevent recursive dispatch */
    if (bus->processing) {
        return 0;
    }

    bus->processing = 1;

    while (bus->queue_size > 0) {
        if (max_count > 0 && processed >= max_count) {
            break;
        }

        qe = &bus->queue[bus->queue_head];

        /* Build event from queue */
        memset(&event, 0, sizeof(event));
        event.type = qe->type;
        event.timestamp = qe->timestamp;
        event.source_id = qe->source_id;
        event.target_id = qe->target_id;

        bus->queue_head = (bus->queue_head + 1) % EVENT_BUS_MAX_QUEUED;
        bus->queue_size--;

        /* Dispatch to handlers */
        {
            int i;
            EventHandlerEntry* entry;
            int to_remove[EVENT_BUS_MAX_HANDLERS];
            int remove_count = 0;

            for (i = 0; i < EVENT_BUS_MAX_HANDLERS; i++) {
                entry = &bus->handlers[i];

                if (!should_handle(entry, event.type)) {
                    continue;
                }

                entry->handler(bus, &event, entry->user_data);
                bus->events_dispatched++;

                if (entry->once) {
                    to_remove[remove_count++] = i;
                }
            }

            for (i = 0; i < remove_count; i++) {
                event_unsubscribe(bus, to_remove[i]);
            }
        }

        processed++;
    }

    bus->processing = 0;

    return processed;
}

int event_queue_count(const EventBus* bus)
{
    return bus ? bus->queue_size : 0;
}

void event_queue_clear(EventBus* bus)
{
    if (!bus) {
        return;
    }

    bus->events_dropped += bus->queue_size;
    bus->queue_head = 0;
    bus->queue_tail = 0;
    bus->queue_size = 0;
}

void event_get_stats(const EventBus* bus,
                     u64* published, u64* dispatched,
                     u64* dropped, int* handler_count)
{
    if (!bus) {
        if (published) *published = 0;
        if (dispatched) *dispatched = 0;
        if (dropped) *dropped = 0;
        if (handler_count) *handler_count = 0;
        return;
    }

    if (published) *published = bus->events_published;
    if (dispatched) *dispatched = bus->events_dispatched;
    if (dropped) *dropped = bus->events_dropped;
    if (handler_count) *handler_count = bus->handler_count;
}
