#ifndef AUCTION_MANUAL_AVERAGE_PRICE_ORM_H
#define AUCTION_MANUAL_AVERAGE_PRICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int item_id;
    signed char upgrade;
    int average_price;
    signed char is_apply;
} AuctionManualAveragePrice;

/* CRUD Operations */
int AuctionManualAveragePrice_Add(DBConnectionManager* manager, const AuctionManualAveragePrice* record);
int AuctionManualAveragePrice_Get(DBConnectionManager* manager, int no, AuctionManualAveragePrice* record);
int AuctionManualAveragePrice_Update(DBConnectionManager* manager, const AuctionManualAveragePrice* record);
int AuctionManualAveragePrice_Delete(DBConnectionManager* manager, int no);
int AuctionManualAveragePrice_Exists(DBConnectionManager* manager, int no);
int AuctionManualAveragePrice_GetAll(DBConnectionManager* manager, AuctionManualAveragePrice* records, int max_count, int* actual_count);

#endif /* AUCTION_MANUAL_AVERAGE_PRICE_ORM_H */