/*
 * auction_types.h - Auction Service Data Types
 * Phase 16 P4: Independent Auction Service
 */

#ifndef AUCTION_TYPES_H
#define AUCTION_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Auction listing states */
typedef enum {
    AUCTION_STATE_ACTIVE = 0,
    AUCTION_STATE_SOLD,
    AUCTION_STATE_CANCELLED,
    AUCTION_STATE_EXPIRED
} AuctionState;

/* In-memory auction listing */
typedef struct {
    uint64_t listing_id;
    uint32_t seller_id;
    char     seller_name[32];
    uint32_t item_id;
    uint32_t item_uid;
    uint64_t price;
    uint64_t instant_price;
    uint64_t current_bid;
    uint32_t current_bidder;
    uint32_t expire_time;
    uint32_t create_time;
    uint8_t  state;
    uint8_t  active;
    /* Item attributes */
    uint8_t  upgrade_level;
    uint8_t  amplify_option;
    int32_t  amplify_value;
    uint16_t endurance;
    uint8_t  random_option[14];
    uint8_t  item_guid[10];
} AuctionListing;

/* Auction search request */
typedef struct {
    uint32_t item_id;
    uint32_t min_price;
    uint32_t max_price;
    uint8_t  sort_type;     /* 0=price_asc, 1=price_desc, 2=time_asc, 3=time_desc */
    uint16_t page;
    uint16_t page_size;
} AuctionSearchRequest;

/* Auction history entry */
typedef struct {
    uint64_t listing_id;
    uint32_t item_id;
    uint64_t price;
    uint32_t buyer_id;
    char     buyer_name[32];
    uint32_t sold_time;
    uint8_t  state;
} AuctionHistoryEntry;

/* Price history for an item */
typedef struct {
    uint32_t item_id;
    uint64_t avg_price;
    uint64_t min_price;
    uint64_t max_price;
    uint32_t total_sold;
    uint32_t last_update;
} AuctionPriceHistory;

/* IPC message types between Game and Auction */
typedef enum {
    IPC_AUCTION_SEARCH = 170,
    IPC_AUCTION_LIST_ITEM = 171,
    IPC_AUCTION_CANCEL = 172,
    IPC_AUCTION_BUY = 173,
    IPC_AUCTION_GET_MY_LISTINGS = 174,
    IPC_AUCTION_GET_HISTORY = 175,
    IPC_AUCTION_BID = 176,
    IPC_AUCTION_GET_PRICE_HISTORY = 177,
    IPC_AUCTION_COLLECT_GOLD = 178
} AuctionIPCMsgType;

/* IPC request header */
typedef struct {
    uint16_t msg_type;
    uint16_t data_len;
    uint32_t player_id;
    uint32_t request_id;
} AuctionIPCHeader;

/* Configuration */
#define MAX_AUCTION_LISTINGS 10000
#define MAX_SEARCH_RESULTS 100
#define MAX_MY_LISTINGS 50
#define MAX_HISTORY_ENTRIES 100
#define AUCTION_DURATION_MIN 1      /* 1 hour min */
#define AUCTION_DURATION_MAX 48     /* 48 hours max */
#define LISTING_FEE_PERCENT 5
#define LISTING_FEE_MIN 1000
#define SELL_TAX_PERCENT 5

#ifdef __cplusplus
}
#endif

#endif /* AUCTION_TYPES_H */
