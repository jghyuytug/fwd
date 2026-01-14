#include "auction_roi_constraint_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AuctionRoiConstraint_Add(DBConnectionManager* manager, const AuctionRoiConstraint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auction_roi_constraint (db_inf_max_price, db_inf_min_price, db_inf_prob, db_inf_limit_count, db_inf_base_mul_min_a, db_inf_base_mul_max_b, last_update_date) "
        "VALUES (%d, %d, %d, %d, %d, %d, '%s')",
        record->db_inf_max_price, record->db_inf_min_price, record->db_inf_prob, record->db_inf_limit_count, record->db_inf_base_mul_min_a, record->db_inf_base_mul_max_b, record->last_update_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionRoiConstraint_GetAll(DBConnectionManager* manager, AuctionRoiConstraint* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT db_inf_max_price, db_inf_min_price, db_inf_prob, db_inf_limit_count, db_inf_base_mul_min_a, db_inf_base_mul_max_b, last_update_date FROM auction_roi_constraint");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_AUCTION_CERA, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AuctionRoiConstraint));
        records[count].db_inf_max_price = row[0] ? atoi(row[0]) : 0;
        records[count].db_inf_min_price = row[1] ? atoi(row[1]) : 0;
        records[count].db_inf_prob = row[2] ? atoi(row[2]) : 0;
        records[count].db_inf_limit_count = row[3] ? atoi(row[3]) : 0;
        records[count].db_inf_base_mul_min_a = row[4] ? atoi(row[4]) : 0;
        records[count].db_inf_base_mul_max_b = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].last_update_date, row[6], sizeof(records[count].last_update_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
