#ifndef AUCTION_HISTORY_BUYER_201603_ORM_H
#define AUCTION_HISTORY_BUYER_201603_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long auction_id;
    char occ_time[20];
    int pre_buyer_id;
    int buyer_id;
    int pre_price;
    int price;
    int pre_buyer_postal_id;
} AuctionHistoryBuyer201603;

/* CRUD Operations */
int AuctionHistoryBuyer201603_Add(DBConnectionManager* manager, const AuctionHistoryBuyer201603* record);
int AuctionHistoryBuyer201603_GetAll(DBConnectionManager* manager, AuctionHistoryBuyer201603* records, int max_count, int* actual_count);

#endif /* AUCTION_HISTORY_BUYER_201603_ORM_H */