/* Storage Money System - DEPOSIT_MONEY(100) / WITHDRAW_MONEY(101)
 * Uses account_cargo.money field for storage gold
 */

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
    /* First try update */
    snprintf(query, sizeof(query),
             "UPDATE account_cargo SET money=%u WHERE m_id=%u",
             gold, account_id);
    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Set storage gold failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }
    if (mysql_affected_rows(g_db_charac2) == 0) {
        /* Insert if not exists */
        snprintf(query, sizeof(query),
                 "INSERT INTO account_cargo (m_id, money) VALUES (%u, %u)",
                 account_id, gold);
        if (mysql_query(g_db_charac2, query)) {
            printf("[DB] Insert storage gold failed: %s\n", mysql_error(g_db_charac2));
            return -1;
        }
    }
    printf("[DB] Storage gold for account %u set to %u\n", account_id, gold);
    return 0;
}

/* msg_no=100: DEPOSIT_MONEY
 * Request: amount(4)
 * Response: result(4) + char_gold(4) + storage_gold(4)
 */
static int handle_storage_deposit(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEPOSIT_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);
    printf("[GAME] DEPOSIT_MONEY: amount=%u char_gold=%u\n", amount, s->inventory.money);

    /* Check if character has enough gold */
    if (s->inventory.money < amount) {
        uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
        return send_response(s, 100, response, 4);
    }

    /* Get current storage gold */
    uint32_t storage_gold = 0;
    db_get_storage_gold(s->account_id, &storage_gold);

    /* Check overflow */
    if (storage_gold + amount < storage_gold) {
        uint8_t response[4] = {3, 0, 0, 0}; /* Error: storage full */
        return send_response(s, 100, response, 4);
    }

    /* Transfer gold */
    s->inventory.money -= amount;
    storage_gold += amount;

    /* Update database */
    db_update_gold(s->character_id, s->inventory.money);
    db_set_storage_gold(s->account_id, storage_gold);

    /* Build response */
    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] DEPOSIT_MONEY: success char=%u storage=%u\n",
           s->inventory.money, storage_gold);

    return send_response(s, 100, response, 12);
}

/* msg_no=101: WITHDRAW_MONEY
 * Request: amount(4)
 * Response: result(4) + char_gold(4) + storage_gold(4)
 */
static int handle_storage_withdraw(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] WITHDRAW_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);
    printf("[GAME] WITHDRAW_MONEY: amount=%u\n", amount);

    /* Get current storage gold */
    uint32_t storage_gold = 0;
    db_get_storage_gold(s->account_id, &storage_gold);

    /* Check if storage has enough gold */
    if (storage_gold < amount) {
        uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold in storage */
        return send_response(s, 101, response, 4);
    }

    /* Check character gold overflow */
    if (s->inventory.money + amount < s->inventory.money) {
        uint8_t response[4] = {3, 0, 0, 0}; /* Error: character inventory full */
        return send_response(s, 101, response, 4);
    }

    /* Transfer gold */
    storage_gold -= amount;
    s->inventory.money += amount;

    /* Update database */
    db_set_storage_gold(s->account_id, storage_gold);
    db_update_gold(s->character_id, s->inventory.money);

    /* Build response */
    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] WITHDRAW_MONEY: success char=%u storage=%u\n",
           s->inventory.money, storage_gold);

    return send_response(s, 101, response, 12);
}
