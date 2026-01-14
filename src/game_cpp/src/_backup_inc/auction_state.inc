
/* Auction listing info */
#define MAX_AUCTION_LISTINGS 256
typedef struct {
    uint32_t listing_id;          /* Unique listing ID */
    uint32_t seller_id;           /* Seller character ID */
    char     seller_name[20];     /* Seller name */
    uint32_t item_ui_id;          /* Item unique ID from user_items */
    uint32_t item_id;             /* Item template ID */
    uint32_t price;               /* Asking price */
    uint32_t expire_time;         /* Unix timestamp when expires */
    uint8_t  active;
} AuctionListing;

static AuctionListing g_auction_listings[MAX_AUCTION_LISTINGS];
static uint32_t g_next_auction_id = 1;

/* Create auction listing */
static AuctionListing* create_auction_listing(uint32_t seller_id, const char* seller_name,
                                                uint32_t item_ui_id, uint32_t item_id,
                                                uint32_t price, uint32_t duration_hours) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (!g_auction_listings[i].active) {
            AuctionListing* a = &g_auction_listings[i];
            memset(a, 0, sizeof(AuctionListing));
            a->listing_id = g_next_auction_id++;
            a->seller_id = seller_id;
            strncpy(a->seller_name, seller_name, 19);
            a->item_ui_id = item_ui_id;
            a->item_id = item_id;
            a->price = price;
            a->expire_time = (uint32_t)time(NULL) + (duration_hours * 3600);
            a->active = 1;
            printf("[AUCTION] Created listing %u: seller=%u item=%u price=%u\n",
                   a->listing_id, seller_id, item_id, price);
            return a;
        }
    }
    return NULL;
}

/* Find auction listing by ID */
static AuctionListing* find_auction_listing(uint32_t listing_id) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (g_auction_listings[i].active && g_auction_listings[i].listing_id == listing_id) {
            return &g_auction_listings[i];
        }
    }
    return NULL;
}

/* Cancel auction listing */
static int cancel_auction_listing(uint32_t listing_id, uint32_t seller_id) {
    AuctionListing* a = find_auction_listing(listing_id);
    if (a && a->seller_id == seller_id) {
        printf("[AUCTION] Cancelled listing %u\n", listing_id);
        a->active = 0;
        return 0;
    }
    return -1;
}

/* Search auction listings by item_id */
static int search_auction_listings(uint32_t item_id, uint32_t min_price, uint32_t max_price,
                                    AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active || a->expire_time < now) continue;

        /* Match by item_id (0 = all items) */
        if (item_id > 0 && a->item_id != item_id) continue;

        /* Match by price range */
        if (min_price > 0 && a->price < min_price) continue;
        if (max_price > 0 && a->price > max_price) continue;

        results[count++] = a;
    }

    return count;
}

/* Get seller's listings */
static int get_seller_listings(uint32_t seller_id, AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active) continue;
        if (a->seller_id == seller_id) {
            /* Mark expired listings */
            if (a->expire_time < now) a->active = 0;
            else results[count++] = a;
        }
    }

    return count;
}

