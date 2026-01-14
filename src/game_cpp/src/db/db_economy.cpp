#include "db/db_economy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_server_context.h"
#include "db/db_game.h"

static int run_query(int db_type, const char* query, DBQueryResult* result) {
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, db_type, query, result) < 0) {
        return -1;
    }
    return 0;
}

static int run_non_query(int db_type, const char* query) {
    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(db_type, query, &result) < 0) {
        return -1;
    }
    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

static int run_insert(int db_type, const char* query, unsigned long long* insert_id) {
    if (DBConnectionManager_ExecuteInsert(&g_db_manager, db_type, query, insert_id) < 0) {
        return -1;
    }
    return 0;
}

static void sanitize_string(char* dest, size_t max_len, const char* src) {
    memset(dest, 0, max_len);
    if (!src) return;
    strncpy(dest, src, max_len - 1);
    for (size_t i = 0; i < max_len - 1 && dest[i]; i++) {
        if (dest[i] == '\'' || dest[i] == '\\') {
            dest[i] = '_';
        }
    }
}

static int db_create_storage(uint32_t m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO account_cargo (m_id, money, capacity, cargo, occ_time) "
             "VALUES (%u, 0, 8, '', NOW())",
             m_id);

    if (run_non_query(DB_TYPE_CAIN, query) < 0) {
        printf("[DB] Create storage failed for m_id=%u\n", m_id);
        return -1;
    }
    return 0;
}

int db_get_storage(uint32_t m_id, DBStorageInfo* storage) {
    if (!storage) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT money, capacity FROM account_cargo WHERE m_id=%u",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        printf("[DB] Storage query failed for m_id=%u\n", m_id);
        return -1;
    }

    char* row[2] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 2) {
        DBQueryResult_Free(&result);
        memset(storage, 0, sizeof(DBStorageInfo));
        return 0;
    }

    storage->money = row[0] ? atoi(row[0]) : 0;
    storage->capacity = row[1] ? atoi(row[1]) : 8;

    DBQueryResult_Free(&result);
    printf("[DB] Loaded storage for m_id=%u: money=%u, capacity=%u\n",
           m_id, storage->money, storage->capacity);
    return 0;
}

int db_update_storage_gold(uint32_t m_id, int32_t delta) {
    db_create_storage(m_id);

    char query[256];
    if (delta >= 0) {
        snprintf(query, sizeof(query),
                 "UPDATE account_cargo SET money=money+%d WHERE m_id=%u",
                 delta, m_id);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE account_cargo SET money=GREATEST(0, CAST(money AS SIGNED) + (%d)) WHERE m_id=%u",
                 delta, m_id);
    }

    if (run_non_query(DB_TYPE_CAIN, query) < 0) {
        printf("[DB] Update storage gold failed for m_id=%u\n", m_id);
        return -1;
    }

    printf("[DB] Updated storage gold for m_id=%u: delta=%d\n", m_id, delta);
    return 0;
}

uint32_t db_get_storage_gold(uint32_t m_id) {
    DBStorageInfo storage;
    if (db_get_storage(m_id, &storage) < 0) {
        return 0;
    }
    return storage.money;
}

int db_get_cash_balance(uint32_t account_id, uint32_t* cash, uint32_t* bind_cash) {
    if (!cash || !bind_cash) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT cash, bind_cash FROM cash_cera WHERE account_id=%u",
             account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    char* row[2] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 2) {
        DBQueryResult_Free(&result);
        *cash = 0;
        *bind_cash = 0;
        return 0;
    }

    *cash = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
    *bind_cash = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

int db_get_vip_info(uint32_t account_id, uint8_t* level, uint32_t* expire_time) {
    if (!level || !expire_time) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT vip_level, UNIX_TIMESTAMP(vip_expire) "
             "FROM member_vip WHERE account_id=%u",
             account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    char* row[2] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 2) {
        DBQueryResult_Free(&result);
        *level = 0;
        *expire_time = 0;
        return 0;
    }

    *level = row[0] ? (uint8_t)atoi(row[0]) : 0;
    *expire_time = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

int db_auction_create_listing(uint32_t seller_id, const char* seller_name,
                              uint32_t item_id, uint32_t price, uint32_t buyout,
                              uint32_t duration_hours) {
    const char* create_table =
        "CREATE TABLE IF NOT EXISTS auction_house ("
        "listing_id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, "
        "seller_id INT UNSIGNED, seller_name VARCHAR(21), "
        "item_id INT UNSIGNED, price INT UNSIGNED, buyout INT UNSIGNED, "
        "expire_time INT UNSIGNED, status TINYINT DEFAULT 0, "
        "create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    run_non_query(DB_TYPE_CAIN, create_table);

    uint32_t expire_time = (uint32_t)time(NULL) + (duration_hours * 3600);

    char query[512];
    char safe_name[64];
    sanitize_string(safe_name, sizeof(safe_name), seller_name);

    snprintf(query, sizeof(query),
             "INSERT INTO auction_house (seller_id, seller_name, item_id, price, buyout, expire_time) "
             "VALUES (%u, '%s', %u, %u, %u, %u)",
             seller_id, safe_name, item_id, price, buyout, expire_time);

    unsigned long long new_id = 0;
    if (run_insert(DB_TYPE_CAIN, query, &new_id) < 0) {
        printf("[AUCTION] Create listing failed for seller=%u\n", seller_id);
        return -1;
    }

    printf("[AUCTION] Created listing %u: seller=%s, item=%u, price=%u\n",
           (uint32_t)new_id, seller_name, item_id, price);
    return (int)new_id;
}

int db_auction_search(const char* keyword, uint32_t min_price, uint32_t max_price,
                      DBAuctionListing* results, int max_count) {
    if (!results) return 0;

    char query[512];
    char safe_keyword[128] = "";
    if (keyword && keyword[0]) {
        sanitize_string(safe_keyword, sizeof(safe_keyword), keyword);
    }

    uint32_t now = (uint32_t)time(NULL);
    uint32_t upper_price = max_price > 0 ? max_price : 999999999;

    if (strlen(safe_keyword) > 0) {
        snprintf(query, sizeof(query),
                 "SELECT listing_id, seller_id, seller_name, item_id, price, buyout, expire_time "
                 "FROM auction_house WHERE status=0 AND expire_time>%u "
                 "AND (seller_name LIKE '%%%s%%' OR item_id LIKE '%%%s%%') "
                 "AND price BETWEEN %u AND %u ORDER BY price LIMIT %d",
                 now, safe_keyword, safe_keyword,
                 min_price, upper_price, max_count);
    } else {
        snprintf(query, sizeof(query),
                 "SELECT listing_id, seller_id, seller_name, item_id, price, buyout, expire_time "
                 "FROM auction_house WHERE status=0 AND expire_time>%u "
                 "AND price BETWEEN %u AND %u ORDER BY price LIMIT %d",
                 now, min_price, upper_price, max_count);
    }

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    int count = 0;
    char* row[7] = {0};
    while (count < max_count && DBQueryResult_FetchRow(&result, row) == 7) {
        DBAuctionListing* l = &results[count];
        memset(l, 0, sizeof(DBAuctionListing));
        l->listing_id = row[0] ? atoi(row[0]) : 0;
        l->seller_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(l->seller_name, row[2], sizeof(l->seller_name) - 1);
        l->item_id = row[3] ? atoi(row[3]) : 0;
        l->price = row[4] ? atoi(row[4]) : 0;
        l->buyout = row[5] ? atoi(row[5]) : 0;
        l->expire_time = row[6] ? atoi(row[6]) : 0;
        l->status = 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int db_auction_buy(uint32_t listing_id, uint32_t buyer_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT seller_id, item_id, buyout FROM auction_house "
             "WHERE listing_id=%u AND status=0 AND expire_time>%u",
             listing_id, (uint32_t)time(NULL));

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* row[3] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 3) {
        DBQueryResult_Free(&result);
        return -2;
    }

    uint32_t seller_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    uint32_t item_id = row[1] ? (uint32_t)atoi(row[1]) : 0;
    uint32_t buyout_price = row[2] ? (uint32_t)atoi(row[2]) : 0;
    DBQueryResult_Free(&result);

    snprintf(query, sizeof(query),
             "UPDATE auction_house SET status=1 WHERE listing_id=%u AND status=0",
             listing_id);

    int affected = run_non_query(DB_TYPE_CAIN, query);
    if (affected <= 0) {
        return -3;
    }

    db_add_gold(seller_id, buyout_price);

    int slot = db_find_free_slot(buyer_id, MAX_EQUIP_SLOTS);
    if (slot >= 0 && slot < 200) {
        db_add_item(buyer_id, item_id, slot, 0);
    }

    printf("[AUCTION] Listing %u sold: buyer=%u paid %u to seller=%u for item=%u\n",
           listing_id, buyer_id, buyout_price, seller_id, item_id);
    return (int)buyout_price;
}

int db_auction_cancel(uint32_t listing_id, uint32_t seller_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE auction_house SET status=3 WHERE listing_id=%u AND seller_id=%u AND status=0",
             listing_id, seller_id);

    int affected = run_non_query(DB_TYPE_CAIN, query);
    if (affected <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
             "SELECT item_id FROM auction_house WHERE listing_id=%u", listing_id);
    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) == 0) {
        char* row[1] = {0};
        if (DBQueryResult_FetchRow(&result, row) == 1 && row[0]) {
            uint32_t item_id = (uint32_t)atoi(row[0]);
            int slot = db_find_free_slot(seller_id, MAX_EQUIP_SLOTS);
            if (slot >= 0 && slot < 200) {
                db_add_item(seller_id, item_id, slot, 0);
            }
        }
        DBQueryResult_Free(&result);
    }

    printf("[AUCTION] Listing %u cancelled by seller %u\n", listing_id, seller_id);
    return 0;
}
