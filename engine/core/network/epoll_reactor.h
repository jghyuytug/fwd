/**
 * @file epoll_reactor.h
 * @brief Epoll-based event reactor for network I/O
 *
 * High-performance event loop using Linux epoll.
 * Supports edge-triggered and level-triggered modes.
 */

#ifndef ENGINE_CORE_NETWORK_EPOLL_REACTOR_H
#define ENGINE_CORE_NETWORK_EPOLL_REACTOR_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration */
#define REACTOR_MAX_EVENTS      1024
#define REACTOR_MAX_FDS         65536
#define REACTOR_DEFAULT_TIMEOUT 100  /* milliseconds */

/* Event types (matching epoll) */
typedef enum {
    REACTOR_EVENT_NONE   = 0x00,
    REACTOR_EVENT_READ   = 0x01,
    REACTOR_EVENT_WRITE  = 0x02,
    REACTOR_EVENT_ERROR  = 0x04,
    REACTOR_EVENT_CLOSE  = 0x08,
    REACTOR_EVENT_ET     = 0x10  /* Edge-triggered */
} ReactorEventType;

/* Forward declaration */
struct EpollReactor;
typedef struct EpollReactor EpollReactor;

/**
 * Event callback function
 * @param reactor   The reactor instance
 * @param fd        File descriptor that triggered the event
 * @param events    Bitfield of ReactorEventType
 * @param user_data User-provided context
 */
typedef void (*ReactorCallback)(EpollReactor* reactor, int fd,
                                u32 events, void* user_data);

/* FD context */
typedef struct {
    int              fd;
    u32              events;
    ReactorCallback  callback;
    void*            user_data;
    u8               active;
} ReactorFdContext;

/* Reactor structure */
struct EpollReactor {
    int              epoll_fd;
    int              running;
    u32              fd_count;
    u64              event_count;    /* Statistics */
    u64              loop_count;
    void*            epoll_events;   /* struct epoll_event[] */
    ReactorFdContext fd_contexts[REACTOR_MAX_FDS];
};

/**
 * Initialize the reactor
 * @param reactor  Reactor instance to initialize
 * @return ENGINE_OK on success, error code on failure
 */
int reactor_init(EpollReactor* reactor);

/**
 * Cleanup and release reactor resources
 * @param reactor  Reactor instance to cleanup
 */
void reactor_cleanup(EpollReactor* reactor);

/**
 * Add a file descriptor to the reactor
 * @param reactor   Reactor instance
 * @param fd        File descriptor to monitor
 * @param events    Events to monitor (REACTOR_EVENT_*)
 * @param callback  Callback function when events occur
 * @param user_data User context passed to callback
 * @return ENGINE_OK on success, error code on failure
 */
int reactor_add(EpollReactor* reactor, int fd, u32 events,
                ReactorCallback callback, void* user_data);

/**
 * Modify events for a file descriptor
 * @param reactor   Reactor instance
 * @param fd        File descriptor to modify
 * @param events    New events to monitor
 * @return ENGINE_OK on success, error code on failure
 */
int reactor_mod(EpollReactor* reactor, int fd, u32 events);

/**
 * Remove a file descriptor from the reactor
 * @param reactor   Reactor instance
 * @param fd        File descriptor to remove
 * @return ENGINE_OK on success, error code on failure
 */
int reactor_del(EpollReactor* reactor, int fd);

/**
 * Run one iteration of the event loop
 * @param reactor    Reactor instance
 * @param timeout_ms Maximum wait time in milliseconds (-1 for infinite)
 * @return Number of events processed, or negative error code
 */
int reactor_poll(EpollReactor* reactor, int timeout_ms);

/**
 * Run the event loop until stopped
 * @param reactor    Reactor instance
 * @param timeout_ms Poll timeout per iteration
 */
void reactor_run(EpollReactor* reactor, int timeout_ms);

/**
 * Stop the reactor event loop
 * @param reactor   Reactor instance
 */
void reactor_stop(EpollReactor* reactor);

/**
 * Check if reactor is running
 * @param reactor   Reactor instance
 * @return 1 if running, 0 if stopped
 */
int reactor_is_running(const EpollReactor* reactor);

/**
 * Get reactor statistics
 * @param reactor     Reactor instance
 * @param fd_count    Output: number of registered FDs
 * @param event_count Output: total events processed
 * @param loop_count  Output: total loop iterations
 */
void reactor_get_stats(const EpollReactor* reactor,
                       u32* fd_count, u64* event_count, u64* loop_count);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_NETWORK_EPOLL_REACTOR_H */
