/**
 * @file engine.h
 * @brief Game Engine main header
 *
 * Include this file to access all engine components.
 */

#ifndef ENGINE_H
#define ENGINE_H

/* Core types */
#include "core/types.h"

/* Network */
#include "core/network/epoll_reactor.h"
#include "core/network/tcp_listener.h"
#include "core/network/tcp_connection.h"
#include "core/network/buffer.h"

/* Event system */
#include "core/event/event_types.h"
#include "core/event/event_bus.h"

/* Timer */
#include "core/timer/timer_wheel.h"

/* Logging */
#include "core/log/logger.h"

/* Configuration */
#include "core/config/config_loader.h"

/* Memory */
#include "core/memory/pool_allocator.h"

/* Data layer */
#include "data/orm/orm_wrapper.h"
#include "data/cache/lru_cache.h"

/* Session layer */
#include "session/session.h"
#include "session/session_manager.h"

/* Protocol layer */
#include "protocol/message_router.h"
#include "protocol/codec/packet_codec.h"

/* IPC layer */
#include "ipc/ipc_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Engine version */
#define ENGINE_VERSION_MAJOR 1
#define ENGINE_VERSION_MINOR 0
#define ENGINE_VERSION_PATCH 0
#define ENGINE_VERSION_STRING "1.0.0"

/**
 * Get engine version string
 * @return Version string
 */
static const char* engine_version(void)
{
    return ENGINE_VERSION_STRING;
}

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_H */
