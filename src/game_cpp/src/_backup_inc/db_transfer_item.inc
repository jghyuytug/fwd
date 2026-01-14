/* Transfer item from one character to another by ui_id */
static int db_transfer_item(uint32_t ui_id, uint32_t from_charac, uint32_t to_charac) {
    if (!g_db_charac2) return -1;

    /* First verify the item belongs to from_charac */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT it_id, slot FROM user_items WHERE ui_id=%u AND charac_no=%u",
             ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("[DB] Transfer failed: item ui_id=%u not owned by charac_no=%u\n",
               ui_id, from_charac);
        mysql_free_result(result);
        return -1;
    }

    uint32_t it_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    mysql_free_result(result);

    /* Find a free slot for the receiving character (bag slots start at 11) */
    int new_slot = db_find_free_slot(to_charac, 11);
    if (new_slot < 0 || new_slot > 200) {
        printf("[DB] Transfer failed: no free slot for charac_no=%u\n", to_charac);
        return -1;
    }

    /* Transfer the item: update charac_no and slot */
    snprintf(query, sizeof(query),
             "UPDATE user_items SET charac_no=%u, slot=%d, m_time=NOW() "
             "WHERE ui_id=%u AND charac_no=%u",
             to_charac, new_slot, ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item update failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    if (affected > 0) {
        printf("[DB] Transferred item ui_id=%u (it_id=%u) from charac=%u to charac=%u slot=%d\n",
               ui_id, it_id, from_charac, to_charac, new_slot);
        return new_slot;  /* Return new slot on success */
    }

    return -1;
}

