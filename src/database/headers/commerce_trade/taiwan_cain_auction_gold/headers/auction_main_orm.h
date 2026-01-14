#ifndef AUCTION_MAIN_ORM_H
#define AUCTION_MAIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long auction_id;
    char occ_time[20];
    int expire_time;
    int owner_id;
    char owner_name[21];
    signed char owner_type;
    char owner_nexon_id[26];
    int buyer_id;
    char buyer_name[21];
    long long price;
    long long instant_price;
    signed char seal_flag;
    int item_id;
    int add_info;
    signed char upgrade;
    signed char amplify_option;
    int amplify_value;
    signed char seal_cnt;
    short endurance;
    int extend_info;
    int black_point;
    int unit_price;
    unsigned char random_option[14];
    long long roi_high_key;
    int roi_low_key;
    signed char seperate_upgrade;
    unsigned char item_guid[10];
} AuctionMain;

/* CRUD Operations */
int AuctionMain_Add(DBConnectionManager* manager, const AuctionMain* record);
int AuctionMain_Get(DBConnectionManager* manager, long long auction_id, AuctionMain* record);
int AuctionMain_Update(DBConnectionManager* manager, const AuctionMain* record);
int AuctionMain_Delete(DBConnectionManager* manager, long long auction_id);
int AuctionMain_Exists(DBConnectionManager* manager, long long auction_id);
int AuctionMain_GetAll(DBConnectionManager* manager, AuctionMain* records, int max_count, int* actual_count);

#endif /* AUCTION_MAIN_ORM_H */