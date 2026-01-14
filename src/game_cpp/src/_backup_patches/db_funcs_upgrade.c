}

/* Get item upgrade level from hidden_option field */
static int db_get_item_upgrade_level(uint32_t charac_no, int slot, uint8_t* level) {
    if (!g_db_charac2 || !level) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT hidden_option FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        uint16_t hidden = (uint16_t)atoi(row[0]);
        *level = (uint8_t)(hidden & 0xFF);
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    return -1;
}

/* Update item upgrade level */
static int db_set_item_upgrade_level(uint32_t charac_no, int slot, uint8_t new_level) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET hidden_option=(hidden_option & 0xFF00) | %u WHERE charac_no=%u AND slot=%d LIMIT 1",
             (unsigned)new_level, charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    return 0;
}

/* Get storage gold for account */
static int db_get_storage_gold(uint32_t account_id, uint32_t* gold) {
    if (!g_db_charac2 || !gold) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT money FROM account_cargo WHERE m_id=%u LIMIT 1",
             account_id);
    if (mysql_query(g_db_charac2, query)) return -1;
    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        *gold = (uint32_t)atoi(row[0]);
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    *gold = 0;
    return 0;
}

/* Update storage gold */
static int db_set_storage_gold(uint32_t account_id, uint32_t gold) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE account_cargo SET money=%u WHERE m_id=%u",
             gold, account_id);
    if (mysql_query(g_db_charac2, query)) return -1;
    if (mysql_affected_rows(g_db_charac2) == 0) {
        snprintf(query, sizeof(query),
                 "INSERT INTO account_cargo (m_id, money) VALUES (%u, %u)",
                 account_id, gold);
        mysql_query(g_db_charac2, query);
    }
    return 0;
}

/* Calculate upgrade success rate */
static int calc_upgrade_success_rate(uint8_t level) {
    if (level <= 3) return 100;
    if (level == 4) return 95;
    if (level == 5) return 90;
    if (level == 6) return 80;
    if (level == 7) return 70;
    if (level == 8) return 60;
    if (level == 9) return 50;
    if (level == 10) return 40;
    if (level == 11) return 30;
    if (level == 12) return 20;
    return 10;
}

/* Calculate upgrade cost */
static uint32_t calc_upgrade_cost(uint32_t item_id, uint8_t level) {
    uint32_t item_level = (item_id / 1000) % 100;
    if (item_level == 0) item_level = 1;
    uint32_t base_cost = item_level * 1000;
    return base_cost + (base_cost * level / 2);
}

/* Update character exp and level in charac_info */