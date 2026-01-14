/* msg_no=83: UPGRADE_ITEM - Item Enhancement/Upgrade System
 * Request: slot(2) + material_slot(2) + upgrade_type(1)
 * upgrade_type: 0=强化, 1=精炼, 2=增幅
 * Response: result(4) + new_level(4) + item_destroyed(1)
 */

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
        /* hidden_option stores upgrade level in lower 8 bits */
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
    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Set upgrade level failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }
    return 0;
}

/* Delete item (on upgrade failure) */
static int db_delete_item_at_slot(uint32_t charac_no, int slot) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);
    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Delete item failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }
    printf("[DB] Deleted item at charac_no=%u slot=%d\n", charac_no, slot);
    return 0;
}

/* Calculate upgrade success rate based on current level */
static int calc_upgrade_success_rate(uint8_t current_level) {
    /* Success rate decreases with level:
     * +0 to +3: 100%
     * +4: 95%
     * +5: 90%
     * +6: 80%
     * +7: 70%
     * +8: 60%
     * +9: 50%
     * +10: 40%
     * +11: 30%
     * +12: 20%
     * +13+: 10%
     */
    if (current_level <= 3) return 100;
    if (current_level == 4) return 95;
    if (current_level == 5) return 90;
    if (current_level == 6) return 80;
    if (current_level == 7) return 70;
    if (current_level == 8) return 60;
    if (current_level == 9) return 50;
    if (current_level == 10) return 40;
    if (current_level == 11) return 30;
    if (current_level == 12) return 20;
    return 10;
}

/* Calculate upgrade cost */
static uint32_t calc_upgrade_cost(uint32_t item_id, uint8_t current_level) {
    uint32_t item_level = (item_id / 1000) % 100;
    if (item_level == 0) item_level = 1;
    /* Base cost * (1 + level * 0.5) */
    uint32_t base_cost = item_level * 1000;
    return base_cost + (base_cost * current_level / 2);
}

static int handle_upgrade_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UPGRADE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 83, error, 4);
    }

    uint16_t slot, material_slot;
    uint8_t upgrade_type;
    memcpy(&slot, data, 2);
    memcpy(&material_slot, data + 2, 2);
    upgrade_type = data[4];

    printf("[GAME] UPGRADE_ITEM: slot=%u material=%u type=%u\n", slot, material_slot, upgrade_type);

    /* Get item to upgrade */
    uint32_t item_id = 0;
    uint8_t dummy_dur = 0;
    if (db_get_item_info_at_slot(s->character_id, slot, &item_id, &dummy_dur) < 0 || item_id == 0) {
        uint8_t response[4] = {3, 0, 0, 0}; /* Error: item not found */
        return send_response(s, 83, response, 4);
    }

    /* Get current upgrade level */
    uint8_t current_level = 0;
    db_get_item_upgrade_level(s->character_id, slot, &current_level);

    /* Check max level */
    if (current_level >= 15) {
        uint8_t response[4] = {4, 0, 0, 0}; /* Error: max level reached */
        return send_response(s, 83, response, 4);
    }

    /* Check gold */
    uint32_t cost = calc_upgrade_cost(item_id, current_level);
    if (s->inventory.money < cost) {
        uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
        return send_response(s, 83, response, 4);
    }

    /* Consume gold */
    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    /* Consume material if specified */
    if (material_slot != 0xFFFF) {
        db_delete_item_at_slot(s->character_id, material_slot);
    }

    /* Calculate success */
    int success_rate = calc_upgrade_success_rate(current_level);
    int roll = rand() % 100;
    int success = (roll < success_rate);
    int destroyed = 0;

    printf("[GAME] UPGRADE: level=%u rate=%d%% roll=%d success=%d\n",
           current_level, success_rate, roll, success);

    if (success) {
        /* Upgrade successful */
        current_level++;
        db_set_item_upgrade_level(s->character_id, slot, current_level);
        printf("[GAME] UPGRADE SUCCESS: new_level=%u\n", current_level);
    } else {
        /* Upgrade failed */
        if (current_level >= 7) {
            /* High level failure can destroy item */
            int destroy_chance = (current_level - 6) * 10; /* 10% at +7, 20% at +8, etc */
            if ((rand() % 100) < destroy_chance) {
                db_delete_item_at_slot(s->character_id, slot);
                destroyed = 1;
                printf("[GAME] UPGRADE FAILED: item destroyed!\n");
            } else {
                /* Level drops by 1 */
                if (current_level > 0) current_level--;
                db_set_item_upgrade_level(s->character_id, slot, current_level);
                printf("[GAME] UPGRADE FAILED: level dropped to %u\n", current_level);
            }
        } else {
            /* Low level failure just prevents upgrade */
            printf("[GAME] UPGRADE FAILED: no penalty\n");
        }
    }

    /* Build response */
    uint8_t response[12];
    memset(response, 0, 12);
    uint32_t result_code = success ? 0 : 1;
    uint32_t new_level = current_level;
    memcpy(response, &result_code, 4);
    memcpy(response + 4, &new_level, 4);
    response[8] = (uint8_t)destroyed;

    printf("[GAME] UPGRADE_ITEM: result=%u new_level=%u destroyed=%d cost=%u\n",
           result_code, new_level, destroyed, cost);

    return send_response(s, 83, response, 12);
}
