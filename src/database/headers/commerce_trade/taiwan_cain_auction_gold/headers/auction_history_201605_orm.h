#ifndef AUCTION_HISTORY_201605_ORM_H
#define AUCTION_HISTORY_201605_ORM_H

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
} AuctionHistory201605;

/* CRUD Operations */
int AuctionHistory201605_Add(DBConnectionManager* manager, const AuctionHistory201605* record);
int AuctionHistory201605_Get(DBConnectionManager* manager, long long auction_id, AuctionHistory201605* record);
int AuctionHistory201605_Update(DBConnectionManager* manager, const AuctionHistory201605* record);
int AuctionHistory201605_Delete(DBConnectionManager* manager, long long auction_id);
int AuctionHistory201605_Exists(DBConnectionManager* manager, long long auction_id);
int AuctionHistory201605_GetAll(DBConnectionManager* manager, AuctionHistory201605* records, int max_count, int* actual_count);

#endif /* AUCTION_HISTORY_201605_ORM_H */