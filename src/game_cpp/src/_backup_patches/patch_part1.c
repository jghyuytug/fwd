
/* Get item info at slot including durability */
static int db_get_item_info_at_slot(uint32_t charac_no, int slot, uint32_t* item_id, uint8_t* durability) {
    if (!g_db_charac2 || !item_id) return -1;
    char query[256];
    snprintf(query, sizeof(query), "SELECT it_id, stat FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1", charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        *item_id = (uint32_t)atoi(row[0]);
        if (durability && row[1]) *durability = (uint8_t)atoi(row[1]);
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    return -1;
}

/* Repair single item */
static int db_repair_item(uint32_t charac_no, int slot) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot=%d LIMIT 1", charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    return (int)mysql_affected_rows(g_db_charac2) > 0 ? 0 : -1;
}

/* Repair all equipped items */
static int db_repair_all_items(uint32_t charac_no) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot BETWEEN 0 AND 12", charac_no);
    if (mysql_query(g_db_charac2, query)) return -1;
    return (int)mysql_affected_rows(g_db_charac2);
}

/* Calculate repair cost */
static uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur) {
    uint32_t lvl = (item_id / 1000) % 100;
    if (lvl == 0) lvl = 1;
    return (100 - cur_dur) * lvl * 10;
}
