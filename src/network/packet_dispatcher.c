/*
 * PacketDispatcher - Packet dispatcher implementation
 *
 * Features:
 * - Maintains packet_type to handler mapping
 * - Dispatches packets to appropriate handlers
 * - Registration/unregistration interface
 * - Statistics tracking
 *
 * Implementation:
 * - Fixed-size array (max 512 handlers)
 * - Linear search (O(n), fast enough for hundreds of handlers)
 * - Thread-safe (pthread_mutex)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "network/headers/PacketDispatcher.h"
#include "security/headers/CHackAnalyzer.h"

/* Internal data structures */

#define MAX_HANDLERS 512

typedef struct {
    PacketHandlerInfo handlers[MAX_HANDLERS];
    int handler_count;
    pthread_mutex_t lock;

    unsigned long total_dispatched;
    unsigned long unknown_packets;
    unsigned long dispatch_errors;
} DispatcherRegistry;

static DispatcherRegistry g_registry = {0};
static int g_initialized = 0;

/* Internal helper functions */

static PacketHandlerInfo* FindHandler(uint16_t packet_type)
{
    int i;
    for (i = 0; i < g_registry.handler_count; i++) {
        if (g_registry.handlers[i].packet_type == packet_type) {
            return &g_registry.handlers[i];
        }
    }
    return NULL;
}

/* Public API */

int PacketDispatcher_Initialize(void)
{
    if (g_initialized) {
        fprintf(stderr, "[PacketDispatcher] WARNING: Already initialized\n");
        return 0;
    }

    memset(&g_registry, 0, sizeof(g_registry));
    pthread_mutex_init(&g_registry.lock, NULL);

    g_initialized = 1;

    printf("[PacketDispatcher] Initialized\n");
    return 0;
}

void PacketDispatcher_Cleanup(void)
{
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_registry.lock);

    printf("[PacketDispatcher] Cleanup: %d handlers, %lu dispatched, %lu unknown\n",
           g_registry.handler_count,
           g_registry.total_dispatched,
           g_registry.unknown_packets);

    memset(&g_registry, 0, sizeof(g_registry));

    pthread_mutex_unlock(&g_registry.lock);
    pthread_mutex_destroy(&g_registry.lock);

    g_initialized = 0;
}

int PacketDispatcher_Register(uint16_t packet_type, PacketHandler handler, const char *name)
{
    if (!g_initialized) {
        fprintf(stderr, "[PacketDispatcher] ERROR: Not initialized\n");
        return -1;
    }

    if (!handler) {
        fprintf(stderr, "[PacketDispatcher] ERROR: Handler cannot be NULL\n");
        return -1;
    }

    pthread_mutex_lock(&g_registry.lock);

    PacketHandlerInfo *existing = FindHandler(packet_type);
    if (existing) {
        printf("[PacketDispatcher] Replacing handler for type %d (old='%s', new='%s')\n",
               packet_type, existing->name, name ? name : "unnamed");
        existing->handler = handler;
        existing->name = name;
        existing->enabled = 1;
        pthread_mutex_unlock(&g_registry.lock);
        return 0;
    }

    if (g_registry.handler_count >= MAX_HANDLERS) {
        fprintf(stderr, "[PacketDispatcher] ERROR: Registry full (%d/%d)\n",
                g_registry.handler_count, MAX_HANDLERS);
        pthread_mutex_unlock(&g_registry.lock);
        return -1;
    }

    PacketHandlerInfo *info = &g_registry.handlers[g_registry.handler_count];
    info->packet_type = packet_type;
    info->handler = handler;
    info->name = name ? name : "unnamed";
    info->enabled = 1;

    g_registry.handler_count++;

    printf("[PacketDispatcher] Registered type %d: '%s' (%d/%d)\n",
           packet_type, info->name, g_registry.handler_count, MAX_HANDLERS);

    pthread_mutex_unlock(&g_registry.lock);
    return 0;
}

int PacketDispatcher_Unregister(uint16_t packet_type)
{
    if (!g_initialized) {
        return -1;
    }

    pthread_mutex_lock(&g_registry.lock);

    PacketHandlerInfo *info = FindHandler(packet_type);
    if (!info) {
        pthread_mutex_unlock(&g_registry.lock);
        return -1;
    }

    int index = info - g_registry.handlers;
    if (index < g_registry.handler_count - 1) {
        g_registry.handlers[index] = g_registry.handlers[g_registry.handler_count - 1];
    }

    g_registry.handler_count--;

    printf("[PacketDispatcher] Unregistered type %d (%d remaining)\n",
           packet_type, g_registry.handler_count);

    pthread_mutex_unlock(&g_registry.lock);
    return 0;
}

int PacketDispatcher_Dispatch(int fd, PacketBuf *packet)
{
    if (!g_initialized) {
        fprintf(stderr, "[PacketDispatcher] ERROR: Not initialized\n");
        return -1;
    }

    if (!packet || !packet->data) {
        fprintf(stderr, "[PacketDispatcher] ERROR: Invalid packet\n");
        return -1;
    }

    tagPacketHeader *header = (tagPacketHeader*)packet->data;
    uint16_t packet_type = header->type;

    pthread_mutex_lock(&g_registry.lock);

    g_registry.total_dispatched++;

    PacketHandlerInfo *info = FindHandler(packet_type);
    if (!info) {
        g_registry.unknown_packets++;
        pthread_mutex_unlock(&g_registry.lock);

        if (g_registry.unknown_packets % 100 == 1) {
            fprintf(stderr, "[PacketDispatcher] WARNING: No handler for type %d (fd=%d, total=%lu)\n",
                    packet_type, fd, g_registry.unknown_packets);
        }
        return 0;
    }

    if (!info->enabled) {
        pthread_mutex_unlock(&g_registry.lock);
        printf("[PacketDispatcher] Type %d disabled, ignoring (fd=%d)\n", packet_type, fd);
        return 0;
    }

    PacketHandler handler = info->handler;
    const char *handler_name = info->name;
    pthread_mutex_unlock(&g_registry.lock);

    printf("[PacketDispatcher] Dispatching type %d to '%s' (fd=%d, len=%d)\n",
           packet_type, handler_name, fd, header->length);

    int result = handler(fd, packet);

    if (result < 0) {
        pthread_mutex_lock(&g_registry.lock);
        g_registry.dispatch_errors++;
        pthread_mutex_unlock(&g_registry.lock);

        fprintf(stderr, "[PacketDispatcher] Handler '%s' error %d (fd=%d), closing connection\n",
                handler_name, result, fd);
    }

    return result;
}

const PacketHandlerInfo* PacketDispatcher_GetHandlerInfo(uint16_t packet_type)
{
    if (!g_initialized) {
        return NULL;
    }

    pthread_mutex_lock(&g_registry.lock);
    PacketHandlerInfo *info = FindHandler(packet_type);
    pthread_mutex_unlock(&g_registry.lock);

    return info;
}

int PacketDispatcher_EnableHandler(uint16_t packet_type, int enabled)
{
    if (!g_initialized) {
        return -1;
    }

    pthread_mutex_lock(&g_registry.lock);

    PacketHandlerInfo *info = FindHandler(packet_type);
    if (!info) {
        pthread_mutex_unlock(&g_registry.lock);
        return -1;
    }

    info->enabled = enabled;

    printf("[PacketDispatcher] Handler type %d '%s': %s\n",
           packet_type, info->name, enabled ? "ENABLED" : "DISABLED");

    pthread_mutex_unlock(&g_registry.lock);
    return 0;
}

void PacketDispatcher_GetStatistics(int *total_handlers, int *enabled_handlers,
                                    unsigned long *total_dispatched, unsigned long *unknown_packets)
{
    if (!g_initialized) {
        if (total_handlers) *total_handlers = 0;
        if (enabled_handlers) *enabled_handlers = 0;
        if (total_dispatched) *total_dispatched = 0;
        if (unknown_packets) *unknown_packets = 0;
        return;
    }

    pthread_mutex_lock(&g_registry.lock);

    if (total_handlers) {
        *total_handlers = g_registry.handler_count;
    }

    if (enabled_handlers) {
        int count = 0;
        int i;
        for (i = 0; i < g_registry.handler_count; i++) {
            if (g_registry.handlers[i].enabled) {
                count++;
            }
        }
        *enabled_handlers = count;
    }

    if (total_dispatched) {
        *total_dispatched = g_registry.total_dispatched;
    }

    if (unknown_packets) {
        *unknown_packets = g_registry.unknown_packets;
    }

    pthread_mutex_unlock(&g_registry.lock);
}

void PacketDispatcher_PrintRegistry(void)
{
    if (!g_initialized) {
        printf("[PacketDispatcher] Not initialized\n");
        return;
    }

    pthread_mutex_lock(&g_registry.lock);

    printf("\n========================================\n");
    printf("PacketDispatcher Registry\n");
    printf("========================================\n");
    printf("Total handlers: %d/%d\n", g_registry.handler_count, MAX_HANDLERS);
    printf("Total dispatched: %lu\n", g_registry.total_dispatched);
    printf("Unknown packets: %lu\n", g_registry.unknown_packets);
    printf("Dispatch errors: %lu\n", g_registry.dispatch_errors);
    printf("\nRegistered handlers:\n");
    printf("%-10s %-30s %-10s\n", "Type", "Name", "Status");
    printf("----------------------------------------\n");

    int i;
    for (i = 0; i < g_registry.handler_count; i++) {
        PacketHandlerInfo *info = &g_registry.handlers[i];
        printf("%-10d %-30s %-10s\n",
               info->packet_type,
               info->name,
               info->enabled ? "ENABLED" : "DISABLED");
    }

    printf("========================================\n\n");

    pthread_mutex_unlock(&g_registry.lock);
}
