/**
 * @file message_router.h
 * @brief Protocol message router
 *
 * Routes incoming messages to registered handlers.
 */

#ifndef ENGINE_PROTOCOL_MESSAGE_ROUTER_H
#define ENGINE_PROTOCOL_MESSAGE_ROUTER_H

#include "../core/types.h"
#include "../session/session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROUTER_MAX_HANDLERS 1024
#define ROUTER_HANDLER_NAME_MAX 32

/* Handler result codes */
typedef enum {
    HANDLER_OK = 0,
    HANDLER_ERROR = -1,
    HANDLER_DISCONNECT = -2,
    HANDLER_NOT_AUTHENTICATED = -3,
    HANDLER_INVALID_STATE = -4
} HandlerResult;

/* Message handler function */
typedef int (*MessageHandler)(Session* session, const u8* data, int len);

/* Handler flags */
#define HANDLER_FLAG_NONE           0x00
#define HANDLER_FLAG_REQUIRE_AUTH   0x01
#define HANDLER_FLAG_REQUIRE_CHAR   0x02
#define HANDLER_FLAG_REQUIRE_GAME   0x04
#define HANDLER_FLAG_ADMIN_ONLY     0x08

/* Handler entry */
typedef struct {
    u16             msg_no;
    MessageHandler  handler;
    char            name[ROUTER_HANDLER_NAME_MAX];
    u8              flags;
    u32             min_interval_ms;    /* Rate limiting */
    /* Statistics */
    u64             call_count;
    u64             error_count;
    u64             total_time_us;
} MessageHandlerEntry;

/* Message router */
typedef struct {
    MessageHandlerEntry handlers[ROUTER_MAX_HANDLERS];
    int                 handler_count;
    MessageHandler      default_handler;
    /* Statistics */
    u64                 total_messages;
    u64                 unhandled_messages;
    u64                 rejected_messages;
} MessageRouter;

/**
 * Initialize message router
 * @param router Router instance
 * @return ENGINE_OK on success
 */
int router_init(MessageRouter* router);

/**
 * Cleanup message router
 * @param router Router instance
 */
void router_cleanup(MessageRouter* router);

/**
 * Register a message handler
 * @param router  Router instance
 * @param msg_no  Message number
 * @param handler Handler function
 * @param name    Handler name (for debugging)
 * @param flags   Handler flags
 * @return ENGINE_OK on success
 */
int router_register(MessageRouter* router, u16 msg_no,
                    MessageHandler handler, const char* name, u8 flags);

/**
 * Register handler with rate limiting
 * @param router      Router instance
 * @param msg_no      Message number
 * @param handler     Handler function
 * @param name        Handler name
 * @param flags       Handler flags
 * @param interval_ms Minimum interval between calls
 * @return ENGINE_OK on success
 */
int router_register_limited(MessageRouter* router, u16 msg_no,
                            MessageHandler handler, const char* name,
                            u8 flags, u32 interval_ms);

/**
 * Unregister a handler
 * @param router Router instance
 * @param msg_no Message number
 * @return ENGINE_OK on success
 */
int router_unregister(MessageRouter* router, u16 msg_no);

/**
 * Set default handler for unregistered messages
 * @param router  Router instance
 * @param handler Default handler function
 */
void router_set_default(MessageRouter* router, MessageHandler handler);

/**
 * Dispatch a message to its handler
 * @param router  Router instance
 * @param session Client session
 * @param msg_no  Message number
 * @param data    Message data
 * @param len     Data length
 * @return Handler result
 */
int router_dispatch(MessageRouter* router, Session* session,
                    u16 msg_no, const u8* data, int len);

/**
 * Get handler info
 * @param router Router instance
 * @param msg_no Message number
 * @return Handler entry or NULL
 */
const MessageHandlerEntry* router_get_handler(const MessageRouter* router,
                                               u16 msg_no);

/**
 * Get registered handler count
 * @param router Router instance
 * @return Number of registered handlers
 */
int router_handler_count(const MessageRouter* router);

/**
 * Get router statistics
 * @param router            Router instance
 * @param total_messages    Output: total dispatched
 * @param unhandled         Output: unhandled messages
 * @param rejected          Output: rejected messages
 */
void router_get_stats(const MessageRouter* router,
                      u64* total_messages, u64* unhandled, u64* rejected);

/**
 * Dump handler list for debugging
 * @param router Router instance
 */
void router_dump_handlers(const MessageRouter* router);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_PROTOCOL_MESSAGE_ROUTER_H */
