/*
 * auction_manager.h - In-memory Auction Book Manager
 * Phase 16 P4: Independent Auction Service
 */

#ifndef AUCTION_MANAGER_H
#define AUCTION_MANAGER_H

#include "auction_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize/cleanup */
int auction_manager_init(void);
void auction_manager_cleanup(void);

/* Listing operations */
AuctionListing* auction_create_listing(
    uint32_t seller_id, const char* seller_name,
    uint32_t item_uid, uint32_t item_id,
    uint64_t price, uint32_t duration_hours
);

AuctionListing* auction_find_listing(uint64_t listing_id);
int auction_cancel_listing(uint64_t listing_id, uint32_t seller_id);
int auction_buy_listing(uint64_t listing_id, uint32_t buyer_id);
int auction_place_bid(uint64_t listing_id, uint32_t bidder_id, uint64_t bid_amount);

/* Search operations */
int auction_search(
    const AuctionSearchRequest* req,
    AuctionListing** results,
    int max_results
);

int auction_get_seller_listings(
    uint32_t seller_id,
    AuctionListing** results,
    int max_results
);

/* Expired listings cleanup */
int auction_process_expired(void);

/* Statistics */
int auction_get_active_count(void);
int auction_get_total_volume(uint64_t* volume);

/* Price history */
int auction_get_price_history(uint32_t item_id, AuctionPriceHistory* history);
void auction_update_price_history(uint32_t item_id, uint64_t sold_price);

/* Pending gold for sellers (sold items) */
uint64_t auction_get_pending_gold(uint32_t seller_id);
uint64_t auction_collect_pending_gold(uint32_t seller_id);

#ifdef __cplusplus
}
#endif

#endif /* AUCTION_MANAGER_H */
