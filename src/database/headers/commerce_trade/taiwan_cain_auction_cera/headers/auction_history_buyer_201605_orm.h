#ifndef AUCTION_HISTORY_BUYER_201605_ORM_H
#define AUCTION_HISTORY_BUYER_201605_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long auction_id;
    char occ_time[20];
    int pre_buyer_id;
    int buyer_id;
    int pre_price;
    int price;
    int pre_buyer_postal_id;
    int commission;
} AuctionHistoryBuyer201605;

/* CRUD Operations */
int AuctionHistoryBuyer201605_Add(DBConnectionManager* manager, const AuctionHistoryBuyer201605* record);
int AuctionHistoryBuyer201605_GetAll(DBConnectionManager* manager, AuctionHistoryBuyer201605* records, int max_count, int* actual_count);

#endif /* AUCTION_HISTORY_BUYER_201605_ORM_H */