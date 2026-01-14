/**
 * @file message_router.c
 * @brief Protocol message router implementation
 */

#include "message_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Find handler by message number */
static MessageHandlerEntry* find_handler(MessageRouter* router, u16 msg_no)
{
    int i;

    for (i = 0; i < router->handler_count; i++) {
        if (router->handlers[i].msg_no == msg_no) {
            return &router->handlers[i];
        }
    }

    return NULL;
}

int router_init(MessageRouter* router)
{
    if (!router) {
        return ENGINE_ERROR_PARAM;
    }

    memset(router, 0, sizeof(MessageRouter));
    return ENGINE_OK;
}

void router_cleanup(MessageRouter* router)
{
    if (router) {
        memset(router, 0, sizeof(MessageRouter));
    }
}

int router_register(MessageRouter* router, u16 msg_no,
                    MessageHandler handler, const char* name, u8 flags)
{
    return router_register_limited(router, msg_no, handler, name, flags, 0);
}

int router_register_limited(MessageRouter* router, u16 msg_no,
                            MessageHandler handler, const char* name,
                            u8 flags, u32 interval_ms)
{
    MessageHandlerEntry* entry;

    if (!router || !handler) {
        return ENGINE_ERROR_PARAM;
    }

    /* Check if already registered */
    entry = find_handler(router, msg_no);
    if (entry) {
        /* Update existing */
        entry->handler = handler;
        if (name) {
            strncpy(entry->name, name, ROUTER_HANDLER_NAME_MAX - 1);
        }
        entry->flags = flags;
        entry->min_interval_ms = interval_ms;
        return ENGINE_OK;
    }

    /* Add new */
    if (router->handler_count >= ROUTER_MAX_HANDLERS) {
        return ENGINE_ERROR_FULL;
    }

    entry = &router->handlers[router->handler_count];
    entry->msg_no = msg_no;
    entry->handler = handler;
    entry->flags = flags;
    entry->min_interval_ms = interval_ms;
    entry->call_count = 0;
    entry->error_count = 0;
    entry->total_time_us = 0;

    if (name) {
        strncpy(entry->name, name, ROUTER_HANDLER_NAME_MAX - 1);
        entry->name[ROUTER_HANDLER_NAME_MAX - 1] = '\0';
    } else {
        snprintf(entry->name, ROUTER_HANDLER_NAME_MAX, "MSG_%u", msg_no);
    }

    router->handler_count++;

    return ENGINE_OK;
}

int router_unregister(MessageRouter* router, u16 msg_no)
{
    int i;
    MessageHandlerEntry* entry;

    if (!router) {
        return ENGINE_ERROR_PARAM;
    }

    for (i = 0; i < router->handler_count; i++) {
        if (router->handlers[i].msg_no == msg_no) {
            /* Shift remaining handlers */
            entry = &router->handlers[i];
            memmove(entry, entry + 1,
                    sizeof(MessageHandlerEntry) * (router->handler_count - i - 1));
            router->handler_count--;
            return ENGINE_OK;
        }
    }

    return ENGINE_ERROR_NOTFOUND;
}

void router_set_default(MessageRouter* router, MessageHandler handler)
{
    if (router) {
        router->default_handler = handler;
    }
}

int router_dispatch(MessageRouter* router, Session* session,
                    u16 msg_no, const u8* data, int len)
{
    MessageHandlerEntry* entry;
    int result;

    if (!router || !session) {
        return HANDLER_ERROR;
    }

    router->total_messages++;

    /* Find handler */
    entry = find_handler(router, msg_no);

    if (!entry) {
        /* Try default handler */
        if (router->default_handler) {
            return router->default_handler(session, data, len);
        }

        router->unhandled_messages++;
        return HANDLER_OK;  /* Silently ignore */
    }

    /* Check authentication requirements */
    if ((entry->flags & HANDLER_FLAG_REQUIRE_AUTH) &&
        !session_is_authenticated(session)) {
        router->rejected_messages++;
        return HANDLER_NOT_AUTHENTICATED;
    }

    /* Check character selection requirement */
    if ((entry->flags & HANDLER_FLAG_REQUIRE_CHAR) &&
        session->character_id == 0) {
        router->rejected_messages++;
        return HANDLER_INVALID_STATE;
    }

    /* Check in-game requirement */
    if ((entry->flags & HANDLER_FLAG_REQUIRE_GAME) &&
        !session_is_in_game(session)) {
        router->rejected_messages++;
        return HANDLER_INVALID_STATE;
    }

    /* Check admin requirement */
    if ((entry->flags & HANDLER_FLAG_ADMIN_ONLY) &&
        session->gm_level == 0) {
        router->rejected_messages++;
        return HANDLER_NOT_AUTHENTICATED;
    }

    /* Call handler */
    entry->call_count++;

    result = entry->handler(session, data, len);

    if (result < 0) {
        entry->error_count++;
    }

    /* Update session activity */
    session_touch(session);

    return result;
}

const MessageHandlerEntry* router_get_handler(const MessageRouter* router,
                                               u16 msg_no)
{
    int i;

    if (!router) {
        return NULL;
    }

    for (i = 0; i < router->handler_count; i++) {
        if (router->handlers[i].msg_no == msg_no) {
            return &router->handlers[i];
        }
    }

    return NULL;
}

int router_handler_count(const MessageRouter* router)
{
    return router ? router->handler_count : 0;
}

void router_get_stats(const MessageRouter* router,
                      u64* total_messages, u64* unhandled, u64* rejected)
{
    if (!router) {
        if (total_messages) *total_messages = 0;
        if (unhandled) *unhandled = 0;
        if (rejected) *rejected = 0;
        return;
    }

    if (total_messages) *total_messages = router->total_messages;
    if (unhandled) *unhandled = router->unhandled_messages;
    if (rejected) *rejected = router->rejected_messages;
}

void router_dump_handlers(const MessageRouter* router)
{
    int i;
    const MessageHandlerEntry* entry;

    if (!router) {
        return;
    }

    printf("=== Message Router Handlers (%d) ===\n", router->handler_count);

    for (i = 0; i < router->handler_count; i++) {
        entry = &router->handlers[i];
        printf("  [%3u] %-24s flags=0x%02X calls=%lu errors=%lu\n",
               entry->msg_no, entry->name, entry->flags,
               (unsigned long)entry->call_count,
               (unsigned long)entry->error_count);
    }

    printf("Total: %lu, Unhandled: %lu, Rejected: %lu\n",
           (unsigned long)router->total_messages,
           (unsigned long)router->unhandled_messages,
           (unsigned long)router->rejected_messages);
}
