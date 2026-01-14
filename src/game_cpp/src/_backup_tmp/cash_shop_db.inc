/*==============================================================================
 * Cash Shop DB Functions (点券商城)
 *============================================================================*/

typedef struct {
    uint32_t item_id;
    uint32_t price;
    uint8_t discount;
    uint16_t stock;
    uint8_t category;
    uint8_t vip_required;
} CashShopItemDef;

static CashShopItemDef g_cash_shop_items[] = {
    {90001, 1000, 0, 0, 1, 0},
    {90002, 500, 10, 100, 2, 0},
    {90003, 200, 0, 0, 2, 0},
    {90004, 2000, 20, 50, 3, 1},
    {90005, 5000, 0, 0, 4, 0},
    {90006, 300, 50, 0, 2, 0},
    {90007, 800, 0, 0, 1, 0},
    {90008, 1500, 15, 0, 3, 2},
    {0, 0, 0, 0, 0, 0}
};

static int db_get_cash_balance(uint32_t account_id, uint32_t* cash, uint32_t* bind_cash) {
    if (!g_db_account || !cash || !bind_cash) return -1;
    *cash = 0; *bind_cash = 0;
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT cash, bind_cash FROM cash_balance WHERE account_id=%u", account_id);
    if (mysql_query(g_db_account, query)) return 0;
    MYSQL_RES* result = mysql_store_result(g_db_account);
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            *cash = row[0] ? atoi(row[0]) : 0;
            *bind_cash = row[1] ? atoi(row[1]) : 0;
        }
        mysql_free_result(result);
    }
    return 0;
}

static int db_deduct_cash(uint32_t account_id, uint32_t amount) {
    if (!g_db_account || amount == 0) return -1;
    uint32_t cash = 0, bind_cash = 0;
    db_get_cash_balance(account_id, &cash, &bind_cash);
    if (cash + bind_cash < amount) return -2;
    uint32_t deduct_bind = (bind_cash >= amount) ? amount : bind_cash;
    uint32_t deduct_cash_amt = amount - deduct_bind;
    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE cash_balance SET cash=cash-%u, bind_cash=bind_cash-%u "
             "WHERE account_id=%u AND cash>=%u AND bind_cash>=%u",
             deduct_cash_amt, deduct_bind, account_id, deduct_cash_amt, deduct_bind);
    if (mysql_query(g_db_account, query) || mysql_affected_rows(g_db_account) == 0) return -3;
    printf("[CASH] Deducted %u from account %u\n", amount, account_id);
    return 0;
}

static int db_add_cash_log(uint32_t account_id, uint8_t type, uint32_t item_id,
                           uint32_t price, uint32_t charac_no) {
    if (!g_db_account) return -1;
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO cash_shop_log (account_id,type,item_id,price,charac_no,create_time) "
             "VALUES (%u,%d,%u,%u,%u,NOW())", account_id, type, item_id, price, charac_no);
    mysql_query(g_db_account, query);
    return 0;
}

static int db_get_vip_info(uint32_t account_id, uint8_t* level, uint32_t* expire_time) {
    if (!g_db_account || !level || !expire_time) return -1;
    *level = 0; *expire_time = 0;
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT vip_level,UNIX_TIMESTAMP(vip_expire) FROM cash_balance "
             "WHERE account_id=%u AND vip_expire>NOW()", account_id);
    if (mysql_query(g_db_account, query)) return -1;
    MYSQL_RES* result = mysql_store_result(g_db_account);
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            *level = row[0] ? atoi(row[0]) : 0;
            *expire_time = row[1] ? atoi(row[1]) : 0;
        }
        mysql_free_result(result);
    }
    return 0;
}

static CashShopItemDef* find_cash_shop_item(uint32_t item_id) {
    for (int i = 0; g_cash_shop_items[i].item_id != 0; i++) {
        if (g_cash_shop_items[i].item_id == item_id) return &g_cash_shop_items[i];
    }
    return NULL;
}

static int cash_shop_buy_item(ClientSession* s, uint32_t item_id, uint8_t count) {
    if (!s || count == 0) return -1;
    CashShopItemDef* item = find_cash_shop_item(item_id);
    if (!item) return -2;
    uint8_t vip_level = 0; uint32_t vip_expire = 0;
    db_get_vip_info(s->account_id, &vip_level, &vip_expire);
    if (item->vip_required > vip_level) return -3;
    uint32_t total_price = (item->price * (100 - item->discount) / 100) * count;
    if (db_deduct_cash(s->account_id, total_price) < 0) return -4;
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no,it_id,count,slot) "
             "SELECT %u,%u,%d,COALESCE(MAX(slot)+1,0) FROM user_items WHERE charac_no=%u",
             s->character_id, item_id, count, s->character_id);
    if (mysql_query(g_db_charac, query)) return -5;
    db_add_cash_log(s->account_id, 1, item_id, total_price, s->character_id);
    printf("[CASH] Account %u bought item %u x%d for %u\n", s->account_id, item_id, count, total_price);
    return 0;
}
