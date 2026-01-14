#ifndef AUCTION_ITEM_TOP100_ORM_H
#define AUCTION_ITEM_TOP100_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char category;
    int item_id;
    int item_value;
} AuctionItemTop100;

/* CRUD Operations */
int AuctionItemTop100_Add(DBConnectionManager* manager, const AuctionItemTop100* record);
int AuctionItemTop100_Get(DBConnectionManager* manager, const char* cur_date, signed char category, int item_id, signed char server_info, AuctionItemTop100* record);
int AuctionItemTop100_Update(DBConnectionManager* manager, const AuctionItemTop100* record);
int AuctionItemTop100_Delete(DBConnectionManager* manager, const char* cur_date, signed char category, int item_id, signed char server_info);
int AuctionItemTop100_Exists(DBConnectionManager* manager, const char* cur_date, signed char category, int item_id, signed char server_info);
int AuctionItemTop100_GetAll(DBConnectionManager* manager, AuctionItemTop100* records, int max_count, int* actual_count);

#endif /* AUCTION_ITEM_TOP100_ORM_H */