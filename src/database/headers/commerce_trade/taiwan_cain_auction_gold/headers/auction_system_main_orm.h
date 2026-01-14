#ifndef AUCTION_SYSTEM_MAIN_ORM_H
#define AUCTION_SYSTEM_MAIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int sys_auction_id;
    char occ_time[20];
    int regist_interval;
    char regist_time[9];
    char start_date[11];
    char end_date[11];
    short expire_interval;
    char last_auction_time[20];
    char expected_regist_time[9];
} AuctionSystemMain;

/* CRUD Operations */
int AuctionSystemMain_Add(DBConnectionManager* manager, const AuctionSystemMain* record);
int AuctionSystemMain_Get(DBConnectionManager* manager, int sys_auction_id, AuctionSystemMain* record);
int AuctionSystemMain_Update(DBConnectionManager* manager, const AuctionSystemMain* record);
int AuctionSystemMain_Delete(DBConnectionManager* manager, int sys_auction_id);
int AuctionSystemMain_Exists(DBConnectionManager* manager, int sys_auction_id);
int AuctionSystemMain_GetAll(DBConnectionManager* manager, AuctionSystemMain* records, int max_count, int* actual_count);

#endif /* AUCTION_SYSTEM_MAIN_ORM_H */