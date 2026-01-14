#ifndef AUCTION_HISTORY_201604_ORM_H
#define AUCTION_HISTORY_201604_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long auction_id;
    char start_time[20];
    char occ_time[20];
    signed char event_type;
    int owner_id;
    int buyer_id;
    int price;
    signed char seal_flag;
    int item_id;
    int add_info;
    signed char upgrade;
    signed char amplify_option;
    int amplify_value;
    signed char seal_cnt;
    short endurance;
    int extend_info;
    int owner_postal_id;
    int buyer_postal_id;
    int unit_price;
} AuctionHistory201604;

/* CRUD Operations */
int AuctionHistory201604_Add(DBConnectionManager* manager, const AuctionHistory201604* record);
int AuctionHistory201604_Get(DBConnectionManager* manager, long long auction_id, AuctionHistory201604* record);
int AuctionHistory201604_Update(DBConnectionManager* manager, const AuctionHistory201604* record);
int AuctionHistory201604_Delete(DBConnectionManager* manager, long long auction_id);
int AuctionHistory201604_Exists(DBConnectionManager* manager, long long auction_id);
int AuctionHistory201604_GetAll(DBConnectionManager* manager, AuctionHistory201604* records, int max_count, int* actual_count);

#endif /* AUCTION_HISTORY_201604_ORM_H */