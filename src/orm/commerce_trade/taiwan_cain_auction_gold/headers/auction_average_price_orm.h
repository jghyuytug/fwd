#ifndef AUCTION_AVERAGE_PRICE_ORM_H
#define AUCTION_AVERAGE_PRICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_id;
    signed char upgrade;
    int average_price;
    signed char seperate_upgrade;
} AuctionAveragePrice;

/* CRUD Operations */
int AuctionAveragePrice_Add(DBConnectionManager* manager, const AuctionAveragePrice* record);
int AuctionAveragePrice_Get(DBConnectionManager* manager, int item_id, signed char upgrade, AuctionAveragePrice* record);
int AuctionAveragePrice_Update(DBConnectionManager* manager, const AuctionAveragePrice* record);
int AuctionAveragePrice_Delete(DBConnectionManager* manager, int item_id, signed char upgrade);
int AuctionAveragePrice_Exists(DBConnectionManager* manager, int item_id, signed char upgrade);
int AuctionAveragePrice_GetAll(DBConnectionManager* manager, AuctionAveragePrice* records, int max_count, int* actual_count);

#endif /* AUCTION_AVERAGE_PRICE_ORM_H */