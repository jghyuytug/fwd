#ifndef DB_ECONOMY_H
#define DB_ECONOMY_H

#include <stdint.h>

typedef struct {
    uint32_t money;
    uint8_t  capacity;
} DBStorageInfo;

typedef struct {
    uint32_t listing_id;
    uint32_t seller_id;
    char     seller_name[21];
    uint32_t item_id;
    uint32_t price;
    uint32_t buyout;
    uint32_t expire_time;
    uint8_t  status;
} DBAuctionListing;

#ifdef __cplusplus
extern "C" {
#endif

int db_get_storage(uint32_t m_id, DBStorageInfo* storage);
int db_update_storage_gold(uint32_t m_id, int32_t delta);
uint32_t db_get_storage_gold(uint32_t m_id);
int db_get_cash_balance(uint32_t account_id, uint32_t* cash, uint32_t* bind_cash);
int db_get_vip_info(uint32_t account_id, uint8_t* level, uint32_t* expire_time);
int db_auction_create_listing(uint32_t seller_id, const char* seller_name,
                              uint32_t item_id, uint32_t price, uint32_t buyout,
                              uint32_t duration_hours);
int db_auction_search(const char* keyword, uint32_t min_price, uint32_t max_price,
                      DBAuctionListing* results, int max_count);
int db_auction_buy(uint32_t listing_id, uint32_t buyer_id);
int db_auction_cancel(uint32_t listing_id, uint32_t seller_id);

#ifdef __cplusplus
}
#endif

#endif /* DB_ECONOMY_H */
