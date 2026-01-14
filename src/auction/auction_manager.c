/*
 * auction_manager.c - In-memory Auction Book Manager
 * Phase 16 P4: Independent Auction Service
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auction_manager.h"

/* Global auction book */
static AuctionListing g_listings[MAX_AUCTION_LISTINGS];
static int g_listing_count = 0;
static uint64_t g_next_listing_id = 1;

/* Pending gold for sellers (from sold items) */
typedef struct {
    uint32_t seller_id;
    uint64_t pending_gold;
} PendingGoldEntry;

#define MAX_PENDING_ENTRIES 1000
static PendingGoldEntry g_pending_gold[MAX_PENDING_ENTRIES];
static int g_pending_count = 0;

/* Price history cache */
#define MAX_PRICE_HISTORY 500
static AuctionPriceHistory g_price_history[MAX_PRICE_HISTORY];
static int g_price_history_count = 0;

/*============================================================================
 * Initialization
 *============================================================================*/

int auction_manager_init(void) {
    memset(g_listings, 0, sizeof(g_listings));
    g_listing_count = 0;
    g_next_listing_id = 1;

    memset(g_pending_gold, 0, sizeof(g_pending_gold));
    g_pending_count = 0;

    memset(g_price_history, 0, sizeof(g_price_history));
    g_price_history_count = 0;

    printf("[AUCTION_MGR] Initialized with capacity %d listings\n", MAX_AUCTION_LISTINGS);
    return 0;
}

void auction_manager_cleanup(void) {
    g_listing_count = 0;
    g_pending_count = 0;
    printf("[AUCTION_MGR] Cleanup complete\n");
}

/*============================================================================
 * Listing Operations
 *============================================================================*/

AuctionListing* auction_create_listing(
    uint32_t seller_id, const char* seller_name,
    uint32_t item_uid, uint32_t item_id,
    uint64_t price, uint32_t duration_hours)
{
    if (g_listing_count >= MAX_AUCTION_LISTINGS) {
        printf("[AUCTION_MGR] ERROR: Max listings reached\n");
        return NULL;
    }

    /* Find free slot */
    AuctionListing* listing = NULL;
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (!g_listings[i].active) {
            listing = &g_listings[i];
            break;
        }
    }

    if (!listing) {
        printf("[AUCTION_MGR] ERROR: No free slot found\n");
        return NULL;
    }

    /* Validate duration */
    if (duration_hours < AUCTION_DURATION_MIN) duration_hours = AUCTION_DURATION_MIN;
    if (duration_hours > AUCTION_DURATION_MAX) duration_hours = AUCTION_DURATION_MAX;

    /* Initialize listing */
    memset(listing, 0, sizeof(AuctionListing));
    listing->listing_id = g_next_listing_id++;
    listing->seller_id = seller_id;
    if (seller_name) {
        strncpy(listing->seller_name, seller_name, sizeof(listing->seller_name) - 1);
    }
    listing->item_uid = item_uid;
    listing->item_id = item_id;
    listing->price = price;
    listing->instant_price = price; /* Direct buy price */
    listing->current_bid = 0;
    listing->current_bidder = 0;
    listing->create_time = (uint32_t)time(NULL);
    listing->expire_time = listing->create_time + (duration_hours * 3600);
    listing->state = AUCTION_STATE_ACTIVE;
    listing->active = 1;

    g_listing_count++;

    printf("[AUCTION_MGR] Created listing %llu: seller=%u, item=%u, price=%llu, expires=%u\n",
           (unsigned long long)listing->listing_id, seller_id, item_id,
           (unsigned long long)price, listing->expire_time);

    return listing;
}

AuctionListing* auction_find_listing(uint64_t listing_id) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (g_listings[i].active && g_listings[i].listing_id == listing_id) {
            return &g_listings[i];
        }
    }
    return NULL;
}

int auction_cancel_listing(uint64_t listing_id, uint32_t seller_id) {
    AuctionListing* listing = auction_find_listing(listing_id);
    if (!listing) {
        return -1; /* Not found */
    }

    if (listing->seller_id != seller_id) {
        return -2; /* Not owner */
    }

    if (listing->state != AUCTION_STATE_ACTIVE) {
        return -3; /* Already sold/expired */
    }

    /* If there's a current bidder, refund needs to happen */
    if (listing->current_bidder != 0 && listing->current_bid > 0) {
        /* Add pending refund for bidder - handled by IPC */
        printf("[AUCTION_MGR] Listing %llu cancelled, refund %llu to bidder %u\n",
               (unsigned long long)listing_id,
               (unsigned long long)listing->current_bid,
               listing->current_bidder);
    }

    listing->state = AUCTION_STATE_CANCELLED;
    listing->active = 0;
    g_listing_count--;

    printf("[AUCTION_MGR] Cancelled listing %llu by seller %u\n",
           (unsigned long long)listing_id, seller_id);

    return 0;
}

int auction_buy_listing(uint64_t listing_id, uint32_t buyer_id) {
    AuctionListing* listing = auction_find_listing(listing_id);
    if (!listing) {
        return -1; /* Not found */
    }

    if (listing->seller_id == buyer_id) {
        return -2; /* Cannot buy own listing */
    }

    if (listing->state != AUCTION_STATE_ACTIVE) {
        return -3; /* Not available */
    }

    /* Mark as sold */
    listing->state = AUCTION_STATE_SOLD;
    listing->active = 0;
    g_listing_count--;

    /* Calculate seller proceeds (minus tax) */
    uint64_t tax = listing->price * SELL_TAX_PERCENT / 100;
    uint64_t proceeds = listing->price - tax;

    /* Add to seller's pending gold */
    int found = 0;
    for (int i = 0; i < g_pending_count; i++) {
        if (g_pending_gold[i].seller_id == listing->seller_id) {
            g_pending_gold[i].pending_gold += proceeds;
            found = 1;
            break;
        }
    }

    if (!found && g_pending_count < MAX_PENDING_ENTRIES) {
        g_pending_gold[g_pending_count].seller_id = listing->seller_id;
        g_pending_gold[g_pending_count].pending_gold = proceeds;
        g_pending_count++;
    }

    /* Update price history */
    auction_update_price_history(listing->item_id, listing->price);

    printf("[AUCTION_MGR] Sold listing %llu: buyer=%u, price=%llu, seller_proceeds=%llu\n",
           (unsigned long long)listing_id, buyer_id,
           (unsigned long long)listing->price, (unsigned long long)proceeds);

    return 0;
}

int auction_place_bid(uint64_t listing_id, uint32_t bidder_id, uint64_t bid_amount) {
    AuctionListing* listing = auction_find_listing(listing_id);
    if (!listing) {
        return -1;
    }

    if (listing->seller_id == bidder_id) {
        return -2; /* Cannot bid on own listing */
    }

    if (listing->state != AUCTION_STATE_ACTIVE) {
        return -3;
    }

    if (bid_amount <= listing->current_bid) {
        return -4; /* Bid too low */
    }

    /* Refund previous bidder - handled by IPC */
    uint32_t prev_bidder = listing->current_bidder;
    uint64_t prev_bid = listing->current_bid;

    /* Update bid */
    listing->current_bid = bid_amount;
    listing->current_bidder = bidder_id;

    printf("[AUCTION_MGR] Bid on listing %llu: bidder=%u, amount=%llu (prev: %u, %llu)\n",
           (unsigned long long)listing_id, bidder_id, (unsigned long long)bid_amount,
           prev_bidder, (unsigned long long)prev_bid);

    return 0;
}

/*============================================================================
 * Search Operations
 *============================================================================*/

int auction_search(
    const AuctionSearchRequest* req,
    AuctionListing** results,
    int max_results)
{
    if (!req || !results || max_results <= 0) {
        return 0;
    }

    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* l = &g_listings[i];

        if (!l->active || l->state != AUCTION_STATE_ACTIVE) {
            continue;
        }

        /* Check expiration */
        if (l->expire_time <= now) {
            continue;
        }

        /* Filter by item_id (0 = any) */
        if (req->item_id != 0 && l->item_id != req->item_id) {
            continue;
        }

        /* Filter by price range */
        if (req->min_price > 0 && l->price < req->min_price) {
            continue;
        }
        if (req->max_price > 0 && l->price > req->max_price) {
            continue;
        }

        results[count++] = l;
    }

    /* TODO: Sort by req->sort_type */
    /* TODO: Pagination with req->page and req->page_size */

    return count;
}

int auction_get_seller_listings(
    uint32_t seller_id,
    AuctionListing** results,
    int max_results)
{
    if (!results || max_results <= 0) {
        return 0;
    }

    int count = 0;

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* l = &g_listings[i];

        if (!l->active) {
            continue;
        }

        if (l->seller_id == seller_id) {
            results[count++] = l;
        }
    }

    return count;
}

/*============================================================================
 * Expiration Processing
 *============================================================================*/

int auction_process_expired(void) {
    uint32_t now = (uint32_t)time(NULL);
    int expired_count = 0;

    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        AuctionListing* l = &g_listings[i];

        if (!l->active || l->state != AUCTION_STATE_ACTIVE) {
            continue;
        }

        if (l->expire_time <= now) {
            /* Check if there was a winning bid */
            if (l->current_bidder != 0 && l->current_bid > 0) {
                /* Sell to highest bidder */
                l->state = AUCTION_STATE_SOLD;
                l->price = l->current_bid; /* Final price is winning bid */

                /* Add proceeds to seller */
                uint64_t tax = l->price * SELL_TAX_PERCENT / 100;
                uint64_t proceeds = l->price - tax;

                int found = 0;
                for (int j = 0; j < g_pending_count; j++) {
                    if (g_pending_gold[j].seller_id == l->seller_id) {
                        g_pending_gold[j].pending_gold += proceeds;
                        found = 1;
                        break;
                    }
                }
                if (!found && g_pending_count < MAX_PENDING_ENTRIES) {
                    g_pending_gold[g_pending_count].seller_id = l->seller_id;
                    g_pending_gold[g_pending_count].pending_gold = proceeds;
                    g_pending_count++;
                }

                auction_update_price_history(l->item_id, l->price);

                printf("[AUCTION_MGR] Listing %llu expired, sold to bidder %u for %llu\n",
                       (unsigned long long)l->listing_id, l->current_bidder,
                       (unsigned long long)l->price);
            } else {
                /* No bids, return item to seller */
                l->state = AUCTION_STATE_EXPIRED;
                printf("[AUCTION_MGR] Listing %llu expired, no bids, return to seller %u\n",
                       (unsigned long long)l->listing_id, l->seller_id);
            }

            l->active = 0;
            g_listing_count--;
            expired_count++;
        }
    }

    return expired_count;
}

/*============================================================================
 * Statistics
 *============================================================================*/

int auction_get_active_count(void) {
    return g_listing_count;
}

int auction_get_total_volume(uint64_t* volume) {
    if (!volume) return -1;

    *volume = 0;
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (g_listings[i].active && g_listings[i].state == AUCTION_STATE_ACTIVE) {
            *volume += g_listings[i].price;
        }
    }

    return 0;
}

/*============================================================================
 * Price History
 *============================================================================*/

int auction_get_price_history(uint32_t item_id, AuctionPriceHistory* history) {
    if (!history) return -1;

    for (int i = 0; i < g_price_history_count; i++) {
        if (g_price_history[i].item_id == item_id) {
            memcpy(history, &g_price_history[i], sizeof(AuctionPriceHistory));
            return 0;
        }
    }

    /* Not found, return default */
    memset(history, 0, sizeof(AuctionPriceHistory));
    history->item_id = item_id;
    return -1;
}

void auction_update_price_history(uint32_t item_id, uint64_t sold_price) {
    AuctionPriceHistory* ph = NULL;

    /* Find existing entry */
    for (int i = 0; i < g_price_history_count; i++) {
        if (g_price_history[i].item_id == item_id) {
            ph = &g_price_history[i];
            break;
        }
    }

    /* Create new entry if not found */
    if (!ph && g_price_history_count < MAX_PRICE_HISTORY) {
        ph = &g_price_history[g_price_history_count++];
        memset(ph, 0, sizeof(AuctionPriceHistory));
        ph->item_id = item_id;
        ph->min_price = sold_price;
        ph->max_price = sold_price;
    }

    if (!ph) return;

    /* Update statistics */
    ph->total_sold++;

    /* Running average */
    ph->avg_price = (ph->avg_price * (ph->total_sold - 1) + sold_price) / ph->total_sold;

    if (sold_price < ph->min_price || ph->min_price == 0) {
        ph->min_price = sold_price;
    }
    if (sold_price > ph->max_price) {
        ph->max_price = sold_price;
    }

    ph->last_update = (uint32_t)time(NULL);
}

/*============================================================================
 * Pending Gold
 *============================================================================*/

uint64_t auction_get_pending_gold(uint32_t seller_id) {
    for (int i = 0; i < g_pending_count; i++) {
        if (g_pending_gold[i].seller_id == seller_id) {
            return g_pending_gold[i].pending_gold;
        }
    }
    return 0;
}

uint64_t auction_collect_pending_gold(uint32_t seller_id) {
    for (int i = 0; i < g_pending_count; i++) {
        if (g_pending_gold[i].seller_id == seller_id) {
            uint64_t amount = g_pending_gold[i].pending_gold;
            g_pending_gold[i].pending_gold = 0;
            printf("[AUCTION_MGR] Seller %u collected %llu gold\n",
                   seller_id, (unsigned long long)amount);
            return amount;
        }
    }
    return 0;
}
