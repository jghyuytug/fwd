/* msg_no=83: UPGRADE_ITEM */
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

    uint32_t item_id = 0;
    uint8_t dummy = 0;
    if (db_get_item_info_at_slot(s->character_id, slot, &item_id, &dummy) < 0 || item_id == 0) {
        uint8_t response[4] = {3, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }

    uint8_t current_level = 0;
    db_get_item_upgrade_level(s->character_id, slot, &current_level);

    if (current_level >= 15) {
        uint8_t response[4] = {4, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }

    uint32_t cost = calc_upgrade_cost(item_id, current_level);
    if (s->inventory.money < cost) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 83, response, 4);
    }

    s->inventory.money -= cost;
    db_update_gold(s->character_id, s->inventory.money);

    if (material_slot != 0xFFFF) {
        db_delete_item_at_slot(s->character_id, material_slot);
    }

    int success_rate = calc_upgrade_success_rate(current_level);
    int roll = rand() % 100;
    int success = (roll < success_rate);
    int destroyed = 0;

    if (success) {
        current_level++;
        db_set_item_upgrade_level(s->character_id, slot, current_level);
    } else {
        if (current_level >= 7) {
            int destroy_chance = (current_level - 6) * 10;
            if ((rand() % 100) < destroy_chance) {
                db_delete_item_at_slot(s->character_id, slot);
                destroyed = 1;
            } else if (current_level > 0) {
                current_level--;
                db_set_item_upgrade_level(s->character_id, slot, current_level);
            }
        }
    }

    uint8_t response[12];
    memset(response, 0, 12);
    uint32_t result_code = success ? 0 : 1;
    uint32_t new_level = current_level;
    memcpy(response, &result_code, 4);
    memcpy(response + 4, &new_level, 4);
    response[8] = (uint8_t)destroyed;

    printf("[GAME] UPGRADE: result=%u level=%u destroyed=%d\n", result_code, new_level, destroyed);
    return send_response(s, 83, response, 12);
}

/* msg_no=100: DEPOSIT_MONEY */
static int handle_storage_deposit(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEPOSIT_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    if (s->inventory.money < amount) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 100, response, 4);
    }

    uint32_t storage_gold = 0;
    db_get_storage_gold(s->account_id, &storage_gold);

    s->inventory.money -= amount;
    storage_gold += amount;

    db_update_gold(s->character_id, s->inventory.money);
    db_set_storage_gold(s->account_id, storage_gold);

    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] DEPOSIT: char=%u storage=%u\n", s->inventory.money, storage_gold);
    return send_response(s, 100, response, 12);
}

/* msg_no=101: WITHDRAW_MONEY */
static int handle_storage_withdraw(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] WITHDRAW_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    uint32_t storage_gold = 0;
    db_get_storage_gold(s->account_id, &storage_gold);

    if (storage_gold < amount) {
        uint8_t response[4] = {2, 0, 0, 0};
        return send_response(s, 101, response, 4);
    }

    storage_gold -= amount;
    s->inventory.money += amount;

    db_set_storage_gold(s->account_id, storage_gold);
    db_update_gold(s->character_id, s->inventory.money);

    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &s->inventory.money, 4);
    memcpy(response + 8, &storage_gold, 4);

    printf("[GAME] WITHDRAW: char=%u storage=%u\n", s->inventory.money, storage_gold);
    return send_response(s, 101, response, 12);
}
