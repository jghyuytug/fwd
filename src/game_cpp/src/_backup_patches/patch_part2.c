static int handle_repair_equipment(ClientSession* s, const uint8_t* data, int len) {
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
        if (s->inventory.money < total_cost) {
            uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
            return send_response(s, 25, response, 4);
        }

        /* Deduct gold and repair */
        if (total_cost > 0) {
            s->inventory.money -= total_cost;
            db_update_gold(s->character_id, s->inventory.money);
        }
        repaired_count = db_repair_all_items(s->character_id);
    } else {
        /* Repair single item */
        uint32_t item_id = 0;
        uint8_t durability = 100;
        if (db_get_item_info_at_slot(s->character_id, slot, &item_id, &durability) == 0) {
            if (item_id > 0 && durability < 100) {
                total_cost = calc_repair_cost(item_id, durability);

                if (s->inventory.money < total_cost) {
                    uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
                    return send_response(s, 25, response, 4);
                }

                s->inventory.money -= total_cost;
                db_update_gold(s->character_id, s->inventory.money);
                if (db_repair_item(s->character_id, slot) == 0) {
                    repaired_count = 1;
                }
            }
        }
    }

    /* Build response: result(4) + repaired_count(4) + total_cost(4) */
    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &repaired_count, 4);
    memcpy(response + 8, &total_cost, 4);

    printf("[GAME] REPAIR_EQUIPMENT: repaired=%d cost=%u gold_left=%u\n",
           repaired_count, total_cost, s->inventory.money);

    return send_response(s, 25, response, 12);
}
