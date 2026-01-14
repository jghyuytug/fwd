/* repair_functions.c - Equipment repair database functions
 * To be inserted into main_db.cpp after db_get_item_at_slot function (line 982)
 */

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

/* Update item durability */
static int db_update_durability(uint32_t charac_no, int slot, uint8_t dur) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE user_items SET stat=%u WHERE charac_no=%u AND slot=%d LIMIT 1", (unsigned)dur, charac_no, slot);
    return mysql_query(g_db_charac2, query) ? -1 : 0;
}

/* Calculate repair cost */
static uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur) {
    uint32_t lvl = (item_id / 1000) % 100;
    if (lvl == 0) lvl = 1;
    return (100 - cur_dur) * lvl * 10;
}

/* Updated handle_repair_equipment handler:
 * Request: slot(2) + repair_all(1)
 * Response: result(4) + repaired_count(4) + total_cost(4)
 */
static int handle_repair_equipment_impl(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] REPAIR_EQUIPMENT from fd=%d\n", s->fd);
    if (s->state < STATE_IN_GAME || len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 25, error, 4);
    }

    uint16_t slot;
    memcpy(&slot, data, 2);
    uint8_t repair_all = data[2];

    printf("[GAME] REPAIR_EQUIPMENT: slot=%u repair_all=%u\n", slot, repair_all);

    uint32_t total_cost = 0;
    int repaired_count = 0;

    if (repair_all) {
        /* Repair all equipped items (slots 0-12) */
        int i;
        for (i = 0; i <= 12; i++) {
            uint32_t item_id = 0;
            uint8_t durability = 100;
            if (db_get_item_info_at_slot(s->character_id, i, &item_id, &durability) == 0) {
                if (item_id > 0 && durability < 100) {
                    total_cost += calc_repair_cost(item_id, durability);
                }
            }
        }

        /* Check if player has enough gold */
        if (s->gold < total_cost) {
            uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
            return send_response(s, 25, response, 4);
        }

        /* Deduct gold and repair */
        if (total_cost > 0) {
            s->gold -= total_cost;
            db_update_gold(s->character_id, s->gold);
        }
        repaired_count = db_repair_all_items(s->character_id);
    } else {
        /* Repair single item */
        uint32_t item_id = 0;
        uint8_t durability = 100;
        if (db_get_item_info_at_slot(s->character_id, slot, &item_id, &durability) == 0) {
            if (item_id > 0 && durability < 100) {
                total_cost = calc_repair_cost(item_id, durability);

                if (s->gold < total_cost) {
                    uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
                    return send_response(s, 25, response, 4);
                }

                s->gold -= total_cost;
                db_update_gold(s->character_id, s->gold);
                if (db_repair_item(s->character_id, slot) == 0) {
                    repaired_count = 1;
                }
            }
        }
    }

    /* Build response */
    uint8_t response[12];
    memset(response, 0, 12);
    /* result = 0 (success) */
    memcpy(response + 4, &repaired_count, 4);
    memcpy(response + 8, &total_cost, 4);

    printf("[GAME] REPAIR_EQUIPMENT: repaired=%d cost=%u gold_left=%u\n",
           repaired_count, total_cost, s->gold);

    return send_response(s, 25, response, 12);
}
