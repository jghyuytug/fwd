#ifndef AUCTION_SYSTEM_ITEMINFO_ORM_H
#define AUCTION_SYSTEM_ITEMINFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int sys_auction_id;
    short probability;
    int price;
    signed char seal_flag;
    int item_id;
    int add_info;
    signed char upgrade;
    signed char seal_cnt;
    short endurance;
    int extend_info;
} AuctionSystemIteminfo;

/* CRUD Operations */
int AuctionSystemIteminfo_Add(DBConnectionManager* manager, const AuctionSystemIteminfo* record);
int AuctionSystemIteminfo_GetAll(DBConnectionManager* manager, AuctionSystemIteminfo* records, int max_count, int* actual_count);

#endif /* AUCTION_SYSTEM_ITEMINFO_ORM_H */