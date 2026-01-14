/*
 * auction_ipc.h - IPC Communication with Game Service
 * Phase 16 P4: Independent Auction Service
 */

#ifndef AUCTION_IPC_H
#define AUCTION_IPC_H

#include <stdint.h>
#include "auction_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IPC connection state */
typedef enum {
    IPC_STATE_DISCONNECTED = 0,
    IPC_STATE_CONNECTING,
    IPC_STATE_CONNECTED
} IPCConnectionState;

/* Game service connection info */
typedef struct {
    int fd;
    uint32_t ip;
    uint16_t port;
    uint8_t state;
    uint32_t last_heartbeat;
    uint8_t recv_buffer[65536];
    int recv_len;
} GameServiceConnection;

/* Initialize IPC module */
int auction_ipc_init(const char* game_host, uint16_t game_port);
void auction_ipc_cleanup(void);

/* Connection management */
int auction_ipc_connect(void);
void auction_ipc_disconnect(void);
int auction_ipc_is_connected(void);

/* Send response back to Game service */
int auction_ipc_send_response(uint32_t request_id, uint16_t msg_type,
                               const uint8_t* data, int len);

/* Process incoming IPC messages from Game */
int auction_ipc_process(void);

/* Heartbeat */
int auction_ipc_send_heartbeat(void);

/* Register service with Monitor */
int auction_ipc_register_with_monitor(const char* monitor_host, uint16_t monitor_port,
                                       const char* service_name, uint16_t service_port);

#ifdef __cplusplus
}
#endif

#endif /* AUCTION_IPC_H */
