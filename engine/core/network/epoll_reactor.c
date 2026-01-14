/**
 * @file epoll_reactor.c
 * @brief Epoll-based event reactor implementation
 */

#include "epoll_reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

/* EPOLLRDHUP was added in Linux 2.6.17, define it if not available */
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif

/* Convert ReactorEventType to epoll events */
static u32 reactor_to_epoll_events(u32 reactor_events)
{
    u32 epoll_ev = 0;

    if (reactor_events & REACTOR_EVENT_READ)
        epoll_ev |= EPOLLIN;
    if (reactor_events & REACTOR_EVENT_WRITE)
        epoll_ev |= EPOLLOUT;
    if (reactor_events & REACTOR_EVENT_ERROR)
        epoll_ev |= EPOLLERR;
    if (reactor_events & REACTOR_EVENT_CLOSE)
        epoll_ev |= EPOLLHUP | EPOLLRDHUP;
    if (reactor_events & REACTOR_EVENT_ET)
        epoll_ev |= EPOLLET;

    return epoll_ev;
}

/* Convert epoll events to ReactorEventType */
static u32 epoll_to_reactor_events(u32 epoll_ev)
{
    u32 reactor_events = 0;

    if (epoll_ev & EPOLLIN)
        reactor_events |= REACTOR_EVENT_READ;
    if (epoll_ev & EPOLLOUT)
        reactor_events |= REACTOR_EVENT_WRITE;
    if (epoll_ev & EPOLLERR)
        reactor_events |= REACTOR_EVENT_ERROR;
    if (epoll_ev & (EPOLLHUP | EPOLLRDHUP))
        reactor_events |= REACTOR_EVENT_CLOSE;

    return reactor_events;
}

int reactor_init(EpollReactor* reactor)
{
    struct epoll_event* events;

    if (!reactor) {
        return ENGINE_ERROR_PARAM;
    }

    memset(reactor, 0, sizeof(EpollReactor));

    /* Create epoll instance */
    reactor->epoll_fd = epoll_create(REACTOR_MAX_EVENTS);
    if (reactor->epoll_fd < 0) {
        fprintf(stderr, "[REACTOR] epoll_create failed: %s\n", strerror(errno));
        return ENGINE_ERROR_IO;
    }

    /* Allocate event buffer */
    events = (struct epoll_event*)malloc(
        sizeof(struct epoll_event) * REACTOR_MAX_EVENTS);
    if (!events) {
        close(reactor->epoll_fd);
        reactor->epoll_fd = -1;
        return ENGINE_ERROR_MEMORY;
    }

    reactor->epoll_events = events;
    reactor->running = 0;
    reactor->fd_count = 0;
    reactor->event_count = 0;
    reactor->loop_count = 0;

    /* Initialize FD contexts */
    memset(reactor->fd_contexts, 0, sizeof(reactor->fd_contexts));

    return ENGINE_OK;
}

void reactor_cleanup(EpollReactor* reactor)
{
    int i;

    if (!reactor) {
        return;
    }

    /* Close all registered FDs */
    for (i = 0; i < REACTOR_MAX_FDS; i++) {
        if (reactor->fd_contexts[i].active) {
            reactor_del(reactor, i);
        }
    }

    /* Free event buffer */
    if (reactor->epoll_events) {
        free(reactor->epoll_events);
        reactor->epoll_events = NULL;
    }

    /* Close epoll FD */
    if (reactor->epoll_fd >= 0) {
        close(reactor->epoll_fd);
        reactor->epoll_fd = -1;
    }

    reactor->running = 0;
}

int reactor_add(EpollReactor* reactor, int fd, u32 events,
                ReactorCallback callback, void* user_data)
{
    struct epoll_event ev;
    ReactorFdContext* ctx;

    if (!reactor || fd < 0 || fd >= REACTOR_MAX_FDS || !callback) {
        return ENGINE_ERROR_PARAM;
    }

    if (reactor->fd_contexts[fd].active) {
        return ENGINE_ERROR_EXISTS;
    }

    /* Setup epoll event */
    memset(&ev, 0, sizeof(ev));
    ev.events = reactor_to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(reactor->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        fprintf(stderr, "[REACTOR] epoll_ctl ADD failed for fd=%d: %s\n",
                fd, strerror(errno));
        return ENGINE_ERROR_IO;
    }

    /* Store context */
    ctx = &reactor->fd_contexts[fd];
    ctx->fd = fd;
    ctx->events = events;
    ctx->callback = callback;
    ctx->user_data = user_data;
    ctx->active = 1;

    reactor->fd_count++;

    return ENGINE_OK;
}

int reactor_mod(EpollReactor* reactor, int fd, u32 events)
{
    struct epoll_event ev;

    if (!reactor || fd < 0 || fd >= REACTOR_MAX_FDS) {
        return ENGINE_ERROR_PARAM;
    }

    if (!reactor->fd_contexts[fd].active) {
        return ENGINE_ERROR_NOTFOUND;
    }

    memset(&ev, 0, sizeof(ev));
    ev.events = reactor_to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(reactor->epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
        fprintf(stderr, "[REACTOR] epoll_ctl MOD failed for fd=%d: %s\n",
                fd, strerror(errno));
        return ENGINE_ERROR_IO;
    }

    reactor->fd_contexts[fd].events = events;

    return ENGINE_OK;
}

int reactor_del(EpollReactor* reactor, int fd)
{
    if (!reactor || fd < 0 || fd >= REACTOR_MAX_FDS) {
        return ENGINE_ERROR_PARAM;
    }

    if (!reactor->fd_contexts[fd].active) {
        return ENGINE_ERROR_NOTFOUND;
    }

    /* Remove from epoll */
    epoll_ctl(reactor->epoll_fd, EPOLL_CTL_DEL, fd, NULL);

    /* Clear context */
    memset(&reactor->fd_contexts[fd], 0, sizeof(ReactorFdContext));

    if (reactor->fd_count > 0) {
        reactor->fd_count--;
    }

    return ENGINE_OK;
}

int reactor_poll(EpollReactor* reactor, int timeout_ms)
{
    struct epoll_event* events;
    int nfds;
    int i;
    int fd;
    u32 ev;
    ReactorFdContext* ctx;

    if (!reactor || !reactor->epoll_events) {
        return ENGINE_ERROR_PARAM;
    }

    events = (struct epoll_event*)reactor->epoll_events;

    nfds = epoll_wait(reactor->epoll_fd, events, REACTOR_MAX_EVENTS, timeout_ms);

    if (nfds < 0) {
        if (errno == EINTR) {
            return 0;  /* Interrupted, not an error */
        }
        fprintf(stderr, "[REACTOR] epoll_wait failed: %s\n", strerror(errno));
        return ENGINE_ERROR_IO;
    }

    reactor->loop_count++;

    /* Process events */
    for (i = 0; i < nfds; i++) {
        fd = events[i].data.fd;

        if (fd < 0 || fd >= REACTOR_MAX_FDS) {
            continue;
        }

        ctx = &reactor->fd_contexts[fd];
        if (!ctx->active || !ctx->callback) {
            continue;
        }

        ev = epoll_to_reactor_events(events[i].events);
        ctx->callback(reactor, fd, ev, ctx->user_data);
        reactor->event_count++;
    }

    return nfds;
}

void reactor_run(EpollReactor* reactor, int timeout_ms)
{
    if (!reactor) {
        return;
    }

    reactor->running = 1;

    while (reactor->running) {
        reactor_poll(reactor, timeout_ms);
    }
}

void reactor_stop(EpollReactor* reactor)
{
    if (reactor) {
        reactor->running = 0;
    }
}

int reactor_is_running(const EpollReactor* reactor)
{
    return reactor ? reactor->running : 0;
}

void reactor_get_stats(const EpollReactor* reactor,
                       u32* fd_count, u64* event_count, u64* loop_count)
{
    if (!reactor) {
        if (fd_count) *fd_count = 0;
        if (event_count) *event_count = 0;
        if (loop_count) *loop_count = 0;
        return;
    }

    if (fd_count) *fd_count = reactor->fd_count;
    if (event_count) *event_count = reactor->event_count;
    if (loop_count) *loop_count = reactor->loop_count;
}
