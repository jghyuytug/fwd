/**
 * @file timer_wheel.c
 * @brief Hierarchical timing wheel implementation
 */

#include "timer_wheel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Convert milliseconds to ticks */
static u64 ms_to_ticks(u32 ms)
{
    return (ms + TIMER_TICK_MS - 1) / TIMER_TICK_MS;
}

/* Get level and slot for a given tick offset */
static void get_level_slot(u64 tick_offset, int* level, u32* slot)
{
    u64 offset = tick_offset;
    int lvl;

    for (lvl = 0; lvl < TIMER_WHEEL_LEVELS; lvl++) {
        if (offset < TIMER_WHEEL_SIZE) {
            *level = lvl;
            *slot = (u32)offset;
            return;
        }
        offset >>= TIMER_WHEEL_BITS;
    }

    /* Overflow - use last level, last slot */
    *level = TIMER_WHEEL_LEVELS - 1;
    *slot = TIMER_WHEEL_SIZE - 1;
}

/* Link timer into a wheel slot */
static void link_timer(TimerWheelLevel* level, u32 slot, TimerNode* node)
{
    node->next = level->slots[slot];
    node->prev = NULL;

    if (level->slots[slot]) {
        level->slots[slot]->prev = node;
    }

    level->slots[slot] = node;
}

/* Unlink timer from its current slot */
static void unlink_timer(TimerWheelLevel* level, u32 slot, TimerNode* node)
{
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        level->slots[slot] = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    node->next = NULL;
    node->prev = NULL;
}

/* Insert timer into the wheel */
static void insert_timer(TimerWheel* wheel, TimerNode* node)
{
    u64 tick_offset;
    int level;
    u32 slot;

    if (node->expire_tick <= wheel->current_tick) {
        tick_offset = 0;
    } else {
        tick_offset = node->expire_tick - wheel->current_tick;
    }

    get_level_slot(tick_offset, &level, &slot);

    /* Adjust slot based on current position in level */
    slot = (wheel->levels[level].current_slot + slot) & TIMER_WHEEL_MASK;

    link_timer(&wheel->levels[level], slot, node);
}

/* Allocate a timer node */
static TimerNode* alloc_timer(TimerWheel* wheel)
{
    TimerNode* node;

    if (!wheel->free_list) {
        return NULL;
    }

    node = wheel->free_list;
    wheel->free_list = node->next;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

/* Free a timer node */
static void free_timer(TimerWheel* wheel, TimerNode* node)
{
    memset(node, 0, sizeof(TimerNode));
    node->next = wheel->free_list;
    wheel->free_list = node;
}

/* Find timer by ID */
static TimerNode* find_timer(TimerWheel* wheel, u32 timer_id)
{
    int i;

    if (timer_id == 0 || timer_id > TIMER_MAX_TIMERS) {
        return NULL;
    }

    /* Timer ID is 1-based index + generation */
    for (i = 0; i < TIMER_MAX_TIMERS; i++) {
        if (wheel->timers[i].timer_id == timer_id &&
            (wheel->timers[i].flags & TIMER_FLAG_ACTIVE)) {
            return &wheel->timers[i];
        }
    }

    return NULL;
}

int timer_wheel_init(TimerWheel* wheel)
{
    int i;

    if (!wheel) {
        return ENGINE_ERROR_PARAM;
    }

    memset(wheel, 0, sizeof(TimerWheel));

    /* Initialize levels */
    for (i = 0; i < TIMER_WHEEL_LEVELS; i++) {
        memset(wheel->levels[i].slots, 0, sizeof(wheel->levels[i].slots));
        wheel->levels[i].current_slot = 0;
    }

    /* Build free list */
    wheel->free_list = NULL;
    for (i = TIMER_MAX_TIMERS - 1; i >= 0; i--) {
        wheel->timers[i].next = wheel->free_list;
        wheel->free_list = &wheel->timers[i];
    }

    wheel->current_tick = 0;
    wheel->last_update_ms = 0;
    wheel->next_timer_id = 1;
    wheel->active_count = 0;

    return ENGINE_OK;
}

void timer_wheel_cleanup(TimerWheel* wheel)
{
    if (!wheel) {
        return;
    }

    /* All timers are in the static array, just clear */
    memset(wheel, 0, sizeof(TimerWheel));
}

static u32 timer_add_internal(TimerWheel* wheel, u32 delay_ms, u32 interval_ms,
                              TimerCallback callback, void* user_data)
{
    TimerNode* node;
    u64 ticks;

    if (!wheel || !callback) {
        return 0;
    }

    node = alloc_timer(wheel);
    if (!node) {
        fprintf(stderr, "[TIMER] No free timer slots\n");
        return 0;
    }

    ticks = ms_to_ticks(delay_ms);
    if (ticks == 0) {
        ticks = 1;  /* Minimum 1 tick */
    }

    node->timer_id = wheel->next_timer_id++;
    node->expire_tick = wheel->current_tick + ticks;
    node->interval_ms = interval_ms;
    node->callback = callback;
    node->user_data = user_data;
    node->flags = TIMER_FLAG_ACTIVE;

    if (interval_ms > 0) {
        node->flags |= TIMER_FLAG_REPEAT;
    }

    insert_timer(wheel, node);

    wheel->active_count++;
    wheel->timers_created++;

    return node->timer_id;
}

u32 timer_add(TimerWheel* wheel, u32 delay_ms,
              TimerCallback callback, void* user_data)
{
    return timer_add_internal(wheel, delay_ms, 0, callback, user_data);
}

u32 timer_add_repeat(TimerWheel* wheel, u32 delay_ms, u32 interval_ms,
                     TimerCallback callback, void* user_data)
{
    return timer_add_internal(wheel, delay_ms, interval_ms, callback, user_data);
}

int timer_cancel(TimerWheel* wheel, u32 timer_id)
{
    TimerNode* node;
    int level;
    u32 slot;
    u64 tick_offset;

    if (!wheel || timer_id == 0) {
        return ENGINE_ERROR_PARAM;
    }

    node = find_timer(wheel, timer_id);
    if (!node) {
        return ENGINE_ERROR_NOTFOUND;
    }

    /* Find which slot it's in */
    if (node->expire_tick <= wheel->current_tick) {
        tick_offset = 0;
    } else {
        tick_offset = node->expire_tick - wheel->current_tick;
    }

    get_level_slot(tick_offset, &level, &slot);
    slot = (wheel->levels[level].current_slot + slot) & TIMER_WHEEL_MASK;

    /* Unlink and free */
    unlink_timer(&wheel->levels[level], slot, node);
    free_timer(wheel, node);

    if (wheel->active_count > 0) {
        wheel->active_count--;
    }
    wheel->timers_cancelled++;

    return ENGINE_OK;
}

int timer_reset(TimerWheel* wheel, u32 timer_id, u32 delay_ms)
{
    TimerNode* node;
    int level;
    u32 slot;
    u64 tick_offset;
    u64 ticks;

    if (!wheel || timer_id == 0) {
        return ENGINE_ERROR_PARAM;
    }

    node = find_timer(wheel, timer_id);
    if (!node) {
        return ENGINE_ERROR_NOTFOUND;
    }

    /* Unlink from current position */
    if (node->expire_tick <= wheel->current_tick) {
        tick_offset = 0;
    } else {
        tick_offset = node->expire_tick - wheel->current_tick;
    }

    get_level_slot(tick_offset, &level, &slot);
    slot = (wheel->levels[level].current_slot + slot) & TIMER_WHEEL_MASK;

    unlink_timer(&wheel->levels[level], slot, node);

    /* Update expire time */
    ticks = ms_to_ticks(delay_ms);
    if (ticks == 0) {
        ticks = 1;
    }

    node->expire_tick = wheel->current_tick + ticks;

    /* Re-insert */
    insert_timer(wheel, node);

    return ENGINE_OK;
}

int timer_is_active(const TimerWheel* wheel, u32 timer_id)
{
    int i;

    if (!wheel || timer_id == 0) {
        return 0;
    }

    for (i = 0; i < TIMER_MAX_TIMERS; i++) {
        if (wheel->timers[i].timer_id == timer_id &&
            (wheel->timers[i].flags & TIMER_FLAG_ACTIVE)) {
            return 1;
        }
    }

    return 0;
}

u32 timer_remaining(const TimerWheel* wheel, u32 timer_id)
{
    int i;
    u64 remaining_ticks;

    if (!wheel || timer_id == 0) {
        return 0;
    }

    for (i = 0; i < TIMER_MAX_TIMERS; i++) {
        if (wheel->timers[i].timer_id == timer_id &&
            (wheel->timers[i].flags & TIMER_FLAG_ACTIVE)) {

            if (wheel->timers[i].expire_tick <= wheel->current_tick) {
                return 0;
            }

            remaining_ticks = wheel->timers[i].expire_tick - wheel->current_tick;
            return (u32)(remaining_ticks * TIMER_TICK_MS);
        }
    }

    return 0;
}

/* Cascade timers from higher level to lower level */
static void cascade_timers(TimerWheel* wheel, int level)
{
    TimerWheelLevel* lvl;
    TimerNode* node;
    TimerNode* next;
    u32 slot;

    if (level >= TIMER_WHEEL_LEVELS) {
        return;
    }

    lvl = &wheel->levels[level];
    slot = lvl->current_slot;

    node = lvl->slots[slot];
    lvl->slots[slot] = NULL;

    while (node) {
        next = node->next;
        node->next = NULL;
        node->prev = NULL;

        /* Re-insert at appropriate level */
        insert_timer(wheel, node);

        node = next;
    }
}

int timer_wheel_update(TimerWheel* wheel, u64 current_ms)
{
    u64 elapsed_ms;
    u64 ticks_to_process;
    int fired = 0;
    TimerNode* node;
    TimerNode* next;
    TimerWheelLevel* lvl0;
    u32 slot;

    if (!wheel) {
        return 0;
    }

    /* Initialize on first call */
    if (wheel->last_update_ms == 0) {
        wheel->last_update_ms = current_ms;
        return 0;
    }

    elapsed_ms = current_ms - wheel->last_update_ms;
    ticks_to_process = elapsed_ms / TIMER_TICK_MS;

    if (ticks_to_process == 0) {
        return 0;
    }

    wheel->last_update_ms += ticks_to_process * TIMER_TICK_MS;

    lvl0 = &wheel->levels[0];

    while (ticks_to_process > 0) {
        wheel->current_tick++;
        ticks_to_process--;

        /* Advance level 0 slot */
        slot = lvl0->current_slot;
        lvl0->current_slot = (slot + 1) & TIMER_WHEEL_MASK;

        /* Cascade if we wrapped around */
        if (lvl0->current_slot == 0) {
            int level;
            for (level = 1; level < TIMER_WHEEL_LEVELS; level++) {
                wheel->levels[level].current_slot =
                    (wheel->levels[level].current_slot + 1) & TIMER_WHEEL_MASK;

                cascade_timers(wheel, level);

                if (wheel->levels[level].current_slot != 0) {
                    break;
                }
            }
        }

        /* Fire all timers in current slot */
        node = lvl0->slots[slot];
        lvl0->slots[slot] = NULL;

        while (node) {
            next = node->next;
            node->next = NULL;
            node->prev = NULL;

            /* Check if timer should fire */
            if (node->expire_tick <= wheel->current_tick) {
                /* Fire callback */
                if (node->callback) {
                    node->callback(wheel, node->timer_id, node->user_data);
                }

                fired++;
                wheel->timers_fired++;

                /* Handle repeat */
                if (node->flags & TIMER_FLAG_REPEAT) {
                    u64 ticks = ms_to_ticks(node->interval_ms);
                    if (ticks == 0) ticks = 1;

                    node->expire_tick = wheel->current_tick + ticks;
                    insert_timer(wheel, node);
                } else {
                    /* One-shot: free the timer */
                    node->flags &= ~TIMER_FLAG_ACTIVE;
                    free_timer(wheel, node);

                    if (wheel->active_count > 0) {
                        wheel->active_count--;
                    }
                }
            } else {
                /* Timer was cascaded but not yet due, re-insert */
                insert_timer(wheel, node);
            }

            node = next;
        }
    }

    return fired;
}

u64 timer_get_tick(const TimerWheel* wheel)
{
    return wheel ? wheel->current_tick : 0;
}

u32 timer_get_active_count(const TimerWheel* wheel)
{
    return wheel ? wheel->active_count : 0;
}

void timer_get_stats(const TimerWheel* wheel,
                     u64* created, u64* fired, u64* cancelled)
{
    if (!wheel) {
        if (created) *created = 0;
        if (fired) *fired = 0;
        if (cancelled) *cancelled = 0;
        return;
    }

    if (created) *created = wheel->timers_created;
    if (fired) *fired = wheel->timers_fired;
    if (cancelled) *cancelled = wheel->timers_cancelled;
}
