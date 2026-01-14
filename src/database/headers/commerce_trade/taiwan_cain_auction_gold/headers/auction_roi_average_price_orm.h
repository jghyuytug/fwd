#ifndef AUCTION_ROI_AVERAGE_PRICE_ORM_H
#define AUCTION_ROI_AVERAGE_PRICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int item_id;
    signed char upgrade;
    long long roi_high_key;
    int roi_low_key;
    short roi_index1;
    short roi_index2;
    short roi_index3;
    int average_price;
    int real_purchase_count;
    signed char seperate_upgrade;
} AuctionRoiAveragePrice;

/* CRUD Operations */
int AuctionRoiAveragePrice_Add(DBConnectionManager* manager, const AuctionRoiAveragePrice* record);
int AuctionRoiAveragePrice_Get(DBConnectionManager* manager, int no, AuctionRoiAveragePrice* record);
int AuctionRoiAveragePrice_Update(DBConnectionManager* manager, const AuctionRoiAveragePrice* record);
int AuctionRoiAveragePrice_Delete(DBConnectionManager* manager, int no);
int AuctionRoiAveragePrice_Exists(DBConnectionManager* manager, int no);
int AuctionRoiAveragePrice_GetAll(DBConnectionManager* manager, AuctionRoiAveragePrice* records, int max_count, int* actual_count);

#endif /* AUCTION_ROI_AVERAGE_PRICE_ORM_H */