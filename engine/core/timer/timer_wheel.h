/**
 * @file timer_wheel.h
 * @brief Hierarchical timing wheel for efficient timer management
 *
 * O(1) timer operations with millisecond resolution.
 */

#ifndef ENGINE_CORE_TIMER_TIMER_WHEEL_H
#define ENGINE_CORE_TIMER_TIMER_WHEEL_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration */
#define TIMER_WHEEL_BITS        8
#define TIMER_WHEEL_SIZE        (1 << TIMER_WHEEL_BITS)    /* 256 slots */
#define TIMER_WHEEL_MASK        (TIMER_WHEEL_SIZE - 1)
#define TIMER_WHEEL_LEVELS      4                           /* 4 levels */
#define TIMER_TICK_MS           10                          /* 10ms resolution */
#define TIMER_MAX_TIMERS        4096

/* Timer flags */
#define TIMER_FLAG_NONE         0x00
#define TIMER_FLAG_REPEAT       0x01    /* Auto-repeat timer */
#define TIMER_FLAG_ACTIVE       0x02    /* Timer is active */

/* Forward declaration */
struct TimerWheel;
typedef struct TimerWheel TimerWheel;

/**
 * Timer callback function
 * @param wheel     Timer wheel instance
 * @param timer_id  Timer ID
 * @param user_data User context
 */
typedef void (*TimerCallback)(TimerWheel* wheel, u32 timer_id, void* user_data);

/* Timer node */
typedef struct TimerNode {
    u32              timer_id;
    u64              expire_tick;     /* Absolute tick when timer fires */
    u32              interval_ms;     /* Repeat interval (0 = one-shot) */
    TimerCallback    callback;
    void*            user_data;
    u8               flags;
    struct TimerNode* next;
    struct TimerNode* prev;
} TimerNode;

/* Timer wheel level */
typedef struct {
    TimerNode* slots[TIMER_WHEEL_SIZE];
    u32        current_slot;
} TimerWheelLevel;

/* Timer wheel structure */
struct TimerWheel {
    TimerWheelLevel levels[TIMER_WHEEL_LEVELS];
    TimerNode       timers[TIMER_MAX_TIMERS];
    TimerNode*      free_list;
    u64             current_tick;
    u64             last_update_ms;
    u32             next_timer_id;
    u32             active_count;
    /* Statistics */
    u64             timers_created;
    u64             timers_fired;
    u64             timers_cancelled;
};

/**
 * Initialize timer wheel
 * @param wheel Timer wheel instance
 * @return ENGINE_OK on success
 */
int timer_wheel_init(TimerWheel* wheel);

/**
 * Cleanup timer wheel
 * @param wheel Timer wheel instance
 */
void timer_wheel_cleanup(TimerWheel* wheel);

/**
 * Add a one-shot timer
 * @param wheel     Timer wheel instance
 * @param delay_ms  Delay in milliseconds
 * @param callback  Callback function
 * @param user_data User context
 * @return Timer ID (>0) on success, 0 on failure
 */
u32 timer_add(TimerWheel* wheel, u32 delay_ms,
              TimerCallback callback, void* user_data);

/**
 * Add a repeating timer
 * @param wheel       Timer wheel instance
 * @param delay_ms    Initial delay in milliseconds
 * @param interval_ms Repeat interval in milliseconds
 * @param callback    Callback function
 * @param user_data   User context
 * @return Timer ID (>0) on success, 0 on failure
 */
u32 timer_add_repeat(TimerWheel* wheel, u32 delay_ms, u32 interval_ms,
                     TimerCallback callback, void* user_data);

/**
 * Cancel a timer
 * @param wheel    Timer wheel instance
 * @param timer_id Timer ID to cancel
 * @return ENGINE_OK on success, ENGINE_ERROR_NOTFOUND if not found
 */
int timer_cancel(TimerWheel* wheel, u32 timer_id);

/**
 * Reset a timer (restart with same callback/interval)
 * @param wheel    Timer wheel instance
 * @param timer_id Timer ID to reset
 * @param delay_ms New delay in milliseconds
 * @return ENGINE_OK on success
 */
int timer_reset(TimerWheel* wheel, u32 timer_id, u32 delay_ms);

/**
 * Check if timer is active
 * @param wheel    Timer wheel instance
 * @param timer_id Timer ID to check
 * @return 1 if active, 0 otherwise
 */
int timer_is_active(const TimerWheel* wheel, u32 timer_id);

/**
 * Get remaining time for a timer
 * @param wheel    Timer wheel instance
 * @param timer_id Timer ID
 * @return Remaining time in milliseconds, 0 if not found or expired
 */
u32 timer_remaining(const TimerWheel* wheel, u32 timer_id);

/**
 * Update timer wheel (call every frame/tick)
 * @param wheel      Timer wheel instance
 * @param current_ms Current time in milliseconds
 * @return Number of timers fired
 */
int timer_wheel_update(TimerWheel* wheel, u64 current_ms);

/**
 * Get current tick count
 * @param wheel Timer wheel instance
 * @return Current tick
 */
u64 timer_get_tick(const TimerWheel* wheel);

/**
 * Get active timer count
 * @param wheel Timer wheel instance
 * @return Number of active timers
 */
u32 timer_get_active_count(const TimerWheel* wheel);

/**
 * Get statistics
 * @param wheel     Timer wheel instance
 * @param created   Output: total timers created
 * @param fired     Output: total timers fired
 * @param cancelled Output: total timers cancelled
 */
void timer_get_stats(const TimerWheel* wheel,
                     u64* created, u64* fired, u64* cancelled);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_TIMER_TIMER_WHEEL_H */
