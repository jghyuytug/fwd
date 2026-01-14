#ifndef GUIDE_AUCTION_RANK_ORM_H
#define GUIDE_AUCTION_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    signed char rank;
    char it_name[101];
    signed char it_rarity;
    char it_url[129];
} GuideAuctionRank;

/* CRUD Operations */
int GuideAuctionRank_Add(DBConnectionManager* manager, const GuideAuctionRank* record);
int GuideAuctionRank_Get(DBConnectionManager* manager, signed char server_id, signed char rank, GuideAuctionRank* record);
int GuideAuctionRank_Update(DBConnectionManager* manager, const GuideAuctionRank* record);
int GuideAuctionRank_Delete(DBConnectionManager* manager, signed char server_id, signed char rank);
int GuideAuctionRank_Exists(DBConnectionManager* manager, signed char server_id, signed char rank);
int GuideAuctionRank_GetAll(DBConnectionManager* manager, GuideAuctionRank* records, int max_count, int* actual_count);

#endif /* GUIDE_AUCTION_RANK_ORM_H */