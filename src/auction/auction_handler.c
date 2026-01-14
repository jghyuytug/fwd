/*
 * auction_handler.c - Auction Protocol Handlers
 * Phase 16 P4: Independent Auction Service
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auction_handler.h"
#include "auction_manager.h"
#include "auction_types.h"

/*============================================================================
 * msg_no=170: AUCTION_SEARCH
 *============================================================================*/
int auction_handle_search(AuctionClientContext* ctx, const uint8_t* data, int len,
                          uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] Search from player %u\n", ctx->player_id);

    /* Parse request */
    AuctionSearchRequest req;
    memset(&req, 0, sizeof(req));

    if (len >= 4) memcpy(&req.item_id, data, 4);
    if (len >= 8) memcpy(&req.min_price, data + 4, 4);
    if (len >= 12) memcpy(&req.max_price, data + 8, 4);

    /* Search */
    AuctionListing* results[MAX_SEARCH_RESULTS];
    int count = auction_search(&req, results, MAX_SEARCH_RESULTS);

    /* Build response */
    memset(response, 0, 512);
    int offset = 0;

    uint32_t result = 0; /* Success */
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        uint32_t lid = (uint32_t)a->listing_id;
        memcpy(response + offset, &lid, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        uint32_t price32 = (uint32_t)a->price;
        memcpy(response + offset, &price32, 4);
        offset += 4;
        memcpy(response + offset, a->seller_name, 20);
        offset += 20;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    *resp_len = offset;
    printf("[AUCTION_HANDLER] Search returned %d results\n", count);
    return 0;
}

/*============================================================================
 * msg_no=171: AUCTION_LIST_ITEM
 *============================================================================*/
int auction_handle_list_item(AuctionClientContext* ctx, const uint8_t* data, int len,
                             uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] ListItem from player %u\n", ctx->player_id);

    if (len < 12) {
        uint32_t err = 1;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t item_uid, price, duration;
    memcpy(&item_uid, data, 4);
    memcpy(&price, data + 4, 4);
    memcpy(&duration, data + 8, 4);

    /* Calculate listing fee */
    uint32_t fee = price * LISTING_FEE_PERCENT / 100;
    if (fee < LISTING_FEE_MIN) fee = LISTING_FEE_MIN;

    /* Check gold (Game service should verify, but double-check) */
    if (ctx->gold < fee) {
        uint32_t err = 2; /* Not enough gold */
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    /* Create listing */
    /* Note: item_id should be passed from Game, here we use item_uid as placeholder */
    uint32_t item_id = item_uid; /* TODO: Get actual item_id from IPC data */

    AuctionListing* listing = auction_create_listing(
        ctx->player_id, ctx->player_name,
        item_uid, item_id, price, duration
    );

    if (!listing) {
        uint32_t err = 4;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    /* Build response */
    memset(response, 0, 12);
    uint32_t result = 0;
    memcpy(response, &result, 4);
    uint32_t lid = (uint32_t)listing->listing_id;
    memcpy(response + 4, &lid, 4);
    memcpy(response + 8, &fee, 4);
    *resp_len = 12;

    printf("[AUCTION_HANDLER] Listed item uid=%u for %u gold (fee=%u, listing_id=%u)\n",
           item_uid, price, fee, lid);

    return 0;
}

/*============================================================================
 * msg_no=172: AUCTION_CANCEL
 *============================================================================*/
int auction_handle_cancel(AuctionClientContext* ctx, const uint8_t* data, int len,
                          uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] Cancel from player %u\n", ctx->player_id);

    if (len < 4) {
        uint32_t err = 1;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    int ret = auction_cancel_listing(listing_id, ctx->player_id);
    if (ret < 0) {
        uint32_t err = 2;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    /* Build response */
    memset(response, 0, 8);
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &listing_id, 4);
    *resp_len = 8;

    printf("[AUCTION_HANDLER] Cancelled listing %u\n", listing_id);
    return 0;
}

/*============================================================================
 * msg_no=173: AUCTION_BUY
 *============================================================================*/
int auction_handle_buy(AuctionClientContext* ctx, const uint8_t* data, int len,
                       uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] Buy from player %u\n", ctx->player_id);

    if (len < 4) {
        uint32_t err = 1;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = auction_find_listing(listing_id);
    if (!listing) {
        uint32_t err = 2;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    if (listing->seller_id == ctx->player_id) {
        uint32_t err = 3;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    if (ctx->gold < listing->price) {
        uint32_t err = 4;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t price = (uint32_t)listing->price;
    uint32_t item_id = listing->item_id;

    int ret = auction_buy_listing(listing_id, ctx->player_id);
    if (ret < 0) {
        uint32_t err = 5;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    /* Build response */
    memset(response, 0, 16);
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &price, 4);
    memcpy(response + 12, &item_id, 4);
    *resp_len = 16;

    printf("[AUCTION_HANDLER] Bought listing %u (item=%u) for %u gold\n",
           listing_id, item_id, price);

    return 0;
}

/*============================================================================
 * msg_no=174: AUCTION_GET_MY_LISTINGS
 *============================================================================*/
int auction_handle_get_my_listings(AuctionClientContext* ctx, const uint8_t* data, int len,
                                   uint8_t* response, int* resp_len)
{
    (void)data; (void)len;
    printf("[AUCTION_HANDLER] GetMyListings from player %u\n", ctx->player_id);

    AuctionListing* results[MAX_MY_LISTINGS];
    int count = auction_get_seller_listings(ctx->player_id, results, MAX_MY_LISTINGS);

    /* Build response */
    memset(response, 0, 512);
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        uint32_t lid = (uint32_t)a->listing_id;
        memcpy(response + offset, &lid, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        uint32_t price32 = (uint32_t)a->price;
        memcpy(response + offset, &price32, 4);
        offset += 4;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    *resp_len = offset;
    printf("[AUCTION_HANDLER] Returned %d listings for seller %u\n", count, ctx->player_id);
    return 0;
}

/*============================================================================
 * msg_no=175: AUCTION_GET_HISTORY
 *============================================================================*/
int auction_handle_get_history(AuctionClientContext* ctx, const uint8_t* data, int len,
                               uint8_t* response, int* resp_len)
{
    (void)data; (void)len;
    printf("[AUCTION_HANDLER] GetHistory from player %u\n", ctx->player_id);

    /* TODO: Implement actual history from DB */
    memset(response, 0, 128);
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 0; /* No history for now */
    response[offset++] = count;

    *resp_len = offset;
    return 0;
}

/*============================================================================
 * msg_no=176: AUCTION_BID
 *============================================================================*/
int auction_handle_bid(AuctionClientContext* ctx, const uint8_t* data, int len,
                       uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] Bid from player %u\n", ctx->player_id);

    if (len < 8) {
        uint32_t err = 1;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t listing_id, bid_amount;
    memcpy(&listing_id, data, 4);
    memcpy(&bid_amount, data + 4, 4);

    if (ctx->gold < bid_amount) {
        uint32_t err = 2;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    int ret = auction_place_bid(listing_id, ctx->player_id, bid_amount);
    if (ret < 0) {
        uint32_t err = 3;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    /* Build response */
    memset(response, 0, 12);
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &bid_amount, 4);
    *resp_len = 12;

    printf("[AUCTION_HANDLER] Bid %u on listing %u\n", bid_amount, listing_id);
    return 0;
}

/*============================================================================
 * msg_no=177: AUCTION_GET_PRICE_HISTORY
 *============================================================================*/
int auction_handle_get_price_history(AuctionClientContext* ctx, const uint8_t* data, int len,
                                     uint8_t* response, int* resp_len)
{
    printf("[AUCTION_HANDLER] GetPriceHistory from player %u\n", ctx->player_id);

    if (len < 4) {
        uint32_t err = 1;
        memcpy(response, &err, 4);
        *resp_len = 4;
        return -1;
    }

    uint32_t item_id;
    memcpy(&item_id, data, 4);

    AuctionPriceHistory history;
    auction_get_price_history(item_id, &history);

    /* Build response */
    memset(response, 0, 64);
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;

    uint32_t avg = (uint32_t)history.avg_price;
    uint32_t min = (uint32_t)history.min_price;
    uint32_t max = (uint32_t)history.max_price;
    memcpy(response + offset, &avg, 4);
    offset += 4;
    memcpy(response + offset, &min, 4);
    offset += 4;
    memcpy(response + offset, &max, 4);
    offset += 4;

    *resp_len = offset;
    return 0;
}

/*============================================================================
 * msg_no=178: AUCTION_COLLECT_GOLD
 *============================================================================*/
int auction_handle_collect_gold(AuctionClientContext* ctx, const uint8_t* data, int len,
                                uint8_t* response, int* resp_len)
{
    (void)data; (void)len;
    printf("[AUCTION_HANDLER] CollectGold from player %u\n", ctx->player_id);

    uint64_t collected = auction_collect_pending_gold(ctx->player_id);

    /* Build response */
    memset(response, 0, 8);
    uint32_t result = 0;
    memcpy(response, &result, 4);
    uint32_t collected32 = (uint32_t)collected;
    memcpy(response + 4, &collected32, 4);
    *resp_len = 8;

    printf("[AUCTION_HANDLER] Collected %u gold for player %u\n",
           collected32, ctx->player_id);
    return 0;
}

/*============================================================================
 * Dispatcher
 *============================================================================*/
int auction_dispatch_message(uint16_t msg_type, AuctionClientContext* ctx,
                             const uint8_t* data, int len,
                             uint8_t* response, int* resp_len)
{
    switch (msg_type) {
        case 170: return auction_handle_search(ctx, data, len, response, resp_len);
        case 171: return auction_handle_list_item(ctx, data, len, response, resp_len);
        case 172: return auction_handle_cancel(ctx, data, len, response, resp_len);
        case 173: return auction_handle_buy(ctx, data, len, response, resp_len);
        case 174: return auction_handle_get_my_listings(ctx, data, len, response, resp_len);
        case 175: return auction_handle_get_history(ctx, data, len, response, resp_len);
        case 176: return auction_handle_bid(ctx, data, len, response, resp_len);
        case 177: return auction_handle_get_price_history(ctx, data, len, response, resp_len);
        case 178: return auction_handle_collect_gold(ctx, data, len, response, resp_len);
        default:
            printf("[AUCTION_HANDLER] Unknown message type: %d\n", msg_type);
            return -1;
    }
}
