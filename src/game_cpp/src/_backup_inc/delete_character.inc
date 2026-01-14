/* Soft delete a character (set delete_flag=1) */
static int db_soft_delete_character(uint32_t charac_no, uint32_t m_id) {
    if (!g_db_charac) return -1;

    /* Verify character belongs to this account */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE charac_no=%u AND m_id=%u AND delete_flag=0",
             charac_no, m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Delete character verify failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("[DB] Delete character failed: charac_no=%u not owned by m_id=%u or already deleted\n",
               charac_no, m_id);
        mysql_free_result(result);
        return -1;
    }
    mysql_free_result(result);

    /* Mark as deleted (soft delete) */
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET delete_flag=1, delete_time=NOW() WHERE charac_no=%u AND m_id=%u",
             charac_no, m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Delete character update failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    if (affected > 0) {
        printf("[DB] Soft deleted character charac_no=%u (m_id=%u)\n", charac_no, m_id);
        return 0;
    }

    return -1;
}

/* Optional: Clean up character items (move to deleted_items or just clear) */
static int db_cleanup_deleted_character_items(uint32_t charac_no) {
    if (!g_db_charac2) return -1;

    /* Delete items from user_items for the deleted character */
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Cleanup items failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Cleaned up %d items for deleted charac_no=%u\n", affected, charac_no);
    return 0;
}

