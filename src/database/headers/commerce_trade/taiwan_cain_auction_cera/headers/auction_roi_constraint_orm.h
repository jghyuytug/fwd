#ifndef AUCTION_ROI_CONSTRAINT_ORM_H
#define AUCTION_ROI_CONSTRAINT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int db_inf_max_price;
    int db_inf_min_price;
    int db_inf_prob;
    int db_inf_limit_count;
    int db_inf_base_mul_min_a;
    int db_inf_base_mul_max_b;
    char last_update_date[11];
} AuctionRoiConstraint;

/* CRUD Operations */
int AuctionRoiConstraint_Add(DBConnectionManager* manager, const AuctionRoiConstraint* record);
int AuctionRoiConstraint_GetAll(DBConnectionManager* manager, AuctionRoiConstraint* records, int max_count, int* actual_count);

#endif /* AUCTION_ROI_CONSTRAINT_ORM_H */