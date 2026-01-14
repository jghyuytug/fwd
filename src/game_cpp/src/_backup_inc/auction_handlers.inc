/* msg_no=170: AUCTION_SEARCH */
static int handle_auction_search(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_SEARCH from fd=%d\n", s->fd);

    uint32_t item_id = 0;
    uint32_t min_price = 0;
    uint32_t max_price = 0;
    if (len >= 4) memcpy(&item_id, data, 4);
    if (len >= 8) memcpy(&min_price, data + 4, 4);
    if (len >= 12) memcpy(&max_price, data + 8, 4);

    AuctionListing* results[20];
    int count = search_auction_listings(item_id, min_price, max_price, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        memcpy(response + offset, a->seller_name, 20);
        offset += 20;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Search returned %d results\n", count);
    return send_response(s, 170, response, offset);
}

/* msg_no=171: AUCTION_LIST_ITEM */
static int handle_auction_list_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_LIST_ITEM from fd=%d\n", s->fd);
    if (len < 12) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 171, error, 4);
    }

    uint32_t item_uid;
    uint32_t price;
    uint32_t duration; /* hours */
    memcpy(&item_uid, data, 4);
    memcpy(&price, data + 4, 4);
    memcpy(&duration, data + 8, 4);

    /* Validate duration (1-48 hours) */
    if (duration < 1) duration = 1;
    if (duration > 48) duration = 48;

    /* Listing fee (5% of price, min 1000) */
    uint32_t fee = price / 20;
    if (fee < 1000) fee = 1000;

    if (s->inventory.money < fee) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 171, error, 4);
    }

    /* Get item info */
    uint32_t item_id = 0;
    if (db_get_item_at_slot(s->character_id, item_uid, &item_id) < 0) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Item not found */
        return send_response(s, 171, error, 4);
    }

    /* Create listing */
    AuctionListing* listing = create_auction_listing(
        s->character_id, s->character_name, item_uid, item_id, price, duration);

    if (!listing) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot create listing */
        return send_response(s, 171, error, 4);
    }

    /* Deduct fee and persist gold */
    s->inventory.money -= fee;
    db_update_gold(s->character_id, s->inventory.money);

    /* Remove item from seller's inventory */
    db_delete_item(s->character_id, item_uid);

    printf("[AUCTION] Listed item %u (id=%u) for %u gold (fee: %u, listing_id=%u)\n",
           item_uid, item_id, price, fee, listing->listing_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing->listing_id, 4);
    memcpy(response + 8, &fee, 4);
    return send_response(s, 171, response, 12);
}

/* msg_no=172: AUCTION_CANCEL */
static int handle_auction_cancel(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_CANCEL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 172, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing || listing->seller_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found or not owner */
        return send_response(s, 172, error, 4);
    }

    /* Return item to seller */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Cancel listing */
    listing->active = 0;
    printf("[AUCTION] Cancelled listing %u, returned item %u\n", listing_id, listing->item_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    return send_response(s, 172, response, 8);
}

/* msg_no=173: AUCTION_BUY */
static int handle_auction_buy(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_BUY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 173, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found */
        return send_response(s, 173, error, 4);
    }

    if (listing->seller_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot buy own listing */
        return send_response(s, 173, error, 4);
    }

    if (s->inventory.money < listing->price) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 173, error, 4);
    }

    /* Deduct gold from buyer */
    s->inventory.money -= listing->price;
    db_update_gold(s->character_id, s->inventory.money);

    /* Add item to buyer */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Credit seller (seller collects later or via auction_collect_gold) */
    /* For simplicity, directly add to seller if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == listing->seller_id) {
            g_sessions[i].inventory.money += listing->price;
            db_update_gold(listing->seller_id, g_sessions[i].inventory.money);
            break;
        }
    }

    uint32_t price = listing->price;
    uint32_t item_id = listing->item_id;

    /* Mark listing as sold */
    listing->active = 0;
    printf("[AUCTION] Bought listing %u (item=%u) for %u gold\n", listing_id, item_id, price);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &price, 4);
    memcpy(response + 12, &item_id, 4);
    return send_response(s, 173, response, 16);
}

/* msg_no=174: AUCTION_GET_MY_LISTINGS */
static int handle_auction_get_my_listings(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_GET_MY_LISTINGS from fd=%d\n", s->fd);

    AuctionListing* results[20];
    int count = get_seller_listings(s->character_id, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Returned %d listings for seller %u\n", count, s->character_id);
    return send_response(s, 174, response, offset);
}
