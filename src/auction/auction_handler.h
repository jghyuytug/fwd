/*
 * auction_handler.h - Auction Protocol Handlers
 * Phase 16 P4: Independent Auction Service
 */

#ifndef AUCTION_HANDLER_H
#define AUCTION_HANDLER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Client session info (passed from Game via IPC) */
typedef struct {
    uint32_t player_id;
    char     player_name[32];
    uint64_t gold;
    uint32_t request_fd;   /* Game server fd to respond to */
    uint32_t request_id;   /* Request correlation ID */
} AuctionClientContext;

/* Handler prototypes - msg_no 170-178 */
int auction_handle_search(AuctionClientContext* ctx, const uint8_t* data, int len,
                          uint8_t* response, int* resp_len);

int auction_handle_list_item(AuctionClientContext* ctx, const uint8_t* data, int len,
                             uint8_t* response, int* resp_len);

int auction_handle_cancel(AuctionClientContext* ctx, const uint8_t* data, int len,
                          uint8_t* response, int* resp_len);

int auction_handle_buy(AuctionClientContext* ctx, const uint8_t* data, int len,
                       uint8_t* response, int* resp_len);

int auction_handle_get_my_listings(AuctionClientContext* ctx, const uint8_t* data, int len,
                                   uint8_t* response, int* resp_len);

int auction_handle_get_history(AuctionClientContext* ctx, const uint8_t* data, int len,
                               uint8_t* response, int* resp_len);

int auction_handle_bid(AuctionClientContext* ctx, const uint8_t* data, int len,
                       uint8_t* response, int* resp_len);

int auction_handle_get_price_history(AuctionClientContext* ctx, const uint8_t* data, int len,
                                     uint8_t* response, int* resp_len);

int auction_handle_collect_gold(AuctionClientContext* ctx, const uint8_t* data, int len,
                                uint8_t* response, int* resp_len);

/* Dispatcher */
int auction_dispatch_message(uint16_t msg_type, AuctionClientContext* ctx,
                             const uint8_t* data, int len,
                             uint8_t* response, int* resp_len);

#ifdef __cplusplus
}
#endif

#endif /* AUCTION_HANDLER_H */
