/*
 * auction_forward.h - Auction Service Forwarding for Game Server
 * Phase 16 P4: Forward auction requests to independent Auction service
 */

#ifndef AUCTION_FORWARD_H
#define AUCTION_FORWARD_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Auction service connection state */
typedef enum {
    AUCTION_CONN_DISCONNECTED = 0,
    AUCTION_CONN_CONNECTING,
    AUCTION_CONN_CONNECTED
} AuctionConnectionState;

/* Initialize auction forwarding module */
int auction_forward_init(const char* auction_host, uint16_t auction_port);
void auction_forward_cleanup(void);

/* Connection management */
int auction_forward_connect(void);
void auction_forward_disconnect(void);
int auction_forward_is_connected(void);

/* Forward request to auction service */
int auction_forward_request(ClientSession* s, uint16_t msg_type,
                            const uint8_t* data, int len);

/* Process auction service responses */
int auction_forward_process_responses(void);

/* Check if message should be forwarded to auction service */
int auction_should_forward(uint16_t msg_type);

#ifdef __cplusplus
}
#endif

#endif /* AUCTION_FORWARD_H */
